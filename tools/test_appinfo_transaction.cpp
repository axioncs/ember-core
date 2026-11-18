

#include "../src/config.hpp"
#include "../src/feats/appinfo_provision.hpp"
#include "../src/feats/appinfo_vdf.hpp"
#include "../src/feats/synthmark.hpp"
#include "../src/log.hpp"
#include "../src/utils/atomic_file.hpp"
#include "../include/base64/base64.hpp"

#include <openssl/sha.h>
#include <yaml-cpp/emitter.h>

#include <cassert>
#include <cstdint>
#include <ctime>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <sys/stat.h>
#include <unordered_set>
#include <vector>
#include <unistd.h>

namespace
{

template <typename T>
void appendLE(std::vector<uint8_t>& out, T value)
{
	const auto* p = reinterpret_cast<const uint8_t*>(&value);
	out.insert(out.end(), p, p + sizeof(value));
}

void createEmptyAppInfo(const std::string& path)
{
	std::vector<uint8_t> bytes;
	appendLE<uint32_t>(bytes, 0x07564429u);
	appendLE<uint32_t>(bytes, 1u);
	appendLE<int64_t>(bytes, 20);
	appendLE<uint32_t>(bytes, 0u);
	appendLE<uint32_t>(bytes, 0u);
	std::string error;
	assert(AtomicFile::write(path,
		reinterpret_cast<const char*>(bytes.data()), bytes.size(), error));
}

std::string wireFor(uint64_t gid)
{
	return "\"appinfo\"\n{\n"
	       "\t\"depots\"\n\t{\n"
	       "\t\t\"123\"\n\t\t{\n"
	       "\t\t\t\"manifests\"\n\t\t\t{\n"
	       "\t\t\t\t\"public\"\n\t\t\t\t{\n"
	       "\t\t\t\t\t\"gid\"\t\"" + std::to_string(gid) + "\"\n"
	       "\t\t\t\t}\n\t\t\t}\n"
	       "\t\t}\n"
	       "\t}\n"
	       "}\n";
}

std::string sha1Of(const std::string& data)
{
	unsigned char digest[SHA_DIGEST_LENGTH]{};
	SHA1(reinterpret_cast<const unsigned char*>(data.data()), data.size(), digest);
	return std::string(reinterpret_cast<const char*>(digest), sizeof(digest));
}

void writeCache(const std::string& cacheDir, uint32_t appid,
		uint32_t change, const std::string& wire, bool synthetic = false,
		bool includeSyntheticMetadata = true)
{
	const auto sha = sha1Of(wire);
	const auto stem = cacheDir + "/picsbuffer_" + std::to_string(appid);
	std::string error;
	assert(AtomicFile::write(stem + ".bin", wire, error));

	YAML::Emitter emitter;
	emitter << YAML::BeginMap
	         << YAML::Key << "appid" << YAML::Value << appid
	         << YAML::Key << "change_number" << YAML::Value << change
	         << YAML::Key << "wire_size" << YAML::Value << wire.size()
	         << YAML::Key << "sha_b64" << YAML::Value << base64::to_base64(sha);
	if (includeSyntheticMetadata)
		emitter << YAML::Key << "synthetic" << YAML::Value << synthetic;
	emitter << YAML::EndMap;
	const std::string metadata(emitter.c_str(), emitter.size());
	assert(AtomicFile::write(stem + ".yaml", metadata, error));
}

std::string readAll(const std::string& path)
{
	std::ifstream in(path, std::ios::binary);
	return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

bool hasScopedValidationArtifact(const std::string& directory)
{
	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		if (entry.path().filename().string().find(".ember-validate.") !=
			std::string::npos)
			return true;
	}
	return false;
}

struct MetadataGuardState
{
	int calls = 0;
	bool allow = false;
};

bool metadataGuard(void* opaque) noexcept
{
	auto& state = *static_cast<MetadataGuardState*>(opaque);
	++state.calls;
	return state.allow;
}

}

namespace AppInfoProvision
{

bool testCacheMarkerAllowsRead = true;
bool cacheMarkerAllowsRead(uint32_t)
{
	return testCacheMarkerAllowsRead;
}

}

int main()
{
	const std::string root =
		"/tmp/ember-appinfo-transaction." + std::to_string(getpid());
	const std::string configHome = root + "/config";
	const std::string cacheDir = configHome + "/ember/cache";
	const std::string appinfo = root + "/appcache/appinfo.vdf";
	std::error_code ec;
	std::filesystem::create_directories(cacheDir, ec);
	std::filesystem::create_directories(root + "/appcache", ec);
	assert(!ec);

	setenv("HOME", root.c_str(), 1);
	setenv("XDG_CONFIG_HOME", configHome.c_str(), 1);
	g_pLog = std::unique_ptr<CLog>(new CLog((root + "/test.log").c_str()));

	createEmptyAppInfo(appinfo);
	const auto activeIds = std::unordered_set<uint32_t>{1001, 1002, 1004, 1005, 1006};
	g_config.managedAppIds.set(activeIds);
	g_config.addedAppIds.set(activeIds);
	const auto first = wireFor(456);
	const auto second = wireFor(789);
	const auto orphan = wireFor(999);
	writeCache(cacheDir, 1001, 10, first);
	writeCache(cacheDir, 1002, 20, second);
	writeCache(cacheDir, 1003, 30, orphan);
	writeCache(cacheDir, 1004, 40, wireFor(111), true);

	assert(AppInfoVdf::injectAllCached(appinfo) == 2);
	assert(!std::filesystem::exists(cacheDir + "/picsbuffer_1003.bin"));
	assert(!std::filesystem::exists(cacheDir + "/picsbuffer_1003.yaml"));
	bool orphanQuarantined = false;
	for (const auto& entry : std::filesystem::directory_iterator(cacheDir))
	{
		const auto name = entry.path().filename().string();
		if (name.rfind("picsbuffer_1003.", 0) == 0 &&
		    name.find(".orphaned.") != std::string::npos)
		{
			orphanQuarantined = true;
			break;
		}
	}
	assert(orphanQuarantined);
	const auto published = readAll(appinfo);
	assert(!published.empty());
	assert(std::filesystem::exists(appinfo + ".ember-previous"));

	const auto scopedAppinfo = root + "/appcache/scoped.vdf";
	createEmptyAppInfo(scopedAppinfo);
	assert(AppInfoVdf::injectCachedApps(
		scopedAppinfo, std::unordered_set<uint32_t>{1001, 1003, 1004}) == 1);
	const auto scopedBytes = readAll(scopedAppinfo);
	assert(scopedBytes.find("456") != std::string::npos);
	assert(scopedBytes.find("789") == std::string::npos);
	assert(!hasScopedValidationArtifact(root + "/appcache"));

	const auto metadataAppinfo = root + "/appcache/metadata.vdf";
	createEmptyAppInfo(metadataAppinfo);
	const std::string dlcWire =
		"\"appinfo\"\n{\n"
		"\t\"appid\"\t\"2001\"\n"
		"\t\"common\"\n\t{\n"
		"\t\t\"name\"\t\"Metadata DLC\"\n"
		"\t\t\"type\"\t\"DLC\"\n"
		"\t\t\"parent\"\t\"1001\"\n"
		"\t}\n"
		"\t\"extended\"\n\t{\n"
		"\t\t\"dlcforappid\"\t\"1001\"\n"
		"\t}\n"
		"}\n";
	const std::vector<AppInfoVdf::MetadataApp> metadataApps{{
		.appid = 2001,
		.changeNumber = 77,
		.sha = sha1Of(dlcWire),
		.wireBuffer = dlcWire,
	}};
	assert(AppInfoVdf::injectValidatedMetadataApps(
		metadataAppinfo, metadataApps) == 1);
	const auto metadataBytes = readAll(metadataAppinfo);
	assert(metadataBytes.find("Metadata DLC") != std::string::npos);
	assert(metadataBytes.find("depots") == std::string::npos);

	const auto guardedAppinfo = root + "/appcache/metadata-guarded.vdf";
	createEmptyAppInfo(guardedAppinfo);
	const auto guardedBefore = readAll(guardedAppinfo);
	MetadataGuardState deniedGuard;
	assert(AppInfoVdf::injectValidatedMetadataAppsGuarded(
		guardedAppinfo, metadataApps, &deniedGuard, &metadataGuard) == 0);
	assert(deniedGuard.calls == 1);
	assert(readAll(guardedAppinfo) == guardedBefore);

	const auto existingDlcAppinfo = root + "/appcache/existing-dlc.vdf";
	createEmptyAppInfo(existingDlcAppinfo);
	const std::string existingDlcWire = wireFor(424242);
	assert(AppInfoVdf::injectApp(
		existingDlcAppinfo, 2001, 78, sha1Of(existingDlcWire),
		existingDlcWire));
	const auto existingDlcBefore = readAll(existingDlcAppinfo);
	assert(AppInfoVdf::injectValidatedMetadataApps(
		existingDlcAppinfo, metadataApps) == 1);
	assert(readAll(existingDlcAppinfo) == existingDlcBefore);

	const auto racedAppinfo = root + "/appcache/raced.vdf";
	createEmptyAppInfo(racedAppinfo);
	const auto raceRollback = racedAppinfo + ".ember-previous";
	std::string rollbackSeedError;
	assert(AtomicFile::write(
		raceRollback, "existing rollback", rollbackSeedError));
#ifdef APPINFO_VDF_TESTING
	AppInfoVdf::setBeforeScopedPublishHook([&] {
		std::string raceError;
		assert(AtomicFile::write(racedAppinfo, "steam writer", raceError));
	});
#endif
	assert(AppInfoVdf::injectCachedApps(
		racedAppinfo, std::unordered_set<uint32_t>{1001}) == 0);
	assert(readAll(racedAppinfo) == "steam writer");
	assert(readAll(raceRollback) == "existing rollback");
	assert(!hasScopedValidationArtifact(root + "/appcache"));

	assert(AppInfoVdf::injectAllCached(appinfo) == 2);
	assert(readAll(appinfo) == published);

	setenv("EMBER_ASYNC_PROVISION", "1", 1);
	setenv("EMBER_PROVISION_TTL", "300", 1);
	const auto staleBuffer = cacheDir + "/picsbuffer_1001.bin";
	struct timespec staleTimes[2]{};
	const auto staleNow = std::time(nullptr) - 3600;
	staleTimes[0].tv_sec = staleNow;
	staleTimes[1].tv_sec = staleNow;
	assert(utimensat(AT_FDCWD, staleBuffer.c_str(), staleTimes, 0) == 0);

	const auto filteredAppinfo = root + "/appcache/filtered.vdf";
	createEmptyAppInfo(filteredAppinfo);

	assert(AppInfoVdf::injectAllCached(filteredAppinfo) == 2);

	const auto fallbackAppinfo = root + "/appcache/fallback.vdf";
	createEmptyAppInfo(fallbackAppinfo);
	assert(AppInfoVdf::injectAllCached(fallbackAppinfo) == 2);

	unsetenv("EMBER_ASYNC_PROVISION");
	unsetenv("EMBER_PROVISION_TTL");

	assert(SynthMark::mark(cacheDir, 1001));
	const auto staleMarkerAppinfo = root + "/appcache/stale-marker.vdf";
	createEmptyAppInfo(staleMarkerAppinfo);
	assert(AppInfoVdf::injectAllCached(staleMarkerAppinfo) == 1);
	assert(SynthMark::unmark(cacheDir, 1001));

	assert(SynthMark::mark(cacheDir, 1004));
	const auto markedSyntheticAppinfo = root + "/appcache/marked-synthetic.vdf";
	createEmptyAppInfo(markedSyntheticAppinfo);
	assert(AppInfoVdf::injectAllCached(markedSyntheticAppinfo) == 3);
	assert(SynthMark::unmark(cacheDir, 1004));

	const auto legacyBaseline = root + "/appcache/legacy-baseline.vdf";
	createEmptyAppInfo(legacyBaseline);
	assert(AppInfoVdf::injectAllCached(legacyBaseline) == 2);
	writeCache(cacheDir, 1006, 60, wireFor(222), true,
	           false);
	assert(SynthMark::mark(cacheDir, 1006));
	const auto legacyAppinfo = root + "/appcache/legacy.vdf";
	createEmptyAppInfo(legacyAppinfo);

	assert(AppInfoVdf::injectAllCached(legacyAppinfo) == 3);
	assert(SynthMark::unmark(cacheDir, 1006));
	std::filesystem::remove(cacheDir + "/picsbuffer_1006.bin");
	std::filesystem::remove(cacheDir + "/picsbuffer_1006.yaml");

	std::filesystem::copy_file(appinfo, appinfo + ".ember-previous",
	                           std::filesystem::copy_options::overwrite_existing);
	{
		std::ofstream out(appinfo, std::ios::binary | std::ios::trunc);
		out.write("\x29\x44\x56\x07", 4);
		out << "broken";
	}
	assert(AppInfoVdf::injectAllCached(appinfo) == 2);
	assert(readAll(appinfo) == published);

	assert(!std::filesystem::exists(appinfo + ".tmp"));

	writeCache(cacheDir, 1005, 50, wireFor(333));
	YAML::Emitter malformed;
	malformed << YAML::BeginMap
	          << YAML::Key << "appid" << YAML::Value << 1005
	          << YAML::Key << "change_number" << YAML::Value << 50
	          << YAML::Key << "wire_size" << YAML::Value << wireFor(333).size()
	          << YAML::Key << "sha_b64" << YAML::Value
	          << base64::to_base64(sha1Of(wireFor(333)))
	          << YAML::Key << "normalized" << YAML::Value << true
	          << YAML::Key << "synthetic" << YAML::Value << "bogus"
	          << YAML::EndMap;
	std::string malformedError;
	assert(AtomicFile::write(
		cacheDir + "/picsbuffer_1005.yaml",
		std::string(malformed.c_str(), malformed.size()), malformedError));
	const auto malformedAppinfo = root + "/appcache/malformed.vdf";
	createEmptyAppInfo(malformedAppinfo);
	assert(AppInfoVdf::injectAllCached(malformedAppinfo) == 2);

	const auto invalidatedAppinfo = root + "/appcache/invalidated.vdf";
	createEmptyAppInfo(invalidatedAppinfo);
	AppInfoProvision::testCacheMarkerAllowsRead = false;
	assert(AppInfoVdf::injectAllCached(invalidatedAppinfo) == 0);
	AppInfoProvision::testCacheMarkerAllowsRead = true;

	const auto untrustedLockAppinfo = root + "/appcache/untrusted-lock.vdf";
	createEmptyAppInfo(untrustedLockAppinfo);
	{
		const auto lockPath = untrustedLockAppinfo + ".ember.lock";
		const int lockFd = open(lockPath.c_str(), O_CREAT | O_RDWR, 0666);
		assert(lockFd >= 0);
		close(lockFd);
		assert(AppInfoVdf::injectAllCached(untrustedLockAppinfo) == 2);
		std::filesystem::remove(lockPath);
	}

	const auto racedBootAppinfo = root + "/appcache/raced-boot.vdf";
	createEmptyAppInfo(racedBootAppinfo);
#ifdef APPINFO_VDF_TESTING
	AppInfoVdf::setBeforeScopedPublishHook([&] {
		std::string raceError;
		assert(AtomicFile::write(racedBootAppinfo, "steam writer", raceError));
	});
#endif
	assert(AppInfoVdf::injectAllCached(racedBootAppinfo) == 0);
	assert(readAll(racedBootAppinfo) == "steam writer");
	assert(!hasScopedValidationArtifact(root + "/appcache"));

	const auto partialMetadataAppinfo = root + "/appcache/partial-metadata.vdf";
	createEmptyAppInfo(partialMetadataAppinfo);
	{
		const std::string goodWire = wireFor(515151);
		const auto emptyBaseline = readAll(partialMetadataAppinfo);
		const std::vector<AppInfoVdf::MetadataApp> mixed = {
			{0, 11, sha1Of(goodWire), goodWire},
			{3001, 12, std::string(4, 'x'), goodWire},
			{3002, 13, sha1Of(goodWire), goodWire},
		};

		assert(AppInfoVdf::injectValidatedMetadataApps(
			partialMetadataAppinfo, mixed) == 1);
		const auto afterPartial = readAll(partialMetadataAppinfo);
		assert(afterPartial != emptyBaseline);

		assert(AppInfoVdf::injectValidatedMetadataApps(
			partialMetadataAppinfo, mixed) == 1);
		assert(readAll(partialMetadataAppinfo) == afterPartial);

		const auto rejectedOnlyAppinfo = root + "/appcache/rejected-only.vdf";
		createEmptyAppInfo(rejectedOnlyAppinfo);
		const auto rejectedBaseline = readAll(rejectedOnlyAppinfo);
		assert(AppInfoVdf::injectValidatedMetadataApps(
			rejectedOnlyAppinfo,
			{{3001, 12, std::string(4, 'x'), goodWire}}) == 0);
		assert(readAll(rejectedOnlyAppinfo) == rejectedBaseline);
	}
	return 0;
}

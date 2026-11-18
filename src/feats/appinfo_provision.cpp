

#include "appinfo_provision.hpp"
#include "provision_result.hpp"

#include "appinfo_vdf.hpp"
#include "appinfostate.hpp"
#include "apps.hpp"
#include "cmclient.hpp"
#include "compattool.hpp"
#include "depotkey.hpp"
#include "emptydepot.hpp"
#include "dlcids.hpp"
#include "dlc_metadata.hpp"
#include "hotreload.hpp"
#include "manifestid.hpp"
#include "manifeststore.hpp"
#include "manifeststore_io.hpp"
#include "manifestsynth.hpp"
#include "provision_cache.hpp"
#include "cache_pair.hpp"
#include "provision_network.hpp"
#include "provision_schedule.hpp"
#include "pending_proton.hpp"
#include "provision_pass.hpp"
#include "provision_terminal.hpp"
#include "synthmark.hpp"
#include "usabledepot.hpp"

#include "../config.hpp"
#include "../ascii.hpp"
#include "../globals.hpp"
#include "../log.hpp"
#include "../ownerwork.hpp"
#include "../bootprof.hpp"
#include "../thread_start.hpp"
#include "../cainfo.hpp"

#include "../utils/ManifestFetch.hpp"
#include "../utils/atomic_file.hpp"
#include "../utils/process_lock.hpp"

#include "base64/base64.hpp"
#include "yaml-cpp/yaml.h"
#include "yaml-cpp/emitter.h"

#include <openssl/sha.h>

#include <curl/curl.h>
#include <dlfcn.h>

#include <algorithm>
#include <chrono>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unordered_map>
#include <unistd.h>
#include <unordered_set>
#include <vector>

namespace AppInfoProvision
{

namespace
{

std::set<uint32_t> g_needProton;

std::set<uint32_t> g_pendingProtonRemovals;

struct CacheValidationResult
{
	bool valid = false;
	std::string diag;
};

std::mutex g_cacheValidationMu;
std::map<cache::CacheValidationKey, CacheValidationResult> g_cacheValidationMemo;
std::unordered_map<uint32_t, CacheProbe> g_cacheProbeMemo;
std::mutex g_provisionPassMu;
std::mutex g_cachePublicationMu;
std::mutex g_cacheReadInvalidationMu;
std::unordered_set<uint32_t> g_cacheReadInvalidated;
std::unordered_map<uint32_t, std::uint64_t> g_cachePublicationGenerations;

struct TerminalMemoryEntry
{
	ProvisionTerminal::Record record;
	std::uint64_t managedGeneration = 0;
};
std::unordered_map<uint32_t, TerminalMemoryEntry> g_terminalProvisionResults;
std::unordered_set<uint32_t> g_loggedMalformedTerminal;

std::unordered_map<uint32_t, std::string> g_localContentFingerprints;
std::mutex g_refreshScheduleMu;
RefreshQueue g_refreshQueue;
std::string g_refreshActivePath;
std::uint64_t g_refreshWorkerToken = 0;
std::mutex g_coldRetryMu;
struct ColdRetryState
{
	std::chrono::steady_clock::time_point retryAfter{};
	unsigned int failures = 0;
};
std::unordered_map<std::uint32_t, ColdRetryState> g_coldRetries;

void markProtonNeeded(uint32_t appId,
                      const CachePublicationToken& publication)
{
	std::lock_guard<std::mutex> passLock(g_provisionPassMu);
	if (!publication.managed ||
	    g_config.managedAppIds.get().count(appId) == 0)
		return;

	std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
	if (!cache::protonPublicationAllowed(
	        publication.managed, publication.generation,
	        cachePublicationGenerationLocked(appId)))
	{
		g_pLog->debug(
		    "AppInfoProvision: rejecting stale Proton mark for app=%u\n",
		    appId);
		return;
	}
	g_pendingProtonRemovals.erase(appId);
	g_needProton.insert(appId);
}

bool coldRetryBlocked(std::uint32_t appId)
{
	std::lock_guard<std::mutex> lock(g_coldRetryMu);
	const auto found = g_coldRetries.find(appId);
	return found != g_coldRetries.end() &&
	       std::chrono::steady_clock::now() < found->second.retryAfter;
}

void noteColdRetryOutcome(std::uint32_t appId, bool unresolved)
{
	std::lock_guard<std::mutex> lock(g_coldRetryMu);
	if (!unresolved)
	{
		g_coldRetries.erase(appId);
		return;
	}

	auto& retry = g_coldRetries[appId];
	++retry.failures;
	const unsigned int exponent =
		retry.failures > 4 ? 4 : retry.failures - 1;
	const unsigned int delaySeconds = std::min(60u, 5u << exponent);
	retry.retryAfter = std::chrono::steady_clock::now() +
	                   std::chrono::seconds(delaySeconds);
}

typedef CURL*       (*curl_easy_init_t)();
typedef CURLcode    (*curl_easy_setopt_t)(CURL*, CURLoption, ...);
typedef CURLcode    (*curl_easy_perform_t)(CURL*);
typedef void        (*curl_easy_cleanup_t)(CURL*);
typedef CURLcode    (*curl_easy_getinfo_t)(CURL*, CURLINFO, ...);
typedef const char* (*curl_easy_strerror_t)(CURLcode);

static curl_easy_init_t     p_curl_easy_init     = nullptr;
static curl_easy_setopt_t   p_curl_easy_setopt   = nullptr;
static curl_easy_perform_t  p_curl_easy_perform  = nullptr;
static curl_easy_cleanup_t  p_curl_easy_cleanup  = nullptr;
static curl_easy_getinfo_t  p_curl_easy_getinfo  = nullptr;
static curl_easy_strerror_t p_curl_easy_strerror = nullptr;

bool loadCurl()
{
	if (p_curl_easy_init) return true;
	void* h = dlopen("libcurl.so.4", RTLD_NOLOAD | RTLD_LAZY);
	if (!h) h = dlopen("libcurl.so.4", RTLD_LAZY);
	if (!h) h = RTLD_DEFAULT;
	p_curl_easy_init     = (curl_easy_init_t)     dlsym(h, "curl_easy_init");
	p_curl_easy_setopt   = (curl_easy_setopt_t)   dlsym(h, "curl_easy_setopt");
	p_curl_easy_perform  = (curl_easy_perform_t)  dlsym(h, "curl_easy_perform");
	p_curl_easy_cleanup  = (curl_easy_cleanup_t)  dlsym(h, "curl_easy_cleanup");
	p_curl_easy_getinfo  = (curl_easy_getinfo_t)  dlsym(h, "curl_easy_getinfo");
	p_curl_easy_strerror = (curl_easy_strerror_t) dlsym(h, "curl_easy_strerror");
	return p_curl_easy_init && p_curl_easy_setopt &&
	       p_curl_easy_perform && p_curl_easy_cleanup;
}

std::size_t curlWriteCb(const char* p, std::size_t sz, std::size_t n, std::string* dst)
{
	dst->append(p, sz * n);
	return sz * n;
}

NetworkFailure httpGetJson(const std::string& url, std::string& body,
                           std::string& diag, long timeoutMs)
{
	if (!loadCurl()) { diag = "libcurl unavailable"; return NetworkFailure::Provider; }
	if (timeoutMs <= 0)
	{
		diag = "startup network budget exhausted";
		return NetworkFailure::Transient;
	}
	CURL* c = p_curl_easy_init();
	if (!c) { diag = "curl_easy_init failed"; return NetworkFailure::Provider; }

	body.clear();
	p_curl_easy_setopt(c, CURLOPT_URL, url.c_str());
	p_curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
	p_curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, curlWriteCb);
	p_curl_easy_setopt(c, CURLOPT_WRITEDATA, &body);

	p_curl_easy_setopt(c, CURLOPT_TIMEOUT_MS, timeoutMs);
	p_curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT_MS,
	                   std::min(timeoutMs, 8000L));

	p_curl_easy_setopt(c, CURLOPT_NOSIGNAL, 1L);
	p_curl_easy_setopt(c, CURLOPT_USERAGENT, "ember-AppInfoProvision/0.1");

	if (const char* f = ca::bundleFile()) p_curl_easy_setopt(c, CURLOPT_CAINFO, f);
	if (const char* d = ca::bundleDir())  p_curl_easy_setopt(c, CURLOPT_CAPATH, d);

	const CURLcode rc = p_curl_easy_perform(c);
	long status = 0;
	if (p_curl_easy_getinfo) p_curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &status);
	p_curl_easy_cleanup(c);

	if (rc != CURLE_OK)
	{
		diag = p_curl_easy_strerror ? p_curl_easy_strerror(rc) : "curl error";
		return classifyCurlFailure(rc);
	}
	if (status != 200)
	{
		std::stringstream s; s << "HTTP " << status;
		diag = s.str();
		return classifyHttpFailure(status);
	}
	if (body.empty()) { diag = "empty body"; return NetworkFailure::Provider; }
	return NetworkFailure::None;
}

const std::vector<std::string>& providerChain()
{

	static const std::vector<std::string> chain = [] {
		std::vector<std::string> c;
		if (const char* ov = std::getenv("EMBER_APPINFO_PROVIDER");
		    ov && *ov)
		{
			c.emplace_back(ov);
		}
		c.emplace_back("https://api.steamcmd.net/v1/info/{appid}");
		return c;
	}();
	return chain;
}

std::string expandUrl(const std::string& tmpl, uint32_t appId)
{
	std::string out;
	out.reserve(tmpl.size() + 16);
	const std::string needle = "{appid}";
	std::size_t i = 0;
	while (i < tmpl.size())
	{
		if (tmpl.compare(i, needle.size(), needle) == 0)
		{
			out += std::to_string(appId);
			i += needle.size();
		}
		else
		{
			out.push_back(tmpl[i++]);
		}
	}
	return out;
}

void emitEscaped(std::string& out, const std::string& s)
{
	out.push_back('"');
	for (char c : s)
	{
		if (c == '\\' || c == '"') out.push_back('\\');
		out.push_back(c);
	}
	out.push_back('"');
}

void emitNode(std::string& out, const YAML::Node& node, int depth);

void emitMap(std::string& out, const YAML::Node& node, int depth)
{
	for (auto it = node.begin(); it != node.end(); ++it)
	{
		const std::string key = it->first.as<std::string>();

		out.append(depth, '\t');
		emitEscaped(out, key);
		const auto& val = it->second;
		if (val.IsMap())
		{
			out.append("\n");
			out.append(depth, '\t');
			out.append("{\n");
			emitNode(out, val, depth + 1);
			out.append(depth, '\t');
			out.append("}\n");
		}
		else if (val.IsScalar())
		{
			out.append("\t\t");
			emitEscaped(out, val.as<std::string>());
			out.append("\n");
		}
		else if (val.IsSequence())
		{

			out.append("\n");
			out.append(depth, '\t');
			out.append("{\n");
			int idx = 0;
			for (const auto& child : val)
			{
				out.append(depth + 1, '\t');
				emitEscaped(out, std::to_string(idx++));
				if (child.IsMap())
				{
					out.append("\n");
					out.append(depth + 1, '\t');
					out.append("{\n");
					emitNode(out, child, depth + 2);
					out.append(depth + 1, '\t');
					out.append("}\n");
				}
				else
				{
					out.append("\t\t");
					emitEscaped(out, child.IsScalar() ? child.as<std::string>() : std::string{});
					out.append("\n");
				}
			}
			out.append(depth, '\t');
			out.append("}\n");
		}
		else
		{

			out.append("\t\t\"\"\n");
		}
	}
}

void emitNode(std::string& out, const YAML::Node& node, int depth)
{
	if (node.IsMap())
	{
		emitMap(out, node, depth);
	}
}

void pruneUnsupportedDepots(YAML::Node& body, uint32_t appId,
                             const CachePublicationToken& publication)
{
	if (!body.IsMap()) return;
	YAML::Node depots = body["depots"];
	if (!depots || !depots.IsMap()) return;

	YAML::Node newDepots(YAML::NodeType::Map);
	std::set<std::string> survivingOs;
	int kept = 0;
	int keptContent = 0;
	int dropped = 0;
	int totalNumeric = 0;
	std::unordered_set<uint32_t> contentDlcAppIds;
	std::unordered_set<uint32_t> usableDlcAppIds;

	for (auto it = depots.begin(); it != depots.end(); ++it)
	{
		const std::string key = it->first.as<std::string>();

		bool numeric = !key.empty();
		for (char c : key) if (c < '0' || c > '9') { numeric = false; break; }
		if (!numeric)
		{
			newDepots[key] = YAML::Clone(it->second);
			continue;
		}
		++totalNumeric;

		uint32_t depotId = 0;
		try { depotId = static_cast<uint32_t>(std::stoul(key)); }
		catch (...) { newDepots[key] = YAML::Clone(it->second); continue; }

		const YAML::Node depotNode = it->second;
		const bool hasDlcMarker = depotNode.IsMap() && depotNode["dlcappid"];
		const bool hasManifests = depotNode.IsMap() && depotNode["manifests"];
		const bool isVirtualDlc = hasDlcMarker && !hasManifests;
		uint32_t dlcAppId = 0;
		if (hasDlcMarker)
		{
			try { dlcAppId = depotNode["dlcappid"].as<uint32_t>(); }
			catch (...) {}
		}

		if (dlcAppId != 0 && hasManifests)
		{
			contentDlcAppIds.insert(dlcAppId);
		}

		if (depotPublicManifestIsEmpty(depotNode))
		{
			++dropped;
			g_pLog->info("AppInfoProvision: app=%u dropping empty depot %u "
			             "(public manifest size 0)\n", appId, depotId);
			continue;
		}

		const auto savedKey = DepotKey::getCachedKey(depotId);
		const bool hasKey = !savedKey.key.empty();

		if (!hasKey && !isVirtualDlc)
		{
			++dropped;
			continue;
		}
		++kept;
		if (!isVirtualDlc) ++keptContent;
		newDepots[key] = YAML::Clone(depotNode);
		if (dlcAppId != 0)
		{
			usableDlcAppIds.insert(dlcAppId);
		}

		if (!isVirtualDlc && depotNode.IsMap())
		{
			std::string osStr;
			if (depotNode["config"] && depotNode["config"]["oslist"])
			{
				try { osStr = depotNode["config"]["oslist"].as<std::string>(); }
				catch (...) {}
			}
			addDepotPlatforms(survivingOs, osStr);
		}
	}
	std::string commonOsList;
	try
	{
		if (body["common"] && body["common"]["oslist"])
			commonOsList = body["common"]["oslist"].as<std::string>();
	}
	catch (...) {}
	const bool protonNeeded = requiresProtonMapping(
		static_cast<std::size_t>(keptContent), survivingOs, commonOsList);

	std::unordered_set<uint32_t> unsupportedDlcAppIds;
	for (uint32_t dlcAppId : contentDlcAppIds)
	{
		if (usableDlcAppIds.count(dlcAppId) == 0)
		{
			unsupportedDlcAppIds.insert(dlcAppId);
		}
	}
	if (!unsupportedDlcAppIds.empty())
	{
		YAML::Node extended = body["extended"];
		YAML::Node listNode = extended && extended.IsMap()
			? extended["listofdlc"]
			: YAML::Node();
		if (listNode && listNode.IsScalar())
		{
			try
			{
				const std::string oldList = listNode.as<std::string>();
				std::size_t removed = 0;
				const std::string newList =
					filterUnsupportedDlcAppIds(oldList, unsupportedDlcAppIds,
					                           &removed);
				if (removed != 0)
				{
					body["extended"]["listofdlc"] = newList;
					g_pLog->info("AppInfoProvision: app=%u removed %zu unsupported "
					             "content DLC appid(s) from extended.listofdlc\n",
					             appId, removed);
				}
			}
			catch (...) {}
		}
	}

	if (dropped == 0)
	{

		if (protonNeeded)
		{
			markProtonNeeded(appId, publication);
		}
		return;
	}

	body["depots"] = newDepots;
	g_pLog->info("AppInfoProvision: app=%u dropped %d/%d unsupported depots (kept %d)\n",
	             appId, dropped, totalNumeric, kept);

	if (protonNeeded)
	{
		markProtonNeeded(appId, publication);
	}

	(void)0;
}

const std::string& getCacheDir();
bool readValidatedCacheBufferLocked(uint32_t appId, std::string& wireOut,
                                    std::string& diag);

int synthesizeDepotsFromStore(YAML::Node& body, uint32_t appId)
{
	std::vector<ManifestSynth::SynthDepot> depots;

	std::vector<std::string> winFiles, linFiles, macFiles;
	for (uint32_t depotId : DepotKey::managedDepotsForApp(appId))
	{
		const uint64_t gid = ManifestStore::bestArchivedGid(depotId, 0);
		if (gid == 0) continue;

		std::string oslist;
		uint64_t size = 0, download = 0;
		{
			const std::string man =
				ManifestStore::dir() + "/" + std::to_string(depotId) + "_" +
				std::to_string(gid) + ".manifest";
			std::ifstream ifs(man, std::ios::binary);
			if (ifs)
			{
				std::string bytes((std::istreambuf_iterator<char>(ifs)),
				                  std::istreambuf_iterator<char>());
				const auto files = ManifestSynth::extractManifestFilenames(bytes);
				oslist = ManifestSynth::detectOsFromFiles(files);
				if (oslist == "windows")     { for (auto& f : files) winFiles.push_back(f); }
				else if (oslist == "linux")  { for (auto& f : files) linFiles.push_back(f); }
				else if (oslist == "macos")  { for (auto& f : files) macFiles.push_back(f); }
				if (ManifestSynth::parseManifestSizes(bytes, size, download))
					ManifestStore::cacheInstalledSize(depotId, gid, size);
			}
		}
		depots.push_back({depotId, gid, oslist, size, download});
	}
	const int n = ManifestSynth::injectSynthesizedDepots(body, depots);
	if (n > 0)
	{

		ManifestSynth::ensureInstallDir(body);

		std::string installdir;
		if (YAML::Node d = body["config"]["installdir"]; d && d.IsScalar())
			installdir = d.as<std::string>();
		std::vector<std::pair<std::string, std::string>> launchers;
		if (!linFiles.empty())
			launchers.push_back({ManifestSynth::pickLauncher(linFiles, installdir, "linux"), "linux"});
		if (!winFiles.empty())
			launchers.push_back({ManifestSynth::pickLauncher(winFiles, installdir, "windows"), "windows"});
		if (!macFiles.empty())
			launchers.push_back({ManifestSynth::pickLauncher(macFiles, installdir, "macos"), "macos"});
		const int le = ManifestSynth::ensureLaunchEntries(body, launchers);
		if (le > 0)
		{
			std::string summary;
			for (const auto& [exe, os] : launchers)
				if (!exe.empty()) summary += " " + os + ":'" + exe + "'";
			g_pLog->info("AppInfoProvision: app=%u synthesized %d launch entry(ies):%s\n",
			             appId, le, summary.c_str());
		}

	}
	return n;
}

void neutralizeLegacyCdKey(YAML::Node& body, uint32_t appId)
{
	if (!body.IsMap()) return;
	YAML::Node ext = body["extended"];
	if (!ext || !ext.IsMap()) return;
	YAML::Node had = ext["hadthirdpartycdkey"];
	if (!had || !had.IsScalar()) return;

	std::string cur;
	try { cur = had.as<std::string>(); } catch (...) { return; }
	if (cur == "0") return;

	ext["hadthirdpartycdkey"] = "0";
	g_pLog->info("AppInfoProvision: app=%u cleared extended.hadthirdpartycdkey "
	             "(was %s) so launch skips GettingLegacyKey\n",
	             appId, cur.c_str());
}

bool isDlcApp(const YAML::Node& body)
{
	if (!body || !body.IsMap()) return false;
	const YAML::Node common = body["common"];
	if (!common || !common.IsMap()) return false;
	const YAML::Node type = common["type"];
	if (!type || !type.IsScalar()) return false;
	std::string value = type.as<std::string>("");
	for (char& ch : value)
		ch = static_cast<char>(
			Ascii::toLower(static_cast<unsigned char>(ch)));
	return value == "dlc";
}

SourceResult renderAppinfoBuffer(
    const YAML::Node& appNode, uint32_t appId, std::string& wireOut,
    const CachePublicationToken& publication, bool* synthesizedOut)
{
	if (synthesizedOut) *synthesizedOut = false;
	if (!appNode.IsMap()) return SourceResult::InvalidResponse;

	YAML::Node body;
	for (auto it = appNode.begin(); it != appNode.end(); ++it)
	{
		const std::string key = it->first.as<std::string>();
		if (!key.empty() && key.front() == '_') continue;
		body[key] = it->second;
	}
	if (!body.IsMap() || body.size() == 0) return SourceResult::InvalidResponse;

	bool hadConcreteContent = hasUsableContentDepot(body);

	if (YAML::Node d = body["depots"]; !d || !d.IsMap() || d.size() == 0)
	{
		const int n = synthesizeDepotsFromStore(body, appId);
		if (n > 0)
		{
			if (synthesizedOut) *synthesizedOut = true;
			g_pLog->info("AppInfoProvision: app=%u synthesized %d depot(s) from "
			             "stored manifests (product-info had none)\n", appId, n);
			hadConcreteContent = hasUsableContentDepot(body);
		}
	}

	pruneUnsupportedDepots(body, appId, publication);
	const SourceResult contentResult = classifyContentResult(
	    hadConcreteContent, hasUsableContentDepot(body), isDlcApp(body));
	if (contentResult != SourceResult::Success)
	{
		g_pLog->info("AppInfoProvision: app=%u has no usable content depots "
		             "after pruning, skipping\n", appId);
		return contentResult;
	}

	neutralizeLegacyCdKey(body, appId);

	wireOut.clear();
	wireOut.append("\"appinfo\"\n{\n");
	emitNode(wireOut, body, 1);
	wireOut.append("}\n");
	return SourceResult::Success;
}

const std::string& getCacheDir()
{
	static const std::string dir = [] {
		const std::string value = g_config.getDir() + "/cache";
		if (!std::filesystem::exists(value))
		{
			std::error_code ec;
			std::filesystem::create_directories(value, ec);
		}
		return value;
	}();
	return dir;
}

std::string getBufferPath(uint32_t appId)
{
	return getCacheDir() + "/picsbuffer_" + std::to_string(appId) + ".bin";
}

std::string getMetaPath(uint32_t appId)
{
	return getCacheDir() + "/picsbuffer_" + std::to_string(appId) + ".yaml";
}

std::string getDlcMetadataPath(uint32_t baseAppId)
{
	return getCacheDir() + "/dlcmetadata_" +
		std::to_string(baseAppId) + ".yaml";
}

bool readTextFileBounded(
	const std::string& path, std::size_t maxBytes, std::string& output)
{
	output.clear();
	std::ifstream input(path, std::ios::binary | std::ios::ate);
	if (!input.is_open()) return false;
	const std::streamsize rawSize = input.tellg();
	if (rawSize <= 0 || static_cast<std::uint64_t>(rawSize) > maxBytes)
		return false;
	output.assign(static_cast<std::size_t>(rawSize), '\0');
	input.seekg(0, std::ios::beg);
	return input.read(output.data(), rawSize) && input.gcount() == rawSize;
}

std::string pendingProtonPath()
{
	return getCacheDir() + "/proton-mappings.pending";
}

enum class PendingProtonFileStatus
{
	Missing,
	Valid,
	Invalid,
};

struct PendingProtonFile
{
	PendingProtonFileStatus status = PendingProtonFileStatus::Invalid;
	std::set<uint32_t> ids;
};

PendingProtonFile readPendingProtonFileLocked()
{
	const auto path = pendingProtonPath();
	std::ifstream ifs(path);
	if (!ifs.is_open())
	{
		std::error_code ec;
		const bool exists = std::filesystem::exists(path, ec);
		if (!exists && !ec)
			return {PendingProtonFileStatus::Missing, {}};
		return {PendingProtonFileStatus::Invalid, {}};
	}

	std::set<uint32_t> ids;
	std::string token;
	while (ifs >> token)
	{
		const auto parsed = parsePendingProtonText(token);
		if (parsed.status != PendingProtonParseStatus::Valid ||
		    parsed.ids.size() != 1)
		{
			return {PendingProtonFileStatus::Invalid, {}};
		}
		ids.insert(*parsed.ids.begin());
	}
	if (ifs.bad() || !ifs.eof())
		return {PendingProtonFileStatus::Invalid, {}};
	return {PendingProtonFileStatus::Valid, std::move(ids)};
}

void persistPendingProtonMappings(bool waitForLock)
{
	if (g_needProton.empty() && g_pendingProtonRemovals.empty()) return;
	(void)getCacheDir();
	ProcessLock::FileLock cacheLock(cacheLockPath(), !waitForLock);
	if (!cacheLock.acquired()) return;

	const auto existing = readPendingProtonFileLocked();
	if (existing.status == PendingProtonFileStatus::Invalid)
	{
		if (g_pLog)
			g_pLog->debug("AppInfoProvision: preserving invalid or unreadable pending Proton file\n");
		return;
	}

	std::set<uint32_t> pending;
	const auto managed = g_config.managedAppIds.get();
	for (uint32_t appId : existing.ids)
	{
		if (managed.count(appId) != 0)
			pending.insert(appId);
	}
	for (uint32_t appId : g_needProton)
	{
		if (managed.count(appId) != 0)
			pending.insert(appId);
	}
	for (uint32_t appId : g_pendingProtonRemovals)
		pending.erase(appId);

	bool persisted = false;
	std::string error;
	if (pending.empty())
	{
		std::error_code ec;
		std::filesystem::remove(pendingProtonPath(), ec);
		persisted = !ec;
	}
	else
	{
		std::string content;
		for (uint32_t appId : pending)
			content += std::to_string(appId) + "\n";
		persisted = AtomicFile::write(pendingProtonPath(), content, error);
	}
	if (!persisted)
	{
		if (g_pLog)
			g_pLog->debug("AppInfoProvision: cannot persist pending Proton mappings: %s\n",
			              error.empty() ? "remove failed" : error.c_str());
		return;
	}
	g_pendingProtonRemovals.clear();
}

bool statBuffer(uint32_t appId, cache::CacheValidationKey& identityOut)
{
	struct stat bufferStat{}, metadataStat{};
	if (stat(getBufferPath(appId).c_str(), &bufferStat) != 0 ||
		bufferStat.st_size <= 0 ||
		stat(getMetaPath(appId).c_str(), &metadataStat) != 0 ||
		metadataStat.st_size <= 0)
	{
		return false;
	}
	identityOut = cache::CacheValidationKey{
	    .appId = appId,
	    .mtimeSecs = static_cast<long long>(bufferStat.st_mtime),
	    .mtimeNsecs = static_cast<long long>(bufferStat.st_mtim.tv_nsec),
	    .size = static_cast<long long>(bufferStat.st_size),
	    .inode = static_cast<std::uint64_t>(bufferStat.st_ino),
	    .metadataMtimeSecs = static_cast<long long>(metadataStat.st_mtime),
	    .metadataMtimeNsecs = static_cast<long long>(metadataStat.st_mtim.tv_nsec),
	    .metadataSize = static_cast<long long>(metadataStat.st_size),
	    .metadataInode = static_cast<std::uint64_t>(metadataStat.st_ino),
	};
	return true;
}

std::int64_t cachePairMtimeSecsImpl(uint32_t appId) noexcept
{
	cache::CacheValidationKey identity{};
	if (!statBuffer(appId, identity)) return 0;
	return std::max<std::int64_t>(
		identity.mtimeSecs, identity.metadataMtimeSecs);
}

long long provisionTtlSecs()
{
	if (const char* ov = std::getenv("EMBER_PROVISION_TTL"); ov && *ov)
	{
		try { return std::stoll(ov); } catch (...) {}
	}
	return 300;
}

bool persistBuffer(uint32_t appId, uint32_t changeNumber,
                   const std::string& sha20, const std::string& wire,
                   const CachePublicationToken& publication,
                   bool markSynthetic)
{
	(void)getCacheDir();
	std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
	ProcessLock::FileLock cacheLock(cacheLockPath(), false);
	if (!cacheLock.acquired())
	{
		g_pLog->info("AppInfoProvision: unable to lock cache pair for app=%u\n", appId);
		return false;
	}
	if (!cache::cachePublicationAllowed(
	        publication.managed, publication.generation,
	        cachePublicationGenerationLocked(appId)))
	{
		g_pLog->debug(
		    "AppInfoProvision: rejecting stale cache publication for app=%u\n",
		    appId);
		return false;
	}

	if (sha20.size() != 20)
	{
		g_pLog->debug("AppInfoProvision: refuse to persist app=%u, sha size %zu\n",
		              appId, sha20.size());
		return false;
	}
	YAML::Emitter em;
	em << YAML::BeginMap;
	em << YAML::Key << "appid"         << YAML::Value << appId;
	em << YAML::Key << "change_number" << YAML::Value << changeNumber;
	em << YAML::Key << "wire_size"     << YAML::Value << wire.size();
	em << YAML::Key << "sha_b64"       << YAML::Value << base64::to_base64(sha20);
	em << YAML::Key << "normalized"    << YAML::Value << true;
	em << YAML::Key << "synthetic"     << YAML::Value << markSynthetic;
	em << YAML::EndMap;
	const std::string metadata(em.c_str(), em.size());

	const bool markerBefore = SynthMark::isMarked(getCacheDir(), appId);
	std::string writeError;
	if (!publishCachePairLocked(appId, wire, metadata, markSynthetic,
	                            markerBefore, writeError))
	{
		g_pLog->debug(
		    "AppInfoProvision: unable to publish cache pair for app=%u: %s\n",
		    appId, writeError.c_str());
		return false;
	}
	ProvisionTerminal::Store(getCacheDir()).erase(appId);
	g_terminalProvisionResults.erase(appId);
	clearCacheReadInvalidation(appId);

	return memoizePublishedCachePairLocked(appId);
}

[[maybe_unused]]
bool hasDepotsForApp(const std::string& appinfoVdfPath, uint32_t appId)
{
	std::ifstream ifs(appinfoVdfPath, std::ios::binary | std::ios::ate);
	if (!ifs.is_open()) return false;
	const std::streamsize sz = ifs.tellg();
	if (sz <= 0 || sz > (1LL << 30)) return false;
	std::string buf;
	buf.resize(static_cast<std::size_t>(sz));
	ifs.seekg(0);
	ifs.read(buf.data(), sz);

	const auto* data = reinterpret_cast<const std::uint8_t*>(buf.data());
	const std::size_t n = buf.size();
	if (n < 76) return false;

	for (std::size_t i = 0; i + 76 <= n; ++i)
	{
		uint32_t a;
		std::memcpy(&a, data + i, sizeof(a));
		if (a != appId) continue;
		uint32_t entrySize;
		std::memcpy(&entrySize, data + i + 4, sizeof(entrySize));

		if (entrySize < 64 || entrySize > 8u * 1024u * 1024u) continue;
		const std::size_t kvStart = i + 72;
		const std::size_t kvEnd   = i + 8 + entrySize;
		if (kvEnd > n || kvEnd <= kvStart) continue;
		const std::string_view kv(reinterpret_cast<const char*>(data) + kvStart,
		                          kvEnd - kvStart);

		if (kv.find("manifests") != std::string_view::npos)
		{
			return true;
		}

		return false;
	}
	return false;
}

void sha1BytesInternal(const void* data, std::size_t n, std::uint8_t out[20])
{
	static unsigned char* (*p_SHA1)(const unsigned char*, size_t, unsigned char*) = nullptr;
	if (!p_SHA1)
	{
		void* h = dlopen("libcrypto.so.3", RTLD_NOLOAD | RTLD_LAZY);
		if (!h) h = dlopen("libcrypto.so.3", RTLD_LAZY);
		if (!h) h = dlopen("libcrypto.so.1.1", RTLD_LAZY);
		if (!h) h = RTLD_DEFAULT;
		p_SHA1 = (unsigned char* (*)(const unsigned char*, size_t, unsigned char*))
		         dlsym(h, "SHA1");
	}
	if (p_SHA1)
	{
		p_SHA1(reinterpret_cast<const unsigned char*>(data), n, out);
	}
	else
	{
		std::memset(out, 0, 20);
	}
}

bool extractAppNode(const std::string& json, uint32_t appId,
                    YAML::Node& outApp, std::string& err)
{
	try
	{
		YAML::Node root = YAML::Load(json);
		if (!root.IsMap()) { err = "root not a map"; return false; }
		if (!root["data"]) { err = "no 'data'"; return false; }
		const auto data = root["data"];
		if (!data.IsMap()) { err = "data not a map"; return false; }
		const std::string key = std::to_string(appId);
		if (!data[key]) { err = "no entry for appid"; return false; }
		outApp = data[key];
		if (!outApp.IsMap()) { err = "app entry not a map"; return false; }
	}
	catch (const std::exception& e)
	{
		err = e.what();
		return false;
	}
	return true;
}

uint32_t pickChangeNumber(const YAML::Node& app)
{
	if (app["_change_number"])
	{
		try { return app["_change_number"].as<uint32_t>(); }
		catch (...) {}
	}
	return 0;
}

std::string steamRootForConfig()
{
	const char* home = std::getenv("HOME");
	if (!home) return {};
	const std::vector<std::string> candidates = {
		std::string(home) + "/.steam/steam",
		std::string(home) + "/.steam/debian-installation",
		std::string(home) + "/.local/share/Steam",
	};
	for (const auto& c : candidates)
	{
		struct stat st{};
		if (stat((c + "/steam.sh").c_str(), &st) == 0) return c;
	}
	return {};
}

std::unordered_set<uint32_t> collectValidManifestDepotIds()
{
	std::unordered_set<uint32_t> depotIds;

	auto scanDirectory = [&depotIds](const std::filesystem::path& directory)
	{
		std::error_code ec;
		if (!std::filesystem::is_directory(directory, ec) || ec) return;

		std::filesystem::directory_iterator it(directory, ec);
		const std::filesystem::directory_iterator end;
		while (!ec && it != end)
		{
			const auto path = it->path();
			uint32_t depotId = 0;
			if (depotIdFromManifestName(path.filename().string(), depotId) &&
			    ManifestStoreIO::isValidManifest(path))
			{
				depotIds.insert(depotId);
			}
			it.increment(ec);
		}
	};

	scanDirectory(ManifestStore::dir());
	const auto root = steamRootForConfig();
	if (!root.empty())
	{
		scanDirectory(std::filesystem::path(root) / "depotcache");
	}
	return depotIds;
}

bool injectProtonMappings()
{
	if (g_needProton.empty()) return true;
	const auto root = steamRootForConfig();
	if (root.empty()) return false;
	const auto path = root + "/config/config.vdf";
	if (!std::filesystem::exists(path)) return false;

	(void)getCacheDir();
	const auto configLockPath = cacheLockPath();
	ProcessLock::FileLock configLock(configLockPath, false);
	if (!configLock.acquired())
	{
		g_pLog->debug("AppInfoProvision: config.vdf writer lock is busy\n");
		return false;
	}

	AtomicFile::FileIdentity expected{};
	if (!AtomicFile::readIdentity(path, expected)) return false;

	std::string content;
	{
		std::ifstream ifs(path);
		if (!ifs.is_open()) return false;
		std::stringstream ss; ss << ifs.rdbuf();
		content = ss.str();
	}

	std::size_t mapPos = content.find("\"CompatToolMapping\"");
	std::size_t mapBrace = std::string::npos;
	if (mapPos != std::string::npos)
	{
		mapBrace = content.find('{', mapPos);
	}
	else
	{

		const auto steamPos = content.find("\"Steam\"");
		if (steamPos == std::string::npos) return false;
		const auto steamBrace = content.find('{', steamPos);
		if (steamBrace == std::string::npos) return false;
		const std::string block =
			"\n\t\t\t\t\t\"CompatToolMapping\"\n\t\t\t\t\t{\n\t\t\t\t\t}";
		content.insert(steamBrace + 1, block);
		mapPos = content.find("\"CompatToolMapping\"");
		mapBrace = content.find('{', mapPos);
	}
	if (mapBrace == std::string::npos) return false;

	std::string toolName = CompatTool::parseDefaultTool(content);
	if (toolName.empty()) toolName = "proton_experimental";

	int added = 0;
	for (uint32_t appId : g_needProton)
	{
		const std::string key = "\"" + std::to_string(appId) + "\"";

		if (content.find(key, mapBrace) != std::string::npos)
		{
			continue;
		}
		const std::string entry =
			"\n\t\t\t\t\t\t" + key + "\n\t\t\t\t\t\t{\n"
			"\t\t\t\t\t\t\t\"name\"\t\t\"" + toolName + "\"\n"
			"\t\t\t\t\t\t\t\"config\"\t\t\"\"\n"
			"\t\t\t\t\t\t\t\"priority\"\t\t\"250\"\n"
			"\t\t\t\t\t\t}";
		content.insert(mapBrace + 1, entry);
		++added;
	}

	if (added == 0) return true;

	std::string writeError;
	if (!AtomicFile::writeIfUnchanged(path, expected, content, writeError))
	{
		g_pLog->debug(
		    "AppInfoProvision: config.vdf changed before conditional publish: %s\n",
		    writeError.c_str());
		return false;
	}
	g_pLog->infoOnce("AppInfoProvision: injected %d Proton CompatToolMapping entr%s (tool=%s) into config.vdf\n",
	             added, added == 1 ? "y" : "ies", toolName.c_str());
	return true;
}

class CmVdfReader
{
public:
	CmVdfReader(const char* p, const char* end) : p_(p), end_(end) {}

	YAML::Node parseAppinfo()
	{
		std::string key;
		Tok t = next(key);
		if (t != Tok::String) return YAML::Node(YAML::NodeType::Undefined);
		t = next(key );

		if (t != Tok::OpenBrace) return YAML::Node(YAML::NodeType::Undefined);
		return parseObject();
	}

private:
	enum class Tok { String, OpenBrace, CloseBrace, End };

	YAML::Node parseObject()
	{
		YAML::Node node(YAML::NodeType::Map);
		std::string key;
		for (;;)
		{
			Tok t = next(key);
			if (t == Tok::CloseBrace || t == Tok::End) break;
			if (t != Tok::String) break;
			std::string val;
			Tok vt = next(val);
			if (vt == Tok::OpenBrace)
			{
				node[key] = parseObject();
			}
			else if (vt == Tok::String)
			{
				node[key] = val;
			}
			else
			{
				break;
			}
		}
		return node;
	}

	Tok next(std::string& out)
	{
		out.clear();
		skipWs();
		if (p_ >= end_) return Tok::End;
		const char c = *p_;
		if (c == '{') { ++p_; return Tok::OpenBrace; }
		if (c == '}') { ++p_; return Tok::CloseBrace; }
		if (c == '"') return readQuoted(out);
		return readBare(out);
	}

	void skipWs()
	{
		while (p_ < end_)
		{
			const unsigned char c = static_cast<unsigned char>(*p_);
			if (c == ' ' || c == '\t' || c == '\r' || c == '\n') { ++p_; continue; }
			if (c == '/' && p_ + 1 < end_ && p_[1] == '/')
			{
				while (p_ < end_ && *p_ != '\n') ++p_;
				continue;
			}
			break;
		}
	}

	Tok readQuoted(std::string& out)
	{
		++p_;
		while (p_ < end_)
		{
			const char c = *p_++;
			if (c == '"') return Tok::String;
			if (c == '\\' && p_ < end_)
			{
				const char e = *p_++;
				switch (e)
				{
					case 'n':  out.push_back('\n'); break;
					case 't':  out.push_back('\t'); break;
					case 'r':  out.push_back('\r'); break;
					case '"':  out.push_back('"');  break;
					case '\\': out.push_back('\\'); break;
					default:   out.push_back(e);    break;
				}
				continue;
			}
			out.push_back(c);
		}
		return Tok::End;
	}

	Tok readBare(std::string& out)
	{
		while (p_ < end_)
		{
			const unsigned char c = static_cast<unsigned char>(*p_);
			if (c == ' ' || c == '\t' || c == '\r' || c == '\n' ||
			    c == '{' || c == '}' || c == '"') break;
			out.push_back(*p_++);
		}
		return Tok::String;
	}

	const char* p_;
	const char* end_;
};

bool cachedWireRequiresProton(const std::string& wire) noexcept
{
	try
	{
		if (wire.empty()) return false;
		CmVdfReader reader(wire.data(), wire.data() + wire.size());
		const YAML::Node body = reader.parseAppinfo();
		const YAML::Node depots =
			body && body.IsMap() ? body["depots"] : YAML::Node();
		if (!depots || !depots.IsMap()) return false;

		std::size_t contentDepots = 0;
		std::set<std::string> depotOs;
		for (auto it = depots.begin(); it != depots.end(); ++it)
		{
			const std::string key = it->first.as<std::string>("");
			if (key.empty() || std::any_of(key.begin(), key.end(),
				[](char ch) { return ch < '0' || ch > '9'; })) continue;
			const YAML::Node depot = it->second;
			if (!depot || !depot.IsMap()) continue;
			const bool virtualDlc = depot["dlcappid"] && !depot["manifests"];
			if (virtualDlc) continue;
			++contentDepots;
			addDepotPlatforms(depotOs,
				depot["config"] && depot["config"]["oslist"]
					? depot["config"]["oslist"].as<std::string>("")
					: std::string());
		}

		std::string commonOsList;
		if (body["common"] && body["common"]["oslist"])
			commonOsList = body["common"]["oslist"].as<std::string>("");
		return requiresProtonMapping(contentDepots, depotOs, commonOsList);
	}
	catch (...)
	{
		return false;
	}
}

bool readCacheMetadataFile(
	uint32_t appId,
	std::string& storage,
	cache::CacheMetadataView& metadata,
	std::string* diag = nullptr)
{
	const auto reject = [&](const char* message)
	{
		if (diag) *diag = message;
		return false;
	};

	std::ifstream input(getMetaPath(appId), std::ios::binary | std::ios::ate);
	if (!input.is_open()) return reject("metadata is missing");
	const std::streamsize rawSize = input.tellg();
	if (rawSize <= 0 || rawSize > (64LL << 10))
		return reject("metadata size is outside the accepted range");

	storage.assign(static_cast<std::size_t>(rawSize), '\0');
	input.seekg(0, std::ios::beg);
	if (!input.read(storage.data(), rawSize) || input.gcount() != rawSize)
		return reject("metadata read failed");
	if (!cache::parseCacheMetadata(storage, metadata))
		return reject("metadata format is invalid");
	return true;
}

bool cacheMetadataMarkerAllowsRead(
	uint32_t appId,
	const cache::CacheMetadataView& metadata)
{
	{
		std::lock_guard<std::mutex> invalidationLock(g_cacheReadInvalidationMu);
		if (g_cacheReadInvalidated.count(appId) != 0)
			return false;
	}
	const bool markerPresent = SynthMark::isMarked(getCacheDir(), appId);
	return cache::syntheticMarkerStateConsistent(
		metadata.hasSynthetic, metadata.synthetic, markerPresent);
}

bool readValidatedCacheBufferLocked(uint32_t appId, std::string& wireOut,
                                    std::string& diag)
{
	try
	{
		std::string metadataText;
		cache::CacheMetadataView metadata;
		if (!readCacheMetadataFile(
				appId, metadataText, metadata, &diag))
		{
			return false;
		}
		if (!cacheMetadataMarkerAllowsRead(appId, metadata))
		{
			diag = "synthetic marker state is inconsistent";
			return false;
		}
		const std::string declaredSha = std::string(
		    base64::from_base64(std::string(metadata.shaBase64)));

		std::ifstream ifs(getBufferPath(appId), std::ios::binary | std::ios::ate);
		if (!ifs.is_open()) { diag = "buffer is missing"; return false; }
		const std::streamsize rawSize = ifs.tellg();
		if (rawSize <= 0 || rawSize > (16LL << 20))
		{
			diag = "buffer size is outside the accepted range";
			return false;
		}
		std::string wire(static_cast<size_t>(rawSize), '\0');
		ifs.seekg(0, std::ios::beg);
		if (!ifs.read(wire.data(), rawSize))
		{
			diag = "buffer read failed";
			return false;
		}

		std::uint8_t digestBytes[20]{};
		sha1BytesInternal(wire.data(), wire.size(), digestBytes);
		const std::string actualSha(
		    reinterpret_cast<const char*>(digestBytes), sizeof(digestBytes));

		CmVdfReader reader(wire.data(), wire.data() + wire.size());
		const YAML::Node appNode = reader.parseAppinfo();
		const bool parsed = appNode && appNode.IsMap() && appNode.size() > 0;
		const cache::CacheRecordFacts facts{
		    .requestedAppId = appId,
		    .metadataAppId = metadata.appId,
		    .declaredSize = metadata.wireSize,
		    .actualSize = wire.size(),
		    .shaSize = declaredSha.size(),
		    .shaMatches = declaredSha == actualSha,
		    .parsed = parsed,
		    .hasUsableContent = parsed && hasUsableContentDepot(appNode),
		};
		if (!cache::isCacheRecordValid(facts))
		{
			diag = "metadata, SHA-1, or depot validation failed";
			return false;
		}
		wireOut = std::move(wire);
		return true;
	}
	catch (const std::exception& e)
	{
		diag = e.what();
		return false;
	}
}

bool hasValidatedCachedBuffer(uint32_t appId, std::string& diag)
{
	std::string wire;
	return readValidatedCacheBufferLocked(appId, wire, diag);
}

bool cachedWireSizeMatches(uint32_t appId, long long actualSize)
{
	if (actualSize <= 0) return false;
	std::string metadataText;
	cache::CacheMetadataView metadata;
	return readCacheMetadataFile(appId, metadataText, metadata) &&
		cache::wireSizeMatches(
			static_cast<unsigned long long>(actualSize), metadata.wireSize);
}

cache::CacheUse cacheUseForApp(uint32_t appId, bool refreshUnavailable)
{
	ProcessLock::FileLock cacheLock(cacheLockPath(), false);
	if (!cacheLock.acquired()) return cache::CacheUse::None;

	cache::CacheValidationKey key{};
	const bool present = statBuffer(appId, key);
	if (!present) return cache::CacheUse::None;

	const long long now = static_cast<long long>(std::time(nullptr));
	const bool fresh = cache::isBufferReusable(
	    present, key.mtimeSecs, now, provisionTtlSecs());
	if (!cache::shouldValidateCache(fresh, refreshUnavailable))
	{

		return cache::CacheUse::None;
	}

	if (!cachedWireSizeMatches(appId, key.size))
	{
		g_pLog->info("AppInfoProvision: app=%u cached buffer rejected (wire_size mismatch)\n",
		             appId);
		return cache::CacheUse::None;
	}

	CacheValidationResult result;
	{
		std::lock_guard<std::mutex> lk(g_cacheValidationMu);
		auto it = g_cacheValidationMemo.find(key);
		if (it != g_cacheValidationMemo.end())
		{
			result = it->second;
		}
		else
		{
			result.valid = hasValidatedCachedBuffer(appId, result.diag);
			g_cacheValidationMemo.emplace(key, result);
		}
	}
	if (result.valid && !cacheMarkerAllowsRead(appId))
	{
		result.valid = false;
		result.diag = "synthetic marker is missing";
	}
	if (!result.valid)
	{
		g_pLog->info("AppInfoProvision: app=%u cached buffer rejected (%s)\n",
		             appId, result.diag.c_str());
	}
	return cache::chooseCacheUse(result.valid, fresh, refreshUnavailable);
}

CacheProbe probeCacheImpl(uint32_t appId, CacheProbeMode mode)
{
	CacheProbe probe;
	if (mode == CacheProbeMode::NonBlockingMemoOnly)
	{
		std::unique_lock<std::mutex> validationLock(
			g_cacheValidationMu, std::try_to_lock);
		if (!validationLock.owns_lock())
		{
			probe.readiness = CacheReadiness::Busy;
			return probe;
		}
		const auto found = g_cacheProbeMemo.find(appId);
		if (found == g_cacheProbeMemo.end())
			probe.readiness = CacheReadiness::Unverified;
		else
			probe = found->second;
		return probe;
	}

	const auto remember = [appId](CacheProbe result) {
		std::lock_guard<std::mutex> validationLock(g_cacheValidationMu);
		g_cacheProbeMemo[appId] = result;
		return result;
	};
	ProcessLock::FileLock cacheLock(cacheLockPath(), false);
	if (!cacheLock.acquired())
	{
		probe.readiness = CacheReadiness::Invalid;
		return remember(probe);
	}

	struct stat bufferStat{}, metadataStat{};
	const bool bufferExists = stat(getBufferPath(appId).c_str(), &bufferStat) == 0 &&
		bufferStat.st_size > 0;
	const bool metadataExists = stat(getMetaPath(appId).c_str(), &metadataStat) == 0 &&
		metadataStat.st_size > 0;
	if (!bufferExists && !metadataExists) return remember(probe);
	if (!bufferExists || !metadataExists)
	{
		probe.readiness = CacheReadiness::Invalid;
		return remember(probe);
	}

	cache::CacheValidationKey key{};
	if (!statBuffer(appId, key))
	{
		probe.readiness = CacheReadiness::Invalid;
		return remember(probe);
	}
	std::string metadataText;
	cache::CacheMetadataView metadata;
	if (!readCacheMetadataFile(appId, metadataText, metadata) ||
		metadata.appId != appId || metadata.wireSize !=
			static_cast<std::uint64_t>(key.size))
	{
		probe.readiness = CacheReadiness::Invalid;
		return remember(probe);
	}
	probe.changeNumber = metadata.changeNumber;

	CacheValidationResult validation;
	bool memoHit = false;
	{
		std::lock_guard<std::mutex> validationLock(g_cacheValidationMu);
		const auto found = g_cacheValidationMemo.find(key);
		if (found != g_cacheValidationMemo.end())
		{
			validation = found->second;
			memoHit = true;
		}
	}
	if (!memoHit)
	{
		validation.valid = hasValidatedCachedBuffer(appId, validation.diag);
		std::lock_guard<std::mutex> validationLock(g_cacheValidationMu);
		g_cacheValidationMemo.emplace(key, validation);
	}
	if (validation.valid && !cacheMarkerAllowsRead(appId))
		validation.valid = false;
	if (!validation.valid)
	{
		probe.readiness = CacheReadiness::Invalid;
		return remember(probe);
	}
	const long long now = static_cast<long long>(std::time(nullptr));
	probe.readiness = cache::isBufferReusable(
		true, key.mtimeSecs, now, provisionTtlSecs())
		? CacheReadiness::Fresh : CacheReadiness::ValidStale;
	return remember(probe);
}

SourceResult renderAndPersist(uint32_t appId, const YAML::Node& appNode,
                              uint32_t changeNumber,
                              const CachePublicationToken& publication)
{
	std::string wire;
	bool synthesized = false;
	const SourceResult renderResult = renderAppinfoBuffer(
	    appNode, appId, wire, publication, &synthesized);
	if (renderResult != SourceResult::Success)
	{
		const char* reason = "invalid response";
		if (renderResult == SourceResult::IncompleteContent)
			reason = "response contains no concrete depot data";
		else if (renderResult == SourceResult::NoUsableContent)
			reason = "concrete depots are not usable";
		else if (renderResult == SourceResult::VirtualDlc)
			reason = "DLC has no usable content depots";
		g_pLog->info("AppInfoProvision: app=%u render stopped (%s)\n", appId, reason);
		return renderResult;
	}
	if (wire.find("\"depots\"") == std::string::npos)
	{
		g_pLog->info("AppInfoProvision: app=%u buffer has no depots, skipping\n", appId);
		return SourceResult::IncompleteContent;
	}

	std::string sha20;
	{
		std::uint8_t tmp[20];
		sha1BytesInternal(wire.data(), wire.size(), tmp);
		sha20.assign(reinterpret_cast<const char*>(tmp), 20);
	}

	if (!persistBuffer(appId, changeNumber, sha20, wire, publication,
	                   synthesized))
	{
		g_pLog->info("AppInfoProvision: app=%u failed to persist buffer to cache\n", appId);
		return SourceResult::LocalFailure;
	}

	g_pLog->infoOnce("AppInfoProvision: app=%u provisioned (change=%u, %zu bytes wire)\n",
	             appId, changeNumber, wire.size());
	return SourceResult::Success;
}

SourceResult provisionAppFromCmBuffer(uint32_t appId, const std::string& cmWire,
                                      uint32_t changeNumber,
                                      const CachePublicationToken& publication)
{
	if (cmWire.empty()) return SourceResult::InvalidResponse;
	CmVdfReader reader(cmWire.data(), cmWire.data() + cmWire.size());
	YAML::Node appNode = reader.parseAppinfo();
	if (!appNode || !appNode.IsMap() || appNode.size() == 0)
	{
		g_pLog->info("AppInfoProvision: app=%u CM buffer parse failed, fallback\n", appId);
		return SourceResult::InvalidResponse;
	}
	return renderAndPersist(appId, appNode, changeNumber, publication);
}

struct ProvisionPassContext
{
	std::unordered_map<uint32_t, std::string> cmBuffers;
	std::unordered_map<uint32_t, uint32_t> cmChanges;
	std::unordered_map<uint32_t, CachePublicationToken> cachePublications;
};

struct PendingProtonLoad
{
	bool lockAcquired = false;
	PendingProtonFileStatus status = PendingProtonFileStatus::Invalid;
	std::set<uint32_t> fileIds;
	std::set<uint32_t> ids;
};

PendingProtonLoad loadPendingProtonMappings()
{
	ProcessLock::FileLock cacheLock(cacheLockPath(), false);
	if (!cacheLock.acquired()) return {};

	const auto file = readPendingProtonFileLocked();
	PendingProtonLoad result;
	result.lockAcquired = true;
	result.status = file.status;
	result.fileIds = file.ids;
	if (file.status != PendingProtonFileStatus::Valid) return result;

	const auto managed = g_config.managedAppIds.get();
	for (uint32_t appId : file.ids)
	{
		if (managed.count(appId) != 0)
			result.ids.insert(appId);
	}
	return result;
}

bool removePendingProtonMappingLocked(uint32_t appId)
{
	const auto file = readPendingProtonFileLocked();
	if (file.status == PendingProtonFileStatus::Missing) return true;
	if (file.status != PendingProtonFileStatus::Valid)
	{
		if (g_pLog)
			g_pLog->debug("AppInfoProvision: preserving invalid or unreadable pending Proton file while removing app=%u\n",
		              appId);
		return false;
	}

	std::set<uint32_t> pending = file.ids;
	if (pending.erase(appId) == 0) return true;

	if (pending.empty())
	{
		std::error_code ec;
		std::filesystem::remove(pendingProtonPath(), ec);
		return !ec;
	}

	std::string content;
	for (uint32_t pendingAppId : pending)
		content += std::to_string(pendingAppId) + "\n";
	std::string error;
	const bool removed = AtomicFile::write(pendingProtonPath(), content, error);
	if (!removed && g_pLog)
	{
		g_pLog->debug("AppInfoProvision: cannot remove pending Proton mapping for app=%u: %s\n",
		              appId, error.c_str());
	}
	return removed;
}

bool clearPendingProtonMappingsIfUnchanged(
    const std::set<uint32_t>& expectedFileIds)
{
	ProcessLock::FileLock cacheLock(cacheLockPath(), false);
	if (!cacheLock.acquired()) return false;

	const auto current = readPendingProtonFileLocked();
	if (current.status != PendingProtonFileStatus::Valid ||
	    current.ids != expectedFileIds)
	{
		if (g_pLog)
			g_pLog->debug(
			    "AppInfoProvision: pending Proton file changed during flush; preserving it\n");
		return false;
	}

	std::error_code ec;
	std::filesystem::remove(pendingProtonPath(), ec);
	return !ec;
}

bool isSynthesizedAppLocked(uint32_t appId)
{
	if (appId == 0) return false;
	const bool active = g_config.isAddedAppId(appId);
	bool invalidated = false;
	{
		std::lock_guard<std::mutex> invalidationLock(g_cacheReadInvalidationMu);
		invalidated = g_cacheReadInvalidated.count(appId) != 0;
	}

	if (invalidated && !active) return false;
	const bool markerPresent = SynthMark::isMarked(getCacheDir(), appId);
	if (!markerPresent) return false;
	std::string metadataText;
	cache::CacheMetadataView metadata;
	if (readCacheMetadataFile(appId, metadataText, metadata))
	{
		if (metadata.hasSynthetic)
			return metadata.synthetic;

		return true;
	}

	std::error_code metadataError;
	const bool metadataPresent = std::filesystem::exists(
	    getMetaPath(appId), metadataError);
	return !metadataError &&
	       cache::retainedSyntheticMarkerProtectionAllowed(
	           markerPresent, metadataPresent, active);
}

}

bool requiresProton(std::uint32_t appId) noexcept
{
	try
	{
		std::lock_guard<std::mutex> passLock(g_provisionPassMu);
		return appId != 0 && g_needProton.count(appId) != 0;
	}
	catch (...)
	{
		return false;
	}
}

CacheProbe probeCache(uint32_t appId, CacheProbeMode mode)
{
	return probeCacheImpl(appId, mode);
}

std::int64_t cachePairMtimeSecs(uint32_t appId) noexcept
{
	return cachePairMtimeSecsImpl(appId);
}

std::int64_t dlcMetadataCacheMtimeSecs(uint32_t appId) noexcept
{
	struct stat st{};
	if (appId == 0 || stat(getDlcMetadataPath(appId).c_str(), &st) != 0)
		return 0;
	return static_cast<std::int64_t>(st.st_mtime);
}

void sha1Bytes(const void* data, std::size_t size, std::uint8_t out[20])
{
	sha1BytesInternal(data, size, out);
}

bool publishCachePairLocked(uint32_t appId, const std::string& wire,
                            const std::string& metadata, bool synthetic,
                            bool markerBefore, std::string& error)
{
	const auto cacheDir = getCacheDir();

	g_terminalProvisionResults.erase(appId);
	if (!ProvisionTerminal::Store(cacheDir).erase(appId))
	{
		error = "unable to invalidate terminal sidecar";
		return false;
	}
	const bool published = CachePair::publish(
		getBufferPath(appId), getMetaPath(appId), wire, metadata, synthetic,
		markerBefore,
		[&](bool desired) {
			return desired ? SynthMark::mark(cacheDir, appId)
			               : SynthMark::unmark(cacheDir, appId);
		},
		[&] { return SynthMark::isMarked(cacheDir, appId); }, error);
	if (!published) return false;

	if (cache::shouldInvalidateDlcMetadata(published))
	{
		std::error_code metadataError;
		std::filesystem::remove(getDlcMetadataPath(appId), metadataError);
		if (metadataError)
		{

			g_pLog->debug(
				"AppInfoProvision: cache pair committed for app=%u but DLC "
				"metadata cleanup failed: %s\n",
				appId, metadataError.message().c_str());
		}
	}
	return true;
}

bool memoizePublishedCachePairLocked(uint32_t appId)
{
	cache::CacheValidationKey publishedKey{};
	const bool identityCaptured = statBuffer(appId, publishedKey);
	std::string validationDiag;
	std::string validatedWire;
	const bool publishedPairValid = identityCaptured &&
		readValidatedCacheBufferLocked(appId, validatedWire, validationDiag);
	std::string metadataText;
	cache::CacheMetadataView metadata;
	const bool metadataReady = publishedPairValid &&
		readCacheMetadataFile(appId, metadataText, metadata) &&
		metadata.appId == appId;
	bool memoized = false;
	{
		std::lock_guard<std::mutex> validationLock(g_cacheValidationMu);
		memoized = memoizeValidatedPublication(
			g_cacheValidationMemo, true,
			publishedPairValid,
			identityCaptured
				? std::optional<cache::CacheValidationKey>{publishedKey}
				: std::nullopt,
			CacheValidationResult{true, {}});
		if (memoized && metadataReady)
		{
			const long long now = static_cast<long long>(std::time(nullptr));
			g_cacheProbeMemo[appId] = CacheProbe{
				cache::isBufferReusable(
					true, publishedKey.mtimeSecs, now, provisionTtlSecs())
					? CacheReadiness::Fresh : CacheReadiness::ValidStale,
				metadata.changeNumber};
		}
	}
	if (!memoized && g_pLog)
	{
		g_pLog->debug(
			"AppInfoProvision: published cache pair failed post-write validation "
			"for app=%u: %s\n", appId,
			validationDiag.empty() ? "identity unavailable" : validationDiag.c_str());
	}
	return memoized;
}

bool readValidatedCacheBuffer(uint32_t appId, std::string& buffer)
{
	ProcessLock::FileLock cacheLock(cacheLockPath(), false);
	if (!cacheLock.acquired()) return false;
	std::string diag;
	return readValidatedCacheBufferLocked(appId, buffer, diag);
}

namespace
{

struct AuthMemoKey
{
	std::int64_t pairMtime = -1;
	std::int64_t dlcMetaMtime = -1;
	bool managed = false;
	bool synthetic = false;

	bool operator==(const AuthMemoKey& o) const
	{
		return pairMtime == o.pairMtime && dlcMetaMtime == o.dlcMetaMtime &&
			managed == o.managed && synthetic == o.synthetic;
	}
};

struct AuthContribution
{
	std::vector<std::uint32_t> ids;
	bool memoizable = true;
};

AuthContribution computeAuthContribution(
	std::uint32_t appId, bool managed, bool synthetic)
{
	AuthContribution out;
	bool authoritative = false;

	if (cache::locallyAuthoritative({
			.managed = managed, .active = true, .synthetic = synthetic}))
	{
		authoritative = true;
	}
	else if (managed)
	{

		ProcessLock::FileLock cacheLock(cacheLockPath(), false);
		if (cacheLock.heldByAnother())
		{
			out.memoizable = false;
			return out;
		}
		std::string wire;
		std::string validationDiag;
		if (readValidatedCacheBufferLocked(appId, wire, validationDiag))
		{
			std::string metadataText;
			cache::CacheMetadataView metadata;
			if (readCacheMetadataFile(appId, metadataText, metadata) &&
				metadata.appId == appId && cache::locallyAuthoritative({
					.managed = true,
					.active = true,
					.cacheValid = true,
					.hasNormalizedMarker = metadata.hasNormalized,
					.normalized = metadata.normalized}))
			{
				authoritative = true;
			}
		}
	}

	if (!authoritative) return out;
	out.ids.push_back(appId);
	if (managed)
	{
		DlcMetadata::CacheRecord metadata;
		if (readValidatedDlcMetadataCache(appId, 0, metadata))
		{
			std::unordered_set<std::uint32_t> children;
			DlcMetadata::appendChildAppIds(metadata, children);
			out.ids.insert(out.ids.end(), children.begin(), children.end());
		}
	}
	return out;
}

std::mutex g_authMemoMu;
std::unordered_map<std::uint32_t, std::pair<AuthMemoKey, std::vector<std::uint32_t>>>
	g_authMemo;
}

std::unordered_set<std::uint32_t> locallyAuthoritativeApps(
	const std::unordered_set<std::uint32_t>& managedCandidates,
	const std::unordered_set<std::uint32_t>& activeCandidates)
{
	std::unordered_set<std::uint32_t> authoritative;
	authoritative.reserve(activeCandidates.size());

	std::lock_guard<std::mutex> memoLock(g_authMemoMu);
	for (auto it = g_authMemo.begin(); it != g_authMemo.end();)
		it = activeCandidates.count(it->first) == 0 ? g_authMemo.erase(it)
		                                            : std::next(it);

	for (const std::uint32_t appId : activeCandidates)
	{
		const AuthMemoKey key{
			cachePairMtimeSecs(appId),
			dlcMetadataCacheMtimeSecs(appId),
			managedCandidates.count(appId) != 0,
			isSynthesizedApp(appId),
		};
		const auto it = g_authMemo.find(appId);
		if (it != g_authMemo.end() && it->second.first == key)
		{
			for (const std::uint32_t id : it->second.second)
				authoritative.insert(id);
			continue;
		}
		const AuthContribution contribution =
			computeAuthContribution(appId, key.managed, key.synthetic);
		if (contribution.memoizable)
			g_authMemo[appId] = {key, contribution.ids};
		else
			g_authMemo.erase(appId);
		for (const std::uint32_t id : contribution.ids)
			authoritative.insert(id);
	}
	return authoritative;
}

bool readValidatedDlcMetadataCache(
	std::uint32_t baseAppId,
	std::uint64_t expectedGeneration,
	DlcMetadata::CacheRecord& record)
{
	record = {};
	if (baseAppId == 0) return false;
	std::error_code metadataPathError;
	if (!std::filesystem::is_regular_file(
		getDlcMetadataPath(baseAppId), metadataPathError) || metadataPathError)
		return false;

	std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
	const bool managed = g_config.managedAppIds.get().count(baseAppId) != 0;
	const std::uint64_t currentGeneration =
		cachePublicationGenerationLocked(baseAppId);
	if (!managed) return false;

	ProcessLock::FileLock cacheLock(cacheLockPath(), false);
	if (!cacheLock.acquired()) return false;
	std::string validatedBaseWire;
	std::string baseValidationDiag;
	if (!readValidatedCacheBufferLocked(
		baseAppId, validatedBaseWire, baseValidationDiag)) return false;
	std::string baseMetadataText;
	cache::CacheMetadataView baseMetadata;
	if (!readCacheMetadataFile(
		baseAppId, baseMetadataText, baseMetadata) ||
		baseMetadata.appId != baseAppId) return false;
	std::string encoded;
	if (!readTextFileBounded(
		getDlcMetadataPath(baseAppId), 32u << 20, encoded)) return false;
	if (!DlcMetadata::decodeCache(encoded, record) ||
		record.baseAppId != baseAppId) return false;
	std::uint8_t baseDigest[20]{};
	sha1BytesInternal(
		validatedBaseWire.data(), validatedBaseWire.size(), baseDigest);
	const std::string baseSha(
		reinterpret_cast<const char*>(baseDigest), sizeof(baseDigest));
	if (!DlcMetadata::baseIdentityMatches(
		record.baseChangeNumber, record.baseSha,
		baseMetadata.changeNumber, baseSha)) return false;
	if (!DlcMetadata::cacheGenerationMatches(
		record.baseGeneration, expectedGeneration, currentGeneration))
		return false;
	for (const auto& app : record.apps)
	{
		std::string normalized;
		if (!DlcMetadata::normalize(
			app.wireBuffer, app.appid, baseAppId, normalized) ||
			normalized != app.wireBuffer) return false;
		std::uint8_t digest[20]{};
		sha1BytesInternal(
			app.wireBuffer.data(), app.wireBuffer.size(), digest);
		if (app.sha != std::string(
			reinterpret_cast<const char*>(digest), sizeof(digest))) return false;
	}
	return true;
}

static bool dlcMetadataPublicationMatchesBaseLocked(
	const DlcMetadata::CacheRecord& record,
	std::uint64_t expectedGeneration)
{
	if (record.baseAppId == 0) return false;
	std::string currentWire;
	std::string validationDiag;
	if (!readValidatedCacheBufferLocked(
		record.baseAppId, currentWire, validationDiag)) return false;
	std::string metadataText;
	cache::CacheMetadataView metadata;
	if (!readCacheMetadataFile(
		record.baseAppId, metadataText, metadata) ||
		metadata.appId != record.baseAppId) return false;
	std::uint8_t digest[20]{};
	sha1BytesInternal(currentWire.data(), currentWire.size(), digest);
	const std::string currentSha(
		reinterpret_cast<const char*>(digest), sizeof(digest));
	return DlcMetadata::publicationMatchesBase(
		record.baseGeneration, expectedGeneration,
		cachePublicationGenerationLocked(record.baseAppId),
		record.baseChangeNumber, record.baseSha,
		metadata.changeNumber, currentSha);
}

std::mutex& cachePublicationMutex()
{
	return g_cachePublicationMu;
}

std::uint64_t cachePublicationGenerationLocked(uint32_t appId)
{
	const auto it = g_cachePublicationGenerations.find(appId);
	return it == g_cachePublicationGenerations.end() ? 0 : it->second;
}

std::string localContentFingerprint(uint32_t appId)
{
	return localContentFingerprint(appId, ManifestStore::archivedGidIndex());
}

std::string localContentFingerprint(
	uint32_t appId,
	const ManifestStore::ArchivedGidIndex& archivedGids)
{
	std::vector<ProvisionTerminal::LocalInput> inputs;
	for (const uint32_t depotId : DepotKey::managedDepotsForApp(appId))
	{
		const auto key = DepotKey::getCachedKey(depotId);
		inputs.push_back({depotId, key.key, 0});
	}
	const std::string result = fingerprintIndexedLocalInputs(
		std::move(inputs), archivedGids);
	{
		std::lock_guard<std::mutex> lock(g_cachePublicationMu);
		g_localContentFingerprints[appId] = result;
	}
	return result;
}

bool terminalMemoryAppliesLocked(uint32_t appId,
	uint32_t observedChangeNumber, ProvisionOutcome* outcome,
	uint32_t* recordChangeNumber)
{
	const auto it = g_terminalProvisionResults.find(appId);
	if (it == g_terminalProvisionResults.end()) return false;
	if (!cache::terminalResultStillApplies(
		true, it->second.managedGeneration,
		cachePublicationGenerationLocked(appId))) return false;
	const auto fingerprint = g_localContentFingerprints.find(appId);
	const std::string_view currentFingerprint = fingerprint ==
		g_localContentFingerprints.end() ? std::string_view{} : fingerprint->second;
	if (it->second.record.changeNumber == 0)
	{
		if (observedChangeNumber != 0 || it->second.record.appId != appId ||
			(it->second.record.kind != ProvisionTerminal::Kind::VirtualDlc &&
			 it->second.record.inputFingerprint != currentFingerprint))
			return false;
	}
	else if (!ProvisionTerminal::applies(
		it->second.record, appId,
		observedChangeNumber == 0 ? it->second.record.changeNumber
			: observedChangeNumber, currentFingerprint))
		return false;
	if (outcome)
		*outcome = it->second.record.kind == ProvisionTerminal::Kind::VirtualDlc
			? ProvisionOutcome::NotApplicable
			: ProvisionOutcome::NoUsableContent;
	if (recordChangeNumber) *recordChangeNumber = it->second.record.changeNumber;
	return true;
}

bool terminalProvisionResultKnown(uint32_t appId,
	uint32_t observedChangeNumber, ProvisionOutcome* outcome)
{
	const std::string fingerprint = localContentFingerprint(appId);

	std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
	if (terminalMemoryAppliesLocked(appId, observedChangeNumber,
		outcome, nullptr)) return true;
	if (!g_config.isAddedAppId(appId)) return false;
	ProcessLock::FileLock cacheLock(cacheLockPath(), false);
	if (!cacheLock.acquired()) return false;
	const auto loaded = ProvisionTerminal::Store(getCacheDir()).load(appId);
	if (loaded.status != ProvisionTerminal::LoadStatus::Valid)
	{
		bool logMalformed = false;
		if (loaded.status == ProvisionTerminal::LoadStatus::Invalid)
			logMalformed = g_loggedMalformedTerminal.insert(appId).second;
		if (logMalformed && g_pLog)
			g_pLog->debug("AppInfoProvision: app=%u terminal sidecar is malformed\n", appId);
		return false;
	}
	if (!ProvisionTerminal::applies(loaded.record, appId,
		observedChangeNumber == 0 ? loaded.record.changeNumber
			: observedChangeNumber, fingerprint)) return false;
	g_terminalProvisionResults[appId] = {
		loaded.record, cachePublicationGenerationLocked(appId)};
	if (outcome)
		*outcome = loaded.record.kind == ProvisionTerminal::Kind::VirtualDlc
			? ProvisionOutcome::NotApplicable
			: ProvisionOutcome::NoUsableContent;
	return true;
}

bool noteTerminalProvisionResult(uint32_t appId, SourceResult result,
	uint32_t changeNumber, const CachePublicationToken& publication)
{

	if (result != SourceResult::VirtualDlc &&
	    result != SourceResult::NoUsableContent &&
	    result != SourceResult::IncompleteContent)
		return false;
	ProvisionTerminal::Record record{
		.schema = ProvisionTerminal::kSchema,
		.appId = appId,
		.kind = result == SourceResult::VirtualDlc
			? ProvisionTerminal::Kind::VirtualDlc
			: ProvisionTerminal::Kind::NoUsableContent,
		.changeNumber = changeNumber,
		.inputFingerprint = result == SourceResult::VirtualDlc
			? "-" : localContentFingerprint(appId),
	};
	std::lock_guard<std::mutex> lock(g_cachePublicationMu);
	ProcessLock::FileLock cacheLock(cacheLockPath(), false);
	if (!cacheLock.acquired() || !cache::cachePublicationAllowed(
		publication.managed, publication.generation,
		cachePublicationGenerationLocked(appId))) return false;

	g_terminalProvisionResults.erase(appId);
	if (!ProvisionTerminal::Store(getCacheDir()).erase(appId)) return false;
	if (changeNumber != 0 && !ProvisionTerminal::Store(getCacheDir()).publish(record))
		return false;
	g_terminalProvisionResults[appId] = {record, publication.generation};
	return true;
}

void clearTerminalProvisionResult(uint32_t appId)
{
	std::lock_guard<std::mutex> lock(g_cachePublicationMu);
	g_terminalProvisionResults.erase(appId);
	ProvisionTerminal::Store(getCacheDir()).erase(appId);
}

TerminalObservation observeTerminal(uint32_t appId,
	uint32_t observedChangeNumber)
{
	TerminalObservation observation;
	std::lock_guard<std::mutex> lock(g_cachePublicationMu);
	observation.applies = terminalMemoryAppliesLocked(
		appId, observedChangeNumber, &observation.outcome,
		&observation.changeNumber);
	return observation;
}

void primeTerminalMemo(uint32_t appId, std::string_view fingerprint)
{
	if (appId == 0) return;
	std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
	g_localContentFingerprints[appId] = fingerprint;
	if (!g_config.isAddedAppId(appId)) return;
	ProcessLock::FileLock cacheLock(cacheLockPath(), false);
	if (!cacheLock.acquired()) return;
	const auto loaded = ProvisionTerminal::Store(getCacheDir()).load(appId);
	if (loaded.status != ProvisionTerminal::LoadStatus::Valid ||
		!ProvisionTerminal::applies(
			loaded.record, appId, loaded.record.changeNumber, fingerprint))
		return;
	g_terminalProvisionResults[appId] = {
		loaded.record, cachePublicationGenerationLocked(appId)};
}

void primeTerminalMemo(uint32_t appId)
{
	primeTerminalMemo(appId, localContentFingerprint(appId));
}

CachePublicationToken snapshotCachePublication(uint32_t appId)
{
	CachePublicationToken token;
	std::lock_guard<std::mutex> passLock(g_provisionPassMu);
	token.managed = g_config.managedAppIds.get().count(appId) != 0;
	std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
	token.generation = cachePublicationGenerationLocked(appId);
	return token;
}

std::mutex& provisioningPassMutex()
{
	return g_provisionPassMu;
}

ProvisionOutcome provisionAppDetailed(uint32_t appId,
                                      const std::string& appinfoVdfPath,
                                      ProvisionPassState& pass,
                                      ProvisionPassContext& context,
                                      bool forceRefresh = false)
{
	(void)appinfoVdfPath;
	if (appId == 0) return ProvisionOutcome::IncompleteContent;

	auto publicationIt = context.cachePublications.find(appId);
	if (publicationIt == context.cachePublications.end())
	{
		publicationIt = context.cachePublications.emplace(
		    appId, snapshotCachePublication(appId)).first;
	}
	const CachePublicationToken publication = publicationIt->second;

	if (!forceRefresh &&
	    cacheUseForApp(appId, false) == cache::CacheUse::Fresh)
	{
		g_pLog->debug("AppInfoProvision: app=%u reusing validated same-boot cache\n",
		              appId);
		return ProvisionOutcome::FreshCache;
	}

	{
		auto it = context.cmBuffers.find(appId);
		if (it != context.cmBuffers.end())
		{
			uint32_t cn = 0;
			if (auto ci = context.cmChanges.find(appId); ci != context.cmChanges.end())
				cn = ci->second;
			const SourceResult cmResult =
			    provisionAppFromCmBuffer(appId, it->second, cn, publication);
			if (cmResult == SourceResult::Success)
			{
				g_pLog->info("AppInfoProvision: app=%u provisioned via CM\n", appId);
				return ProvisionOutcome::Updated;
			}
			if (!shouldTryProviderFallback(cmResult))
			{
				g_pLog->info(
				    "AppInfoProvision: app=%u CM result is terminal (%s); "
				    "provider fallback suppressed\n", appId,
				    cmResult == SourceResult::NoUsableContent
				        ? "concrete depots are not usable"
				        : cmResult == SourceResult::VirtualDlc
				            ? "DLC has no usable content depots"
				            : cmResult == SourceResult::IncompleteContent
				                ? "no depot data available anonymously"
				                : "local cache write failed");

				if (cmResult == SourceResult::NoUsableContent ||
				    cmResult == SourceResult::VirtualDlc ||
				    cmResult == SourceResult::IncompleteContent)
					noteTerminalProvisionResult(appId, cmResult, cn, publication);
				if (cmResult == SourceResult::VirtualDlc)
					return ProvisionOutcome::NotApplicable;
				if (cmResult == SourceResult::NoUsableContent)
					return ProvisionOutcome::NoUsableContent;
				return cmResult == SourceResult::LocalFailure
				    ? ProvisionOutcome::LocalFailure
				    : ProvisionOutcome::IncompleteContent;
			}
			g_pLog->info("AppInfoProvision: app=%u CM response %s, trying steamcmd\n",
			             appId,
			             cmResult == SourceResult::IncompleteContent
			                 ? "contains no concrete depot data"
			                 : "is invalid");
		}
	}

	if (!pass.shouldAttemptProvider())
	{
		if (cacheUseForApp(appId, true) == cache::CacheUse::Fallback)
		{
			g_pLog->info("AppInfoProvision: app=%u using validated cached buffer "
			             "because live sources are unavailable\n", appId);
			return ProvisionOutcome::FallbackCache;
		}
		return ProvisionOutcome::NetworkUnavailable;
	}

	std::string body, diag;
	std::string url;
	bool fetched = false;
	NetworkFailure finalFailure = NetworkFailure::Provider;
	for (const auto& tmpl : providerChain())
	{
		url = expandUrl(tmpl, appId);
		g_pLog->info("AppInfoProvision: app=%u GET %s\n", appId, url.c_str());

		finalFailure = retryNetworkOperation(
			[&] {
				return httpGetJson(
				    url, body, diag,
				    pass.providerOperationTimeoutMs(12000));
			},
			3, 1000,
			[&](int ms) {
				const long delay = pass.providerDelayMs(ms);
				if (delay > 0)
					std::this_thread::sleep_for(std::chrono::milliseconds(delay));
			});
		if (finalFailure == NetworkFailure::None)
		{
			fetched = true;
			pass.noteProviderSuccess();
			break;
		}

		g_pLog->info("AppInfoProvision: app=%u provider failed after retries (%s), trying next\n",
		             appId, diag.c_str());
		if (pass.providerBudgetExhausted()) break;
	}
	if (!fetched)
	{
		g_pLog->info("AppInfoProvision: app=%u all providers failed\n", appId);
		pass.noteFinalProviderFailure(finalFailure);
		if (pass.providerCircuitOpen() &&
		    cacheUseForApp(appId, true) == cache::CacheUse::Fallback)
		{
			g_pLog->info("AppInfoProvision: app=%u using validated cached buffer "
			             "after provider transport failure\n", appId);
			return ProvisionOutcome::FallbackCache;
		}
		return pass.providerCircuitOpen()
		    ? ProvisionOutcome::NetworkUnavailable
		    : ProvisionOutcome::IncompleteContent;
	}

	YAML::Node appNode;
	std::string err;
	if (!extractAppNode(body, appId, appNode, err))
	{
		g_pLog->info("AppInfoProvision: app=%u parse failed: %s\n", appId, err.c_str());
		return ProvisionOutcome::IncompleteContent;
	}
	const uint32_t changeNumber = pickChangeNumber(appNode);

	std::string wire;
	bool synthesized = false;
	const SourceResult renderResult = renderAppinfoBuffer(
	    appNode, appId, wire, publication, &synthesized);
	if (renderResult != SourceResult::Success)
	{
		const char* reason = "invalid response";
		if (renderResult == SourceResult::IncompleteContent)
			reason = "response contains no concrete depot data";
		else if (renderResult == SourceResult::NoUsableContent)
			reason = "concrete depots are not usable";
		else if (renderResult == SourceResult::VirtualDlc)
			reason = "DLC has no usable content depots";
		g_pLog->info("AppInfoProvision: app=%u render stopped (%s)\n", appId, reason);
		if (renderResult == SourceResult::VirtualDlc ||
		    renderResult == SourceResult::NoUsableContent)
			noteTerminalProvisionResult(
				appId, renderResult, changeNumber, publication);
		if (renderResult == SourceResult::VirtualDlc)
			return ProvisionOutcome::NotApplicable;
		if (renderResult == SourceResult::NoUsableContent)
			return ProvisionOutcome::NoUsableContent;
		return renderResult == SourceResult::LocalFailure
		    ? ProvisionOutcome::LocalFailure
		    : ProvisionOutcome::IncompleteContent;
	}

	if (wire.find("\"depots\"") == std::string::npos)
	{
		g_pLog->info("AppInfoProvision: app=%u JSON has no depots, skipping\n", appId);
		return ProvisionOutcome::IncompleteContent;
	}

	std::string sha20;
	{
		std::uint8_t tmp[20];
		sha1BytesInternal(wire.data(), wire.size(), tmp);
		sha20.assign(reinterpret_cast<const char*>(tmp), 20);
	}

	if (!persistBuffer(appId, changeNumber, sha20, wire, publication,
	                   synthesized))
	{
		g_pLog->info("AppInfoProvision: app=%u failed to persist buffer to cache\n", appId);
		return ProvisionOutcome::LocalFailure;
	}

	g_pLog->infoOnce("AppInfoProvision: app=%u provisioned (change=%u, %zu bytes wire)\n",
	             appId, changeNumber, wire.size());
	return ProvisionOutcome::Updated;
}

bool provisionApp(uint32_t appId, const std::string& appinfoVdfPath)
{
	ProvisionPassState pass;
	ProvisionPassContext context;
	return isProvisioned(provisionAppDetailed(
		appId, appinfoVdfPath, pass, context));
}

std::vector<std::uint32_t> publishRuntimeAppInfo(
	const std::string& appinfoVdfPath,
	const std::vector<RefreshRequest>& requested);

void enrichAndPublishDlcMetadata(
	const std::string& appinfoVdfPath,
	const std::vector<RefreshRequest>& publishedBases);

int provisionAppsPass(const std::string& appinfoVdfPath,
                       const std::unordered_set<uint32_t>& added,
                       bool onlyMissing, ProvisionPassContext& context,
	                       std::unordered_set<uint32_t>* fallbackApps,
	                       const char* origin)
{
	if (added.empty()) return 0;
	ProvisionPassSummary summary;
	summary.requested = added.size();

	ProvisionPassCoordinator coordinator(g_provisionPassMu);
	coordinator.snapshot([&] {
		const auto managed = g_config.managedAppIds.get();
		std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
		context.cachePublications.clear();
		for (const uint32_t appId : added)
		{
			context.cachePublications.emplace(
			    appId,
			    CachePublicationToken{
			        .managed = managed.count(appId) != 0,
			        .generation = cachePublicationGenerationLocked(appId),
			    });
		}
	});

	ProvisionPassState pass;
	const bool cmDisabled = [] {
		const char* v = std::getenv("EMBER_DISABLE_CM");
		return v && *v && std::string(v) != "0";
	}();
	if (!cmDisabled)
	{
		std::vector<uint32_t> toFetch;
		for (uint32_t appId : added)
		{
			if (!onlyMissing) primeTerminalMemo(appId);
			const bool terminalKnown = onlyMissing
				? observeTerminal(appId, 0).applies
				: terminalProvisionResultKnown(appId, 0, nullptr);
			if (terminalKnown)
				continue;
			if (onlyMissing)
			{

				toFetch.push_back(appId);
			}
			else if (cacheUseForApp(appId, false) != cache::CacheUse::Fresh)
			{
				toFetch.push_back(appId);
			}
		}
		if (!toFetch.empty())
		{
			summary.fetched = toFetch.size();
			g_pLog->info("AppInfoProvision: fetching %zu app(s) via native CM\n",
			             toFetch.size());
			const auto cmResult = coordinator.network([&] {
				return CmClient::fetchProductInfoDetailed(
				    toFetch, context.cmBuffers, &context.cmChanges);
			});
			if (cmResult != CmClient::FetchResult::Success)
			{
				context.cmBuffers.clear();
				context.cmChanges.clear();
				pass.noteCmBatchFailure();
				if (cmResult == CmClient::FetchResult::NetworkUnavailable)
				{
					pass.noteFinalProviderFailure(NetworkFailure::Connectivity);
					g_pLog->info("AppInfoProvision: native CM batch found no network; "
					             "using validated local buffers\n");
				}
				else
				{
					g_pLog->info("AppInfoProvision: native CM batch failed, "
					             "probing provider fallback\n");
				}
			}
		}
	}

	int provisioned = 0;
	auto appIt = added.begin();
	while (appIt != added.end())
	{
		const uint32_t appId = *appIt++;
		ProvisionOutcome outcome = ProvisionOutcome::IncompleteContent;
		uint32_t observedChange = 0;
		if (const auto change = context.cmChanges.find(appId);
			change != context.cmChanges.end()) observedChange = change->second;
		const bool terminalKnown = onlyMissing
			? observeTerminal(appId, observedChange).applies
			: terminalProvisionResultKnown(appId, observedChange, &outcome);
		if (!terminalKnown)
			outcome = provisionAppDetailed(appId, appinfoVdfPath, pass, context);
		if (fallbackApps && outcome == ProvisionOutcome::FallbackCache)
			fallbackApps->insert(appId);
		if (outcome == ProvisionOutcome::Updated) ++summary.updated;
		else if (outcome == ProvisionOutcome::FreshCache) ++summary.ready;
		else if (outcome == ProvisionOutcome::FallbackCache) ++summary.fallback;
		else if (isTerminalOutcome(outcome)) ++summary.terminal;
		else ++summary.failed;
		if (isProvisioned(outcome))
		{
			++provisioned;
		}

		if (pass.takeCmRecoveryRequest() && appIt != added.end())
		{
			std::vector<uint32_t> remaining(appIt, added.end());
			g_pLog->info("AppInfoProvision: provider reachable; retrying native CM "
			             "for %zu remaining app(s)\n", remaining.size());
			const auto recovery = coordinator.network([&] {
				return CmClient::fetchProductInfoDetailed(
				    remaining, context.cmBuffers, &context.cmChanges);
			});
			if (recovery == CmClient::FetchResult::Success)
			{
				g_pLog->info("AppInfoProvision: native CM recovered for remaining apps\n");
			}
			else if (recovery == CmClient::FetchResult::NetworkUnavailable)
			{
				pass.noteFinalProviderFailure(NetworkFailure::Connectivity);
				g_pLog->info("AppInfoProvision: native CM recovery lost connectivity; "
				             "using validated local buffers\n");
			}
			else
			{
				g_pLog->info("AppInfoProvision: native CM recovery failed; "
				             "provider fallback remains active\n");
			}
		}

		const ProvisionNotice notice = noticeForOutcome(outcome);
		if (notice == ProvisionNotice::MetadataUnavailable)
		{
			if (pass.takeConnectivityNotice())
				g_pLog->notifyUser(UserMsg::GameMetadataUnavailable,
				                   std::to_string(appId));
		}
		else if (notice == ProvisionNotice::ReviewGameData)
		{

			if (pass.takePreparationNotice(appId))
				g_pLog->notifyUser(UserMsg::GamePreparationFailed,
				                   std::to_string(appId));
		}
		else if (notice == ProvisionNotice::LocalStorage)
		{
			g_pLog->notifyUser(UserMsg::LocalStorageError);
		}
	}

	if (const std::size_t suppressed = pass.pendingPreparationSummary())
	{
		g_pLog->notifyUser(UserMsg::GamePreparationFailedBatch,
		                   std::to_string(pass.preparationFailureCount()));
		g_pLog->info(
		    "AppInfoProvision: %zu app(s) without installable content "
		    "(first=%u); %zu notification(s) aggregated\n",
		    pass.preparationFailureCount(), pass.firstPreparationFailure(),
		    suppressed);
	}
	if (provisioned > 0 && summary.terminal == 0)
	{
		g_pLog->info("AppInfoProvision: %d/%zu AdditionalApps provisioned\n",
		             provisioned, added.size());
	}

	context.cmBuffers.clear();
	context.cmChanges.clear();
	g_pLog->info(
		"AppInfoProvision: pass origin=%s requested=%zu fetched=%zu updated=%zu "
		"ready=%zu terminal=%zu failed=%zu\n",
		origin, summary.requested, summary.fetched, summary.updated,
		summary.ready + summary.fallback, summary.terminal, summary.failed);

	return provisioned;
}

int provisionApps(const std::string& appinfoVdfPath,
                  const std::unordered_set<uint32_t>& added,
                  bool onlyMissing)
{
	if (added.empty()) return 0;

	ProvisionPassCoordinator coordinator(g_provisionPassMu);
	const auto addedSnapshot = coordinator.snapshot([&] { return added; });
	if (addedSnapshot.empty()) return 0;

	ProvisionPassContext context;
	const int provisioned = provisionAppsPass(
		appinfoVdfPath, addedSnapshot, onlyMissing, context, nullptr, "scoped");
	coordinator.commit([&] {
		persistPendingProtonMappings(false);
	});
	return provisioned;
}

int provisionAllAddedApps(const std::string& appinfoVdfPath,
                          bool allowConfigWrite)
{
	ProvisionPassCoordinator coordinator(g_provisionPassMu);
	const auto added = coordinator.snapshot(
		[] { return g_config.managedAppIds.get(); });
	if (added.empty()) return 0;

	ProvisionPassContext context;
	const int provisioned = provisionAppsPass(
		appinfoVdfPath, added, false, context, nullptr, "startup");
	coordinator.commit([&] {
		if (allowConfigWrite)
		{
			if (!injectProtonMappings())
				persistPendingProtonMappings(true);
		}
		else
		{
			persistPendingProtonMappings(false);
		}
	});
	return provisioned;
}

ProvisionPassSummary provisionRequestedApps(
	const std::string& appinfoVdfPath,
	const std::vector<RefreshRequest>& requests,
	bool allowConfigWrite)
{
	ProvisionPassSummary summary;
	if (requests.empty()) return summary;
	ProvisionPassCoordinator coordinator(g_provisionPassMu);
	ProvisionPassContext context;
	std::vector<RefreshRequest> accepted;
	coordinator.snapshot([&] {
		const auto managed = g_config.managedAppIds.get();
		std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
		for (const RefreshRequest& request : requests)
		{
			if (request.appId == 0 || managed.count(request.appId) == 0)
				continue;
			const std::uint64_t generation =
				cachePublicationGenerationLocked(request.appId);
			if (generation != request.managedGeneration) continue;
			context.cachePublications.emplace(request.appId,
				CachePublicationToken{true, generation});
			accepted.push_back(request);
		}
	});
	if (accepted.empty()) return summary;
	summary.requested = accepted.size();

	std::vector<RefreshRequest> fetch;
	std::vector<RefreshRequest> cachedPublish;
	fetch.reserve(accepted.size());
	cachedPublish.reserve(accepted.size());
	std::size_t busy = 0;
	for (const RefreshRequest& request : accepted)
	{
		const CacheProbe probe = probeCache(
			request.appId, CacheProbeMode::BlockingValidate);
		if (probe.readiness == CacheReadiness::Busy) ++busy;
		if (!requestNeedsFetch(request, probe.readiness, probe.changeNumber))
		{
			++summary.ready;
			if (cachedRuntimePublicationAllowed(request, probe.readiness))
				cachedPublish.push_back(request);
			continue;
		}
		if (terminalProvisionResultKnown(
			request.appId, request.minimumChangeNumber, nullptr))
		{
			++summary.terminal;
			continue;
		}
		fetch.push_back(request);
	}
	const auto planOrigin = [](const std::vector<RefreshRequest>& batch) {
		std::uint8_t reasons = 0;
		for (const auto& request : batch) reasons |= request.reasons;
		if (reasons & reasonMask(RefreshReason::DlcMetadata)) return "dlc-metadata";
		if (reasons & reasonMask(RefreshReason::ForceFull)) return "force-full";
		if (reasons & reasonMask(RefreshReason::LocalInputs)) return "local-inputs";
		if (reasons & reasonMask(RefreshReason::HotAdd)) return "hot-add";
		if (reasons & reasonMask(RefreshReason::PicsChanges)) return "pics-changes";
		return "pics-product";
	};
	g_pLog->info(
		"AppInfoProvision: plan origin=%s candidates=%zu fetch=%zu ready=%zu "
		"terminal=%zu busy=%zu\n",
		planOrigin(accepted), accepted.size(), fetch.size(), summary.ready,
		summary.terminal, busy);

	ProvisionPassState pass;
	if (!fetch.empty())
	{
		std::vector<uint32_t> appIds;
		appIds.reserve(fetch.size());
		for (const auto& request : fetch) appIds.push_back(request.appId);
		summary.fetched = appIds.size();
		const auto cmResult = coordinator.network([&] {
			return CmClient::fetchProductInfoDetailed(
				appIds, context.cmBuffers, &context.cmChanges);
		});
		if (cmResult != CmClient::FetchResult::Success)
		{
			context.cmBuffers.clear();
			context.cmChanges.clear();
			pass.noteCmBatchFailure();
			if (cmResult == CmClient::FetchResult::NetworkUnavailable)
				pass.noteFinalProviderFailure(NetworkFailure::Connectivity);
		}
	}

	std::vector<RefreshRequest> publish = std::move(cachedPublish);
	for (const RefreshRequest& request : fetch)
	{
		ProvisionOutcome outcome = ProvisionOutcome::IncompleteContent;
		const auto change = context.cmChanges.find(request.appId);
		const uint32_t observed = change == context.cmChanges.end()
			? request.minimumChangeNumber : change->second;
		if (!terminalProvisionResultKnown(request.appId, observed, &outcome))
			outcome = provisionAppDetailed(
				request.appId, appinfoVdfPath, pass, context,
				request.forceRefresh);
		if (outcome == ProvisionOutcome::Updated) ++summary.updated;
		else if (outcome == ProvisionOutcome::FreshCache) ++summary.ready;
		else if (outcome == ProvisionOutcome::FallbackCache) ++summary.fallback;
		else if (isTerminalOutcome(outcome)) ++summary.terminal;
		else ++summary.failed;
		if (runtimePublicationAllowed(request.publishRuntime, outcome))
			publish.push_back(request);
	}
	for (const RefreshRequest& request : accepted)
	{
		const std::uint8_t nonMetadataReasons = static_cast<std::uint8_t>(
			request.reasons & ~reasonMask(RefreshReason::DlcMetadata));
		if (nonMetadataReasons == 0)
			publish.push_back(request);
	}

	std::vector<RefreshRequest> runtimePublished;
	coordinator.commit([&] {
		if (allowConfigWrite)
		{
			if (!injectProtonMappings()) persistPendingProtonMappings(true);
		}
		else persistPendingProtonMappings(false);
		std::vector<RefreshRequest> basePublish;
		for (const auto& request : publish)
		{
			if ((request.reasons & ~reasonMask(RefreshReason::DlcMetadata)) != 0)
				basePublish.push_back(request);
		}
		if (!basePublish.empty())
		{
			const auto publishedIds =
				publishRuntimeAppInfo(appinfoVdfPath, basePublish);
			const std::unordered_set<std::uint32_t> published(
				publishedIds.begin(), publishedIds.end());
			for (const auto& request : basePublish)
				if (published.count(request.appId) != 0)
					runtimePublished.push_back(request);
		}
	});
	for (const auto& request : runtimePublished)
	{
		if (requiresProton(request.appId))
		{
			(void)OwnerWork::submitCompatReadiness(
				request.appId, request.managedGeneration);
		}
		else
		{
			(void)HotReload::publishPreparedBase(
				request.appId, request.managedGeneration);
		}
	}
	const bool metadataOnly = std::any_of(
		publish.begin(), publish.end(), [](const RefreshRequest& request) {
			return (request.reasons & reasonMask(RefreshReason::DlcMetadata)) != 0;
		});
	if (!runtimePublished.empty() || metadataOnly)
		enrichAndPublishDlcMetadata(appinfoVdfPath, publish);
	g_pLog->info(
		"AppInfoProvision: pass origin=targeted requested=%zu fetched=%zu "
		"updated=%zu ready=%zu terminal=%zu failed=%zu\n",
		summary.requested, summary.fetched, summary.updated,
		summary.ready + summary.fallback, summary.terminal, summary.failed);
	return summary;
}

int provisionColdStartApps(const std::string& appinfoVdfPath,
                           const std::unordered_set<uint32_t>& candidates,
                           ColdStartMode mode,
                           std::unordered_set<uint32_t>* sanitizedApps,
                           bool allowConfigWrite)
{
	if (sanitizedApps) sanitizedApps->clear();
	std::unordered_set<uint32_t> fallbackApps;

	ProvisionPassCoordinator coordinator(g_provisionPassMu);
	const auto managedApps = coordinator.snapshot([&] {
		const auto managed = g_config.managedAppIds.get();
		std::unordered_set<uint32_t> scoped;
		for (const uint32_t appId : candidates)
			if (appId != 0 && managed.count(appId) != 0) scoped.insert(appId);
		return scoped;
	});

	if (coldStartPrimesTerminalMemo(mode))
	{
		const auto archivedGids = ManifestStore::archivedGidIndex();
		for (const uint32_t appId : managedApps)
		{
			const std::string fingerprint =
				localContentFingerprint(appId, archivedGids);
			primeTerminalMemo(appId, fingerprint);
		}
	}

	std::vector<std::pair<uint32_t, CacheReadiness>> probed;
	probed.reserve(managedApps.size());
	std::size_t skippedTerminal = 0;
	for (const uint32_t appId : managedApps)
	{
		const CacheProbeMode probeMode = mode == ColdStartMode::StartupRequireUsablePair
			? CacheProbeMode::BlockingValidate
			: CacheProbeMode::NonBlockingMemoOnly;
		const CacheProbe cacheProbe = probeCache(appId, probeMode);

		const bool terminalKnown =
			observeTerminal(appId, cacheProbe.changeNumber).applies;
		if (terminalKnown)
		{
			++skippedTerminal;
			noteColdRetryOutcome(appId, false);
			continue;
		}
		probed.emplace_back(appId, cacheProbe.readiness);
	}
	std::unordered_set<uint32_t> cold = selectColdStartApps(probed, mode);
	for (auto it = cold.begin(); it != cold.end();)
	{
		if (coldRetryBlocked(*it)) it = cold.erase(it);
		else ++it;
	}
	if (skippedTerminal != 0)
	{
		g_pLog->debug(
		    "AppInfoProvision: skipping %zu app(s) with a known terminal "
		    "content result\n", skippedTerminal);
	}
	if (cold.empty())
	{
		return 0;
	}

	g_pLog->info("AppInfoProvision: cold-start fallback for %zu app(s)\n",
	             cold.size());
	ProvisionPassContext context;
	const int provisioned = provisionAppsPass(
		appinfoVdfPath, cold, true, context, &fallbackApps,
		allowConfigWrite ? "startup-cold" : "runtime-cold");
	std::size_t unresolvedCount = 0;
	for (const uint32_t appId : cold)
	{
		const bool explicitFallback = fallbackApps.count(appId) != 0;
		if (explicitFallback)
		{

			if (sanitizedApps &&
			    shouldMarkColdCacheSanitized(explicitFallback))
				sanitizedApps->insert(appId);
			noteColdRetryOutcome(appId, false);
			continue;
		}
		const CacheProbe completedProbe = probeCache(
			appId, CacheProbeMode::NonBlockingMemoOnly);
		const bool completedReady =
			completedProbe.readiness == CacheReadiness::Fresh ||
			completedProbe.readiness == CacheReadiness::ValidStale;
		if (!completedReady)
		{

			const auto completedTerminal =
				observeTerminal(appId, completedProbe.changeNumber);
			const bool unresolved = !completedTerminal.applies;
			noteColdRetryOutcome(appId, unresolved);
			if (unresolved) ++unresolvedCount;
			continue;
		}
		if (sanitizedApps &&
		    shouldMarkColdCacheSanitized(explicitFallback))
			sanitizedApps->insert(appId);
		noteColdRetryOutcome(appId, false);
	}
	if (unresolvedCount != 0)
	{
		g_pLog->debug(
		    "AppInfoProvision: cold-start fallback incomplete for %zu app(s); "
		    "their subsequent PICS responses are temporarily rate-limited\n",
		    unresolvedCount);
	}

	coordinator.commit([&] {
		if (allowConfigWrite)
		{
			if (!injectProtonMappings())
				persistPendingProtonMappings(true);
		}
		else
		{
			persistPendingProtonMappings(false);
		}
	});
	return provisioned;
}

bool asyncProvisioningEnabled()
{
	return asyncProvisionEnabled(
		g_config.asyncProvision.get(),
		std::getenv("EMBER_ASYNC_PROVISION"));
}

void flushPendingProtonMappings()
{
	std::lock_guard<std::mutex> passLock(g_provisionPassMu);

	for (const std::uint32_t appId : g_config.managedAppIds.get())
	{
		std::string wire;
		if (readValidatedCacheBuffer(appId, wire) &&
			cachedWireRequiresProton(wire))
		{
			g_needProton.insert(appId);
		}
	}
	const auto loaded = loadPendingProtonMappings();
	if (!loaded.lockAcquired || loaded.status == PendingProtonFileStatus::Invalid)
	{

		if (!g_needProton.empty()) (void)injectProtonMappings();
		return;
	}
	if (loaded.status == PendingProtonFileStatus::Missing)
	{
		if (!g_needProton.empty()) (void)injectProtonMappings();
		return;
	}
	g_needProton.insert(loaded.ids.begin(), loaded.ids.end());
	if (g_needProton.empty())
	{

		(void)clearPendingProtonMappingsIfUnchanged(loaded.fileIds);
		return;
	}
	if (injectProtonMappings())
		(void)clearPendingProtonMappingsIfUnchanged(loaded.fileIds);
}

enum class RefreshWorkerStartResult
{
	Started,
	NotStarted,
	Uncertain,
};

std::vector<std::uint32_t> publishRuntimeAppInfo(
    const std::string& appinfoVdfPath,
    const std::vector<RefreshRequest>& requested)
{
	std::unordered_map<std::uint32_t, std::uint64_t> generations;
	{
		std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
		for (const RefreshRequest& request : requested)
			generations[request.appId] =
				cachePublicationGenerationLocked(request.appId);
	}
	const auto selected = selectRuntimePublishCandidates(
		requested, g_config.managedAppIds.get(), SynthMark::loadAll(getCacheDir()),
		generations);
	if (selected.empty())
		return {};

	const std::unordered_set<std::uint32_t> scoped(
		selected.begin(), selected.end());

	const auto managed = g_config.managedAppIds.get();
	AppInfoState::publishAuthoritative(locallyAuthoritativeApps(
		managed, g_config.addedAppIds.get()));

	if (AppInfoVdf::injectCachedApps(appinfoVdfPath, scoped) == 0)
	{
		g_pLog->warn(
			"AppInfoProvision: live appinfo splice failed for %zu app(s); "
			"restart remains available\n",
			selected.size());
		return {};
	}

	const AppInfoReload::Result reloaded =
		AppInfoState::reloadFromDisk(selected);
	switch (reloaded.status)
	{
		case AppInfoReload::Status::Unavailable:
			g_pLog->warn(
				"AppInfoProvision: live appinfo disk reload unavailable for %zu "
				"app(s); restart remains available\n",
				selected.size());
			return {};
		case AppInfoReload::Status::ReadFailed:
			g_pLog->warn(
				"AppInfoProvision: Steam rejected live appinfo disk reload for %zu "
				"app(s); restart remains available\n",
				selected.size());
			return {};
		case AppInfoReload::Status::Loaded:
			if (!AppInfoReload::allRequestedPresent(reloaded, selected.size()))
			{
				g_pLog->warn(
					"AppInfoProvision: live appinfo reload found %zu/%zu current "
					"app(s); restart remains available\n",
					reloaded.present, selected.size());
				return {};
			}
			else
			{
				g_pLog->info(
					"AppInfoProvision: published %zu app(s) into the live "
					"appinfo cache\n",
					reloaded.present);
			}
			return selected;
	}
	return {};
}

struct DlcMetadataWork
{
	DlcMetadata::CacheRecord record;
	std::uint64_t expectedGeneration = 0;
	std::unordered_set<std::uint32_t> candidates;
	bool cacheReady = false;
	bool fetchComplete = false;
	bool publishLive = false;
};

struct DlcMetadataLiveCommitGuard
{
	std::vector<const DlcMetadataWork*> work;
	std::unique_lock<std::mutex> publicationLock;
	std::optional<ProcessLock::FileLock> cacheLock;
};

bool acquireDlcMetadataLiveCommitGuard(void* opaque) noexcept
{
	try
	{
		auto& guard = *static_cast<DlcMetadataLiveCommitGuard*>(opaque);
		guard.publicationLock = std::unique_lock<std::mutex>(g_cachePublicationMu);
		guard.cacheLock.emplace(cacheLockPath(), false);

		if (guard.cacheLock->heldByAnother()) return false;
		const auto managed = g_config.managedAppIds.get();
		for (const DlcMetadataWork* item : guard.work)
		{
			if (item == nullptr ||
				managed.count(item->record.baseAppId) == 0 ||
				!dlcMetadataPublicationMatchesBaseLocked(
					item->record, item->expectedGeneration)) return false;
		}
		return !guard.work.empty();
	}
	catch (...)
	{
		return false;
	}
}

std::vector<DlcMetadataWork> discoverDlcMetadataWork(
	const std::vector<RefreshRequest>& publishedBases)
{
	std::vector<DlcMetadataWork> work;
	work.reserve(publishedBases.size());
	for (const RefreshRequest& request : publishedBases)
	{
		std::string baseWire;
		if (!readValidatedCacheBuffer(request.appId, baseWire)) continue;
		const auto candidates = selectDlcMetadataCandidates(
			request.appId,
			extractDlcAppIdsBySource(baseWire, request.appId));

		DlcMetadataWork item;
		item.publishLive = dlcMetadataPublishesLive(request);
		item.expectedGeneration = request.managedGeneration;
		item.record.baseAppId = request.appId;
		item.record.baseGeneration = request.managedGeneration;
		std::uint8_t baseDigest[20]{};
		sha1BytesInternal(baseWire.data(), baseWire.size(), baseDigest);
		item.record.baseSha.assign(
			reinterpret_cast<const char*>(baseDigest), sizeof(baseDigest));
		const CacheProbe probe = probeCache(
			request.appId, CacheProbeMode::BlockingValidate);
		item.record.baseChangeNumber = probe.changeNumber;
		item.candidates.insert(candidates.begin(), candidates.end());
		DlcMetadata::CacheRecord cached;
		if (readValidatedDlcMetadataCache(
			request.appId, request.managedGeneration, cached))
		{
			item.record = std::move(cached);
			item.cacheReady = true;
			item.fetchComplete = true;
		}
		work.push_back(std::move(item));
	}
	return work;
}

void enrichAndPublishDlcMetadata(
	const std::string& appinfoVdfPath,
	const std::vector<RefreshRequest>& publishedBases)
{
	auto work = discoverDlcMetadataWork(publishedBases);
	if (work.empty()) return;

	std::vector<std::uint32_t> candidates;
	std::unordered_set<std::uint32_t> candidateSeen;
	for (const auto& item : work)
	{
		if (item.cacheReady) continue;
		for (const std::uint32_t appId : item.candidates)
			if (candidateSeen.insert(appId).second) candidates.push_back(appId);
	}
	std::sort(candidates.begin(), candidates.end());
	std::unordered_map<std::uint32_t, std::string> cmBuffers;
	std::unordered_map<std::uint32_t, std::uint32_t> cmChanges;
	if (!candidates.empty())
	{
		g_pLog->info(
			"AppInfoProvision: fetching metadata for %zu DLC candidate(s)\n",
			candidates.size());
		if (CmClient::fetchProductInfoDetailed(
			candidates, cmBuffers, &cmChanges) != CmClient::FetchResult::Success)
		{
			g_pLog->info(
				"AppInfoProvision: DLC metadata fetch unavailable; base publication retained\n");
			return;
		}
	}

	for (auto& item : work)
	{
		if (item.cacheReady) continue;
		item.fetchComplete = std::all_of(
			item.candidates.begin(), item.candidates.end(),
			[&](std::uint32_t candidate) {
				return cmBuffers.count(candidate) != 0;
			});
		if (!item.fetchComplete)
		{
			g_pLog->info(
				"AppInfoProvision: DLC metadata for base=%u was partial; completion remains pending\n",
				item.record.baseAppId);
			continue;
		}
		for (const std::uint32_t candidate : item.candidates)
		{
			const auto buffer = cmBuffers.find(candidate);
			std::string normalized;
			if (buffer == cmBuffers.end() ||
				!DlcMetadata::normalize(
					buffer->second, candidate, item.record.baseAppId, normalized))
			{
				item.record.rejectedAppIds.push_back(candidate);
				continue;
			}
			std::uint8_t digest[20]{};
			sha1BytesInternal(normalized.data(), normalized.size(), digest);
			const auto change = cmChanges.find(candidate);
			item.record.apps.push_back({
				.appid = candidate,
				.changeNumber = change == cmChanges.end() ? 0 : change->second,
				.sha = std::string(
					reinterpret_cast<const char*>(digest), sizeof(digest)),
				.wireBuffer = std::move(normalized),
			});
		}
		std::sort(item.record.rejectedAppIds.begin(),
		          item.record.rejectedAppIds.end());
	}

	struct CompletedDlcMetadata
	{
		std::uint32_t baseAppId = 0;
		std::uint64_t generation = 0;
		bool publishLive = false;
	};
	std::vector<CompletedDlcMetadata> completedBases;
	{
		std::lock_guard<std::mutex> passLock(g_provisionPassMu);
		std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
		const auto managed = g_config.managedAppIds.get();
		ProcessLock::FileLock cacheLock(cacheLockPath(), false);
		if (!cacheLock.acquired()) return;
		for (const auto& item : work)
		{
			if (!item.fetchComplete) continue;
			if (managed.count(item.record.baseAppId) == 0 ||
				!dlcMetadataPublicationMatchesBaseLocked(
					item.record, item.expectedGeneration))
				continue;
			if (!item.cacheReady)
			{
				std::string encoded;
				if (!DlcMetadata::encodeCache(item.record, encoded)) continue;
				std::string error;
				if (!AtomicFile::write(
					getDlcMetadataPath(item.record.baseAppId), encoded, error))
				{
					g_pLog->debug(
						"AppInfoProvision: cannot persist DLC metadata for base=%u: %s\n",
						item.record.baseAppId, error.c_str());
					continue;
				}
			}
			completedBases.push_back({
				item.record.baseAppId, item.expectedGeneration,
				item.publishLive});
		}
	}
	if (completedBases.empty()) return;
	for (const auto& base : completedBases)
	{
		if (!base.publishLive)
			(void)HotReload::noteDlcMetadataCacheCompletion(
				base.baseAppId, base.generation);
	}
	completedBases.erase(
		std::remove_if(
			completedBases.begin(), completedBases.end(),
			[](const auto& base) { return !base.publishLive; }),
		completedBases.end());
	if (completedBases.empty())
	{
		g_pLog->info(
			"AppInfoProvision: cached DLC metadata for cold-start migration; "
			"live Steam state unchanged\n");
		return;
	}

	std::vector<AppInfoVdf::MetadataApp> liveApps;
	std::vector<std::uint32_t> liveIds;
	{

		std::lock_guard<std::mutex> passLock(g_provisionPassMu);
		DlcMetadataLiveCommitGuard guard;
		{
			std::unordered_set<std::uint32_t> completedIds;
			for (const auto& base : completedBases)
				completedIds.insert(base.baseAppId);
			for (const auto& item : work)
			{
				if (completedIds.count(item.record.baseAppId) == 0) continue;
				guard.work.push_back(&item);
				for (const auto& app : item.record.apps)
				{
					liveApps.push_back(
						{app.appid, app.changeNumber, app.sha, app.wireBuffer});
					liveIds.push_back(app.appid);
				}
			}
		}
		const int injected = liveApps.empty() ? 0 :
			AppInfoVdf::injectValidatedMetadataAppsGuarded(
				appinfoVdfPath, liveApps, &guard,
				&acquireDlcMetadataLiveCommitGuard);
		guard.cacheLock.reset();
		if (guard.publicationLock.owns_lock()) guard.publicationLock.unlock();
		if (!liveApps.empty() && injected != static_cast<int>(liveApps.size()))
		{
			g_pLog->warn(
				"AppInfoProvision: live DLC metadata splice failed or its base changed; completion remains pending\n");
			return;
		}
		std::sort(liveIds.begin(), liveIds.end());
		liveIds.erase(std::unique(liveIds.begin(), liveIds.end()), liveIds.end());
		if (!liveIds.empty())
		{
			const auto reloaded = AppInfoState::reloadFromDisk(liveIds);
			if (reloaded.status != AppInfoReload::Status::Loaded ||
				reloaded.present != liveIds.size())
			{
				g_pLog->warn(
					"AppInfoProvision: Steam rejected live DLC metadata reload; completion remains pending\n");
				return;
			}
		}
	}
	std::size_t publishedBasesCount = 0;
	for (const auto& base : completedBases)
	{
		if (HotReload::publishMetadataCompletion(
			base.baseAppId, base.generation))
			++publishedBasesCount;
	}
	if (publishedBasesCount == 0) return;

	bool complete = false;
	DlcInjectionIds allDlc;
	{
		std::lock_guard<std::mutex> passLock(g_provisionPassMu);
		allDlc = collectDlcAppIdsForAddedApps(&complete);
	}
	if (complete) Apps::setDiscoveredAppDlcIds(allDlc.appDlc);
	g_pLog->info(
		"AppInfoProvision: published %zu validated DLC metadata record(s) for %zu base app(s)\n",
		liveApps.size(), publishedBasesCount);
}

void refreshWorkerStartFailed() noexcept
{
	if (g_pLog)
		g_pLog->warn(
		    "AppInfoProvision: unable to start async refresh worker; "
		    "will retry on the next refresh request\n");
}

void finishRefresh(const std::string& completedPath, std::uint64_t token);

RefreshWorkerStartResult startRefreshWorker(
    const std::string& appinfoVdfPath, std::uint64_t token,
    const std::vector<RefreshRequest>& requests)
{
	bool workerMayStillExist = false;
	const bool started = ThreadStart::startDetached(
		[path = appinfoVdfPath, token, requests]
		{
			ThreadStart::runGuarded(
				[path, requests]
				{
					ScopedNotifySuppression suppression;
					BootProf::Span profile(g_pLog.get(), "provision.async");
					(void)provisionRequestedApps(path, requests, false);
				},
				[]
				{
					if (g_pLog)
						g_pLog->warn(
						    "AppInfoProvision: async refresh worker failed; will retry\n");
				},
				[path, token] { finishRefresh(path, token); });
		},
		[] { refreshWorkerStartFailed(); },
		ThreadStart::NoopDetachFailure{},
		ThreadStart::StdThreadDetacher{},
		ThreadStart::StdThreadJoiner{},
		[&workerMayStillExist] {
			workerMayStillExist = true;
			refreshWorkerStartFailed();
		});
	if (started)
		return RefreshWorkerStartResult::Started;
	if (workerMayStillExist)
		return RefreshWorkerStartResult::Uncertain;

	return RefreshWorkerStartResult::NotStarted;
}

void retainRefreshAfterFailedStart(std::uint64_t token,
                                   RefreshWorkerStartResult result,
                                   const std::vector<RefreshRequest>& requests)
{
	if (result != RefreshWorkerStartResult::NotStarted ||
	    !shouldRequeueRefreshAfterStartFailure(false))
		return;

	std::lock_guard<std::mutex> lock(g_refreshScheduleMu);
	if (g_refreshWorkerToken != token) return;
	g_refreshQueue.restoreAfterStartFailure(requests);
	g_refreshActivePath.clear();
}

void finishRefresh(const std::string& completedPath, std::uint64_t token)
{
	std::string nextPath;
	std::vector<RefreshRequest> nextRequests;
	std::uint64_t nextToken = 0;
	{
		std::lock_guard<std::mutex> lock(g_refreshScheduleMu);
		if (g_refreshWorkerToken != token || !g_refreshQueue.active()) return;
		nextRequests = g_refreshQueue.finishActive();
		if (nextRequests.empty())
		{
			g_refreshActivePath.clear();
			return;
		}
		nextPath = g_refreshActivePath.empty() ? completedPath : g_refreshActivePath;
		nextToken = ++g_refreshWorkerToken;
	}

	const auto startResult = startRefreshWorker(nextPath, nextToken, nextRequests);
	retainRefreshAfterFailedStart(nextToken, startResult, nextRequests);
}

void refreshInBackground(
	const std::string& appinfoVdfPath,
	const std::vector<RefreshRequest>& requests)
{
	if (requests.empty() || !asyncProvisioningEnabled()) return;
	std::vector<RefreshRequest> batch;
	std::uint64_t token = 0;
	{
		std::lock_guard<std::mutex> lock(g_refreshScheduleMu);
		const RefreshQueueDecision decision = g_refreshQueue.enqueue(requests);
		if (decision.action != RefreshQueueAction::Start) return;
		batch = decision.batch;
		g_refreshActivePath = appinfoVdfPath;
		token = ++g_refreshWorkerToken;
	}

	const auto startResult = startRefreshWorker(appinfoVdfPath, token, batch);
	retainRefreshAfterFailedStart(token, startResult, batch);
}

DlcInjectionIds collectDlcAppIdsForAddedApps(bool* complete)
{
	if (complete) *complete = false;
	DlcAppIds sources;
	std::unordered_set<uint32_t> taggedSeen;
	std::unordered_set<uint32_t> advertisedSeen;
	std::unordered_set<uint32_t> advertisedWithContent;

	const auto added = g_config.managedAppIds.get();
	if (added.empty())
	{
		if (complete) *complete = true;
		return {};
	}

	std::unordered_set<uint32_t> terminalApps;
	const auto archivedGids = ManifestStore::archivedGidIndex();
	for (const uint32_t appId : added)
	{
		const std::string fingerprint =
			localContentFingerprint(appId, archivedGids);
		primeTerminalMemo(appId, fingerprint);
		if (observeTerminal(appId, 0).applies)
			terminalApps.insert(appId);
	}

	ProcessLock::FileLock cacheLock(cacheLockPath(), false);
	if (!cacheLock.acquired())
	{
		if (g_pLog)
			g_pLog->debug(
			    "AppInfoProvision: DLC snapshot cache lock busy\n");
		return {};
	}

	const auto manifestDepotIds = collectValidManifestDepotIds();

	for (uint32_t appId : added)
	{
		if (terminalApps.count(appId) != 0) continue;

		std::string validationDiag;
		std::string wire;
		if (!readValidatedCacheBufferLocked(appId, wire, validationDiag))
		{

			if (g_pLog)
				g_pLog->info(
				    "AppInfoProvision: DLC snapshot rejected cache pair for app=%u "
				    "(%s); DLC injection stays on its previous set this pass\n",
				    appId, validationDiag.c_str());
			return {};
		}

		const auto grouped = extractDlcAppIdsBySource(wire, appId);
		const bool baseHasDlcDepots = hasDepotsInDlc(wire);
		for (uint32_t dlcId : grouped.depotTagged)
		{

			if (!added.count(dlcId))
			{
				appendUnique(sources.depotTagged, taggedSeen, dlcId);
			}
		}
		for (uint32_t dlcId : grouped.advertised)
		{
			if (added.count(dlcId)) continue;
			appendUnique(sources.advertised, advertisedSeen, dlcId);

			if (baseHasDlcDepots || manifestDepotIds.count(dlcId) != 0)
			{
				advertisedWithContent.insert(dlcId);
			}
		}
	}

	const auto selected = selectDlcInjectionIds(
		sources, advertisedWithContent, g_config.injectAllAdvertisedDlc.get());
	if (!selected.appDlc.empty())
	{

		constexpr std::size_t kMaxLogged = 24;
		std::string ids;
		for (std::size_t i = 0; i < selected.appDlc.size() && i < kMaxLogged; ++i)
		{
			if (i) ids += ' ';
			ids += std::to_string(selected.appDlc[i]);
		}
		if (selected.appDlc.size() > kMaxLogged)
		{
			ids += " ... (+" +
				std::to_string(selected.appDlc.size() - kMaxLogged) + ")";
		}
		g_pLog->info(
			"AppInfoProvision: collected %zu DLC appid(s) from %zu AdditionalApps: "
			"package0=%zu local=%zu: %s\n",
			selected.appDlc.size(), added.size(), selected.package0.size(),
			selected.appDlc.size(), ids.c_str());
	}
	if (complete) *complete = true;
	return selected;
}

bool forgetAppImpl(uint32_t appId, bool preserveTicketArtifacts)
{
	if (appId == 0) return false;

	{
		std::lock_guard<std::mutex> passLock(g_provisionPassMu);
		std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
		++g_cachePublicationGenerations[appId];

		g_terminalProvisionResults.erase(appId);
		{
			std::lock_guard<std::mutex> invalidationLock(
			    g_cacheReadInvalidationMu);
			g_cacheReadInvalidated.insert(appId);
		}
		g_needProton.erase(appId);
		g_pendingProtonRemovals.insert(appId);
	}
	{
		std::lock_guard<std::mutex> validationLock(g_cacheValidationMu);
		g_cacheProbeMemo.erase(appId);
	}

	const auto cacheDir = getCacheDir();
	bool pendingMappingRemoved = false;
	bool complete = false;
	{
		std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
		ProcessLock::FileLock cacheLock(cacheLockPath(), false);
		if (!cacheLock.acquired()) return false;
		ProvisionTerminal::Store(cacheDir).erase(appId);

		const bool preserveSyntheticMarker =
			cache::shouldPreserveSyntheticMarker(
				preserveTicketArtifacts,
				preserveTicketArtifacts && isSynthesizedAppLocked(appId));
		pendingMappingRemoved = removePendingProtonMappingLocked(appId);

		const auto relatedDepots = ManifestId::getExclusiveDepotsForApp(appId);
		const std::string suffix =
			".forgotten." +
			std::to_string(static_cast<long long>(std::time(nullptr))) + "." +
			std::to_string(static_cast<long long>(::getpid()));
		const auto records = SynthMark::quarantineAppArtifacts(
			cacheDir, appId, relatedDepots, suffix,
			!preserveTicketArtifacts,
			!preserveSyntheticMarker);
		for (const auto& record : records)
		{
			g_pLog->infoOnce("AppInfoProvision: quarantined removed-app cache %s -> %s\n",
			                record.original.string().c_str(),
			                record.quarantined.string().c_str());
		}
		complete = !SynthMark::hasAppArtifacts(
			cacheDir, appId, relatedDepots,
			!preserveTicketArtifacts,
			!preserveSyntheticMarker);
		if (complete)
		{

			ManifestId::forgetApp(appId);
		}
		if (!complete)
		{
			g_pLog->warn("AppInfoProvision: app=%u cleanup left one or more cache "
			             "artifacts in place\n", appId);
		}
		g_pLog->infoOnce("AppInfoProvision: forgot app=%u (%zu cache artifact(s), %s)\n",
		             appId, records.size(), complete ? "complete" : "partial");
	}

	if (pendingMappingRemoved)
	{
		std::lock_guard<std::mutex> passLock(g_provisionPassMu);
		g_pendingProtonRemovals.erase(appId);
	}
	return complete;
}

bool forgetApp(uint32_t appId)
{
	return forgetAppImpl(appId, false);
}

bool forgetManagedSourceApp(uint32_t appId)
{
	return forgetAppImpl(appId, true);
}

void clearCacheReadInvalidation(uint32_t appId)
{
	if (appId == 0) return;
	std::lock_guard<std::mutex> invalidationLock(g_cacheReadInvalidationMu);
	g_cacheReadInvalidated.erase(appId);
}

bool cacheMarkerAllowsRead(uint32_t appId)
{
	if (appId == 0) return false;
	std::string metadataText;
	cache::CacheMetadataView metadata;
	return readCacheMetadataFile(appId, metadataText, metadata) &&
		cacheMetadataMarkerAllowsRead(appId, metadata);
}

bool isSynthesizedApp(uint32_t appId)
{
	std::lock_guard<std::mutex> publicationLock(g_cachePublicationMu);
	return isSynthesizedAppLocked(appId);
}

}

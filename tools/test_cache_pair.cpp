

#include "../src/feats/cache_pair.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <unistd.h>

namespace
{

std::string readAll(const std::string& path)
{
	std::ifstream in(path, std::ios::binary);
	return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

void writeFixture(const std::string& path, const std::string& contents)
{
	std::string error;
	assert(AtomicFile::write(path, contents, error));
}

bool backupsPresent(const std::string& bufferPath, const std::string& metaPath)
{
	std::error_code ec;
	return std::filesystem::exists(bufferPath + ".ember-pair-backup", ec) ||
	       std::filesystem::exists(metaPath + ".ember-pair-backup", ec);
}

}

int main()
{
	const std::string root =
		"/tmp/ember-cache-pair." + std::to_string(getpid());
	std::error_code ec;
	std::filesystem::remove_all(root, ec);
	std::filesystem::create_directories(root, ec);
	assert(!ec);

	const auto binPath = root + "/picsbuffer.bin";
	const auto metaPath = root + "/picsbuffer.yaml";
	std::string error;

	writeFixture(binPath, "old-buffer");
	writeFixture(metaPath, "old-metadata");

	bool markerPresent = true;
	const bool markerFailurePublished = CachePair::publish(
		binPath, metaPath, "new-buffer", "new-metadata", false,
		true,
		[&](bool desired) {

			if (!desired) return false;
			markerPresent = desired;
			return true;
		},
		[&] { return markerPresent; }, error);

	assert(!markerFailurePublished);
	assert(readAll(binPath) == "old-buffer");
	assert(readAll(metaPath) == "old-metadata");
	assert(markerPresent);
	assert(!backupsPresent(binPath, metaPath));

	markerPresent = false;
	const bool published = CachePair::publish(
		binPath, metaPath, "fresh-buffer", "fresh-metadata",
		true, false,
		[&](bool desired) { markerPresent = desired; return true; },
		[&] { return markerPresent; }, error);

	assert(published);
	assert(readAll(binPath) == "fresh-buffer");
	assert(readAll(metaPath) == "fresh-metadata");
	assert(markerPresent);
	assert(!backupsPresent(binPath, metaPath));

	const auto unwritableMeta = root + "/absent-dir/picsbuffer.yaml";
	const bool metadataFailurePublished = CachePair::publish(
		binPath, unwritableMeta, "torn-buffer", "torn-metadata",
		true, true,
		[&](bool desired) { markerPresent = desired; return true; },
		[&] { return markerPresent; }, error);

	assert(!metadataFailurePublished);
	assert(readAll(binPath) == "fresh-buffer");
	assert(!error.empty());
	assert(!backupsPresent(binPath, metaPath));

	const auto unstageableMeta = binPath + "/picsbuffer.yaml";
	const bool unstageablePublished = CachePair::publish(
		binPath, unstageableMeta, "ignored-buffer", "ignored-metadata",
		true, true,
		[&](bool desired) { markerPresent = desired; return true; },
		[&] { return markerPresent; }, error);

	assert(!unstageablePublished);
	assert(readAll(binPath) == "fresh-buffer");
	assert(!backupsPresent(binPath, metaPath));

	const auto freshBin = root + "/first.bin";
	const auto freshMeta = root + "/first.yaml";
	markerPresent = false;
	const bool firstPublished = CachePair::publish(
		freshBin, freshMeta, "first-buffer", "first-metadata",
		false, false,
		[&](bool desired) { markerPresent = desired; return true; },
		[&] { return markerPresent; }, error);

	assert(firstPublished);
	assert(readAll(freshBin) == "first-buffer");
	assert(readAll(freshMeta) == "first-metadata");
	assert(!backupsPresent(freshBin, freshMeta));

	const auto blockedFirstMeta = root + "/absent-dir/first.yaml";
	const auto orphanBin = root + "/orphan.bin";
	const bool orphanPublished = CachePair::publish(
		orphanBin, blockedFirstMeta, "orphan-buffer", "orphan-metadata",
		false, false,
		[&](bool desired) { markerPresent = desired; return true; },
		[&] { return markerPresent; }, error);

	assert(!orphanPublished);
	assert(!std::filesystem::exists(orphanBin, ec));

	std::filesystem::remove_all(root, ec);
	return 0;
}

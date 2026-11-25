// SPDX-License-Identifier: AGPL-3.0-only
//
// A re-upload of identical save bytes is a successful PUT to Steam, not a
// rejected one. The regression this covers made Celeste's cloud sync fail with
// HTTP 400 whenever one of its two save files had not changed.

#include "../src/feats/cloudstore.hpp"
#include "../src/feats/sha256.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <unistd.h>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

int main()
{
	const auto root = std::filesystem::temp_directory_path()
		/ ("ember-cloudstore-put-" + std::to_string(::getpid()));
	std::filesystem::remove_all(root);

	const std::string path = "%GameInstall%Saves/0.celeste";
	CloudStore::Entry entry;

	CHECK(CloudStore::put(root, 710463858, 504230, path, "save-v1", entry)
		== CloudStore::PutResult::Stored, "first upload stores the save");
	CHECK(entry.sha == Ember::sha256Hex("save-v1") && entry.size == 7,
	      "first upload reports the stored bytes");

	CloudStore::setChangeNumber(root, 710463858, 504230, 7);
	const auto manifest = root / "710463858/504230/manifest";
	std::string before;
	{
		std::ifstream file(manifest, std::ios::binary);
		before.assign(std::istreambuf_iterator<char>(file), {});
	}

	CloudStore::Entry unchanged;
	CHECK(CloudStore::put(root, 710463858, 504230, path, "save-v1", unchanged)
		== CloudStore::PutResult::Unchanged, "identical re-upload is not an error");
	CHECK(unchanged.sha == entry.sha && unchanged.size == entry.size,
	      "unchanged re-upload still describes the save");
	CHECK(CloudStore::changeNumber(root, 710463858, 504230) == 7,
	      "unchanged re-upload leaves the change number alone");
	{
		std::ifstream file(manifest, std::ios::binary);
		const std::string after(std::istreambuf_iterator<char>(file), {});
		CHECK(after == before, "unchanged re-upload leaves the manifest alone");
	}

	std::string data;
	CHECK(CloudStore::get(root, 710463858, 504230, path, data) && data == "save-v1",
	      "the save is still readable after a no-op re-upload");

	CloudStore::Entry changed;
	CHECK(CloudStore::put(root, 710463858, 504230, path, "save-v2", changed)
		== CloudStore::PutResult::Stored, "changed bytes are stored");
	data.clear();
	CHECK(CloudStore::get(root, 710463858, 504230, path, data) && data == "save-v2",
	      "the new bytes replace the old save");

	CloudStore::Entry rejected;
	CHECK(CloudStore::put(root, 710463858, 504230, "../escape", "x", rejected)
		== CloudStore::PutResult::Error, "an unsafe path is still a real error");

	std::filesystem::remove_all(root);

	std::printf(g_failures == 0 ? "\nALL PASS\n" : "\n%d FAILURE(S)\n", g_failures);
	return g_failures == 0 ? 0 : 1;
}

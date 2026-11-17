// SPDX-License-Identifier: AGPL-3.0-only
//
// Host test for the cloud store layer: SHA-256, the protobuf codec, and the
// on-disk store. No Steam, no config, no logging — every case here is a bug the
// store could plausibly ship with.

#include "../src/feats/cloudstore.hpp"
#include "../src/feats/cloudpb.hpp"
#include "../src/feats/sha256.hpp"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

static int failures = 0;

#define CHECK(cond)                                                          \
	do                                                                     \
	{                                                                      \
		if (!(cond))                                                       \
		{                                                                  \
			std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
			++failures;                                                    \
		}                                                                  \
	} while (0)

namespace
{
	// Scratch root per test group, removed on the way out.
	struct Scratch
	{
		std::filesystem::path root;

		Scratch()
		{
			const char* tmp = std::getenv("TMPDIR");
			root = std::filesystem::path(tmp ? tmp : "/tmp") / "ember-cloud-test";
			std::error_code ec;
			std::filesystem::remove_all(root, ec);
			std::filesystem::create_directories(root, ec);
		}

		~Scratch()
		{
			std::error_code ec;
			std::filesystem::remove_all(root, ec);
		}
	};

	const char* kKnownEmpty = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	const char* kKnownAbc = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
	// The 56-byte boundary case: the message fills the block, so the length has
	// to go into a second one.
	const char* kKnown56 = "b35439a4ac6f0948b6d6f9e3c6af0f5f590ce20f1bde7090ef7970686ec6738a";
	const char* kKnown64 = "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb";
}

static void testSha256()
{
	CHECK(Ember::sha256Hex("") == kKnownEmpty);
	CHECK(Ember::sha256Hex("abc") == kKnownAbc);
	CHECK(Ember::sha256Hex(std::string(56, 'a')) == kKnown56);

	// 64 bytes exactly: one full block, then the padding needs a second.
	CHECK(Ember::sha256Hex(std::string(64, 'a')) == kKnown64);

	// Binary data, including a NUL, must hash correctly.
	{
		const std::string binary("a\0b", 3);
		CHECK(Ember::sha256Hex(binary) ==
			"59b271ae1bbcb1d31d41929817f4b16fb439eb4f31520b5ad1d5ce98920a7138");
	}
}

static void testPbCodec()
{
	using namespace CloudPB;

	// Varint boundaries: 0, 1, 127, 128, 300, and a 64-bit value.
	{
		Writer w;
		w.writeVarint(1, 0);
		w.writeVarint(2, 127);
		w.writeVarint(3, 128);
		w.writeVarint(4, 300);
		w.writeVarint(5, 0xffffffffffffffffull);

		const auto fields = parse(w.bytes(), w.size());
		CHECK(fields.size() == 5);
		CHECK(varint(fields, 1) == 0);
		CHECK(varint(fields, 2) == 127);
		CHECK(varint(fields, 3) == 128);
		CHECK(varint(fields, 4) == 300);
		CHECK(varint(fields, 5) == 0xffffffffffffffffull);
	}

	// Strings and bytes survive a round trip, including embedded NULs.
	{
		Writer w;
		const std::string withNul("a\0b", 3);
		w.writeString(1, "hello");
		w.writeBytes(2, reinterpret_cast<const uint8_t*>(withNul.data()), withNul.size());

		const auto fields = parse(w.bytes(), w.size());
		CHECK(string(fields, 1) == "hello");
		CHECK(bytes(fields, 2) == std::vector<uint8_t>({ 'a', 0, 'b' }));
	}

	// A missing field returns the fallback, not a default-constructed value.
	{
		const auto fields = parse(std::string("\x08\x01", 2));
		CHECK(varint(fields, 99, 42) == 42);
		CHECK(string(fields, 99).empty());
	}

	// Nested messages: the shapes Cloud.BeginAppUploadBatch uses.
	{
		Writer block;
		block.writeString(1, "127.0.0.1:12345");
		block.writeString(2, "/upload/1/2/save.dat");
		block.writeVarint(3, 0);
		block.writeVarint(4, 4);
		block.writeVarint(6, 0);
		block.writeVarint(7, 1024);

		Writer body;
		body.writeVarint(1, 0);
		body.writeMessage(2, block);

		const auto fields = parse(body.bytes(), body.size());
		CHECK(varint(fields, 1) == 0);

		const auto* nested = find(fields, 2);
		CHECK(nested != nullptr);
		CHECK(nested->wireType == LengthDelimited);

		const auto inner = parse(nested->bytes);
		CHECK(string(inner, 1) == "127.0.0.1:12345");
		CHECK(varint(inner, 4) == 4);
		CHECK(varint(inner, 7) == 1024);
	}

	// A truncated buffer must not read past the end.
	{
		// Tag says length-delimited with 10 bytes, only 3 present.
		const std::string truncated("\x0a\x0a\x01\x02\x03", 5);
		const auto fields = parse(truncated);
		CHECK(fields.empty());
	}

	// A malformed varint (continuation bit set on the last byte) must bail.
	{
		const std::string malformed("\x08\xff", 2);
		const auto fields = parse(malformed);
		CHECK(fields.empty());
	}
}

static void testSafePath()
{
	CHECK(CloudStore::isSafePath("save.dat"));
	CHECK(CloudStore::isSafePath("nested/dir/save.dat"));

	// Every one of these would escape or confuse the app directory.
	CHECK(!CloudStore::isSafePath(""));
	CHECK(!CloudStore::isSafePath("/etc/passwd"));
	CHECK(!CloudStore::isSafePath("../escape"));
	CHECK(!CloudStore::isSafePath("nested/../../escape"));
	CHECK(!CloudStore::isSafePath("nested/./save"));
	CHECK(!CloudStore::isSafePath("nested//save"));
	CHECK(!CloudStore::isSafePath("windows\\path"));
	CHECK(!CloudStore::isSafePath(".ember"));
	CHECK(!CloudStore::isSafePath("nested/.ember"));
	CHECK(!CloudStore::isSafePath(std::string("bad\0name", 9)));
	CHECK(!CloudStore::isSafePath(std::string("bad\nname", 9)));
}

static void testStore()
{
	Scratch scratch;

	// A missing app directory is empty, not an error.
	CHECK(CloudStore::list(scratch.root, 76561198u, 1091500).empty());

	// Put, read back, list.
	{
		CloudStore::Entry entry;
		CHECK(CloudStore::put(scratch.root, 1, 100, "save.dat", "hello world", entry));
		CHECK(entry.size == 11);
		CHECK(entry.sha == Ember::sha256Hex("hello world"));
		CHECK(entry.timestamp > 0);

		std::string out;
		CHECK(CloudStore::get(scratch.root, 1, 100, "save.dat", out));
		CHECK(out == "hello world");

		const auto entries = CloudStore::list(scratch.root, 1, 100);
		CHECK(entries.size() == 1);
		CHECK(entries[0].path == "save.dat");
	}

	// Re-putting identical bytes reports "unchanged" and keeps the timestamp.
	{
		std::string out;
		CHECK(CloudStore::get(scratch.root, 1, 100, "save.dat", out));

		const auto before = CloudStore::list(scratch.root, 1, 100);
		CHECK(before.size() == 1);

		CloudStore::Entry entry;
		CHECK(!CloudStore::put(scratch.root, 1, 100, "save.dat", "hello world", entry));
		CHECK(entry.sha == Ember::sha256Hex("hello world"));

		const auto after = CloudStore::list(scratch.root, 1, 100);
		CHECK(after.size() == 1);
		CHECK(after[0].timestamp == before[0].timestamp);
	}

	// Changing the content records a new blob, keeps the timestamp.
	{
		const auto before = CloudStore::list(scratch.root, 1, 100);

		CloudStore::Entry entry;
		CHECK(CloudStore::put(scratch.root, 1, 100, "save.dat", "hello world v2", entry));
		CHECK(entry.sha == Ember::sha256Hex("hello world v2"));

		const auto after = CloudStore::list(scratch.root, 1, 100);
		CHECK(after.size() == 1);	// Replaced, not appended.
		CHECK(after[0].timestamp == before[0].timestamp);

		std::string out;
		CHECK(CloudStore::get(scratch.root, 1, 100, "save.dat", out));
		CHECK(out == "hello world v2");
	}

	// Multiple paths, sorted, independent.
	{
		CloudStore::Entry entry;
		CHECK(CloudStore::put(scratch.root, 1, 100, "zeta.dat", "z", entry));
		CHECK(CloudStore::put(scratch.root, 1, 100, "alpha/save.dat", "a", entry));

		const auto entries = CloudStore::list(scratch.root, 1, 100);
		CHECK(entries.size() == 3);
		CHECK(entries[0].path == "alpha/save.dat");	// Sorted, not insertion order.
		CHECK(entries[1].path == "save.dat");
		CHECK(entries[2].path == "zeta.dat");
	}

	// Same path under a different account or app is isolated.
	{
		CloudStore::Entry entry;
		CHECK(CloudStore::put(scratch.root, 1, 200, "save.dat", "other app", entry));
		CHECK(CloudStore::put(scratch.root, 2, 100, "save.dat", "other account", entry));

		std::string out;
		CHECK(CloudStore::get(scratch.root, 1, 100, "save.dat", out));
		CHECK(out == "hello world v2");
		CHECK(CloudStore::get(scratch.root, 1, 200, "save.dat", out));
		CHECK(out == "other app");
		CHECK(CloudStore::get(scratch.root, 2, 100, "save.dat", out));
		CHECK(out == "other account");
	}

	// Unsafe paths are refused by put, not just by isSafePath.
	{
		CloudStore::Entry entry;
		CHECK(!CloudStore::put(scratch.root, 1, 100, "../escape", "x", entry));
		CHECK(!CloudStore::put(scratch.root, 1, 100, "/etc/passwd", "x", entry));
	}

	// Remove drops the manifest entry, not the blob.
	{
		CHECK(CloudStore::remove(scratch.root, 1, 100, "zeta.dat"));
		CHECK(CloudStore::list(scratch.root, 1, 100).size() == 2);
		CHECK(!CloudStore::remove(scratch.root, 1, 100, "zeta.dat"));	// Second time: gone.
	}

	// Change number defaults to 0, then round-trips.
	{
		CHECK(CloudStore::changeNumber(scratch.root, 1, 100) == 0);
		CloudStore::setChangeNumber(scratch.root, 1, 100, 42);
		CHECK(CloudStore::changeNumber(scratch.root, 1, 100) == 42);
	}

	// Total size and app enumeration.
	{
		CHECK(CloudStore::totalSize(scratch.root, 1, 100) > 0);

		const auto apps = CloudStore::listApps(scratch.root, 1);
		CHECK(apps.size() == 2);
		CHECK(apps[0] == 100);
		CHECK(apps[1] == 200);
	}

	// A malformed manifest line is skipped, not fatal.
	{
		const auto path = scratch.root / "1" / "300";
		std::filesystem::create_directories(path);
		{
			std::FILE* f = std::fopen((path / "manifest").string().c_str(), "wb");
			std::fputs("garbage line without tabs\n", f);
			std::fputs("deadbeef\tnotanumber\t1\tbad.dat\n", f);
			std::fclose(f);
		}

		CHECK(CloudStore::list(scratch.root, 1, 300).empty());
	}
}

static void testDefaultRoot()
{
	CHECK(CloudStore::defaultRoot("/home/x/.config/ember").string() == "/home/x/.config/ember/cloud");
}

int main()
{
	testSha256();
	testPbCodec();
	testSafePath();
	testStore();
	testDefaultRoot();

	if (failures != 0)
	{
		std::printf("\n%d check(s) failed\n", failures);
		return 1;
	}

	std::printf("cloud store: all checks passed\n");
	return 0;
}
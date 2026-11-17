// SPDX-License-Identifier: AGPL-3.0-only
//
// Host test for the Cloud.* RPC handlers. Requests are built with the same codec
// the client uses and fed through dispatch(), so the assertions are about wire
// shapes, not about internal state.

#include "../src/feats/cloudrpc.hpp"
#include "../src/feats/cloudpb.hpp"
#include "../src/feats/cloudstore.hpp"

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
	struct Scratch
	{
		std::filesystem::path root;

		Scratch()
		{
			const char* tmp = std::getenv("TMPDIR");
			root = std::filesystem::path(tmp ? tmp : "/tmp") / "ember-cloud-rpc-test";
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

	constexpr uint32_t kAccount = 76561198u;
	constexpr uint32_t kApp = 1091500u;

	// A fixed endpoint keeps the assertions readable; the real one is random.
	Ember::CloudEndpoint endpoint()
	{
		Ember::CloudEndpoint e;
		e.port = 45678;
		e.token = "0123456789abcdef0123456789abcdef";
		return e;
	}

	// Runs one request through dispatch and hands back the parsed response.
	CloudRPC::Result call(const std::string& method, const std::filesystem::path& root,
		const uint32_t account, const CloudPB::Writer& request)
	{
		return CloudRPC::dispatch(method, root, account, endpoint(), request.bytes(), request.size());
	}
}

static void testHandledNames()
{
	CHECK(CloudRPC::isHandled("Cloud.GetAppFileChangelist#1"));
	CHECK(CloudRPC::isHandled("Cloud.ClientFileDownload#1"));
	CHECK(CloudRPC::isHandled("Cloud.ClientBeginFileUpload#1"));

	// Anything not in the set must fall through to Steam.
	CHECK(!CloudRPC::isHandled("Player.GetUserStats#1"));
	CHECK(!CloudRPC::isHandled("Cloud.GetClientEncryptionKey#1"));
	CHECK(!CloudRPC::isHandled(nullptr));
}

static void testChangelistEmpty()
{
	Scratch scratch;

	CloudPB::Writer request;
	request.writeVarint(1, kApp);
	request.writeVarint(2, 0);	// synced_change_number

	const auto result = call("Cloud.GetAppFileChangelist#1", scratch.root, kAccount, request);

	CHECK(result.handled);
	CHECK(result.eresult == 1);

	const auto fields = CloudPB::parse(result.body.data(), result.body.size());
	CHECK(CloudPB::varint(fields, 1) == 0);		// current_change_number
	CHECK(CloudPB::varint(fields, 3) == 0);		// is_only_delta: a full list

	// One machine name so the client has something to record.
	CHECK(CloudPB::string(fields, 5) == "ember");
}

static void testChangelistWithFiles()
{
	Scratch scratch;

	CloudStore::Entry entry;
	CHECK(CloudStore::put(scratch.root, kAccount, kApp, "save.dat", "hello", entry));
	CloudStore::Entry nested;
	CHECK(CloudStore::put(scratch.root, kAccount, kApp, "profile/a.bin", "xyz", nested));

	CloudPB::Writer request;
	request.writeVarint(1, kApp);
	request.writeVarint(2, 0);

	const auto result = call("Cloud.GetAppFileChangelist#1", scratch.root, kAccount, request);
	CHECK(result.handled);

	const auto fields = CloudPB::parse(result.body.data(), result.body.size());

	// Two file entries, field 2, in sorted order.
	std::vector<CloudPB::Field> files;
	for (const auto& field : fields)
	{
		if (field.number == 2)
		{
			files.push_back(field);
		}
	}

	CHECK(files.size() == 2);

	const auto first = CloudPB::parse(files[0].bytes);
	const auto second = CloudPB::parse(files[1].bytes);

	CHECK(CloudPB::string(first, 1) == "profile/a.bin");
	CHECK(CloudPB::varint(first, 4) == 3);
	CHECK(CloudPB::varint(first, 6) == 0xffffffffu);	// platforms_to_sync = all
	CHECK(CloudPB::string(second, 1) == "save.dat");
	CHECK(CloudPB::varint(second, 4) == 5);

	// The sha is raw bytes, 32 of them for SHA-256.
	const auto sha = CloudPB::bytes(second, 2);
	CHECK(sha.size() == 32);
}

static void testDownloadKnownFile()
{
	Scratch scratch;

	CloudStore::Entry entry;
	CHECK(CloudStore::put(scratch.root, kAccount, kApp, "save.dat", "hello world", entry));

	CloudPB::Writer request;
	request.writeVarint(1, kApp);
	request.writeString(2, "save.dat");

	const auto result = call("Cloud.ClientFileDownload#1", scratch.root, kAccount, request);
	CHECK(result.handled);
	CHECK(result.eresult == 1);

	const auto fields = CloudPB::parse(result.body.data(), result.body.size());

	// The URL points at our loopback server, not at Steam.
	const auto host = CloudPB::string(fields, 7);
	CHECK(host == "127.0.0.1:45678");
	CHECK(CloudPB::string(fields, 8) == "/ember/cloud/76561198/1091500/0123456789abcdef0123456789abcdef/save.dat");
	CHECK(CloudPB::varint(fields, 9) == 0);		// use_https
	CHECK(CloudPB::varint(fields, 2) == 11);	// file_size
	CHECK(CloudPB::varint(fields, 3) == 11);	// raw_file_size
}

static void testDownloadUnknownFile()
{
	Scratch scratch;

	CloudPB::Writer request;
	request.writeVarint(1, kApp);
	request.writeString(2, "missing.dat");

	const auto result = call("Cloud.ClientFileDownload#1", scratch.root, kAccount, request);
	CHECK(result.handled);

	const auto fields = CloudPB::parse(result.body.data(), result.body.size());

	// No URL: Steam must not fall through to the real server, which would
	// answer Access Denied for an unowned app.
	CHECK(CloudPB::string(fields, 7).empty());
	CHECK(CloudPB::varint(fields, 2) == 0);
}

static void testDownloadUnsafePath()
{
	Scratch scratch;

	CloudPB::Writer request;
	request.writeVarint(1, kApp);
	request.writeString(2, "../../etc/passwd");

	const auto result = call("Cloud.ClientFileDownload#1", scratch.root, kAccount, request);
	CHECK(!result.handled);	// Not ours to answer.
	CHECK(result.eresult == 2);
}

static void testBatchRoundTrip()
{
	Scratch scratch;

	// BeginAppUploadBatch assigns the change number.
	{
		CloudPB::Writer request;
		request.writeVarint(1, kApp);
		request.writeString(2, "machine");
		request.writeString(3, "save.dat");

		const auto result = call("Cloud.BeginAppUploadBatch#1", scratch.root, kAccount, request);
		CHECK(result.handled);
		CHECK(result.eresult == 1);

		const auto fields = CloudPB::parse(result.body.data(), result.body.size());
		CHECK(CloudPB::varint(fields, 1) == 1);	// batch_id
		CHECK(CloudPB::varint(fields, 4) == 1);	// app_change_number
	}

	// An upload cannot start without an open batch.
	{
		CloudPB::Writer request;
		request.writeVarint(1, kApp);
		request.writeVarint(2, 1024);
		request.writeString(6, "save.dat");

		const auto result = call("Cloud.ClientBeginFileUpload#1", scratch.root, kAccount, request);
		CHECK(result.eresult == 2);	// No upload_batch_id.
	}

	// With a batch id, the client is told where to PUT.
	{
		CloudPB::Writer request;
		request.writeVarint(1, kApp);
		request.writeVarint(2, 5);
		request.writeVarint(3, 5);
		request.writeString(6, "save.dat");
		request.writeVarint(13, 1);	// upload_batch_id

		const auto result = call("Cloud.ClientBeginFileUpload#1", scratch.root, kAccount, request);
		CHECK(result.handled);
		CHECK(result.eresult == 1);

		const auto fields = CloudPB::parse(result.body.data(), result.body.size());
		CHECK(CloudPB::varint(fields, 1) == 0);	// encrypt_file

		const auto* block = CloudPB::find(fields, 2);
		CHECK(block != nullptr);

		const auto inner = CloudPB::parse(block->bytes);
		CHECK(CloudPB::string(inner, 1) == "127.0.0.1:45678");
		CHECK(CloudPB::string(inner, 2) == "/ember/cloud/76561198/1091500/0123456789abcdef0123456789abcdef/save.dat");
		CHECK(CloudPB::varint(inner, 4) == 4);	// PUT
		CHECK(CloudPB::varint(inner, 6) == 0);	// block_offset
		CHECK(CloudPB::varint(inner, 7) == 5);	// block_length
	}

	// CompleteAppUploadBatch publishes the change number.
	{
		CloudPB::Writer request;
		request.writeVarint(1, kApp);
		request.writeVarint(2, 1);	// batch_id
		request.writeVarint(3, 1);	// batch_eresult

		const auto result = call("Cloud.CompleteAppUploadBatchBlocking#1", scratch.root, kAccount, request);
		CHECK(result.handled);

		CHECK(CloudStore::changeNumber(scratch.root, kAccount, kApp) == 1);
	}
}

static void testCommitAndDelete()
{
	Scratch scratch;

	CloudStore::Entry entry;
	CHECK(CloudStore::put(scratch.root, kAccount, kApp, "save.dat", "hello", entry));

	// Commit with success reports committed.
	{
		CloudPB::Writer request;
		request.writeVarint(1, 1);		// transfer_succeeded
		request.writeVarint(2, kApp);	// appid
		request.writeString(4, "save.dat");

		const auto result = call("Cloud.ClientCommitFileUpload#1", scratch.root, kAccount, request);
		CHECK(result.handled);

		const auto fields = CloudPB::parse(result.body.data(), result.body.size());
		CHECK(CloudPB::varint(fields, 1) == 1);
	}

	// Commit with failure does not.
	{
		CloudPB::Writer request;
		request.writeVarint(1, 0);
		request.writeVarint(2, kApp);
		request.writeString(4, "save.dat");

		const auto result = call("Cloud.ClientCommitFileUpload#1", scratch.root, kAccount, request);
		const auto fields = CloudPB::parse(result.body.data(), result.body.size());
		CHECK(CloudPB::varint(fields, 1) == 0);
	}

	// Delete drops it from the manifest.
	{
		CloudPB::Writer request;
		request.writeVarint(1, kApp);
		request.writeString(2, "save.dat");

		const auto result = call("Cloud.ClientDeleteFile#1", scratch.root, kAccount, request);
		CHECK(result.handled);
		CHECK(CloudStore::list(scratch.root, kAccount, kApp).empty());
	}
}

static void testQuota()
{
	Scratch scratch;

	CloudStore::Entry entry;
	CHECK(CloudStore::put(scratch.root, kAccount, kApp, "save.dat", "12345", entry));

	CloudPB::Writer request;
	request.writeVarint(1, kApp);

	const auto result = call("Cloud.ClientGetAppQuotaUsage#1", scratch.root, kAccount, request);
	CHECK(result.handled);

	const auto fields = CloudPB::parse(result.body.data(), result.body.size());
	CHECK(CloudPB::varint(fields, 1) == 1);		// existing_files
	CHECK(CloudPB::varint(fields, 2) == 5);		// existing_bytes
	CHECK(CloudPB::varint(fields, 3) == 10000);	// max_num_files
	CHECK(CloudPB::varint(fields, 4) == 1073741824ull);
}

static void testLaunchIntent()
{
	Scratch scratch;

	CloudPB::Writer request;
	request.writeVarint(1, kApp);
	request.writeVarint(2, 12345);	// client_id
	request.writeString(3, "machine");

	const auto result = call("Cloud.SignalAppLaunchIntent#1", scratch.root, kAccount, request);
	CHECK(result.handled);
	CHECK(result.body.empty());	// No pending remote operations.
}

static void testUnhandledFallsThrough()
{
	Scratch scratch;

	CloudPB::Writer request;
	request.writeVarint(1, 1);

	const auto result = CloudRPC::dispatch("Cloud.GetClientEncryptionKey#1", scratch.root, kAccount,
		endpoint(), request.bytes(), request.size());

	CHECK(!result.handled);
}

int main()
{
	testHandledNames();
	testChangelistEmpty();
	testChangelistWithFiles();
	testDownloadKnownFile();
	testDownloadUnknownFile();
	testDownloadUnsafePath();
	testBatchRoundTrip();
	testCommitAndDelete();
	testQuota();
	testLaunchIntent();
	testUnhandledFallsThrough();

	if (failures != 0)
	{
		std::printf("\n%d check(s) failed\n", failures);
		return 1;
	}

	std::printf("cloud rpc: all checks passed\n");
	return 0;
}
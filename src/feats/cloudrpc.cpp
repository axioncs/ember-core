// SPDX-License-Identifier: AGPL-3.0-only

#include "cloudrpc.hpp"

#include <algorithm>
#include <ctime>

namespace
{
	// Steam's EResult values used by the cloud layer.
	constexpr uint32_t kFail = 2;

	// Cloud messages encode file_size / raw_file_size as uint32. Steam truncates
	// rather than rejecting, so clamp and carry on instead of refusing a save.
	uint32_t clampSize(const uint64_t value)
	{
		if (value > 0xffffffffull)
		{
			return 0xffffffffu;
		}

		return static_cast<uint32_t>(value);
	}

	// CCloud_ClientCommitFileUpload_Request: transfer_succeeded = 1, appid = 2,
	// file_sha = 3, filename = 4.
	constexpr uint32_t kCommitAppIdField = 2;

	// CCloud_BeginAppUploadBatch_Response: batch_id = 1, app_change_number = 4.
	constexpr uint32_t kBatchIdField = 1;
	constexpr uint32_t kBatchChangeField = 4;

	constexpr const char* kRPCChangelist = "Cloud.GetAppFileChangelist#1";
	constexpr const char* kRPCFileDownload = "Cloud.ClientFileDownload#1";
	constexpr const char* kRPCBeginBatch = "Cloud.BeginAppUploadBatch#1";
	constexpr const char* kRPCCompleteBatch = "Cloud.CompleteAppUploadBatchBlocking#1";
	constexpr const char* kRPCBeginUpload = "Cloud.ClientBeginFileUpload#1";
	constexpr const char* kRPCCommitUpload = "Cloud.ClientCommitFileUpload#1";
	constexpr const char* kRPCDeleteFile = "Cloud.ClientDeleteFile#1";
	constexpr const char* kRPCQuota = "Cloud.ClientGetAppQuotaUsage#1";
	constexpr const char* kRPCLaunchIntent = "Cloud.SignalAppLaunchIntent#1";

	bool isHandledName(const std::string& method)
	{
		return method == kRPCChangelist
			|| method == kRPCFileDownload
			|| method == kRPCBeginBatch
			|| method == kRPCCompleteBatch
			|| method == kRPCBeginUpload
			|| method == kRPCCommitUpload
			|| method == kRPCDeleteFile
			|| method == kRPCQuota
			|| method == kRPCLaunchIntent;
	}

	// URL path segment: everything outside the unreserved set is %XX encoded, so
	// a save named "a b/c.dat" cannot change the shape of the path.
	std::string urlEncode(const std::string& value, const bool keepSlash)
	{
		static const char* hex = "0123456789ABCDEF";
		std::string out;

		for (const unsigned char c : value)
		{
			const bool unreserved = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
				|| (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~';

			if (unreserved || (keepSlash && c == '/'))
			{
				out.push_back(static_cast<char>(c));
				continue;
			}

			out.push_back('%');
			out.push_back(hex[c >> 4]);
			out.push_back(hex[c & 0xf]);
		}

		return out;
	}
}

namespace CloudRPC
{
	uint32_t extractAppId(const std::string& method, const std::vector<CloudPB::Field>& request)
	{
		const uint32_t field = (method == kRPCCommitUpload) ? kCommitAppIdField : 1;
		return static_cast<uint32_t>(CloudPB::varint(request, field));
	}

	bool isHandled(const char* method)
	{
		return method && isHandledName(method);
	}

	Result getAppFileChangelist(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request)
	{
		Result result;
		result.handled = true;

		const uint32_t appId = static_cast<uint32_t>(CloudPB::varint(request, 1));
		if (appId == 0)
		{
			// No app id means we cannot answer; an empty delta with only_delta
			// set keeps Steam from queueing deletes for files it thinks we have.
			CloudPB::Writer body;
			body.writeVarint(1, 0);
			body.writeVarint(3, 1);
			body.writeString(5, "ember");

			result.body = { body.bytes(), body.bytes() + body.size() };
			return result;
		}

		const auto entries = CloudStore::list(root, accountId, appId);
		const uint64_t change = CloudStore::changeNumber(root, accountId, appId);

		CloudPB::Writer body;
		body.writeVarint(1, change);

		for (const auto& entry : entries)
		{
			CloudPB::Writer file;
			file.writeString(1, entry.path);
			file.writeBytes(2, reinterpret_cast<const uint8_t*>(entry.sha.data()), entry.sha.size() / 2);
			file.writeVarint(3, entry.timestamp);
			file.writeVarint(4, clampSize(entry.size));
			file.writeVarint(5, 0);			// persist_state = Persisted
			file.writeVarint(6, 0xffffffffu);	// platforms_to_sync = all
			file.writeVarint(7, 0);			// path_prefix_index
			file.writeVarint(8, 0);			// machine_name_index

			body.writeMessage(2, file);
		}

		// is_only_delta stays false: this is a full list, and Steam reconciles
		// per file against it. A "delta" here would leave Steam holding files we
		// never mentioned.
		body.writeVarint(3, 0);
		body.writeString(5, "ember");
		body.writeVarint(6, 0);

		result.body = { body.bytes(), body.bytes() + body.size() };
		return result;
	}

	Result clientFileDownload(const std::filesystem::path& root, const uint32_t accountId,
		const uint32_t port, const std::vector<CloudPB::Field>& request)
	{
		Result result;
		result.handled = true;

		const uint32_t appId = static_cast<uint32_t>(CloudPB::varint(request, 1));
		const std::string requested = CloudPB::string(request, 2);

		if (appId == 0 || !CloudStore::isSafePath(requested))
		{
			// Not ours: let Steam ask the real server.
			result.handled = false;
			result.eresult = kFail;
			return result;
		}

		const auto entries = CloudStore::list(root, accountId, appId);
		const auto it = std::find_if(entries.begin(), entries.end(), [&requested](const CloudStore::Entry& entry) {
			return entry.path == requested;
		});

		CloudPB::Writer body;
		body.writeVarint(1, appId);

		if (it == entries.end())
		{
			// The client asked for a file we do not have. Report success with a
			// zero size and no URL; Steam treats that as "nothing to fetch"
			// rather than retrying the real server and getting Access Denied.
			body.writeVarint(2, 0);
			body.writeVarint(3, 0);
			body.writeVarint(5, 0);
			body.writeVarint(6, 0);

			result.body = { body.bytes(), body.bytes() + body.size() };
			return result;
		}

		const auto host = "127.0.0.1:" + std::to_string(port);
		const auto path = "/ember/cloud/" + std::to_string(accountId) + "/" + std::to_string(appId)
			+ "/" + urlEncode(it->path, true);

		body.writeVarint(2, clampSize(it->size));
		body.writeVarint(3, clampSize(it->size));
		body.writeBytes(4, reinterpret_cast<const uint8_t*>(it->sha.data()), it->sha.size() / 2);
		body.writeVarint(5, it->timestamp);
		body.writeVarint(6, 0);
		body.writeString(7, host);
		body.writeString(8, path);
		body.writeVarint(9, 0);		// use_https
		body.writeVarint(11, 0);	// encrypted

		result.body = { body.bytes(), body.bytes() + body.size() };
		return result;
	}

	Result beginAppUploadBatch(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request)
	{
		Result result;
		result.handled = true;

		const uint32_t appId = static_cast<uint32_t>(CloudPB::varint(request, 1));
		if (appId == 0 || accountId == 0)
		{
			result.eresult = kFail;
			return result;
		}

		// The change number this batch will publish. Steam records it as synced,
		// so CompleteBatch must report the same one or it re-downloads its own
		// upload.
		const uint64_t change = CloudStore::changeNumber(root, accountId, appId) + 1;

		CloudPB::Writer body;
		body.writeVarint(kBatchIdField, change);	// batch_id doubles as the CN
		body.writeVarint(kBatchChangeField, change);

		result.body = { body.bytes(), body.bytes() + body.size() };
		return result;
	}

	Result completeAppUploadBatch(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request)
	{
		Result result;
		result.handled = true;

		const uint32_t appId = static_cast<uint32_t>(CloudPB::varint(request, 1));
		const uint64_t batchId = CloudPB::varint(request, 2);

		if (appId != 0 && accountId != 0 && batchId != 0)
		{
			CloudStore::setChangeNumber(root, accountId, appId, batchId);
		}

		// Empty response: CCloud_CompleteAppUploadBatch_Response has no fields.
		return result;
	}

	Result clientBeginFileUpload(const std::filesystem::path& root, const uint32_t accountId,
		const uint32_t port, const std::vector<CloudPB::Field>& request)
	{
		Result result;
		result.handled = true;

		const uint32_t appId = static_cast<uint32_t>(CloudPB::varint(request, 1));
		const std::string requested = CloudPB::string(request, 6);
		const uint64_t uploadBatchId = CloudPB::varint(request, 13);

		if (appId == 0 || accountId == 0 || !CloudStore::isSafePath(requested))
		{
			result.handled = false;
			result.eresult = kFail;
			return result;
		}

		// Without an open batch, the client's upload would be orphaned: it has
		// nowhere to commit to.
		if (uploadBatchId == 0)
		{
			result.eresult = kFail;
			return result;
		}

		const auto host = "127.0.0.1:" + std::to_string(port);
		const auto path = "/ember/cloud/" + std::to_string(accountId) + "/" + std::to_string(appId)
			+ "/" + urlEncode(requested, true);

		(void)root;

		const uint64_t size = CloudPB::varint(request, 2);
		const uint64_t rawSize = CloudPB::varint(request, 3);

		CloudPB::Writer block;
		block.writeString(1, host);
		block.writeString(2, path);
		block.writeVarint(3, 0);	// use_https
		block.writeVarint(4, 4);	// http_method = PUT
		block.writeVarint(6, 0);	// block_offset
		block.writeVarint(7, size > 0 ? size : rawSize);

		CloudPB::Writer body;
		body.writeVarint(1, 0);		// encrypt_file
		body.writeMessage(2, block);

		result.body = { body.bytes(), body.bytes() + body.size() };
		return result;
	}

	Result clientCommitFileUpload(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request)
	{
		Result result;
		result.handled = true;

		const bool succeeded = CloudPB::varint(request, 1) != 0;
		const uint32_t appId = static_cast<uint32_t>(CloudPB::varint(request, kCommitAppIdField));
		const std::string requested = CloudPB::string(request, 4);

		(void)root;

		CloudPB::Writer body;

		if (!succeeded || appId == 0 || accountId == 0 || !CloudStore::isSafePath(requested))
		{
			// The PUT failed, so the store must not claim the file exists. The
			// blob may have landed; the manifest is what Steam reads.
			body.writeVarint(1, 0);
			result.body = { body.bytes(), body.bytes() + body.size() };
			return result;
		}

		// The bytes were PUT to the loopback server, which already wrote them
		// through CloudStore::put. The commit only makes the upload official.
		body.writeVarint(1, 1);		// file_committed

		result.body = { body.bytes(), body.bytes() + body.size() };
		return result;
	}

	Result clientDeleteFile(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request)
	{
		Result result;
		result.handled = true;

		const uint32_t appId = static_cast<uint32_t>(CloudPB::varint(request, 1));
		const std::string requested = CloudPB::string(request, 2);

		if (appId != 0 && CloudStore::isSafePath(requested))
		{
			CloudStore::remove(root, accountId, appId, requested);
		}

		// CCloud_ClientDeleteFile_Response has no fields.
		return result;
	}

	Result clientGetAppQuotaUsage(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request)
	{
		Result result;
		result.handled = true;

		const uint32_t appId = static_cast<uint32_t>(CloudPB::varint(request, 1));
		const auto entries = CloudStore::list(root, accountId, appId);

		CloudPB::Writer body;
		body.writeVarint(1, static_cast<uint32_t>(entries.size()));
		body.writeVarint(2, CloudStore::totalSize(root, accountId, appId));
		body.writeVarint(3, 10000);			// max_num_files
		body.writeVarint(4, 1073741824ull);	// max_num_bytes = 1 GiB

		result.body = { body.bytes(), body.bytes() + body.size() };
		return result;
	}

	Result launchIntent(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request)
	{
		Result result;
		result.handled = true;

		const uint32_t appId = static_cast<uint32_t>(CloudPB::varint(request, 1));
		(void)appId;
		(void)root;
		(void)accountId;

		// No pending remote operations: the store is local, so there is nothing
		// for the client to reconcile before launch. An empty repeated field is
		// a valid response.
		return result;
	}

	Result dispatch(const std::string& method, const std::filesystem::path& root,
		const uint32_t accountId, const uint32_t port, const uint8_t* request, const size_t requestLen)
	{
		const std::string raw(reinterpret_cast<const char*>(request), requestLen);
		const auto fields = CloudPB::parse(raw);

		if (method == kRPCChangelist)
		{
			return getAppFileChangelist(root, accountId, fields);
		}
		if (method == kRPCFileDownload)
		{
			return clientFileDownload(root, accountId, port, fields);
		}
		if (method == kRPCBeginBatch)
		{
			return beginAppUploadBatch(root, accountId, fields);
		}
		if (method == kRPCCompleteBatch)
		{
			return completeAppUploadBatch(root, accountId, fields);
		}
		if (method == kRPCBeginUpload)
		{
			return clientBeginFileUpload(root, accountId, port, fields);
		}
		if (method == kRPCCommitUpload)
		{
			return clientCommitFileUpload(root, accountId, fields);
		}
		if (method == kRPCDeleteFile)
		{
			return clientDeleteFile(root, accountId, fields);
		}
		if (method == kRPCQuota)
		{
			return clientGetAppQuotaUsage(root, accountId, fields);
		}
		if (method == kRPCLaunchIntent)
		{
			return launchIntent(root, accountId, fields);
		}

		Result result;
		result.handled = false;
		return result;
	}
}
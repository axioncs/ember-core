// SPDX-License-Identifier: AGPL-3.0-only
//
// Cloud.* RPC handlers. Pure request-bytes -> response-bytes: no Steam, no
// sockets, no logging, so the protocol can be exercised on the host against
// captured requests.
//
// Only the RPCs Steam needs to move save files are implemented. Everything else
// falls through to the real server (see cloudrpc.hpp).

#pragma once

#include "cloudpb.hpp"
#include "cloudstore.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace CloudRPC
{
	// One handler's answer: response bytes plus the eresult to report.
	struct Result
	{
		std::vector<uint8_t> body;
		uint32_t eresult = 1;	// k_EResultOK
		bool handled = false;	// false = caller should call the original
	};

	Result getAppFileChangelist(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result clientFileDownload(const std::filesystem::path& root, const uint32_t accountId,
		const uint32_t port, const std::vector<CloudPB::Field>& request);
	Result beginAppUploadBatch(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result completeAppUploadBatch(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result clientBeginFileUpload(const std::filesystem::path& root, const uint32_t accountId,
		const uint32_t port, const std::vector<CloudPB::Field>& request);
	Result clientCommitFileUpload(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result clientDeleteFile(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result clientGetAppQuotaUsage(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result launchIntent(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);

	// The RPC names this layer answers. Anything else is Steam's business.
	bool isHandled(const char* method);

	// Dispatch by method name. Returns handled=false for anything unknown, which
	// the caller passes to the original.
	Result dispatch(const std::string& method, const std::filesystem::path& root,
		const uint32_t accountId, const uint32_t port, const uint8_t* request, const size_t requestLen);

	// App id out of a request body, for logging and for the namespace check.
	// Field 1 for most Cloud RPCs; ClientCommitFileUpload puts appid in field 2.
	uint32_t extractAppId(const std::string& method, const std::vector<CloudPB::Field>& request);
}
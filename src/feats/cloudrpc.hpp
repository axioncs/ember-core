// SPDX-License-Identifier: AGPL-3.0-only
//
// Cloud.* RPC handlers. Pure request-bytes -> response-bytes: no Steam, no
// sockets, no logging, so the protocol can be exercised on the host against
// requests built with the same codec.
//
// Only the RPCs Steam needs to move save files are implemented. Anything else
// falls through to the real server (see cloudrpc.hpp).

#pragma once

#include "cloudendpoint.hpp"
#include "cloudpb.hpp"
#include "cloudstore.hpp"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace CloudRPC
{
	// One handler's answer: response bytes, the eresult to report, and whether
	// the caller should fall through to the original.
	struct Result
	{
		std::vector<uint8_t> body;
		uint32_t eresult = 1;	// k_EResultOK
		bool handled = false;
	};

	Result getAppFileChangelist(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result clientFileDownload(const std::filesystem::path& root, const uint32_t accountId,
		const Ember::CloudEndpoint& endpoint, const std::vector<CloudPB::Field>& request);
	Result beginAppUploadBatch(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result completeAppUploadBatch(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result clientBeginFileUpload(const std::filesystem::path& root, const uint32_t accountId,
		const Ember::CloudEndpoint& endpoint, const std::vector<CloudPB::Field>& request);
	Result clientCommitFileUpload(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result clientDeleteFile(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result clientGetAppQuotaUsage(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);
	Result launchIntent(const std::filesystem::path& root, const uint32_t accountId,
		const std::vector<CloudPB::Field>& request);

	// True when this layer answers the named method.
	bool isHandled(const char* method);

	// Dispatch by method name. handled=false for anything unknown, which the
	// caller passes to the original.
	Result dispatch(const std::string& method, const std::filesystem::path& root,
		const uint32_t accountId, const Ember::CloudEndpoint& endpoint,
		const uint8_t* request, const size_t requestLen);

	// App id out of a request body, for logging and the namespace check.
	// Field 1 for most Cloud RPCs; ClientCommitFileUpload puts appid in field 2.
	uint32_t extractAppId(const std::string& method, const std::vector<CloudPB::Field>& request);
}
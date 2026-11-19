#pragma once

#include <cstdint>
#include <vector>

namespace CloudTransport
{
	// Called from the websocket send hook, before the frame goes out.
	//
	// `jobId` is the header's jobid_source: the client matches the reply by it,
	// so that is the key the answer is stashed under. Returns true when this
	// process answered the call locally; the request is still forwarded,
	// because the client needs a response frame to attach the answer to.
	bool onSend(uint32_t jobId, const char* method, const uint8_t* body, uint32_t length);

	// Called from the websocket receive hook for a service-method response.
	//
	// `jobId` is the header's jobid_target. Returns true when it names a call
	// answered here: `outBody` then holds the bytes to deliver and `outEResult`
	// the value to report. False means the response must pass through.
	bool onRecv(uint32_t jobId, std::vector<uint8_t>& outBody, uint32_t& outEResult);

	// Drops every stashed answer, so a job id from a previous connection cannot
	// be answered by a stale body.
	void reset();
}
// SPDX-License-Identifier: AGPL-3.0-only
//
// The cloud bridge: ember's answer to Steam's Cloud.* RPCs for added apps.
//
// Steam asks its own servers to move save files. For an app the account does not
// own, that request is refused, so saves never leave the machine and never come
// back. This answers those RPCs from a local store instead, and points the
// client at a loopback HTTP server for the bytes.
//
// Owned apps are untouched: their RPCs go to the original.

#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>

namespace Cloud
{
	// True when this layer answers the named method. Needs no Steam, so the hook
	// can skip work before it knows the app.
	bool isHandled(const char* method);

	// Handles one Cloud.* call on the client's behalf.
	//
	// `send` and `recv` are the protobuf objects the client passed to
	// SendAndRecvMsg. They are serialized and parsed here, so the hook needs
	// neither the protobuf headers nor any knowledge of the message layout.
	//
	// False means the caller must call the original.
	bool handleMessage(const char* method, void* send, void* recv, uint32_t& eresult);

	// App id out of a serialized request. Field 1 for most Cloud RPCs;
	// ClientCommitFileUpload carries it in field 2. Zero when absent.
	uint32_t appIdFor(const char* method, const void* request, const size_t requestLen);

	// Starts the blob server and prepares the store. Call once, after config
	// load. False when cloud is disabled or the port cannot be bound; the caller
	// then runs with Steam's own cloud behaviour.
	bool init();

	// Stops the server. Called on unload.
	void shutdown();

	// True when this process is answering cloud RPCs.
	bool isEnabled();

	// Handles one serialized Cloud.* request. `request` is the wire bytes of the
	// send message; `out` receives the response bytes and `eresult` the value to
	// report.
	//
	// False means the caller must call the original: an app we do not own, a
	// method we do not implement, or a request that does not parse.
	bool handle(const char* method, const std::string& request, std::string& out, uint32_t& eresult);

	// True when the app's cloud RPCs belong to us: the feature is on, the app is
	// added rather than owned, and the account is known.
	bool owns(const uint32_t appId);

	// Where the saves live, for the CLI and for logs.
	std::filesystem::path storeRoot();
}
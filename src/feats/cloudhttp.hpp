// SPDX-License-Identifier: AGPL-3.0-only
//
// Loopback HTTP server for save blobs. Steam fetches and uploads saves through
// the URL ember puts in the cloud RPC responses, so the server is what makes
// "the store is local" actually transfer bytes.
//
// Deliberately minimal: GET and PUT only, one connection at a time, HTTP/1.1
// with Connection: close. Steam's ufs client issues one request per file and
// does not pipeline, so a thread per connection would be cost without benefit.
// Requests are served on a worker thread so a slow client cannot stall the
// client's cloud threads.

#pragma once

#include "cloudendpoint.hpp"
#include "cloudstore.hpp"

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <string>
#include <thread>

namespace CloudHTTP
{
	enum class Method
	{
		Get,
		Put,
		Unsupported
	};

	struct Request
	{
		Method method = Method::Unsupported;
		std::string path;
		size_t contentLength = 0;
	};

	// Parses the request line and headers only. The body is left for the caller,
	// because it may still be in flight on the socket.
	//
	// `raw` must include the terminating blank line. False when the request line
	// is malformed, the method is not GET/PUT, or Content-Length is unusable.
	bool parseHead(const std::string& raw, Request& out, std::string& error);

	// Decodes /ember/cloud/<account>/<app>/<token>/<encoded save path>.
	// False when the token does not match, an id is not numeric, or the save path
	// is unsafe.
	bool decodePath(const Ember::CloudEndpoint& endpoint, const std::string& path,
		uint32_t& accountId, uint32_t& appId, std::string& savePath);

	std::string urlDecode(const std::string& value);

	class Server
	{
	public:
		// Binds 127.0.0.1:0 and writes the chosen port into `endpoint`.
		// False when the port cannot be bound; the caller then runs without
		// cloud rather than aborting the client.
		bool start(const std::filesystem::path& root, const Ember::CloudEndpoint& endpoint);
		void stop();

		const Ember::CloudEndpoint& endpoint() const { return endpoint_; }
		bool running() const { return running_.load(); }

	private:
		void serveLoop();
		void handle(int fd);

		std::filesystem::path root_;
		Ember::CloudEndpoint endpoint_;
		std::atomic_bool running_ = false;
		int listenFd_ = -1;
		std::thread thread_;
	};
}
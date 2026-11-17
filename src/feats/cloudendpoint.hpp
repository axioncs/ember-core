// SPDX-License-Identifier: AGPL-3.0-only
//
// The loopback endpoint Steam is pointed at for save blobs: one port and a
// per-session token.
//
// The port is unauthenticated like every other loopback service, and any local
// process could read or write a save through it. The token is what stops that:
// it is embedded in the URL ember hands to Steam and checked on every request,
// so a process that did not read it out of a cloud RPC cannot use the port.

#pragma once

#include <cstdint>
#include <string>

namespace Ember
{
	struct CloudEndpoint
	{
		uint32_t port = 0;
		std::string token;

		bool valid() const
		{
			return port != 0 && token.size() == 32;
		}

		// "127.0.0.1:41234", the url_host field of a cloud response.
		std::string host() const;

		// The url_path field: /ember/cloud/<account>/<app>/<encoded save path>.
		std::string urlPath(const uint32_t accountId, const uint32_t appId, const std::string& savePath) const;

		// Full URL, for logs and for the CLI.
		std::string url(const uint32_t accountId, const uint32_t appId, const std::string& savePath) const;
	};

	// 32 hex characters from /dev/urandom, falling back to a time-seeded PRNG
	// only if the device is unreadable.
	std::string randomToken();
}
// SPDX-License-Identifier: AGPL-3.0-only

#include "cloudendpoint.hpp"

#include "sha256.hpp"

#include <cstdio>
#include <fstream>
#include <random>

namespace
{
	constexpr size_t kTokenChars = 32;

	std::string hexEncode(const uint8_t* data, const size_t len)
	{
		static const char* hex = "0123456789abcdef";
		std::string out(len * 2, '0');

		for (size_t i = 0; i < len; ++i)
		{
			out[i * 2] = hex[data[i] >> 4];
			out[i * 2 + 1] = hex[data[i] & 0xf];
		}

		return out;
	}
}

namespace Ember
{
	std::string randomToken()
	{
		uint8_t bytes[kTokenChars / 2] = {};

		std::ifstream urandom("/dev/urandom", std::ios::binary);
		if (urandom.is_open())
		{
			urandom.read(reinterpret_cast<char*>(bytes), sizeof(bytes));
		}

		const auto got = urandom.gcount();

		if (got != static_cast<std::streamsize>(sizeof(bytes)))
		{
			// No /dev/urandom: still not guessable by a process that cannot read
			// our memory, but not cryptographically strong either.
			std::random_device device;
			for (auto& byte : bytes)
			{
				byte = static_cast<uint8_t>(device() & 0xff);
			}
		}

		return hexEncode(bytes, sizeof(bytes));
	}

	std::string CloudEndpoint::host() const
	{
		return "127.0.0.1:" + std::to_string(port);
	}

	std::string CloudEndpoint::urlPath(const uint32_t accountId, const uint32_t appId,
		const std::string& savePath) const
	{
		// The token sits in the path, not a query string: Steam copies url_path
		// verbatim into its HTTP request, and a query would have to survive its
		// own URL handling.
		return "/ember/cloud/" + std::to_string(accountId) + "/" + std::to_string(appId)
			+ "/" + token + "/" + savePath;
	}

	std::string CloudEndpoint::url(const uint32_t accountId, const uint32_t appId,
		const std::string& savePath) const
	{
		return "http://" + host() + urlPath(accountId, appId, savePath);
	}
}
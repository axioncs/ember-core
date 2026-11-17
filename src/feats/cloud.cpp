// SPDX-License-Identifier: AGPL-3.0-only

#include "cloud.hpp"

#include "cloudhttp.hpp"
#include "cloudrpc.hpp"
#include "cloudstore.hpp"
#include "../config.hpp"
#include "../globals.hpp"
#include "../log.hpp"
#include "../sdk/sdk.hpp"

#include <google/protobuf/message_lite.h>

#include <memory>
#include <mutex>
#include <string>

namespace
{
	std::unique_ptr<CloudHTTP::Server> g_server;
	std::mutex g_initMutex;
	bool g_tried = false;
}

namespace Cloud
{
	bool isHandled(const char* method)
	{
		return CloudRPC::isHandled(method);
	}

	uint32_t appIdFor(const char* method, const void* request, const size_t requestLen)
	{
		if (!method || request == nullptr)
		{
			return 0;
		}

		const std::string raw(reinterpret_cast<const char*>(request), requestLen);
		const auto fields = CloudPB::parse(raw);

		return CloudRPC::extractAppId(method, fields);
	}

	bool init()
	{
		std::lock_guard<std::mutex> lock(g_initMutex);

		if (g_tried)
		{
			return g_server != nullptr;
		}

		g_tried = true;

		if (!g_config.cloud.copy())
		{
			LOG_INFO("Cloud: disabled by config\n");
			return false;
		}

		const auto root = storeRoot();

		std::error_code ec;
		std::filesystem::create_directories(root, ec);

		auto server = std::make_unique<CloudHTTP::Server>();

		// The token is per-process, so a URL from an earlier session cannot be
		// replayed against this one.
		Ember::CloudEndpoint endpoint;
		endpoint.token = Ember::randomToken();

		if (!server->start(root, endpoint))
		{
			LOG_WARN("Cloud: no loopback port, saves stay on this machine\n");
			return false;
		}

		g_server = std::move(server);

		LOG_INFO("Cloud: answering save RPCs from %s\n", root.c_str());
		return true;
	}

	void shutdown()
	{
		std::lock_guard<std::mutex> lock(g_initMutex);

		if (!g_server)
		{
			return;
		}

		g_server->stop();
		g_server.reset();

		LOG_INFO("Cloud: stopped\n");
	}

	bool isEnabled()
	{
		std::lock_guard<std::mutex> lock(g_initMutex);
		return g_server != nullptr;
	}

	bool owns(const uint32_t appId)
	{
		if (!isEnabled() || appId == 0)
		{
			return false;
		}

		// Owned apps keep Steam's cloud: their saves belong to the account, and
		// answering for them would fork a paying customer's save data.
		if (g_pSteamEngine && g_pSteamEngine->getUser(0)->isSubscribed(appId))
		{
			return false;
		}

		// The account id is only known after login, and a zero id would write
		// saves under <root>/0/<appid>, which no account owns.
		if (!g_currentSteamId.isSet() || g_currentSteamId.accountId() == 0)
		{
			return false;
		}

		return true;
	}

	bool handle(const char* method, const std::string& request, std::string& out, uint32_t& eresult)
	{
		if (!isEnabled() || method == nullptr)
		{
			return false;
		}

		const auto endpoint = g_server->endpoint();

		const auto result = CloudRPC::dispatch
		(
			method,
			storeRoot(),
			g_currentSteamId.accountId(),
			endpoint,
			reinterpret_cast<const uint8_t*>(request.data()),
			request.size()
		);

		if (!result.handled)
		{
			return false;
		}

		out.assign(reinterpret_cast<const char*>(result.body.data()), result.body.size());
		eresult = result.eresult;

		LOG_INFO("Cloud: %s answered here (%zu bytes, eresult %u)\n", method, out.size(), eresult);
		return true;
	}

	bool handleMessage(const char* method, void* send, void* recv, uint32_t& eresult)
	{
		if (!isEnabled() || method == nullptr || send == nullptr || recv == nullptr)
		{
			return false;
		}

		auto* request = reinterpret_cast<google::protobuf::MessageLite*>(send);
		auto* response = reinterpret_cast<google::protobuf::MessageLite*>(recv);

		// The app id lives in the request body, so the bytes come first.
		std::string wire;
		if (!request->SerializeToString(&wire))
		{
			LOG_WARN("Cloud: %s request did not serialize\n", method);
			return false;
		}

		if (!owns(appIdFor(method, wire.data(), wire.size())))
		{
			return false;
		}

		std::string body;
		uint32_t result = 0;

		if (!handle(method, wire, body, result))
		{
			return false;
		}

		if (!body.empty() && !response->ParseFromString(body))
		{
			// The client keeps the object it allocated, so a half-parsed response
			// is worse than an unanswered call.
			LOG_WARN("Cloud: %s response did not parse, falling through\n", method);
			return false;
		}

		eresult = result;
		return true;
	}

	std::filesystem::path storeRoot()
	{
		return CloudStore::defaultRoot(CConfig::getDir());
	}
}
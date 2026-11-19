#include "cloudtransport.hpp"

#include "cloud.hpp"
#include "../log.hpp"

#include <mutex>
#include <unordered_map>

namespace
{
	constexpr size_t kMaxStash = 256;

	struct Pending
	{
		std::string method;
		std::vector<uint8_t> body;
	};

	std::mutex g_mutex;
	std::unordered_map<uint32_t, Pending> g_pending;

	void pruneLocked()
	{
		if (g_pending.size() <= kMaxStash)
		{
			return;
		}

		const auto drop = g_pending.size() - kMaxStash;
		auto it = g_pending.begin();
		for (size_t i = 0; i < drop && it != g_pending.end(); ++i)
		{
			it = g_pending.erase(it);
		}
	}
}

namespace CloudTransport
{
	bool onSend(uint32_t jobId, const char* method, const uint8_t* body, uint32_t length)
	{
		if (jobId == 0 || method == nullptr || body == nullptr || length == 0)
		{
			return false;
		}

		if (!Cloud::isHandled(method))
		{
			return false;
		}

		const auto appId = Cloud::appIdFor(method, body, length);

		if (!Cloud::owns(appId))
		{
			return false;
		}

		const std::string raw(reinterpret_cast<const char*>(body), length);

		std::string reply;
		uint32_t eresult = 0;

		if (!Cloud::handle(method, raw, reply, eresult))
		{
			return false;
		}

		{
			std::lock_guard lock(g_mutex);
			g_pending.erase(jobId);
			g_pending.emplace(jobId,
				Pending{method, std::vector<uint8_t>(reply.begin(), reply.end())});
			pruneLocked();
		}

		g_pLog->info("Cloud: %s app=%u answered here (%zu bytes, eresult %u)\n",
			method, appId, reply.size(), eresult);

		return true;
	}

	bool onRecv(uint32_t jobId, std::vector<uint8_t>& outBody, uint32_t& outEResult)
	{
		Pending pending;

		{
			std::lock_guard lock(g_mutex);
			auto it = g_pending.find(jobId);
			if (it == g_pending.end())
			{
				return false;
			}
			pending = std::move(it->second);
			g_pending.erase(it);
		}

		if (pending.body.empty())
		{
			return false;
		}

		outBody = std::move(pending.body);
		outEResult = 1;

		g_pLog->info("Cloud: %s job=%u answered here (%zu bytes)\n",
			pending.method.c_str(), jobId, outBody.size());

		return true;
	}

	void reset()
	{
		std::lock_guard lock(g_mutex);
		g_pending.clear();
	}
}
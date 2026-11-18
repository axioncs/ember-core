

#pragma once

#include "provision_refresh.hpp"

#include <cstdint>
#include <functional>
#include <unordered_set>
#include <utility>
#include <vector>

class CProtoBufMsgBase;
class CMsgClientPICSProductInfoResponse;
class CMsgClientPICSChangesSinceResponse;

namespace PICS
{
	void recvMsg(CProtoBufMsgBase* msg);
	void recvProductInfoResponse(CMsgClientPICSProductInfoResponse* resp);
	void recvChangesSinceResponse(CMsgClientPICSChangesSinceResponse* resp);

	inline bool legacyManifestStagingEnabled(const char* envValue)
	{
		return envValue && envValue[0] == '1' && envValue[1] == '\0';
	}

	inline std::unordered_set<uint32_t> selectManagedResponseApps(
	    const std::unordered_set<uint32_t>& managed,
	    const std::vector<uint32_t>& responseAppIds)
	{
		std::unordered_set<uint32_t> selected;
		for (const uint32_t appId : responseAppIds)
			if (managed.count(appId) != 0) selected.insert(appId);
		return selected;
	}

	inline bool rawResponseCanRepairCache(
		bool hasBuffer, AppInfoProvision::CacheReadiness readiness) noexcept
	{
		return hasBuffer &&
			(readiness == AppInfoProvision::CacheReadiness::Missing ||
			 readiness == AppInfoProvision::CacheReadiness::Invalid ||
			 readiness == AppInfoProvision::CacheReadiness::Busy ||
			 readiness == AppInfoProvision::CacheReadiness::Unverified);
	}

	inline bool rawCacheItemShouldReplace(
		std::uint64_t pendingGeneration, std::uint32_t pendingChange,
		std::uint64_t incomingGeneration, std::uint32_t incomingChange) noexcept
	{
		if (incomingGeneration != pendingGeneration)
			return incomingGeneration > pendingGeneration;
		return incomingChange >= pendingChange;
	}

	inline bool rawCacheWorkerShouldContinueAfterDeferral(
		bool hadConcurrentPending) noexcept
	{

		return hadConcurrentPending;
	}

	inline std::vector<AppInfoProvision::RefreshRequest>
	excludeRefreshRequestsForApps(
	    const std::vector<AppInfoProvision::RefreshRequest>& requests,
	    const std::unordered_set<uint32_t>& excluded)
	{
		std::vector<AppInfoProvision::RefreshRequest> pending;
		pending.reserve(requests.size());
		for (const auto& request : requests)
			if (excluded.count(request.appId) == 0) pending.push_back(request);
		return pending;
	}

	inline std::vector<AppInfoProvision::RefreshRequest>
	markRuntimePublicationForSuppressedApps(
		const std::vector<AppInfoProvision::RefreshRequest>& requests,
		const std::unordered_set<uint32_t>& suppressed)
	{
		auto tagged = requests;
		for (auto& request : tagged)
			request.publishRuntime = request.publishRuntime ||
				suppressed.count(request.appId) != 0;
		return tagged;
	}

	using DepotGid = std::pair<uint32_t, uint64_t>;

	struct AppDepots
	{
		uint32_t appId;
		std::vector<DepotGid> depots;
	};

	struct StageTarget
	{
		uint32_t appId;
		uint32_t depotId;
		uint64_t gid;
	};

	inline std::vector<StageTarget> buildSyncStagePlan(
	    const std::vector<AppDepots>& apps,
	    const std::function<bool(uint32_t depotId)>& hasKey)
	{
		std::vector<StageTarget> out;
		std::unordered_set<uint64_t> seen;
		for (const auto& app : apps)
		{
			for (const auto& [depotId, gid] : app.depots)
			{
				if (gid == 0) continue;
				if (hasKey && !hasKey(depotId)) continue;
				const uint64_t key =
				    gid ^ (static_cast<uint64_t>(depotId) * 0x9E3779B97F4A7C15ULL);
				if (seen.insert(key).second)
				{
					out.push_back({app.appId, depotId, gid});
				}
			}
		}
		return out;
	}

	inline std::vector<StageTarget> buildPendingStagePlan(
	    const std::vector<StageTarget>& plan,
	    const std::function<bool(const StageTarget&)>& isReady)
	{
		std::vector<StageTarget> pending;
		pending.reserve(plan.size());
		for (const auto& target : plan)
		{
			if (!isReady || !isReady(target))
			{
				pending.push_back(target);
			}
		}
		return pending;
	}
}

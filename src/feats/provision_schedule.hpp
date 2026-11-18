

#pragma once

#include "provision_cache.hpp"
#include "provision_refresh.hpp"

#include <algorithm>
#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace AppInfoProvision
{

inline bool shouldRequeueRefreshAfterStartFailure(bool workerMayStillExist)
{
	return !workerMayStillExist;
}

inline std::vector<std::uint32_t> mergeRuntimePublishCandidates(
	std::vector<std::uint32_t> current,
	const std::vector<std::uint32_t>& incoming)
{
	current.insert(current.end(), incoming.begin(), incoming.end());
	std::sort(current.begin(), current.end());
	current.erase(std::unique(current.begin(), current.end()), current.end());
	current.erase(std::remove(current.begin(), current.end(), 0), current.end());
	return current;
}

inline std::vector<std::uint32_t> selectRuntimePublishCandidates(
	const std::vector<RefreshRequest>& requested,
	const std::unordered_set<std::uint32_t>& managed,
	const std::unordered_set<std::uint32_t>& synthetic,
	const std::unordered_map<std::uint32_t, std::uint64_t>& generations)
{
	std::vector<std::uint32_t> selected;
	selected.reserve(requested.size());
	for (const RefreshRequest& request : requested)
	{
		if (request.appId == 0 || !request.publishRuntime ||
			managed.count(request.appId) == 0)
			continue;
		const auto generation = generations.find(request.appId);
		if (generation == generations.end() ||
			generation->second != request.managedGeneration)
			continue;
		const bool localUpdate =
			(request.reasons & (reasonMask(RefreshReason::HotAdd) |
			                    reasonMask(RefreshReason::LocalInputs))) != 0;
		const bool suppressedSynthetic =
			synthetic.count(request.appId) != 0 &&
			(request.reasons & (reasonMask(RefreshReason::PicsProductInfo) |
			                    reasonMask(RefreshReason::PicsChanges) |
			                    reasonMask(RefreshReason::ForceFull))) != 0;
		if (localUpdate || suppressedSynthetic)
			selected.push_back(request.appId);
	}
	std::sort(selected.begin(), selected.end());
	selected.erase(std::unique(selected.begin(), selected.end()), selected.end());
	return selected;
}

enum class PreinitProvisionAction
{
	SynchronousProvision,
	ColdFallbackThenSplice,
};

inline PreinitProvisionAction preinitProvisionAction(bool asyncEnabled)
{
	return asyncEnabled
		? PreinitProvisionAction::ColdFallbackThenSplice
		: PreinitProvisionAction::SynchronousProvision;
}

inline bool cachePairReady(bool bufferExists, bool metadataExists,
                           bool recordValid)
{
	return bufferExists && metadataExists && recordValid;
}

inline bool coldFallbackNeeded(CacheReadiness readiness)
{
	if (readiness == CacheReadiness::Missing ||
	    readiness == CacheReadiness::Invalid) return true;
	return false;
}

inline bool shouldWarmCurlBeforePics(bool )
{
	return true;
}

inline bool asyncProvisionEnabled(bool configEnabled, const char* envOverride)
{
	if (!envOverride || *envOverride == '\0') return configEnabled;

	const std::string_view value(envOverride);
	if (value == "1" || value == "true" || value == "yes" || value == "on")
		return true;
	if (value == "0" || value == "false" || value == "no" || value == "off")
		return false;
	return false;
}

inline bool shouldMarkColdCacheSanitized(bool explicitFallback)
{
	return !explicitFallback;
}

}

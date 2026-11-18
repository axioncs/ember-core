

#pragma once

#include "hotreload_state.hpp"
#include "provision_refresh.hpp"

#include <cstdint>
#include <unordered_set>
#include <vector>

namespace HotReload
{

	void initialize() noexcept;

	void publish(
		const std::unordered_set<std::uint32_t>& managedAppIds,
		bool forceSourceRefresh = false) noexcept;

	bool publishPreparedBase(
		std::uint32_t baseAppId,
		std::uint64_t expectedManagedGeneration) noexcept;

	bool publishMetadataCompletion(
		std::uint32_t baseAppId,
		std::uint64_t expectedManagedGeneration) noexcept;

	void repairMissingDlcMetadata(
		const std::string& appinfoVdfPath) noexcept;

	std::vector<AppInfoProvision::RefreshRequest>
	takeMissingCacheRepairRequests() noexcept;

	bool noteDlcMetadataCacheCompletion(
		std::uint32_t baseAppId,
		std::uint64_t expectedManagedGeneration) noexcept;

	HotReloadState::Store& store() noexcept;

	void shutdown() noexcept;
}

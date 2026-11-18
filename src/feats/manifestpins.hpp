

#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace ManifestPins
{
	struct AppPins
	{
		bool locked = false;
		uint32_t buildId = 0;
		std::unordered_map<uint32_t, uint64_t> depots;
	};

	using PinMap = std::unordered_map<uint32_t, AppPins>;

	inline std::unordered_map<uint32_t, uint64_t> flattenDepots(const PinMap& pins)
	{
		std::unordered_map<uint32_t, uint64_t> flat;
		std::unordered_set<uint32_t> ambiguous;
		for (const auto& [appId, app] : pins)
		{
			if (!appId) continue;
			for (const auto& [depotId, gid] : app.depots)
			{
				if (!depotId || !gid) continue;
				if (ambiguous.contains(depotId)) continue;

				auto [it, inserted] = flat.emplace(depotId, gid);
				if (!inserted && it->second != gid)
				{
					flat.erase(it);
					ambiguous.insert(depotId);
				}
			}
		}
		return flat;
	}

	inline std::unordered_set<uint32_t> lockedAppSet(const PinMap& pins)
	{
		std::unordered_set<uint32_t> locked;
		for (const auto& [appId, app] : pins)
		{
			if (!appId || !app.locked) continue;
			for (const auto& [depotId, gid] : app.depots)
			{
				if (depotId && gid)
				{
					locked.insert(appId);
					break;
				}
			}
		}
		return locked;
	}

	inline uint64_t getPin(const PinMap& pins, uint32_t appId, uint32_t depotId)
	{
		if (!appId || !depotId) return 0ULL;
		const auto appIt = pins.find(appId);
		if (appIt == pins.end()) return 0ULL;
		const auto depotIt = appIt->second.depots.find(depotId);
		return depotIt == appIt->second.depots.end() ? 0ULL : depotIt->second;
	}

	inline uint64_t getPinForContext(const PinMap& pins, uint32_t appId,
	                                uint32_t depotId)
	{
		if (!appId || !depotId) return 0ULL;
		return getPin(pins, appId, depotId);
	}

	inline uint64_t getPinForUniqueOwner(const PinMap& pins, uint32_t depotId)
	{
		if (!depotId) return 0ULL;
		uint64_t pin = 0ULL;
		bool found = false;
		for (const auto& [appId, app] : pins)
		{
			if (!appId) continue;
			const auto depotIt = app.depots.find(depotId);
			if (depotIt == app.depots.end() || depotIt->second == 0ULL) continue;
			if (found) return 0ULL;
			found = true;
			pin = depotIt->second;
		}
		return found ? pin : 0ULL;
	}

	inline uint64_t getPinForPlannerEntry(const PinMap& pins, uint32_t appId,
	                                      uint32_t depotId)
	{
		return appId ? getPinForContext(pins, appId, depotId)
		             : getPinForUniqueOwner(pins, depotId);
	}

	inline uint64_t getPin(const std::unordered_map<uint32_t, uint64_t>& flat,
	                       uint32_t depotId)
	{
		if (!depotId) return 0ULL;
		const auto it = flat.find(depotId);
		return it == flat.end() ? 0ULL : it->second;
	}

	inline bool isLocked(const std::unordered_set<uint32_t>& locked,
	                     uint32_t appId)
	{
		return locked.count(appId) != 0;
	}

	inline void purgeApps(PinMap& pins,
	                      const std::unordered_set<uint32_t>& dropApps)
	{
		for (uint32_t appId : dropApps) pins.erase(appId);
	}

	inline void purgeOrphans(PinMap& pins,
	                         const std::unordered_set<uint32_t>& keepApps)
	{
		for (auto it = pins.begin(); it != pins.end(); )
		{
			if (keepApps.count(it->first) == 0) it = pins.erase(it);
			else ++it;
		}
	}
}

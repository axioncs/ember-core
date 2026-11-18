

#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace ManifestId
{

	class AppDepotIndex
	{
	public:
		void add(uint32_t appId, uint32_t depotId)
		{
			if (appId == 0 || depotId == 0) return;
			appDepots_[appId].insert(depotId);
			depotApps_[depotId].insert(appId);
		}

		std::vector<uint32_t> replaceApp(
			uint32_t appId, const std::vector<uint32_t>& depots)
		{
			if (appId == 0) return {};

			std::set<uint32_t> desired;
			for (const uint32_t depotId : depots)
				if (depotId != 0) desired.insert(depotId);

			std::vector<uint32_t> released;
			const auto currentIt = appDepots_.find(appId);
			if (currentIt != appDepots_.end())
			{
				for (const uint32_t depotId : currentIt->second)
				{
					if (desired.contains(depotId)) continue;
					auto owners = depotApps_.find(depotId);
					if (owners == depotApps_.end())
					{
						released.push_back(depotId);
						continue;
					}
					if (owners->second.size() == 1)
						released.push_back(depotId);
					owners->second.erase(appId);
					if (owners->second.empty()) depotApps_.erase(owners);
				}
			}

			if (desired.empty())
			{
				appDepots_.erase(appId);
				return released;
			}

			appDepots_[appId] = desired;
			for (const uint32_t depotId : desired)
				depotApps_[depotId].insert(appId);
			return released;
		}

		std::vector<uint32_t> depotsForApp(uint32_t appId) const
		{
			std::vector<uint32_t> out;
			const auto it = appDepots_.find(appId);
			if (it == appDepots_.end()) return out;
			out.assign(it->second.begin(), it->second.end());
			return out;
		}

		std::vector<uint32_t> exclusiveDepotsForApp(uint32_t appId) const
		{
			std::vector<uint32_t> out;
			const auto it = appDepots_.find(appId);
			if (it == appDepots_.end()) return out;
			for (const uint32_t depotId : it->second)
			{
				const auto owners = depotApps_.find(depotId);
				if (owners != depotApps_.end() && owners->second.size() == 1)
					out.push_back(depotId);
			}
			return out;
		}

		std::vector<uint32_t> releaseApp(uint32_t appId)
		{
			const auto it = appDepots_.find(appId);
			if (it == appDepots_.end()) return {};
			const auto released = exclusiveDepotsForApp(appId);
			for (const uint32_t depotId : it->second)
			{
				auto owners = depotApps_.find(depotId);
				if (owners == depotApps_.end()) continue;
				owners->second.erase(appId);
				if (owners->second.empty()) depotApps_.erase(owners);
			}
			appDepots_.erase(it);
			return released;
		}

	private:
		std::map<uint32_t, std::set<uint32_t>> appDepots_;
		std::map<uint32_t, std::set<uint32_t>> depotApps_;
	};

	std::string getCatalogDir();
	std::string getCatalogPath(uint32_t depotId);

	std::string getPinnedGid(uint32_t depotId);

	bool savePin(uint32_t depotId, const std::string& gid);

	void retireDepots(const std::vector<uint32_t>& depotIds);

	std::vector<uint32_t> getExclusiveDepotsForApp(uint32_t appId);

	std::vector<uint32_t> forgetApp(uint32_t appId);

	void importLuaScripts();
}

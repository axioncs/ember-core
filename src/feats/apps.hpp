#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

class CAppOwnershipInfo;
class CNetPacket;
class CSteamId;

namespace Apps
{
	extern bool applistRequested;
	extern std::map<uint32_t, int> appIdOwnerOverride;

	inline bool genericOwnershipOverrideAllowed(
		bool playNotOwnedGames, bool ownsLicense, bool appTypeKnown,
		bool isDlc, bool automaticFilter, bool isGameOrApplication)
	{
		if (!playNotOwnedGames || ownsLicense || !appTypeKnown || isDlc)
		{
			return false;
		}
		return !automaticFilter || isGameOrApplication;
	}

	inline bool ownershipOverrideAllowed(
		bool managedApp, bool playNotOwnedGames, bool ownsLicense,
		bool appTypeKnown, bool isDlc, bool automaticFilter,
		bool isGameOrApplication)
	{
		return managedApp || genericOwnershipOverrideAllowed(
			playNotOwnedGames, ownsLicense, appTypeKnown, isDlc,
			automaticFilter, isGameOrApplication);
	}

	bool unlockApp(uint32_t appId, CAppOwnershipInfo* info, const CSteamId& ownerId);
	bool unlockApp(uint32_t appId, CAppOwnershipInfo* info);

	bool checkAppOwnership(uint32_t appId, CAppOwnershipInfo* info);
	void getSubscribedApps(uint32_t* appList, size_t size, uint32_t& count);

	void setDiscoveredAppDlcIds(const std::vector<uint32_t>& dlcIds);
	void setConfiguredAppDlcIds(const std::vector<uint32_t>& dlcIds);
	bool isAddedAppDlcId(uint32_t appId);

	bool shouldDisableCloud(uint32_t appId);
	bool shouldDisableCDKey(uint32_t appId);
	bool shouldDisableUpdates(uint32_t appId);

	void sendGamesPlayed(CNetPacket* packet);
	void sendPICSInfoRequest(CNetPacket* packet);
	void sendMsg(CNetPacket* packet);
};

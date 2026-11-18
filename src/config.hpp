#pragma once

#include "mtvar.hpp"
#include "log.hpp"
#include "sdk/steam.hpp"
#include "feats/manifestpins.hpp"
#include "feats/librarydates.hpp"

#include "yaml-cpp/exceptions.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/yaml.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <pthread.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class CFileWatcher;

class CConfig {
public:
	typedef unsigned int SmartTicketsFlags;

	enum ESmartTickets : SmartTicketsFlags
	{
		k_ESmartTicketsSteamDRM = 1 << 0,
		k_ESmartTicketsDenuvo = 1 << 1,
	};

	struct FakeGame_t
	{
		uint32_t appId = 0;
		std::string title;
	};

	class CDlcData
	{
	public:
		uint32_t parentId;
		std::unordered_map<uint32_t, std::string> dlcIds;

	};

	struct DonateSettings
	{
		bool enabled = true;
		std::string url = "https://manifest.luastools.xyz";
		uint32_t intervalSecs = 30;
		uint32_t wantedRefreshSecs = 300;
		uint32_t maxMintsPerCycle = 25;
		uint32_t minMintIntervalMs = 2000;
		uint32_t maxMintsPerSession = 0;
	};

	static std::vector<std::uint32_t> selectConfiguredDlcIds(
		const std::unordered_set<std::uint32_t>& managedParents,
		const std::unordered_map<std::uint32_t, CDlcData>& configured)
	{
		std::vector<std::uint32_t> selected;
		for (const auto& [parentId, data] : configured)
		{
			if (parentId == 0 || !managedParents.contains(parentId)) continue;
			for (const auto& [dlcId, name] : data.dlcIds)
			{
				(void)name;
				if (dlcId != 0) selected.push_back(dlcId);
			}
		}
		std::sort(selected.begin(), selected.end());
		selected.erase(std::unique(selected.begin(), selected.end()), selected.end());
		return selected;
	}

	enum class ELoadError : uint32_t
	{
		None,
		MissingKey,
		ParsingException
	};
	MTVariable<ELoadError> __loadErrors;

	MTVariable<std::unordered_set<uint32_t>> appIds;

	MTVariable<std::unordered_set<uint32_t>> managedAppIds;

	MTVariable<std::unordered_set<uint32_t>> addedAppIds;

	MTVariable<std::unordered_set<uint32_t>> installedAppIds;
	MTVariable<std::unordered_map<uint32_t, CDlcData>> dlcData;
	MTVariable<std::unordered_map<uint32_t, uint64_t>> appTokens;
	MTVariable<std::unordered_set<uint32_t>> fakeOffline;
	MTVariable<std::unordered_map<uint32_t, uint32_t>> fakeAppIds;
	MTVariable<FakeGame_t> idleStatus;
	MTVariable<std::unordered_map<uint32_t, std::string>> gameTitles;
	MTVariable<std::unordered_map<uint32_t, uint32_t>> subscriptionTimestamps;
	LibraryDates::Store libraryDates;

	MTVariable<std::unordered_map<uint32_t, uint64_t>> manifestPins;
	MTVariable<std::unordered_set<uint32_t>> lockedApps;
	MTVariable<ManifestPins::PinMap> manifestPinsByApp;

	MTVariable<std::unordered_map<uint32_t, std::unordered_set<uint32_t>>> denuvoGames;
	MTVariable<std::unordered_map<AppId_t, uint64_t>> steamIdOverride;

	MTVariable<bool> disableFamilyLock;
	MTVariable<bool> disableParentalRestrictions;
	MTVariable<bool> useWhiteList;
	MTVariable<bool> automaticFilter;
	MTVariable<bool> playNotOwnedGames;
	MTVariable<SmartTicketsFlags> smartTickets;
	MTVariable<bool> safeMode;
	MTVariable<bool> notifications;
	MTVariable<bool> warnHashMissmatch;
	MTVariable<bool> notifyInit;
	MTVariable<bool> api;
	MTVariable<bool> disableCloud;

	MTVariable<bool> autoUpdateApps;
	MTVariable<DonateSettings> donate;

	MTVariable<bool> injectAllAdvertisedDlc;
	MTVariable<bool> achievements;
	MTVariable<uint64_t> achievementOwnerId;
	MTVariable<std::unordered_map<uint32_t, uint64_t>> achievementOwners;
	MTVariable<std::string> fakeEmail;
	MTVariable<int32_t> fakeWalletBalance;
	MTVariable<unsigned int> logLevel;
	MTVariable<bool> patternCache;

	MTVariable<bool> asyncProvision;
	MTVariable<bool> extendedLogging;

	MTVariable<std::size_t> maxManagedApps;

	CFileWatcher* watcher;

	~CConfig();

	std::string getDir();
	std::string getPath();
	std::string getLuaAppIdsPath();
	bool createFile();
	bool init();

	std::unordered_set<uint32_t> discoverStPluginAppIds();
	std::unordered_set<uint32_t> loadLuaAppIdsYaml();

	void setError(ELoadError err);
	bool loadSettings();

	template<typename T>
	T getSetting(YAML::Node& node, const char* name, T defVal)
	{
		if (!node[name])
		{

			setError(ELoadError::MissingKey);
			return defVal;
		}

		return node[name].as<T>(defVal);
	};

	template<typename T>
	std::unordered_set<T> getList(YAML::Node& rootNode, const char* name)
	{
		auto list = std::unordered_set<T>();

		const auto node = rootNode[name];
		if (!node)
		{

			setError(ELoadError::MissingKey);
			return list;
		}

		for(auto subNode : node)
		{
			try
			{
				T val = subNode.as<T>();
				list.emplace(val);

				if (std::is_same_v<T, uint32_t>)
				{
					g_pLog->info("Added %u to %s\n", val, name);
				}
			}
			catch(...)
			{

				setError(ELoadError::ParsingException);
			}
		}

		return list;
	}

	template<typename T, typename T2>
	std::unordered_map<T, T2> getMap(YAML::Node& rootNode, const char* name)
	{
		auto map = std::unordered_map<T, T2>();

		const auto node = rootNode[name];
		if (!node)
		{

			setError(ELoadError::MissingKey);
			return map;
		}

		for(auto& subNode : node)
		{
			try
			{

				auto k = subNode.first.as<T>();
				auto v = subNode.second.as<T2>();

				map[k] = v;

				if (std::is_same_v<T, uint32_t> && std::is_same_v<T, T2>)
				{
					g_pLog->info("Added %u to %u in %s\n", k, v, name);
				}
				else if (std::is_same_v<T, uint32_t> && std::is_same_v<T2, uint64_t>)
				{
					g_pLog->info("Added %u to %llu in %s\n", k, v, name);
				}
			}
			catch(...)
			{

				setError(ELoadError::ParsingException);
			}
		}

		return map;
	}

	bool isAddedAppId(uint32_t appId);
	bool addAdditionalAppId(uint32_t appId);

	uint64_t getManifestPin(uint32_t depotId);

	uint64_t getManifestPin(uint32_t appId, uint32_t depotId);

	uint64_t getManifestPinForPlanner(uint32_t appId, uint32_t depotId);
	bool isAppLocked(uint32_t appId);

	std::unordered_map<uint32_t, uint64_t> getAppPinnedDepots(uint32_t appId);
	void purgePinsForApps(const std::unordered_set<uint32_t>& appIds);

	bool shouldExcludeAppId(uint32_t appId);
	uint32_t getDenuvoGameOwner(uint32_t appId);
};

extern CConfig g_config;

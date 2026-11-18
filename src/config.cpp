#include "config.hpp"
#include "config_path.hpp"

#include "afftrace.hpp"
#include "confload.hpp"
#include "config_default.hpp"
#include "filewatcher.hpp"
#include "log.hpp"
#include "yaml-cpp/yaml.h"

#include "feats/appinfo_provision.hpp"
#include "feats/apps.hpp"
#include "feats/depotkey.hpp"
#include "config_discovery.hpp"
#include "feats/hotreload.hpp"
#include "feats/manifestdonor_policy.hpp"
#include "feats/manifestid.hpp"
#include "feats/ticket.hpp"

#include <cerrno>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace
{

	bool parsePinGid(const std::string& s, uint64_t& out)
	{
		if (s.empty()) return false;
		errno = 0;
		char* end = nullptr;
		const unsigned long long v = std::strtoull(s.c_str(), &end, 10);
		if (errno != 0 || end == s.c_str() || *end != '\0') return false;
		out = static_cast<uint64_t>(v);
		return true;
	}

	bool readFileText(const std::string& path, std::string& out)
	{
		std::ifstream f(path, std::ios::binary);
		if (!f.is_open()) return false;
		std::ostringstream ss;
		ss << f.rdbuf();
		out = ss.str();
		return true;
	}

	bool writeFileTextAtomic(const std::string& path, const std::string& text)
	{
		const std::string tmp = path + ".emberheal.tmp";
		{
			std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
			if (!f.is_open()) return false;
			f << text;
			f.flush();
			if (!f.good()) return false;
		}
		std::error_code ec;
		std::filesystem::rename(tmp, path, ec);
		if (ec)
		{
			std::filesystem::remove(tmp, ec);
			return false;
		}
		return true;
	}
}

std::string CConfig::getDir()
{
	return ConfigPath::emberConfigDir(
	    std::getenv("XDG_CONFIG_HOME"), std::getenv("HOME"));
}

std::string CConfig::getPath()
{
	return getDir().append("/config.yaml");
}

std::string CConfig::getLuaAppIdsPath()
{
	return getDir().append("/luaappids.yaml");
}

static std::string findSteamRootForConfig()
{
	const char* home = std::getenv("HOME");
	if (!home) return "";
	const std::vector<std::string> candidates = {
		std::string(home) + "/.steam/steam",
		std::string(home) + "/.steam/debian-installation",
		std::string(home) + "/.local/share/Steam"
	};
	for (const auto& candidate : candidates)
	{
		if (std::filesystem::exists(candidate + "/steam.sh"))
		{
			return candidate;
		}
	}
	return "";
}

std::unordered_set<uint32_t> CConfig::discoverStPluginAppIds()
{
	std::unordered_set<uint32_t> result;
	const auto steamRoot = findSteamRootForConfig();
	if (steamRoot.empty()) return result;

	const auto stplug = steamRoot + "/config/stplug-in";
	if (!std::filesystem::exists(stplug)) return result;

	std::error_code ec;
	for (const auto& entry : std::filesystem::directory_iterator(stplug, ec))
	{
		if (!entry.is_regular_file()) continue;
		const auto& path = entry.path();
		if (path.extension() != ".lua") continue;

		const uint32_t appIdStem =
		    ConfigDiscovery::appIdFromScriptName(path.filename().string());
		if (ConfigDiscovery::keepDiscoveredMainApp(
		        appIdStem, false))
		{
			result.insert(appIdStem);
		}
	}

	for (const uint32_t appId : ConfigDiscovery::recoverRacyScanDrops(
	         result, managedAppIds.get(),
	         [&](uint32_t id) {
		         std::error_code fex;
		         return std::filesystem::exists(
		             stplug + "/" + std::to_string(id) + ".lua", fex);
	         }))
	{
		result.insert(appId);
	}
	return result;
}

std::unordered_set<uint32_t> CConfig::loadLuaAppIdsYaml()
{
	std::unordered_set<uint32_t> result;
	const auto path = getLuaAppIdsPath();
	if (!std::filesystem::exists(path)) return result;

	try
	{
		YAML::Node node = YAML::LoadFile(path);
		if (node && node["AdditionalApps"])
		{
			for (auto subNode : node["AdditionalApps"])
			{
				try
				{
					const uint32_t val = subNode.as<uint32_t>();
					if (ConfigDiscovery::keepDiscoveredMainApp(
					        val, false))
					{
						result.insert(val);
					}
				}
				catch (...) {}
			}
		}
	}
	catch (...) {}

	return result;
}

bool CConfig::createFile()
{
	std::string path = getPath();
	if (!std::filesystem::exists(path))
	{
		std::string dir = getDir();
		if (!std::filesystem::exists(dir))
		{
			if (!std::filesystem::create_directory(dir))
			{
				g_pLog->notify("Unable to create config directory at %s!\n", dir.c_str());
				g_pLog->notifyUser(UserMsg::ConfigWriteFailed);
				return false;
			}

			g_pLog->debug("Created config directory at %s\n", dir.c_str());
		}

		FILE* file = fopen(path.c_str(), "w");
		if (!file)
		{
			g_pLog->notify("Unable to create config at %s!\n", path.c_str());
			g_pLog->notifyUser(UserMsg::ConfigWriteFailed);
			return false;
		}

		fputs(defaultConfig, file);
		fflush(file);
		fclose(file);
	}

	return true;
}

static void onFileChange()
{

	auto watchSpan = AffTrace::watchSpan(AffTrace::Src::Config);

	const auto before = g_config.addedAppIds.get();
	const auto beforeManaged = g_config.managedAppIds.get();

	g_config.loadSettings();

	const auto after = g_config.addedAppIds.get();
	const auto afterManaged = g_config.managedAppIds.get();
	const auto removals = ConfigDiscovery::classifyReloadRemovals(
	    beforeManaged, afterManaged, before, after);

	for (const uint32_t appId : removals.active)
	{
		const bool ticketsForgotten = Ticket::forgetApp(appId);
		const bool cacheForgotten = AppInfoProvision::forgetApp(appId);
		g_pLog->info("Config watcher: removed app=%u cache=%s tickets=%s\n",
		             appId, cacheForgotten ? "cleared" : "partial",
		             ticketsForgotten ? "cleared" : "rejected");
	}

	for (const uint32_t appId : removals.managed)
	{
		bool alsoActiveRemoved = false;
		for (const uint32_t activeId : removals.active)
		{
			if (activeId == appId)
			{
				alsoActiveRemoved = true;
				break;
			}
		}
		if (alsoActiveRemoved) continue;

		const bool cacheForgotten =
			AppInfoProvision::forgetManagedSourceApp(appId);
		g_pLog->info("Config watcher: removed managed source app=%u cache=%s "
		             "tickets=retained\n",
		             appId, cacheForgotten ? "cleared" : "partial");
	}

	DepotKey::reloadLuaScripts();
	ManifestId::importLuaScripts();

	for (uint32_t appId : after)
	{
		if (!before.contains(appId))
		{
			Ticket::restoreApp(appId);
		}
	}

	HotReload::publish(afterManaged, true);
}

bool CConfig::init()
{
	const bool created = createFile();

	loadSettings();

	if(created)
	{
		watcher = new CFileWatcher(onFileChange);
		watcher->addFile(getPath().c_str());

		watcher->addFile(getLuaAppIdsPath().c_str());
		const auto steamRoot = findSteamRootForConfig();
		if (!steamRoot.empty())
		{

			const auto stplug = steamRoot + "/config/stplug-in";
			std::error_code stplugError;
			std::filesystem::create_directories(stplug, stplugError);
			if (!watcher->addFile(stplug.c_str()))
			{

				g_pLog->warn(
				    "Config watcher: cannot watch %s (%s); adding a game will "
				    "need a Steam restart\n",
				    stplug.c_str(),
				    stplugError ? stplugError.message().c_str()
				                : "inotify_add_watch failed");
			}
		}
		else
		{
			g_pLog->warn("Config watcher: no Steam root; adding a game will need "
			             "a Steam restart\n");
		}

		if (!watcher->start())
		{
			g_pLog->warn("Config watcher: watcher thread did not start; every "
			             "change will need a Steam restart\n");
		}
	}

	return true;
}

CConfig::~CConfig()
{
	if (watcher)
	{
		delete watcher;
	}
}

void CConfig::setError(ELoadError err)
{
	if (__loadErrors.get() > err)
	{
		return;
	}

	__loadErrors = err;
}

bool CConfig::loadSettings()
{

	YAML::Node node;
	std::string raw;
	if (!readFileText(getPath(), raw))
	{
		g_pLog->notifyLong("Can not read config.yaml!\nUsing defaults");
		g_pLog->notifyUser(UserMsg::ConfigUnreadable);
		node = YAML::Node();
	}
	else
	{
		std::string repaired;
		const ConfLoad::Outcome outcome =
		    ConfLoad::parseWithRepair(raw, node, repaired);

		if (outcome == ConfLoad::Outcome::Failed)
		{
			g_pLog->notifyLong("Error parsing config.yaml!\nUsing defaults");
			g_pLog->notifyUser(UserMsg::ConfigParseFailed);
			node = YAML::Node();
		}
		else if (outcome == ConfLoad::Outcome::Repaired)
		{
			if (writeFileTextAtomic(getPath(), repaired))
				g_pLog->notify("Config had inconsistent list indentation; auto-repaired on disk\n");
			else
				g_pLog->notify("Config had inconsistent list indentation; repaired in memory (disk write failed)\n");
			g_pLog->notifyUser(UserMsg::ConfigRepaired);
		}
	}

	__loadErrors = ELoadError::None;

	dlcData = dlcData.empty();
	Apps::setConfiguredAppDlcIds({});

	disableFamilyLock = getSetting<bool>(node, "DisableFamilyShareLock", true);
	disableParentalRestrictions = getSetting<bool>(node, "DisableParentalRestrictions", false);
	useWhiteList = getSetting<bool>(node, "UseWhitelist", false);
	automaticFilter = getSetting<bool>(node, "AutoFilterList", true);
	playNotOwnedGames = getSetting<bool>(node, "PlayNotOwnedGames", false);
	smartTickets = getSetting<SmartTicketsFlags>(node, "SmartTickets", 1);

	(void)getSetting<bool>(node, "SafeMode", false);
	safeMode = false;
	notifications = getSetting<bool>(node, "Notifications", true);
	warnHashMissmatch = getSetting<bool>(node, "WarnHashMissmatch", false);
	notifyInit = getSetting<bool>(node, "NotifyInit", true);
	api = getSetting<bool>(node, "API", true);
	fakeEmail = getSetting<std::string>(node, "FakeEmail", "");
	fakeWalletBalance = getSetting<int32_t>(node, "FakeWalletBalance", 0);
	disableCloud = getSetting<bool>(node, "DisableCloud", true);
	autoUpdateApps = getSetting<bool>(node, "AutoUpdateApps", true);
	DonateSettings donateSettings;
	const YAML::Node donateNode = node["Donate"];
	if (donateNode && donateNode.IsMap())
	{
		donateSettings.enabled = donateNode["Enabled"].as<bool>(donateSettings.enabled);
		const std::string url = donateNode["Url"].as<std::string>(donateSettings.url);
		if (ManifestDonor::isBaseUrlAllowed(url) && url.size() <= 2048)
		{
			donateSettings.url = url;
			while (donateSettings.url.size() > 8 && donateSettings.url.back() == '/')
				donateSettings.url.pop_back();
		}
		auto bounded = [&](const char* key, uint32_t low, uint32_t high, uint32_t& field)
		{
			const uint64_t value = donateNode[key].as<uint64_t>(field);
			if (value >= low && value <= high) field = static_cast<uint32_t>(value);
		};
		bounded("IntervalSecs", 30, 86400, donateSettings.intervalSecs);
		bounded("WantedRefreshSecs", 30, 86400, donateSettings.wantedRefreshSecs);
		bounded("MaxMintsPerCycle", 1, 500, donateSettings.maxMintsPerCycle);
		bounded("MinMintIntervalMs", 0, 60000, donateSettings.minMintIntervalMs);
		bounded("MaxMintsPerSession", 0, 100000, donateSettings.maxMintsPerSession);
	}
	donate.set(donateSettings);
	injectAllAdvertisedDlc = getSetting<bool>(node, "InjectAllAdvertisedDlc", false);
	achievements = getSetting<bool>(node, "Achievements", true);
	achievementOwnerId = getSetting<uint64_t>(node, "AchievementOwnerId", 76561198028121353ULL);
	extendedLogging = getSetting<bool>(node, "ExtendedLogging", false);
	patternCache = getSetting<bool>(node, "PatternCache", true);
	asyncProvision = getSetting<bool>(node, "AsyncProvision", true);
	logLevel = getSetting<unsigned int>(node, "LogLevel", 2);

	maxManagedApps = static_cast<std::size_t>(getSetting<uint64_t>(
		node, "MaxManagedApps",
		static_cast<uint64_t>(ConfigDiscovery::kMaxManagedSourceApps)));

	g_pLog->info(
		"Config: DisableFamilyShareLock=%i DisableParentalRestrictions=%i "
		"UseWhitelist=%i AutoFilterList=%i PlayNotOwnedGames=%i SafeMode=%i "
		"Notifications=%i WarnHashMissmatch=%i NotifyInit=%i API=%i "
		"FakeEmail=%s FakeWalletBalance=%i DisableCloud=%i Donate=%i "
		"InjectAllAdvertisedDlc=%i Achievements=%i PatternCache=%i "
		"AsyncProvision=%i ExtendedLogging=%i AutoUpdateApps=%i LogLevel=%u\n",
		disableFamilyLock.get(), disableParentalRestrictions.get(),
		useWhiteList.get(), automaticFilter.get(), playNotOwnedGames.get(),
		safeMode.get(), notifications.get(), warnHashMissmatch.get(),
		notifyInit.get(), api.get(), fakeEmail.get().c_str(),
		fakeWalletBalance.get(), disableCloud.get(), donate.get().enabled,
		injectAllAdvertisedDlc.get(),
		achievements.get(), patternCache.get(), asyncProvision.get(),
		extendedLogging.get(), autoUpdateApps.get(), logLevel.get());

	appIds = getList<uint32_t>(node, "AppIds");

	{
		auto stplugApps  = discoverStPluginAppIds();
		auto luaYamlApps = loadLuaAppIdsYaml();
		auto legacyApps  = getList<uint32_t>(node, "AdditionalApps");
		ConfigDiscovery::InstalledApps installed;
		const auto steamRoot = findSteamRootForConfig();
		if (!steamRoot.empty())
		{
			const auto steamAppsRoots =
			    ConfigDiscovery::steamAppsRootsFor(steamRoot);
			installed = ConfigDiscovery::scanInstalledApps(steamAppsRoots);

			for (const uint32_t appId : ConfigDiscovery::recoverRacyScanDrops(
			         installed.all, installedAppIds.get(),
			         [&](uint32_t id) {
				         bool acc = false;
				         return ConfigDiscovery::confirmInstalledApp(
				             steamAppsRoots, id, acc);
			         }))
			{
				bool isAccela = false;
				if (ConfigDiscovery::confirmInstalledApp(
				        steamAppsRoots, appId, isAccela))
				{
					installed.all.insert(appId);
					if (isAccela) installed.accela.insert(appId);
				}
			}
			installedAppIds = installed.all;
		}

		const std::size_t configuredCap = maxManagedApps.get();
		const std::size_t effectiveCap = configuredCap != 0
			? configuredCap
			: stplugApps.size() + luaYamlApps.size();
		const auto selected = ConfigDiscovery::selectManagedSources(
			stplugApps, luaYamlApps, managedAppIds.get(), effectiveCap,
			installed.all);
		const auto ids = ConfigDiscovery::classifyAppIds(
		    selected.active, {}, legacyApps, installed.all, installed.accela);

		std::size_t installedLegacy = 0;
		for (uint32_t appId : legacyApps)
			if (installed.all.contains(appId)) ++installedLegacy;

		g_pLog->info("App sources: stplug-in=%zu luaappids.yaml=%zu managed=%zu "
		             "ignored-over-limit=%zu limit=%zu "
		             "Accela-installed=%zu legacy-installed=%zu legacy-stale=%zu "
		             "-> active=%zu\n",
		             stplugApps.size(), luaYamlApps.size(), ids.managed.size(),
		             selected.ignored, effectiveCap,
		             installed.accela.size(), installedLegacy,
		             legacyApps.size() - installedLegacy, ids.active.size());
		if (selected.ignored != 0)
		{
			g_pLog->notify(
			    "%zu script(s) exceed the MaxManagedApps safety limit (%zu) and "
			    "are inactive this session; the files are untouched on disk. "
			    "Raise MaxManagedApps in config.yaml (0 disables the limit) if "
			    "this is intentional.\n",
			    selected.ignored, effectiveCap);
		}

		std::string dateError;
		if (!libraryDates.refresh(getDir(),
		        steamRoot.empty() ? "" : steamRoot + "/config/stplug-in",
		        ids.active, static_cast<uint32_t>(std::time(nullptr)), dateError))
		{
			g_pLog->infoOnce("Library dates: using session dates (%s)\n", dateError.c_str());
		}
		managedAppIds = ids.managed;
		addedAppIds = ids.active;
	}

	fakeOffline = getList<uint32_t>(node, "FakeOffline");

	fakeAppIds = getMap<uint32_t, uint32_t>(node, "FakeAppIds");
	appTokens = getMap<uint32_t, uint64_t>(node, "AppTokens");
	achievementOwners = getMap<uint32_t, uint64_t>(node, "AchievementOwners");
	gameTitles = getMap<uint32_t, std::string>(node, "GameTitles");
	subscriptionTimestamps = getMap<uint32_t, uint32_t>(node, "SubscriptionTimestamps");
	steamIdOverride = getMap<AppId_t, uint64_t>(node, "SteamIdOverride");

	const auto idleStatusNode = node["IdleStatus"];
	if (idleStatusNode)
	{
		try
		{
			auto appId = idleStatusNode["AppId"].as<uint32_t>();
			auto title = idleStatusNode["Title"].as<std::string>();

			idleStatus = FakeGame_t
			{
				appId,
				title
			};

			g_pLog->info("Idle status %s with AppId %u\n", title.c_str(), appId);
		}
		catch(...)
		{

			setError(ELoadError::ParsingException);
		}
	}

	const auto dlcDataNode = node["DlcData"];
	if(dlcDataNode)
	{
		auto _dlcData = dlcData.empty();

		for(auto& app : dlcDataNode)
		{
			try
			{
				const uint32_t parentId = app.first.as<uint32_t>();

				CDlcData data;
				data.parentId = parentId;
				g_pLog->info("Adding DlcData for %u\n", parentId);

				for(auto& dlc : app.second)
				{
					const uint32_t dlcId = dlc.first.as<uint32_t>();

					const std::string dlcName = dlc.second.as<std::string>();

					data.dlcIds[dlcId] = dlcName;
					g_pLog->info("DlcId %u -> %s\n", dlcId, dlcName.c_str());
				}

				_dlcData[parentId] = data;
			}
			catch(...)
			{

				setError(ELoadError::ParsingException);
				break;
			}
		}

		auto configuredDlcIds = selectConfiguredDlcIds(
			managedAppIds.get(), _dlcData);
		dlcData = _dlcData;
		Apps::setConfiguredAppDlcIds(configuredDlcIds);
		if (!configuredDlcIds.empty())
		{
			g_pLog->info(
				"Config: authorized %zu explicit DLC id(s) for managed parents\n",
				configuredDlcIds.size());
		}
	}
	else
	{

		setError(ELoadError::MissingKey);
	}

	const auto denuvoGamesNode = node["DenuvoGames"];
	if (denuvoGamesNode)
	{
		auto _denuvoGames = denuvoGames.empty();

		for (auto& steamIdNode : denuvoGamesNode)
		{
			try
			{
				const uint32_t steamId = steamIdNode.first.as<uint32_t>();
				_denuvoGames[steamId] = std::unordered_set<uint32_t>();

				for (auto& appIdNode : steamIdNode.second)
				{
					const uint32_t appId = appIdNode.as<uint32_t>();
					_denuvoGames[steamId].emplace(appId);

					g_pLog->info("Added DenuvoGame %u\n", appId);
				}
			}
			catch (...)
			{

				setError(ELoadError::ParsingException);
			}
		}

		denuvoGames.set(_denuvoGames);
	}
	else
	{

		setError(ELoadError::MissingKey);
	}

	{
		ManifestPins::PinMap pinMap;
		const auto pinsNode = node["ManifestPins"];
		if (pinsNode && pinsNode.IsMap())
		{
			for (auto& appNode : pinsNode)
			{
				try
				{
					const auto& idNode = appNode.first;
					const auto& body = appNode.second;

					const uint32_t appId =
					    idNode.IsScalar() ? idNode.as<uint32_t>(0) : 0;
					if (appId == 0) continue;

					if (!body.IsMap()) continue;

					ManifestPins::AppPins app;
					app.locked = body["locked"].as<bool>(false);
					app.buildId = body["build_id"].as<uint32_t>(0);

					const auto depotsNode = body["depots"];
					if (depotsNode.IsMap())
					{
						for (auto& d : depotsNode)
						{
							if (!d.first.IsScalar() || !d.second.IsScalar())
								continue;
							const uint32_t depotId = d.first.as<uint32_t>(0);
							if (depotId == 0) continue;
							uint64_t gid = 0;
							if (parsePinGid(d.second.as<std::string>(""), gid) &&
							    gid != 0)
							{
								app.depots[depotId] = gid;
							}
						}
					}
					pinMap[appId] = app;
				}
				catch (...)
				{
					setError(ELoadError::ParsingException);
				}
			}
		}

		ManifestPins::purgeOrphans(pinMap, addedAppIds.get());

		manifestPinsByApp.set(pinMap);
		manifestPins.set(ManifestPins::flattenDepots(pinMap));
		lockedApps.set(ManifestPins::lockedAppSet(pinMap));
	}

	switch(__loadErrors.get())
	{
		case ELoadError::MissingKey:
			g_pLog->notify("Issues during config loading encountered! Missing key(s)");
			break;
		case ELoadError::ParsingException:
			g_pLog->notify("Issues during config loading encountered! Parsing error(s)");
			break;

		default:
			break;
	}

	return true;
}

bool CConfig::isAddedAppId(uint32_t appId)
{
	return addedAppIds.contains(appId);
}

uint64_t CConfig::getManifestPin(uint32_t depotId)
{
	return ManifestPins::getPin(manifestPins.get(), depotId);
}

uint64_t CConfig::getManifestPin(uint32_t appId, uint32_t depotId)
{
	return ManifestPins::getPinForContext(manifestPinsByApp.get(), appId, depotId);
}

uint64_t CConfig::getManifestPinForPlanner(uint32_t appId, uint32_t depotId)
{
	return ManifestPins::getPinForPlannerEntry(
	    manifestPinsByApp.get(), appId, depotId);
}

bool CConfig::isAppLocked(uint32_t appId)
{
	return ManifestPins::isLocked(lockedApps.get(), appId);
}

std::unordered_map<uint32_t, uint64_t>
CConfig::getAppPinnedDepots(uint32_t appId)
{
	const auto pinMap = manifestPinsByApp.get();
	const auto it = pinMap.find(appId);
	if (it == pinMap.end()) return {};
	return it->second.depots;
}

void CConfig::purgePinsForApps(const std::unordered_set<uint32_t>& appIds)
{
	auto pinMap = manifestPinsByApp.get();
	ManifestPins::purgeApps(pinMap, appIds);
	manifestPinsByApp.set(pinMap);
	manifestPins.set(ManifestPins::flattenDepots(pinMap));
	lockedApps.set(ManifestPins::lockedAppSet(pinMap));
}

bool CConfig::shouldExcludeAppId(uint32_t appId)
{
	bool exclude = false;

	constexpr uint32_t ONE_BILLION = 1E9;
	if (appId >= ONE_BILLION)
	{
		exclude = true;
	}
	else
	{
		bool found = appIds.get().contains(appId);
		exclude = !isAddedAppId(appId) && ((useWhiteList.get() && !found) || (!useWhiteList.get() && found));
	}

	g_pLog->debugOnce("shouldExcludeAppId(%u) -> %i\n", appId, exclude);
	return exclude;
}

uint32_t CConfig::getDenuvoGameOwner(uint32_t appId)
{
	for(const auto& tpl : denuvoGames.get())
	{
		if (tpl.second.contains(appId))
		{

			return tpl.first;
		}
	}

	return 0;
}

CConfig g_config = CConfig();

#include "apps.hpp"
#include "stats_policy.hpp"

#include "stats_policy.hpp"

#include "../sdk/CAppOwnershipInfo.hpp"
#include "../sdk/CNetPacket.hpp"
#include "../sdk/CProtoBufMsgBase.hpp"
#include "../sdk/CSteamEngine.hpp"
#include "../sdk/CUser.hpp"
#include "../sdk/EReleaseState.hpp"
#include "../sdk/IClientApps.hpp"
#include "../sdk/IClientAppManager.hpp"

#include "../config.hpp"
#include "../globals.hpp"

#include "appinfo_provision.hpp"
#include "appinfostate.hpp"
#include "clouddecision.hpp"
#include "fakeappid.hpp"
#include "synthmark.hpp"
#include "../utils/ManifestFetch.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <sys/stat.h>
#include <unordered_set>
#include <vector>

namespace
{

	std::string steamRootForManifests()
	{
		const char* home = std::getenv("HOME");
		if (!home) return {};
		const std::string roots[] = {
			std::string(home) + "/.steam/steam",
			std::string(home) + "/.steam/debian-installation",
			std::string(home) + "/.local/share/Steam",
		};
		for (const auto& r : roots)
		{
			struct stat st{};
			if (stat((r + "/steamapps/libraryfolders.vdf").c_str(), &st) == 0)
				return r;
		}
		return {};
	}

	std::vector<std::string> librarySteamappsDirs()
	{
		std::vector<std::string> dirs;
		const std::string root = steamRootForManifests();
		if (root.empty()) return dirs;
		dirs.push_back(root + "/steamapps");

		std::ifstream f(root + "/steamapps/libraryfolders.vdf");
		if (!f) return dirs;
		std::string line;
		while (std::getline(f, line))
		{

			const auto k = line.find("\"path\"");
			if (k == std::string::npos) continue;
			const auto q1 = line.find('"', k + 6);
			if (q1 == std::string::npos) continue;
			const auto q2 = line.find('"', q1 + 1);
			if (q2 == std::string::npos) continue;
			dirs.push_back(line.substr(q1 + 1, q2 - q1 - 1) + "/steamapps");
		}
		return dirs;
	}

	std::string findAppManifestPath(uint32_t appId)
	{
		const std::string name = "/appmanifest_" + std::to_string(appId) + ".acf";
		for (const auto& d : librarySteamappsDirs())
		{
			const std::string p = d + name;
			struct stat st{};
			if (stat(p.c_str(), &st) == 0) return p;
		}
		return {};
	}

	std::unordered_map<uint32_t, uint64_t> installedDepotGids(uint32_t appId)
	{
		std::unordered_map<uint32_t, uint64_t> out;
		const std::string path = findAppManifestPath(appId);
		if (path.empty()) return out;

		std::ifstream f(path);
		if (!f) return out;
		std::stringstream ss;
		ss << f.rdbuf();
		const std::string s = ss.str();

		const auto idStart = s.find("\"InstalledDepots\"");
		if (idStart == std::string::npos) return out;

		auto pos = s.find('{', idStart);
		if (pos == std::string::npos) return out;
		int depth = 1;
		size_t i = pos + 1;
		uint32_t curDepot = 0;
		while (i < s.size() && depth > 0)
		{
			const char c = s[i];
			if (c == '{') { ++depth; ++i; continue; }
			if (c == '}') { --depth; ++i; continue; }
			if (c == '"')
			{
				const auto end = s.find('"', i + 1);
				if (end == std::string::npos) break;
				const std::string tok = s.substr(i + 1, end - i - 1);
				i = end + 1;
				if (depth == 1)
				{

					try { curDepot = static_cast<uint32_t>(std::stoul(tok)); }
					catch (...) { curDepot = 0; }
				}
				else if (depth == 2 && tok == "manifest" && curDepot)
				{
					const auto v1 = s.find('"', i);
					if (v1 == std::string::npos) break;
					const auto v2 = s.find('"', v1 + 1);
					if (v2 == std::string::npos) break;
					try {
						out[curDepot] =
						    std::stoull(s.substr(v1 + 1, v2 - v1 - 1));
					} catch (...) {}
					i = v2 + 1;
				}
				continue;
			}
			++i;
		}
		return out;
	}

	bool appAtPinnedGids(uint32_t appId)
	{
		const auto pins = g_config.getAppPinnedDepots(appId);
		if (pins.empty()) return false;
		const auto installed = installedDepotGids(appId);
		if (installed.empty()) return false;

		bool anyInstalledPinMatched = false;
		for (const auto& [depot, gid] : pins)
		{
			const auto it = installed.find(depot);
			if (it == installed.end()) continue;
			if (it->second != gid) return false;
			anyInstalledPinMatched = true;
		}
		return anyInstalledPinMatched;
	}

	bool stripInstallEligible(uint32_t appId)
	{
		const bool appManagerResolved = g_pClientAppManager != nullptr;
		bool fullyInstalled = false;
		if (appManagerResolved)
		{
			fullyInstalled = (g_pClientAppManager->getAppInstallState(appId) &
			                  APPSTATE_FULLY_INSTALLED) != 0;
		}

		const bool locallyAuthoritative = SynthMark::localAppInfoAuthority(
		    AppInfoState::isAuthoritative(appId),
		    AppInfoProvision::cachePairMtimeSecs(appId) > 0);
		return SynthMark::installStateAllowsStrip(
		    locallyAuthoritative,
		    g_config.isAddedAppId(appId), appManagerResolved, fullyInstalled);
	}

	bool permitSynthStrip(uint32_t appId)
	{
		return stripInstallEligible(appId);
	}
}

bool Apps::applistRequested;
std::map<uint32_t, int> Apps::appIdOwnerOverride;

bool Apps::unlockApp(uint32_t appId, CAppOwnershipInfo* info, const CSteamId& ownerId)
{

	info->owner = ownerId.accountId();
	info->realOwner = 0;
	info->familyShared = info->owner != g_currentSteamId.accountId();

	info->licensePermanent = !info->familyShared;
	info->retailLicense = false;
	info->licenseExpired = false;
	info->licensePending = false;
	info->licenseLocked = false;

	info->releaseState = ERELEASESTATE_RELEASED;
	info->ownsLicense = true;

	info->lowViolence = false;
	info->regionRestricted = false;

	info->autoGrant = false;
	info->trialTime = 0;
	info->fromFreeWeekend = false;
	info->freeLicense = info->familyShared;
	info->siteLicense = false;

	g_pLog->infoOnce("Unlocked %u\n", appId);
	return true;
}

bool Apps::unlockApp(uint32_t appId, CAppOwnershipInfo* info)
{
	return unlockApp(appId, info, g_currentSteamId);
}

bool Apps::checkAppOwnership(uint32_t appId, CAppOwnershipInfo* pInfo)
{
	if (!applistRequested || !pInfo || !g_currentSteamId.isSet())
	{
		return false;
	}

	const uint32_t denuvoOwner = g_config.getDenuvoGameOwner(appId);

	if (denuvoOwner && denuvoOwner != g_currentSteamId.accountId())
	{
		g_pLog->infoOnce("Skipping %u because it's a Denuvo game from someone else\n", appId);
		return false;
	}

	if (g_config.shouldExcludeAppId(appId))
	{
		return false;
	}
	if (StatsPolicy::hasNativeLicense(appId))
	{
		return false;
	}

	const bool manualUnlock = g_config.isAddedAppId(appId);
	const EAppType type = g_pClientApps
		? g_pClientApps->getAppType(appId)
		: APPTYPE_INVALID;
	const bool typeKnown = type != APPTYPE_INVALID;
	const bool gameOrApplication =
		type == APPTYPE_APPLICATION || type == APPTYPE_GAME;
	if (!ownershipOverrideAllowed(
	        manualUnlock, g_config.playNotOwnedGames.get(),
	        pInfo->ownsLicense, typeKnown, type == APPTYPE_DLC,
	        g_config.automaticFilter.get(), gameOrApplication))
	{
		return false;
	}

	if (pInfo->lowViolence)
	{
		pInfo->lowViolence = false;
		g_pLog->infoOnce("Decensoring %u\n", appId);
	}
	if (pInfo->regionRestricted)
	{
		pInfo->regionRestricted = false;
		g_pLog->infoOnce("Bypassing region restriction for %u\n", appId);
	}

	const auto times = g_config.subscriptionTimestamps.get();
	if (times.contains(appId))
	{
		pInfo->purchaseTime = times.at(appId);
	}
	else if (manualUnlock && (pInfo->subId == 0 ||
	         (pInfo->subId == -1 && !pInfo->ownsLicense)))
	{

		pInfo->purchaseTime = g_config.libraryDates.get(appId);
	}

	unlockApp(appId, pInfo);

	return true;
}

void Apps::getSubscribedApps(uint32_t* appList, size_t size, uint32_t& count)
{
	const auto& added = g_config.addedAppIds.get();
	if (!size || !appList)
	{

		count = count + added.size();
		return;
	}

	for (const auto& appId : added)
	{
		if (count < size)
			appList[count] = appId;
		++count;
	}

	applistRequested = true;
}

bool Apps::shouldDisableCloud(uint32_t appId)
{
	// `Cloud` is the only switch. The stock `DisableCloud` key is gone: two
	// switches over one behaviour meant an old config silently turned the
	// feature off while the file still showed a live-looking option.
	(void)appId;
	return !g_config.cloud.get();
}

bool Apps::shouldDisableCDKey(uint32_t appId)
{

	if (g_config.isAddedAppId(appId) || Apps::isAddedAppDlcId(appId))
	{
		return true;
	}

	CUser* user = getLocalUser();
	if (user == nullptr)
	{
		return false;
	}
	return !user->isSubscribed(appId);
}

bool Apps::shouldDisableUpdates(uint32_t appId)
{
	const bool added = g_config.isAddedAppId(appId);
	if (!added)
	{
		CUser* user = getLocalUser();
		if (user == nullptr)
		{
			return false;
		}
		return !user->isSubscribed(appId);
	}

	if (g_pClientAppManager != nullptr)
	{
		const EAppState state = g_pClientAppManager->getAppInstallState(appId);
		if (!(state & APPSTATE_FULLY_INSTALLED))
		{
			return false;
		}
	}

	if (!g_config.isAppLocked(appId))
	{
		if (ManifestFetch::areProvidersOffline())
		{
			return true;
		}
		if (!g_config.autoUpdateApps.get())
		{

			return true;
		}
		return false;
	}

	const bool atPinned = appAtPinnedGids(appId);
	g_pLog->infoOnce("Pin-lock %u: installed%s at pinned build -> updates %s\n",
	                 appId, atPinned ? "" : " NOT",
	                 atPinned ? "frozen" : "allowed (apply pin)");
	return atPinned;
}

void Apps::sendGamesPlayed(CNetPacket* pkt)
{
	auto msg = pkt->deserializeBody<CMsgClientGamesPlayed>();

	auto titles = g_config.gameTitles.get();
	bool owned = false;

	for (int i = 0; i < msg.games_played_size(); i++)
	{
		auto game = msg.mutable_games_played(i);
		if (!game->game_id())
		{
			continue;
		}

		const uint64_t gameId = game->game_id();

		if (gameId & 0x2000000ULL)
		{
			g_pLog->debug("Preserving non-Steam shortcut %llu\n", gameId);
			continue;
		}

		if(!owned)
		{
			CUser* user = getLocalUser();
			if (user != nullptr && user->isSubscribed(gameId))
			{
				owned = true;
			}
		}

		if (g_config.disableFamilyLock.get())
		{
			game->set_owner_id(1);
		}

		if (titles.contains(gameId))
		{
			game->set_game_extra_info(titles[gameId]);
		}
		else if (!owned || FakeAppIds::getFakeAppId(gameId))
		{
			char name[256] {};
			if (g_pClientApps)
			{
				const int len = g_pClientApps->getAppData(
					gameId, "common/name", name, sizeof(name));
				if (len > 0)
				{
					g_pLog->debug("AppName %s (%i)\n", name, len);
					game->set_game_extra_info(name);
				}
			}
		}

		g_pLog->debug("Playing game %llu with flags %u & pid %u\n", gameId, game->game_flags(), game->process_id());
	}

	if (msg.games_played_size() < 1)
	{
		const auto statusApp = g_config.idleStatus.get();
		if (statusApp.appId)
		{
			auto game = msg.add_games_played();
			game->set_game_id(statusApp.appId);
			game->set_game_extra_info(statusApp.title);
			game->set_game_flags(0);

			if (g_config.disableFamilyLock.get())
			{
				game->set_owner_id(1);
			}

		}
	}

	pkt->serialize(msg);
}

void Apps::sendPICSInfoRequest(CNetPacket* pkt)
{
	const auto tokens = g_config.appTokens.get();
	auto msg = pkt->deserializeBody<CMsgClientPICSProductInfoRequest>();

	{
		std::vector<uint32_t> requested;
		requested.reserve(static_cast<size_t>(msg.apps_size()));
		for (int i = 0; i < msg.apps_size(); ++i)
			requested.push_back(msg.apps(i).appid());

		const auto strip = SynthMark::stripIndices(
		    requested, [](uint32_t appId) { return permitSynthStrip(appId); });

		for (int idx : strip)
		{
			g_pLog->debug("PICS-request: stripping locally authoritative app %u\n",
			              msg.apps(idx).appid());
			msg.mutable_apps()->DeleteSubrange(idx, 1);
		}
	}

	for (int i = 0; i < msg.apps_size(); i++)
	{
		auto app = msg.mutable_apps(i);
		if (tokens.contains(app->appid()))
		{
			app->set_access_token(tokens.at(app->appid()));
			g_pLog->debug("PICS-request: attached access token for %u\n", app->appid());
		}
	}

	std::stringstream sentIds;
	for (int i = 0; i < msg.apps_size(); ++i)
	{
		if (i) sentIds << ',';
		sentIds << msg.apps(i).appid();
	}
	g_pLog->debug("PICS-request: apps=%d packages=%d ids=[%s]\n",
	              msg.apps_size(), msg.packages_size(), sentIds.str().c_str());
	pkt->serialize(msg);
}

void Apps::sendMsg(CNetPacket *pkt)
{
	switch(pkt->getProtoBufType())
	{
		case EMSG_PICS_PRODUCTINFO_REQUEST:
			sendPICSInfoRequest(pkt);
			break;

		case EMSG_GAMESPLAYED:
		case EMSG_GAMESPLAYED_NO_DATABLOB:
		case EMSG_GAMESPLAYED_WITH_DATABLOB:
			sendGamesPlayed(pkt);
			break;
	}
}

#include "apps.hpp"

#include "../config.hpp"
#include "../globals.hpp"
#include "../utils.hpp"

#include "fakeappid.hpp"
#include "stplugin.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <string>


bool Apps::applistRequested;
std::unordered_set<AppId_t> Apps::privateApps = std::unordered_set<AppId_t>();

namespace
{
	bool allDigits(const std::string& value)
	{
		return !value.empty() && std::all_of(value.begin(), value.end(), [](unsigned char c) {
			return c >= '0' && c <= '9';
		});
	}
}

std::mutex Apps::pendingLicenseChangesMutex;
std::unordered_set<AppId_t> Apps::pendingLicenseChanges = std::unordered_set<AppId_t>();

bool Apps::unlockApp(const AppId_t appId, AppOwnershipInfo_t* info, const CSteamId& ownerId)
{
	if (!info->familyShared)
	{
		//Changing the purchased field is enough, but just for nicety in the Steamclient UI we change the owner too
		info->owner = ownerId.accountId();
		info->realOwner = 0;
		info->familyShared = false;
	}
	else
	{
		LOG_ONCE("Not taking ownership of %u since it's a family shared title\n", appId);
	}

	info->licensePermanent = !info->familyShared;
	info->retailLicense = false;
	info->licenseExpired = false;
	info->licensePending = false;
	info->licenseLocked = false;

	info->releaseState = EAppReleaseState::Released;
	info->ownsLicense = true;

	info->lowViolence = false;
	info->regionRestricted = false;

	info->autoGrant = false;
	info->trialTime = 0;
	info->fromFreeWeekend = false;
	info->freeLicense = info->familyShared;
	info->siteLicense = false;

	LOG_ONCE("Unlocked %u\n", appId);
	return true;
}

bool Apps::unlockApp(const AppId_t appId, AppOwnershipInfo_t* info)
{
	return unlockApp(appId, info, g_currentSteamId);
}

// Steam hands CUserAppManager::BuildDepotDependency back an empty plan for apps
// the account does not own, and the rewrite loop below can only edit entries
// that already exist. These two helpers rebuild the plan from the stplug-in
// pins instead.
//
// Memory ownership: the client's CUtlMemory for this vector allocates through
// g_pMemAllocSteam and frees the buffer with the same interface when the vector
// dies, so the replacement buffer must come from there too. tier0's Plat_* are
// thin wrappers over exactly that interface (verified: Plat_Alloc returns a
// plain glibc chunk, and Plat_Free/libc free accept each other's pointers).
namespace
{
	// CUtlVector<T> is { T* base, i32 alloc, i32 growSize, i32 size }; the client
	// reads exactly those offsets out of the plan it hands over. ember's SDK types
	// are packed, so the struct is 0x14 here while the client's is 0x18 — the
	// member offsets are the contract, not the total size.
	static_assert(offsetof(CUtlVector<DepotInfo_t>, mem) == 0x0);
	static_assert(offsetof(CUtlMemory<DepotInfo_t>, alloc) == 0x8);
	static_assert(offsetof(CUtlMemory<DepotInfo_t>, growSize) == 0xC);
	static_assert(offsetof(CUtlVector<DepotInfo_t>, size) == 0x10);

	bool growDepotVector(CUtlVector<DepotInfo_t>* depots, const uint32_t count)
	{
		if (depots->mem.alloc >= count)
		{
			return true;
		}

		if (!Steam::Plat_Alloc || !Steam::Plat_Realloc)
		{
			LOG_ERROR("Depot plan: Steam's platform allocators are unresolved, cannot grow the depot vector\n");
			return false;
		}

		const auto bytes = static_cast<int>(sizeof(DepotInfo_t) * static_cast<size_t>(count));
		void* mem = depots->mem.base
			? Steam::Plat_Realloc(depots->mem.base, bytes)
			: Steam::Plat_Alloc(bytes);

		if (!mem)
		{
			LOG_ERROR("Depot plan: allocating %i bytes for %u depots failed\n", bytes, count);
			return false;
		}

		// CUtlMemory keeps the allocation count next to the pointer; the size is
		// the caller's. Nothing else may change: growSize stays whatever Steam set.
		depots->mem.base = static_cast<DepotInfo_t*>(mem);
		depots->mem.alloc = count;
		return true;
	}

	// The client's content language, from HKCU\Software\Valve\Steam\language in its
	// registry file. Steam uses the same setting to pick depots, so a depot whose
	// appinfo config names a different language is not part of the install.
	// Read once: changing it needs a client restart anyway.
	const std::string& clientLanguage()
	{
		static const std::string language = [] {
			const char* home = getenv("HOME");
			if (!home)
			{
				return std::string();
			}

			const std::filesystem::path candidates[] = {
				std::filesystem::path(home) / ".steam" / "registry.vdf",
				std::filesystem::path(home) / ".local" / "share" / "Steam" / "registry.vdf",
			};

			for (const auto& path : candidates)
			{
				std::ifstream file(path);
				if (!file.is_open())
				{
					continue;
				}

				std::string line;
				while (std::getline(file, line))
				{
					const auto first = line.find_first_not_of(" 	");
					if (first == std::string::npos || line.compare(first, 10, "\"language\"") != 0)
					{
						continue;
					}

					const auto open = line.find('"', first + 10);
					if (open == std::string::npos)
					{
						continue;
					}
					const auto close = line.find('"', open + 1);
					if (close == std::string::npos)
					{
						continue;
					}

					return line.substr(open + 1, close - open - 1);
				}
			}

			return std::string();
		}();

		return language;
	}

	// Empty when the app has no appinfo loaded, or the key is absent: both mean
	// "cannot tell", and the caller keeps the depot.
	std::string appData(const AppId_t appId, const std::string& key)
	{
		const auto user = g_pSteamEngine ? g_pSteamEngine->getUser() : nullptr;
		if (!user)
		{
			return {};
		}

		const auto appInfo = user->getClientApps();
		if (!appInfo)
		{
			return {};
		}

		char buf[256] {};

		if (appInfo->getAppData(appId, key.c_str(), buf, sizeof(buf)) <= 0)
		{
			return {};
		}

		return buf;
	}

	std::string depotConfig(const AppId_t appId, const AppId_t depotId, const char* name)
	{
		return appData(appId, "depots/" + std::to_string(depotId) + "/config/" + name);
	}

	// A DLC's depots are described in the DLC's own appinfo, not the base app's,
	// so asking only the base app reports "cannot tell" for them — which is how
	// the macOS half of Phantom Liberty survived the platform filter.
	std::vector<AppId_t> dlcAppIds(const AppId_t appId)
	{
		std::vector<AppId_t> ids;

		for (auto part : Utils::strsplit(appData(appId, "extended/listofdlc"), ","))
		{
			const auto first = part.find_first_not_of(" 	");
			if (first == std::string::npos)
			{
				continue;
			}

			const auto last = part.find_last_not_of(" 	");
			part = part.substr(first, last - first + 1);

			if (!part.empty() && std::all_of(part.begin(), part.end(), [](unsigned char c) { return c >= '0' && c <= '9'; }))
			{
				ids.push_back(static_cast<AppId_t>(std::stoul(part)));
			}
		}

		return ids;
	}

	// DepotLanguages from config, else the client's language. Empty means "no
	// filtering": a depot whose language cannot be read is kept either way.
	std::vector<std::string> acceptedLanguages()
	{
		std::vector<std::string> languages;

		const auto configured = g_config.depotLanguages.copy();
		if (!configured.empty())
		{
			for (auto part : Utils::strsplit(configured, ","))
			{
				const auto first = part.find_first_not_of(" 	");
				if (first == std::string::npos)
				{
					continue;
				}

				const auto last = part.find_last_not_of(" 	");
				part = part.substr(first, last - first + 1);
				std::transform(part.begin(), part.end(), part.begin(), [](unsigned char c) {
					return static_cast<char>(std::tolower(c));
				});

				languages.push_back(part);
			}

			return languages;
		}

		const auto& client = clientLanguage();
		if (!client.empty())
		{
			languages.push_back(client);
		}

		return languages;
	}

	// One entry per depot the app's own script pins to a manifest. Pins without a
	// manifest id are useless here: the plan asserts a valid manifest id, and
	// Steam would have to ask the server for one, which it denies for an unowned
	// depot.
	uint32_t synthesiseDepotPlan(const AppId_t appId, CUtlVector<DepotInfo_t>* depots, CUtlVector<DepotInfo_t>* sharedDepots)
	{
		const auto pins = StPlugin::depotPinsFor(appId);
		if (pins.empty())
		{
			LOG_INFO("Depot plan: app %u has no stplug-in depot lines, plan stays empty\n", appId);
			return 0;
		}

		size_t pinned = 0;
		for (const auto& pin : pins)
		{
			pinned += pin.manifestId ? 1 : 0;
		}

		if (!pinned)
		{
			LOG_INFO("Depot plan: app %u has %zu depot(s) but none pinned to a manifest, plan stays empty\n", appId, pins.size());
			return 0;
		}

		if (pinned != pins.size())
		{
			LOG_INFO("Depot plan: %zu of %zu depot(s) for app %u have no manifest pin and are left out\n",
				pins.size() - pinned, pins.size(), appId);
		}

		// A script pins every depot the app has: one per language, and the macOS
		// set as well. Steam installs neither, so apply the same two filters it
		// does — oslist must cover this platform, and the depot language must be
		// one we accept.
		const auto languages = acceptedLanguages();
		const auto dlcs = dlcAppIds(appId);

		// A depot's config lives in the appinfo of the app that owns it: the base
		// app for its own depots, the DLC app for the DLC's.
		const auto configFor = [&](const AppId_t depotId, const char* name) {
			auto value = depotConfig(appId, depotId, name);
			if (!value.empty())
			{
				return value;
			}

			for (const auto dlc : dlcs)
			{
				value = depotConfig(dlc, depotId, name);
				if (!value.empty())
				{
					return value;
				}
			}

			return value;
		};

		size_t localised = 0;
		size_t droppedLanguage = 0;
		size_t droppedPlatform = 0;
		size_t droppedEmpty = 0;
		size_t droppedForeign = 0;

		// The languages the app actually ships. A depot localised outside this set
		// is never needed, whichever language the user prefers.
		std::vector<std::string> shipped;
		for (auto part : Utils::strsplit(appData(appId, "depots/baselanguages"), ","))
		{
			const auto first = part.find_first_not_of(" 	");
			if (first == std::string::npos)
			{
				continue;
			}
			const auto last = part.find_last_not_of(" 	");
			part = part.substr(first, last - first + 1);
			std::transform(part.begin(), part.end(), part.begin(), [](unsigned char c) {
				return static_cast<char>(std::tolower(c));
			});
			if (!part.empty())
			{
				shipped.push_back(part);
			}
		}

		// The size may come from the script or, when it carries none, from the
		// appinfo's public manifest entry.
		struct KeptDepot_t
		{
			AppId_t depotId;
			uint64_t manifestId;
			uint64_t size;
			bool shared;
		};

		std::vector<KeptDepot_t> keep;
		keep.reserve(pinned);

		for (const auto& pin : pins)
		{
			if (!pin.manifestId)
			{
				continue;
			}

			const auto osList = configFor(pin.depotId, "oslist");
			if (!osList.empty() && osList.find("windows") == std::string::npos && osList.find("linux") == std::string::npos)
			{
				++droppedPlatform;
				continue;
			}

			const auto depotLang = configFor(pin.depotId, "language");
			if (!depotLang.empty())
			{
				++localised;

				// Not shipped by the app at all: no user can need it.
				if (!shipped.empty() && std::find(shipped.begin(), shipped.end(), depotLang) == shipped.end())
				{
					++droppedLanguage;
					continue;
				}

				if (!languages.empty() && std::find(languages.begin(), languages.end(), depotLang) == languages.end())
				{
					++droppedLanguage;
					continue;
				}
			}

			// A depot provided by another app is not this app's content.
			const auto fromApp = configFor(pin.depotId, "depotfromapp");
			if (!fromApp.empty() && allDigits(fromApp) && std::stoul(fromApp) != appId)
			{
				++droppedForeign;
				continue;
			}

			// The script's size is authoritative when present; otherwise take the
			// public manifest size from appinfo so the install total is right.
			uint64_t size = pin.size;
			bool sizeKnown = size != 0;
			if (!sizeKnown)
			{
				const auto sizeText = configFor(pin.depotId, "manifests/public/size");
				if (!sizeText.empty() && allDigits(sizeText))
				{
					size = std::stoull(sizeText);
					sizeKnown = true;
				}
			}

			// A depot we can prove is empty has nothing to install, and its stub
			// manifest trips an assert in the client's manifest loader.
			if (sizeKnown && size == 0)
			{
				++droppedEmpty;
				continue;
			}

			// Shared redistributable depots belong in sharedDepots: the caller has
			// "already provided by another app" logic for that vector.
			std::string sharedText = configFor(pin.depotId, "sharedinstall");
			if (sharedText.empty())
			{
				sharedText = appData(appId, "depots/" + std::to_string(pin.depotId) + "/sharedinstall");
			}
			const bool shared = sharedText == "1" || sharedText == "true";

			keep.push_back({ pin.depotId, pin.manifestId, size, shared });
		}

		std::string accepted;
		for (const auto& language : languages)
		{
			accepted += (accepted.empty() ? "" : ",") + language;
		}

		// localised == 0 means the appinfo carries no depot language config (or no
		// appinfo at all), so nothing could be filtered.
		LOG_INFO("Depot plan: app %u: %zu of %zu depot(s) localised, %zu dropped for language [%s], %zu dropped for platform, %zu empty, %zu provided by another app\n",
			appId, localised, pinned, droppedLanguage, accepted.c_str(), droppedPlatform, droppedEmpty, droppedForeign);

		// The depot list itself, once per distinct plan: DEBUG is stripped from
		// release builds, so this is the only way to see what the plan contains.
		std::string plan;
		for (const auto& depot : keep)
		{
			plan += (plan.empty() ? "" : ",") + std::to_string(depot.depotId);
		}

		{
			static std::mutex planMutex;
			static std::unordered_map<AppId_t, std::string> loggedPlans;
			const std::lock_guard lock(planMutex);

			if (loggedPlans[appId] != plan)
			{
				loggedPlans[appId] = plan;
				LOG_INFO("Depot plan: app %u keeps %s\n", appId, plan.c_str());
			}
		}

		if (keep.empty())
		{
			LOG_INFO("Depot plan: app %u: nothing left after language filtering, plan stays empty\n", appId);
			return 0;
		}

		if (!growDepotVector(depots, static_cast<uint32_t>(keep.size())))
		{
			return 0;
		}

		uint32_t normal = 0;
		uint32_t sharedCount = 0;
		for (const auto& depot : keep)
		{
			depot.shared ? ++sharedCount : ++normal;
		}

		if (normal && !growDepotVector(depots, normal))
		{
			return 0;
		}
		if (sharedCount && !growDepotVector(sharedDepots, sharedCount))
		{
			return 0;
		}

		uint32_t written = 0;
		uint32_t writtenShared = 0;
		for (const auto& depot : keep)
		{
			// Same shape the client itself builds: depot, app, manifest, size, rest
			// zero. The size is what the caller sums into the install size, so a
			// plan without it reads as 0 B.
			CUtlVector<DepotInfo_t>* target = depot.shared ? sharedDepots : depots;
			uint32_t& index = depot.shared ? writtenShared : written;

			DepotInfo_t& entry = target->mem.base[index];
			memset(&entry, 0, sizeof(entry));
			entry.depotId = depot.depotId;
			entry.appId = appId;
			entry.manifestId = depot.manifestId;
			entry.size = depot.size;
			++index;
		}

		depots->size = written;
		sharedDepots->size = writtenShared;
		LOG_INFO("Depot plan: synthesised %u depot(s) for unowned app %u (%u shared)\n", written, appId, writtenShared);
		return written + writtenShared;
	}
}

uint32_t Apps::buildDepotDependency(const AppId_t appId, CUtlVector<DepotInfo_t>* depots, CUtlVector<DepotInfo_t>* sharedDepots)
{
	LOG_DEBUG("Vec Alloc %u, Grow %u, Size %u\n", depots->mem.alloc, depots->mem.growSize, depots->size);

	uint32_t synthesised = 0;
	if (depots->size == 0 && g_config.isAddedAppId(appId))
	{
		synthesised = synthesiseDepotPlan(appId, depots, sharedDepots);
	}
	else if (depots->size != 0)
	{
		// Steam's own planner built this from appinfo, which is the preferred
		// path: its oslist/language rules apply instead of ours.
		LOG_INFO("Depot plan: app %u: planner produced %u depot(s)\n", appId, depots->size);
	}

	const auto depotBlacklist = g_config.depotBlacklist.get();
	const auto manifestOverrides = g_config.manifestIds.get();

	for (unsigned int i = 0; i < depots->size; )
	{
		const auto depot = depots->at(i);

		if (depotBlacklist->contains(depot->depotId))
		{
			LOG_DEBUG("Removing %u with %lu\n", depot->depotId, depot->manifestId);
			depots->swap(i, depots->size - 1);
			depots->size--;
			// The element swapped into i has not been examined yet.
			continue;
		}

		if (manifestOverrides->contains(depot->depotId))
		{
			const uint64_t oldId = depot->manifestId;
			depot->manifestId = manifestOverrides->at(depot->depotId);
			LOG_DEBUG("Overrode %u's manifest %lu with %lu\n", depot->depotId, oldId, depot->manifestId);
		}

		LOG_DEBUG("Depot %u for %u -> %lu\n", depot->depotId, depot->appId, depot->manifestId);
		++i;
	}

	for (unsigned int i = 0; i < sharedDepots->size; i++)
	{
		const auto depot = sharedDepots->at(i);
		LOG_DEBUG("Shared Depot %u for %u -> %lu\n", depot->depotId, depot->appId, depot->manifestId);
	}

	return synthesised;
}

bool Apps::checkAppOwnership(const AppId_t appId, AppOwnershipInfo_t* pInfo)
{
	if (!g_currentSteamId.isSet())
	{
		// Expected before login completes; a notification per call would spam.
		LOG_ONCE("Ownership check for %u skipped: SteamID not captured yet\n", appId);
		return false;
	}

	const CSteamId denuvoOwner = g_config.getDenuvoGameOwner(appId);

	//Do not modify Denuvo enabled Games
	if (denuvoOwner.isSet() && denuvoOwner.steamId64 != g_currentSteamId.steamId64)
	{
		//Would love to log the SteamId, but for users anonymity I won't
		LOG_ONCE("Skipping %u because it's a Denuvo game from someone else\n", appId);
		return false;
	}

	if (g_config.shouldExcludeAppId(appId))
	{
		return false;
	}

	if (pInfo->lowViolence)
	{
		pInfo->lowViolence = false;
		LOG_ONCE("Decensoring %u\n", appId);
	}
	if (pInfo->regionRestricted)
	{
		pInfo->regionRestricted = false;
		LOG_ONCE("Bypassing region restriction for %u\n", appId);
	}

	const auto times = g_config.subscriptionTimestamps.get();
	if (times->contains(appId))
	{
		pInfo->purchaseTime = times->at(appId);
	}

	if (!g_config.isAddedAppId(appId))
	{
		return false;
	}

	unlockApp(appId, pInfo);

	return true;
}

void Apps::getAppStateInfo(const AppId_t appId, AppStateInfo_t* info)
{
	if (g_config.shouldExcludeAppId(appId))
	{
		return;
	}

	const auto utils = g_pSteamEngine->getUtils();
	if (!utils)
	{
		return;
	}

	if (!utils->getAppId())
	{
		return;
	}
	
	info->ownerAccountId = g_currentSteamId.accountId();
	info->realOwner = 0;
	info->ownershipFlags = static_cast<EAppOwnershipFlags>(info->ownershipFlags & ~k_EAppOwnershipFlagsBorrowed);

	LOG_ONCE("Spoof ownership from %u\n", appId);
}

void Apps::getLegacyCDKey(const AppId_t appId)
{
	const auto user = g_pSteamEngine->getUser();
	if (user->isSubscribed(appId))
	{
		return;
	}

	std::string newKey;

	const auto keys = g_config.cdKeys.get();
	if (keys->contains(appId))
	{
		newKey = keys->at(appId);
		LOG_DEBUG("Using key from config for %u\n", appId);
	}
	else
	{
		constexpr const char* CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		static const unsigned int CHARS_SIZE = strlen(CHARS);

		//Some games use 5 SEGMENT_CHARS/5 SEGMENT_NUM or a mix of both
		//4 * 4 is the most common one though
		constexpr unsigned int SEGMENT_CHARS = 4;
		constexpr unsigned int SEGMENT_NUM = 4;

		constexpr unsigned int SEGMENT_SIZE = SEGMENT_CHARS + 1; //AAAA-, BBBB-, etc
		constexpr unsigned int KEY_SIZE = SEGMENT_SIZE * SEGMENT_NUM - 1; //Do not end with -
		
		//Don't forget null terminator since we
		//do not pass a size argument to SetLegacyCDKey
		newKey.resize(KEY_SIZE);

		srand(g_currentSteamId.steamId.accountId + appId);

		for (unsigned int i = 0; i < KEY_SIZE; i++)
		{
			if ((i + 1) % SEGMENT_SIZE == 0)
			{
				newKey[i] = '-';
				continue;
			}

			const unsigned int num = rand() % CHARS_SIZE;
			newKey[i] = CHARS[num];
		}

		LOG_DEBUG("Generated random key %s for %u\n", newKey.c_str(), appId);
	}

	const auto clientUser = user->getClientUser();

	//Wrapper function for CUser::SetLegacyCDKey, which gets called
	//from CCMInterface when a legacy cd key packet arrives
	//Injecting them only in GetLegacyCDKey doesn't work right, so we set it instead
	if (!clientUser->setLegacyCDKey(appId, newKey.c_str()))
	{
		LOG_ERROR("Failed to set CDKey for %u\n", appId);
	}
}

void Apps::getSubscribedApps(AppId_t* appList, const uint32_t size, uint32_t& count)
{
	const auto user = g_pSteamEngine->getUser();
	const auto addedApps = g_config.addedAppIds.get();

	auto apps = std::vector<AppId_t>();
	apps.resize(count);
	user->getSubscribedApps(apps.data(), apps.size());

	//Collect subscribed + AdditionalApps in a set to deduplicate them
	//Proly entirely unnecessary, but whatever
	auto merged = std::unordered_set<AppId_t>();
	for (const auto& app : apps)
	{
		merged.emplace(app);
	}
	for (const auto& app : *addedApps)
	{
		merged.emplace(app);
	}

	count = merged.size();

	//Valve calls this function twice, once with size of 0 then again
	if (!size || !appList)
	{
		return;
	}

	for (unsigned int i = 0; i < merged.size(); i++)
	{
		appList[i] = *std::next(merged.begin(), i);
	}

	applistRequested = true;
}

void Apps::parseProductInfoFromResponse(CMsgClientPICSProductInfoResponse* msg)
{
	std::lock_guard lock(pendingLicenseChangesMutex);

	auto set = std::unordered_set<AppId_t>();
	for (const auto& app : msg->apps())
	{
		if (!pendingLicenseChanges.contains(app.appid()))
		{
			continue;
		}

		set.emplace(app.appid());
		pendingLicenseChanges.erase(app.appid());
	}

	for (const auto& app : msg->unknown_appids())
	{
		if (!pendingLicenseChanges.contains(app))
		{
			continue;
		}

		pendingLicenseChanges.erase(app);
	}

	postAppLicensesChanged(set);
}

void Apps::postAppLicensesChanged(const std::unordered_set<AppId_t>& apps)
{
	if (!apps.size())
	{
		return;
	}

	const auto user = g_pSteamEngine->getUser(0);
	if (!user)
	{
		return;
	}

	AppLicensesChanged_t cb { };
	unsigned int totalPackets = std::floor(apps.size() / AppLicensesChanged_t::MAX_APPS_PER_CALLBACK);

	for (unsigned int i = 0; i < apps.size(); i++)
	{
		unsigned int idx = i % AppLicensesChanged_t::MAX_APPS_PER_CALLBACK;

		cb.apps[idx] = *std::next(apps.begin(), i);
		cb.count = idx + 1;
		cb.appsAdded |= 1llu << idx;
		cb.remainingPackets = totalPackets;

		LOG_DEBUG("AppLicensesChanged_t.apps[%u] -> %u (i -> %i, packets left -> %i, appsAdded %lu)\n", idx, cb.apps[idx], i, totalPackets, cb.appsAdded);

		if (idx + 1 >= AppLicensesChanged_t::MAX_APPS_PER_CALLBACK)
		{
			user->postCallback(ECallbackType::AppLicensesChanged_t, &cb, sizeof(cb));
			totalPackets--;
			memset(&cb, 0, sizeof(cb));
		}
	}

	if (cb.count)
	{
		user->postCallback(ECallbackType::AppLicensesChanged_t, &cb, sizeof(cb));
	}

	std::ostringstream appsLog;
	for (const auto& app : apps)
	{
		appsLog << (appsLog.str().size() ? ", " : "") << app;
	}

	LOG_API("AppLicensesChanged callback invoked for %s!\n", appsLog.str().c_str());
}

void Apps::runIPCFrame()
{
	const auto usr = g_pSteamEngine->getUser();
	if (!usr)
	{
		return;
	}

	const auto addedApps = g_config.newApps.get();
	const auto removedApps = g_config.removedApps.get();

	const auto appInfo = usr->getClientApps();

	if (removedApps->size())
	{
		postAppLicensesChanged(*removedApps);
		g_config.removedApps = g_config.removedApps.defaultInst();
	}

	const auto added = g_config.newApps;

	if (!addedApps->size())
	{
		return;
	}

	const std::lock_guard pendingLicensesLock(pendingLicenseChangesMutex);

	//Max batch of 15, otherwise not all apps will get a response which means they won't get added
	constexpr unsigned int MAX_APPS_PER_REQUEST = 15;
	AppId_t apps[MAX_APPS_PER_REQUEST] { };

	unsigned int i = 0;
	for (; i < addedApps->size(); i++)
	{
		const unsigned int idx = i % MAX_APPS_PER_REQUEST;
		const AppId_t appId = *std::next(addedApps->begin(), i);

		apps[idx] = appId;

		LOG_DEBUG("AppInfoRequest %u -> %u from (%i)\n", idx, apps[idx], i);

		if (idx + 1 >= MAX_APPS_PER_REQUEST)
		{
			appInfo->requestAppInfoUpdate(apps, MAX_APPS_PER_REQUEST);
			memset(apps, 0, sizeof(apps));
		}

		pendingLicenseChanges.emplace(appId);
	}

	const unsigned int idx = i % MAX_APPS_PER_REQUEST;
	if (apps[0])
	{
		appInfo->requestAppInfoUpdate(apps, idx);
	}

	g_config.newApps = g_config.newApps.defaultInst();
}

void Apps::spawnGame(const GameId_t* gameId, std::string& cmd, std::string& cmdline)
{
	if (!gameId)
	{
		return;
	}

	if (*gameId & GAME_TYPE_SHORTCUT)
	{
		return;
	}

	const auto options = g_config.launchOptions.get();
	std::string option;

	if (options->contains(*gameId))
	{
		option = options->at(*gameId);
	}
	else if (options->contains(UINT32_MAX - 1) && !g_pSteamEngine->getUser()->isSubscribed(*gameId))
	{
		option = options->at(UINT32_MAX - 1);
	}
	else if (options->contains(UINT32_MAX))
	{
		option = options->at(UINT32_MAX);
	}

	if (option.size() < 1)
	{
		return;
	}

	static const auto CMD_ARG = std::string("%command%");
	const size_t cmdPos = option.find(CMD_ARG);

	//Find and replace
	if (cmdPos != std::string::npos)
	{
		option.replace(cmdPos, CMD_ARG.size(), cmdline);
		cmdline = option;
	}
	else
	{
		cmdline = option;
	}

	cmd = cmdline.substr(0, cmdline.find_first_of(" "));
}

bool Apps::shouldDisableCloud(const AppId_t appId)
{
	if (!g_config.disableCloud.copy())
	{
		return false;
	}

	return !g_pSteamEngine->getUser(0)->isSubscribed(appId);
}

bool Apps::shouldDisableCDKey(const AppId_t appId)
{
	return !g_pSteamEngine->getUser(0)->isSubscribed(appId);
}

bool Apps::shouldDisableUpdates(const AppId_t appId)
{
	if (!g_config.disableUpdates.copy())
	{
		return false;
	}

	//Using AdditionalApps here aswell so users can manually block updates
	return g_config.isAddedAppId(appId) || !g_pSteamEngine->getUser(0)->isSubscribed(appId);
}

void Apps::sendAndRecvLastPlayedTimes(const char* name, CPlayer_GetLastPlayedTimes_Response* recv)
{
	if (strcmp(name, "Player.ClientGetLastPlayedTimes#1") != 0)
	{
		return;
	}

	const auto apps = g_config.addedAppIds.get();
	for (int i = recv->games_size() - 1; i >= 0; i--)
	{
		auto game = recv->mutable_games(i);
		if (!apps->contains(game->appid()))
		{
			continue;
		}

		LOG_DEBUG("Removed serverside PlayTime for %u\n", game->appid());
		recv->mutable_games()->DeleteSubrange(i, 1);
	}
}

void Apps::sendGamesPlayed(CNetPacket* pkt)
{
	const auto titles = g_config.gameTitles.get();
	const auto usr = g_pSteamEngine->getUser();
	const auto appInfo = usr->getClientApps();

	auto msg = pkt->deserializeBody<CMsgClientGamesPlayed>();

	for (int i = 0; i < msg.games_played_size(); i++)
	{
		auto game = msg.mutable_games_played(i);
		if (!game->game_id())
		{
			continue;
		}

		const uint64_t gameId = game->game_id();

		// Native non-Steam shortcut IDs use 0x2000000 in their low 32 bits.
		// Leave the original shortcut title and 64-bit ID untouched.
		if (gameId & GAME_TYPE_SHORTCUT)
		{
			LOG_DEBUG("Preserving non-Steam shortcut %lu\n", gameId);
			continue;
		}

		if (titles->contains(gameId))
		{
			game->set_game_extra_info(titles->at(gameId));
		}
		//This probably belongs into FakeAppIds, but the GameTitles does not so it stays here
		else if (FakeAppIds::getFakeAppId(gameId))
		{
			char name[256] {}; //No clue how long titles can get
			int len;

			if (privateApps.contains(gameId))
			{
				strcpy(name, "Redacted");
				len = strlen(name);
			}
			else
			{
				len = appInfo->getAppData(gameId, "common/name", name, sizeof(name));
			}

			if (len > 0)
			{
				LOG_DEBUG("AppName %s (%i)\n", name, len);
				game->set_game_extra_info(name);
			}
		}

		//msg->mutable_games_played(i)->ParseFromString(game.SerializeAsString());

		LOG_DEBUG("Playing game %lu with flags %u & pid %u\n", gameId, game->game_flags(), game->process_id());
	}

	if (msg.games_played_size() < 1)
	{
		const auto statusApp = g_config.idleStatus.get();
		if (statusApp->appId)
		{
			auto game = msg.add_games_played();

			game->set_game_id(statusApp->appId);
			game->set_game_extra_info(statusApp->title);
			game->set_game_flags(0);
			//game->set_game_flags(EGAMEFLAG_MULTIPLAYER);

			AppOwnershipInfo_t info { };
			if (usr->checkAppOwnership(statusApp->appId, &info))
			{
				//Lock the selected license
				game->set_owner_id(info.owner);
			}
			else
			{
				game->set_owner_id(g_currentSteamId.accountId());
			}
		}
	}

	pkt->serialize(msg);
}

void Apps::sendPICSInfoRequest(CNetPacket* pkt)
{
	const auto tokens = g_config.appTokens.get();
	auto msg = pkt->deserializeBody<CMsgClientPICSProductInfoRequest>();

	for (int i = 0; i < msg.apps_size(); i++)
	{
		auto app = msg.mutable_apps(i);
		if (tokens->contains(app->appid()))
		{
			app->set_access_token(tokens->at(app->appid()));
			LOG_DEBUG("Used access token from config for %u\n", app->appid());
		}
	}

	pkt->serialize(msg);
}

void Apps::sendMsg(CNetPacket *pkt)
{
	switch(pkt->getProtoBufType())
	{
		case k_EMsgClientPICSProductInfoRequest:
			sendPICSInfoRequest(pkt);
			break;

		case k_EMsgClientGamesPlayed:
		case k_EMsgClientGamesPlayedNoDataBlob:
		case k_EMsgClientGamesPlayedWithDataBlob:
			sendGamesPlayed(pkt);
			break;

		default:
			break;
	}
}

void Apps::setConfigStoreString(const char* key, const char* value)
{
	if (!std::string(key).starts_with("WebStorage\\PrivateApps"))
	{
		return;
	}

	LOG_DEBUG("%s -> %s\n", key, value);

	auto str = std::string(value);
	if (str.size() < 3) //List is empty, nope out
	{
		return;
	}

	privateApps.clear();
	str = str.substr(1, str.size() - 2); //[730,240,440,etc]
	const auto split = Utils::strsplit(str, ",");

	for (const auto& s : split)
	{
		if (!Utils::isNumber(s.c_str()))
		{
			LOG_WARN("%s is not a number! Skipping\n", s.c_str());
		}

		const AppId_t appId = std::stoul(s);
		privateApps.emplace(appId);
		LOG_DEBUG("Added %u to privateApps\n", appId);
	}
}

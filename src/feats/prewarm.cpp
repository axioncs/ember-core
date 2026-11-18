

#include "prewarm.hpp"

#include "appinfo_provision.hpp"

#include "depotkey.hpp"
#include "manifeststore.hpp"

#include "../config.hpp"
#include "../globals.hpp"
#include "../log.hpp"
#include "../thread_start.hpp"

#include "../utils/ManifestFetch.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <mutex>
#include <set>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

namespace Prewarm
{

namespace
{

struct WarmTarget
{
	uint32_t appId;
	uint32_t depotId;
	uint64_t gid;
};

std::atomic<bool> g_started{false};
std::atomic<bool> g_stopRequested{false};
std::mutex g_stopMu;
std::condition_variable g_stopCv;

template <typename Rep, typename Period>
bool waitOrStop(std::chrono::duration<Rep, Period> delay)
{
	std::unique_lock<std::mutex> lock(g_stopMu);
	return g_stopCv.wait_for(lock, delay, []
	{
		return g_stopRequested.load(std::memory_order_acquire);
	});
}

std::string findSteamRoot()
{
	const char* home = std::getenv("HOME");
	if (!home) return {};
	static const char* suffixes[] = {
		"/.steam/steam",
		"/.steam/debian-installation",
		"/.local/share/Steam",
	};
	for (const char* suffix : suffixes)
	{
		const std::string candidate = std::string(home) + suffix;
		std::error_code ec;
		if (std::filesystem::exists(candidate + "/steam.sh", ec))
		{
			return candidate;
		}
	}
	return {};
}

std::string readWorkshopAcf(const std::string& steamRoot, uint32_t appId)
{
	if (steamRoot.empty()) return {};
	const std::string path = steamRoot + "/steamapps/workshop/appworkshop_"
	                         + std::to_string(appId) + ".acf";
	std::ifstream ifs(path, std::ios::binary | std::ios::ate);
	if (!ifs.is_open()) return {};
	const std::streamsize sz = ifs.tellg();
	if (sz <= 0 || sz > (8LL << 20)) return {};
	std::string out;
	out.resize(static_cast<std::size_t>(sz));
	ifs.seekg(0);
	if (!ifs.read(out.data(), sz)) return {};
	return out;
}

std::string readBuffer(uint32_t appId)
{
	std::string out;
	if (!AppInfoProvision::readValidatedCacheBuffer(appId, out)) return {};
	return out;
}

void runLoop()
{
	using namespace std::chrono_literals;

	constexpr auto kPerDepotGap = 200ms;

	t_suppressNotify = true;

	constexpr int kMaxFails = 3;
	FailureTracker failures(kMaxFails);
	PassBackoff backoff;

	for (;;)
	{
		if (g_stopRequested.load(std::memory_order_acquire))
			return;
		std::vector<WarmTarget> targets;
		bool newTarget = false;
		bool hasEligibleTarget = false;
		const auto added = g_config.addedAppIds.get();
		if (!added.empty())
		{
			std::set<DepotGid> seenTargets;
			auto appendTarget = [&targets, &seenTargets](uint32_t appId,
			                                                  const DepotGid& target)
			{
				const auto [depotId, gid] = target;
				if (depotId && gid && seenTargets.insert(target).second)
					targets.push_back({appId, depotId, gid});
			};
			auto appendManagedTarget = [&](uint32_t appId,
			                               const DepotGid& target,
			                               bool pinned)
			{
				if (DepotKey::manifestInManagedScope(
				        appId, target.first, pinned))
				{
					appendTarget(appId, target);
				}
			};

			const auto hasKey = [](uint32_t depotId) {
				return !DepotKey::getCachedKey(depotId).key.empty();
			};
			for (uint32_t appId : added)
			{
				const auto pins = g_config.getAppPinnedDepots(appId);
				std::string buf = readBuffer(appId);
				if (!pins.empty())
				{

					const auto publicTargets = buf.empty()
					    ? std::vector<DepotGid>{}
					    : planStageTargets({buf}, hasKey);
					for (const auto& target : planPinnedStageTargets(publicTargets, pins))
						appendManagedTarget(
							appId, target, pins.count(target.first) != 0);
				}
				else if (!buf.empty())
				{
					for (const auto& target : planStageTargets({buf}, hasKey))
						appendManagedTarget(appId, target, false);
				}
			}

			const std::string steamRoot = findSteamRoot();
			std::size_t workshopCount = 0;
			for (uint32_t appId : added)
			{
				const std::string acf = readWorkshopAcf(steamRoot, appId);
				if (acf.empty()) continue;

				if (!hasKey(appId)) continue;
				for (const auto& wm : extractWorkshopManifests(acf, appId))
				{
					const auto before = targets.size();
					appendManagedTarget(appId, wm, false);
					if (targets.size() != before) ++workshopCount;
				}
			}

			if (!targets.empty())
			{
				g_pLog->debug(
				    "Prewarm: keeping %zu manifest(s) warm across %zu AddedApp(s) "
				    "(incl. %zu workshop)\n",
				    targets.size(), added.size(), workshopCount);
			}

			for (const auto& [appId, depotId, gid] : targets)
			{

				if (failures.isBlacklisted(depotId, gid)) continue;
				hasEligibleTarget = true;

				const bool ready = ManifestFetch::awaitManifestBlob(
				    gid, appId, depotId, ManifestFetch::getTimeoutSec());
				if (ready)
				{
					ManifestStore::archiveManifest(depotId, gid);
					ManifestStore::markPreferredGid(depotId, gid);
				}

				if (!steamRoot.empty())
				{
					const std::string mpath = steamRoot + "/depotcache/"
					    + std::to_string(depotId) + "_"
					    + std::to_string(gid) + ".manifest";
					std::error_code ec;
					const auto sz = std::filesystem::file_size(mpath, ec);
					if (!ec && sz > 0)
					{
						failures.recordSuccess(depotId, gid);
					}
					else if (failures.recordFailure(depotId, gid))
					{
						g_pLog->debug(
						    "Prewarm: depot=%u gid=%llu blacklisted after %d "
						    "failed passes (cooling down)\n",
						    depotId, static_cast<unsigned long long>(gid),
						    kMaxFails);
					}
				}
				if (waitOrStop(kPerDepotGap))
					return;
			}

		}

		std::vector<DepotGid> targetKeys;
		targetKeys.reserve(targets.size());
		for (const auto& target : targets)
			targetKeys.emplace_back(target.depotId, target.gid);
		newTarget = backoff.observeTargets(targetKeys);
		const bool noOpPass = !targets.empty() && !hasEligibleTarget && !newTarget;
		backoff.recordPass(noOpPass);
		if (noOpPass && backoff.noOpPasses() == PassBackoff::kNoOpPassesBeforeBackoff)
		{
			g_pLog->debug(
			    "Prewarm: all targets cooling down; retrying within 1 minute\n");
			failures.resetAll();
		}
		if (waitOrStop(backoff.interval()))
			return;
	}
}

}

void ensureStarted()
{
	bool expected = false;
	if (!g_started.compare_exchange_strong(expected, true))
	{
		return;
	}

	if (g_config.addedAppIds.get().empty())
	{
		g_started.store(false);
		return;
	}

	g_stopRequested.store(false, std::memory_order_release);
	g_pLog->info("Prewarm: starting background manifest pre-warm worker\n");

	const bool started = ThreadStart::startDetached(
		[]
		{
			ThreadStart::runGuarded(
				[] { runLoop(); },
				[]
				{
					g_pLog->warn("Prewarm: background worker failed unexpectedly; will retry\n");
				},
				[]
				{
					g_started.store(false, std::memory_order_release);
				});
		},
		[]
		{
			g_stopRequested.store(false, std::memory_order_release);
			g_started.store(false, std::memory_order_release);
		},
		[]
		{
			g_stopRequested.store(true, std::memory_order_release);
			g_stopCv.notify_all();
		});
	if (!started)
	{
		g_pLog->warn("Prewarm: unable to start background worker; will retry\n");
	}
}

}

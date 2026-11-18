

#include "../src/config_discovery.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <unordered_set>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

int main()
{
	using ConfigDiscovery::appIdFromScriptName;
	using ConfigDiscovery::classifyAppIds;
	using ConfigDiscovery::classifyReloadRemovals;
	using ConfigDiscovery::confirmInstalledApp;
	using ConfigDiscovery::keepDiscoveredMainApp;
	using ConfigDiscovery::recoverRacyScanDrops;
	using ConfigDiscovery::scanInstalledApps;
	using ConfigDiscovery::selectManagedSources;
	using ConfigDiscovery::steamAppsRootsFor;

	CHECK(appIdFromScriptName("275850.lua") == 275850,
	      "numeric stem parses to app id");
	CHECK(appIdFromScriptName("815370.lua") == 815370,
	      "another numeric stem parses");
	CHECK(appIdFromScriptName("keys.lua") == 0,
	      "non-numeric stem rejected");
	CHECK(appIdFromScriptName("275850_backup.lua") == 0,
	      "stem with trailing text rejected");
	CHECK(appIdFromScriptName("275850.txt") == 0,
	      "non-.lua extension rejected");
	CHECK(appIdFromScriptName(".lua") == 0,
	      "empty stem rejected");
	CHECK(appIdFromScriptName("275850") == 0,
	      "missing extension rejected");
	CHECK(appIdFromScriptName("99999999999.lua") == 0,
	      "stem overflowing uint32 rejected");

	CHECK(keepDiscoveredMainApp(275850, true),
	      "single-depot title kept even when its appid is a managed depot");

	CHECK(keepDiscoveredMainApp(815370, false),
	      "normal title kept");

	CHECK(!keepDiscoveredMainApp(0, false),
	      "zero id rejected");
	CHECK(!keepDiscoveredMainApp(0, true),
	      "zero id rejected regardless of managed flag");

	{
		const std::unordered_set<uint32_t> scripts{10, 20, 30, 40, 50};
		const std::unordered_set<uint32_t> manual{90};
		const std::unordered_set<uint32_t> existing{40};
		const auto selected = selectManagedSources(
			scripts, manual, existing, 3);
		CHECK(selected.active == std::unordered_set<uint32_t>({10, 40, 90}),
		      "bounded source selection prioritizes manual and existing apps");
		CHECK(selected.discovered == 6 && selected.ignored == 3,
		      "bounded source selection reports the complete overflow");
	}
	{
		const std::unordered_set<uint32_t> scripts{50, 20, 40, 10, 30};
		const auto selected = selectManagedSources(
			scripts, {}, {}, 3);
		CHECK(selected.active == std::unordered_set<uint32_t>({10, 20, 30}),
		      "cold source overflow selects the lowest AppIDs deterministically");
	}
	{
		std::unordered_set<uint32_t> scripts;
		for (uint32_t appId = 1; appId <= 67395; ++appId)
			scripts.insert(appId);
		const auto selected = selectManagedSources(scripts, {}, {});
		CHECK(selected.active.size() ==
		          ConfigDiscovery::kMaxManagedSourceApps,
		      "Skyapi-sized source burst stays within the session budget");
		CHECK(selected.discovered == 67395 &&
		          selected.ignored ==
		              67395 - ConfigDiscovery::kMaxManagedSourceApps,
		      "Skyapi-sized source burst reports every deferred script");
		const auto boundary = static_cast<uint32_t>(
			ConfigDiscovery::kMaxManagedSourceApps);
		CHECK(selected.active.contains(1) &&
		          selected.active.contains(boundary) &&
		          !selected.active.contains(boundary + 1),
		      "Skyapi-sized cold selection is deterministic at the boundary");
	}

	{
		std::unordered_set<uint32_t> scripts;
		for (uint32_t appId = 1; appId <= 3000; ++appId)
			scripts.insert(400000 + appId);
		const auto selected = selectManagedSources(scripts, {}, {});
		CHECK(selected.active.size() == 3000 && selected.ignored == 0,
		      "a realistic large library is never truncated by the default cap");
	}

	{
		const std::unordered_set<uint32_t> scripts{10, 20, 30, 2050650};
		const std::unordered_set<uint32_t> installed{2050650};
		const auto selected = selectManagedSources(
			scripts, {}, {}, 2, installed);
		CHECK(selected.active.contains(2050650),
		      "an installed game outranks lower AppIDs under the cap");
		CHECK(selected.active == std::unordered_set<uint32_t>({10, 2050650}),
		      "remaining budget still fills deterministically by AppID");
	}

	{
		const std::unordered_set<uint32_t> scripts{10, 20};
		const std::unordered_set<uint32_t> installed{999999};
		const auto selected = selectManagedSources(
			scripts, {}, {}, 2, installed);
		CHECK(!selected.active.contains(999999),
		      "an installed app without a script is not a managed source");
		CHECK(selected.active == std::unordered_set<uint32_t>({10, 20}),
		      "installed-app priority cannot fabricate managed sources");
	}

	{
		const std::unordered_set<uint32_t> stplug{100};
		const std::unordered_set<uint32_t> luaYaml{101};
		const std::unordered_set<uint32_t> legacy{100, 200, 201};
		const std::unordered_set<uint32_t> installed{200};
		const std::unordered_set<uint32_t> accela{300};
		const auto ids = classifyAppIds(stplug, luaYaml, legacy, installed, accela);

		CHECK(ids.managed == std::unordered_set<uint32_t>({100, 101}),
		      "only stplug-in and luaappids ids are managed");
		CHECK(ids.active == std::unordered_set<uint32_t>({100, 101, 200, 300}),
		      "installed legacy and Accela ids remain active");
		CHECK(ids.compatibility == std::unordered_set<uint32_t>({200, 300}),
		      "managed ids are not duplicated in compatibility-only set");
		CHECK(!ids.active.contains(201),
		      "stale legacy id is discarded");
		CHECK(!ids.managed.contains(200) && !ids.managed.contains(300),
		      "compatibility ids cannot reach appinfo providers");
	}

	{
		const std::unordered_set<uint32_t> beforeManaged{100, 101};
		const std::unordered_set<uint32_t> afterManaged{101};
		const std::unordered_set<uint32_t> beforeActive{100, 101, 200};
		const std::unordered_set<uint32_t> afterActive{100, 101, 200};
		const auto removals = classifyReloadRemovals(
		    beforeManaged, afterManaged, beforeActive, afterActive);

		CHECK(removals.managed == std::vector<uint32_t>{100},
		      "managed-source removal is reported independently");
		CHECK(removals.active.empty(),
		      "compatibility-active id is not reported as active removal");
	}
	{
		const std::unordered_set<uint32_t> beforeManaged{100, 101};
		const std::unordered_set<uint32_t> afterManaged{100, 101};
		const std::unordered_set<uint32_t> beforeActive{100, 101, 200};
		const std::unordered_set<uint32_t> afterActive{100, 101};
		const auto removals = classifyReloadRemovals(
		    beforeManaged, afterManaged, beforeActive, afterActive);

		CHECK(removals.managed.empty(),
		      "active-only compatibility removal has no managed removal");
		CHECK(removals.active == std::vector<uint32_t>{200},
		      "active compatibility removal is reported for ownership cleanup");
	}

	{
		const std::unordered_set<uint32_t> beforeManaged{100};
		const std::unordered_set<uint32_t> afterManaged{100, 200};
		const std::unordered_set<uint32_t> beforeActive{100, 200};
		const std::unordered_set<uint32_t> afterActive{100, 200};
		const auto changes = classifyReloadRemovals(
		    beforeManaged, afterManaged, beforeActive, afterActive);

		CHECK(changes.managedAdded == std::vector<uint32_t>{200},
		      "managed-source addition is reported independently");
	}

	{
		const std::unordered_set<uint32_t> freshScan{100, 101};
		const std::unordered_set<uint32_t> previouslyKnown{100, 101, 102};

		const auto onDisk = std::unordered_set<uint32_t>{100, 101, 102};
		const auto recovered = recoverRacyScanDrops(
		    freshScan, previouslyKnown,
		    [&](uint32_t id) { return onDisk.contains(id); });
		CHECK(recovered == std::vector<uint32_t>{102},
		      "a still-present id dropped by a racy scan is recovered");
	}
	{
		const std::unordered_set<uint32_t> freshScan{100, 101};
		const std::unordered_set<uint32_t> previouslyKnown{100, 101, 102};

		const auto onDisk = std::unordered_set<uint32_t>{100, 101};
		const auto recovered = recoverRacyScanDrops(
		    freshScan, previouslyKnown,
		    [&](uint32_t id) { return onDisk.contains(id); });
		CHECK(recovered.empty(),
		      "a genuinely removed id (source gone) is not recovered");
	}
	{

		const std::unordered_set<uint32_t> freshScan{50};
		const std::unordered_set<uint32_t> previouslyKnown{0, 50, 30, 10, 40};
		const auto onDisk =
		    std::unordered_set<uint32_t>{0, 10, 30, 40, 50};
		const auto recovered = recoverRacyScanDrops(
		    freshScan, previouslyKnown,
		    [&](uint32_t id) { return id == 30 || id == 40; });
		CHECK(recovered == (std::vector<uint32_t>{30, 40}),
		      "racy-drop recovery is sorted and skips present/zero ids");
	}

	{
		char tempTemplate[] = "/tmp/sls-config-discovery-XXXXXX";
		char* made = mkdtemp(tempTemplate);
		CHECK(made != nullptr, "temporary Steam library created");
		if (made)
		{
			const auto steamRoot = std::filesystem::path(made) / "steam-root";
			const auto steamapps = steamRoot / "steamapps";
			const auto external = std::filesystem::path(made) / "external-library";
			std::filesystem::create_directories(steamapps / "common" / "LegacyGame");
			std::filesystem::create_directories(
			    steamapps / "common" / "AccelaGame" / ".DepotDownloader");
			std::filesystem::create_directories(
			    steamapps / "common" / "MarkerWithoutManifest" / ".DepotDownloader");
			std::filesystem::create_directories(external / "steamapps");
			{
				std::ofstream f(steamapps / "libraryfolders.vdf");
				f << "\"libraryfolders\"\n{\n\t\"1\"\n\t{\n"
				  << "\t\t\"path\"\t\"" << external.string() << "\"\n\t}\n}\n";
			}

			{
				std::ofstream f(steamapps / "appmanifest_700.acf");
				f << "\"AppState\"\n{\n\t\"installdir\"\t\"LegacyGame\"\n}\n";
			}
			{
				std::ofstream f(steamapps / "appmanifest_701.acf");
				f << "\"AppState\"\n{\n\t\"installdir\"\t\"AccelaGame\"\n}\n";
			}
			{
				std::ofstream f(steamapps / "common" / "LegacyGame" / "game.bin");
				f << "content";
			}
			{
				std::ofstream f(steamapps / "common" / "AccelaGame" / "game.bin");
				f << "content";
			}
			{
				std::ofstream f(steamapps / "appmanifest_702.acf");
				f << "\"AppState\"\n{\n\t\"installdir\"\t\"MissingGame\"\n}\n";
			}
			{
				std::ofstream f(steamapps / "appmanifest_703.acf");
				f << "\"AppState\"\n{\n\t\"installdir\"\t\"MarkerWithoutManifest\"\n}\n";
			}
			{
				std::ofstream f(steamapps / "appmanifest_bad.acf");
				f << "\"AppState\"\n{\n\t\"installdir\"\t\"AccelaGame\"\n}\n";
			}

			const auto roots = steamAppsRootsFor(steamRoot);
			CHECK(std::find(roots.begin(), roots.end(), steamapps) != roots.end(),
			      "default Steam library is scanned");
			CHECK(std::find(roots.begin(), roots.end(), external / "steamapps") != roots.end(),
			      "external Steam library is scanned");

			const auto installed = scanInstalledApps(roots);
			CHECK(installed.all == std::unordered_set<uint32_t>({700, 701}),
			      "only manifests with existing game content are installed");
			CHECK(installed.accela == std::unordered_set<uint32_t>({701}),
			      "Accela marker rediscovers the matching app id");

			{
				bool accela = true;
				CHECK(confirmInstalledApp(roots, 700, accela) && !accela,
				      "confirmInstalledApp: installed non-Accela app confirmed");
				CHECK(confirmInstalledApp(roots, 701, accela) && accela,
				      "confirmInstalledApp: installed Accela app confirmed with marker");
				CHECK(!confirmInstalledApp(roots, 702, accela),
				      "confirmInstalledApp: manifest without content is not installed");
				CHECK(!confirmInstalledApp(roots, 999999, accela),
				      "confirmInstalledApp: absent manifest is not installed");
			}

			std::error_code ec;
			std::filesystem::remove_all(made, ec);
		}
	}

	if (g_failures == 0) std::printf("\nall config-discovery checks passed\n");
	else                 std::printf("\n%d config-discovery check(s) FAILED\n", g_failures);
	return g_failures == 0 ? 0 : 1;
}



#include "../src/feats/prewarm.hpp"

#include <algorithm>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

static bool hasDepot(const std::vector<Prewarm::DepotGid>& v,
                     uint32_t depotId, uint64_t gid)
{
	return std::any_of(v.begin(), v.end(),
	    [&](const Prewarm::DepotGid& d) {
	        return d.first == depotId && d.second == gid;
	    });
}

static std::string boiWire()
{
	return
		"\"appinfo\"\n{\n"
		"\t\"appid\"\t\t\"250900\"\n"
		"\t\"depots\"\n\t{\n"
		"\t\t\"250902\"\n\t\t{\n"
		"\t\t\t\"config\"\n\t\t\t{\n\t\t\t\t\"oslist\"\t\t\"windows\"\n\t\t\t}\n"
		"\t\t\t\"manifests\"\n\t\t\t{\n\t\t\t\t\"public\"\n\t\t\t\t{\n"
		"\t\t\t\t\t\"gid\"\t\t\"1002\"\n\t\t\t\t\t\"size\"\t\t\"999\"\n\t\t\t\t}\n\t\t\t}\n\t\t}\n"
		"\t\t\"250903\"\n\t\t{\n"
		"\t\t\t\"config\"\n\t\t\t{\n\t\t\t\t\"oslist\"\t\t\"linux\"\n\t\t\t}\n"
		"\t\t\t\"manifests\"\n\t\t\t{\n\t\t\t\t\"public\"\n\t\t\t\t{\n"
		"\t\t\t\t\t\"gid\"\t\t\"1003\"\n\t\t\t\t}\n\t\t\t}\n\t\t}\n"
		"\t\t\"250904\"\n\t\t{\n"
		"\t\t\t\"config\"\n\t\t\t{\n\t\t\t\t\"oslist\"\t\t\"macos\"\n\t\t\t}\n"
		"\t\t\t\"dlcappid\"\t\t\"401920\"\n"
		"\t\t\t\"manifests\"\n\t\t\t{\n\t\t\t\t\"public\"\n\t\t\t\t{\n"
		"\t\t\t\t\t\"gid\"\t\t\"1004\"\n\t\t\t\t}\n\t\t\t}\n\t\t}\n"
		"\t\t\"250906\"\n\t\t{\n"
		"\t\t\t\"config\"\n\t\t\t{\n\t\t\t\t\"oslist\"\t\t\"linux\"\n\t\t\t}\n"
		"\t\t\t\"dlcappid\"\t\t\"401920\"\n"
		"\t\t\t\"manifests\"\n\t\t\t{\n\t\t\t\t\"public\"\n\t\t\t\t{\n"
		"\t\t\t\t\t\"gid\"\t\t\"1006\"\n\t\t\t\t}\n\t\t\t}\n\t\t}\n"
		"\t\t\"250911\"\n\t\t{\n"
		"\t\t\t\"config\"\n\t\t\t{\n\t\t\t\t\"oslist\"\t\t\"windows\"\n\t\t\t}\n"
		"\t\t\t\"dlcappid\"\t\t\"1426300\"\n"
		"\t\t\t\"manifests\"\n\t\t\t{\n\t\t\t\t\"public\"\n\t\t\t\t{\n"
		"\t\t\t\t\t\"gid\"\t\t\"1011\"\n\t\t\t\t}\n\t\t\t}\n\t\t}\n"
		"\t}\n}\n";
}

int main()
{

	{
		auto d = Prewarm::extractDepotsAndGids(boiWire());
		CHECK(d.size() == 5, "extract: all 5 depots' public gids");
		CHECK(hasDepot(d, 250902, 1002), "extract: windows base depot");
		CHECK(hasDepot(d, 250903, 1003), "extract: linux base depot");
		CHECK(hasDepot(d, 250904, 1004), "extract: macos depot (still parsed)");
		CHECK(hasDepot(d, 250906, 1006), "extract: linux DLC depot");
		CHECK(hasDepot(d, 250911, 1011), "extract: windows-only DLC depot");
	}

	{
		const std::string wire =
			"\"appinfo\"\n{\n\t\"depots\"\n\t{\n"
			"\t\t\"300\"\n\t\t{\n\t\t\t\"manifests\"\n\t\t\t{\n"
			"\t\t\t\t\"public\"\n\t\t\t\t{\n\t\t\t\t\t\"gid\"\t\t\"7\"\n\t\t\t\t}\n"
			"\t\t\t\t\"beta\"\n\t\t\t\t{\n\t\t\t\t\t\"gid\"\t\t\"8\"\n\t\t\t\t}\n"
			"\t\t\t}\n\t\t}\n\t}\n}\n";
		auto d = Prewarm::extractDepotsAndGids(wire);
		CHECK(d.size() == 1 && hasDepot(d, 300, 7),
		      "extract: takes public gid, ignores beta branch");
	}

	{
		const std::string wire =
			"\"appinfo\"\n{\n\t\"depots\"\n\t{\n"
			"\t\t\"400\"\n\t\t{\n\t\t\t\"config\"\n\t\t\t{\n\t\t\t\t\"oslist\"\t\t\"linux\"\n\t\t\t}\n\t\t}\n"
			"\t}\n}\n";
		CHECK(Prewarm::extractDepotsAndGids(wire).empty(),
		      "extract: depot without manifests -> nothing");
		CHECK(Prewarm::extractDepotsAndGids("").empty(),
		      "extract: empty input -> empty");
		CHECK(Prewarm::extractDepotsAndGids("no depots here").empty(),
		      "extract: no depots block -> empty");
	}

	{
		const std::vector<std::string> wires{ boiWire() };

		auto hasKey = [](uint32_t depotId) {
			return depotId == 250903 || depotId == 250906;
		};
		auto t = Prewarm::planStageTargets(wires, hasKey);
		CHECK(t.size() == 2, "plan: keeps only keyed depots");
		CHECK(hasDepot(t, 250903, 1003) && hasDepot(t, 250906, 1006),
		      "plan: kept the keyed depots");
		CHECK(!hasDepot(t, 250902, 1002) && !hasDepot(t, 250911, 1011),
		      "plan: dropped keyless depots");
	}

	{
		const std::vector<std::string> wires{ boiWire() };
		auto allKeys = [](uint32_t) { return true; };
		auto t = Prewarm::planStageTargets(wires, allKeys);
		CHECK(t.size() == 4, "plan: all-keys keeps linux+windows, drops macOS-only");
		CHECK(hasDepot(t, 250902, 1002), "plan: keeps windows base (Proton)");
		CHECK(hasDepot(t, 250903, 1003), "plan: keeps linux base");
		CHECK(hasDepot(t, 250906, 1006), "plan: keeps linux DLC");
		CHECK(hasDepot(t, 250911, 1011), "plan: keeps windows-only DLC (Proton)");
		CHECK(!hasDepot(t, 250904, 1004), "plan: drops macOS-only DLC depot");
	}

	{
		const std::vector<std::string> wires{ boiWire(), boiWire() };
		auto allKeys = [](uint32_t) { return true; };
		auto t = Prewarm::planStageTargets(wires, allKeys);
		CHECK(t.size() == 4, "plan: dedups identical depots across apps");
	}

	{
		const std::string wire =
			"\"appinfo\"\n{\n\t\"depots\"\n\t{\n"
			"\t\t\"500\"\n\t\t{\n\t\t\t\"manifests\"\n\t\t\t{\n"
			"\t\t\t\t\"public\"\n\t\t\t\t{\n\t\t\t\t\t\"gid\"\t\t\"55\"\n\t\t\t\t}\n\t\t\t}\n\t\t}\n"
			"\t}\n}\n";
		auto allKeys = [](uint32_t) { return true; };
		auto t = Prewarm::planStageTargets({ wire }, allKeys);
		CHECK(t.size() == 1 && hasDepot(t, 500, 55),
		      "plan: depot with no oslist is kept");
	}

	{
		const std::string wire =
			"\"appinfo\"\n{\n\t\"depots\"\n\t{\n"
			"\t\t\"600\"\n\t\t{\n\t\t\t\"config\"\n\t\t\t{\n\t\t\t\t\"oslist\"\t\t\"windows,macos,linux\"\n\t\t\t}\n"
			"\t\t\t\"manifests\"\n\t\t\t{\n\t\t\t\t\"public\"\n\t\t\t\t{\n\t\t\t\t\t\"gid\"\t\t\"66\"\n\t\t\t\t}\n\t\t\t}\n\t\t}\n"
			"\t\t\"601\"\n\t\t{\n\t\t\t\"config\"\n\t\t\t{\n\t\t\t\t\"oslist\"\t\t\"macos\"\n\t\t\t}\n"
			"\t\t\t\"manifests\"\n\t\t\t{\n\t\t\t\t\"public\"\n\t\t\t\t{\n\t\t\t\t\t\"gid\"\t\t\"67\"\n\t\t\t\t}\n\t\t\t}\n\t\t}\n"
			"\t}\n}\n";
		auto allKeys = [](uint32_t) { return true; };
		auto t = Prewarm::planStageTargets({ wire }, allKeys);
		CHECK(t.size() == 1 && hasDepot(t, 600, 66) && !hasDepot(t, 601, 67),
		      "plan: keeps multi-OS depot, drops macOS-only sibling");
	}

	{
		auto allKeys = [](uint32_t) { return true; };
		CHECK(Prewarm::planStageTargets({}, allKeys).empty(),
		      "plan: no buffers -> empty");
		CHECK(Prewarm::planStageTargets({ "" }, allKeys).empty(),
		      "plan: empty buffer -> empty");
	}

	{
		const std::vector<Prewarm::DepotGid> publicTargets{
			{100, 1000}, {101, 1001},
		};
		const std::unordered_map<uint32_t, uint64_t> pins{
			{100, 900}, {102, 902},
		};
		const auto targets = Prewarm::planPinnedStageTargets(publicTargets, pins);
		CHECK(targets.size() == 3, "pins: exact pins plus public siblings are required");
		CHECK(hasDepot(targets, 100, 900) && !hasDepot(targets, 100, 1000),
		      "pins: exact pin replaces that depot's public gid");
		CHECK(hasDepot(targets, 101, 1001) && hasDepot(targets, 102, 902),
		      "pins: unpinned public and pin-only depots are both retained");
	}

	{

		const std::string acf =
			"\"AppWorkshop\"\n{\n"
			"\t\"appid\"\t\t\"250900\"\n"
			"\t\"WorkshopItemsInstalled\"\n\t{\n"
			"\t\t\"3735300204\"\n\t\t{\n"
			"\t\t\t\"size\"\t\t\"8160\"\n"
			"\t\t\t\"manifest\"\t\t\"7988188911654056057\"\n\t\t}\n"
			"\t\t\"3735369753\"\n\t\t{\n"
			"\t\t\t\"size\"\t\t\"208273\"\n"
			"\t\t\t\"manifest\"\t\t\"9070506685956686971\"\n\t\t}\n"
			"\t}\n"
			"\t\"WorkshopItemDetails\"\n\t{\n"
			"\t\t\"3735300204\"\n\t\t{\n"
			"\t\t\t\"manifest\"\t\t\"7988188911654056057\"\n"
			"\t\t\t\"latest_manifest\"\t\t\"7988188911654056057\"\n\t\t}\n"
			"\t\t\"3735369753\"\n\t\t{\n"
			"\t\t\t\"manifest\"\t\t\"9070506685956686971\"\n"
			"\t\t\t\"latest_manifest\"\t\t\"9070506685956686971\"\n\t\t}\n"
			"\t}\n}\n";
		auto w = Prewarm::extractWorkshopManifests(acf, 250900);
		CHECK(w.size() == 2, "workshop: two distinct item manifests, deduped");
		CHECK(hasDepot(w, 250900, 7988188911654056057ULL),
		      "workshop: installed manifest of item 1 (depotId==appId)");
		CHECK(hasDepot(w, 250900, 9070506685956686971ULL),
		      "workshop: installed manifest of item 2 (depotId==appId)");
	}

	{
		const std::string acf =
			"\"AppWorkshop\"\n{\n"
			"\t\"appid\"\t\t\"250900\"\n"
			"\t\"WorkshopItemsInstalled\"\n\t{\n"
			"\t\t\"111\"\n\t\t{\n\t\t\t\"manifest\"\t\t\"1000\"\n\t\t}\n\t}\n"
			"\t\"WorkshopItemDetails\"\n\t{\n"
			"\t\t\"111\"\n\t\t{\n"
			"\t\t\t\"manifest\"\t\t\"1000\"\n"
			"\t\t\t\"latest_manifest\"\t\t\"2000\"\n\t\t}\n\t}\n}\n";
		auto w = Prewarm::extractWorkshopManifests(acf, 250900);
		CHECK(w.size() == 2, "workshop: installed + differing latest both warmed");
		CHECK(hasDepot(w, 250900, 1000ULL), "workshop: currently-installed gid");
		CHECK(hasDepot(w, 250900, 2000ULL), "workshop: pending latest gid");
	}

	{
		CHECK(Prewarm::extractWorkshopManifests("", 250900).empty(),
		      "workshop: empty ACF -> empty");
		CHECK(Prewarm::extractWorkshopManifests("no manifests here", 250900).empty(),
		      "workshop: garbage ACF -> empty");
		const std::string zero =
			"\"AppWorkshop\"\n{\n\t\"WorkshopItemsInstalled\"\n\t{\n"
			"\t\t\"1\"\n\t\t{\n\t\t\t\"manifest\"\t\t\"0\"\n\t\t}\n\t}\n}\n";
		CHECK(Prewarm::extractWorkshopManifests(zero, 250900).empty(),
		      "workshop: manifest '0' ignored");
	}

	{
		Prewarm::FailureTracker ft(3);

		CHECK(!ft.isBlacklisted(100, 9), "tracker: unseen depot is not blacklisted");

		CHECK(!ft.recordFailure(100, 9), "tracker: 1st failure does not blacklist");
		CHECK(!ft.recordFailure(100, 9), "tracker: 2nd failure does not blacklist");
		CHECK(!ft.isBlacklisted(100, 9), "tracker: below threshold, still allowed");
		CHECK(ft.recordFailure(100, 9), "tracker: 3rd failure crosses threshold");
		CHECK(ft.isBlacklisted(100, 9), "tracker: blacklisted after kMax failures");
		ft.resetAll();
		CHECK(!ft.isBlacklisted(100, 9),
		      "tracker: a cooled-down pass re-admits the real manifest target");
	}

	{
		Prewarm::FailureTracker ft(3);
		ft.recordFailure(7, 1);
		ft.recordFailure(7, 1);
		ft.recordSuccess(7, 1);
		ft.recordFailure(7, 1);
		ft.recordFailure(7, 1);
		CHECK(!ft.isBlacklisted(7, 1),
		      "tracker: success resets the streak (2 fails after reset != 3)");
	}

	{
		Prewarm::FailureTracker ft(2);
		ft.recordFailure(10, 100);
		ft.recordFailure(10, 100);
		CHECK(ft.isBlacklisted(10, 100), "tracker: depot 10 blacklisted");
		CHECK(!ft.isBlacklisted(11, 101), "tracker: sibling depot 11 unaffected");

		ft.recordFailure(10, 200);
		CHECK(!ft.isBlacklisted(10, 200),
		      "tracker: same depot, different gid tracked separately");
	}

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}

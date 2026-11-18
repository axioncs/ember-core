

#include "../src/feats/dlcids.hpp"
#include "../src/config_default.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <unordered_set>
#include <vector>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

static bool has(const std::vector<uint32_t>& v, uint32_t x)
{
	return std::find(v.begin(), v.end(), x) != v.end();
}

int main()
{

	{
		const std::string wire =
			"\"appinfo\"\n{\n"
			"\t\"appid\"\t\t\"250900\"\n"
			"\t\"extended\"\n\t{\n"
			"\t\t\"listofdlc\"\t\t\"401920,570660,1426300,3353470\"\n"
			"\t}\n"
			"}\n";
		auto dlc = AppInfoProvision::extractDlcAppIds(wire, 250900);
		CHECK(dlc.size() == 4, "listofdlc: parses all 4 ids");
		CHECK(has(dlc, 401920) && has(dlc, 570660) &&
		      has(dlc, 1426300) && has(dlc, 3353470),
		      "listofdlc: ids correct");
	}

	{
		const std::string wire =
			"\"appinfo\"\n{\n"
			"\t\"depots\"\n\t{\n"
			"\t\t\"250905\"\n\t\t{\n\t\t\t\"dlcappid\"\t\t\"401920\"\n\t\t}\n"
			"\t\t\"250906\"\n\t\t{\n\t\t\t\"dlcappid\"\t\t\"401920\"\n\t\t}\n"
			"\t\t\"250911\"\n\t\t{\n\t\t\t\"dlcappid\"\t\t\"1426300\"\n\t\t}\n"
			"\t}\n"
			"}\n";
		auto dlc = AppInfoProvision::extractDlcAppIds(wire, 250900);
		CHECK(dlc.size() == 2, "dlcappid: dedups repeats (401920 x2 -> 1)");
		CHECK(has(dlc, 401920) && has(dlc, 1426300), "dlcappid: ids correct");
	}

	{
		const std::string wire =
			"\"appinfo\"\n{\n"
			"\t\"extended\"\n\t{\n"
			"\t\t\"listofdlc\"\t\t\"401920,570660\"\n"
			"\t}\n"
			"\t\"depots\"\n\t{\n"
			"\t\t\"250911\"\n\t\t{\n\t\t\t\"dlcappid\"\t\t\"1426300\"\n\t\t}\n"
			"\t\t\"250906\"\n\t\t{\n\t\t\t\"dlcappid\"\t\t\"401920\"\n\t\t}\n"
			"\t}\n"
			"}\n";
		auto dlc = AppInfoProvision::extractDlcAppIds(wire, 250900);
		CHECK(dlc.size() == 3, "merge: 401920,570660 + 1426300 (401920 dedup)");
		CHECK(has(dlc, 401920) && has(dlc, 570660) && has(dlc, 1426300),
		      "merge: union correct");
	}

	{
		const std::string wire =
			"\"appinfo\"\n{\n"
			"\t\"extended\"\n\t{\n"
			"\t\t\"listofdlc\"\t\t\"250900,401920\"\n"
			"\t}\n"
			"}\n";
		auto dlc = AppInfoProvision::extractDlcAppIds(wire, 250900);
		CHECK(!has(dlc, 250900), "excludes the base appid");
		CHECK(has(dlc, 401920) && dlc.size() == 1, "keeps the real dlc only");
	}

	{
		const std::string wire =
			"\"appinfo\"\n{\n"
			"\t\"extended\"\n\t{\n"
			"\t\t\"listofdlc\"\t\t\"401920,570660,401920,250900\"\n"
			"\t}\n"
			"\t\"depots\"\n\t{\n"
			"\t\t\"250905\"\n\t\t{\n"
			"\t\t\t\"dlcappid\"\t\t\"1426300\"\n"
			"\t\t}\n"
			"\t\t\"250906\"\n\t\t{\n"
			"\t\t\t\"dlcappid\"\t\t\"1426300\"\n"
			"\t\t}\n"
			"\t}\n"
			"}\n";
		const auto tagged =
			AppInfoProvision::extractDlcAppIdsBySource(wire, 250900);
		CHECK(tagged.advertised.size() == 2 &&
		      has(tagged.advertised, 401920) &&
		      has(tagged.advertised, 570660),
		      "tagged: advertised ids stay in the advertised source");
		CHECK(tagged.depotTagged.size() == 1 &&
		      has(tagged.depotTagged, 1426300),
		      "tagged: depot dlcappid ids stay planner-critical");
		CHECK(!has(tagged.advertised, 250900) &&
		      !has(tagged.depotTagged, 250900),
		      "tagged: excludes the base appid from both sources");
	}

	{
		AppInfoProvision::DlcAppIds sources;
		sources.depotTagged = {100};
		sources.advertised = {200, 300};
		const std::unordered_set<uint32_t> content = {200};
		const auto selected =
			AppInfoProvision::selectDlcInjectionIds(sources, content, false);

		CHECK(selected.package0.size() == 2 &&
		      has(selected.package0, 100) && has(selected.package0, 200) &&
		      !has(selected.package0, 300),
		      "select: default package injection is tagged plus content-backed advertised");
		CHECK(selected.appDlc.size() == 3 &&
		      has(selected.appDlc, 100) && has(selected.appDlc, 200) &&
		      has(selected.appDlc, 300),
		      "select: app-local DLC coverage keeps all sources");
	}

	{
		AppInfoProvision::DlcAppIds sources;
		sources.depotTagged = {100};
		sources.advertised = {200, 300};
		const auto selected = AppInfoProvision::selectDlcInjectionIds(
			sources, {}, true);
		CHECK(selected.package0.size() == 3 &&
		      has(selected.package0, 100) && has(selected.package0, 200) &&
		      has(selected.package0, 300),
		      "select: InjectAllAdvertisedDlc restores the merged package set");
	}

	{
		CHECK(AppInfoProvision::hasDepotsInDlc(
		          "\"depots\"\n{\n\t\"hasdepotsindlc\"\t\"1\"\n}\n"),
		      "content marker: hasdepotsindlc=1 is detected");
		CHECK(!AppInfoProvision::hasDepotsInDlc(
		          "\"depots\"\n{\n\t\"hasdepotsindlc\"\t\"0\"\n}\n"),
		      "content marker: hasdepotsindlc=0 is rejected");
		CHECK(!AppInfoProvision::hasDepotsInDlc("\"appinfo\"\n{\n}\n"),
		      "content marker: missing hasdepotsindlc is rejected");
	}

	{
		uint32_t depotId = 0;
		CHECK(AppInfoProvision::depotIdFromManifestName(
		              "250905_123456.manifest", depotId) && depotId == 250905,
		      "artifact: parses depot id from a manifest filename");
		CHECK(!AppInfoProvision::depotIdFromManifestName(
		              "250905.manifest", depotId),
		      "artifact: rejects a manifest without gid separator");
		CHECK(!AppInfoProvision::depotIdFromManifestName(
		              "250905_bad.manifest", depotId),
		      "artifact: rejects a non-numeric manifest gid");
		CHECK(!AppInfoProvision::depotIdFromManifestName(
		              "0_123456.manifest", depotId),
		      "artifact: rejects depot id zero");
	}

	{
		AppInfoProvision::DlcAppIds sources;
		sources.advertised = {1799420, 2778580, 1799420};
		sources.depotTagged = {2778580, 3655690, 1245620, 0};
		const auto candidates =
			AppInfoProvision::selectDlcMetadataCandidates(1245620, sources);
		CHECK(candidates == std::vector<uint32_t>({1799420, 2778580, 3655690}),
		      "metadata candidates: union is deterministic and excludes base/zero");
	}

	{
		const AppInfoProvision::DlcMetadataFacts valid{
			.requestedBaseAppId = 1245620,
			.requestedDlcAppId = 2778580,
			.wireAppId = 2778580,
			.parentAppId = 1245620,
			.hasCommon = true,
			.typeIsDlc = true,
		};
		CHECK(AppInfoProvision::isValidDlcMetadata(valid),
		      "metadata validation: matching DLC child is accepted");
		auto publicOnly = valid;
		publicOnly.hasCommon = false;
		CHECK(!AppInfoProvision::isValidDlcMetadata(publicOnly),
		      "metadata validation: public_only record is rejected");
		auto wrongParent = valid;
		wrongParent.parentAppId = 999;
		CHECK(!AppInfoProvision::isValidDlcMetadata(wrongParent),
		      "metadata validation: unrelated DLC is rejected");
		auto wrongIdentity = valid;
		wrongIdentity.wireAppId = 2778590;
		CHECK(!AppInfoProvision::isValidDlcMetadata(wrongIdentity),
		      "metadata validation: mismatched app identity is rejected");
	}

	{
		const std::string wire =
			"\"appinfo\"\n{\n\t\"appid\"\t\t\"285900\"\n\t\"depots\"\n\t{\n"
			"\t\t\"285903\"\n\t\t{\n\t\t\t\"config\"\n\t\t\t{\n"
			"\t\t\t\t\"oslist\"\t\t\"linux\"\n\t\t\t}\n\t\t}\n\t}\n}\n";
		auto dlc = AppInfoProvision::extractDlcAppIds(wire, 285900);
		CHECK(dlc.empty(), "no listofdlc / dlcappid -> empty");
	}

	{
		CHECK(AppInfoProvision::extractDlcAppIds("", 1).empty(),
		      "empty input -> empty");
		CHECK(AppInfoProvision::extractDlcAppIds("listofdlc no quotes here", 1).empty(),
		      "garbage without quoted value -> empty");
	}

	{
		const std::string wire =
			"\"appinfo\"\n{\n\t\"extended\"\n\t{\n"
			"\t\t\"listofdlc\"\t\t\"401920,,0,570660\"\n\t}\n}\n";
		auto dlc = AppInfoProvision::extractDlcAppIds(wire, 250900);
		CHECK(dlc.size() == 2 && has(dlc, 401920) && has(dlc, 570660),
		      "skips empty/zero fields in listofdlc");
	}

	{
		const std::unordered_set<uint32_t> unsupported = {4229450};
		std::size_t removed = 0;
		const std::string filtered =
			AppInfoProvision::filterUnsupportedDlcAppIds(
				"4173830,4229450,4556380", unsupported, &removed);

		CHECK(filtered == "4173830,4556380",
		      "filter: removes unsupported content DLC only");
		CHECK(removed == 1, "filter: reports one removed DLC");

		const std::string wire =
			"\"appinfo\"\n{\n"
			"\t\"extended\"\n\t{\n"
			"\t\t\"listofdlc\"\t\t\"" + filtered + "\"\n"
			"\t}\n"
			"\t\"depots\"\n\t{\n"
			"\t\t\"4173830\"\n\t\t{\n\t\t\t\"dlcappid\"\t\t\"4173830\"\n\t\t}\n"
			"\t\t\"4556380\"\n\t\t{\n\t\t\t\"dlcappid\"\t\t\"4556380\"\n\t\t}\n"
			"\t}\n"
			"}\n";
		auto dlc = AppInfoProvision::extractDlcAppIds(wire, 2968420);
		CHECK(!has(dlc, 4229450),
		      "filter: rejected content DLC is no longer injected as owned");
		CHECK(has(dlc, 4173830) && has(dlc, 4556380),
		      "filter: keyed and virtual DLCs remain injected");
	}

	{
		const std::unordered_set<uint32_t> unsupported = {4229450};
		std::size_t removed = 0;
		const std::string filtered =
			AppInfoProvision::filterUnsupportedDlcAppIds(
				"4229450", unsupported, &removed);
		CHECK(filtered.empty(), "filter: all unsupported -> empty list");
		CHECK(removed == 1, "filter: all unsupported count is correct");
	}

	{

		const std::string wire =
			"\"appinfo\"\n{\n"
			"\t\"appid\"\t\t\"1902690\"\n"
			"\t\"extended\"\n\t{\n"
			"\t\t\"listofdlc\"\t\t\"2407210,2426930,2473120,2473121,2494230\"\n"
			"\t}\n"
			"\t\"depots\"\n\t{\n"
			"\t\t\"hasdepotsindlc\"\t\t\"1\"\n"
			"\t\t\"1902696\"\n\t\t{\n\t\t\t\"config\"\n\t\t\t{\n"
			"\t\t\t\t\"oslist\"\t\t\"windows\"\n\t\t\t}\n\t\t}\n"
			"\t}\n"
			"}\n";

		CHECK(AppInfoProvision::dlcAppIdForDepot(wire, 1902690, 2473120)
		      == 2473120,
		      "classify: an advertised DLC depot resolves to its dlc appid");
		CHECK(AppInfoProvision::dlcAppIdForDepot(wire, 1902690, 2473121)
		      == 2473121,
		      "classify: the sibling DLC depot resolves too");
		CHECK(AppInfoProvision::dlcAppIdForDepot(wire, 1902690, 1902696) == 0,
		      "classify: the base depot is never classified as DLC");
		CHECK(AppInfoProvision::dlcAppIdForDepot(wire, 1902690, 1902690) == 0,
		      "classify: the base appid itself is never classified as DLC");
		CHECK(AppInfoProvision::dlcAppIdForDepot(wire, 1902690, 228988) == 0,
		      "classify: a shared runtime depot is not classified as DLC");
		CHECK(AppInfoProvision::dlcAppIdForDepot(wire, 1902690, 0) == 0,
		      "classify: a zero depot id yields no classification");
		CHECK(AppInfoProvision::dlcAppIdForDepot("", 1902690, 2473120) == 0,
		      "classify: without appinfo nothing is classified");

		const std::string tagged =
			"\"appinfo\"\n{\n"
			"\t\"depots\"\n\t{\n"
			"\t\t\"250911\"\n\t\t{\n\t\t\t\"dlcappid\"\t\t\"1426300\"\n\t\t}\n"
			"\t}\n"
			"}\n";
		CHECK(AppInfoProvision::dlcAppIdForDepot(tagged, 250900, 250911)
		      == 1426300,
		      "classify: a dlcappid-tagged depot resolves to the tag");
		CHECK(AppInfoProvision::dlcAppIdForDepot(tagged, 250900, 250900) == 0,
		      "classify: tagged form still protects the base app");
		const std::string siblingTag =
			"\"depots\"\n{\n"
			"\t\"250901\"\n\t{\n\t\t\"manifests\"\n\t\t{\n\t\t}\n\t}\n"
			"\t\"250911\"\n\t{\n\t\t\"dlcappid\"\t\t\"1426300\"\n\t}\n"
			"}\n";
		CHECK(AppInfoProvision::dlcAppIdForDepot(
		          siblingTag, 250900, 250901) == 0,
		      "classify: a sibling depot cannot donate its dlcappid tag");
	}

	{
		const std::unordered_set<uint32_t> base = {100, 200};
		const std::vector<uint32_t> extra = {200, 300, 300, 0};
		const auto merged = AppInfoProvision::mergePackage0AppIds(base, extra);
		CHECK(merged.size() == 3 && has(merged, 100) && has(merged, 200) &&
		      has(merged, 300),
		      "package0 merge: late DLC ids join managed apps once");
	}

	{
		const std::string defaults(defaultConfig);
		CHECK(defaults.find("InjectAllAdvertisedDlc: no") != std::string::npos,
		      "config: InjectAllAdvertisedDlc defaults to no");
	}

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}

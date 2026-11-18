

#include "../src/feats/manifestsynth.hpp"

#include "yaml-cpp/yaml.h"

#include <cstdio>
#include <string>
#include <vector>

static int g_failures = 0;
#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

using ManifestSynth::SynthDepot;
using ManifestSynth::injectSynthesizedDepots;
using ManifestSynth::detectOsFromFiles;
using ManifestSynth::parseManifestSizes;
using ManifestSynth::ensureInstallDir;
using ManifestSynth::extractManifestFilenames;
using ManifestSynth::pickLauncher;
using ManifestSynth::ensureLaunchEntries;

static std::string gid(const YAML::Node& body, const std::string& depot)
{
	return body["depots"][depot]["manifests"]["public"]["gid"].as<std::string>();
}

static void putU32le(std::string& s, uint32_t v)
{
	for (int i = 0; i < 4; ++i) s.push_back(static_cast<char>((v >> (i * 8)) & 0xFF));
}

static void putVarint(std::string& s, uint64_t v)
{
	while (v >= 0x80) { s.push_back(static_cast<char>((v & 0x7F) | 0x80)); v >>= 7; }
	s.push_back(static_cast<char>(v));
}

static std::string makeManifest(uint64_t cbOriginal, uint64_t cbCompressed)
{
	std::string payload;
	putVarint(payload, (1 << 3) | 0); putVarint(payload, 632361);
	putVarint(payload, (5 << 3) | 0); putVarint(payload, cbOriginal);
	putVarint(payload, (6 << 3) | 0); putVarint(payload, cbCompressed);

	std::string out;

	putU32le(out, 0x71F617D0);
	putU32le(out, 3);
	out += "abc";
	putU32le(out, 0x1F4812BE);
	putU32le(out, static_cast<uint32_t>(payload.size()));
	out += payload;
	return out;
}

static std::string makeFilenamePayload(const std::vector<std::string>& names)
{
	std::string payload;
	for (const auto& n : names)
	{
		std::string fm;
		fm.push_back('\x0A');
		putVarint(fm, n.size());
		fm += n;
		fm.push_back('\x10');
		putVarint(fm, 123);

		payload.push_back('\x0A');
		putVarint(payload, fm.size());
		payload += fm;
	}
	std::string out;
	putU32le(out, 0x71F617D0);
	putU32le(out, static_cast<uint32_t>(payload.size()));
	out += payload;

	putU32le(out, 0x1F4812BE);
	putU32le(out, 0);
	return out;
}

int main()
{

	{
		YAML::Node body(YAML::NodeType::Map);
		body["common"]["name"] = "Risk of Rain 2";
		std::vector<SynthDepot> d = {{632361, 2538203695974683966ULL, "windows", 0, 0}};
		const int n = injectSynthesizedDepots(body, d);
		CHECK(n == 1, "one valid depot -> returns 1");
		CHECK(gid(body, "632361") == "2538203695974683966",
		      "gid emitted as the manifests.public.gid string");
		CHECK(body["depots"]["632361"]["config"]["oslist"].as<std::string>() == "windows",
		      "config.oslist set from the tuple");
		CHECK(body["common"]["name"].as<std::string>() == "Risk of Rain 2",
		      "existing common block preserved");
	}

	{
		YAML::Node body(YAML::NodeType::Map);
		std::vector<SynthDepot> d = {{632361, 7ULL, "windows", 2254733415ULL, 1772992768ULL}};
		const int n = injectSynthesizedDepots(body, d);
		CHECK(n == 1, "depot with sizes injected");
		auto pub = body["depots"]["632361"]["manifests"]["public"];
		CHECK(pub["size"].as<std::string>() == "2254733415",
		      "manifests.public.size emitted (download dialog estimate)");
		CHECK(pub["download"].as<std::string>() == "1772992768",
		      "manifests.public.download emitted");
	}

	{
		YAML::Node body(YAML::NodeType::Map);
		std::vector<SynthDepot> d = {{500, 7ULL, "", 0, 0}};
		injectSynthesizedDepots(body, d);
		auto pub = body["depots"]["500"]["manifests"]["public"];
		CHECK(!pub["size"], "no size leaf when size unknown (0)");
		CHECK(!pub["download"], "no download leaf when download unknown (0)");
	}

	{
		YAML::Node body(YAML::NodeType::Map);
		std::vector<SynthDepot> d = {{632361, 0ULL, "windows"}};
		const int n = injectSynthesizedDepots(body, d);
		CHECK(n == 0, "depot with gid 0 is skipped");
		CHECK(!body["depots"] || body["depots"].size() == 0,
		      "no depots key created when nothing usable");
	}

	{
		YAML::Node body(YAML::NodeType::Map);
		std::vector<SynthDepot> d = {{500, 123ULL, ""}};
		const int n = injectSynthesizedDepots(body, d);
		CHECK(n == 1, "depot with empty oslist still injected");
		CHECK(!body["depots"]["500"]["config"],
		      "no config node when oslist empty");
		CHECK(gid(body, "500") == "123", "gid still present without oslist");
	}

	{
		YAML::Node body(YAML::NodeType::Map);
		body["depots"]["111"]["manifests"]["public"]["gid"] = std::string("999");
		std::vector<SynthDepot> d = {{222, 333ULL, "windows"}};
		const int n = injectSynthesizedDepots(body, d);
		CHECK(n == 0, "no-op when body already has depots");
		CHECK(!body["depots"]["222"], "synthesized depot not added over real depots");
		CHECK(gid(body, "111") == "999", "existing real depot untouched");
	}

	{
		YAML::Node body(YAML::NodeType::Map);
		std::vector<SynthDepot> d = {
			{10, 1000ULL, "windows"},
			{20, 0ULL, "windows"},
			{30, 3000ULL, ""},
		};
		const int n = injectSynthesizedDepots(body, d);
		CHECK(n == 2, "two of three depots injected (one had gid 0)");
		CHECK(gid(body, "10") == "1000", "first depot gid");
		CHECK(!body["depots"]["20"], "zero-gid depot omitted");
		CHECK(gid(body, "30") == "3000", "third depot gid");
	}

	{
		YAML::Node body(YAML::NodeType::Map);
		const int n = injectSynthesizedDepots(body, {});
		CHECK(n == 0, "empty depot list -> returns 0");
		CHECK(!body["depots"], "no depots key for empty input");
	}

	{
		uint64_t size = 0, dl = 0;
		const bool ok = parseManifestSizes(makeManifest(2254733415ULL, 1772992768ULL),
		                                   size, dl);
		CHECK(ok, "metadata section parsed");
		CHECK(size == 2254733415ULL, "cb_disk_original -> size");
		CHECK(dl == 1772992768ULL, "cb_disk_compressed -> download");
	}

	{
		uint64_t size = 123, dl = 456;
		std::string junk;
		putU32le(junk, 0x71F617D0); putU32le(junk, 3); junk += "abc";
		const bool ok = parseManifestSizes(junk, size, dl);
		CHECK(!ok, "no metadata section -> returns false");
	}

	{
		uint64_t size = 0, dl = 0;
		CHECK(!parseManifestSizes("", size, dl), "empty buffer -> false");
		CHECK(!parseManifestSizes(std::string(3, '\0'), size, dl),
		      "sub-header buffer -> false");
	}

	{
		YAML::Node body(YAML::NodeType::Map);
		body["common"]["name"] = "Risk of Rain 2";
		const bool set = ensureInstallDir(body);
		CHECK(set, "installdir derived when absent");
		CHECK(body["config"]["installdir"].as<std::string>() == "Risk of Rain 2",
		      "installdir = common.name");
	}

	{
		YAML::Node body(YAML::NodeType::Map);
		body["common"]["name"] = "Risk of Rain 2";
		body["config"]["installdir"] = "RoR2Custom";
		const bool set = ensureInstallDir(body);
		CHECK(!set, "no-op when config.installdir already present");
		CHECK(body["config"]["installdir"].as<std::string>() == "RoR2Custom",
		      "existing installdir untouched");
	}

	{
		YAML::Node body(YAML::NodeType::Map);
		body["common"]["name"] = "Game: Edition/X";
		ensureInstallDir(body);
		CHECK(body["config"]["installdir"].as<std::string>() == "Game Edition X",
		      "invalid path chars replaced + collapsed");
	}

	{
		YAML::Node body(YAML::NodeType::Map);
		const bool set = ensureInstallDir(body);
		CHECK(!set, "no installdir when common.name absent");
		CHECK(!body["config"], "no config node fabricated without a name");
	}

	CHECK(detectOsFromFiles({"Risk of Rain 2.exe", "data\\foo.dll"}) == "windows",
	      "an .exe -> windows");
	CHECK(detectOsFromFiles({"oneshot", "lib/python3.7/binascii.cpython-37m-x86_64-linux-gnu.so"}) == "linux",
	      ".so + -linux-gnu -> linux");
	CHECK(detectOsFromFiles({"OneShot.app\\Contents\\MacOS\\oneshot", "x.dylib"}) == "macos",
	      ".app / .dylib -> macos");
	CHECK(detectOsFromFiles({"Audio\\music.ogg", "Data\\stuff.bin"}).empty(),
	      "shared data depot (no platform marker) -> empty");

	CHECK(detectOsFromFiles({"game.exe", "weird.so"}) == "windows",
	      "windows precedence over a stray .so");

	{
		auto names = extractManifestFilenames(makeFilenamePayload(
		    {"Risk of Rain 2.exe",
		     "Risk of Rain 2_Data\\Managed\\Assembly-CSharp.dll",
		     "UnityCrashHandler64.exe"}));
		CHECK(names.size() == 3, "all three filenames parsed");
		bool hasExe = false, hasDll = false;
		for (auto& n : names)
		{
			if (n == "Risk of Rain 2.exe") hasExe = true;
			if (n == "Risk of Rain 2_Data\\Managed\\Assembly-CSharp.dll") hasDll = true;
		}
		CHECK(hasExe, "root exe name parsed verbatim");
		CHECK(hasDll, "long subdir path parsed verbatim (no prefix corruption)");
	}

	CHECK(extractManifestFilenames("").empty(), "empty manifest -> no filenames");
	CHECK(extractManifestFilenames("garbage no sections").empty(),
	      "non-manifest bytes -> no filenames");

	{
		std::vector<std::string> files = {
		    "Risk of Rain 2.exe",
		    "Risk of Rain 2_Data\\Managed\\foo.dll",
		    "UnityCrashHandler64.exe"};
		CHECK(pickLauncher(files, "Risk of Rain 2", "windows") == "Risk of Rain 2.exe",
		      "windows: picks exe matching installdir, not the crash handler");
	}
	{

		std::vector<std::string> files = {"vc_redist.x64.exe", "Game.exe"};
		CHECK(pickLauncher(files, "Totally Different", "windows") == "Game.exe",
		      "windows: skips redist installer");
	}
	{

		std::vector<std::string> files = {
		    "oneshot",
		    "Data",
		    "lib/foo-x86_64-linux-gnu.so",
		    "UnityCrashHandler"};
		CHECK(pickLauncher(files, "OneShot", "linux") == "oneshot",
		      "linux: picks root no-ext binary matching installdir");
	}
	{

		std::vector<std::string> files = {"RiskOfRain.x86_64", "data.pck"};
		CHECK(pickLauncher(files, "Risk Of Rain", "linux") == "RiskOfRain.x86_64",
		      "linux: matches installdir ignoring spaces");
	}
	{

		std::vector<std::string> files = {
		    "OneShot.app\\Contents\\MacOS\\oneshot",
		    "OneShot.app\\Contents\\Info.plist"};
		CHECK(pickLauncher(files, "OneShot", "macos") == "OneShot.app",
		      "macos: recovers the .app bundle path");
	}
	{

		std::vector<std::string> files = {
		    "oneshot.exe", "steamshim.exe", "_______.exe"};
		CHECK(pickLauncher(files, "OneShot", "windows") == "steamshim.exe",
		      "windows: steamshim wrapper wins over the installdir-named game exe");
	}
	{

		std::vector<std::string> files = {"oneshot", "steamshim", "Data"};
		CHECK(pickLauncher(files, "OneShot", "linux") == "steamshim",
		      "linux: native steamshim wrapper wins over the game binary");
	}
	{

		std::vector<std::string> files = {"oneshot.exe", "helper.exe"};
		CHECK(pickLauncher(files, "OneShot", "windows") == "oneshot.exe",
		      "windows: no shim present -> installdir-named exe still wins");
	}
	CHECK(pickLauncher({"sub\\dir\\helper.exe"}, "X", "windows").empty(),
	      "no root-level launcher -> empty");
	CHECK(pickLauncher({}, "X", "linux").empty(), "no files -> empty");

	{
		YAML::Node body(YAML::NodeType::Map);
		body["common"]["name"] = "OneShot";
		body["config"]["installdir"] = "OneShot";
		const int n = ensureLaunchEntries(body, {
		    {"oneshot", "linux"},
		    {"oneshot.exe", "windows"},
		    {"OneShot.app", "macos"}});
		CHECK(n == 3, "three launch entries added (one per OS)");
		auto launch = body["config"]["launch"];
		CHECK(launch["0"]["executable"].as<std::string>() == "oneshot" &&
		      launch["0"]["config"]["oslist"].as<std::string>() == "linux",
		      "launch.0 = linux launcher");
		CHECK(launch["1"]["config"]["oslist"].as<std::string>() == "windows",
		      "launch.1 = windows launcher");
		CHECK(launch["2"]["config"]["oslist"].as<std::string>() == "macos",
		      "launch.2 = macos launcher");
	}
	{

		YAML::Node body(YAML::NodeType::Map);
		const int n = ensureLaunchEntries(body, {{"", "windows"}, {"game", "linux"}});
		CHECK(n == 1, "empty executable skipped, one entry added");
		CHECK(body["config"]["launch"]["0"]["executable"].as<std::string>() == "game",
		      "the non-empty entry lands at index 0");
	}
	{

		YAML::Node body(YAML::NodeType::Map);
		body["config"]["launch"]["0"]["executable"] = "Real.exe";
		const int n = ensureLaunchEntries(body, {{"Synth.exe", "windows"}});
		CHECK(n == 0, "no-op when launch already present");
		CHECK(body["config"]["launch"]["0"]["executable"].as<std::string>() == "Real.exe",
		      "existing launch untouched");
	}

	if (g_failures == 0) std::printf("\nAll manifestsynth tests passed.\n");
	else                 std::printf("\n%d manifestsynth test(s) FAILED.\n", g_failures);
	return g_failures == 0 ? 0 : 1;
}

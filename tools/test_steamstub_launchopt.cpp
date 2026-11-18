

#include "../src/feats/steamstub_launchopt.hpp"

#include <cstdio>
#include <string>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

using SteamlessLaunchOpt::parseAppLaunchOptions;
using SteamlessLaunchOpt::requestsSteamless;

static std::string localConfig()
{
	return
		"\"UserLocalConfigStore\"\n{\n"
		"\t\"Software\"\n\t{\n\t\t\"Valve\"\n\t\t{\n\t\t\t\"Steam\"\n\t\t\t{\n"
		"\t\t\t\t\"apps\"\n\t\t\t\t{\n"
		"\t\t\t\t\t\"250900\"\n\t\t\t\t\t{\n"
		"\t\t\t\t\t\t\"LastPlayed\"\t\t\"1700000000\"\n"
		"\t\t\t\t\t\t\"LaunchOptions\"\t\t\"game-performance mangohud %command%\"\n"
		"\t\t\t\t\t}\n"
		"\t\t\t\t\t\"1868140\"\n\t\t\t\t\t{\n"
		"\t\t\t\t\t\t\"LaunchOptions\"\t\t\"--steamless %command%\"\n"
		"\t\t\t\t\t}\n"
		"\t\t\t\t\t\"620\"\n\t\t\t\t\t{\n"
		"\t\t\t\t\t\t\"LaunchOptions\"\t\t\"\"\n"
		"\t\t\t\t\t}\n"
		"\t\t\t\t}\n"
		"\t\t\t}\n\t\t}\n\t}\n"

		"\t\"friends\"\n\t{\n"
		"\t\t\"250900\"\t\t\"00696e7465726e616c00 --steamless\"\n"
		"\t}\n"
		"}\n";
}

int main()
{
	const std::string cfg = localConfig();

	CHECK(parseAppLaunchOptions(cfg, 250900) == "game-performance mangohud %command%",
	      "parse: apps-scoped LaunchOptions for 250900");
	CHECK(parseAppLaunchOptions(cfg, 1868140) == "--steamless %command%",
	      "parse: apps-scoped LaunchOptions for 1868140");

	CHECK(parseAppLaunchOptions(cfg, 620).empty(),
	      "parse: empty LaunchOptions -> empty");

	CHECK(parseAppLaunchOptions(cfg, 250900).find("--steamless") == std::string::npos,
	      "parse: does not leak the out-of-apps blob value");

	CHECK(parseAppLaunchOptions(cfg, 999999).empty(),
	      "parse: absent appid -> empty");

	CHECK(parseAppLaunchOptions(cfg, 2509000).empty(),
	      "parse: superstring appid (2509000) not matched");

	CHECK(parseAppLaunchOptions("\"x\"\n{\n}\n", 250900).empty(),
	      "parse: no apps section -> empty");
	CHECK(parseAppLaunchOptions("", 250900).empty(),
	      "parse: empty input -> empty");

	{
		const std::string cfg2 =
			"\"apps\"\n{\n\t\"10\"\n\t{\n"
			"\t\t\"LaunchOptions\"\t\t\"WINEDLLOVERRIDES=\\\"steam_api64=n,b\\\" %command% -steamless\"\n"
			"\t}\n}\n";
		const std::string v = parseAppLaunchOptions(cfg2, 10);
		CHECK(v == "WINEDLLOVERRIDES=\"steam_api64=n,b\" %command% -steamless",
		      "parse: unescapes \\\" in the value");
		CHECK(requestsSteamless(v), "parse+token: -steamless after escaped value is detected");
	}

	CHECK(requestsSteamless("--steamless"),           "token: bare --steamless");
	CHECK(requestsSteamless("-steamless"),            "token: bare -steamless");
	CHECK(requestsSteamless("%command% --steamless"), "token: --steamless at end");
	CHECK(requestsSteamless("--steamless %command%"), "token: --steamless at start-ish");
	CHECK(requestsSteamless("mangohud\t--steamless"), "token: tab-separated");
	CHECK(!requestsSteamless(""),                     "token: empty -> false");
	CHECK(!requestsSteamless("game-performance %command%"), "token: unrelated opts -> false");
	CHECK(!requestsSteamless("/opt/no-steamlessness"), "token: substring not matched");
	CHECK(!requestsSteamless("--steamlessX"),         "token: trailing junk not matched");
	CHECK(!requestsSteamless("x--steamlessy"),        "token: embedded not matched");

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}

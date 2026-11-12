// SPDX-License-Identifier: AGPL-3.0-only
//
// Host test for the pure stplug-in parser. Every case here is a bug that shipped
// at some point, or a format the archives actually use.

#include "../src/feats/stplugin_parse.hpp"

#include <cstdio>
#include <string>

static int failures = 0;

#define CHECK(cond)                                                       \
	do                                                                    \
	{                                                                     \
		if (!(cond))                                                      \
		{                                                                 \
			std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);   \
			++failures;                                                   \
		}                                                                 \
	} while (0)

int main()
{
	// Bare addappid lines, plus the filename as the app id.
	{
		StPluginParse::Script s;
		StPluginParse::parse("addappid(1245620)\naddappid(1799420)\n", "1245620", s);
		CHECK(s.scope == 1245620);
		CHECK(s.appIds.count(1245620) == 1);
		CHECK(s.appIds.count(1799420) == 1);
		CHECK(s.appIds.size() == 2);
	}

	// Three-argument setManifestid carrying the size (Hubcap archives).
	{
		StPluginParse::Script s;
		StPluginParse::parse("setManifestid(1091501, \"3389237510439361019\", 66366228927)\n", "1091500", s);
		CHECK(s.manifestIds.count(1091501) == 1);
		CHECK(s.manifestIds[1091501] == 3389237510439361019ull);
		CHECK(s.depots.size() == 1);
		CHECK(s.depots[0].size == 66366228927ull);
	}

	// Two-argument form leaves the size unknown.
	{
		StPluginParse::Script s;
		StPluginParse::parse("setManifestid(1222141,\"7324084008489949045\")\n", "1222140", s);
		CHECK(s.depots.size() == 1);
		CHECK(s.depots[0].size == 0);
		CHECK(s.depots[0].manifestId == 7324084008489949045ull);
	}

	// Commented-out and trailing-comment lines.
	{
		StPluginParse::Script s;
		StPluginParse::parse("--addappid(999)\naddappid(1000) -- trailing\n", "1000", s);
		CHECK(s.appIds.count(999) == 0);
		CHECK(s.appIds.count(1000) == 1);
	}

	// CRLF line endings.
	{
		StPluginParse::Script s;
		StPluginParse::parse("addappid(2000)\r\nsetManifestid(2001,\"5\")\r\n", "2000", s);
		CHECK(s.appIds.count(2000) == 1);
		CHECK(s.manifestIds.count(2001) == 1);
	}

	// A depot equal to the app is the main-application key line, not a depot.
	{
		StPluginParse::Script s;
		StPluginParse::parse("addappid(3000,1,\"" + std::string(64, 'a') + "\")\n", "3000", s);
		CHECK(s.depotKeys.count(3000) == 1);
		CHECK(s.depots.empty());
	}

	// Malformed lines are skipped, not fatal.
	{
		StPluginParse::Script s;
		StPluginParse::parse("addappid(\naddappid(abc)\nsetManifestid(1)\n", "4000", s);
		CHECK(s.appIds.size() == 1);
		CHECK(s.depots.empty());
	}

	// A non-numeric filename falls back to the first bare addappid.
	{
		StPluginParse::Script s;
		StPluginParse::parse("addappid(5000)\nsetManifestid(5001,\"7\")\n", "game", s);
		CHECK(s.scope == 5000);
		CHECK(s.depots.size() == 1);
	}

	// Repeated depots collapse, last pin wins.
	{
		StPluginParse::Script s;
		StPluginParse::parse("setManifestid(6001,\"1\")\nsetManifestid(6001,\"2\")\n", "6000", s);
		CHECK(s.depots.size() == 1);
		CHECK(s.depots[0].manifestId == 2);
	}

	if (failures)
	{
		std::printf("test-stplugin: %d failure(s)\n", failures);
		return 1;
	}

	std::printf("test-stplugin: all checks passed\n");
	return 0;
}

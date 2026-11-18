

#include "../src/feats/playerstats.hpp"

#include <cstdio>
#include <cstdint>
#include <vector>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

using namespace PlayerStats;

static void putVarint(std::vector<uint8_t>& out, uint64_t v)
{
	while (v >= 0x80) { out.push_back(static_cast<uint8_t>(v) | 0x80); v >>= 7; }
	out.push_back(static_cast<uint8_t>(v));
}

int main()
{

	{
		std::vector<uint8_t> body;
		body.push_back(0x08); putVarint(body, 76561197960265728ULL);
		body.push_back(0x10); putVarint(body, 2506160);
		auto appid = parseRequestAppId(body.data(), body.size());
		CHECK(appid.has_value() && *appid == 2506160, "parseRequestAppId reads field 2");
	}

	{
		std::vector<uint8_t> body;
		body.push_back(0x10); putVarint(body, 480);
		body.push_back(0x08); putVarint(body, 123456789ULL);
		auto appid = parseRequestAppId(body.data(), body.size());
		CHECK(appid.has_value() && *appid == 480, "parseRequestAppId order-independent");
	}

	{
		std::vector<uint8_t> body;
		body.push_back(0x08); putVarint(body, 999ULL);
		auto appid = parseRequestAppId(body.data(), body.size());
		CHECK(!appid.has_value(), "parseRequestAppId returns nullopt without appid");
	}

	{
		std::vector<uint8_t> body;
		body.push_back(0x08); putVarint(body, 1ULL);
		body.push_back(0x1a); putVarint(body, 4); body.insert(body.end(), {0xDE,0xAD,0xBE,0xEF});
		body.push_back(0x10); putVarint(body, 730);
		auto appid = parseRequestAppId(body.data(), body.size());
		CHECK(appid.has_value() && *appid == 730, "parseRequestAppId skips bytes field");
	}

	{
		const uint64_t owner = 76561198028121353ULL;
		auto out = buildSpoofedRequest(owner, 2506160);

		auto appid = parseRequestAppId(out.data(), out.size());
		CHECK(appid.has_value() && *appid == 2506160, "buildSpoofedRequest carries appid");
		auto sid = parseRequestSteamId(out.data(), out.size());
		CHECK(sid.has_value() && *sid == owner, "buildSpoofedRequest carries owner steamid");

		CHECK(!hasField(out.data(), out.size(), 3), "spoofed request drops sha_schema");
		CHECK(!hasField(out.data(), out.size(), 4), "spoofed request drops crc_stats");
	}

	{
		std::vector<uint8_t> resp;
		resp.push_back(0x1a); putVarint(resp, 5); resp.insert(resp.end(), {'h','e','l','l','o'});
		resp.push_back(0x22); putVarint(resp, 3); resp.insert(resp.end(), {0x08,0x01,0x02});
		resp.push_back(0x22); putVarint(resp, 2); resp.insert(resp.end(), {0x08,0x07});
		auto stripped = stripResponseStats(resp.data(), resp.size());
		CHECK(hasField(stripped.data(), stripped.size(), 3), "stripResponseStats keeps schema");
		CHECK(!hasField(stripped.data(), stripped.size(), 4), "stripResponseStats drops stats");

		auto schema = fieldBytes(stripped.data(), stripped.size(), 3);
		CHECK(schema.size() == 5 && schema[0] == 'h', "stripResponseStats preserves schema bytes");
	}

	{
		std::vector<uint8_t> resp;
		resp.push_back(0x1a); putVarint(resp, 2); resp.insert(resp.end(), {'h','i'});
		auto stripped = stripResponseStats(resp.data(), resp.size());
		CHECK(hasField(stripped.data(), stripped.size(), 3), "no-stats response keeps schema");
		CHECK(!hasField(stripped.data(), stripped.size(), 4), "no-stats response still has no stats");
	}

	{
		const uint8_t bad[] = { 0x1a, 0x7f };
		auto appid = parseRequestAppId(bad, sizeof(bad));
		CHECK(!appid.has_value(), "truncated input -> no appid, no crash");
		auto stripped = stripResponseStats(bad, sizeof(bad));
		CHECK(stripped.empty() || !hasField(stripped.data(), stripped.size(), 4),
		      "truncated response strip -> no crash");
	}

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}

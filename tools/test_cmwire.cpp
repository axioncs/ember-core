

#include "../src/feats/cmwire.hpp"

#include <cstdio>
#include <cstring>
#include <unordered_map>

static int g_failures = 0;

#define CHECK(cond, msg)                                               \
	do {                                                               \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; } \
		else         { std::printf("ok:   %s\n", msg); }               \
	} while (0)

int main()
{

	{

		const std::string hdr = "HDR", body = "BODY";
		const auto pkt = CmWire::packPacket(820, hdr, body);

		uint32_t emsg = 0;
		std::string outHdr, outBody;
		const bool ok = CmWire::parsePacket(pkt, emsg, outHdr, outBody);
		CHECK(ok, "parse ok");
		CHECK(emsg == 820, "emsg masked off");
		CHECK(outHdr == "HDR", "header slice");
		CHECK(outBody == "BODY", "body slice");

		uint32_t rawEmsg = 0;
		std::memcpy(&rawEmsg, pkt.data(), 4);
		CHECK((rawEmsg & 0x80000000u) != 0, "proto mask set on wire");

		const auto pkt2 = CmWire::packPacket(1, "", "");
		uint32_t e2 = 0; std::string h2 = "x", b2 = "x";
		CHECK(CmWire::parsePacket(pkt2, e2, h2, b2), "empty hdr/body parses");
		CHECK(e2 == 1 && h2.empty() && b2.empty(), "empty hdr/body slices");

		CHECK(!CmWire::parsePacket(std::string("\x01\x02", 2), emsg, outHdr, outBody),
		      "short pkt rejected");

		std::string lying;
		lying.resize(8);
		uint32_t big = 0xFFFFFFu;
		std::memcpy(&lying[4], &big, 4);
		CHECK(!CmWire::parsePacket(lying, emsg, outHdr, outBody),
		      "oversized header length rejected");
	}

	{

		const std::string inner1 = "AAA", inner2 = "BBBB";
		std::string blob;
		const uint32_t l1 = 3, l2 = 4;
		blob.append(reinterpret_cast<const char*>(&l1), 4); blob += inner1;
		blob.append(reinterpret_cast<const char*>(&l2), 4); blob += inner2;

		const auto parts = CmWire::expandMultiBody(blob);
		CHECK(parts.size() == 2, "two inner packets");
		CHECK(parts.size() == 2 && parts[0] == "AAA" && parts[1] == "BBBB",
		      "inner slices");

		CHECK(CmWire::expandMultiBody("").empty(), "empty multi body");

		const auto bad = CmWire::expandMultiBody(std::string("\x05\x00\x00\x00""AB", 6));
		CHECK(bad.empty(), "short inner rejected");
	}

#ifdef CMWIRE_PROTOBUF
	{

		const auto reqBytes = CmWire::buildPicsRequest({3035500u, 250900u});
		CMsgClientPICSProductInfoRequest rt;
		CHECK(rt.ParseFromString(reqBytes), "req parses");
		CHECK(rt.apps_size() == 2, "two apps in request");
		CHECK(rt.apps_size() == 2 && rt.apps(0).appid() == 3035500u &&
		      rt.apps(1).appid() == 250900u, "request appids");
		CHECK(rt.meta_data_only() == false, "meta_data_only false");

		const auto logonBytes = CmWire::buildAnonLogon();
		CMsgClientLogon lt;
		CHECK(lt.ParseFromString(logonBytes), "logon parses");
		CHECK(lt.protocol_version() == 65580u, "logon protocol_version");

		CMsgClientPICSProductInfoResponse resp;
		auto* a = resp.add_apps();
		a->set_appid(3035500u);
		a->set_buffer("X");
		auto* empty = resp.add_apps();
		empty->set_appid(999u);
		resp.set_response_pending(true);
		const std::string respBytes = resp.SerializeAsString();

		std::unordered_map<uint32_t, std::string> out;
		const bool pending = CmWire::parsePicsResponse(respBytes, out);
		CHECK(pending, "response_pending propagated");
		CHECK(out.size() == 1, "only non-empty buffers kept");
		CHECK(out.count(3035500u) && out[3035500u] == "X", "buffer mapped by appid");
		CHECK(out.count(999u) == 0, "empty-buffer app skipped");

		CMsgClientPICSProductInfoResponse resp2;
		auto* b = resp2.add_apps();
		b->set_appid(3035500u);
		b->set_buffer("X");
		b->set_change_number(4242u);
		std::unordered_map<uint32_t, std::string> out3;
		std::unordered_map<uint32_t, uint32_t> changes;
		CmWire::parsePicsResponse(resp2.SerializeAsString(), out3, &changes);
		CHECK(changes.count(3035500u) && changes[3035500u] == 4242u,
		      "change_number captured");

		CMsgClientPICSProductInfoResponse done;
		done.set_response_pending(false);
		std::unordered_map<uint32_t, std::string> out2;
		CHECK(!CmWire::parsePicsResponse(done.SerializeAsString(), out2),
		      "no pending when flag unset");
	}
#endif

	if (g_failures == 0) std::printf("\nall cmwire checks passed\n");
	else                 std::printf("\n%d cmwire check(s) FAILED\n", g_failures);
	return g_failures == 0 ? 0 : 1;
}

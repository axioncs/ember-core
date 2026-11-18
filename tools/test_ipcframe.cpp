

#include "../src/feats/ipcframe.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

static int g_failures = 0;
static int g_checks = 0;

#define CHECK(cond, msg)                                                      \
	do {                                                                      \
		++g_checks;                                                           \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }        \
	} while (0)

struct TextSection { std::vector<uint8_t> bytes; uint32_t va = 0; bool ok = false; };

template <class T> static T rd(const std::vector<uint8_t>& d, size_t off)
{
	T v{}; std::memcpy(&v, d.data() + off, sizeof(T)); return v;
}

static TextSection loadText(const std::string& path)
{
	TextSection ts;
	std::ifstream f(path, std::ios::binary);
	if (!f) return ts;
	std::vector<uint8_t> d((std::istreambuf_iterator<char>(f)), {});
	if (d.size() < 0x34 || std::memcmp(d.data(), "\x7f""ELF", 4) != 0) return ts;

	const uint32_t e_shoff   = rd<uint32_t>(d, 0x20);
	const uint16_t e_shentsz = rd<uint16_t>(d, 0x2e);
	const uint16_t e_shnum   = rd<uint16_t>(d, 0x30);
	const uint16_t e_shstrndx= rd<uint16_t>(d, 0x32);

	auto shName   = [&](int i){ return rd<uint32_t>(d, e_shoff + i*e_shentsz + 0x00); };
	auto shAddr   = [&](int i){ return rd<uint32_t>(d, e_shoff + i*e_shentsz + 0x0c); };
	auto shOffset = [&](int i){ return rd<uint32_t>(d, e_shoff + i*e_shentsz + 0x10); };
	auto shSize   = [&](int i){ return rd<uint32_t>(d, e_shoff + i*e_shentsz + 0x14); };

	const uint32_t strOff = shOffset(e_shstrndx);
	for (int i = 0; i < e_shnum; ++i)
	{
		const char* nm = reinterpret_cast<const char*>(d.data() + strOff + shName(i));
		if (std::strcmp(nm, ".text") == 0)
		{
			const uint32_t off = shOffset(i), sz = shSize(i);
			ts.bytes.assign(d.begin() + off, d.begin() + off + sz);
			ts.va = shAddr(i);
			ts.ok = true;
			return ts;
		}
	}
	return ts;
}

struct IfaceCase
{
	const char* name;
	uint32_t seedRoot;
	const uint32_t* pivots;
	size_t pivotCount;
};

static constexpr uint32_t kRemoteStoragePivots[] = {
	0x396376, 0x5DB472, 0x84692E, 0x8694E9, 0x8712DD,
	0xC0F5C7, 0xDB2410, 0xE82BD7, 0xF5841E, 0xFD3CA9,
};
static constexpr uint32_t kUserStatsPivots[] = {
	0x84EDDC, 0x85DE33, 0x85E5D6, 0xF7452C,
	0xF991AC, 0xF9B9E3, 0xFF7DFB, 0xFF94F2,
};
static constexpr size_t kPivotMinMatches = 4;

static const IfaceCase kCases[] = {
	{ "IClientApps",          0xA6889C37, nullptr, 0 },
	{ "IClientAppManager",    0x7A0A85B7, nullptr, 0 },
	{ "IClientRemoteStorage", 0x8712DD65, kRemoteStoragePivots, std::size(kRemoteStoragePivots) },
	{ "IClientUGC",           0x71D20C62, nullptr, 0 },
	{ "IClientUserStats",     0x876D658F, kUserStatsPivots, std::size(kUserStatsPivots) },
};

static void test_resolveConfident_pure()
{
	std::printf("[1] resolveConfident (pure)\n");
	CHECK(IpcFrame::resolveConfident({}, 0x1234, 0x100) == SIZE_MAX, "empty -> SIZE_MAX");

	std::vector<IpcFrame::Cand> one = { { 0x10, 0x00001000 }, { 0x20, 0x00009999 } };
	CHECK(IpcFrame::resolveConfident(one, 0x00001005, 0x100) == 0, "single in-band -> its index");

	CHECK(IpcFrame::resolveConfident(one, 0x00001000, 0x100) == 0, "exact match -> index");

	std::vector<IpcFrame::Cand> two = { { 0x10, 0x00001000 }, { 0x20, 0x00001040 } };
	CHECK(IpcFrame::resolveConfident(two, 0x00001010, 0x100) == SIZE_MAX, "two in-band -> ambiguous -> SIZE_MAX");

	std::vector<IpcFrame::Cand> far = { { 0x10, 0x00009999 } };
	CHECK(IpcFrame::resolveConfident(far, 0x00001000, 0x100) == SIZE_MAX, "out-of-band nearest -> SIZE_MAX");
}

static void test_catalog_scan_gate()
{
	std::printf("[1a] exact catalog gates the fallback scan (pure)\n");
	int scans = 0;
	const std::array<bool, 6> complete = { true, true, true, true, true, true };
	CHECK(!IpcFrame::scanWhenCatalogIncomplete(complete, [&]{ ++scans; }),
	      "complete catalog -> fallback scan not needed");
	CHECK(scans == 0, "complete catalog -> scan callback never invoked");

	const std::array<bool, 6> incomplete = { true, true, false, true, true, true };
	CHECK(IpcFrame::scanWhenCatalogIncomplete(incomplete, [&]{ ++scans; }),
	      "missing catalog entry -> fallback scan needed");
	CHECK(scans == 1, "incomplete catalog -> scan callback invoked exactly once");
}

static void test_pattern_root_roundtrip()
{
	std::printf("[1b] parseTrailingRoot / setTrailingRoot (pure)\n");

	const std::string apps = "E8 ? ? ? ? 8B 85 ? ? ? ? 83 C4 10 3D 37 9C 88 A6";
	CHECK(IpcFrame::parseTrailingRoot(apps) == 0xA6889C37, "parse Apps root LE");

	std::string p = apps;
	IpcFrame::setTrailingRoot(p, 0x71D20C62);
	CHECK(IpcFrame::parseTrailingRoot(p) == 0x71D20C62, "set then parse round-trips");
	CHECK(p.size() >= 11 && p.substr(p.size() - 11) == "62 0C D2 71",
	      "trailing bytes rewritten little-endian, uppercase");

	CHECK(p.rfind("E8 ? ? ? ? 8B 85 ? ? ? ? 83 C4 10 3D ", 0) == 0, "prefix preserved");
}

static void test_scan_synthetic()
{
	std::printf("[2] scan (synthetic buffer)\n");

	std::vector<uint8_t> buf = {
		0x90, 0x90,
		0xE8, 0x11, 0x22, 0x33, 0x44,
		0x8B, 0x85, 0x01, 0x02, 0x03, 0x04,
		0x83, 0xC4, 0x10,
		0x3D, 0xEF, 0xBE, 0xAD, 0xDE,
		0x90, 0x90,
	};
	auto cs = IpcFrame::scan(buf.data(), buf.size());
	CHECK(cs.size() == 1, "exactly one candidate found");
	CHECK(cs.size() == 1 && cs[0].offset == 2, "candidate at the call opcode");
	CHECK(cs.size() == 1 && cs[0].root == 0xDEADBEEF, "root read little-endian");

	std::vector<uint8_t> empty(8, 0x90);
	CHECK(IpcFrame::scan(empty.data(), empty.size()).empty(), "no candidate in noise");
}

static void test_highpivots_synthetic()
{
	std::printf("[2a] high-24 pivot count (synthetic buffer)\n");

	std::vector<uint8_t> buf(0x200, 0x90);
	size_t at = 0x20;
	for (uint32_t pivot : kRemoteStoragePivots)
	{
		const uint32_t live = (pivot << 8) | 0x7C;
		buf[at++] = 0x3D;
		std::memcpy(buf.data() + at, &live, sizeof(live));
		at += sizeof(live) + 3;
	}
	CHECK(IpcFrame::countHighPivots(
		buf.data(), buf.size(), kRemoteStoragePivots,
		std::size(kRemoteStoragePivots)) == std::size(kRemoteStoragePivots),
		"all pivots present despite low-byte drift -> full count");

	static constexpr uint32_t alien[] = { 0x123456 };
	CHECK(IpcFrame::countHighPivots(buf.data(), buf.size(), alien, 1) == 0,
		"unrelated pivot -> not counted");

	buf[0x20] = 0x90;
	CHECK(IpcFrame::countHighPivots(
		buf.data(), buf.size(), kRemoteStoragePivots,
		std::size(kRemoteStoragePivots)) == std::size(kRemoteStoragePivots) - 1,
		"removing one pivot lowers the count by one");
}

static std::vector<IpcFrame::Cand> scanNaive(const uint8_t* code, size_t size)
{
	static const int16_t mask[15] = {
		0xE8, -1, -1, -1, -1, 0x8B, 0x85, -1, -1, -1, -1, 0x83, 0xC4, 0x10, 0x3D };
	std::vector<IpcFrame::Cand> out;
	if (size < 19) return out;
	for (size_t i = 0; i + 19 <= size; ++i)
	{
		bool ok = true;
		for (size_t k = 0; k < 15; ++k)
			if (mask[k] != -1 && code[i + k] != mask[k]) { ok = false; break; }
		if (!ok) continue;
		uint32_t root = (uint32_t)code[i + 15] | (uint32_t)code[i + 16] << 8
		              | (uint32_t)code[i + 17] << 16 | (uint32_t)code[i + 18] << 24;
		out.push_back({ i, root });
	}
	return out;
}

static bool sameCands(const std::vector<IpcFrame::Cand>& a, const std::vector<IpcFrame::Cand>& b)
{
	if (a.size() != b.size()) return false;
	for (size_t i = 0; i < a.size(); ++i)
		if (a[i].offset != b[i].offset || a[i].root != b[i].root) return false;
	return true;
}

static void test_scan_equivalence()
{
	std::printf("[2b] scan == naive reference (memchr optimisation is behaviour-preserving)\n");

	std::vector<std::vector<uint8_t>> cases;
	cases.push_back({});
	cases.push_back(std::vector<uint8_t>(18, 0xE8));
	{

		std::vector<uint8_t> v = {
			0xE8, 0xE8, 0xE8, 0xE8, 0xE8, 0x8B, 0x85, 0, 0, 0, 0,
			0x83, 0xC4, 0x10, 0x3D, 0x11, 0x22, 0x33, 0x44 };
		cases.push_back(v);
	}
	{

		std::vector<uint8_t> v(40, 0x90);
		v[39] = 0xE8;
		cases.push_back(v);
	}
	for (size_t n = 0; n < cases.size(); ++n)
	{
		const auto& v = cases[n];
		bool eq = sameCands(IpcFrame::scan(v.data(), v.size()), scanNaive(v.data(), v.size()));
		CHECK(eq, (std::string("synthetic case #") + std::to_string(n) + " matches naive").c_str());
	}
}

static void test_binary_robustness(const std::string& path)
{
	std::printf("[3] robustness against the real post-update steamclient.so\n      %s\n", path.c_str());
	TextSection ts = loadText(path);
	if (!ts.ok)
	{
		std::printf("      SKIP: .text not loadable (binary absent on this host)\n");
		return;
	}

	auto cands = IpcFrame::scan(ts.bytes.data(), ts.bytes.size());
	CHECK(cands.size() >= 4, "found the generic RunIPCFrame candidates");

	CHECK(sameCands(cands, scanNaive(ts.bytes.data(), ts.bytes.size())),
	      "optimised scan == naive scan over the real .text");

	std::vector<size_t> chosen;
	for (const auto& tc : kCases)
	{

		size_t idx = IpcFrame::resolveConfident(cands, tc.seedRoot, IpcFrame::kMaxRootDrift);
		if (idx == SIZE_MAX && tc.pivots)
		{
			size_t hits = 0;
			for (size_t i = 0; i < cands.size(); ++i)
			{
				const auto& cand = cands[i];
				if (IpcFrame::countHighPivots(
					ts.bytes.data() + cand.offset, cand.available,
					tc.pivots, tc.pivotCount) >= kPivotMinMatches)
				{
					idx = i;
					++hits;
				}
			}
			if (hits != 1) idx = SIZE_MAX;
		}
		bool resolved = idx != SIZE_MAX;
		CHECK(resolved, (std::string(tc.name) + ": resolves on the current client").c_str());
		if (!resolved) continue;
		chosen.push_back(cands[idx].offset);
	}

	bool distinct = true;
	for (size_t i = 0; i < chosen.size(); ++i)
		for (size_t j = i + 1; j < chosen.size(); ++j)
			if (chosen[i] == chosen[j]) distinct = false;
	CHECK(distinct, "the interfaces resolve to distinct offsets");
}

static std::vector<int16_t> parseMasked(const char* p)
{
	std::vector<int16_t> out;
	char* s = const_cast<char*>(p);
	char* e = s + std::strlen(p);
	while (s < e)
	{
		if (*s == '?')      out.push_back(-1);
		else if (*s != ' ') out.push_back(static_cast<int16_t>(std::strtoul(s, &s, 16)));
		++s;
	}
	return out;
}

static size_t countMatches(const uint8_t* code, size_t size, const char* pat)
{
	auto b = parseMasked(pat);
	if (b.empty() || size < b.size()) return 0;
	size_t n = 0;
	for (size_t i = 0; i + b.size() <= size; ++i)
	{
		bool ok = true;
		for (size_t k = 0; k < b.size(); ++k)
			if (b[k] != -1 && code[i + k] != b[k]) { ok = false; break; }
		if (ok) ++n;
	}
	return n;
}

static const char* kRlckWild =
	"75 ? 83 C4 1C 31 C0 5B 5E 5F 5D C3 ? ? ? ? ? 8B 44 24 ? 83 C4 1C 89 F9 89 F2 5B 5E 5F 5D 2D ? ? 00 00";
static const char* kRlckOldExact =
	"75 ? 83 C4 1C 31 C0 5B 5E 5F 5D C3 ? ? ? ? ? 8B 44 24 ? 83 C4 1C 89 F9 89 F2 5B 5E 5F 5D 2D D8 18 00 00";

static const char* kNluWild =
	"55 89 E5 57 56 53 E8 ? ? ? ? 81 C3 ? ? ? ? 81 EC BC 01 00 00 8B 45 08 8B ? ? 1B 00 00 89 9D 54 FE FF FF 85 ? 0F 8E ? ? ? ? 8D 83 ? ? ? ? 83 EC 08 6A 7D 50";
static const char* kNluOldExact =
	"55 89 E5 57 56 53 E8 ? ? ? ? 81 C3 ? ? ? ? 81 EC BC 01 00 00 8B 45 08 8B B8 18 1B 00 00 89 9D 54 FE FF FF 85 FF 0F 8E ? ? ? ? 8D 83 ? ? ? ? 83 EC 08 6A 7D 50";

static void test_offset_patterns(const std::string& newPath, const std::string& oldPath)
{
	std::printf("[4] offset patterns self-heal via wildcarded offset (RequiresLegacyCDKey, NotifyLicensesUpdated)\n");
	TextSection nw = loadText(newPath);
	TextSection od = loadText(oldPath);
	if (!nw.ok || !od.ok)
	{
		std::printf("      SKIP: need both pre- and post-update steamclient.so\n"
		            "      NEW=%s (%s)  OLD=%s (%s)\n",
		            newPath.c_str(), nw.ok ? "ok" : "absent",
		            oldPath.c_str(), od.ok ? "ok" : "absent");
		return;
	}

	CHECK(countMatches(nw.bytes.data(), nw.bytes.size(), kRlckWild) == 1, "RLCK wildcard: 1 hit on new");
	CHECK(countMatches(od.bytes.data(), od.bytes.size(), kRlckWild) == 1, "RLCK wildcard: 1 hit on old");
	CHECK(countMatches(nw.bytes.data(), nw.bytes.size(), kNluWild) == 1, "NLU wildcard: 1 hit on new");
	CHECK(countMatches(od.bytes.data(), od.bytes.size(), kNluWild) == 1, "NLU wildcard: 1 hit on old");

	CHECK(countMatches(od.bytes.data(), od.bytes.size(), kRlckOldExact) == 1, "RLCK old-exact: matched old");
	CHECK(countMatches(nw.bytes.data(), nw.bytes.size(), kRlckOldExact) == 0, "RLCK old-exact: misses new (drift)");
	CHECK(countMatches(od.bytes.data(), od.bytes.size(), kNluOldExact) == 1, "NLU old-exact: matched old");
	CHECK(countMatches(nw.bytes.data(), nw.bytes.size(), kNluOldExact) == 0, "NLU old-exact: misses new (drift)");
}

static const char* kRequestInternetServerList =
	"C7 04 24 ? ? 00 00 E8 ? ? ? ? 5A 89 45 ? 59 FF B6 ? ? ? ? FF B6 ? ? ? ? FF B6 ? ? ? ? FF B6 ? ? ? ? FF B6 ? ? ? ? 6A 01";

static void test_required_patterns(const std::string& newPath, const std::string& oldPath)
{
	std::printf("[5] refreshed required patterns\n");
	TextSection nw = loadText(newPath);
	TextSection od = loadText(oldPath);
	if (!nw.ok)
	{
		std::printf("      SKIP: current steamclient.so unavailable\n");
		return;
	}

	CHECK(countMatches(nw.bytes.data(), nw.bytes.size(), kRequestInternetServerList) == 1,
	      "RequestInternetServerList: 1 hit on current client");
	if (od.ok)
	{
		CHECK(countMatches(od.bytes.data(), od.bytes.size(), kRequestInternetServerList) == 1,
		      "RequestInternetServerList: allocation-size wildcard matches old client");
	}
}

int main(int argc, char** argv)
{
	test_resolveConfident_pure();
	test_catalog_scan_gate();
	test_pattern_root_roundtrip();
	test_scan_synthetic();
	test_highpivots_synthetic();
	test_scan_equivalence();

	std::string path;
	if (argc > 1) path = argv[1];
	else { const char* home = std::getenv("HOME");
	       path = std::string(home ? home : "") + "/.steam/steam/ubuntu12_32/steamclient.so"; }
	test_binary_robustness(path);

	const char* oldEnv = std::getenv("EMBER_OLD_STEAMCLIENT");
	std::string oldPath = (argc > 2) ? argv[2] : (oldEnv ? oldEnv : "");
	test_offset_patterns(path, oldPath);
	test_required_patterns(path, oldPath);

	if (g_failures == 0) { std::printf("\ntest_ipcframe: ALL PASS (%d checks)\n", g_checks); return 0; }
	std::printf("\ntest_ipcframe: %d/%d CHECK(S) FAILED\n", g_failures, g_checks);
	return 1;
}



#include "../src/feats/reconcilepin.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

using ReconcilePin::detail::findBuilderCallRel32;
using ReconcilePin::detail::findCtxDepotVectorOffset;

static void putCall(std::vector<uint8_t>& buf, std::size_t off,
                    std::uintptr_t base, std::uintptr_t targetAbs,
                    bool cleanup = true)
{
	buf[off] = 0xE8;
	const int32_t rel = static_cast<int32_t>(
	    static_cast<intptr_t>(targetAbs) -
	    static_cast<intptr_t>(base + off + 5));
	std::memcpy(buf.data() + off + 1, &rel, sizeof(rel));
	if (cleanup)
	{
		buf[off + 5] = 0x83;
		buf[off + 6] = 0xC4;
		buf[off + 7] = 0x20;
	}
}

static void putDepotIndex(std::vector<uint8_t>& buf, std::size_t off,
                          uint32_t disp, bool disp32)
{
	const uint8_t head[] = {0x8B, 0x7D, 0x08, 0x89, 0xF0, 0xC1, 0xE0, 0x05, 0x03};
	std::memcpy(buf.data() + off, head, sizeof(head));
	if (disp32)
	{
		buf[off + sizeof(head)] = 0x87;
		std::memcpy(buf.data() + off + sizeof(head) + 1, &disp, sizeof(disp));
	}
	else
	{
		buf[off + sizeof(head)] = 0x47;
		buf[off + sizeof(head) + 1] = static_cast<uint8_t>(disp);
	}
}

int main()
{
	constexpr std::size_t EXP = 0x183;
	constexpr std::size_t WIN = 0x40;
	constexpr std::uintptr_t BASE = 0;

	const auto execOk = [](std::uintptr_t a) { return a >= 0x10000 && a < 0x20000; };

	const std::vector<uint8_t> nop(0x300, 0x90);

	{
		auto b = nop; putCall(b, EXP, BASE, 0x18000);
		const long r = findBuilderCallRel32(b.data(), b.size(), BASE, EXP, WIN, execOk);
		CHECK(r == static_cast<long>(EXP),
		      "exact RE'd offset with executable target is used");
	}

	{
		auto b = nop; b[EXP] = 0xFF; putCall(b, EXP + 9, BASE, 0x18500);
		const long r = findBuilderCallRel32(b.data(), b.size(), BASE, EXP, WIN, execOk);
		CHECK(r == static_cast<long>(EXP + 9),
		      "a unique drifted call in the window is recovered");
	}

	{
		auto b = nop; b[EXP] = 0xFF;
		putCall(b, EXP + 9, BASE, 0x18500);
		putCall(b, EXP + 22, BASE, 0x18600);
		const long r = findBuilderCallRel32(b.data(), b.size(), BASE, EXP, WIN, execOk);
		CHECK(r == -1, "two candidate calls in the window are rejected as ambiguous");
	}

	{
		auto b = nop; b[EXP] = 0xFF; putCall(b, EXP + 9, BASE, 0x99999);
		const long r = findBuilderCallRel32(b.data(), b.size(), BASE, EXP, WIN, execOk);
		CHECK(r == -1, "an E8 whose target is not executable is not accepted");
	}

	{
		auto b = nop; b[EXP] = 0xFF;
		const long r = findBuilderCallRel32(b.data(), b.size(), BASE, EXP, WIN, execOk);
		CHECK(r == -1, "no call in the window leaves the fix disabled");
	}

	{
		auto b = nop; putCall(b, EXP, BASE, 0x18000); putCall(b, EXP + 16, BASE, 0x18700);
		const long r = findBuilderCallRel32(b.data(), b.size(), BASE, EXP, WIN, execOk);
		CHECK(r == static_cast<long>(EXP),
		      "the exact RE'd offset wins over a nearby call");
	}

	{
		auto b = nop; b[EXP] = 0xFF;
		putCall(b, EXP + 3, BASE, 0x18500);
		putCall(b, EXP + 0x33, BASE, 0x18600, false);
		const long r = findBuilderCallRel32(b.data(), b.size(), BASE, EXP, WIN, execOk);
		CHECK(r == static_cast<long>(EXP + 3),
		      "a call without the builder's 8-argument cleanup is not a candidate");
	}

	{
		auto b = nop; putCall(b, EXP, BASE, 0x18000, false);
		const long r = findBuilderCallRel32(b.data(), b.size(), BASE, EXP, WIN, execOk);
		CHECK(r == -1, "the exact offset without the 8-argument cleanup is rejected");
	}

	{
		auto b = nop; putDepotIndex(b, 0x25b - 5, 0x78, false);
		CHECK(findCtxDepotVectorOffset(b.data(), b.size()) == 0x78,
		      "the disp8 depot-vector index decodes the context offset");
	}

	{
		auto b = nop; putDepotIndex(b, 0x263 - 5, 0x90, true);
		CHECK(findCtxDepotVectorOffset(b.data(), b.size()) == 0x90,
		      "the disp32 depot-vector index decodes the context offset");
	}

	{
		CHECK(findCtxDepotVectorOffset(nop.data(), nop.size()) == -1,
		      "no depot-vector index leaves the context patch disabled");
		auto b = nop;
		putDepotIndex(b, 0x40, 0x78, false);
		putDepotIndex(b, 0x80, 0x90, true);
		CHECK(findCtxDepotVectorOffset(b.data(), b.size()) == -1,
		      "two depot-vector index loads are rejected as ambiguous");
	}

	{
		auto b = nop; putDepotIndex(b, 0x10, 0x90, true);
		CHECK(findCtxDepotVectorOffset(b.data(), 0x10 + 11) == -1,
		      "a truncated disp32 index is not decoded");
	}

	{
		auto b = nop; putDepotIndex(b, 0x10, 0x7A, false);
		CHECK(findCtxDepotVectorOffset(b.data(), b.size()) == -1,
		      "a misaligned context offset is rejected");
		auto c = nop; putDepotIndex(c, 0x10, 0x20000, true);
		CHECK(findCtxDepotVectorOffset(c.data(), c.size()) == -1,
		      "an out-of-range context offset is rejected");
	}

	if (g_failures == 0) std::printf("\nall reconcilepin-scan checks passed\n");
	else                 std::printf("\n%d reconcilepin-scan check(s) FAILED\n", g_failures);
	return g_failures == 0 ? 0 : 1;
}

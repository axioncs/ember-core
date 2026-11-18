

#include "reconcilepin.hpp"

#include "../config.hpp"
#include "../globals.hpp"
#include "../log.hpp"
#include "../patterns.hpp"

#include "depotkey.hpp"
#include "manageddepotfilter.hpp"
#include "manifeststore.hpp"

#include "libmem/libmem.h"

#include <atomic>
#include <climits>
#include <cstdint>
#include <cstdlib>

namespace
{

#if defined(__x86_64__)
	typedef void* (*ReconFn_t)(void* ctx, void* a1, void* a2);
#else
	typedef void* (__attribute__((regparm(1))) *ReconFn_t)(
	    void* mgr, void* ctx, void* a1, void* a2);
#endif

	ReconFn_t    g_orig = nullptr;
	lm_address_t g_addr = LM_ADDRESS_BAD;
	lm_address_t g_tramp = LM_ADDRESS_BAD;
	lm_size_t    g_size = 0;

	bool g_pinActive = false;
	bool g_trace = false;
	std::atomic<int> g_traceBudget{400};

	constexpr size_t kCtxFlagsOff = 0x04;
	constexpr size_t kCtxAppIdOff = 0x08;

	size_t g_ctxDepotVecOff = 0;
	constexpr size_t kCtxVecCountOff = 0x0c;

	constexpr uint32_t kFlagTargetSide = 0x8;

	constexpr size_t kDepotEntryStride = ManagedDepotFilter::kDepotEntryStride;
	constexpr size_t kDepotEntryGidOff = 0x08;
	constexpr size_t kDepotEntrySizeOff = ManagedDepotFilter::kDepotSizeOff;
	constexpr int32_t kMaxDepots = 512;

	bool applyPinnedEntry(const char* site, uint32_t appId, uint8_t* entry,
	                      uint32_t depotId, uint64_t pin)
	{
		auto* const gidp =
		    reinterpret_cast<uint64_t*>(entry + kDepotEntryGidOff);
		auto* const sizep =
		    reinterpret_cast<uint64_t*>(entry + kDepotEntrySizeOff);
		const auto pinSize = ManifestStore::installedSize(depotId, pin);
		if (!pinSize)
		{
			g_pLog->debugOnce(
			    "ReconcilePin[%s]: app=%u depot=%u pinned gid=%llu has no "
			    "known size; leaving target gid=%llu size=%llu\n",
			    site, appId, depotId, static_cast<unsigned long long>(pin),
			    static_cast<unsigned long long>(*gidp),
			    static_cast<unsigned long long>(*sizep));
			return false;
		}
		if (*gidp != pin || *sizep != *pinSize)
		{
			g_pLog->info(
			    "ReconcilePin[%s]: app=%u depot=%u target gid=%llu size=%llu "
			    "-> pinned gid=%llu size=%llu\n",
			    site, appId, depotId, static_cast<unsigned long long>(*gidp),
			    static_cast<unsigned long long>(*sizep),
			    static_cast<unsigned long long>(pin),
			    static_cast<unsigned long long>(*pinSize));
		}
		*gidp = pin;
		*sizep = *pinSize;
		return true;
	}

	void traceLog(uint32_t appId, uint32_t flags, void* base, int32_t count)
	{
		if (!g_trace) return;
		const bool added = g_config.isAddedAppId(appId);
		const bool locked = g_config.isAppLocked(appId);

		if (!locked && g_traceBudget.fetch_sub(1) <= 0) return;
		g_pLog->info("ReconcilePin[trace]: app=%u flags=0x%x added=%d locked=%d "
		             "side=%s depots@%p count=%d\n",
		             appId, flags, static_cast<int>(added),
		             static_cast<int>(locked),
		             (flags & kFlagTargetSide) ? "target" : "active",
		             base, count);
		if (base && count > 0 && count <= kMaxDepots)
		{
			const auto* e = reinterpret_cast<const uint8_t*>(base);
			for (int32_t i = 0; i < count; ++i, e += kDepotEntryStride)
			{
				g_pLog->info("ReconcilePin[trace]:   depot=%u gid=%llu pin=%llu\n",
				    *reinterpret_cast<const uint32_t*>(e),
				    static_cast<unsigned long long>(
				        *reinterpret_cast<const uint64_t*>(e + kDepotEntryGidOff)),
				    static_cast<unsigned long long>(
				        g_config.getManifestPin(appId,
				            *reinterpret_cast<const uint32_t*>(e))));
			}
		}
	}

	int32_t filterEmptyManaged(uint8_t* base, int32_t count,
	                           uint32_t appId, const char* side)
	{
		return ManagedDepotFilter::compactEmptyManaged(
		    base, count,
		    [](uint32_t depotId) { return DepotKey::isManagedDepot(depotId); },
		    [appId, side](uint32_t depotId)
		    {
			    g_pLog->info(
			        "ReconcilePin: app=%u %s dropping empty managed depot=%u "
			        "from target\n",
			        appId, side, depotId);
		    });
	}

	void patchTargetCtx(void* ctxv, uint32_t appId, uint32_t flags)
	{
		if (!(flags & kFlagTargetSide)) return;
		if (g_ctxDepotVecOff == 0) return;

		auto* ctx = reinterpret_cast<uint8_t*>(ctxv);
		auto* base = *reinterpret_cast<uint8_t* const*>(ctx + g_ctxDepotVecOff);
		auto* const countPtr = reinterpret_cast<int32_t*>(
		    ctx + g_ctxDepotVecOff + kCtxVecCountOff);
		int32_t count = *countPtr;
		if (!base || count <= 0 || count > kMaxDepots) return;

		count = filterEmptyManaged(base, count, appId, "target-ctx");
		*countPtr = count;

		if (!g_pinActive) return;

		auto* e = base;
		for (int32_t i = 0; i < count; ++i, e += kDepotEntryStride)
		{
			const uint32_t depotId = *reinterpret_cast<const uint32_t*>(e);
			const uint64_t pin = g_config.getManifestPin(appId, depotId);
			if (!pin) continue;
			applyPinnedEntry("target-ctx", appId, e, depotId, pin);
		}
	}

	constexpr size_t kVecBaseOff = 0x00;
	constexpr size_t kVecCountOff = 0x0c;

	bool executableAddress(lm_address_t address)
	{
		lm_segment_t segment{};
		return address != 0 && address != LM_ADDRESS_BAD &&
			LM_FindSegment(address, &segment) &&
			(segment.prot & LM_PROT_XR) == LM_PROT_XR &&
			address >= segment.base && address < segment.end;
	}

	using BuildTargetFn_t = void* (*)(void*, uint32_t, void*, void*, void*,
	                                  void*, void*, void*);
	BuildTargetFn_t g_origBuild = nullptr;
	lm_address_t    g_buildAddr = LM_ADDRESS_BAD;
	lm_size_t       g_buildSize = 0;
	uint8_t         g_buildCall[5]{};

	void patchTargetVec(void* vecv, uint32_t appId)
	{
		if (!vecv) return;
		auto* vec = reinterpret_cast<uint8_t*>(vecv);
		auto* base = *reinterpret_cast<uint8_t* const*>(vec + kVecBaseOff);
		auto* const countPtr = reinterpret_cast<int32_t*>(vec + kVecCountOff);
		int32_t count = *countPtr;
		if (!base || count <= 0 || count > kMaxDepots) return;

		count = filterEmptyManaged(base, count, appId, "target-local");
		*countPtr = count;

		if (!g_pinActive) return;

		auto* e = base;
		for (int32_t i = 0; i < count; ++i, e += kDepotEntryStride)
		{
			const uint32_t depotId = *reinterpret_cast<const uint32_t*>(e);
			const uint64_t pin = g_config.getManifestPin(appId, depotId);
			if (!pin) continue;
			applyPinnedEntry("target-local", appId, e, depotId, pin);
		}
	}

	__attribute__((noinline))
	void* hkBuildTarget(void* a0, uint32_t appId, void* a2, void* targetVec,
	                    void* a4, void* ctx, void* a6, void* a7)
	{
		void* r = g_origBuild(a0, appId, a2, targetVec, a4, ctx, a6, a7);
		if (targetVec) patchTargetVec(targetVec, appId);
		return r;
	}

	bool installBuildTargetHook(lm_address_t evalAddr)
	{
		const auto* fn = reinterpret_cast<const uint8_t*>(evalAddr);
		const size_t scanLen = ReconcilePin::detail::kBuilderCallOff
		                     + ReconcilePin::detail::kBuilderCallWindow + 8;

		const long callOff = ReconcilePin::detail::findBuilderCallRel32(
		    fn, scanLen, reinterpret_cast<uintptr_t>(fn),
		    ReconcilePin::detail::kBuilderCallOff,
		    ReconcilePin::detail::kBuilderCallWindow,
		    [](uintptr_t target) {
			    return executableAddress(static_cast<lm_address_t>(target));
		    });
		if (callOff < 0)
		{
			g_pLog->warn("ReconcilePin: target-vector builder call not located "
			             "near +0x%zx (byte 0x%02x); target-local fix disabled\n",
			             ReconcilePin::detail::kBuilderCallOff,
			             fn[ReconcilePin::detail::kBuilderCallOff]);
			return false;
		}
		const auto* site = fn + callOff;
		int32_t rel = 0;
		__builtin_memcpy(&rel, site + 1, sizeof(rel));
		const lm_address_t builderAddr = reinterpret_cast<lm_address_t>(
		    const_cast<uint8_t*>(site) + 5 + rel);

		const intptr_t hookRel = reinterpret_cast<intptr_t>(&hkBuildTarget)
		    - (reinterpret_cast<intptr_t>(site) + 5);
		if (hookRel < INT32_MIN || hookRel > INT32_MAX)
		{
			g_pLog->warn("%s", "ReconcilePin: target-vector call redirect is out of "
			                    "rel32 range; target-local fix disabled\n");
			return false;
		}

		uint8_t replacement[5] = {0xE8, 0, 0, 0, 0};
		const int32_t hookRel32 = static_cast<int32_t>(hookRel);
		__builtin_memcpy(replacement + 1, &hookRel32, sizeof(hookRel32));
		__builtin_memcpy(g_buildCall, site, sizeof(g_buildCall));
		g_buildAddr = reinterpret_cast<lm_address_t>(const_cast<uint8_t*>(site));

		lm_prot_t oldProt = LM_PROT_NONE;
		if (!LM_ProtMemory(
		        g_buildAddr, sizeof(replacement), LM_PROT_XRW, &oldProt))
		{
			g_pLog->warn("%s", "ReconcilePin: failed to make target-vector call "
			                    "writable; target-local fix disabled\n");
			g_buildAddr = LM_ADDRESS_BAD;
			return false;
		}
		const bool wrote =
		    LM_WriteMemory(g_buildAddr, replacement, sizeof(replacement))
		    == sizeof(replacement);
		LM_ProtMemory(g_buildAddr, sizeof(replacement), oldProt, nullptr);
		if (!wrote)
		{
			g_pLog->warn("%s", "ReconcilePin: failed to redirect target-vector "
			                    "call; target-local fix disabled\n");
			g_buildAddr = LM_ADDRESS_BAD;
			return false;
		}
		g_buildSize = sizeof(replacement);
		g_origBuild = reinterpret_cast<BuildTargetFn_t>(builderAddr);
		g_pLog->info("ReconcilePin: redirected target-vector call at %p "
		             "(builder=%p)\n",
		             reinterpret_cast<void*>(g_buildAddr),
		             reinterpret_cast<void*>(builderAddr));
		return true;
	}

#if defined(__x86_64__)
	void* hkEvaluate(void* ctx, void* a1, void* a2)
#else
	__attribute__((regparm(1)))
	void* hkEvaluate(void* mgr, void* ctx, void* a1, void* a2)
#endif
	{
		if (ctx)
		{
			const auto* c = reinterpret_cast<const uint8_t*>(ctx);
			const uint32_t appId = *reinterpret_cast<const uint32_t*>(c + kCtxAppIdOff);
			const uint32_t flags = *reinterpret_cast<const uint32_t*>(c + kCtxFlagsOff);

			if (g_trace && g_ctxDepotVecOff != 0)
			{
				void* base =
				    *reinterpret_cast<void* const*>(c + g_ctxDepotVecOff);
				const int32_t count = *reinterpret_cast<const int32_t*>(
				    c + g_ctxDepotVecOff + kCtxVecCountOff);
				traceLog(appId, flags, base, count);
			}
			patchTargetCtx(ctx, appId, flags);
		}
#if defined(__x86_64__)
		return g_orig(ctx, a1, a2);
#else
		return g_orig(mgr, ctx, a1, a2);
#endif
	}
}

namespace ReconcilePin
{
	bool setup()
	{

		g_pinActive = true;
		if (const char* e = std::getenv("EMBER_RECONCILE_PIN"))
		{
			g_pinActive = !(e[0] == '0' && e[1] == '\0');
		}
		if (const char* e = std::getenv("EMBER_RECONCILE_TRACE"))
		{
			g_trace = !(e[0] == '0' && e[1] == '\0');
		}
		auto& pat = Patterns::CDepotDownloadMgr::EvaluateConfigChanges;
		if (pat.address == LM_ADDRESS_BAD)
		{
			g_pLog->warn("ReconcilePin: EvaluateConfigChanges pattern not found; "
			             "loop fix disabled\n");
			return false;
		}

		g_ctxDepotVecOff = 0;
		lm_segment_t segment{};
		if (LM_FindSegment(pat.address, &segment) && segment.end > pat.address)
		{
			const size_t readable = static_cast<size_t>(segment.end - pat.address);
			const long vecOff = ReconcilePin::detail::findCtxDepotVectorOffset(
			    reinterpret_cast<const uint8_t*>(pat.address),
			    readable < ReconcilePin::detail::kCtxDepotLoopScanLen
			        ? readable : ReconcilePin::detail::kCtxDepotLoopScanLen);
			if (vecOff > 0) g_ctxDepotVecOff = static_cast<size_t>(vecOff);
		}
		if (g_ctxDepotVecOff == 0)
		{
			g_pLog->warn("%s", "ReconcilePin: context depot vector not located; "
			                    "ctx-vector fix disabled\n");
		}

		g_addr = pat.address;
		g_size = LM_HookCode(g_addr,
		                     reinterpret_cast<lm_address_t>(&hkEvaluate),
		                     &g_tramp);
		if (!g_size || g_tramp == LM_ADDRESS_BAD)
		{
			g_pLog->warn("ReconcilePin: failed to install hook\n");
			g_addr = LM_ADDRESS_BAD;
			g_tramp = LM_ADDRESS_BAD;
			g_size = 0;
			return false;
		}
		g_orig = reinterpret_cast<ReconFn_t>(g_tramp);
		g_pLog->info("ReconcilePin: hooked EvaluateConfigChanges at %p "
		             "(pin=%d empty=1 trace=%d ctx_depots=+0x%zx)\n",
		             reinterpret_cast<void*>(g_addr),
		             static_cast<int>(g_pinActive), static_cast<int>(g_trace),
		             g_ctxDepotVecOff);

		installBuildTargetHook(g_addr);
		return true;
	}

	void remove()
	{
		if (g_buildSize && g_buildAddr != LM_ADDRESS_BAD)
		{
			lm_prot_t oldProt = LM_PROT_NONE;
			if (LM_ProtMemory(g_buildAddr, g_buildSize, LM_PROT_XRW, &oldProt))
			{
				LM_WriteMemory(g_buildAddr, g_buildCall, g_buildSize);
				LM_ProtMemory(g_buildAddr, g_buildSize, oldProt, nullptr);
			}
		}
		g_origBuild = nullptr;
		g_buildAddr = LM_ADDRESS_BAD;
		g_buildSize = 0;

		if (g_size && g_addr != LM_ADDRESS_BAD && g_tramp != LM_ADDRESS_BAD)
		{
			LM_UnhookCode(g_addr, g_tramp, g_size);
		}
		g_orig = nullptr;
		g_addr = LM_ADDRESS_BAD;
		g_tramp = LM_ADDRESS_BAD;
		g_size = 0;
	}
}

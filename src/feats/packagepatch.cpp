

#include "packagepatch.hpp"

#include "../afftrace.hpp"
#include "../config.hpp"
#include "../globals.hpp"
#include "../log.hpp"
#include "../memhlp.hpp"
#include "../patterns.hpp"

#include "../sdk/CPackageInfo.hpp"
#include "../sdk/IClientApps.hpp"
#include "../sdk/CSteamEngine.hpp"
#include "../sdk/CUser.hpp"

#include "depotquarantine.hpp"
#include "depotquarantine_store.hpp"
#include "appinfostate.hpp"
#include "hotreload_package.hpp"
#include "hotreload_capabilities.hpp"
#include "license_refresh_policy.hpp"
#include "libraryremoval.hpp"
#include "stats_policy.hpp"
#include "manifestdonor.hpp"

#include "libmem/libmem.h"

#include <atomic>
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <mutex>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace
{

	using LoadPackage_t = bool(*)(PackageInfo*, uint8_t*, int32_t, void*);

	using CUtlMemoryGrow_t = void*(*)(void* , int );
	using MarkLicenseAsChanged_t = std::int64_t (__attribute__((cdecl)) *)(
		void*, std::uint32_t, bool);
	using ProcessPendingLicenseUpdates_t = bool (__attribute__((cdecl)) *)(void*);

	LoadPackage_t      g_pOrigLoadPackage = nullptr;
	CUtlMemoryGrow_t   g_pCUtlMemoryGrow  = nullptr;
	MarkLicenseAsChanged_t g_pMarkLicenseAsChanged = nullptr;
	ProcessPendingLicenseUpdates_t g_pProcessPendingLicenseUpdates = nullptr;

	lm_address_t g_loadPackageAddr = LM_ADDRESS_BAD;
	lm_address_t g_loadPackageTramp = LM_ADDRESS_BAD;
	lm_size_t    g_loadPackageHookSize = 0;

	std::atomic<PackageInfo*> g_pPackage0{nullptr};

	std::mutex g_seededMutex;
	std::unordered_set<uint32_t> g_seededAppIds;
	std::unordered_set<uint32_t> g_seededDepotIds;

	std::atomic<bool> g_licenseReconciled{false};

	std::atomic<bool> g_package0Injected{false};

	std::vector<uint32_t> g_extraAppIds;

	PackageSnapshot g_desiredSnapshot;
	bool g_hasDesiredSnapshot = false;
	std::uint64_t g_appliedGeneration = 0;
	std::uint64_t g_processedGeneration = 0;
	std::uint64_t g_pendingGeneration = 0;
	std::uint64_t g_deferredLogGeneration = 0;
	bool g_deferredLogValid = false;
	bool g_runtimeRefreshPending = false;
	std::atomic<bool> g_runtimeRefreshPendingHint{false};
	std::unordered_set<std::uint32_t> g_pendingMetadataAppIds;
	HotReloadPackage::AppInfoRequestState g_appInfoRequestState;
	std::uint64_t g_fallbackReconcileGeneration = 0;
	thread_local bool t_processingRuntimeRefresh = false;
	std::atomic<bool> g_runtimeRefreshExecuting{false};

	void reconcileLicensesOnce()
	{
		if (g_licenseReconciled.load(std::memory_order_acquire))
		{
			return;
		}
		if (!g_package0Injected.load(std::memory_order_acquire))
		{

			return;
		}

		CUser* user = getLocalUser();
		if (user == nullptr)
		{

			g_pLog->debugOnce("PackagePatch: license reconcile deferred (no local user yet)\n");
			return;
		}

		bool notified = false;
		{

			auto span = AffTrace::fnSpan(AffTrace::Call::NotifyLicensesUpdated,
			                             AffTrace::Mode::NA);
			notified = user->notifyLicensesUpdated();
		}
		if (notified)
		{
			g_licenseReconciled.store(true, std::memory_order_release);
			g_pLog->info("PackagePatch: broadcast LicensesUpdated_t to reconcile package 0\n");
		}
		else
		{
			g_pLog->warn
			(
				"PackagePatch: NotifyLicensesUpdated unavailable; "
				"cold-cache reconcile skipped (warm cache still works)\n"
			);

			g_licenseReconciled.store(true, std::memory_order_release);
		}
	}

	void reconcileGenerationOnce(std::uint64_t generation)
	{
		bool claimed = false;
		{
			std::lock_guard<std::mutex> lock(g_seededMutex);
			if (generation > g_fallbackReconcileGeneration)
			{
				g_fallbackReconcileGeneration = generation;
				claimed = true;
			}
		}
		if (!claimed) return;
		g_licenseReconciled.store(false, std::memory_order_release);
		reconcileLicensesOnce();
	}

	std::vector<uint32_t> collectDepotsForApps(const std::unordered_set<uint32_t>& appFilter)
	{
		std::vector<uint32_t> out;
		const auto dir = std::filesystem::path(g_config.getDir().c_str()) / "cache";
		if (appFilter.empty() || !std::filesystem::is_directory(dir))
		{
			return out;
		}

		static const std::regex appIdRe("^appId:\\s*(\\d+)\\s*$");
		static const std::regex depotIdRe("^depotId:\\s*(\\d+)\\s*$");

		for (const auto& entry : std::filesystem::directory_iterator(dir))
		{
			if (!entry.is_regular_file()) continue;
			const auto& p = entry.path();
			const auto name = p.filename().string();
			if (name.rfind("depotkey_", 0) != 0 || p.extension() != ".yaml")
			{
				continue;
			}

			std::ifstream f(p);
			if (!f.is_open()) continue;

			uint32_t fileAppId = 0;
			uint32_t fileDepotId = 0;
			std::string line;
			while (std::getline(f, line))
			{
				std::smatch m;
				if (std::regex_match(line, m, appIdRe))
				{
					fileAppId = static_cast<uint32_t>(std::stoul(m[1]));
				}
				else if (std::regex_match(line, m, depotIdRe))
				{
					fileDepotId = static_cast<uint32_t>(std::stoul(m[1]));
				}
			}

			if (fileAppId && fileDepotId && appFilter.count(fileAppId))
			{
				out.push_back(fileDepotId);
			}
		}
		return out;
	}

	struct VectorPlan
	{
		std::unordered_set<std::uint32_t> desired;
		std::set<std::uint32_t> missing;
		std::uint32_t targetSize = 0;
		bool removes = false;
	};

	struct SnapshotApplyResult
	{
		bool available = false;
		bool changed = false;
		bool appAdded = false;
		std::vector<std::uint32_t> appInfoRequestIds;
	};

	enum class RuntimeRefreshResult
	{
		Complete,
		Deferred,
		Busy,
	};

	bool validLiveVector(const CUtlVector<std::uint32_t>& vec) noexcept
	{
		return vec.size <= vec.memory.alloc &&
			(vec.size == 0 || vec.memory.base != nullptr);
	}

	bool buildVectorPlan(
		const CUtlVector<std::uint32_t>& vec,
		const std::unordered_set<std::uint32_t>& seeded,
		const std::vector<std::uint32_t>& desiredValues,
		VectorPlan& plan
	)
	{
		if (!validLiveVector(vec))
			return false;

		plan = {};
		for (const std::uint32_t id : desiredValues)
		{
			if (id != 0)
				plan.desired.insert(id);
		}

		std::uint32_t kept = 0;
		for (std::uint32_t index = 0; index < vec.size; ++index)
		{
			const std::uint32_t id = vec.memory.base[index];
			if (seeded.count(id) != 0 && plan.desired.count(id) == 0)
			{
				plan.removes = true;
				continue;
			}
			++kept;
		}

		plan.missing = HotReloadPackage::missingFromVector(
			vec.memory.base, vec.size, plan.desired);
		const std::size_t target = static_cast<std::size_t>(kept) +
			plan.missing.size();
		if (target > std::numeric_limits<std::uint32_t>::max())
			return false;
		plan.targetSize = static_cast<std::uint32_t>(target);
		return true;
	}

	bool prepareVectorLocked(
		CUtlVector<std::uint32_t>& vec,
		const VectorPlan& plan,
		const char* label
	)
	{
		if (!validLiveVector(vec))
			return false;
		if (vec.memory.alloc >= plan.targetSize)
			return true;
		if (g_pCUtlMemoryGrow == nullptr || plan.targetSize <= vec.size)
			return false;

		const std::uint32_t growBy = plan.targetSize - vec.size;
		if (growBy > static_cast<std::uint32_t>(
				std::numeric_limits<int>::max()))
		{
			return false;
		}
		{
			auto span = AffTrace::fnSpan(
				AffTrace::Call::CutlMemoryGrow, AffTrace::Mode::NA);
			g_pCUtlMemoryGrow(&vec.memory, static_cast<int>(growBy));
		}
		if (!validLiveVector(vec) ||
			vec.memory.alloc < plan.targetSize)
		{
			g_pLog->warn(
				"PackagePatch: %s capacity preparation failed (%u required, %u available)\n",
				label, plan.targetSize, vec.memory.alloc);
			return false;
		}
		return true;
	}

	std::unordered_set<std::uint32_t> buildNextSeeded(
		const std::unordered_set<std::uint32_t>& seeded,
		const VectorPlan& plan)
	{
		std::unordered_set<std::uint32_t> next;
		next.reserve(seeded.size() + plan.missing.size());
		for (const std::uint32_t id : seeded)
		{
			if (plan.desired.count(id) != 0)
				next.insert(id);
		}
		next.insert(plan.missing.begin(), plan.missing.end());
		return next;
	}

	bool applyVectorPlanLocked(
		CUtlVector<std::uint32_t>& vec,
		const std::unordered_set<std::uint32_t>& seeded,
		const VectorPlan& plan
	) noexcept
	{
		const std::uint32_t previousSize = vec.size;
		HotReloadPackage::compactInjected(
			vec.memory.base, vec.size, seeded, plan.desired);

		for (const std::uint32_t id : plan.missing)
			vec.memory.base[vec.size++] = id;

		return plan.removes || !plan.missing.empty() || vec.size != previousSize;
	}

	SnapshotApplyResult applySnapshotLocked(
		PackageInfo* package,
		const PackageSnapshot& snapshot
	)
	{
		SnapshotApplyResult result;
		if (package == nullptr || package->PackageId != 0 ||
			package->Status != EPackageStatus::Available)
		{
			return result;
		}

		const auto excluded = DepotQuarantine::package0Exclusions();
		const auto desiredApps = DepotQuarantineStore::withoutIds(
			snapshot.appIds, excluded);
		const auto desiredDepots = DepotQuarantineStore::withoutIds(
			snapshot.depotIds, excluded);
		VectorPlan appPlan;
		VectorPlan depotPlan;
		if (!buildVectorPlan(package->AppIdVec, g_seededAppIds,
				desiredApps, appPlan) ||
			!buildVectorPlan(package->DepotIdVec, g_seededDepotIds,
				desiredDepots, depotPlan))
		{
			g_pLog->warn(
				"PackagePatch: package 0 vectors are invalid; runtime sync deferred\n");
			return result;
		}
		auto nextSeededApps = buildNextSeeded(g_seededAppIds, appPlan);
		auto nextSeededDepots = buildNextSeeded(g_seededDepotIds, depotPlan);
		auto appInfoRequestIds =
			HotReloadPackage::snapshotAppInfoRequestIdsAfterApply(true, snapshot);
		auto nextPendingMetadata = g_pendingMetadataAppIds;
		if (snapshot.metadataComplete)
		{
			nextPendingMetadata.clear();
		}
		else
		{
			for (auto current = nextPendingMetadata.begin();
				current != nextPendingMetadata.end();)
			{
				if (appPlan.desired.count(*current) == 0)
					current = nextPendingMetadata.erase(current);
				else
					++current;
			}
			nextPendingMetadata.insert(
				snapshot.addedAppIds.begin(), snapshot.addedAppIds.end());
		}

		if (!prepareVectorLocked(package->AppIdVec, appPlan, "AppIdVec") ||
			!prepareVectorLocked(package->DepotIdVec, depotPlan, "DepotIdVec"))
		{
			return result;
		}

		const bool appsChanged = applyVectorPlanLocked(
			package->AppIdVec, g_seededAppIds, appPlan);
		const bool depotsChanged = applyVectorPlanLocked(
			package->DepotIdVec, g_seededDepotIds, depotPlan);
		if (!validLiveVector(package->AppIdVec) ||
			!validLiveVector(package->DepotIdVec))
		{

			g_pLog->warn(
				"PackagePatch: package 0 vectors became invalid during runtime sync\n");
			return result;
		}
		g_seededAppIds.swap(nextSeededApps);
		g_seededDepotIds.swap(nextSeededDepots);
		g_pendingMetadataAppIds.swap(nextPendingMetadata);

		result.available = true;
		result.changed = appsChanged || depotsChanged;
		result.appInfoRequestIds = std::move(appInfoRequestIds);
		result.appAdded = !snapshot.addedAppIds.empty();
		g_appliedGeneration = snapshot.generation;
		if (result.appAdded)
			g_package0Injected.store(true, std::memory_order_release);
		return result;
	}

	void logDeferredOnce(std::uint64_t generation, const char* reason)
	{
		bool shouldLog = false;
		{
			std::lock_guard<std::mutex> lock(g_seededMutex);
			if (!g_deferredLogValid || g_deferredLogGeneration != generation)
			{
				g_deferredLogGeneration = generation;
				g_deferredLogValid = true;
				shouldLog = true;
			}
		}
		if (shouldLog)
		{
			g_pLog->warn(
				"PackagePatch: runtime refresh generation %llu deferred (%s); restart remains available\n",
				static_cast<unsigned long long>(generation), reason);
		}
	}

	void requestLiveAppInfo(
		std::uint64_t generation,
		const std::vector<std::uint32_t>& appIds)
	{
		if (appIds.empty()) return;
		std::vector<std::uint32_t> pending;
		{
			std::lock_guard<std::mutex> lock(g_seededMutex);
			pending = g_appInfoRequestState.reserve(generation, appIds);
		}
		if (pending.empty()) return;

		bool accepted = false;
		if (g_pClientApps == nullptr)
		{
			g_pLog->info(
				"PackagePatch: live appinfo request generation %llu deferred "
				"(client apps unavailable)\n",
				static_cast<unsigned long long>(generation));
		}
		else if (g_pClientApps->requestAppInfoUpdate(pending))
		{
			accepted = true;
			g_pLog->info(
				"PackagePatch: requested live appinfo for %zu app(s) "
				"in generation %llu\n",
				pending.size(), static_cast<unsigned long long>(generation));
		}
		else
		{
			g_pLog->info(
				"PackagePatch: live appinfo request generation %llu not accepted "
				"(client offline or updater unavailable)\n",
				static_cast<unsigned long long>(generation));
		}
		{
			std::lock_guard<std::mutex> lock(g_seededMutex);
			g_appInfoRequestState.finish(generation, pending, accepted);
		}
	}

	RuntimeRefreshResult processRuntimeRefresh(
		std::uint64_t generation,
		bool unresolvedStateSafe,
		bool changed
	)
	{
		using LicenseRefreshPolicy::Action;
		const Action action = LicenseRefreshPolicy::decide(
			g_pMarkLicenseAsChanged != nullptr,
			g_pProcessPendingLicenseUpdates != nullptr,
			unresolvedStateSafe,
			changed);
		if (action == Action::NoChange)
		{
			std::lock_guard<std::mutex> lock(g_seededMutex);
			g_processedGeneration = std::max(g_processedGeneration, generation);
			if (g_runtimeRefreshPending && g_pendingGeneration == generation)
			{
				g_runtimeRefreshPending = false;
				g_pendingGeneration = 0;
			}
			g_runtimeRefreshPendingHint.store(
				g_runtimeRefreshPending, std::memory_order_release);
			return RuntimeRefreshResult::Complete;
		}
		if (action == Action::Defer)
		{
			const char* reason = !unresolvedStateSafe
				? "app metadata is unresolved and the guard is unavailable"
				: "mark/process capability is unavailable";
			logDeferredOnce(generation, reason);
			return RuntimeRefreshResult::Deferred;
		}
		if (t_processingRuntimeRefresh)
			return RuntimeRefreshResult::Busy;
		bool expected = false;
		if (!g_runtimeRefreshExecuting.compare_exchange_strong(
			expected, true, std::memory_order_acq_rel))
			return RuntimeRefreshResult::Busy;
		t_processingRuntimeRefresh = true;
		struct ProcessingGuard
		{
			~ProcessingGuard()
			{
				t_processingRuntimeRefresh = false;
				g_runtimeRefreshExecuting.store(false, std::memory_order_release);
			}
		} processingGuard;

		CUser* const user = getLocalUser();
		if (user == nullptr)
		{
			logDeferredOnce(generation, "the local user is not ready");
			return RuntimeRefreshResult::Deferred;
		}

		{
			auto span = AffTrace::fnSpan(
				AffTrace::Call::MarkLicenseChanged, AffTrace::Mode::NA);
			(void)g_pMarkLicenseAsChanged(user, 0, true);
		}
		bool processed = false;
		{
			auto span = AffTrace::fnSpan(
				AffTrace::Call::ProcessLicenseUpdates, AffTrace::Mode::NA);
			processed = g_pProcessPendingLicenseUpdates(user);
		}
		if (!processed)
		{
			logDeferredOnce(generation, "Steam retained the pending update");
			return RuntimeRefreshResult::Deferred;
		}

		{
			std::lock_guard<std::mutex> lock(g_seededMutex);
			g_processedGeneration = std::max(g_processedGeneration, generation);
			if (g_runtimeRefreshPending && g_pendingGeneration == generation)
			{
				g_runtimeRefreshPending = false;
				g_pendingGeneration = 0;
			}
			g_runtimeRefreshPendingHint.store(
				g_runtimeRefreshPending, std::memory_order_release);
		}
		g_pLog->info(
			"PackagePatch: processed runtime package change generation %llu\n",
			static_cast<unsigned long long>(generation));
		return RuntimeRefreshResult::Complete;
	}

	uint32_t appendToVecLocked(CUtlVector<uint32_t>& vec,
	                           const std::vector<uint32_t>& ids,
	                           std::unordered_set<uint32_t>& seenSet,
	                           const char* vecLabel)
	{
		if (!g_pCUtlMemoryGrow || ids.empty() || !validLiveVector(vec))
		{
			return 0;
		}

		auto missing = HotReloadPackage::missingFromVector(
			vec.memory.base, vec.size, ids);
		missing.erase(0);
		const std::vector<uint32_t> fresh(missing.begin(), missing.end());
		if (fresh.empty())
		{
			return 0;
		}

		const uint32_t oldSize = vec.size;
		const uint32_t toAdd = static_cast<uint32_t>(fresh.size());
		if (toAdd > static_cast<uint32_t>(std::numeric_limits<int>::max()) ||
		    toAdd > UINT32_MAX - oldSize) return 0;
		{

			auto span = AffTrace::fnSpan(AffTrace::Call::CutlMemoryGrow,
			                             AffTrace::Mode::NA);
			g_pCUtlMemoryGrow(&vec.memory, static_cast<int>(toAdd));
		}

		const uint32_t available = vec.memory.alloc;
		if (available < oldSize + toAdd)
		{
			g_pLog->warn
			(
				"PackagePatch: %s grow returned only %u slots for %u + %u; "
				"aborting injection\n",
				vecLabel, available, oldSize, toAdd
			);
			return 0;
		}
		if (!vec.memory.base)
		{
			g_pLog->warn("PackagePatch: %s.memory.base still null after Grow\n", vecLabel);
			return 0;
		}

		for (uint32_t i = 0; i < toAdd; ++i)
		{
			vec.memory.base[oldSize + i] = fresh[i];
			seenSet.insert(fresh[i]);
		}
		vec.size = oldSize + toAdd;
		return toAdd;
	}

	uint32_t injectFullSetLocked(PackageInfo* pPkg, const std::vector<uint32_t>& appIds)
	{
		if (!pPkg || !g_pCUtlMemoryGrow || appIds.empty())
		{
			return 0;
		}

		const auto excluded = DepotQuarantine::package0Exclusions();
		const auto injectable = DepotQuarantineStore::withoutIds(appIds, excluded);
		if (injectable.empty())
		{
			return 0;
		}

		const uint32_t appsAdded = appendToVecLocked(
			pPkg->AppIdVec, injectable, g_seededAppIds, "AppIdVec");

		std::unordered_set<uint32_t> appFilter(injectable.begin(), injectable.end());
		const auto depotIds = DepotQuarantineStore::withoutIds(
			collectDepotsForApps(appFilter), excluded);
		const uint32_t depotsAdded = appendToVecLocked(
			pPkg->DepotIdVec, depotIds, g_seededDepotIds, "DepotIdVec");

		if (appsAdded || depotsAdded)
		{
			g_pLog->info
			(
				"PackagePatch: package 0 now has AppIdVec.size=%u "
				"DepotIdVec.size=%u (added %u apps, %u depots)\n",
				pPkg->AppIdVec.size,
				pPkg->DepotIdVec.size,
				appsAdded,
				depotsAdded
			);
		}
		if (appsAdded)
		{
			g_package0Injected.store(true, std::memory_order_release);
		}
		return appsAdded;
	}

	bool hkLoadPackage(PackageInfo* pInfo, uint8_t* sha1, int32_t cn, void* p4)
	{
		const bool result = g_pOrigLoadPackage(pInfo, sha1, cn, p4);

		if (!pInfo)
		{
			return result;
		}

		g_pLog->debug
		(
			"LoadPackage: PackageId=%u Status=%d AppIdVec.size=%u result=%d\n",
			pInfo->PackageId,
			static_cast<int>(pInfo->Status),
			pInfo->AppIdVec.size,
			result
		);

		if (pInfo->PackageId != 0)
		{
			ManifestDonor::observePackage(
				pInfo, result && pInfo->Status == EPackageStatus::Available);
			return result;
		}

		g_pPackage0.store(pInfo, std::memory_order_release);

		if (pInfo->Status != EPackageStatus::Available)
		{
			g_pLog->debug
			(
				"LoadPackage(PackageId=0): status=%d not Available; "
				"deferring injection\n",
				static_cast<int>(pInfo->Status)
			);
			return result;
		}

		try
		{
			SnapshotApplyResult applied;
			PackageSnapshot desired;
			bool hasDesired = false;
			bool refreshNeeded = false;
			bool unresolvedStateSafe = false;
			bool allowColdFallbackReconcile = false;
			{
				std::lock_guard<std::mutex> lock(g_seededMutex);
				if (g_hasDesiredSnapshot)
				{
					allowColdFallbackReconcile = g_processedGeneration == 0;
					desired = g_desiredSnapshot;
					applied = applySnapshotLocked(pInfo, desired);
					if (applied.available)
					{
						if (applied.changed)
						{
							g_runtimeRefreshPending = true;
							g_runtimeRefreshPendingHint.store(
								true, std::memory_order_release);
							g_pendingGeneration = desired.generation;
						}
						else if (g_runtimeRefreshPending)
						{
							g_pendingGeneration = desired.generation;
						}
						refreshNeeded = g_runtimeRefreshPending;
						unresolvedStateSafe =
							LicenseRefreshPolicy::unresolvedStateSafe(
								!g_pendingMetadataAppIds.empty(),
								AppInfoState::ready());
					}
					hasDesired = true;
				}
			}
			if (hasDesired)
			{
				if (!applied.available)
				{
					{
						std::lock_guard<std::mutex> lock(g_seededMutex);
						g_pendingGeneration = desired.generation;
					}
					logDeferredOnce(
						desired.generation, "package 0 apply did not complete");
					return result;
				}
				if (applied.changed)
				{
					g_pLog->info(
						"PackagePatch: reapplied runtime package state generation %llu\n",
						static_cast<unsigned long long>(desired.generation));
				}

				RuntimeRefreshResult refreshResult = RuntimeRefreshResult::Busy;
				if (!t_processingRuntimeRefresh)
				{
					refreshResult = processRuntimeRefresh(
						desired.generation, unresolvedStateSafe, refreshNeeded);
				}
				if (refreshResult == RuntimeRefreshResult::Complete)
				{
					for (const std::uint32_t appId : desired.addedAppIds)
						LibraryRemoval::restore(appId);
				}
				else if (refreshResult == RuntimeRefreshResult::Deferred &&
					allowColdFallbackReconcile &&
					!t_processingRuntimeRefresh)
				{
					reconcileGenerationOnce(desired.generation);
				}
				if (refreshResult != RuntimeRefreshResult::Busy &&
					!t_processingRuntimeRefresh)
				{
					requestLiveAppInfo(
						desired.generation, applied.appInfoRequestIds);
				}
				return result;
			}
		}
		catch (...)
		{
			g_pLog->warn(
				"PackagePatch: runtime package reapply failed; current Steam state retained\n");
			return result;
		}

		const auto managed = g_config.managedAppIds.get();
		std::vector<uint32_t> ids(managed.begin(), managed.end());
		{

			std::lock_guard<std::mutex> lk(g_seededMutex);
			ids.insert(ids.end(), g_extraAppIds.begin(), g_extraAppIds.end());
		}
		if (ids.empty())
		{
			return result;
		}

		{
			std::lock_guard<std::mutex> lk(g_seededMutex);
			(void)injectFullSetLocked(pInfo, ids);
		}

		reconcileLicensesOnce();
		return result;
	}
}

namespace PackagePatch
{
	bool isInjectedAppId(uint32_t appId)
	{
		std::lock_guard<std::mutex> lock(g_seededMutex);
		return g_seededAppIds.count(appId) != 0;
	}
	bool setup()
	{

		if (Patterns::CPackageInfoCache::LoadPackage.address == LM_ADDRESS_BAD)
		{
			g_pLog->warn("PackagePatch: LoadPackage pattern not found; feature disabled\n");
			return false;
		}
		if (Patterns::CUtlMemory::Grow.address == LM_ADDRESS_BAD)
		{
			g_pLog->warn("PackagePatch: CUtlMemoryGrow pattern not found; feature disabled\n");
			return false;
		}

		g_pCUtlMemoryGrow = reinterpret_cast<CUtlMemoryGrow_t>(
			Patterns::CUtlMemory::Grow.address);
		g_pMarkLicenseAsChanged =
			Patterns::CUser::MarkLicenseAsChanged.address == LM_ADDRESS_BAD
				? nullptr
				: reinterpret_cast<MarkLicenseAsChanged_t>(
					Patterns::CUser::MarkLicenseAsChanged.address);
		g_pProcessPendingLicenseUpdates =
			Patterns::CUser::ProcessPendingLicenseUpdates.address == LM_ADDRESS_BAD
				? nullptr
				: reinterpret_cast<ProcessPendingLicenseUpdates_t>(
					Patterns::CUser::ProcessPendingLicenseUpdates.address);
		if (g_pMarkLicenseAsChanged == nullptr ||
			g_pProcessPendingLicenseUpdates == nullptr)
		{
			g_pLog->warn(
				"PackagePatch: runtime mark/process capability incomplete; boot path remains available\n");
		}

		g_loadPackageAddr  = Patterns::CPackageInfoCache::LoadPackage.address;
		g_loadPackageHookSize = LM_HookCode
		(
			g_loadPackageAddr,
			reinterpret_cast<lm_address_t>(&hkLoadPackage),
			&g_loadPackageTramp
		);
		if (!g_loadPackageHookSize || g_loadPackageTramp == LM_ADDRESS_BAD)
		{
			g_pLog->warn("PackagePatch: failed to install LoadPackage hook\n");
			g_loadPackageHookSize = 0;
			return false;
		}

		MemHlp::fixPICThunkCall("CPackageInfoCache::LoadPackage",
			g_loadPackageAddr, g_loadPackageTramp);

		g_pOrigLoadPackage = reinterpret_cast<LoadPackage_t>(g_loadPackageTramp);

		g_pLog->debug
		(
			"PackagePatch: LoadPackage detour at %p, tramp at %p, CUtlMemoryGrow at %p\n",
			reinterpret_cast<void*>(g_loadPackageAddr),
			reinterpret_cast<void*>(g_loadPackageTramp),
			reinterpret_cast<void*>(g_pCUtlMemoryGrow)
		);
		return true;
	}

	void remove()
	{
		if (g_loadPackageHookSize && g_loadPackageAddr != LM_ADDRESS_BAD
		    && g_loadPackageTramp != LM_ADDRESS_BAD)
		{
			LM_UnhookCode(g_loadPackageAddr, g_loadPackageTramp, g_loadPackageHookSize);
			g_loadPackageHookSize = 0;
		}
		g_pOrigLoadPackage = nullptr;
		g_pCUtlMemoryGrow  = nullptr;
		g_pMarkLicenseAsChanged = nullptr;
		g_pProcessPendingLicenseUpdates = nullptr;
		g_pPackage0.store(nullptr, std::memory_order_release);
		g_licenseReconciled.store(false, std::memory_order_release);
		g_package0Injected.store(false, std::memory_order_release);
		std::lock_guard<std::mutex> lk(g_seededMutex);
		g_seededAppIds.clear();
		g_seededDepotIds.clear();
		g_extraAppIds.clear();
		g_desiredSnapshot = {};
		g_hasDesiredSnapshot = false;
		g_appliedGeneration = 0;
		g_processedGeneration = 0;
		g_pendingGeneration = 0;
		g_deferredLogGeneration = 0;
		g_deferredLogValid = false;
		g_runtimeRefreshPending = false;
		g_runtimeRefreshPendingHint.store(false, std::memory_order_release);
		g_runtimeRefreshExecuting.store(false, std::memory_order_release);
		g_pendingMetadataAppIds.clear();
		g_appInfoRequestState.clear();
		g_fallbackReconcileGeneration = 0;
	}

	void setExtraAppIds(const std::vector<uint32_t>& appIds)
	{
		std::lock_guard<std::mutex> lk(g_seededMutex);
		g_extraAppIds = appIds;
	}

	bool injectIntoPackage0(const std::vector<uint32_t>& appIds)
	{
		PackageInfo* pPkg = g_pPackage0.load(std::memory_order_acquire);
		if (!pPkg || !g_pCUtlMemoryGrow || appIds.empty())
		{
			return false;
		}
		bool injected;
		{
			std::lock_guard<std::mutex> lk(g_seededMutex);
			injected = injectFullSetLocked(pPkg, appIds) > 0;
		}

		reconcileLicensesOnce();
		return injected;
	}

	void tryReconcileLicenses()
	{

		reconcileLicensesOnce();
	}

	void forceReconcileLicenses()
	{

		if (!g_package0Injected.load(std::memory_order_acquire))
		{
			return;
		}
		g_licenseReconciled.store(false, std::memory_order_release);
		reconcileLicensesOnce();
	}

	void synchronizePackage0(const PackageSnapshot& snapshot)
	{
		SnapshotApplyResult applied;
		PackageSnapshot desired = snapshot;
		bool refreshNeeded = false;
		bool unresolvedStateSafe = false;
		bool stale = false;
		bool conflicting = false;
		{
			std::lock_guard<std::mutex> lock(g_seededMutex);
			if (g_hasDesiredSnapshot &&
				snapshot.generation < g_desiredSnapshot.generation)
			{
				stale = true;
			}
			else if (g_hasDesiredSnapshot &&
				snapshot.generation == g_desiredSnapshot.generation &&
				!(snapshot == g_desiredSnapshot))
			{

				conflicting = true;
			}
			else
			{
				if (g_hasDesiredSnapshot)
				{
					const bool previousAppInfoRequested =
						g_appInfoRequestState.allAccepted(
							g_desiredSnapshot.generation,
							g_desiredSnapshot.appInfoRequestIds);
					desired = HotReloadPackage::carryPendingSnapshotWork(
						g_desiredSnapshot, desired,
						g_processedGeneration >= g_desiredSnapshot.generation,
						previousAppInfoRequested);
				}
				g_desiredSnapshot = desired;
				g_hasDesiredSnapshot = true;
				applied = applySnapshotLocked(
					g_pPackage0.load(std::memory_order_acquire), desired);
				if (applied.available)
				{
					if (applied.changed)
					{
						g_runtimeRefreshPending = true;
						g_runtimeRefreshPendingHint.store(
							true, std::memory_order_release);
						g_pendingGeneration = snapshot.generation;
					}
					else if (g_runtimeRefreshPending)
					{

						g_pendingGeneration = snapshot.generation;
					}
					refreshNeeded = g_runtimeRefreshPending;
					unresolvedStateSafe =
						LicenseRefreshPolicy::unresolvedStateSafe(
							!g_pendingMetadataAppIds.empty(), AppInfoState::ready());
				}
			}
		}

		if (stale)
		{
			g_pLog->debug(
				"PackagePatch: ignored stale runtime package generation %llu\n",
				static_cast<unsigned long long>(snapshot.generation));
			return;
		}
		if (conflicting)
		{
			g_pLog->warn(
				"PackagePatch: ignored conflicting payload for runtime package generation %llu\n",
				static_cast<unsigned long long>(snapshot.generation));
			return;
		}
		if (!applied.available)
		{
			logDeferredOnce(snapshot.generation, "package 0 is not ready");
			return;
		}

		if (applied.changed)
		{
			g_pLog->info(
				"PackagePatch: synchronized runtime package generation %llu\n",
				static_cast<unsigned long long>(snapshot.generation));
		}
		const RuntimeRefreshResult refreshResult = processRuntimeRefresh(
			snapshot.generation, unresolvedStateSafe, refreshNeeded);
		if (refreshResult == RuntimeRefreshResult::Complete)
		{
			for (const std::uint32_t appId : desired.addedAppIds)
				LibraryRemoval::restore(appId);
		}

		if (refreshResult != RuntimeRefreshResult::Busy)
			requestLiveAppInfo(snapshot.generation, applied.appInfoRequestIds);
	}

	bool runtimeRefreshReady()
	{
		HotReloadCapabilities::Matrix capabilities;
		capabilities.markLicenseChanged =
			g_pMarkLicenseAsChanged != nullptr;
		capabilities.processLicenseUpdates =
			g_pProcessPendingLicenseUpdates != nullptr;
		capabilities.unresolvedAppGuard = AppInfoState::ready();
		return capabilities.canProcessUnresolvedAdd();
	}

	bool runtimeRefreshPending() noexcept
	{
		return g_runtimeRefreshPendingHint.load(std::memory_order_acquire);
	}

	void reprocessCurrentState() noexcept
	{
		try
		{
			std::uint64_t generation = 0;
			std::vector<std::uint32_t> addedAppIds;
			std::vector<std::uint32_t> appInfoRequestIds;
			bool unresolvedStateSafe = false;
			{
				std::lock_guard<std::mutex> lock(g_seededMutex);
				if (!g_runtimeRefreshPending || !g_hasDesiredSnapshot ||
					g_appliedGeneration != g_desiredSnapshot.generation)
				{
					return;
				}
				generation = g_appliedGeneration;
				addedAppIds = g_desiredSnapshot.addedAppIds;
				appInfoRequestIds = g_desiredSnapshot.appInfoRequestIds;
				g_pendingGeneration = generation;
				unresolvedStateSafe =
					LicenseRefreshPolicy::unresolvedStateSafe(
						!g_pendingMetadataAppIds.empty(), AppInfoState::ready());
			}
			const RuntimeRefreshResult result = processRuntimeRefresh(
				generation, unresolvedStateSafe, true);
			if (result == RuntimeRefreshResult::Complete)
			{
				for (const std::uint32_t appId : addedAppIds)
					LibraryRemoval::restore(appId);
			}
			if (result != RuntimeRefreshResult::Busy)
				requestLiveAppInfo(generation, appInfoRequestIds);
		}
		catch (...)
		{

			return;
		}
	}
}

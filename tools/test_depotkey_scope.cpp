

#include "../src/feats/depotkey_scope.hpp"
#include "../src/feats/depotkey_import.hpp"
#include "../src/feats/depotkey_index.hpp"

#include <atomic>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

using DepotKey::depotInManifestScope;

int main()
{

	CHECK(!depotInManifestScope(false,
	                            false,
	                            false,
	                            false),
	      "observed-only depot (owned game / runtime) is out of scope");

	CHECK(depotInManifestScope(true, false, false, false),
	      "AddedApp by app id is in scope");

	CHECK(depotInManifestScope(false, true, false, false),
	      "AddedApp by depot id is in scope");

	CHECK(depotInManifestScope(false, false, true, false),
	      "Lua-managed depot key is in scope even when app_id is absent");

	CHECK(depotInManifestScope(false, false, false, true),
	      "explicitly pinned depot is in scope");

	CHECK(!depotInManifestScope(false, false, false, false),
	      "observed key alone never forces scope (no downgrade of the fix)");

	CHECK(depotInManifestScope(true, true, true, true),
	      "all signals present is in scope");

	CHECK(!depotInManifestScope(true, false, true, false,
	                            true),
	      "owned AddedApp base depot uses Steam's manifest path");
	CHECK(!depotInManifestScope(true, false, true, false,
	                           true),
	      "owned AddedApp DLC depot uses Steam's manifest path");
	CHECK(depotInManifestScope(true, false, true, false,
	                          false),
	      "unlicensed AddedApp DLC depot remains in managed manifest scope");
	CHECK(!depotInManifestScope(true, false, true, true,
	                            true),
	      "native ownership wins over an explicit manifest pin");
	CHECK(!depotInManifestScope(true, true, true, false,
	                            true),
	      "owned AddedApp workshop depot uses Steam's manifest path");
	CHECK(DepotKey::manifestContentAppId(227300, 227300, 227300, 0) == 227300,
	      "workshop content follows the base app license");
	CHECK(DepotKey::manifestContentAppId(227300, 227300, 227300, 2004210) == 2004210,
	      "DLC depot follows its own content license");
	CHECK(DepotKey::manifestContentAppId(227300, 227300, 2780810, 0) == 227300,
	      "explicit base context wins over a shared catalog entry");
	CHECK(DepotKey::manifestContentAppId(0, 0, 2780810, 0) == 2780810,
	      "catalog content is used when the request has no app context");

	using DepotKey::mergeManagedFlag;
	CHECK(mergeManagedFlag(false, false) == false,
	      "observed then observed stays unmanaged");
	CHECK(mergeManagedFlag(false, true) == true,
	      "observed then Lua-import upgrades to managed");
	CHECK(mergeManagedFlag(true, false) == true,
	      "Lua-managed then observed stays managed (no downgrade)");
	CHECK(mergeManagedFlag(true, true) == true,
	      "managed then managed stays managed");

	DepotKey::LuaScriptImportGate importGate;
	bool prerequisitesReady = false;
	int imported = 0;
	CHECK(!importGate.run([&] { return prerequisitesReady; },
	                      [&] { ++imported; }),
	      "missing Lua import prerequisite does not claim the gate");
	CHECK(!importGate.done(),
	      "missing Lua import prerequisite leaves the gate retryable");
	prerequisitesReady = true;
	CHECK(importGate.run([&] { return prerequisitesReady; },
	                     [&] { ++imported; }),
	      "Lua import claims the gate when prerequisites become available");
	CHECK(imported == 1,
	      "Lua import executes once after a retryable prerequisite miss");
	CHECK(!importGate.run([&] { return prerequisitesReady; },
	                      [&] { ++imported; }),
	      "completed Lua import rejects later claims");
	CHECK(imported == 1,
	      "completed Lua import remains idempotent");
	CHECK(importGate.rerun([&] { return prerequisitesReady; },
	                       [&] { ++imported; }),
	      "watcher rescan runs after the startup gate completed");
	CHECK(imported == 2,
	      "watcher rescan executes the importer exactly once per request");

	DepotKey::LuaScriptImportGate failureGate;
	int attempts = 0;
	CHECK(!failureGate.run([] { return true; }, [&]
	{
		++attempts;
		throw std::runtime_error("deterministic import failure");
	}), "failed Lua import does not consume the gate");
	CHECK(!failureGate.done(), "failed Lua import remains retryable");
	CHECK(failureGate.run([] { return true; }, [&] { ++attempts; }),
	      "Lua import succeeds on the retry after a failure");
	CHECK(attempts == 2, "failed Lua import is attempted again exactly once");

	DepotKey::LuaScriptImportGate concurrentGate;
	std::atomic<int> concurrentImports{0};
	std::atomic<int> acceptedClaims{0};
	std::vector<std::thread> callers;
	for (int i = 0; i < 8; ++i)
	{
		callers.emplace_back([&]
		{
			if (concurrentGate.run([] { return true; },
			                         [&] { ++concurrentImports; }))
			{
				++acceptedClaims;
			}
		});
	}
	for (auto& caller : callers) caller.join();
	CHECK(concurrentImports.load() == 1,
	      "concurrent Lua import callers execute one body");
	CHECK(acceptedClaims.load() == 1,
	      "concurrent Lua import callers receive one claim");

	DepotKey::LazyIndex<unsigned int, bool> index;
	int loadCalls = 0;
	index.loadOnce([&](auto& entries) {
		++loadCalls;
		entries.emplace(701u, false);
	});
	index.loadOnce([&](auto&) { ++loadCalls; });
	CHECK(loadCalls == 1, "depot-key index loader runs once");
	CHECK(index.find(701u) && !*index.find(701u),
	      "depot-key index keeps the parsed record");
	index.upsert(701u, DepotKey::mergeManagedFlag(*index.find(701u), true));
	index.upsert(701u, DepotKey::mergeManagedFlag(*index.find(701u), false));
	CHECK(index.find(701u) && *index.find(701u),
	      "depot-key index preserves managed after passive re-observation");

	DepotKey::ManagedDepotIndex appIndex;
	appIndex.replace(501u, 0u, false,
	                 100u, true);
	appIndex.replace(502u, 0u, false,
	                 100u, true);
	appIndex.replace(601u, 0u, false,
	                 200u, true);
	CHECK(appIndex.forApp(100u) == std::vector<unsigned int>({501u, 502u}),
	      "app depot index returns only the managed depots for one app");
	CHECK(appIndex.forApp(999u).empty(),
	      "app depot index misses do not scan or leak another app's depots");
	appIndex.replace(502u, 100u, true,
	                 200u, true);
	CHECK(appIndex.forApp(100u) == std::vector<unsigned int>({501u}) &&
	      appIndex.forApp(200u) == std::vector<unsigned int>({502u, 601u}),
	      "app depot index moves a rewritten depot between app ids");
	appIndex.replace(501u, 100u, true,
	                 100u, false);
	CHECK(appIndex.forApp(100u).empty(),
	      "app depot index removes a depot that is no longer managed");

	if (g_failures == 0) std::printf("\nall depotkey-scope checks passed\n");
	else                 std::printf("\n%d depotkey-scope check(s) FAILED\n", g_failures);
	return g_failures == 0 ? 0 : 1;
}

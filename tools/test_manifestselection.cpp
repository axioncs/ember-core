

#include "../src/feats/manifestselection.hpp"

#include <cstdio>
#include <optional>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

using ManifestSelection::ChoiceSource;
using ManifestSelection::ExactState;

int main()
{

	{
		const auto d = ManifestSelection::choose(
		    900, ExactState::Ready, 800, 700);
		CHECK(d.final && d.gid == 900 && d.source == ChoiceSource::Exact,
		      "ready exact gid wins over every local fallback");
	}

	{
		const auto d = ManifestSelection::choose(
		    900, ExactState::Pending, 800, 700);
		CHECK(!d.final && d.gid == 900,
		      "pending exact gid remains unresolved");
	}

	{
		const auto d = ManifestSelection::choose(
		    900, ExactState::Unavailable,
		    800, 850);
		CHECK(d.final && d.gid == 800 &&
		          d.source == ChoiceSource::PreferredLocal,
		      "unavailable exact gid falls back to last observed local gid");
	}

	{
		const auto d = ManifestSelection::choose(
		    100, ExactState::Unavailable,
		    20, 999999);
		CHECK(d.gid == 20 && d.source == ChoiceSource::PreferredLocal,
		      "manifest recency never depends on numeric gid ordering");
	}

	{
		const auto d = ManifestSelection::choose(
		    900, ExactState::Unavailable,
		    0, 700);
		CHECK(d.gid == 700 && d.source == ChoiceSource::LegacyLocal,
		      "legacy archived manifest is used when no preferred marker exists");
	}

	{
		const auto d = ManifestSelection::choose(
		    900, ExactState::Unavailable,
		    900, 700);
		CHECK(d.gid == 700 && d.source == ChoiceSource::LegacyLocal,
		      "failed exact gid is excluded from fallback candidates");
	}

	{
		const auto d = ManifestSelection::choose(
		    900, ExactState::Unavailable, 0, 0);
		CHECK(d.final && d.gid == 900 &&
		          d.source == ChoiceSource::ExactUnavailable,
		      "no local fallback leaves the requested gid unchanged");
	}

	CHECK(ManifestSelection::remainingBudgetMs(12000, 3000) == 9000,
	      "plan budget subtracts elapsed time");
	CHECK(ManifestSelection::remainingBudgetMs(12000, 12000) == 0,
	      "plan budget reaches zero at the deadline");
	CHECK(ManifestSelection::remainingBudgetMs(12000, 15000) == 0,
	      "plan budget never becomes negative");

	{
		std::optional<uint64_t> size;
		int waits = 0;
		const auto pair = ManifestSelection::resolvePinnedPair(
		    100, 1000, 200, 9000,
		    [&]() { return size; },
		    [&](int budgetMs)
		    {
			++waits;
			CHECK(budgetMs == 9000,
			      "first-import wait receives only the remaining plan budget");
			size = 2222;
			return true;
		    });
		CHECK(waits == 1 && pair.pinned && pair.gid == 200 && pair.size == 2222,
		      "first-import fetch applies pinned gid and size in one plan");
	}

	for (const int budget : {9000, 0})
	{
		int waits = 0;
		const auto pair = ManifestSelection::resolvePinnedPair(
		    100, 1000, 200, budget,
		    []() { return std::optional<uint64_t>{}; },
		    [&](int) { ++waits; return false; });
		CHECK(!pair.pinned && pair.gid == 100 && pair.size == 1000,
		      "unavailable pinned metadata preserves the public pair atomically");
		CHECK(waits == (budget > 0 ? 1 : 0),
		      "an exhausted shared budget never starts another wait");
	}
	{
		const auto pair = ManifestSelection::resolvePinnedPair(
		    100, 1000, 200, 9000,
		    []() { return std::optional<uint64_t>{}; },
		    [](int) { return true; });
		CHECK(!pair.pinned && pair.gid == 100 && pair.size == 1000,
		      "fetched manifest without readable size preserves the public pair");
	}

	{
		int waits = 0;
		const auto pair = ManifestSelection::resolvePinnedPair(
		    100, 1000, 200, 9000,
		    []() { return std::optional<uint64_t>{2222}; },
		    [&](int) { ++waits; return true; });
		CHECK(waits == 0 && pair.pinned && pair.gid == 200 && pair.size == 2222,
		      "cached pinned size avoids the blocking provider path");
	}

	CHECK(ManifestSelection::validVectorBounds(5000, 5000, 0x20),
	      "valid depot vectors are not capped at 4096 entries");
	CHECK(!ManifestSelection::validVectorBounds(5001, 5000, 0x20),
	      "depot vector count cannot exceed its allocation");
	CHECK(!ManifestSelection::validVectorBounds(-1, 5000, 0x20),
	      "negative depot vector counts are rejected");

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}



#include "../src/feats/retry.hpp"

#include <cstdio>
#include <vector>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

int main()
{

	{
		int attempts = 0;
		std::vector<int> sleeps;
		const bool ok = AppInfoProvision::retryWithBackoff(
			[&] { ++attempts; return true; },
			3, 100,
			[&](int ms) { sleeps.push_back(ms); });
		CHECK(ok, "first-try success returns true");
		CHECK(attempts == 1, "first-try success makes exactly 1 attempt");
		CHECK(sleeps.empty(), "first-try success never sleeps");
	}

	{
		int attempts = 0;
		std::vector<int> sleeps;
		const bool ok = AppInfoProvision::retryWithBackoff(
			[&] { ++attempts; return attempts >= 3; },
			5, 100,
			[&](int ms) { sleeps.push_back(ms); });
		CHECK(ok, "recovers after transient failures");
		CHECK(attempts == 3, "stops attempting once it succeeds");
		CHECK(sleeps.size() == 2, "sleeps only between the 3 attempts");
		CHECK(sleeps.size() == 2 && sleeps[0] == 100 && sleeps[1] == 200,
		      "backoff grows (100ms, 200ms)");
	}

	{
		int attempts = 0;
		std::vector<int> sleeps;
		const bool ok = AppInfoProvision::retryWithBackoff(
			[&] { ++attempts; return false; },
			3, 100,
			[&](int ms) { sleeps.push_back(ms); });
		CHECK(!ok, "all-fail returns false");
		CHECK(attempts == 3, "all-fail makes exactly maxAttempts attempts");
		CHECK(sleeps.size() == 2, "no sleep after the final failed attempt");
	}

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}

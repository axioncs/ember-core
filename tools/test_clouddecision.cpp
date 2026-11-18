

#include "../src/feats/clouddecision.hpp"

#include <cstdio>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

int main()
{

	CHECK(Apps::cloudDisableDecision(false, true, false, false) == false,
	      "config off keeps cloud enabled even for a managed app");

	CHECK(Apps::cloudDisableDecision(true, true, false, false) == true,
	      "managed app disables cloud");
	CHECK(Apps::cloudDisableDecision(true, true, false, true) == true,
	      "managed app disables cloud even when Steam reports it owned");

	CHECK(Apps::cloudDisableDecision(true, false, false, false) == false,
	      "unmanaged app keeps cloud enabled when we unlock nothing globally");
	CHECK(Apps::cloudDisableDecision(true, false, false, true) == false,
	      "owned unmanaged app keeps cloud enabled");

	CHECK(Apps::cloudDisableDecision(true, false, true, false) == true,
	      "globally unlocked app disables cloud when Steam reports it unowned");
	CHECK(Apps::cloudDisableDecision(true, false, true, true) == false,
	      "owned app keeps cloud enabled with global unlocking on");

	std::printf(g_failures == 0 ? "\nALL PASS\n" : "\n%d FAILURE(S)\n",
	            g_failures);
	return g_failures == 0 ? 0 : 1;
}

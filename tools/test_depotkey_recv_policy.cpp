

#include "../src/feats/depotkey_recv_policy.hpp"

#include <cassert>
#include <cstdio>

using DepotKey::classifyRecv;
using DepotKey::RecvAction;

int main()
{

	assert(classifyRecv(true, true, false, false) == RecvAction::CacheObserved);
	assert(classifyRecv(true, true, true, true) == RecvAction::CacheObserved);

	assert(classifyRecv(true, false, false, false) == RecvAction::None);
	assert(classifyRecv(true, false, true, true) == RecvAction::None);

	assert(classifyRecv(false, false, true, false) == RecvAction::SubstituteCached);
	assert(classifyRecv(false, false, true, true) == RecvAction::SubstituteCached);

	assert(classifyRecv(false, false, false, true) == RecvAction::SynthZero);

	assert(classifyRecv(false, false, false, false) == RecvAction::None);

	assert(classifyRecv(false, true, false, false) == RecvAction::None);

	std::puts("test_depotkey_recv_policy: all assertions passed");
	return 0;
}

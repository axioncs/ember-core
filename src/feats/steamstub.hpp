

#pragma once

#include <cstdint>

namespace SteamStub
{

	void setup(const char* installRoot);

	void warmupAsync();

	void onLaunchApp(uint32_t appId);
}

#include "IClientUtils.hpp"

#include "../hooks.hpp"
#include "../patterns.hpp"

#include "libmem/libmem.h"
#include "steam.hpp"

HSteamPipe IClientUtils::getCurrentSteamPipe()
{

#if defined(__x86_64__)
	const static int32_t offset = *reinterpret_cast<int32_t*>(Patterns::IClientUtils::Offset_GetPipeIndex.address + 0x2);
	return *reinterpret_cast<HSteamPipe*>(reinterpret_cast<uintptr_t>(this) + static_cast<intptr_t>(offset));
#else
	const static auto offset = *reinterpret_cast<lm_address_t*>(Patterns::IClientUtils::Offset_GetPipeIndex.address + 0x2);
	return *reinterpret_cast<HSteamPipe*>(this + offset);
#endif
}

uint32_t IClientUtils::getAppId()
{
	return Hooks::IClientUtils_GetAppId.originalFn.fn(this);
}

IClientUtils* g_pClientUtils;

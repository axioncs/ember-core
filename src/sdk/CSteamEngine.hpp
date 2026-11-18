#pragma once

#include "CUtl.hpp"
#include "steam.hpp"

#include <cstddef>
#include <cstdint>

class CUser;
class IClientCompat;
class IClientUtils;

class IProcessPipe { };
using CUtlString = char*;

class __attribute__((packed)) __attribute__((aligned(1))) CServerPipe
{
public:
	IProcessPipe* internalPipe;
	IProcessPipe* singleProcessPipe;
	HSteamPipe pipeHandle;
	uint8_t __pad0xC[0x8];
	int32_t pid;
	int32_t threadId;
	CUtlString processName;
	uint8_t __pad0x20[0x1];
	HSteamUser userHandle;
	uint8_t __pad0x25[0x7];
	void* queueCallbackMsg;
	uint8_t __pad0x30[0x8];
	uint32_t numQueuedCallbacks;
	uint8_t __pad0x3C[0x14];
	CUtlVector<void> debugText;
};

#if defined(__i386__)
static_assert(offsetof(CServerPipe, pipeHandle) == 0x8);
static_assert(offsetof(CServerPipe, pid) == 0x14);
#endif

class CSteamEngine
{
public:
	CServerPipe* getServerPipe(HSteamPipe pipe);
	CUser* getUser(uint32_t index);
	IClientUtils* getUtils();
	void setAppIdForCurrentPipe(uint32_t appId);
};

extern CSteamEngine* g_pSteamEngine;

extern CUser* g_pLocalUser;

CUser* getLocalUser();

IClientCompat* getLocalClientCompat();

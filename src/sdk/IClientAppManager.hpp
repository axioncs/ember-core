#pragma once

#include "types.hpp"

#include <cstdint>


enum EAppState : uint32_t
{
	k_EAppStateInvalid = 0x0,
	k_EAppStateUninstalled = 0x1,
	k_EAppStateUpdateRequired = 0x2,
	k_EAppStateFullyInstalled = 0x4,
	k_EAppStateUpdateQueued = 0x8,
	k_EAppStateUpdateOptional = 0x10,
	k_EAppStateFilesMissing = 0x20,
	k_EAppStateSharedOnly = 0x40,
	k_EAppStateFilesCorrupt = 0x80,
	k_EAppStateUpdateRunning = 0x100,
	k_EAppStateUpdatePaused = 0x200,
	k_EAppStateUpdateStarted = 0x400,
	k_EAppStateUninstalling = 0x800,
	k_EAppStateBackupRunning = 0x1000,
	k_EAppStateAppRunning = 0x2000,
	k_EAppStateComponentInUse = 0x4000,
	k_EAppStateMovingFolder = 0x8000,
	k_EAppStateTerminating = 0x10000,
	k_EAppStatePrefetchingInfo = 0x20000,
	k_EAppStatePeerServer = 0x40000,
	k_EAppStateUpdatedDisabledbyapp = 0x80000
};

// Recovered from steamclient.so's own DepotInfo_t handling (the <T> entry point
// of CUserAppManager::BuildDepotDependency at 0x1079640 in the Nov-2025 build):
// elements are 0x20 bytes (every walk of the vector is `shl $0x5`), the client
// compares element+0x0 against depot ids and element+0x8 against m_ulManifestID,
// and its reset path zero-fills 0x00-0x1E, leaving 0x1F as padding.
SDK_Struct DepotInfo_t
{
	AppId_t depotId;			//0x0
	AppId_t appId;				//0x4
	uint64_t manifestId;		//0x8  asserted != k_uManifestIdInvalid
	uint64_t size;				//0x10 summed into the install size by the caller
	uint32_t unk0x18;			//0x18 gates a second size accumulator; the client writes 0
	uint8_t unk0x1C;			//0x1C
	uint8_t unk0x1D;			//0x1D
	uint8_t unk0x1E;			//0x1E
	uint8_t __pad0x1F;			//0x1F
}; //0x20

// The client's vector stride is a literal 0x20 (every walk is `shl $0x5`), so a
// change to this struct silently misaligns the whole depot plan.
static_assert(sizeof(DepotInfo_t) == 0x20, "DepotInfo_t must stay 0x20 bytes: steamclient walks the plan in 0x20 strides");

SDK_Class IClientAppManager
{
public:
	EAppState getAppInstallState(const AppId_t appId);
	uint32_t getLibraryFolderLabel(const uint32_t index, char* pChLabel, const uint32_t labelSize);
	uint32_t getLibraryFolderPath(const uint32_t index, char* pChPath, const steamsize_t pathSize);
	uint32_t getNumLibraryFolders();
	bool installApp(const AppId_t appId, const uint32_t libraryIndex);
	uint32_t uninstallApp(const AppId_t appId);
};

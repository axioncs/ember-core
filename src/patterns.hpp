#pragma once

#include "memhlp.hpp"

#include "libmem/libmem.h"

#include <string>
#include <vector>

struct Pattern_t
{
public:
	const std::string name;

	const std::string symbol;

	std::string pattern;
	const MemHlp::SigFollowMode followMode;
	std::vector<int16_t> prologue;

	bool optional = false;

	lm_address_t address = LM_ADDRESS_BAD;

	lm_address_t matchAddress = LM_ADDRESS_BAD;
	lm_module_t* const module;

	Pattern_t(const char* name, const char* pattern,
	          MemHlp::SigFollowMode followMode, lm_module_t* module = nullptr,
	          const char* symbol = nullptr);
	Pattern_t(const char* name, const char* pattern,
	          MemHlp::SigFollowMode followMode, std::vector<uint8_t> prologue,
	          lm_module_t* module = nullptr, const char* symbol = nullptr);
	Pattern_t(const char* name, const char* pattern,
	          MemHlp::SigFollowMode followMode, std::vector<int16_t> prologue,
	          lm_module_t* module = nullptr, const char* symbol = nullptr);

	bool find();
};

namespace Patterns
{
	extern Pattern_t ParentalSignatureCheck;
	extern Pattern_t ParentalSettingsReceived;

	extern Pattern_t TraceIPC;

	namespace SteamUI
	{
		extern Pattern_t AppControllerRunFrame;
		extern Pattern_t GetAppByID;
		extern Pattern_t MarkAppChange;
		extern Pattern_t BuildCompleteAppOverviewChange;
		extern Pattern_t OwnershipFlagsReference;
	}

	namespace CAPIJob
	{
		extern Pattern_t GetPlayerStats;
	}

	namespace CProtoBufMsgBase
	{
		extern Pattern_t InitFromPacket;
		extern Pattern_t Send;
	};

	namespace CCMInterface
	{
		extern Pattern_t RecvPkt;
	}

	namespace CSteamEngine
	{
		extern Pattern_t GetServerPipe;
		extern Pattern_t Init;
		extern Pattern_t SetAppIdForCurrentPipe;
		extern Pattern_t ProcessIPCFrame;
		extern Pattern_t Offset_ClientUtils;
		extern Pattern_t Offset_User;
	}

	namespace CSteamMatchmakingServers
	{
		extern Pattern_t GetServerDetails;
		extern Pattern_t RequestInternetServerList;
	}

	namespace CUser
	{
		extern Pattern_t Offset_CompatManager;
		extern Pattern_t CheckAppOwnership;
		extern Pattern_t GetSubscribedApps;
		extern Pattern_t PostCallback;
		extern Pattern_t PostCallbackToAppId;
		extern Pattern_t UpdateAppOwnershipTicket;
		extern Pattern_t MarkLicenseAsChanged;
		extern Pattern_t ProcessPendingLicenseUpdates;
		extern Pattern_t NotifyLicensesUpdated;
	}

	namespace CAppInfoCache
	{
		extern Pattern_t GetOrAddAppData;
		extern Pattern_t ThreadedReadFromDisk;
		extern Pattern_t SkipFlagReference;
		extern Pattern_t ShaReference;
	}

	namespace IClientAppManager
	{
		extern Pattern_t RunIPCFrame;
		extern Pattern_t BCanRemotePlayTogether;
	}

	namespace IClientApps
	{
		extern Pattern_t RunIPCFrame;
	}

	namespace IClientRemoteStorage
	{
		extern Pattern_t RunIPCFrame;
	}

	namespace IClientUser
	{
		extern Pattern_t RunIPCFrame;

		extern Pattern_t BLoggedOn;
		extern Pattern_t BUpdateAppOwnershipTicket;
		extern Pattern_t GetAppOwnershipTicketExtendedData;
		extern Pattern_t IsUserSubscribedAppInTicket;
		extern Pattern_t RequiresLegacyCDKey;
	}

	namespace IClientUGC
	{
		extern Pattern_t RunIPCFrame;
	}

	namespace IClientUserStats
	{
		extern Pattern_t RunIPCFrame;
	}

	namespace CPackageInfoCache
	{
		extern Pattern_t LoadPackage;
	}

	namespace CUtlMemory
	{
		extern Pattern_t Grow;
	}

	namespace CDepotDownloadMgr
	{
		extern Pattern_t ProcessDepotManifest;
		extern Pattern_t PrepareDepotDownload;
		extern Pattern_t BuildDepotDependency;
		extern Pattern_t EvaluateConfigChanges;
		extern Pattern_t OnChunkUnpackedStack;
		extern Pattern_t OnChunkUnpackedReg;
	}

	namespace IClientFriends
	{
		extern Pattern_t GetFriendGamePlayed;
	}

	namespace IClientUtils
	{
		extern Pattern_t RunIPCFrame;
		extern Pattern_t Offset_GetPipeIndex;
	}

	namespace CWebSocketConnection
	{
		extern Pattern_t BBuildAndAsyncSendFrame;
	}

	namespace CRemoteClientManager
	{
		extern Pattern_t RecvPkt;
	}

	namespace CJobMgr
	{
		extern Pattern_t BRouteMsgToJob;
	}

	namespace CDepotDownloadMgr
	{
		extern Pattern_t BYldRequestDepotManifest;
	}

	namespace ISteamMatchmakingPingResponse
	{
		extern Pattern_t ServerResponded;
	}

	std::vector<Pattern_t*>& patterns();
	bool init();
}

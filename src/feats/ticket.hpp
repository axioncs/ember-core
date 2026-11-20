#pragma once

#include "../sdk/CSteamID.hpp"
#include "../sdk/steam.hpp"

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

class CMsgClientGetAppOwnershipTicketResponse;
class CMsgClientRequestEncryptedAppTicketResponse;
class CProtoBufMsgBase;

namespace Ticket
{

	inline bool shouldStampAppOwnershipTicket(bool isAddedApp,
	                                           bool isAddedAppDlc) noexcept
	{
		return isAddedApp || isAddedAppDlc;
	}

	class SavedTicket
	{
public:
		CSteamId steamId;
		std::string ticket;

		constexpr bool isValid() const
		{
			return steamId.isSet() && ticket.size() > 0;
		}
	};

	extern std::unordered_map<AppId_t, CSteamId> oneTimeSteamIdSpoof;
	extern std::unordered_map<AppId_t, SavedTicket> ticketMap;
	extern std::unordered_map<AppId_t, SavedTicket> encryptedTicketMap;
	inline std::mutex cacheMutex;
	inline std::unordered_set<uint32_t> invalidatedApps;

	inline bool forgetApp(uint32_t appId)
	{
		if (appId == 0) return false;
		std::lock_guard<std::mutex> lock(cacheMutex);
		invalidatedApps.insert(appId);
		ticketMap.erase(appId);
		encryptedTicketMap.erase(appId);
		return true;
	}

	inline bool isAppInvalidated(uint32_t appId)
	{
		if (appId == 0) return false;
		std::lock_guard<std::mutex> lock(cacheMutex);
		return invalidatedApps.contains(appId);
	}

	inline bool restoreApp(uint32_t appId)
	{
		if (appId == 0) return false;
		std::lock_guard<std::mutex> lock(cacheMutex);
		invalidatedApps.erase(appId);
		return true;
	}

	// A cached ticket is only valid for the app it was minted for. Anything
	// outside the added set is an orphan: Steam keeps its own ticket for apps
	// the account owns, so there is nothing for ember to keep here.
	inline bool isOrphaned(uint32_t appId, const std::unordered_set<uint32_t>& addedAppIds) noexcept
	{
		return appId != 0 && !addedAppIds.contains(appId);
	}

	std::string getTicketDir();

	std::string getTicketPath(uint32_t appId);
	SavedTicket getCachedTicket(uint32_t appId);
	bool saveTicketToCache(CMsgClientGetAppOwnershipTicketResponse* resp);

	// Drop the cached tickets for apps no longer in the added set. A ticket
	// is only valid for the app it was minted for, and nothing else in the
	// tree removed one when its app stopped being added, so a stale file
	// could outlive the app and be handed to Steam by a later caller.
	void purgeOrphans(const std::unordered_set<uint32_t>& addedAppIds);

	void closePipe(HSteamPipe pipe);
	void connectPipe(HSteamPipe pipe);
	void launchApp(uint32_t appId);
	void getEncryptedAppTicket(uint32_t appId);
	void getTicketOwnershipExtendedData(uint32_t appId);

	std::string getEncryptedTicketPath(uint32_t appId);
	SavedTicket getCachedEncryptedTicket(uint32_t appId);
	bool saveEncryptedTicketToCache(CMsgClientRequestEncryptedAppTicketResponse* resp);

	bool recvEncryptedAppTicket(CMsgClientRequestEncryptedAppTicketResponse* msg);
	bool recvAppTicket(CMsgClientGetAppOwnershipTicketResponse* msg);
	void recvMsg(CProtoBufMsgBase* msg);
}



#pragma once

#include "depotkey_recv_policy.hpp"

#include "../sdk/protobufs/steammessages_clientserver_2.pb.h"

#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <vector>

class CProtoBufMsgBase;

namespace DepotKey
{
	struct SavedKey
	{
		uint32_t appId  = 0;
		uint32_t depotId = 0;
		std::string key;

		bool managed = false;
	};

	std::string getKeyDir();
	std::string getKeyPath(uint32_t depotId);

	SavedKey getCachedKey(uint32_t depotId);
	bool saveKeyToCache(uint32_t appId, uint32_t depotId, const std::string& key, bool managed);

	bool isManagedDepot(uint32_t depotId);

	bool manifestInManagedScope(uint32_t appId, uint32_t depotId,
	                            bool depotHasPin = false,
	                            uint32_t contentAppId = 0);

	std::vector<uint32_t> managedDepotsForApp(uint32_t appId);

	void importLuaScripts();

	void reloadLuaScripts();

	void provisionManifests();

	void onStartup();

	void disableShaderCache();

	void recvMsg(CProtoBufMsgBase* msg);
	void sendMsg(CProtoBufMsgBase* msg);

	bool recvDepotKey(CMsgClientGetDepotDecryptionKeyResponse* resp);
	void sendDepotKey(CMsgClientGetDepotDecryptionKey* req);
}

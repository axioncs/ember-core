#include "ticket.hpp"

#include "fakeappid.hpp"
#include "apps.hpp"

#include "../config.hpp"
#include "../globals.hpp"
#include "../process.hpp"

#include "../sdk/CProtoBufMsgBase.hpp"
#include "../sdk/CSteamEngine.hpp"
#include "../sdk/CUser.hpp"
#include "../sdk/EResult.hpp"
#include "../sdk/IClientUtils.hpp"

#include "base64/base64.hpp"
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/emittermanip.h"

#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <mutex>
#include <sstream>

std::unordered_map<AppId_t, CSteamId> Ticket::oneTimeSteamIdSpoof = std::unordered_map<AppId_t, CSteamId>();
std::unordered_map<AppId_t, Ticket::SavedTicket> Ticket::ticketMap = std::unordered_map<AppId_t, SavedTicket>();
std::unordered_map<AppId_t, Ticket::SavedTicket> Ticket::encryptedTicketMap = std::unordered_map<AppId_t, SavedTicket>();

std::string Ticket::getTicketDir()
{
	std::stringstream ss;
	ss << g_config.getDir().c_str() << "/cache";

	const auto dir = ss.str();
	if (!std::filesystem::exists(dir.c_str()))
	{
		std::filesystem::create_directory(dir.c_str());
	}

	return ss.str();
}

std::string Ticket::getTicketPath(uint32_t appId)
{
	std::stringstream ss;
	ss << getTicketDir().c_str() << "/ticket_" << appId << ".yaml";

	return ss.str();
}

Ticket::SavedTicket Ticket::getCachedTicket(uint32_t appId)
{
	std::lock_guard<std::mutex> lock(cacheMutex);
	if (invalidatedApps.contains(appId)) return {};

	const auto it = ticketMap.find(appId);
	if (it != ticketMap.end()) return it->second;

	SavedTicket ticket {};
	const auto path = getTicketPath(appId);
	if (!std::filesystem::exists(path.c_str()))
	{
		return ticket;
	}

	g_pLog->debug("Reading ticket for %u\n", appId);

	try
	{
		auto node = YAML::LoadFile(path);
		ticket.steamId = CSteamId(node["steamId"].as<uint64_t>());
		ticket.ticket = std::string
		(
			base64::from_base64(node["ticket"].as<std::string>())
		);
	}
	catch (const std::exception& e)
	{
		g_pLog->warn("Ignoring corrupt ticket cache for %u: %s\n", appId, e.what());
		return {};
	}

	ticketMap[appId] = ticket;
	return ticket;
}

bool Ticket::saveTicketToCache(CMsgClientGetAppOwnershipTicketResponse* resp)
{
	const uint32_t appId = resp->app_id();

	g_pLog->debug("Saving ticket for %u...\n", appId);

	auto bytes = resp->ticket();

	std::lock_guard<std::mutex> lock(cacheMutex);
	if (invalidatedApps.contains(appId)) return false;

	YAML::Emitter node;
	node << YAML::BeginMap;
	node << YAML::Key << "steamId";
	node << YAML::Value << g_currentSteamId.steamId64;
	node << YAML::Key << "ticket";
	node << YAML::Value << base64::to_base64(bytes);
	node << YAML::EndMap;

	const auto path = Ticket::getTicketPath(appId);
	std::ofstream ofs(path.c_str(), std::ios::out | std::ios::trunc);
	if (!ofs.is_open()) return false;
	ofs.write(node.c_str(), node.size());
	if (!ofs.good()) return false;

	g_pLog->infoOnce("Saved ticket for %u\n", appId);

	SavedTicket& ticket = ticketMap[appId];
	ticket.steamId = g_currentSteamId;
	ticket.ticket = bytes;
	return true;
}

void Ticket::connectPipe(const HSteamPipe pipe)
{
	const auto process = g_processMap.find(pipe);
	if (process == g_processMap.end())
	{
		return;
	}
	const auto& proc = process->second;

	if (!proc.steamDRM)
	{
		return;
	}

	const SavedTicket ticket = getCachedTicket(proc.appId);
	if (!ticket.isValid())
	{
		return;
	}

	oneTimeSteamIdSpoof[proc.appId] = ticket.steamId;
}

void Ticket::launchApp(uint32_t appId)
{
	auto ticket = getCachedTicket(appId);
	if (!ticket.isValid())
	{
		return;
	}

	CUser* user = getLocalUser();
	if (user == nullptr)
	{
		g_pLog->debug("Ticket::launchApp(%u): no local user yet; skipping\n", appId);
		return;
	}
	user->updateAppOwnershipTicket(appId, reinterpret_cast<void*>(ticket.ticket.data()), ticket.ticket.size());
	g_pLog->infoOnce("Force loaded AppOwnershipTicket for %i\n", appId);
}

void Ticket::getEncryptedAppTicket(uint32_t appId)
{
	const SavedTicket cached = Ticket::getCachedEncryptedTicket(appId);
	if (!cached.isValid())
	{
		return;
	}

	oneTimeSteamIdSpoof[appId] = cached.steamId;
}

void Ticket::getTicketOwnershipExtendedData(uint32_t appId)
{
	if ((g_config.smartTickets.get() & CConfig::k_ESmartTicketsSteamDRM)
		&& g_pSteamEngine)
	{
		const auto utils = g_pSteamEngine->getUtils();
		const auto process = utils
			? g_processMap.find(utils->getCurrentSteamPipe())
			: g_processMap.end();
		if (process != g_processMap.end() && process->second.steamDRM)
		{

			return;
		}
	}

	const SavedTicket cached = Ticket::getCachedTicket(appId);
	if (!cached.isValid())
	{
		return;
	}

	oneTimeSteamIdSpoof[appId] = cached.steamId;
}

std::string Ticket::getEncryptedTicketPath(uint32_t appId)
{
	std::stringstream ss;
	ss << getTicketDir().c_str() << "/encryptedTicket_" << appId << ".yaml";

	return ss.str();
}

Ticket::SavedTicket Ticket::getCachedEncryptedTicket(uint32_t appId)
{
	SavedTicket ticket {};
	const AppId_t fakeAppId = FakeAppIds::getFakeAppId(appId);
	const auto smartTickets = g_config.smartTickets.get();

	if (!(smartTickets & CConfig::k_ESmartTicketsDenuvo) && appId && fakeAppId && fakeAppId != appId)
	{
		g_pLog->infoOnce("Returning empty cached encrypted Ticket for %u because it's running as %u\n", appId, fakeAppId);
		return ticket;
	}

	std::lock_guard<std::mutex> lock(cacheMutex);
	if (invalidatedApps.contains(appId)) return {};

	const auto it = encryptedTicketMap.find(appId);
	if (it != encryptedTicketMap.end()) return it->second;

	const auto path = getEncryptedTicketPath(appId);
	if (!std::filesystem::exists(path.c_str()))
	{
		return ticket;
	}

	g_pLog->debug("Reading encrypted ticket for %u\n", appId);

	try
	{
		auto node = YAML::LoadFile(path);
		ticket.steamId = CSteamId(node["steamId"].as<uint64_t>());
		ticket.ticket = std::string
		(

			base64::from_base64(node["encryptedTicket"].as<std::string>())
		);
	}
	catch (const std::exception& e)
	{
		g_pLog->warn("Ignoring corrupt encrypted ticket cache for %u: %s\n", appId, e.what());
		return {};
	}

	encryptedTicketMap[appId] = ticket;
	return ticket;
}

bool Ticket::saveEncryptedTicketToCache(CMsgClientRequestEncryptedAppTicketResponse* resp)
{
	const uint32_t appId = resp->app_id();

	g_pLog->debug("Saving encrypted ticket for %u...\n", appId);

	auto bytes = resp->SerializeAsString();

	std::lock_guard<std::mutex> lock(cacheMutex);
	if (invalidatedApps.contains(appId)) return false;

	YAML::Emitter node;
	node << YAML::BeginMap;
	node << YAML::Key << "steamId";
	node << YAML::Value << g_currentSteamId.steamId64;
	node << YAML::Key << "encryptedTicket";

	node << YAML::Value << base64::to_base64(bytes);
	node << YAML::EndMap;

	const auto path = getEncryptedTicketPath(appId);
	std::ofstream ofs(path.c_str(), std::ios::out | std::ios::trunc);
	if (!ofs.is_open()) return false;
	ofs.write(node.c_str(), node.size());
	if (!ofs.good()) return false;

	g_pLog->infoOnce("Saved encrypted ticket for %u\n", appId);

	SavedTicket& ticket = encryptedTicketMap[appId];
	ticket.steamId = g_currentSteamId;
	ticket.ticket = bytes;
	return true;
}

bool Ticket::recvEncryptedAppTicket(CMsgClientRequestEncryptedAppTicketResponse* msg)
{
	if (msg->eresult() == ERESULT_OK)
	{
		saveEncryptedTicketToCache(msg);
		return false;
	}

	SavedTicket ticket = getCachedEncryptedTicket(msg->app_id());
	if(!ticket.isValid())
	{
		return false;
	}

	msg->ParseFromString(ticket.ticket);
	g_pLog->debug("Using encryptedTicket_%u from disk\n", msg->app_id());
	return true;
}

bool Ticket::recvAppTicket(CMsgClientGetAppOwnershipTicketResponse* msg)
{
	if(msg->eresult() == ERESULT_OK)
	{
		saveTicketToCache(msg);
		return false;
	}

	const uint32_t appId = msg->app_id();

	if (Ticket::shouldStampAppOwnershipTicket(
			g_config.isAddedAppId(appId), Apps::isAddedAppDlcId(appId)))
	{
		msg->set_eresult(static_cast<int32_t>(ERESULT_OK));

		g_pLog->info("Ticket: stamped eresult=OK on AppOwnershipTicket response for AdditionalApp=%u\n",
		             appId);
		return true;
	}

	return false;
}

void Ticket::recvMsg(CProtoBufMsgBase* msg)
{
	switch(msg->type)
	{
		case EMSG_APPOWNERSHIPTICKET_RESPONSE:
			recvAppTicket(msg->getBody<CMsgClientGetAppOwnershipTicketResponse>());
			break;

		case EMSG_ENCRYPTED_APPTICKET_RESPONSE:
			recvEncryptedAppTicket(msg->getBody<CMsgClientRequestEncryptedAppTicketResponse>());
			break;
	}
}

void Ticket::purgeOrphans(const std::unordered_set<uint32_t>& addedAppIds)
{
	// Collect first: the removal path below mutates the maps, and a range-for
	// over a map that is being erased from is undefined.
	std::vector<uint32_t> orphans;

	{
		std::lock_guard<std::mutex> lock(cacheMutex);
		for (const auto& [appId, ticket] : ticketMap)
		{
			if (isOrphaned(appId, addedAppIds)) orphans.push_back(appId);
		}
		for (const auto& [appId, ticket] : encryptedTicketMap)
		{
			if (isOrphaned(appId, addedAppIds)
			    && std::find(orphans.begin(), orphans.end(), appId) == orphans.end())
			{
				orphans.push_back(appId);
			}
		}
	}

	for (const uint32_t appId : orphans)
	{
		std::error_code ec;
		std::filesystem::remove(getTicketPath(appId), ec);
		std::filesystem::remove(getEncryptedTicketPath(appId), ec);

		std::lock_guard<std::mutex> lock(cacheMutex);
		ticketMap.erase(appId);
		encryptedTicketMap.erase(appId);
		oneTimeSteamIdSpoof.erase(appId);
		invalidatedApps.insert(appId);

		g_pLog->info("Ticket: dropped cached ticket for %u, not an added app\n", appId);
	}
}

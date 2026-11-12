// SPDX-License-Identifier: AGPL-3.0-only

#include "depotkey.hpp"

#include "stplugin.hpp"

#include "../log.hpp"

#include <cstdint>
#include <string>

namespace
{
	int nibble(const char c)
	{
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		return -1;
	}

	bool hexToBytes(const std::string& hex, std::string& out)
	{
		if (hex.empty() || hex.size() % 2 != 0)
		{
			return false;
		}

		out.clear();
		out.reserve(hex.size() / 2);

		for (size_t i = 0; i < hex.size(); i += 2)
		{
			const int hi = nibble(hex[i]);
			const int lo = nibble(hex[i + 1]);
			if (hi < 0 || lo < 0)
			{
				return false;
			}
			out.push_back(static_cast<char>((hi << 4) | lo));
		}

		return true;
	}
}

void DepotKey::recvMsg(CNetPacket* pkt)
{
	if (pkt->getProtoBufType() != k_EMsgClientGetDepotDecryptionKeyResponse)
	{
		return;
	}

	auto msg = pkt->deserializeBody<CMsgClientGetDepotDecryptionKeyResponse>();
	if (msg.eresult() == k_EResultOK)
	{
		return;
	}

	const auto* hex = StPlugin::depotKeyFor(msg.depot_id());
	if (!hex)
	{
		return;
	}

	std::string key;
	if (!hexToBytes(*hex, key))
	{
		LOG_NOTIFYWARN("DepotKey: malformed key for depot %u\n", msg.depot_id());
		return;
	}

	const int32_t denied = msg.eresult();
	msg.set_eresult(k_EResultOK);
	msg.set_depot_encryption_key(key.data(), key.size());
	pkt->serialize(msg);

	LOG_INFO("DepotKey: substituted key for depot %u (eresult was %i)\n", msg.depot_id(), denied);
}

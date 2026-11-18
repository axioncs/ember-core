

#pragma once

#include <cstdint>

class CNetPacket;

namespace CmRecv
{
	enum class Transport
	{
		Unknown,
		Classic,
		NetPkt,
	};

	bool isTracked(uint32_t eMsg);

	bool claim(Transport who);

	void dispatchNetPacket(CNetPacket* pkt);
}

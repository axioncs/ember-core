#pragma once

#include "steam.hpp"

#include <cstdint>

struct servernetadr_t
{
	uint16_t connectPort;
	uint16_t queryPort;
	uint32_t ip;
};

struct gameserverdetails_t
{
public:
	union
	{
		servernetadr_t address;
		uint64_t ip64;
	};

	uint8_t __pad_0x0[0x88];
	AppId_t appId;
};

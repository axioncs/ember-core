#pragma once

#include "steam.hpp"

#include "protobufs/steammessages_base.pb.h"

#include "../log.hpp"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

class CNetPacketBody
{
public:

	EMsg type;
	uint32_t headerSize;

};

class CNetPacket
{
public:

	constexpr static unsigned int PROTOBUF_TYPE_MASK = 0x80000000;
	constexpr static unsigned int INVALID_MESSAGE_TYPE = 0xFFFFFFFF;

	constexpr static uint32_t MAX_PLAUSIBLE_SIZE = 100u * 1024 * 1024;

#if defined(__x86_64__)
	constexpr static std::size_t OFF_BODY = 0x8;
	constexpr static std::size_t OFF_SIZE = 0x10;
	constexpr static std::size_t OFF_ORIGBODY = 0x18;
#else
	constexpr static std::size_t OFF_BODY = 0x4;
	constexpr static std::size_t OFF_SIZE = 0x8;
	constexpr static std::size_t OFF_ORIGBODY = 0x10;
#endif

	static inline int s_fieldShift = 0;

private:

#if defined(__x86_64__)
	uint8_t raw_[0x30];
#else
	uint8_t raw_[0x20];
#endif

	template <typename T>
	T fieldGet(std::size_t off) const
	{
		T value;
		std::memcpy(&value, raw_ + off + s_fieldShift, sizeof(T));
		return value;
	}
	template <typename T>
	void fieldSet(std::size_t off, T value)
	{
		std::memcpy(raw_ + off + s_fieldShift, &value, sizeof(T));
	}

	static bool plausibleBody(CNetPacketBody* body)
	{
		const auto address = reinterpret_cast<std::uintptr_t>(body);
#if defined(__x86_64__)
		return address > 0xFFFFFFFFULL && address < 0x0000800000000000ULL;
#else
		return address >= 0x10000u && address < 0xFFFFF000u;
#endif
	}

public:

	CNetPacketBody* body() const { return fieldGet<CNetPacketBody*>(OFF_BODY); }
	void setBody(CNetPacketBody* value) { fieldSet<CNetPacketBody*>(OFF_BODY, value); }
	uint32_t sizeBytes() const { return fieldGet<uint32_t>(OFF_SIZE); }
	void setSize(uint32_t value) { fieldSet<uint32_t>(OFF_SIZE, value); }
	CNetPacketBody* originalBody() const { return fieldGet<CNetPacketBody*>(OFF_ORIGBODY); }
	void setOriginalBody(CNetPacketBody* value) { fieldSet<CNetPacketBody*>(OFF_ORIGBODY, value); }

	static void detectLayout(const CNetPacket* packet)
	{
		static std::atomic<bool> settled{false};
		if (settled.load(std::memory_order_acquire) || packet == nullptr)
			return;
		const auto* base = reinterpret_cast<const uint8_t*>(packet);
		for (const int shift : {0, 8})
		{
			CNetPacketBody* body;
			std::memcpy(&body, base + OFF_BODY + shift, sizeof(body));
			uint32_t size;
			std::memcpy(&size, base + OFF_SIZE + shift, sizeof(size));
			if (!plausibleBody(body)
			    || size < sizeof(CNetPacketBody) || size > MAX_PLAUSIBLE_SIZE)
				continue;
			uint32_t headerSize;
			std::memcpy(&headerSize,
			            reinterpret_cast<const uint8_t*>(body) + offsetof(CNetPacketBody, headerSize),
			            sizeof(headerSize));
			if (headerSize > size - sizeof(CNetPacketBody))
				continue;
			s_fieldShift = shift;
			settled.store(true, std::memory_order_release);
			g_pLog->info("CNetPacket: field layout settled at shift %d\n", shift);
			return;
		}
	}

	bool isValid() const
	{
		return getType() != INVALID_MESSAGE_TYPE;
	}

	EMsg getType() const
	{
		CNetPacketBody* b = body();
		if (!plausibleBody(b))
			return INVALID_MESSAGE_TYPE;
		const uint32_t bytes = sizeBytes();
		if (bytes < sizeof(CNetPacketBody) || bytes > MAX_PLAUSIBLE_SIZE)
			return INVALID_MESSAGE_TYPE;
		return b->type;
	}

	bool isProtoBuf() const
	{
		return isValid() && (getType() & PROTOBUF_TYPE_MASK);
	}

	EMsg getProtoBufType() const
	{
		return getType() & ~PROTOBUF_TYPE_MASK;
	}

	bool deserializeHeader(CMsgProtoBufHeader& header) const;

	template<typename T>
	void serialize(const T& message, const CMsgProtoBufHeader* header)
	{
		if (!isValid())
		{
			return;
		}

		CNetPacketBody* oldBody = body();
		const uint32_t oldSize = sizeBytes();
		constexpr std::size_t headerOffset = sizeof(CNetPacketBody);
		constexpr std::size_t maxPacketSize =
			static_cast<std::size_t>(std::numeric_limits<int>::max());
		const std::size_t headerSize = header
			? header->ByteSizeLong()
			: oldBody->headerSize;
		const std::size_t messageSize = message.ByteSizeLong();
		if (headerSize > maxPacketSize - headerOffset ||
			messageSize > maxPacketSize - headerOffset - headerSize ||
			(!header && headerSize > oldSize - headerOffset))
		{
			return;
		}

		const std::size_t messageOffset = headerOffset + headerSize;
		const std::size_t newSize = messageSize + messageOffset;
		auto* memory = static_cast<uint8_t*>(
			Steam::Plat_Alloc(static_cast<int>(newSize)));
		if (!memory)
		{
			g_pLog->warn("Failed to allocate packet body with size %zu\n", newSize);
			return;
		}
		auto* newBody = reinterpret_cast<CNetPacketBody*>(memory);
		newBody->type = oldBody->type;
		newBody->headerSize = static_cast<uint32_t>(headerSize);
		if (header)
		{
			if (!header->SerializeToArray(
				memory + headerOffset, static_cast<int>(headerSize)))
			{
				Steam::Plat_Free(memory);
				return;
			}
		}
		else
		{
			std::memcpy(memory + headerOffset,
				reinterpret_cast<uint8_t*>(oldBody) + headerOffset,
				headerSize);
		}

		if (!message.SerializeToArray(
			memory + messageOffset, static_cast<int>(messageSize)))
		{
			Steam::Plat_Free(memory);
			return;
		}

		Steam::Plat_Free(oldBody);
		setBody(reinterpret_cast<CNetPacketBody*>(memory));
		setSize(static_cast<uint32_t>(newSize));
		setOriginalBody(reinterpret_cast<CNetPacketBody*>(memory));
	}

	template<typename T>
	void serialize(const T& message)
	{
		serialize(message, nullptr);
	}

	template<typename T>
	T deserializeBody() const
	{
		T message;
		if (!isValid())
		{
			return message;
		}

		CNetPacketBody* b = body();
		const uint32_t bytes = sizeBytes();
		constexpr std::size_t headerOffset = sizeof(CNetPacketBody);
		const std::size_t available = bytes - headerOffset;
		if (b->headerSize > available)
		{
			return message;
		}

		const std::size_t messageOffset = headerOffset + b->headerSize;
		const std::size_t messageSize = bytes - messageOffset;
		if (messageSize > static_cast<std::size_t>(
				std::numeric_limits<int>::max()))
		{
			return message;
		}

		message.ParseFromArray(reinterpret_cast<uint8_t*>(b) + messageOffset,
			static_cast<int>(messageSize));
		return message;
	}

	void free();
};

#if defined(__x86_64__)
static_assert(sizeof(CNetPacket) == 0x30);
#else
static_assert(sizeof(CNetPacket) == 0x20);
#endif

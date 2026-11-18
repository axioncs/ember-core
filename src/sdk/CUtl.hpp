#pragma once

#include <cstddef>
#include <cstdint>

template<typename T>
class CUtlMemory
{
public:

	T* base;
	uint32_t alloc;
	uint32_t growSize;
};

template<typename T>
class CUtlVector
{
public:

	CUtlMemory<T> memory;
	uint32_t size;

	constexpr T* at(uint32_t index)
	{
		if (index >= size)
		{
			return nullptr;
		}

		return &memory.base[index];
	}
};

class CUtlBuffer
{
public:

	CUtlMemory<uint8_t> mem;
	int32_t get;
	int32_t put;
	int32_t offset;
	uint32_t flags;
	uint8_t __pad0x1B[0x8];

	constexpr bool hasBytes(const std::size_t count) const
	{
		return mem.base && put >= 0 &&
			static_cast<std::size_t>(put) >= count;
	}
};

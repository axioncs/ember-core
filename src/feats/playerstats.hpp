

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace PlayerStats
{

namespace detail
{

inline bool readVarint(const uint8_t* d, std::size_t len, std::size_t& pos, uint64_t& out)
{
	uint64_t result = 0;
	int shift = 0;
	while (pos < len && shift <= 63)
	{
		const uint8_t b = d[pos++];
		if (shift == 63 && b > 1) return false;
		result |= static_cast<uint64_t>(b & 0x7F) << shift;
		if (!(b & 0x80))
		{
			out = result;
			return true;
		}
		shift += 7;
	}
	return false;
}

struct Field
{
	uint32_t    number;
	uint8_t     wireType;
	std::size_t start;
	std::size_t end;
	std::size_t valueOff;
	std::size_t valueLen;
};

template <typename Fn>
inline bool walk(const uint8_t* d, std::size_t len, Fn&& fn)
{
	std::size_t pos = 0;
	while (pos < len)
	{
		const std::size_t tagStart = pos;
		uint64_t tag = 0;
		if (!readVarint(d, len, pos, tag)) return false;
		const uint32_t number   = static_cast<uint32_t>(tag >> 3);
		const uint8_t  wireType = static_cast<uint8_t>(tag & 0x7);
		if (number == 0) return false;

		std::size_t valueOff = pos;
		std::size_t valueLen = 0;
		switch (wireType)
		{
		case 0:
		{
			const std::size_t vstart = pos;
			uint64_t tmp = 0;
			if (!readVarint(d, len, pos, tmp)) return false;
			valueOff = vstart;
			valueLen = pos - vstart;
			break;
		}
		case 1:
			if (pos + 8 > len) return false;
			valueOff = pos; valueLen = 8; pos += 8;
			break;
		case 5:
			if (pos + 4 > len) return false;
			valueOff = pos; valueLen = 4; pos += 4;
			break;
		case 2:
		{
			uint64_t l = 0;
			if (!readVarint(d, len, pos, l)) return false;
			if (l > len - pos) return false;
			valueOff = pos; valueLen = static_cast<std::size_t>(l); pos += valueLen;
			break;
		}
		default:
			return false;
		}

		fn(Field{ number, wireType, tagStart, pos, valueOff, valueLen });
	}
	return true;
}

inline void putVarint(std::vector<uint8_t>& out, uint64_t v)
{
	while (v >= 0x80) { out.push_back(static_cast<uint8_t>(v) | 0x80); v >>= 7; }
	out.push_back(static_cast<uint8_t>(v));
}

}

inline std::optional<uint32_t> parseRequestAppId(const uint8_t* d, std::size_t len)
{
	std::optional<uint32_t> found;
	detail::walk(d, len, [&](const detail::Field& f) {
		if (f.number == 2 && f.wireType == 0)
		{
			std::size_t p = f.valueOff;
			uint64_t v = 0;
			if (detail::readVarint(d, len, p, v)) found = static_cast<uint32_t>(v);
		}
	});
	return found;
}

inline std::optional<uint64_t> parseRequestSteamId(const uint8_t* d, std::size_t len)
{
	std::optional<uint64_t> found;
	detail::walk(d, len, [&](const detail::Field& f) {
		if (f.number == 1 && f.wireType == 0)
		{
			std::size_t p = f.valueOff;
			uint64_t v = 0;
			if (detail::readVarint(d, len, p, v)) found = v;
		}
	});
	return found;
}

inline bool hasField(const uint8_t* d, std::size_t len, uint32_t number)
{
	bool present = false;
	detail::walk(d, len, [&](const detail::Field& f) {
		if (f.number == number) present = true;
	});
	return present;
}

inline std::vector<uint8_t> fieldBytes(const uint8_t* d, std::size_t len, uint32_t number)
{
	std::vector<uint8_t> out;
	bool done = false;
	detail::walk(d, len, [&](const detail::Field& f) {
		if (!done && f.number == number && f.wireType == 2)
		{
			out.assign(d + f.valueOff, d + f.valueOff + f.valueLen);
			done = true;
		}
	});
	return out;
}

inline std::vector<uint8_t> buildSpoofedRequest(uint64_t steamid, uint32_t appid)
{
	std::vector<uint8_t> out;
	out.push_back(0x08);
	detail::putVarint(out, steamid);
	out.push_back(0x10);
	detail::putVarint(out, appid);
	return out;
}

inline std::vector<uint8_t> stripResponseStats(const uint8_t* d, std::size_t len)
{
	std::vector<uint8_t> out;
	detail::walk(d, len, [&](const detail::Field& f) {
		if (f.number == 4) return;
		out.insert(out.end(), d + f.start, d + f.end);
	});
	return out;
}

}

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <array>
#include <string>
#include <vector>

namespace IpcFrame
{
	template <size_t N, typename ScanFn>
	inline bool scanWhenCatalogIncomplete(
		const std::array<bool, N>& catalogResolved,
		ScanFn&& scan)
	{
		for (bool resolved : catalogResolved)
		{
			if (!resolved)
			{
				scan();
				return true;
			}
		}
		return false;
	}

	struct Cand
	{
		size_t   offset;
		uint32_t root;
		size_t   available = 0;
	};

	inline constexpr size_t kRootByteOffset = 15;

	inline constexpr size_t kMatchSpan = kRootByteOffset + 4;

	inline std::vector<Cand> scan(const uint8_t* code, size_t size)
	{

		static const int16_t mask[kRootByteOffset] = {
			0xE8, -1, -1, -1, -1,
			0x8B, 0x85, -1, -1, -1, -1,
			0x83, 0xC4, 0x10,
			0x3D,
		};

		std::vector<Cand> out;
		if (size < kMatchSpan)
			return out;

		const size_t last = size - kMatchSpan;
		size_t i = 0;
		while (i <= last)
		{
			const void* hit = std::memchr(code + i, 0xE8, last - i + 1);
			if (hit == nullptr)
				break;
			i = static_cast<size_t>(reinterpret_cast<const uint8_t*>(hit) - code);

			bool ok = true;
			for (size_t k = 1; k < kRootByteOffset; ++k)
			{
				if (mask[k] != -1 && code[i + k] != mask[k])
				{
					ok = false;
					break;
				}
			}
			if (ok)
			{
				const uint32_t root =
					  static_cast<uint32_t>(code[i + 15])
					| static_cast<uint32_t>(code[i + 16]) << 8
					| static_cast<uint32_t>(code[i + 17]) << 16
					| static_cast<uint32_t>(code[i + 18]) << 24;
				out.push_back({ i, root, size - i });
			}
			++i;
		}
		return out;
	}

	inline constexpr uint32_t kMaxRootDrift = 0x10000;

	inline size_t resolveConfident(const std::vector<Cand>& cands, uint32_t seedRoot, uint32_t maxDrift)
	{
		size_t hit     = SIZE_MAX;
		size_t inBand   = 0;
		for (size_t i = 0; i < cands.size(); ++i)
		{
			const uint32_t r = cands[i].root;
			const uint64_t d = (r > seedRoot)
				? static_cast<uint64_t>(r) - seedRoot
				: static_cast<uint64_t>(seedRoot) - r;
			if (d <= maxDrift)
			{
				++inBand;
				hit = i;
			}
		}
		return (inBand == 1) ? hit : SIZE_MAX;
	}

	inline size_t countHighPivots(
		const uint8_t* code,
		size_t size,
		const uint32_t* pivots,
		size_t pivotCount,
		size_t scanSpan = 0x600)
	{
		if (code == nullptr || pivots == nullptr || pivotCount == 0)
			return 0;

		const size_t end = (size < scanSpan) ? size : scanSpan;
		if (end < 5)
			return 0;

		size_t hits = 0;
		for (size_t p = 0; p < pivotCount; ++p)
		{
			for (size_t i = 0; i + 5 <= end; ++i)
			{
				if (code[i] != 0x3D)
					continue;
				const uint32_t live =
					  static_cast<uint32_t>(code[i + 1])
					| static_cast<uint32_t>(code[i + 2]) << 8
					| static_cast<uint32_t>(code[i + 3]) << 16
					| static_cast<uint32_t>(code[i + 4]) << 24;
				if ((live >> 8) == pivots[p])
				{
					++hits;
					break;
				}
			}
		}
		return hits;
	}

	inline uint32_t parseTrailingRoot(const std::string& pattern)
	{
		uint8_t b[4];
		size_t  filled = 0;
		size_t  end    = pattern.size();

		while (filled < 4 && end != 0)
		{
			size_t e = pattern.find_last_not_of(' ', end - 1);
			if (e == std::string::npos) break;
			size_t s = pattern.find_last_of(' ', e);
			size_t tokStart = (s == std::string::npos) ? 0 : s + 1;
			b[filled++] = static_cast<uint8_t>(
				std::strtoul(pattern.substr(tokStart, e - tokStart + 1).c_str(), nullptr, 16));
			if (tokStart == 0) break;
			end = tokStart - 1;
		}
		if (filled < 4)
			return 0;

		return static_cast<uint32_t>(b[3])
			| static_cast<uint32_t>(b[2]) << 8
			| static_cast<uint32_t>(b[1]) << 16
			| static_cast<uint32_t>(b[0]) << 24;
	}

	inline void setTrailingRoot(std::string& pattern, uint32_t root)
	{

		size_t end   = pattern.size();
		size_t cut   = std::string::npos;
		for (int i = 0; i < 4 && end != 0; ++i)
		{
			size_t e = pattern.find_last_not_of(' ', end - 1);
			if (e == std::string::npos) break;
			size_t s = pattern.find_last_of(' ', e);
			cut = (s == std::string::npos) ? 0 : s + 1;
			if (cut == 0) break;
			end = cut - 1;
		}
		if (cut == std::string::npos)
			return;

		char tail[16];
		std::snprintf(tail, sizeof(tail), "%02X %02X %02X %02X",
			root & 0xFF, (root >> 8) & 0xFF, (root >> 16) & 0xFF, (root >> 24) & 0xFF);
		pattern.replace(cut, std::string::npos, tail);
	}
}



#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace ReconcilePin
{

	bool setup();

	void remove();

	namespace detail
	{

		inline constexpr std::size_t kBuilderCallOff = 0x183;

		inline constexpr std::size_t kBuilderCallWindow = 0x40;

		template <typename TargetExecutable>
		inline long findBuilderCallRel32(
			const std::uint8_t* code, std::size_t len, std::uintptr_t codeAddr,
			std::size_t expectedOff, std::size_t window,
			TargetExecutable targetExecutable)
		{

			const auto callTargetOk = [&](std::size_t off) -> bool
			{
				if (off + 8 > len) return false;
				if (code[off] != 0xE8) return false;
				if (code[off + 5] != 0x83 || code[off + 6] != 0xC4
				    || code[off + 7] != 0x20)
					return false;
				std::int32_t rel = 0;
				std::memcpy(&rel, code + off + 1, sizeof(rel));
				const std::uintptr_t target = codeAddr + off + 5 +
					static_cast<std::uintptr_t>(static_cast<std::intptr_t>(rel));
				return targetExecutable(target);
			};

			if (callTargetOk(expectedOff)) return static_cast<long>(expectedOff);

			long found = -1;
			const std::size_t lo = expectedOff > window ? expectedOff - window : 0;
			const std::size_t hi = expectedOff + window;
			for (std::size_t off = lo; off <= hi; ++off)
			{
				if (off == expectedOff) continue;
				if (!callTargetOk(off)) continue;
				if (found != -1) return -1;
				found = static_cast<long>(off);
			}
			return found;
		}

		inline constexpr std::size_t kMaxCtxDepotVectorOff = 0x1000;

		inline constexpr std::size_t kCtxDepotLoopScanLen = 0x600;

		inline long findCtxDepotVectorOffset(
			const std::uint8_t* code, std::size_t len)
		{
			static constexpr std::uint8_t kHead[] = {
				0x8B, 0x7D, 0x08, 0x89, 0xF0, 0xC1, 0xE0, 0x05, 0x03};
			long found = -1;
			for (std::size_t off = 0; off + sizeof(kHead) + 2 <= len; ++off)
			{
				if (std::memcmp(code + off, kHead, sizeof(kHead)) != 0) continue;
				const std::size_t modrm = off + sizeof(kHead);
				std::uint32_t disp = 0;
				if (code[modrm] == 0x47)
				{
					disp = code[modrm + 1];
				}
				else if (code[modrm] == 0x87 && modrm + 5 <= len)
				{
					std::memcpy(&disp, code + modrm + 1, sizeof(disp));
				}
				else
				{
					continue;
				}
				if (found != -1) return -1;
				if (disp == 0 || disp % 4 != 0 || disp > kMaxCtxDepotVectorOff)
					return -1;
				found = static_cast<long>(disp);
			}
			return found;
		}
	}
}

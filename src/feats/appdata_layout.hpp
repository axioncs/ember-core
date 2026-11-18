#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

namespace AppDataLayout
{
struct Layout
{
	std::uint32_t skipOffset;
	std::uint32_t shaOffset;

	friend constexpr bool operator==(const Layout&, const Layout&) = default;
};

namespace detail
{
inline constexpr std::size_t kShaBytes = 20;
inline constexpr std::size_t kMaximumFieldEnd = 0x1000;

inline std::optional<std::int64_t> readDisplacement(
	std::span<const std::uint8_t> instruction,
	std::size_t offset,
	std::size_t width
) noexcept
{
	if (offset > instruction.size() || width > instruction.size() - offset)
	{
		return std::nullopt;
	}

	if (width == 1)
	{
		const std::uint8_t raw = instruction[offset];
		return raw < 0x80
			? static_cast<std::int64_t>(raw)
			: static_cast<std::int64_t>(raw) - 0x100;
	}

	if (width != 4)
	{
		return std::nullopt;
	}

	const std::uint32_t raw =
		static_cast<std::uint32_t>(instruction[offset]) |
		(static_cast<std::uint32_t>(instruction[offset + 1]) << 8) |
		(static_cast<std::uint32_t>(instruction[offset + 2]) << 16) |
		(static_cast<std::uint32_t>(instruction[offset + 3]) << 24);

	return (raw & 0x80000000U) != 0
		? static_cast<std::int64_t>(raw) - 0x100000000LL
		: static_cast<std::int64_t>(raw);
}

inline std::optional<std::int64_t> directDisplacement(
	std::span<const std::uint8_t> instruction,
	std::uint8_t opcode,
	std::optional<std::uint8_t> requiredReg,
	bool requiresZeroImmediate
) noexcept
{
	if (instruction.size() < 2 || instruction[0] != opcode)
	{
		return std::nullopt;
	}

	const std::uint8_t modrm = instruction[1];
	const std::uint8_t mod = static_cast<std::uint8_t>(modrm >> 6);
	const std::uint8_t reg = static_cast<std::uint8_t>((modrm >> 3) & 0x07);
	const std::uint8_t rm = static_cast<std::uint8_t>(modrm & 0x07);

	if ((mod != 1 && mod != 2) || rm == 4 ||
		(requiredReg.has_value() && reg != *requiredReg))
	{
		return std::nullopt;
	}

	const std::size_t displacementWidth = mod == 1 ? 1 : 4;
	const auto displacement = readDisplacement(instruction, 2, displacementWidth);
	if (!displacement.has_value())
	{
		return std::nullopt;
	}

	if (requiresZeroImmediate)
	{
		const std::size_t immediateOffset = 2 + displacementWidth;
		if (immediateOffset >= instruction.size() ||
			instruction[immediateOffset] != 0)
		{
			return std::nullopt;
		}
	}

	return displacement;
}

inline bool inBounds(std::int64_t offset, std::size_t width) noexcept
{
	if (offset < 0 || width > kMaximumFieldEnd)
	{
		return false;
	}

	const auto unsignedOffset = static_cast<std::uint64_t>(offset);
	return unsignedOffset < kMaximumFieldEnd &&
		unsignedOffset <= kMaximumFieldEnd - width;
}

inline bool overlaps(
	std::int64_t firstOffset,
	std::size_t firstWidth,
	std::int64_t secondOffset,
	std::size_t secondWidth
) noexcept
{
	if (!inBounds(firstOffset, firstWidth) ||
		!inBounds(secondOffset, secondWidth))
	{
		return true;
	}

	const auto first = static_cast<std::size_t>(firstOffset);
	const auto second = static_cast<std::size_t>(secondOffset);
	const auto firstEnd = first + firstWidth;
	const auto secondEnd = second + secondWidth;
	return first < secondEnd && second < firstEnd;
}
}

inline std::optional<Layout> derive(
	std::span<const std::uint8_t> skipInstruction,
	std::span<const std::uint8_t> shaInstruction
) noexcept
{
	std::optional<std::int64_t> skip;
	if (skipInstruction.size() >= 4 &&
		skipInstruction[0] == 0x0F &&
		skipInstruction[1] == 0xB6 &&
		skipInstruction[2] == 0x40)
	{

		skip = detail::readDisplacement(skipInstruction, 3, 1);
	}
	else
	{

		skip = detail::directDisplacement(
			skipInstruction, 0x80, std::uint8_t{7}, true
		);
	}

	std::optional<std::int64_t> sha;
	if (shaInstruction.size() >= 4 &&
		shaInstruction[0] == 0x48 &&
		shaInstruction[1] == 0x39 &&
		(shaInstruction[2] == 0x58 || shaInstruction[2] == 0x4B))
	{

		sha = detail::readDisplacement(shaInstruction, 3, 1);
	}
	else
	{

		sha = detail::directDisplacement(
			shaInstruction, 0x8D, std::nullopt, false
		);
	}
	if (!skip.has_value() || !sha.has_value() ||
		!detail::inBounds(*skip, 1) ||
		!detail::inBounds(*sha, detail::kShaBytes) ||
		detail::overlaps(*skip, 1, *sha, detail::kShaBytes))
	{
		return std::nullopt;
	}

	return Layout{
		static_cast<std::uint32_t>(*skip),
		static_cast<std::uint32_t>(*sha)
	};
}
}

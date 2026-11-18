#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <optional>
#include <vector>

namespace MemHlp
{
	struct PatternScanRangeResult
	{
		uintptr_t address = std::numeric_limits<uintptr_t>::max();
		std::size_t matches = 0;
	};

	struct PatternScanTotal
	{
		uintptr_t address = std::numeric_limits<uintptr_t>::max();
		std::size_t matches = 0;

		bool resolved() const noexcept
		{
			return matches == 1 && address != std::numeric_limits<uintptr_t>::max();
		}
	};

	inline void accumulateScan(PatternScanTotal& total,
		const PatternScanRangeResult& range) noexcept
	{
		if (range.matches == 0)
		{
			return;
		}
		total.matches += range.matches;
		if (total.address == std::numeric_limits<uintptr_t>::max())
		{
			total.address = range.address;
		}
	}

	inline constexpr std::size_t kMaxConvergenceCandidates = 256;

	inline std::optional<uintptr_t> convergedTarget(
		const std::vector<uintptr_t>& targets) noexcept
	{
		if (targets.empty())
		{
			return std::nullopt;
		}
		for (const uintptr_t target : targets)
		{
			if (target != targets.front())
			{
				return std::nullopt;
			}
		}
		return targets.front();
	}

	inline PatternScanRangeResult scanPatternRange(const std::vector<int16_t>& bytes,
		uintptr_t begin, uintptr_t end, bool countDuplicates,
		std::vector<uintptr_t>* addresses = nullptr, std::size_t addressCap = 0)
	{
		PatternScanRangeResult result;
		const std::size_t patternSize = bytes.size();
		if (patternSize == 0 || end < begin || end - begin < patternSize)
		{
			return result;
		}

		const std::size_t candidateCount =
			static_cast<std::size_t>(end - begin - patternSize + 1);
		std::size_t firstLiteral = 0;
		while (firstLiteral < patternSize && bytes[firstLiteral] < 0)
		{
			++firstLiteral;
		}

		const auto matchesAt = [&](const std::uint8_t* candidate)
		{
			for (std::size_t i = 0; i < patternSize; ++i)
			{
				if (bytes[i] >= 0 && candidate[i] != static_cast<std::uint8_t>(bytes[i]))
				{
					return false;
				}
			}
			return true;
		};

		const auto record = [&](const std::uint8_t* candidate)
		{
			++result.matches;
			if (result.matches == 1 || countDuplicates)
			{
				result.address = reinterpret_cast<uintptr_t>(candidate);
			}
			if (addresses != nullptr && addresses->size() < addressCap)
			{
				addresses->push_back(reinterpret_cast<uintptr_t>(candidate));
			}
		};

		const auto* data = reinterpret_cast<const std::uint8_t*>(begin);
		if (firstLiteral == patternSize)
		{
			for (std::size_t offset = 0; offset < candidateCount; ++offset)
			{
				const auto* candidate = data + offset;
				if (matchesAt(candidate))
				{
					record(candidate);
					if (!countDuplicates)
					{
						return result;
					}
				}
			}
			return result;
		}

		const std::uint8_t needle = static_cast<std::uint8_t>(bytes[firstLiteral]);
		const auto* search = data + firstLiteral;
		const auto* literalEnd = search + candidateCount;
		while (search < literalEnd)
		{
			const auto* literal = reinterpret_cast<const std::uint8_t*>(
				std::memchr(search, needle, static_cast<std::size_t>(literalEnd - search)));
			if (literal == nullptr)
			{
				break;
			}

			const auto* candidate = literal - firstLiteral;
			if (matchesAt(candidate))
			{
				record(candidate);
				if (!countDuplicates)
				{
					return result;
				}
			}
			search = literal + 1;
		}

		return result;
	}
}

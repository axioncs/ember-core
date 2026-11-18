

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "feats/hotreload_types.hpp"

namespace OwnerWork
{

	inline constexpr unsigned int kDefaultMaxStaleMs = 30000;
	inline constexpr unsigned int kMaxMaxStaleMs = 600000;

	inline constexpr int kIdleTickMs = 500;

	enum class Mode : std::uint8_t
	{
		Queued = 0,
		OnOwner,
		NoOwner,
		Overflow,
		Disabled,
		Abandoned,
	};

	const char* modeName(Mode mode);

	inline unsigned int parseBoundedMs(const char* value, unsigned int fallback,
	                                   unsigned int maxMs, bool* rejected = nullptr)
	{
		if (rejected != nullptr)
			*rejected = false;
		if (value == nullptr || value[0] == '\0')
			return fallback;

		unsigned long parsed = 0;
		for (const char* p = value; *p != '\0'; ++p)
		{
			if (*p < '0' || *p > '9' || parsed > maxMs)
			{
				if (rejected != nullptr)
					*rejected = true;
				return fallback;
			}
			parsed = parsed * 10 + static_cast<unsigned long>(*p - '0');
		}
		if (parsed > maxMs)
		{
			if (rejected != nullptr)
				*rejected = true;
			return fallback;
		}
		return static_cast<unsigned int>(parsed);
	}

	inline bool parseQueueEnabled(const char* value, bool fallback = true,
	                              bool* rejected = nullptr)
	{
		if (rejected != nullptr)
			*rejected = false;
		if (value == nullptr || value[0] == '\0')
			return fallback;

		std::string v(value);
		for (char& c : v)
			c = static_cast<char>((c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c);

		if (v == "0" || v == "off" || v == "no" || v == "false" || v == "disable"
		    || v == "disabled" || v == "n")
			return false;
		if (v == "1" || v == "on" || v == "yes" || v == "true" || v == "enable"
		    || v == "enabled" || v == "y")
			return true;

		if (rejected != nullptr)
			*rejected = true;
		return fallback;
	}

	void init();

	void latchOwnerThread();

	bool ownerLatched();
	bool onOwnerThread();

	inline bool compatExecutionAllowed(bool onOwner) noexcept
	{
		return onOwner;
	}

	inline constexpr bool compatReadinessRequestValid(
		std::uint32_t appId,
		std::uint64_t ) noexcept
	{
		return appId != 0;
	}

	void drainOnOwnerFrame();

	Mode submitHotAdd(const std::vector<std::uint32_t>& appIds);

	Mode submitManagedState(const PackageSnapshot& snapshot);

	Mode submitCompatReadiness(
		std::uint32_t appId,
		std::uint64_t managedGeneration);

	Mode submitInstallApp(std::uint32_t appId, std::uint32_t library);

	void idleTick();

	void notePlacement();

	bool shutdownIfPlaced();

	std::size_t pendingDepth();
}

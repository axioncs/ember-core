

#pragma once

#include <charconv>
#include <cstdint>
#include <limits>
#include <string_view>

namespace AppInfoProvision
{

enum class CacheReadiness
{
	Missing,
	Invalid,
	Fresh,
	ValidStale,
	Busy,
	Unverified,
};

namespace cache
{

struct CacheMetadataView
{
	std::uint32_t appId = 0;
	std::uint32_t changeNumber = 0;
	std::uint64_t wireSize = 0;
	std::string_view shaBase64;
	bool hasNormalized = false;
	bool normalized = false;
	bool hasSynthetic = false;
	bool synthetic = false;
};

inline bool parseCacheMetadata(
	std::string_view text,
	CacheMetadataView& output) noexcept
{
	CacheMetadataView parsed;
	bool appIdSeen = false;
	bool wireSizeSeen = false;
	bool shaSeen = false;
	bool changeSeen = false;
	bool normalizedSeen = false;
	bool syntheticSeen = false;

	const auto parseUnsigned = [](
		std::string_view value,
		std::uint64_t& result) noexcept
	{
		if (value.empty()) return false;
		result = 0;
		const auto converted = std::from_chars(
			value.data(), value.data() + value.size(), result, 10);
		return converted.ec == std::errc{} &&
			converted.ptr == value.data() + value.size();
	};
	const auto parseBool = [](
		std::string_view value,
		bool& result) noexcept
	{
		if (value == "true") { result = true; return true; }
		if (value == "false") { result = false; return true; }
		return false;
	};

	std::size_t cursor = 0;
	while (cursor < text.size())
	{
		const std::size_t newline = text.find('\n', cursor);
		const std::size_t end = newline == std::string_view::npos
			? text.size() : newline;
		const std::string_view line = text.substr(cursor, end - cursor);
		cursor = newline == std::string_view::npos ? text.size() : newline + 1;
		if (line.empty() || line.find('\r') != std::string_view::npos)
			return false;

		const std::size_t separator = line.find(": ");
		if (separator == std::string_view::npos || separator == 0 ||
			separator + 2 >= line.size())
		{
			return false;
		}
		const std::string_view key = line.substr(0, separator);
		const std::string_view value = line.substr(separator + 2);
		std::uint64_t number = 0;

		if (key == "appid")
		{
			if (appIdSeen || !parseUnsigned(value, number) || number == 0 ||
				number > std::numeric_limits<std::uint32_t>::max())
				return false;
			appIdSeen = true;
			parsed.appId = static_cast<std::uint32_t>(number);
		}
		else if (key == "change_number")
		{
			if (changeSeen || !parseUnsigned(value, number) ||
				number > std::numeric_limits<std::uint32_t>::max())
				return false;
			changeSeen = true;
			parsed.changeNumber = static_cast<std::uint32_t>(number);
		}
		else if (key == "wire_size")
		{
			if (wireSizeSeen || !parseUnsigned(value, number) || number == 0)
				return false;
			wireSizeSeen = true;
			parsed.wireSize = number;
		}
		else if (key == "sha_b64")
		{

			if (shaSeen || value.size() != 28 || value.back() != '=')
				return false;
			for (const unsigned char byte : value.substr(0, value.size() - 1))
			{
				const bool valid =
					(byte >= 'A' && byte <= 'Z') ||
					(byte >= 'a' && byte <= 'z') ||
					(byte >= '0' && byte <= '9') || byte == '+' || byte == '/';
				if (!valid) return false;
			}
			shaSeen = true;
			parsed.shaBase64 = value;
		}
		else if (key == "normalized")
		{
			if (normalizedSeen || !parseBool(value, parsed.normalized))
				return false;
			normalizedSeen = true;
			parsed.hasNormalized = true;
		}
		else if (key == "synthetic")
		{
			if (syntheticSeen || !parseBool(value, parsed.synthetic))
				return false;
			syntheticSeen = true;
			parsed.hasSynthetic = true;
		}
		else
		{
			return false;
		}
	}

	if (!appIdSeen || !wireSizeSeen || !shaSeen)
		return false;
	output = parsed;
	return true;
}

struct CacheValidationKey
{
	uint32_t appId = 0;
	long long mtimeSecs = 0;
	long long mtimeNsecs = 0;
	long long size = 0;
	std::uint64_t inode = 0;
	long long metadataMtimeSecs = 0;
	long long metadataMtimeNsecs = 0;
	long long metadataSize = 0;
	std::uint64_t metadataInode = 0;

	bool operator==(const CacheValidationKey& other) const noexcept
	{
		return appId == other.appId
		    && mtimeSecs == other.mtimeSecs
		    && mtimeNsecs == other.mtimeNsecs
		    && size == other.size
		    && inode == other.inode
		    && metadataMtimeSecs == other.metadataMtimeSecs
		    && metadataMtimeNsecs == other.metadataMtimeNsecs
		    && metadataSize == other.metadataSize
		    && metadataInode == other.metadataInode;
	}

	bool operator<(const CacheValidationKey& other) const noexcept
	{
		if (appId != other.appId) return appId < other.appId;
		if (mtimeSecs != other.mtimeSecs) return mtimeSecs < other.mtimeSecs;
		if (mtimeNsecs != other.mtimeNsecs)
			return mtimeNsecs < other.mtimeNsecs;
		if (size != other.size) return size < other.size;
		if (inode != other.inode) return inode < other.inode;
		if (metadataMtimeSecs != other.metadataMtimeSecs)
			return metadataMtimeSecs < other.metadataMtimeSecs;
		if (metadataMtimeNsecs != other.metadataMtimeNsecs)
			return metadataMtimeNsecs < other.metadataMtimeNsecs;
		if (metadataSize != other.metadataSize)
			return metadataSize < other.metadataSize;
		return metadataInode < other.metadataInode;
	}
};

enum class CacheUse
{
	None,
	Fresh,
	Fallback,
};

inline bool shouldPreserveCacheFromRawPics(bool hasNormalizedMarker,
                                           bool normalized)
{
	return !hasNormalizedMarker || normalized;
}

struct LocalAuthorityFacts
{
	bool managed = false;
	bool active = false;
	bool synthetic = false;
	bool cacheValid = false;
	bool hasNormalizedMarker = false;
	bool normalized = false;
};

inline bool locallyAuthoritative(const LocalAuthorityFacts& facts) noexcept
{
	if (!facts.active) return false;
	if (facts.synthetic) return true;
	return facts.managed && facts.cacheValid &&
		shouldPreserveCacheFromRawPics(
			facts.hasNormalizedMarker, facts.normalized);
}

inline bool cachePublicationAllowed(bool managed,
                                    std::uint64_t expectedGeneration,
                                    std::uint64_t currentGeneration)
{
	return managed && expectedGeneration == currentGeneration;
}

inline bool protonPublicationAllowed(bool managed,
                                      std::uint64_t expectedGeneration,
                                      std::uint64_t currentGeneration)
{
	return cachePublicationAllowed(managed, expectedGeneration,
	                               currentGeneration);
}

inline bool terminalResultStillApplies(bool recorded,
                                       std::uint64_t recordedGeneration,
                                       std::uint64_t currentGeneration)
{
	return recorded && recordedGeneration == currentGeneration;
}

inline bool syntheticMarkerConsistent(bool synthetic, bool markerPresent)
{
	return synthetic == markerPresent;
}

inline bool syntheticMarkerPublicationConsistent(bool synthetic,
                                                 bool operationSucceeded,
                                                 bool markerPresent)
{
	return operationSucceeded && syntheticMarkerConsistent(synthetic, markerPresent);
}

inline bool syntheticMarkerStateConsistent(bool hasSyntheticMetadata,
                                            bool synthetic,
                                            bool markerPresent)
{

	if (!hasSyntheticMetadata) return true;
	return syntheticMarkerConsistent(synthetic, markerPresent);
}

inline bool retainedSyntheticMarkerProtectionAllowed(
    bool markerPresent, bool metadataPresent, bool activeCompatibility)
{
	return markerPresent && !metadataPresent && activeCompatibility;
}

inline bool shouldPreserveSyntheticMarker(bool retainCompatibility,
                                           bool isSynthetic)
{
	return retainCompatibility && isSynthetic;
}

inline bool shouldInvalidateDlcMetadata(bool basePublicationSucceeded)
{
	return basePublicationSucceeded;
}

struct CacheRecordFacts
{
	unsigned int requestedAppId = 0;
	unsigned int metadataAppId = 0;
	unsigned long long declaredSize = 0;
	unsigned long long actualSize = 0;
	unsigned long long shaSize = 0;
	bool shaMatches = false;
	bool parsed = false;
	bool hasUsableContent = false;
};

inline bool isCacheRecordValid(const CacheRecordFacts& facts)
{
	return facts.requestedAppId != 0
	    && facts.metadataAppId == facts.requestedAppId
	    && facts.declaredSize > 0
	    && facts.declaredSize == facts.actualSize
	    && facts.shaSize == 20
	    && facts.shaMatches
	    && facts.parsed
	    && facts.hasUsableContent;
}

inline CacheUse chooseCacheUse(bool cacheValid, bool cacheFresh,
                               bool refreshUnavailable)
{
	if (!cacheValid) return CacheUse::None;
	if (cacheFresh) return CacheUse::Fresh;
	return refreshUnavailable ? CacheUse::Fallback : CacheUse::None;
}

inline bool shouldValidateCache(bool cacheFresh, bool refreshUnavailable)
{
	return cacheFresh || refreshUnavailable;
}

inline bool wireSizeMatches(unsigned long long actualSize,
                            unsigned long long declaredSize)
{
	return declaredSize > 0 && actualSize == declaredSize;
}

inline bool isBufferReusable(bool bufExists, long long mtimeSecs,
                             long long nowSecs, long long ttlSecs)
{
	if (!bufExists)    return false;
	if (ttlSecs <= 0)  return false;

	const long long age = nowSecs - mtimeSecs;
	if (age < 0)       return false;
	return age < ttlSecs;
}

}
}

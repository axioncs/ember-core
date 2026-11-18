

#pragma once

#include <cstdint>
#include <vector>

struct PackageSnapshot
{
	std::uint64_t generation = 0;
	std::vector<std::uint32_t> appIds;
	std::vector<std::uint32_t> depotIds;
	bool metadataComplete = false;

	std::vector<std::uint32_t> addedAppIds;

	std::vector<std::uint32_t> appInfoRequestIds;

	bool operator==(const PackageSnapshot&) const = default;
};

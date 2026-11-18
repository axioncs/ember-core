

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ManifestStore
{
	using ArchivedGidIndex = std::unordered_map<std::uint32_t, std::uint64_t>;

	std::string dir();

	void archiveDepot(uint32_t depotId);
	void archiveDepots(const std::vector<uint32_t>& depotIds);

	bool archiveManifest(uint32_t depotId, uint64_t gid);

	bool publishDownloadedManifest(uint32_t depotId, uint64_t gid,
	                               const std::string& sourcePath);

	bool isInDepotcache(uint32_t depotId, uint64_t gid);
	bool isArchived(uint32_t depotId, uint64_t gid);

	std::optional<uint64_t> installedSize(uint32_t depotId, uint64_t gid);
	void cacheInstalledSize(uint32_t depotId, uint64_t gid, uint64_t size);

	bool restoreToDepotcache(uint32_t depotId, uint64_t gid);

	bool markPreferredGid(uint32_t depotId, uint64_t gid);
	uint64_t preferredArchivedGid(uint32_t depotId, uint64_t excludeGid);

	uint64_t bestArchivedGid(uint32_t depotId, uint64_t excludeGid);

	ArchivedGidIndex archivedGidIndex();

	void purgeDepots(const std::vector<uint32_t>& depotIds);
}

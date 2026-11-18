

#pragma once

#include "dlcids.hpp"
#include "dlc_metadata.hpp"
#include "manifeststore.hpp"
#include "provision_cache.hpp"
#include "provision_refresh.hpp"
#include "provision_result.hpp"
#include "provision_terminal.hpp"
#include "../config_path.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace AppInfoProvision
{

enum class CacheProbeMode { BlockingValidate, NonBlockingMemoOnly };

struct CacheProbe
{
	CacheReadiness readiness = CacheReadiness::Missing;
	std::uint32_t changeNumber = 0;
};

CacheProbe probeCache(std::uint32_t appId, CacheProbeMode mode);

std::int64_t cachePairMtimeSecs(std::uint32_t appId) noexcept;

std::int64_t dlcMetadataCacheMtimeSecs(std::uint32_t appId) noexcept;
std::string localContentFingerprint(std::uint32_t appId);
std::string localContentFingerprint(
	std::uint32_t appId,
	const ManifestStore::ArchivedGidIndex& archivedGids);
inline std::string fingerprintIndexedLocalInputs(
	std::vector<ProvisionTerminal::LocalInput> inputs,
	const ManifestStore::ArchivedGidIndex& archivedGids)
{
	for (auto& input : inputs)
	{
		const auto found = archivedGids.find(input.depotId);
		input.manifestGid = found == archivedGids.end() ? 0 : found->second;
	}
	return ProvisionTerminal::fingerprint(std::move(inputs));
}

inline std::optional<cache::CacheValidationKey> validatedPublicationIdentity(
	bool publicationSucceeded,
	bool validationSucceeded,
	std::optional<cache::CacheValidationKey> postPublicationIdentity) noexcept
{
	if (!publicationSucceeded || !validationSucceeded ||
		!postPublicationIdentity)
	{
		return std::nullopt;
	}
	return postPublicationIdentity;
}

template <typename Memo, typename Value>
inline bool memoizeValidatedPublication(
	Memo& memo,
	bool publicationSucceeded,
	bool validationSucceeded,
	std::optional<cache::CacheValidationKey> postPublicationIdentity,
	Value&& value)
{
	const auto identity = validatedPublicationIdentity(
		publicationSucceeded, validationSucceeded,
		std::move(postPublicationIdentity));
	if (!identity) return false;
	memo[*identity] = std::forward<Value>(value);
	return true;
}

void primeTerminalMemo(std::uint32_t appId, std::string_view fingerprint);
void primeTerminalMemo(std::uint32_t appId);

inline bool requestNeedsFetch(const RefreshRequest& request,
	CacheReadiness readiness,
	std::uint32_t cachedChangeNumber) noexcept
{
	if (request.appId == 0) return false;
	const std::uint8_t nonMetadataReasons = static_cast<std::uint8_t>(
		request.reasons & ~reasonMask(RefreshReason::DlcMetadata));
	if (nonMetadataReasons == 0) return false;
	if (request.forceRefresh) return true;
	if (readiness == CacheReadiness::Missing ||
	    readiness == CacheReadiness::Invalid ||
	    readiness == CacheReadiness::Unverified ||
	    readiness == CacheReadiness::Busy)
		return true;
	return request.minimumChangeNumber > cachedChangeNumber;
}

inline bool cachedRuntimePublicationAllowed(
	const RefreshRequest& request,
	CacheReadiness readiness) noexcept
{
	if (!request.publishRuntime || request.appId == 0) return false;
	const std::uint8_t nonMetadataReasons = static_cast<std::uint8_t>(
		request.reasons & ~reasonMask(RefreshReason::DlcMetadata));
	if (nonMetadataReasons == 0) return false;
	return readiness == CacheReadiness::Fresh ||
		readiness == CacheReadiness::ValidStale;
}

enum class ColdStartMode { StartupRequireUsablePair, RuntimeMissingOnly };

inline bool coldStartPrimesTerminalMemo(ColdStartMode mode) noexcept
{
	return mode == ColdStartMode::StartupRequireUsablePair;
}

inline std::unordered_set<std::uint32_t> selectColdStartApps(
	const std::vector<std::pair<std::uint32_t, CacheReadiness>>& candidates,
	ColdStartMode )
{
	std::unordered_set<std::uint32_t> selected;
	for (const auto& [appId, readiness] : candidates)
	{
		if (appId == 0) continue;
		const bool recoveryNeeded = readiness == CacheReadiness::Missing ||
			readiness == CacheReadiness::Invalid;
		if (recoveryNeeded)
			selected.insert(appId);
	}
	return selected;
}

struct TerminalObservation
{
	bool applies = false;
	std::uint32_t changeNumber = 0;
	ProvisionOutcome outcome = ProvisionOutcome::IncompleteContent;
};

TerminalObservation observeTerminal(
	std::uint32_t appId, std::uint32_t observedChangeNumber);

std::mutex& provisioningPassMutex();

void sha1Bytes(const void* data, std::size_t size, std::uint8_t out[20]);

struct CachePublicationToken
{
	bool managed = false;
	std::uint64_t generation = 0;
};

std::mutex& cachePublicationMutex();
std::uint64_t cachePublicationGenerationLocked(uint32_t appId);
CachePublicationToken snapshotCachePublication(uint32_t appId);

DlcInjectionIds collectDlcAppIdsForAddedApps(bool* complete = nullptr);

bool readValidatedDlcMetadataCache(
	std::uint32_t baseAppId,
	std::uint64_t expectedGeneration,
	DlcMetadata::CacheRecord& record);

bool provisionApp(uint32_t appId, const std::string& appinfoVdfPath);

int provisionAllAddedApps(const std::string& appinfoVdfPath,
                          bool allowConfigWrite = true);

struct ProvisionPassSummary
{
	std::size_t requested = 0;
	std::size_t fetched = 0;
	std::size_t updated = 0;
	std::size_t ready = 0;
	std::size_t fallback = 0;
	std::size_t terminal = 0;
	std::size_t failed = 0;
};

ProvisionPassSummary provisionRequestedApps(
	const std::string& appinfoVdfPath,
	const std::vector<RefreshRequest>& requests,
	bool allowConfigWrite = false);

void flushPendingProtonMappings();

bool requiresProton(std::uint32_t appId) noexcept;

int provisionColdStartApps(
    const std::string& appinfoVdfPath,
    const std::unordered_set<std::uint32_t>& candidates,
    ColdStartMode mode,
    std::unordered_set<uint32_t>* sanitizedApps = nullptr,
    bool allowConfigWrite = false);

bool asyncProvisioningEnabled();

void refreshInBackground(
    const std::string& appinfoVdfPath,
    const std::vector<RefreshRequest>& requests);

inline std::string cacheLockPath()
{
	return ConfigPath::emberConfigDir(
	           std::getenv("XDG_CONFIG_HOME"), std::getenv("HOME")) +
	       "/cache/.picsbuffer.lock";
}

bool forgetApp(uint32_t appId);

bool forgetManagedSourceApp(uint32_t appId);

bool cacheMarkerAllowsRead(uint32_t appId);

bool publishCachePairLocked(uint32_t appId, const std::string& wire,
                            const std::string& metadata, bool synthetic,
                            bool markerBefore, std::string& error);

bool memoizePublishedCachePairLocked(uint32_t appId);

bool readValidatedCacheBuffer(uint32_t appId, std::string& buffer);

std::unordered_set<std::uint32_t> locallyAuthoritativeApps(
	const std::unordered_set<std::uint32_t>& managedCandidates,
	const std::unordered_set<std::uint32_t>& activeCandidates);

void clearCacheReadInvalidation(uint32_t appId);

bool isSynthesizedApp(uint32_t appId);

}

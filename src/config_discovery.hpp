#pragma once

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace ConfigDiscovery
{

inline constexpr std::size_t kMaxManagedSourceApps = 4096;

struct ManagedSourceSelection
{
	std::unordered_set<uint32_t> active;
	std::size_t discovered = 0;
	std::size_t ignored = 0;
};

inline ManagedSourceSelection selectManagedSources(
	const std::unordered_set<uint32_t>& stplugApps,
	const std::unordered_set<uint32_t>& luaYamlApps,
	const std::unordered_set<uint32_t>& previouslyActive = {},
	std::size_t maxApps = kMaxManagedSourceApps,
	const std::unordered_set<uint32_t>& installedApps = {})
{
	ManagedSourceSelection result;
	std::unordered_set<uint32_t> discovered = stplugApps;
	discovered.insert(luaYamlApps.begin(), luaYamlApps.end());
	result.discovered = discovered.size();
	result.active.reserve(std::min(maxApps, result.discovered));

	const auto addSorted = [&](const std::unordered_set<uint32_t>& candidates,
	                           bool requireDiscovered) {
		std::vector<uint32_t> sorted(candidates.begin(), candidates.end());
		std::sort(sorted.begin(), sorted.end());
		for (const uint32_t appId : sorted)
		{
			if (result.active.size() >= maxApps) break;
			if (appId == 0 ||
			    (requireDiscovered && !discovered.contains(appId))) continue;
			result.active.insert(appId);
		}
	};

	addSorted(luaYamlApps, false);
	addSorted(installedApps, true);
	addSorted(previouslyActive, true);
	addSorted(stplugApps, false);
	result.ignored = result.discovered - result.active.size();
	return result;
}

inline uint32_t appIdFromScriptName(std::string_view filename)
{
	const auto dot = filename.rfind('.');
	if (dot == std::string_view::npos) return 0;
	if (filename.substr(dot) != ".lua") return 0;

	const auto stem = filename.substr(0, dot);
	if (stem.empty()) return 0;

	uint64_t v = 0;
	for (char c : stem)
	{
		if (c < '0' || c > '9') return 0;
		v = v * 10 + static_cast<uint64_t>(c - '0');
		if (v > 0xFFFFFFFFull) return 0;
	}
	return static_cast<uint32_t>(v);
}

inline bool keepDiscoveredMainApp(uint32_t appId, bool idIsAlsoManagedDepot)
{
	(void)idIsAlsoManagedDepot;
	return appId > 0;
}

inline std::vector<uint32_t> removedAppIds(
    const std::unordered_set<uint32_t>& before,
    const std::unordered_set<uint32_t>& after)
{
	std::vector<uint32_t> removed;
	for (const uint32_t appId : before)
		if (!after.contains(appId)) removed.push_back(appId);
	std::sort(removed.begin(), removed.end());
	return removed;
}

template <typename SourceExistsFn>
inline std::vector<uint32_t> recoverRacyScanDrops(
    const std::unordered_set<uint32_t>& freshScan,
    const std::unordered_set<uint32_t>& previouslyKnown,
    SourceExistsFn sourceExists)
{
	std::vector<uint32_t> recovered;
	for (const uint32_t appId : previouslyKnown)
	{
		if (appId == 0 || freshScan.contains(appId)) continue;
		if (sourceExists(appId)) recovered.push_back(appId);
	}
	std::sort(recovered.begin(), recovered.end());
	return recovered;
}

struct ReloadRemovals
{
	std::vector<uint32_t> managed;
	std::vector<uint32_t> active;
	std::vector<uint32_t> managedAdded;
};

inline ReloadRemovals classifyReloadRemovals(
    const std::unordered_set<uint32_t>& beforeManaged,
    const std::unordered_set<uint32_t>& afterManaged,
    const std::unordered_set<uint32_t>& beforeActive,
    const std::unordered_set<uint32_t>& afterActive)
{
	return ReloadRemovals{
	    .managed = removedAppIds(beforeManaged, afterManaged),
	    .active = removedAppIds(beforeActive, afterActive),
	    .managedAdded = removedAppIds(afterManaged, beforeManaged),
	};
}

struct InstalledApps
{
	std::unordered_set<uint32_t> all;
	std::unordered_set<uint32_t> accela;
};

struct AppIdSets
{

	std::unordered_set<uint32_t> managed;

	std::unordered_set<uint32_t> compatibility;

	std::unordered_set<uint32_t> active;
};

inline uint32_t appIdFromManifestName(std::string_view filename)
{
	constexpr std::string_view prefix = "appmanifest_";
	constexpr std::string_view suffix = ".acf";
	if (!filename.starts_with(prefix) || !filename.ends_with(suffix)) return 0;

	const auto digits = filename.substr(
	    prefix.size(), filename.size() - prefix.size() - suffix.size());
	if (digits.empty()) return 0;

	uint64_t value = 0;
	for (char c : digits)
	{
		if (c < '0' || c > '9') return 0;
		value = value * 10 + static_cast<uint64_t>(c - '0');
		if (value > 0xFFFFFFFFull) return 0;
	}
	return static_cast<uint32_t>(value);
}

inline std::string quotedField(const std::string& text, std::string_view key)
{
	const std::string quotedKey = "\"" + std::string(key) + "\"";
	const auto keyPos = text.find(quotedKey);
	if (keyPos == std::string::npos) return {};
	const auto open = text.find('"', keyPos + quotedKey.size());
	if (open == std::string::npos) return {};
	const auto close = text.find('"', open + 1);
	if (close == std::string::npos) return {};
	return text.substr(open + 1, close - open - 1);
}

inline std::vector<std::filesystem::path>
steamAppsRootsFor(const std::filesystem::path& steamRoot)
{
	std::vector<std::filesystem::path> roots;
	const auto addUnique = [&](std::filesystem::path path) {
		path = path.lexically_normal();
		if (std::find(roots.begin(), roots.end(), path) == roots.end())
			roots.push_back(std::move(path));
	};

	addUnique(steamRoot / "steamapps");
	const std::filesystem::path libraryFiles[] = {
		steamRoot / "steamapps" / "libraryfolders.vdf",
		steamRoot / "config" / "libraryfolders.vdf",
	};
	for (const auto& path : libraryFiles)
	{
		std::ifstream file(path);
		if (!file.is_open()) continue;
		std::ostringstream contents;
		contents << file.rdbuf();
		const std::string text = contents.str();

		std::size_t cursor = 0;
		while (cursor < text.size())
		{
			const auto keyPos = text.find("\"path\"", cursor);
			if (keyPos == std::string::npos) break;
			const auto open = text.find('"', keyPos + 6);
			if (open == std::string::npos) break;
			const auto close = text.find('"', open + 1);
			if (close == std::string::npos) break;
			const std::string library = text.substr(open + 1, close - open - 1);
			if (!library.empty()) addUnique(std::filesystem::path(library) / "steamapps");
			cursor = close + 1;
		}
	}
	return roots;
}

inline bool manifestInstalledState(
    const std::filesystem::path& steamapps,
    const std::filesystem::path& manifestPath,
    bool& isAccela)
{
	isAccela = false;

	std::ifstream manifest(manifestPath);
	if (!manifest.is_open()) return false;
	std::ostringstream contents;
	contents << manifest.rdbuf();
	const std::string installDir = quotedField(contents.str(), "installdir");
	const std::filesystem::path relative(installDir);
	if (relative.empty() || relative.is_absolute() || relative.has_parent_path()
	    || relative == "." || relative == "..")
	{
		return false;
	}

	std::error_code ec;
	const auto gameDir = steamapps / "common" / relative;
	if (!std::filesystem::is_directory(gameDir, ec) || ec) return false;

	const bool hasAccelaMarker =
	    std::filesystem::exists(gameDir / ".DepotDownloader", ec) && !ec;
	ec.clear();
	bool hasContent = false;
	std::filesystem::directory_iterator gameEntries(
	    gameDir, std::filesystem::directory_options::skip_permission_denied, ec);
	if (ec) return false;
	for (const auto& gameEntry : gameEntries)
	{
		if (gameEntry.path().filename() != ".DepotDownloader")
		{
			hasContent = true;
			break;
		}
	}
	if (!hasContent) return false;

	isAccela = hasAccelaMarker;
	return true;
}

inline InstalledApps
scanInstalledApps(const std::vector<std::filesystem::path>& steamAppsRoots)
{
	InstalledApps result;
	for (const auto& steamapps : steamAppsRoots)
	{
		std::error_code ec;
		std::filesystem::directory_iterator entries(
		    steamapps, std::filesystem::directory_options::skip_permission_denied, ec);
		if (ec) continue;

		for (const auto& entry : entries)
		{
			if (!entry.is_regular_file(ec) || ec) { ec.clear(); continue; }
			const uint32_t appId = appIdFromManifestName(
			    entry.path().filename().string());
			if (appId == 0) continue;

			bool isAccela = false;
			if (!manifestInstalledState(steamapps, entry.path(), isAccela))
				continue;
			result.all.insert(appId);
			if (isAccela) result.accela.insert(appId);
		}
	}
	return result;
}

inline bool confirmInstalledApp(
    const std::vector<std::filesystem::path>& steamAppsRoots,
    uint32_t appId, bool& isAccela)
{
	isAccela = false;
	if (appId == 0) return false;

	const std::string manifestName =
	    "appmanifest_" + std::to_string(appId) + ".acf";
	for (const auto& steamapps : steamAppsRoots)
	{
		std::error_code ec;
		const auto manifestPath = steamapps / manifestName;
		if (!std::filesystem::is_regular_file(manifestPath, ec) || ec) continue;
		if (manifestInstalledState(steamapps, manifestPath, isAccela))
			return true;
	}
	isAccela = false;
	return false;
}

inline AppIdSets classifyAppIds(
    const std::unordered_set<uint32_t>& stplugApps,
    const std::unordered_set<uint32_t>& luaYamlApps,
    const std::unordered_set<uint32_t>& legacyApps,
    const std::unordered_set<uint32_t>& installedApps,
    const std::unordered_set<uint32_t>& accelaApps)
{
	AppIdSets result;
	result.managed.insert(stplugApps.begin(), stplugApps.end());
	result.managed.insert(luaYamlApps.begin(), luaYamlApps.end());
	result.active = result.managed;

	for (uint32_t appId : accelaApps)
	{
		if (!result.managed.contains(appId)) result.compatibility.insert(appId);
		result.active.insert(appId);
	}
	for (uint32_t appId : legacyApps)
	{
		if (!installedApps.contains(appId)) continue;
		if (!result.managed.contains(appId)) result.compatibility.insert(appId);
		result.active.insert(appId);
	}
	return result;
}

}

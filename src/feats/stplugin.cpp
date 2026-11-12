// SPDX-License-Identifier: AGPL-3.0-only

#include "stplugin.hpp"

#include "../config.hpp"
#include "../globals.hpp"
#include "../log.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <mutex>
#include <regex>
#include <system_error>

namespace
{
	// Anchored on a whole, comment-stripped line. The bare form must not match
	// the three-argument form, hence the immediate closing parenthesis.
	const std::regex kAddAppIdBare(R"(^addappid\(\s*(\d+)\s*\)$)");
	const std::regex kAddAppIdKey(R"(^addappid\(\s*(\d+)\s*,\s*\d+\s*,\s*"([0-9A-Fa-f]{64})"\s*\)$)");
	const std::regex kSetManifestId(R"(^setmanifestid\(\s*(\d+)\s*,\s*"(\d+)"\s*\)$)");
	const std::regex kInlineComment(R"(\s*--.*$)");

	std::mutex g_mutex;
	StPlugin::Import_t g_current;

	std::string stripComment(std::string line)
	{
		line = std::regex_replace(line, kInlineComment, "");
		// Trim; the file may use CRLF or tabs.
		const auto first = line.find_first_not_of(" \t\r\n");
		if (first == std::string::npos)
		{
			return {};
		}
		const auto last = line.find_last_not_of(" \t\r\n");
		return line.substr(first, last - first + 1);
	}

	void parseFile(const std::filesystem::path& path, StPlugin::Import_t& out)
	{
		std::ifstream file(path);
		if (!file.is_open())
		{
			LOG_NOTIFYWARN("stplug-in: cannot open %s\n", path.c_str());
			return;
		}

		std::string raw;
		while (std::getline(file, raw))
		{
			const std::string line = stripComment(raw);
			if (line.empty())
			{
				continue;
			}

			std::smatch match;
			try
			{
				if (std::regex_match(line, match, kAddAppIdBare))
				{
					out.appIds.insert(static_cast<AppId_t>(std::stoul(match[1].str())));
				}
				else if (std::regex_match(line, match, kAddAppIdKey))
				{
					out.depotKeys[static_cast<AppId_t>(std::stoul(match[1].str()))] = match[2].str();
				}
				else if (std::regex_match(line, match, kSetManifestId))
				{
					out.manifestIds[static_cast<AppId_t>(std::stoul(match[1].str()))] =
						std::stoull(match[2].str());
				}
			}
			catch (const std::exception& e)
			{
				LOG_NOTIFYWARN("stplug-in: %s: bad line '%s' (%s)\n", path.c_str(), line.c_str(), e.what());
			}
		}
	}
}

std::vector<std::filesystem::path> StPlugin::scriptDirs()
{
	std::vector<std::filesystem::path> dirs;
	const char* home = getenv("HOME");
	if (!home)
	{
		return dirs;
	}

	const std::filesystem::path roots[] = {
		std::filesystem::path(home) / ".steam" / "steam",
		std::filesystem::path(home) / ".local" / "share" / "Steam",
	};

	std::vector<std::filesystem::path> seen;
	for (const auto& root : roots)
	{
		auto dir = root / "config" / "stplug-in";
		std::error_code ec;
		if (!std::filesystem::is_directory(dir, ec))
		{
			continue;
		}
		// ~/.steam/steam is usually a symlink to ~/.local/share/Steam; do not
		// parse the same directory twice.
		const auto canonical = std::filesystem::weakly_canonical(dir, ec).string();
		if (std::find(seen.begin(), seen.end(), canonical) != seen.end())
		{
			continue;
		}
		seen.push_back(canonical);
		dirs.push_back(dir);
	}
	return dirs;
}

StPlugin::Import_t StPlugin::parseAll()
{
	Import_t result;
	for (const auto& dir : scriptDirs())
	{
		std::error_code ec;
		for (const auto& entry : std::filesystem::directory_iterator(dir, ec))
		{
			if (!entry.is_regular_file(ec) || entry.path().extension() != ".lua")
			{
				continue;
			}
			parseFile(entry.path(), result);
			++result.files;
		}
	}
	return result;
}

void StPlugin::mergeInto(Import_t const& import)
{
	{
		std::lock_guard<std::mutex> lock(g_mutex);
		g_current = import;
	}

	if (import.files == 0)
	{
		return;
	}

	// setAdditionalApps() overwrites addedAppIds on every load, so union rather
	// than insert-into-live: the config list stays authoritative for its own
	// entries, and stplug-in ids are re-added each reload.
	auto ids = g_config.addedAppIds.copy();
	const auto before = ids.size();
	ids.insert(import.appIds.begin(), import.appIds.end());
	g_config.addedAppIds = ids;

	auto manifests = g_config.manifestIds.copy();
	const auto manifestBefore = manifests.size();
	for (const auto& [depot, gid] : import.manifestIds)
	{
		manifests[depot] = gid;
	}
	g_config.manifestIds = manifests;

	LOG_INFO(
		"stplug-in: %u file(s), %zu app id(s) (+%zu), %zu depot key(s), %zu manifest pin(s) (+%zu)\n",
		import.files,
		import.appIds.size(),
		ids.size() - before,
		import.depotKeys.size(),
		import.manifestIds.size(),
		manifests.size() - manifestBefore);
}

const std::string* StPlugin::depotKeyFor(AppId_t depot)
{
	std::lock_guard<std::mutex> lock(g_mutex);
	const auto it = g_current.depotKeys.find(depot);
	return it == g_current.depotKeys.end() ? nullptr : &it->second;
}

// SPDX-License-Identifier: AGPL-3.0-only

#include "stplugin.hpp"

#include "stplugin_parse.hpp"

#include "../config.hpp"
#include "../globals.hpp"
#include "../log.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <mutex>
#include <system_error>

namespace
{
	std::mutex g_mutex;
	StPlugin::Import_t g_current;

	void parseFile(const std::filesystem::path& path, StPlugin::Import_t& out)
	{
		std::ifstream file(path);
		if (!file.is_open())
		{
			LOG_NOTIFYWARN("stplug-in: cannot open %s\n", path.c_str());
			return;
		}

		const std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		StPluginParse::Script script;
		StPluginParse::parse(text, path.stem().string(), script);

		out.appIds.insert(script.appIds.begin(), script.appIds.end());

		for (const auto& [depot, key] : script.depotKeys)
		{
			out.depotKeys[depot] = key;
		}

		for (const auto& [depot, gid] : script.manifestIds)
		{
			out.manifestIds[depot] = gid;
		}

		if (!script.scope)
		{
			return;
		}

		auto& pins = out.depotsByApp[static_cast<AppId_t>(script.scope)];
		for (const auto& pin : script.depots)
		{
			const auto existing = std::find_if(pins.begin(), pins.end(), [&](const StPlugin::DepotPin_t& candidate) {
				return candidate.depotId == pin.depotId;
			});

			if (existing != pins.end())
			{
				if (pin.manifestId)
				{
					existing->manifestId = pin.manifestId;
				}
				if (pin.size)
				{
					existing->size = pin.size;
				}
				continue;
			}

			pins.push_back({ static_cast<AppId_t>(pin.depotId), pin.manifestId, pin.size });
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

	std::vector<std::string> seen;
	for (const auto& root : roots)
	{
		auto dir = root / "config" / "stplug-in";
		std::error_code ec;
		if (!std::filesystem::is_directory(dir, ec))
		{
			continue;
		}
		// ~/.steam/steam is usually a symlink to ~/.local/share/Steam.
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

	// setAdditionalApps() overwrites addedAppIds on every load, so union.
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

	size_t pinnedDepots = 0;
	for (const auto& [appId, pins] : import.depotsByApp)
	{
		for (const auto& pin : pins)
		{
			if (pin.manifestId)
			{
				++pinnedDepots;
			}
		}
	}

	LOG_INFO(
		"stplug-in: %u file(s), %zu app id(s) (+%zu), %zu depot key(s), %zu manifest pin(s) (+%zu), %zu depot(s) planned over %zu app(s)\n",
		import.files,
		import.appIds.size(),
		ids.size() - before,
		import.depotKeys.size(),
		import.manifestIds.size(),
		manifests.size() - manifestBefore,
		pinnedDepots,
		import.depotsByApp.size());
}

const std::string* StPlugin::depotKeyFor(AppId_t depot)
{
	std::lock_guard<std::mutex> lock(g_mutex);
	const auto it = g_current.depotKeys.find(depot);
	return it == g_current.depotKeys.end() ? nullptr : &it->second;
}

std::vector<StPlugin::DepotPin_t> StPlugin::depotPinsFor(const AppId_t appId)
{
	std::lock_guard<std::mutex> lock(g_mutex);
	const auto it = g_current.depotsByApp.find(appId);
	if (it == g_current.depotsByApp.end())
	{
		return {};
	}
	return it->second;
}

// SPDX-License-Identifier: AGPL-3.0-only

#include "stplugin.hpp"

#include "../config.hpp"
#include "../globals.hpp"
#include "../log.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <mutex>
#include <system_error>

namespace
{
	std::mutex g_mutex;
	StPlugin::Import_t g_current;

	const char* kWhitespace = " \t\r\n";

	std::string trim(const std::string& value)
	{
		const auto first = value.find_first_not_of(kWhitespace);
		if (first == std::string::npos)
		{
			return {};
		}
		const auto last = value.find_last_not_of(kWhitespace);
		return value.substr(first, last - first + 1);
	}

	// Keys are hex, so a plain search for '--' cannot hit a quoted value.
	std::string stripComment(const std::string& line)
	{
		const auto comment = line.find("--");
		return trim(comment == std::string::npos ? line : line.substr(0, comment));
	}

	std::vector<std::string> splitArgs(const std::string& args)
	{
		std::vector<std::string> out;
		std::string current;
		bool inQuotes = false;
		for (const char ch : args)
		{
			if (ch == '"')
			{
				inQuotes = !inQuotes;
				continue;
			}
			if (ch == ',' && !inQuotes)
			{
				out.push_back(trim(current));
				current.clear();
				continue;
			}
			current += ch;
		}
		out.push_back(trim(current));
		return out;
	}

	bool allDigits(const std::string& value)
	{
		return !value.empty() && std::all_of(value.begin(), value.end(), [](unsigned char c) {
			return c >= '0' && c <= '9';
		});
	}

	bool isDepotKey(const std::string& value)
	{
		return value.size() == 64 && std::all_of(value.begin(), value.end(), [](unsigned char c) {
			return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
		});
	}

	bool parseCall(const std::string& line, std::string& name, std::vector<std::string>& args)
	{
		const auto open = line.find('(');
		const auto close = line.rfind(')');
		if (open == std::string::npos || close == std::string::npos || close < open)
		{
			return false;
		}
		name = trim(line.substr(0, open));
		args = splitArgs(line.substr(open + 1, close - open - 1));
		return !name.empty();
	}

	std::string lowered(const std::string& name)
	{
		auto out = name;
		std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
			return static_cast<char>(std::tolower(c));
		});
		return out;
	}

	// The app a script unlocks is its own frame of reference: a depot line is a
	// depot of that app. Depot == app happens (the MAIN APPLICATION key line) and
	// is not a depot, so it is dropped here.
	StPlugin::DepotPin_t* recordDepot(StPlugin::Import_t& out, const AppId_t appScope, const AppId_t depotId)
	{
		if (!appScope || !depotId || depotId == appScope)
		{
			return nullptr;
		}

		auto& pins = out.depotsByApp[appScope];
		for (auto& pin : pins)
		{
			if (pin.depotId == depotId)
			{
				return &pin;
			}
		}

		pins.push_back({ depotId, 0, 0 });
		return &pins.back();
	}

	void applyCall(const std::string& name, const std::vector<std::string>& args, StPlugin::Import_t& out, const AppId_t appScope)
	{
		const auto call = lowered(name);

		if (call == "addappid")
		{
			if (args.size() == 1 && allDigits(args[0]))
			{
				out.appIds.insert(static_cast<AppId_t>(std::stoul(args[0])));
				return;
			}
			if (args.size() == 3 && allDigits(args[0]) && isDepotKey(args[2]))
			{
				const auto depot = static_cast<AppId_t>(std::stoul(args[0]));
				out.depotKeys[depot] = args[2];
				recordDepot(out, appScope, depot);
			}
			return;
		}

		if (call == "setmanifestid")
		{
			// Two-argument form, plus a three-argument form carrying the
			// manifest size that some archives emit.
			if ((args.size() == 2 || args.size() == 3) && allDigits(args[0]) && allDigits(args[1]))
			{
				const auto depot = static_cast<AppId_t>(std::stoul(args[0]));
				const uint64_t gid = std::stoull(args[1]);

				out.manifestIds[depot] = gid;

				if (auto* pin = recordDepot(out, appScope, depot))
				{
					pin->manifestId = gid;
					if (args.size() == 3 && allDigits(args[2]))
					{
						pin->size = std::stoull(args[2]);
					}
				}
			}
		}
	}

	void parseFile(const std::filesystem::path& path, StPlugin::Import_t& out)
	{
		std::ifstream file(path);
		if (!file.is_open())
		{
			LOG_NOTIFYWARN("stplug-in: cannot open %s\n", path.c_str());
			return;
		}

		std::vector<std::string> lines;
		std::string raw;
		while (std::getline(file, raw))
		{
			auto line = stripComment(raw);
			if (!line.empty())
			{
				lines.push_back(std::move(line));
			}
		}

		// The script is named after the app it unlocks. Some generators emit the
		// app id only inside a depot-key line, which is filed as a key, so the
		// filename is the authoritative source for the app id itself; a bare
		// addappid is only the fallback for non-numeric names.
		AppId_t appScope = 0;
		const auto stem = path.stem().string();
		if (allDigits(stem))
		{
			appScope = static_cast<AppId_t>(std::stoul(stem));
		}
		else
		{
			for (const auto& line : lines)
			{
				std::string name;
				std::vector<std::string> args;
				if (parseCall(line, name, args) && lowered(name) == "addappid" && args.size() == 1 && allDigits(args[0]))
				{
					appScope = static_cast<AppId_t>(std::stoul(args[0]));
					break;
				}
			}
		}

		if (appScope)
		{
			out.appIds.insert(appScope);
		}

		for (const auto& line : lines)
		{
			std::string name;
			std::vector<std::string> args;
			if (!parseCall(line, name, args))
			{
				continue;
			}

			try
			{
				applyCall(name, args, out, appScope);
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

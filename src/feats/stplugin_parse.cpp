// SPDX-License-Identifier: AGPL-3.0-only

#include "stplugin_parse.hpp"

#include <algorithm>
#include <cctype>

namespace
{
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

	std::string lowered(const std::string& name)
	{
		auto out = name;
		std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
			return static_cast<char>(std::tolower(c));
		});
		return out;
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

	// The script is its own frame of reference: every depot line belongs to the
	// script's app. Depot == app is the main-application key line, not a depot.
	StPluginParse::DepotPin* recordDepot(StPluginParse::Script& out, const uint32_t appScope, const uint32_t depotId)
	{
		if (!out.scope || !appScope || !depotId || depotId == appScope)
		{
			return nullptr;
		}

		for (auto& pin : out.depots)
		{
			if (pin.depotId == depotId)
			{
				return &pin;
			}
		}

		out.depots.push_back({ depotId, 0, 0 });
		return &out.depots.back();
	}

	void applyCall(const std::string& name, const std::vector<std::string>& args, StPluginParse::Script& out, const uint32_t appScope)
	{
		const auto call = lowered(name);

		if (call == "addappid")
		{
			if (args.size() == 1 && allDigits(args[0]))
			{
				out.appIds.insert(static_cast<uint32_t>(std::stoul(args[0])));
				return;
			}
			if (args.size() == 3 && allDigits(args[0]) && isDepotKey(args[2]))
			{
				const auto depot = static_cast<uint32_t>(std::stoul(args[0]));
				out.depotKeys[depot] = args[2];
				recordDepot(out, appScope, depot);
			}
			return;
		}

		if (call == "setmanifestid")
		{
			// Two-argument form, plus a three-argument form carrying the manifest
			// size that some archives emit.
			if ((args.size() == 2 || args.size() == 3) && allDigits(args[0]) && allDigits(args[1]))
			{
				const auto depot = static_cast<uint32_t>(std::stoul(args[0]));
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
}

void StPluginParse::parse(const std::string& text, const std::string& stem, Script& out)
{
	std::vector<std::string> lines;
	size_t start = 0;
	while (start <= text.size())
	{
		const auto end = text.find('\n', start);
		const auto raw = text.substr(start, end == std::string::npos ? std::string::npos : end - start);
		start = (end == std::string::npos) ? text.size() + 1 : end + 1;

		const std::string line = stripComment(raw);
		if (!line.empty())
		{
			lines.push_back(line);
		}
	}

	// The filename is authoritative; a bare addappid is the fallback for scripts
	// that are not named after their app.
	uint32_t appScope = 0;
	if (allDigits(stem))
	{
		appScope = static_cast<uint32_t>(std::stoul(stem));
	}
	else
	{
		for (const auto& line : lines)
		{
			std::string name;
			std::vector<std::string> args;
			if (parseCall(line, name, args) && lowered(name) == "addappid" && args.size() == 1 && allDigits(args[0]))
			{
				appScope = static_cast<uint32_t>(std::stoul(args[0]));
				break;
			}
		}
	}

	if (appScope)
	{
		out.scope = appScope;
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
		catch (const std::exception&)
		{
			// A malformed argument is skipped, not fatal.
		}
	}
}

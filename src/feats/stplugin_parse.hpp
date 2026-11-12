// SPDX-License-Identifier: AGPL-3.0-only
//
// Pure stplug-in parser: no Steam, config or logging dependencies, so it can be
// unit-tested on the host.

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace StPluginParse
{
	struct DepotPin
	{
		uint32_t depotId = 0;
		uint64_t manifestId = 0;	// 0 when the script pins no manifest
		uint64_t size = 0;			// 0 when the script's setManifestid carries no size
	};

	struct Script
	{
		std::unordered_set<uint32_t> appIds;
		std::unordered_map<uint32_t, std::string> depotKeys;
		std::unordered_map<uint32_t, uint64_t> manifestIds;
		std::vector<DepotPin> depots;	// deduplicated, file order
		uint32_t scope = 0;				// app id parsed from the filename, 0 if none
	};

	// `stem` is the script's filename without .lua. It is the authoritative app id:
	// some generators emit the app only inside a depot-key line.
	void parse(const std::string& text, const std::string& stem, Script& out);
}

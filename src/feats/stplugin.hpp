// SPDX-License-Identifier: AGPL-3.0-only
//
// Importer for SteamTools/LuaTools-compatible stplug-in scripts.
//
// Steam itself never reads config/stplug-in; the directory is a convention
// shared by the SLSsteam family. Reading it here is what keeps the config
// self-maintaining instead of a hand-written AdditionalApps/ManifestIds list.
//
// Recognised directives, one per line:
//   addappid(<appid>)                     -> grant ownership of <appid>
//   addappid(<depot>, 1, "<64 hex>")      -> depot decryption key
//   setManifestid(<depot>, "<gid>")       -> pin <depot> to manifest <gid>

#pragma once

#include "../sdk/sdk.hpp"

#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace StPlugin
{
	struct Import_t
	{
		std::unordered_set<AppId_t> appIds;
		std::unordered_map<AppId_t, std::string> depotKeys;
		std::unordered_map<AppId_t, uint64_t> manifestIds;
		uint32_t files = 0;
	};

	// Candidate stplug-in directories, in precedence order. Missing ones are
	// skipped; duplicates (symlinked Steam roots) are collapsed.
	std::vector<std::filesystem::path> scriptDirs();

	// Parse every *.lua under the candidate dirs. App ids union; a later file
	// wins for a depot key or manifest pin.
	Import_t parseAll();

	// Merge into the live config and publish the depot keys for the CM hook.
	// Idempotent: safe to call on every config reload.
	void mergeInto(Import_t const& import);

	// Depot decryption key for a depot, or nullptr. Used by the CM response
	// hook when the server denies GetDepotDecryptionKey.
	const std::string* depotKeyFor(AppId_t depot);
}

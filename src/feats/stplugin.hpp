// SPDX-License-Identifier: AGPL-3.0-only
//
// Importer for stplug-in scripts:
//   addappid(<appid>)                  grant ownership
//   addappid(<depot>, 1, "<64 hex>")   depot decryption key
//   setManifestid(<depot>, "<gid>")    pin a depot to a manifest

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

	std::vector<std::filesystem::path> scriptDirs();
	Import_t parseAll();

	// Idempotent; safe on every config reload.
	void mergeInto(Import_t const& import);

	// Nullptr when the depot has no imported key.
	const std::string* depotKeyFor(AppId_t depot);
}

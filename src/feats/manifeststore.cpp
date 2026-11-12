// SPDX-License-Identifier: AGPL-3.0-only

#include "manifeststore.hpp"

#include "../config.hpp"
#include "../globals.hpp"
#include "../log.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <system_error>

namespace
{
	bool allDigits(const std::string& value)
	{
		return !value.empty() && std::all_of(value.begin(), value.end(), [](unsigned char c) {
			return c >= '0' && c <= '9';
		});
	}

	// Steam's content manifest starts with the 32-bit magic 0x71F617D0.
	bool looksLikeManifest(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
		{
			return false;
		}

		unsigned char magic[4] = {};
		if (!file.read(reinterpret_cast<char*>(magic), sizeof(magic)))
		{
			return false;
		}

		return magic[0] == 0xd0 && magic[1] == 0x17 && magic[2] == 0xf6 && magic[3] == 0x71;
	}

	// <depot>_<gid>.manifest
	bool isManifestName(const std::string& name)
	{
		constexpr size_t suffixLen = 9; // ".manifest"
		if (name.size() <= suffixLen + 2 || name.compare(name.size() - suffixLen, suffixLen, ".manifest") != 0)
		{
			return false;
		}

		const auto underscore = name.find('_');
		if (underscore == std::string::npos)
		{
			return false;
		}

		const auto gidLen = name.size() - suffixLen - underscore - 1;
		return allDigits(name.substr(0, underscore)) && allDigits(name.substr(underscore + 1, gidLen));
	}

	std::filesystem::path depotCacheDir()
	{
		if (g_modSteamClient && g_modSteamClient->path)
		{
			// <root>/steamrt64/steamclient.so
			const auto root = std::filesystem::path(g_modSteamClient->path).parent_path().parent_path();
			return root / "depotcache";
		}

		const char* home = getenv("HOME");
		if (!home)
		{
			return {};
		}
		return std::filesystem::path(home) / ".local" / "share" / "Steam" / "depotcache";
	}
}

void ManifestStore::publish()
{
	std::error_code ec;
	const auto store = CConfig::getDir() / "manifests";
	if (!std::filesystem::is_directory(store, ec))
	{
		return;
	}

	const auto cache = depotCacheDir();
	if (cache.empty() || !std::filesystem::is_directory(cache, ec))
	{
		return;
	}

	uint32_t published = 0;
	for (const auto& entry : std::filesystem::directory_iterator(store, ec))
	{
		if (!entry.is_regular_file(ec))
		{
			continue;
		}

		const auto name = entry.path().filename().string();
		if (!isManifestName(name))
		{
			LOG_NOTIFYWARN("ManifestStore: ignoring %s (expected <depot>_<gid>.manifest)\n", name.c_str());
			continue;
		}

		const auto dest = cache / name;
		if (std::filesystem::exists(dest, ec) &&
			std::filesystem::file_size(dest, ec) == std::filesystem::file_size(entry.path(), ec))
		{
			continue;
		}

		// A manifest that is empty, or does not start with Steam's content-manifest
		// magic, would fail to mount — refuse it rather than stage it.
		const auto size = std::filesystem::file_size(entry.path(), ec);
		if (ec || size == 0 || !looksLikeManifest(entry.path()))
		{
			LOG_NOTIFYWARN("ManifestStore: refusing %s (empty or not a manifest)\n", name.c_str());
			continue;
		}

		std::filesystem::copy_file(entry.path(), dest, std::filesystem::copy_options::overwrite_existing, ec);
		if (ec)
		{
			LOG_NOTIFYWARN("ManifestStore: cannot publish %s (%s)\n", name.c_str(), ec.message().c_str());
			continue;
		}

		++published;
	}

	if (published)
	{
		LOG_INFO("ManifestStore: published %u manifest(s) to depotcache\n", published);
	}
}

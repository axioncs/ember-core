#include "update.hpp"

#include "config.hpp"
#include "curl.hpp"
#include "globals.hpp"
#include "log.hpp"
#include "utils.hpp"
#include "version.hpp"

#include "update_cache.hpp"
#include "thread_start.hpp"

#include <sys/stat.h>

#include <atomic>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <string>
#include <thread>

std::map<uint64_t, std::unordered_set<std::string>> Updater::clientHashMap = std::map<uint64_t, std::unordered_set<std::string>>();

namespace
{

	std::mutex g_hashMapMtx;

	Updater::cache::RefreshGate g_refreshGate;

	constexpr const char* kUpdatesUrl =
	    "https://raw.githubusercontent.com/ember/ember/refs/heads/main/res/updates.yaml";

	long long updatesTtlSecs()
	{
		if (const char* ov = std::getenv("EMBER_UPDATES_TTL"); ov && *ov)
		{
			try { return std::stoll(ov); } catch (...) {}
		}
		return 24 * 3600;
	}

	bool parseHashMap(const std::string& data,
	                  std::map<uint64_t, std::unordered_set<std::string>>& out)
	{
		try
		{
			YAML::Node node = YAML::Load(data);
			std::map<uint64_t, std::unordered_set<std::string>> parsed;
			for (const auto& sub : node["SafeModeHashes"])
			{
				uint64_t version = sub.first.as<uint64_t>();
				parsed[version] = std::unordered_set<std::string>();
				for (const auto& hash : sub.second)
				{
					parsed[version].emplace(hash.as<std::string>());
				}
			}
			out = std::move(parsed);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	long long cacheMtimeSecs()
	{
		struct stat st{};
		const auto path = Updater::getCacheFilePath();
		if (stat(path.c_str(), &st) != 0) return -1;
		if (st.st_size <= 0) return -1;
		return static_cast<long long>(st.st_mtime);
	}

	bool fetchParseAndStore()
	{
		std::string data;
		const int res = Curl::getString(kUpdatesUrl, data);
		if (res != 0 || data.empty())
		{
			g_pLog->debug("Updater: network fetch failed (curl=%d)\n", res);
			return false;
		}

		std::map<uint64_t, std::unordered_set<std::string>> parsed;
		if (!parseHashMap(data, parsed))
		{
			g_pLog->info("Failed to parse updates!\n");
			return false;
		}

		{
			std::lock_guard<std::mutex> lk(g_hashMapMtx);
			Updater::clientHashMap = std::move(parsed);
		}
		Updater::saveToCache(data);
		return true;
	}
}

bool Updater::init()
{

	if (cache::mustFetchSynchronously(g_config.safeMode.get(),
	                                  g_config.warnHashMissmatch.get()))
	{
		if (fetchParseAndStore())
		{
			return true;
		}
		const std::string cached = loadFromCache();
		if (cached.empty()) return false;
		g_pLog->info("Using cached updates.yaml\n");
		std::map<uint64_t, std::unordered_set<std::string>> parsed;
		if (!parseHashMap(cached, parsed)) return false;
		std::lock_guard<std::mutex> lk(g_hashMapMtx);
		clientHashMap = std::move(parsed);
		return true;
	}

	const std::string cached = loadFromCache();
	if (!cached.empty())
	{
		std::map<uint64_t, std::unordered_set<std::string>> parsed;
		if (parseHashMap(cached, parsed))
		{
			std::lock_guard<std::mutex> lk(g_hashMapMtx);
			clientHashMap = std::move(parsed);
		}
	}
	return true;
}

void Updater::refreshInBackgroundIfStale()
{

	if (!g_refreshGate.tryStart())
	{
		return;
	}

	const long long ttl = updatesTtlSecs();
	const long long mtime = cacheMtimeSecs();
	const long long now = static_cast<long long>(std::time(nullptr));
	if (cache::isCacheFresh(mtime >= 0, mtime, now, ttl))
	{
		g_refreshGate.markSucceeded();
		g_refreshGate.finish();
		g_pLog->debug("Updater: cache fresh (age<%llds), skipping refresh\n", ttl);
		return;
	}

	const bool started = ThreadStart::startDetached(
		[]
		{
			ThreadStart::runGuarded(
				[]
				{
					if (fetchParseAndStore())
					{
						g_refreshGate.markSucceeded();
						g_pLog->debug("Updater: background refresh of updates.yaml done\n");
					}
					else
					{
						g_pLog->debug("Updater: background refresh returned no usable data; will retry\n");
					}
				},
				[]
				{
					g_pLog->warn("Updater: background refresh worker failed unexpectedly; will retry\n");
				},
				[]
				{

					g_refreshGate.finish();
				});
		},
		[]
		{
			g_refreshGate.finish();
		},
		[]
		{
			g_pLog->warn("Updater: background refresh detach failed; joining worker\n");
		});
	if (!started)
	{
		g_pLog->warn("Updater: unable to start background refresh; will retry\n");
	}
}

std::string Updater::getCacheFilePath()
{
	auto path = g_config.getDir().append("/.updates.yaml");
	return path;
}

void Updater::saveToCache(std::string yaml)
{
	auto path = Updater::getCacheFilePath();

	std::ofstream stream = std::ofstream(path.c_str());
	stream << yaml;
	stream.close();

	g_pLog->debug("Cached res/updates.yaml!\n");
}

std::string Updater::loadFromCache()
{
	auto path = Updater::getCacheFilePath();
	if (!std::filesystem::exists(path))
	{
		return std::string();
	}

	g_pLog->debug("Loading updates.ymal from disk!\n");

	std::ifstream fstream = std::ifstream(path.c_str());
	std::stringstream buf;
	buf << fstream.rdbuf();

	fstream.close();
	return buf.str();
}

bool Updater::verifySafeModeHash()
{
	auto path = std::filesystem::path(g_modSteamClient.path);

	if (!cache::mustVerifyClientHash(g_config.safeMode.get(),
	                                 g_config.warnHashMissmatch.get(),
	                                 g_config.extendedLogging.get()))
	{
		g_pLog->debug("Skipping steamclient.so hash (no hash check enabled)\n");
		return true;
	}

	try
	{
		std::string sha256 = Utils::getFileSHA256(path.c_str());
		g_pLog->info("steamclient.so hash is %s\n", sha256.c_str());

		std::lock_guard<std::mutex> lk(g_hashMapMtx);
		if (!clientHashMap.contains(VERSION))
		{
			return false;
		}

		const auto& safeHashes = clientHashMap[VERSION];
		if (safeHashes.contains(sha256))
		{
			return true;
		}

		return false;
	}
	catch(std::runtime_error& err)
	{
		g_pLog->debug("Unable to read steamclient.so hash!\n");
		return false;
	}

	return true;
}

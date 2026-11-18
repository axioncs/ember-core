

#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Prewarm
{

using DepotGid = std::pair<uint32_t, uint64_t>;

class FailureTracker
{
public:
	explicit FailureTracker(int maxConsecutiveFailures)
	    : m_max(maxConsecutiveFailures) {}

	void recordSuccess(uint32_t depotId, uint64_t gid)
	{
		m_fails.erase(pack(depotId, gid));
	}

	bool recordFailure(uint32_t depotId, uint64_t gid)
	{
		const int n = ++m_fails[pack(depotId, gid)];
		return n == m_max;
	}

	bool isBlacklisted(uint32_t depotId, uint64_t gid) const
	{
		const auto it = m_fails.find(pack(depotId, gid));
		return it != m_fails.end() && it->second >= m_max;
	}

	void resetAll()
	{
		m_fails.clear();
	}

private:
	static uint64_t pack(uint32_t depotId, uint64_t gid)
	{
		return gid ^ (static_cast<uint64_t>(depotId) * 0x9E3779B97F4A7C15ULL);
	}

	int m_max;
	std::unordered_map<uint64_t, int> m_fails;
};

class PassBackoff
{
public:
	static constexpr int kNoOpPassesBeforeBackoff = 3;
	static constexpr auto kBaseInterval = std::chrono::seconds(30);
	static constexpr auto kBackoffInterval = std::chrono::minutes(1);

	bool observeTargets(const std::vector<DepotGid>& targets)
	{
		std::unordered_set<uint64_t> current;
		current.reserve(targets.size());
		for (const auto& [depotId, gid] : targets)
			current.insert(pack(depotId, gid));

		bool newTarget = false;
		for (const auto key : current)
		{
			if (!m_targets.count(key))
			{
				newTarget = true;
				break;
			}
		}
		m_targets = std::move(current);
		return newTarget;
	}

	void recordPass(bool noOp)
	{
		if (noOp) ++m_noOpPasses;
		else m_noOpPasses = 0;
	}

	std::chrono::seconds interval() const
	{
		return m_noOpPasses >= kNoOpPassesBeforeBackoff
			? kBackoffInterval
			: kBaseInterval;
	}

	int noOpPasses() const { return m_noOpPasses; }

private:
	static uint64_t pack(uint32_t depotId, uint64_t gid)
	{
		return gid ^ (static_cast<uint64_t>(depotId) * 0x9E3779B97F4A7C15ULL);
	}

	std::unordered_set<uint64_t> m_targets;
	int m_noOpPasses = 0;
};

namespace detail
{

struct DepotInfo
{
	uint32_t depotId = 0;
	uint64_t gid = 0;
	std::string oslist;
};

inline std::vector<DepotInfo> parseDepots(const std::string& buf)
{
	std::vector<DepotInfo> out;
	std::size_t depotsPos = buf.find("\"depots\"");
	if (depotsPos == std::string::npos) return out;

	std::size_t openBrace = buf.find('{', depotsPos + 8);
	if (openBrace == std::string::npos) return out;

	int depth = 1;
	std::size_t scan = openBrace + 1;
	uint32_t currentDepotId = 0;
	DepotInfo* current = nullptr;
	std::string currentSection;
	std::string currentBranch;

	auto entryFor = [&out](uint32_t depotId) -> DepotInfo* {
		for (auto& e : out)
		{
			if (e.depotId == depotId) return &e;
		}
		out.push_back(DepotInfo{depotId, 0, {}});
		return &out.back();
	};

	while (scan < buf.size() && depth > 0)
	{
		char c = buf[scan];
		if (c == '"')
		{
			std::size_t end = buf.find('"', scan + 1);
			if (end == std::string::npos) break;
			std::string token = buf.substr(scan + 1, end - scan - 1);
			scan = end + 1;

			if (depth == 1)
			{
				bool isDigits = !token.empty();
				for (char ch : token)
				{
					if (ch < '0' || ch > '9') { isDigits = false; break; }
				}
				if (isDigits)
				{
					try
					{
						currentDepotId = std::stoul(token);
						current = entryFor(currentDepotId);
					}
					catch (...) { currentDepotId = 0; current = nullptr; }
				}
			}
			else if (depth == 2)
			{
				currentSection = token;
			}
			else if (depth == 3 && currentSection == "manifests")
			{
				currentBranch = token;
			}
			else if (depth == 3 && currentSection == "config" &&
			         token == "oslist" && current)
			{
				std::size_t valStart = buf.find('"', scan);
				if (valStart != std::string::npos)
				{
					std::size_t valEnd = buf.find('"', valStart + 1);
					if (valEnd != std::string::npos)
					{
						current->oslist =
						    buf.substr(valStart + 1, valEnd - valStart - 1);
						scan = valEnd + 1;
					}
				}
			}
			else if (depth == 4 && currentSection == "manifests" &&
			         currentBranch == "public" && token == "gid" &&
			         current)
			{
				std::size_t valStart = buf.find('"', scan);
				if (valStart != std::string::npos)
				{
					std::size_t valEnd = buf.find('"', valStart + 1);
					if (valEnd != std::string::npos)
					{
						std::string valToken =
						    buf.substr(valStart + 1, valEnd - valStart - 1);
						bool isDigits = !valToken.empty();
						for (char ch : valToken)
						{
							if (ch < '0' || ch > '9') { isDigits = false; break; }
						}
						if (isDigits)
						{
							try { current->gid = std::stoull(valToken); }
							catch (...) {}
						}
						scan = valEnd + 1;
					}
				}
			}
			continue;
		}
		if (c == '{') ++depth;
		else if (c == '}')
		{
			--depth;
			if (depth == 1)
			{
				currentDepotId = 0;
				current = nullptr;
			}
		}
		++scan;
	}
	return out;
}

inline bool isMacOnly(const std::string& oslist)
{
	if (oslist.empty()) return false;
	const bool hasMac = oslist.find("macos") != std::string::npos ||
	                    oslist.find("osx") != std::string::npos;
	if (!hasMac) return false;
	const bool hasLinux = oslist.find("linux") != std::string::npos;
	const bool hasWindows = oslist.find("windows") != std::string::npos ||
	                        oslist.find("win") != std::string::npos;
	return !hasLinux && !hasWindows;
}

}

inline std::vector<DepotGid> extractDepotsAndGids(const std::string& buf)
{
	std::vector<DepotGid> results;
	for (const auto& d : detail::parseDepots(buf))
	{
		if (d.gid != 0)
		{
			results.push_back({d.depotId, d.gid});
		}
	}
	return results;
}

inline std::vector<DepotGid> planStageTargets(
    const std::vector<std::string>& provisionedBuffers,
    const std::function<bool(uint32_t depotId)>& hasKey)
{
	std::vector<DepotGid> out;
	std::set<DepotGid> seen;

	for (const std::string& buf : provisionedBuffers)
	{
		for (const auto& d : detail::parseDepots(buf))
		{
			if (d.gid == 0) continue;
			if (hasKey && !hasKey(d.depotId)) continue;
			if (detail::isMacOnly(d.oslist)) continue;

			const DepotGid target{d.depotId, d.gid};
			if (seen.insert(target).second)
			{
				out.push_back(target);
			}
		}
	}
	return out;
}

inline std::vector<DepotGid> planPinnedStageTargets(
    const std::vector<DepotGid>& publicTargets,
    const std::unordered_map<uint32_t, uint64_t>& pins)
{
	std::vector<DepotGid> out;
	std::set<DepotGid> seen;
	for (const auto& [depotId, gid] : pins)
	{
		const DepotGid target{depotId, gid};
		if (depotId && gid && seen.insert(target).second) out.push_back(target);
	}
	for (const auto& target : publicTargets)
	{
		if (!target.first || !target.second || pins.count(target.first)) continue;
		if (seen.insert(target).second) out.push_back(target);
	}
	return out;
}

inline std::vector<DepotGid> extractWorkshopManifests(const std::string& acf,
                                                      uint32_t appId)
{
	std::vector<DepotGid> out;
	std::unordered_set<uint64_t> seen;

	auto collectKey = [&](const std::string& key) {
		std::size_t pos = 0;
		const std::string needle = "\"" + key + "\"";
		while ((pos = acf.find(needle, pos)) != std::string::npos)
		{
			pos += needle.size();

			std::size_t q1 = acf.find('"', pos);
			if (q1 == std::string::npos) break;
			std::size_t q2 = acf.find('"', q1 + 1);
			if (q2 == std::string::npos) break;
			const std::string val = acf.substr(q1 + 1, q2 - q1 - 1);
			pos = q2 + 1;

			bool isDigits = !val.empty();
			for (char ch : val)
			{
				if (ch < '0' || ch > '9') { isDigits = false; break; }
			}
			if (!isDigits) continue;

			uint64_t gid = 0;
			try { gid = std::stoull(val); }
			catch (...) { continue; }
			if (gid == 0) continue;

			if (seen.insert(gid).second)
			{
				out.push_back({appId, gid});
			}
		}
	};

	collectKey("manifest");
	collectKey("latest_manifest");
	return out;
}

void ensureStarted();

}

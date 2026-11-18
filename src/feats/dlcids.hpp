

#pragma once

#include <charconv>
#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

namespace AppInfoProvision
{

namespace detail
{

inline void addDlcId(std::vector<uint32_t>& out,
                     std::unordered_set<uint32_t>& seen,
                     const std::string& tok, uint32_t baseAppId)
{
	if (tok.empty()) return;
	for (char c : tok) if (c < '0' || c > '9') return;
	uint32_t id = 0;
	try { id = static_cast<uint32_t>(std::stoul(tok)); }
	catch (...) { return; }
	if (id == 0 || id == baseAppId) return;
	if (seen.insert(id).second) out.push_back(id);
}

inline bool nextQuotedValueFor(const std::string& s, const std::string& key,
                               std::size_t from, std::string& value,
                               std::size_t& next)
{
	const std::string needle = "\"" + key + "\"";
	const std::size_t k = s.find(needle, from);
	if (k == std::string::npos) return false;

	std::size_t i = k + needle.size();
	const std::size_t open = s.find('"', i);
	if (open == std::string::npos) { next = s.size(); return false; }
	const std::size_t close = s.find('"', open + 1);
	if (close == std::string::npos) { next = s.size(); return false; }

	value = s.substr(open + 1, close - (open + 1));
	next = close + 1;
	return true;
}

inline std::size_t matchingBrace(const std::string& s, std::size_t open)
{
	if (open >= s.size() || s[open] != '{') return std::string::npos;
	unsigned int depth = 0;
	bool quoted = false;
	bool escaped = false;
	for (std::size_t i = open; i < s.size(); ++i)
	{
		const char c = s[i];
		if (quoted)
		{
			if (escaped) escaped = false;
			else if (c == '\\') escaped = true;
			else if (c == '"') quoted = false;
			continue;
		}
		if (c == '"') quoted = true;
		else if (c == '{') ++depth;
		else if (c == '}' && --depth == 0) return i;
	}
	return std::string::npos;
}

}

struct DlcAppIds
{
	std::vector<uint32_t> depotTagged;
	std::vector<uint32_t> advertised;
};

struct DlcMetadataFacts
{
	uint32_t requestedBaseAppId = 0;
	uint32_t requestedDlcAppId = 0;
	uint32_t wireAppId = 0;
	uint32_t parentAppId = 0;
	bool hasCommon = false;
	bool typeIsDlc = false;
};

inline bool isValidDlcMetadata(const DlcMetadataFacts& facts) noexcept
{
	return facts.requestedBaseAppId != 0 &&
		facts.requestedDlcAppId != 0 &&
		facts.requestedDlcAppId != facts.requestedBaseAppId &&
		facts.wireAppId == facts.requestedDlcAppId &&
		facts.parentAppId == facts.requestedBaseAppId &&
		facts.hasCommon && facts.typeIsDlc;
}

inline std::vector<uint32_t> selectDlcMetadataCandidates(
	uint32_t baseAppId, const DlcAppIds& sources)
{
	std::unordered_set<uint32_t> seen;
	std::vector<uint32_t> out;
	for (const uint32_t id : sources.advertised)
		if (id != 0 && id != baseAppId && seen.insert(id).second)
			out.push_back(id);
	for (const uint32_t id : sources.depotTagged)
		if (id != 0 && id != baseAppId && seen.insert(id).second)
			out.push_back(id);
	std::sort(out.begin(), out.end());
	return out;
}

inline bool depotIdFromManifestName(const std::string& name,
                                    uint32_t& depotIdOut)
{
	depotIdOut = 0;
	constexpr const char* kSuffix = ".manifest";
	const std::size_t underscore = name.find('_');
	if (underscore == std::string::npos || underscore == 0 ||
	    name.size() <= underscore + 1 + std::char_traits<char>::length(kSuffix) ||
	    name.compare(name.size() - std::char_traits<char>::length(kSuffix),
	                 std::char_traits<char>::length(kSuffix), kSuffix) != 0)
	{
		return false;
	}

	uint32_t parsedDepot = 0;
	const auto first = name.data();
	const auto last = first + underscore;
	const auto depotResult = std::from_chars(first, last, parsedDepot);
	if (depotResult.ec != std::errc{} || depotResult.ptr != last ||
	    parsedDepot == 0)
	{
		return false;
	}

	const std::size_t suffixLength = std::char_traits<char>::length(kSuffix);
	const auto gidFirst = name.data() + underscore + 1;
	const auto gidLast = name.data() + name.size() - suffixLength;
	uint64_t parsedGid = 0;
	const auto gidResult = std::from_chars(gidFirst, gidLast, parsedGid);
	if (gidResult.ec != std::errc{} || gidResult.ptr != gidLast ||
	    parsedGid == 0)
	{
		return false;
	}

	depotIdOut = parsedDepot;
	return true;
}

inline DlcAppIds extractDlcAppIdsBySource(const std::string& wire,
                                          uint32_t baseAppId)
{
	DlcAppIds out;
	std::unordered_set<uint32_t> advertisedSeen;
	std::unordered_set<uint32_t> depotTaggedSeen;
	if (wire.empty()) return out;

	{
		std::size_t pos = 0;
		std::string value;
		while (detail::nextQuotedValueFor(wire, "listofdlc", pos, value, pos))
		{
			std::size_t i = 0;
			while (i < value.size())
			{
				std::size_t j = value.find(',', i);
				if (j == std::string::npos) j = value.size();
				detail::addDlcId(out.advertised, advertisedSeen,
				                 value.substr(i, j - i), baseAppId);
				i = j + 1;
			}
		}
	}

	{
		std::size_t pos = 0;
		std::string value;
		while (detail::nextQuotedValueFor(wire, "dlcappid", pos, value, pos))
		{
			detail::addDlcId(out.depotTagged, depotTaggedSeen, value, baseAppId);
		}
	}

	return out;
}

inline std::vector<uint32_t> extractDlcAppIds(const std::string& wire,
                                              uint32_t baseAppId)
{
	const DlcAppIds grouped = extractDlcAppIdsBySource(wire, baseAppId);
	std::vector<uint32_t> out;
	std::unordered_set<uint32_t> seen;
	for (uint32_t id : grouped.advertised)
	{
		if (seen.insert(id).second) out.push_back(id);
	}
	for (uint32_t id : grouped.depotTagged)
	{
		if (seen.insert(id).second) out.push_back(id);
	}
	return out;
}

struct DlcInjectionIds
{
	std::vector<uint32_t> package0;
	std::vector<uint32_t> appDlc;
};

inline bool hasDepotsInDlc(const std::string& wire)
{
	std::size_t pos = 0;
	std::string value;
	while (detail::nextQuotedValueFor(wire, "hasdepotsindlc", pos, value, pos))
	{
		return value == "1" || value == "yes" || value == "true";
	}
	return false;
}

inline void appendUnique(std::vector<uint32_t>& out,
                         std::unordered_set<uint32_t>& seen, uint32_t id)
{
	if (id != 0 && seen.insert(id).second) out.push_back(id);
}

inline std::vector<uint32_t> mergePackage0AppIds(
    const std::unordered_set<uint32_t>& baseAppIds,
    const std::vector<uint32_t>& extraAppIds)
{
	std::vector<uint32_t> out;
	out.reserve(baseAppIds.size() + extraAppIds.size());
	std::unordered_set<uint32_t> seen;
	seen.reserve(baseAppIds.size() + extraAppIds.size());
	for (uint32_t id : baseAppIds)
		appendUnique(out, seen, id);
	for (uint32_t id : extraAppIds)
		appendUnique(out, seen, id);
	return out;
}

inline DlcInjectionIds selectDlcInjectionIds(
	const DlcAppIds& sources,
	const std::unordered_set<uint32_t>& advertisedWithContent,
	bool injectAllAdvertised)
{
	DlcInjectionIds out;
	std::unordered_set<uint32_t> packageSeen;
	std::unordered_set<uint32_t> appSeen;

	for (uint32_t id : sources.depotTagged)
	{
		appendUnique(out.package0, packageSeen, id);
		appendUnique(out.appDlc, appSeen, id);
	}
	for (uint32_t id : sources.advertised)
	{
		appendUnique(out.appDlc, appSeen, id);
		if (injectAllAdvertised || advertisedWithContent.count(id) != 0)
		{
			appendUnique(out.package0, packageSeen, id);
		}
	}
	return out;
}

inline uint32_t dlcAppIdForDepot(const std::string& wire, uint32_t baseAppId,
                                 uint32_t depotId)
{
	if (wire.empty() || depotId == 0 || depotId == baseAppId) return 0;

	{
		const std::string key = "\"" + std::to_string(depotId) + "\"";
		std::size_t pos = 0;
		while ((pos = wire.find(key, pos)) != std::string::npos)
		{
			pos += key.size();
			const std::size_t nextQuote = wire.find('"', pos);
			const std::size_t open = wire.find('{', pos);
			if (open == std::string::npos) break;
			if (nextQuote != std::string::npos && nextQuote < open) continue;
			const std::size_t close = detail::matchingBrace(wire, open);
			if (close == std::string::npos) break;
			const std::size_t tag = wire.find("\"dlcappid\"", open + 1);
			if (tag == std::string::npos || tag >= close)
			{
				pos = close + 1;
				continue;
			}

			std::string value;
			std::size_t next = tag;
			if (detail::nextQuotedValueFor(wire, "dlcappid", tag, value, next) &&
				next <= close)
			{
				std::vector<uint32_t> parsed;
				std::unordered_set<uint32_t> seen;
				detail::addDlcId(parsed, seen, value, baseAppId);
				if (!parsed.empty()) return parsed.front();
			}
			break;
		}
	}

	for (uint32_t advertised : extractDlcAppIds(wire, baseAppId))
	{
		if (advertised == depotId) return depotId;
	}

	return 0;
}

inline std::string filterUnsupportedDlcAppIds(
	const std::string& value,
	const std::unordered_set<uint32_t>& unsupported,
	std::size_t* removedOut = nullptr)
{
	std::string out;
	std::size_t removed = 0;
	bool firstKept = true;
	std::size_t i = 0;

	while (i <= value.size())
	{
		std::size_t j = value.find(',', i);
		if (j == std::string::npos) j = value.size();
		const std::string token = value.substr(i, j - i);

		bool remove = false;
		if (!token.empty())
		{
			bool numeric = true;
			for (char c : token)
			{
				if (c < '0' || c > '9') { numeric = false; break; }
			}
			if (numeric)
			{
				try
				{
					const unsigned long parsed = std::stoul(token);
					if (parsed <= UINT32_MAX &&
					    unsupported.count(static_cast<uint32_t>(parsed)) != 0)
					{
						remove = true;
					}
				}
				catch (...) {}
			}
		}

		if (remove)
		{
			++removed;
		}
		else
		{
			if (!firstKept) out.push_back(',');
			out += token;
			firstKept = false;
		}

		if (j == value.size()) break;
		i = j + 1;
	}

	if (removedOut) *removedOut = removed;
	return out;
}

}

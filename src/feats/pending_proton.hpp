

#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <set>
#include <string>
#include <string_view>
#include <system_error>

namespace AppInfoProvision
{

enum class PendingProtonParseStatus
{
	Valid,
	Invalid,
};

struct PendingProtonParseResult
{
	PendingProtonParseStatus status = PendingProtonParseStatus::Invalid;
	std::set<uint32_t> ids;
};

inline std::set<std::string> parsePlatformOsList(std::string_view text)
{
	std::set<std::string> out;
	std::size_t pos = 0;
	while (pos < text.size())
	{
		std::size_t end = text.find(',', pos);
		if (end == std::string_view::npos) end = text.size();
		while (pos < end && (text[pos] == ' ' || text[pos] == '\t')) ++pos;
		while (end > pos && (text[end - 1] == ' ' || text[end - 1] == '\t')) --end;
		if (pos < end) out.emplace(text.substr(pos, end - pos));
		pos = end + 1;
	}
	return out;
}

inline constexpr std::string_view kAnyPlatform = "*";

inline void addDepotPlatforms(std::set<std::string>& out, std::string_view oslist)
{
	if (oslist.empty())
	{
		out.emplace(kAnyPlatform);
		return;
	}
	const auto parsed = parsePlatformOsList(oslist);
	out.insert(parsed.begin(), parsed.end());
}

inline bool requiresProtonMapping(
	std::size_t keptContentDepots,
	const std::set<std::string>& depotOs,
	std::string_view commonOsList)
{
	if (keptContentDepots == 0) return false;

	const auto commonOs = parsePlatformOsList(commonOsList);

	const bool anyPlatform = depotOs.count(std::string(kAnyPlatform)) != 0;
	const bool commonHasLinux = commonOs.count("linux") != 0;

	if (depotOs.size() == (anyPlatform ? 1u : 0u))
		return !commonOs.empty() && !commonHasLinux;

	const bool depotTargetsLinux =
		depotOs.count("linux") != 0 || (anyPlatform && commonHasLinux);
	const bool nativeLinuxAvailable = commonOs.empty()
		? depotTargetsLinux
		: (depotTargetsLinux && commonOs.count("linux") != 0);
	return !nativeLinuxAvailable;
}

inline PendingProtonParseResult parsePendingProtonText(std::string_view text)
{
	PendingProtonParseResult result;
	std::size_t pos = 0;
	while (pos < text.size())
	{
		while (pos < text.size() &&
		       (text[pos] == ' ' || text[pos] == '\t' || text[pos] == '\n' ||
	        text[pos] == '\r' || text[pos] == '\f' || text[pos] == '\v'))
		{
			++pos;
		}
		if (pos == text.size()) break;

		const std::size_t tokenStart = pos;
		while (pos < text.size() &&
		       text[pos] != ' ' && text[pos] != '\t' && text[pos] != '\n' &&
		       text[pos] != '\r' && text[pos] != '\f' && text[pos] != '\v')
		{
			++pos;
		}

		uint32_t id = 0;
		const auto first = text.data() + tokenStart;
		const auto last = text.data() + pos;
		const auto parsed = std::from_chars(first, last, id, 10);
		if (parsed.ec != std::errc{} || parsed.ptr != last || id == 0)
		{
			result.ids.clear();
			return result;
		}
		result.ids.insert(id);
	}

	result.status = PendingProtonParseStatus::Valid;
	return result;
}

}

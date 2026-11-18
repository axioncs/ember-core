

#pragma once

#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace DepotQuarantineStore
{
	struct Record
	{
		uint32_t appId = 0;
		uint32_t depotId = 0;

		uint32_t dlcAppId = 0;
		uint64_t keyFingerprint = 0;
	};

	inline uint64_t fingerprintKey(const std::string& key)
	{
		if (key.size() != 32) return 0;
		uint64_t hash = 14695981039346656037ULL;
		for (unsigned char byte : key)
		{
			hash ^= byte;
			hash *= 1099511628211ULL;
		}
		return hash ? hash : 1;
	}

	inline bool isUsable(const Record& record)
	{
		return record.appId && record.depotId && record.dlcAppId
		    && record.keyFingerprint;
	}

	inline bool upsert(std::vector<Record>& records, const Record& record)
	{
		if (!isUsable(record)) return false;

		for (auto& existing : records)
		{
			if (existing.depotId != record.depotId) continue;
			if (existing.appId == record.appId
			    && existing.dlcAppId == record.dlcAppId
			    && existing.keyFingerprint == record.keyFingerprint)
			{
				return false;
			}
			existing = record;
			return true;
		}

		records.push_back(record);
		return true;
	}

	inline bool isQuarantined(const std::vector<Record>& records,
	                          uint32_t depotId, uint64_t keyFingerprint)
	{
		if (!depotId || !keyFingerprint) return false;
		for (const auto& record : records)
		{
			if (record.depotId == depotId
			    && record.keyFingerprint == keyFingerprint)
			{
				return true;
			}
		}
		return false;
	}

	inline std::string serialize(const std::vector<Record>& records)
	{
		std::ostringstream out;
		for (const auto& record : records)
		{
			if (!isUsable(record)) continue;
			out << record.appId << ' ' << record.depotId << ' '
			    << record.dlcAppId << ' ' << record.keyFingerprint << '\n';
		}
		return out.str();
	}

	inline std::vector<Record> parse(const std::string& text)
	{
		std::vector<Record> records;
		std::istringstream in(text);
		std::string line;
		while (std::getline(in, line))
		{
			std::istringstream fields(line);
			Record record;
			if (!(fields >> record.appId >> record.depotId >> record.dlcAppId
			      >> record.keyFingerprint))
			{
				continue;
			}
			upsert(records, record);
		}
		return records;
	}

	inline std::vector<uint32_t> withoutIds(
	    const std::vector<uint32_t>& ids,
	    const std::unordered_set<uint32_t>& drop)
	{
		if (drop.empty()) return ids;
		std::vector<uint32_t> kept;
		kept.reserve(ids.size());
		for (uint32_t id : ids)
		{
			if (!drop.count(id)) kept.push_back(id);
		}
		return kept;
	}
}

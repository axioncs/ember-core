

#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace DepotQuarantine
{

	inline constexpr uint32_t kUnpackFailed = 4;
	inline constexpr uint32_t kDownloadedChunkLocation = 4;
	inline constexpr std::size_t kDistinctFailureThreshold = 3;

	inline constexpr bool isTargetFailure(uint32_t location,
	                                      uint32_t unpackResult)
	{
		return location == kDownloadedChunkLocation
		    && unpackResult == kUnpackFailed;
	}

	class Registry
	{
	public:

		bool markFailure(uint32_t depotId, uint32_t appId, bool inManagedScope,
		                 const std::string& key, uint32_t unpackResult,
		                 uint64_t chunkFingerprint)
		{
			if (!depotId || !appId || !inManagedScope || key.size() != 32
			    || unpackResult != kUnpackFailed || !chunkFingerprint)
			{
				return false;
			}

			std::lock_guard<std::mutex> lock(m_lock);
			auto [it, inserted] = m_records.try_emplace(
			    depotId, Record{appId, key, {}, false});
			if (!inserted
			    && (it->second.appId != appId || it->second.key != key))
			{
				it->second = Record{appId, key, {}, false};
			}

			Record& record = it->second;
			if (record.quarantined) return false;
			record.failedChunks.insert(chunkFingerprint);
			if (record.failedChunks.size() < kDistinctFailureThreshold)
			{
				return false;
			}
			record.quarantined = true;
			return true;
		}

		bool adopt(uint32_t depotId, uint32_t appId, const std::string& key)
		{
			if (!depotId || !appId || key.size() != 32) return false;

			std::lock_guard<std::mutex> lock(m_lock);
			auto [it, inserted] = m_records.try_emplace(
			    depotId, Record{appId, key, {}, true});
			if (inserted) return true;
			if (it->second.quarantined && it->second.appId == appId
			    && it->second.key == key)
			{
				return false;
			}
			it->second = Record{appId, key, {}, true};
			return true;
		}

		bool contains(uint32_t depotId) const
		{
			std::lock_guard<std::mutex> lock(m_lock);
			auto it = m_records.find(depotId);
			return it != m_records.end() && it->second.quarantined;
		}

		bool shouldDrop(uint32_t depotId, uint32_t dlcAppId,
		                bool inManagedScope, const std::string& currentKey)
		{
			std::lock_guard<std::mutex> lock(m_lock);
			auto it = m_records.find(depotId);
			if (it == m_records.end() || !it->second.quarantined) return false;

			if (!inManagedScope || currentKey.size() != 32
			    || currentKey != it->second.key)
			{
				m_records.erase(it);
				return false;
			}

			return dlcAppId != 0;
		}

	private:
		struct Record
		{
			uint32_t appId;
			std::string key;
			std::unordered_set<uint64_t> failedChunks;
			bool quarantined;
		};

		mutable std::mutex m_lock;
		std::unordered_map<uint32_t, Record> m_records;
	};

	bool setup();
	void remove();
	bool shouldDropManagedDlc(uint32_t depotId, uint32_t dlcAppId);

	std::unordered_set<uint32_t> package0Exclusions();
}

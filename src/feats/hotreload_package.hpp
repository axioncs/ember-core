

#pragma once

#include "hotreload_types.hpp"

#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <map>
#include <set>
#include <unordered_set>
#include <vector>

namespace HotReloadPackage
{
	using IdSet = std::unordered_set<uint32_t>;

	struct Contribution
	{
		uint32_t baseAppId = 0;
		std::vector<uint32_t> appIds;
		std::vector<uint32_t> depotIds;
	};

	struct Aggregate
	{
		IdSet appIds;
		IdSet depotIds;
	};

	namespace detail
	{
		template <typename Range>
		inline bool contains(const Range& values, uint32_t id)
		{
			return std::find(std::begin(values), std::end(values), id) !=
			       std::end(values);
		}

		inline bool containsLive(const uint32_t* data, uint32_t size,
		                        uint32_t id)
		{
			if (!data) return false;
			for (uint32_t index = 0; index < size; ++index)
			{
				if (data[index] == id) return true;
			}
			return false;
		}
	}

	template <typename ActiveBases>
	inline Aggregate aggregate(
		const ActiveBases& activeBases,
		const std::vector<Contribution>& contributions)
	{
		Aggregate out;
		for (const Contribution& contribution : contributions)
		{
			if (!detail::contains(activeBases, contribution.baseAppId)) continue;
			out.appIds.insert(contribution.appIds.begin(),
			                  contribution.appIds.end());
			out.depotIds.insert(contribution.depotIds.begin(),
			                    contribution.depotIds.end());
		}
		return out;
	}

	inline Aggregate aggregate(
		std::initializer_list<uint32_t> activeBases,
		const std::vector<Contribution>& contributions)
	{
		const IdSet active(activeBases);
		return aggregate(active, contributions);
	}

	template <typename Seeded, typename Desired>
	inline void compactInjected(uint32_t* data, uint32_t& size,
	                            const Seeded& seeded,
	                            const Desired& desired)
	{
		if (!data || size == 0) return;

		uint32_t writeIndex = 0;
		for (uint32_t readIndex = 0; readIndex < size; ++readIndex)
		{
			const uint32_t id = data[readIndex];
			if (detail::contains(seeded, id) &&
			    !detail::contains(desired, id))
			{
				continue;
			}

			data[writeIndex] = id;
			++writeIndex;
		}
		size = writeIndex;
	}

	template <typename Desired>
	inline std::set<uint32_t> missingFromVector(const uint32_t* data,
	                                             uint32_t size,
	                                             const Desired& desired)
	{
		std::set<uint32_t> missing;
		for (const uint32_t id : desired)
		{
			if (!detail::containsLive(data, size, id)) missing.insert(id);
		}
		return missing;
	}

	template <typename Missing>
	inline std::vector<uint32_t> idsToRequestAfterApply(
		bool applied,
		const Missing& missing)
	{
		if (!applied) return {};
		return std::vector<uint32_t>(std::begin(missing), std::end(missing));
	}

	inline std::vector<uint32_t> snapshotAppInfoRequestIdsAfterApply(
		bool applied,
		const PackageSnapshot& snapshot)
	{
		return idsToRequestAfterApply(applied, snapshot.appInfoRequestIds);
	}

	inline PackageSnapshot carryPendingSnapshotWork(
		const PackageSnapshot& previous,
		PackageSnapshot next,
		bool previousOwnershipProcessed,
		bool previousAppInfoRequested)
	{
		const std::unordered_set<uint32_t> nextAppIds(
			next.appIds.begin(), next.appIds.end());
		const auto carryRelevant = [&nextAppIds](
			const std::vector<uint32_t>& pending,
			const std::vector<uint32_t>& current)
		{
			std::vector<uint32_t> out = pending;
			out.insert(out.end(), current.begin(), current.end());
			out.erase(
				std::remove_if(out.begin(), out.end(), [&nextAppIds](uint32_t appId) {
					return nextAppIds.count(appId) == 0;
				}),
				out.end());
			std::sort(out.begin(), out.end());
			out.erase(std::unique(out.begin(), out.end()), out.end());
			return out;
		};
		if (!previousOwnershipProcessed)
		{
			next.addedAppIds = carryRelevant(
				previous.addedAppIds, next.addedAppIds);
		}
		if (!previousAppInfoRequested)
		{
			next.appInfoRequestIds = carryRelevant(
				previous.appInfoRequestIds, next.appInfoRequestIds);
		}
		return next;
	}

	class AppInfoRequestState
	{
	public:
		std::vector<uint32_t> reserve(
			std::uint64_t generation,
			const std::vector<uint32_t>& appIds)
		{
			highestGeneration_ = std::max(highestGeneration_, generation);
			pruneAccepted();
			std::vector<uint32_t> reserved;
			reserved.reserve(appIds.size());
			const auto accepted = accepted_.find(generation);
			for (const uint32_t appId : appIds)
			{
				if (appId == 0 ||
					(accepted != accepted_.end() &&
					 accepted->second.count(appId) != 0) ||
					inFlight_.count({generation, appId}) != 0)
					continue;
				inFlight_.insert({generation, appId});
				reserved.push_back(appId);
			}
			return reserved;
		}

		void finish(
			std::uint64_t generation,
			const std::vector<uint32_t>& appIds,
			bool accepted)
		{
			for (const uint32_t appId : appIds)
				inFlight_.erase({generation, appId});
			if (!accepted ||
				(generation < highestGeneration_ &&
				 highestGeneration_ - generation > 1)) return;
			auto& completed = accepted_[generation];
			completed.insert(appIds.begin(), appIds.end());
			pruneAccepted();
		}

		bool allAccepted(
			std::uint64_t generation,
			const std::vector<uint32_t>& appIds) const
		{
			if (appIds.empty()) return true;
			const auto accepted = accepted_.find(generation);
			if (accepted == accepted_.end()) return false;
			return std::all_of(
				appIds.begin(), appIds.end(), [&](uint32_t appId) {
					return accepted->second.count(appId) != 0;
				});
		}

		void clear()
		{
			highestGeneration_ = 0;
			accepted_.clear();
			inFlight_.clear();
		}

	private:
		void pruneAccepted()
		{
			for (auto current = accepted_.begin(); current != accepted_.end();)
			{
				if (current->first < highestGeneration_ &&
					highestGeneration_ - current->first > 1)
					current = accepted_.erase(current);
				else
					++current;
			}
		}

		std::uint64_t highestGeneration_ = 0;
		std::map<std::uint64_t, std::set<uint32_t>> accepted_;
		std::set<std::pair<std::uint64_t, uint32_t>> inFlight_;
	};
}

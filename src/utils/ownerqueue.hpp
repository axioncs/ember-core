

#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <functional>
#include <mutex>
#include <utility>
#include <vector>

#include "../feats/hotreload_types.hpp"
#include "../feats/hotreload_package.hpp"

namespace OwnerQueue
{

	inline constexpr std::size_t kDefaultCapacity = 32;

	inline constexpr std::size_t kMaxIdsPerCommand = 4096;

	inline std::uint64_t monotonicUs()
	{
		struct timespec ts{};
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return static_cast<std::uint64_t>(ts.tv_sec) * 1000000ULL
		     + static_cast<std::uint64_t>(ts.tv_nsec) / 1000ULL;
	}

	enum class Kind : std::uint8_t
	{
		InjectPackage0 = 0,
		ReconcileLicenses = 1,
		InstallApp = 2,
		SyncPackage0 = 3,
		EnsureCompat = 4,
	};

	class Command
	{
	public:
		static Command injectPackage0(std::vector<std::uint32_t> appIds)
		{
			return Command(Kind::InjectPackage0, std::move(appIds), 0, 0, {});
		}
		static Command reconcileLicenses()
		{
			return Command(Kind::ReconcileLicenses, {}, 0, 0, {});
		}
		static Command installApp(std::uint32_t appId, std::uint32_t library)
		{
			return Command(Kind::InstallApp, {}, appId, library, {});
		}
		static Command syncPackage0(PackageSnapshot snapshot)
		{
			return Command(Kind::SyncPackage0, {}, 0, 0, std::move(snapshot));
		}
		static Command ensureCompat(
			std::uint32_t appId,
			std::uint64_t managedGeneration,
			std::uint32_t attempt)
		{
			return Command(Kind::EnsureCompat, {}, appId, 0, {},
				managedGeneration, attempt);
		}

		Kind kind() const { return m_kind; }
		const std::vector<std::uint32_t>& appIds() const { return m_appIds; }
		const PackageSnapshot& packageSnapshot() const { return m_packageSnapshot; }
		std::uint32_t appId() const { return m_appId; }
		std::uint32_t library() const { return m_library; }
		std::uint64_t managedGeneration() const { return m_managedGeneration; }
		std::uint32_t attempt() const { return m_attempt; }

		bool sameWork(const Command& other) const
		{
			if (m_kind != other.m_kind)
				return false;
			switch (m_kind)
			{
				case Kind::InjectPackage0:
					return m_appIds == other.m_appIds;
				case Kind::ReconcileLicenses:
					return true;
				case Kind::InstallApp:
					return m_appId == other.m_appId && m_library == other.m_library;
				case Kind::SyncPackage0:
					return m_packageSnapshot == other.m_packageSnapshot;
				case Kind::EnsureCompat:
					return m_appId == other.m_appId &&
						m_managedGeneration == other.m_managedGeneration;
			}
			return false;
		}

	private:
		Command(Kind kind, std::vector<std::uint32_t> appIds,
		        std::uint32_t appId, std::uint32_t library,
		        PackageSnapshot packageSnapshot,
		        std::uint64_t managedGeneration = 0,
		        std::uint32_t attempt = 0)
			: m_kind(kind), m_appIds(std::move(appIds)),
			  m_packageSnapshot(std::move(packageSnapshot)),
			  m_appId(appId), m_library(library),
			  m_managedGeneration(managedGeneration), m_attempt(attempt)
		{
		}

		Kind                       m_kind;
		std::vector<std::uint32_t> m_appIds;
		PackageSnapshot            m_packageSnapshot;
		std::uint32_t              m_appId;
		std::uint32_t              m_library;
		std::uint64_t              m_managedGeneration;
		std::uint32_t              m_attempt;
	};

	enum class PushResult : std::uint8_t
	{
		Queued = 0,
		Coalesced = 1,
		Duplicate = 2,
		Full = 3,
		ShuttingDown = 4,
	};

	struct Stats
	{
		std::uint64_t pushed = 0;
		std::uint64_t coalesced = 0;
		std::uint64_t duplicates = 0;
		std::uint64_t rejectedFull = 0;
		std::uint64_t rejectedShutdown = 0;
		std::uint64_t drained = 0;
		std::uint64_t flushedStale = 0;
		std::uint64_t ranInline = 0;
		std::uint64_t executed = 0;
		std::uint64_t failed = 0;
		std::uint64_t abandoned = 0;
	};

	inline bool shouldDrain(long selfTid, long ownerTid, bool alreadyDraining,
	                        std::size_t depthHint)
	{
		return depthHint != 0
		    && !alreadyDraining
		    && ownerTid != 0
		    && selfTid == ownerTid;
	}

	inline bool isStale(std::uint64_t oldestUs, std::uint64_t nowUs,
	                    std::uint64_t maxStaleUs)
	{
		if (maxStaleUs == 0 || oldestUs == 0 || nowUs <= oldestUs)
			return false;
		return (nowUs - oldestUs) >= maxStaleUs;
	}

	class PlacementGate
	{
	public:
		void notePlacement() { m_placed.store(true, std::memory_order_release); }
		bool placed() const { return m_placed.load(std::memory_order_acquire); }

		bool claimTeardown() { return m_placed.exchange(false, std::memory_order_acq_rel); }

	private:
		std::atomic<bool> m_placed{ false };
	};

	class Queue
	{
	public:
		using Runner = std::function<void(const Command&)>;

		explicit Queue(std::size_t capacity = kDefaultCapacity,
		               std::size_t maxIdsPerCommand = kMaxIdsPerCommand)
			: m_capacity(capacity ? capacity : 1),
			  m_maxIds(maxIdsPerCommand ? maxIdsPerCommand : 1)
		{
		}

		Queue(const Queue&) = delete;
		Queue& operator=(const Queue&) = delete;

		PushResult push(const Command& cmd, std::uint64_t nowUs)
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			const PushResult r = pushLocked(cmd, nowUs);
			syncHintLocked();
			return r;
		}

		std::size_t depthHint() const
		{
			return m_depthHint.load(std::memory_order_relaxed);
		}

		bool pushBatch(const std::vector<Command>& batch, std::uint64_t nowUs,
		               std::vector<PushResult>* perCommand = nullptr)
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			if (m_shuttingDown)
			{
				m_stats.rejectedShutdown += batch.size();
				if (perCommand)
					perCommand->assign(batch.size(), PushResult::ShuttingDown);
				return false;
			}

			std::vector<Entry> scratch = m_pending;
			for (const Command& cmd : batch)
			{
				if (!applyPush(scratch, cmd, nowUs, nullptr))
				{
					m_stats.rejectedFull += batch.size();
					if (perCommand)
						perCommand->assign(batch.size(), PushResult::Full);
					return false;
				}
			}

			if (perCommand)
				perCommand->clear();
			for (const Command& cmd : batch)
			{
				const PushResult r = pushLocked(cmd, nowUs);
				if (perCommand)
					perCommand->push_back(r);
			}
			syncHintLocked();
			return true;
		}

		std::size_t drain(const Runner& run)
		{
			std::vector<Entry> batch;
			if (!claim(batch, ClaimKind::Drain, 0, 0))
				return 0;
			runClaimed(batch, run);
			return batch.size();
		}

		std::size_t flushStale(std::uint64_t nowUs, std::uint64_t maxStaleUs,
		                       const Runner& run)
		{
			std::vector<Entry> batch;
			if (!claim(batch, ClaimKind::Stale, nowUs, maxStaleUs))
				return 0;
			runClaimed(batch, run);
			return batch.size();
		}

		std::size_t runPendingThen(const std::vector<Command>& extra, const Runner& run)
		{
			std::vector<Entry> batch;

			const bool claimed = claim(batch, ClaimKind::Inline, 0, 0);
			for (const Command& cmd : extra)
				batch.push_back(Entry{ cmd, 0 });

			if (batch.empty())
			{
				if (claimed)
					releaseExecuting();
				return 0;
			}
			{
				std::lock_guard<std::mutex> lk(m_mutex);
				m_stats.ranInline += batch.size();
			}
			runBatch(batch, run);
			if (claimed)
				releaseExecuting();
			return batch.size();
		}

		void shutdown()
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			if (m_shuttingDown)
				return;
			m_shuttingDown = true;
			m_stats.abandoned += m_pending.size();
			m_pending.clear();
			syncHintLocked();
		}

		bool isShuttingDown() const
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_shuttingDown;
		}

		std::size_t depth() const
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_pending.size();
		}

		std::uint64_t oldestAgeUs(std::uint64_t nowUs) const
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			if (m_pending.empty())
				return 0;
			const std::uint64_t oldest = m_pending.front().enqueuedUs;
			return (nowUs > oldest) ? nowUs - oldest : 0;
		}

		std::size_t capacity() const { return m_capacity; }

		Stats stats() const
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_stats;
		}

	private:

		struct Entry
		{
			Command       cmd;
			std::uint64_t enqueuedUs = 0;
		};

		enum class ClaimKind : std::uint8_t
		{
			Drain = 0,
			Stale = 1,
			Inline = 2,
		};

		static std::vector<std::uint32_t> mergeIds(
			const std::vector<std::uint32_t>& base,
			const std::vector<std::uint32_t>& extra)
		{
			std::vector<std::uint32_t> out = base;
			for (std::uint32_t id : extra)
			{
				bool seen = false;
				for (std::uint32_t have : out)
				{
					if (have == id)
					{
						seen = true;
						break;
					}
				}
				if (!seen)
					out.push_back(id);
			}
			return out;
		}

		bool applyPush(std::vector<Entry>& into, const Command& cmd,
		               std::uint64_t nowUs, PushResult* result) const
		{
			if (cmd.appIds().size() > m_maxIds)
				return false;
			if (cmd.kind() == Kind::SyncPackage0 &&
			    (cmd.packageSnapshot().appIds.size() > m_maxIds ||
			     cmd.packageSnapshot().depotIds.size() > m_maxIds ||
			     cmd.packageSnapshot().addedAppIds.size() > m_maxIds ||
			     cmd.packageSnapshot().appInfoRequestIds.size() > m_maxIds))
				return false;

			for (std::size_t i = 0; i < into.size(); ++i)
			{
				Entry& pending = into[i];
				if (pending.cmd.kind() != cmd.kind())
					continue;

				if (pending.cmd.sameWork(cmd))
				{
					if (result)
						*result = PushResult::Duplicate;
					return true;
				}

				if (cmd.kind() == Kind::InjectPackage0)
				{
					auto merged = mergeIds(pending.cmd.appIds(), cmd.appIds());
					if (merged.size() > m_maxIds)
						return false;
					if (merged.size() == pending.cmd.appIds().size())
					{

						if (result)
							*result = PushResult::Duplicate;
						return true;
					}

					into[i] = Entry{ Command::injectPackage0(std::move(merged)),
					                 pending.enqueuedUs };
					if (result)
						*result = PushResult::Coalesced;
					return true;
				}

				if (cmd.kind() == Kind::SyncPackage0)
				{

					if (cmd.packageSnapshot().generation <=
					    pending.cmd.packageSnapshot().generation)
					{
						if (result)
							*result = PushResult::Duplicate;
						return true;
					}

					const std::uint64_t oldest = pending.enqueuedUs;
					PackageSnapshot replacement =
						HotReloadPackage::carryPendingSnapshotWork(
							pending.cmd.packageSnapshot(), cmd.packageSnapshot(),
							false, false);
					if (replacement.addedAppIds.size() > m_maxIds ||
						replacement.appInfoRequestIds.size() > m_maxIds)
						return false;
					into[i] = Entry{
						Command::syncPackage0(std::move(replacement)), oldest };
					if (result)
						*result = PushResult::Coalesced;
					return true;
				}
			}

			if (into.size() >= m_capacity)
				return false;

			into.push_back(Entry{ cmd, nowUs });
			if (result)
				*result = PushResult::Queued;
			return true;
		}

		PushResult pushLocked(const Command& cmd, std::uint64_t nowUs)
		{
			if (m_shuttingDown)
			{
				++m_stats.rejectedShutdown;
				return PushResult::ShuttingDown;
			}

			PushResult result = PushResult::Queued;
			if (!applyPush(m_pending, cmd, nowUs, &result))
			{
				++m_stats.rejectedFull;
				return PushResult::Full;
			}

			switch (result)
			{
				case PushResult::Queued:    ++m_stats.pushed; break;
				case PushResult::Coalesced: ++m_stats.coalesced; break;
				case PushResult::Duplicate: ++m_stats.duplicates; break;
				default: break;
			}
			return result;
		}

		bool claim(std::vector<Entry>& out, ClaimKind kind,
		           std::uint64_t nowUs, std::uint64_t maxStaleUs)
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			if (m_executing)
				return false;
			if (kind == ClaimKind::Stale && m_shuttingDown)
				return false;
			if (m_pending.empty())
			{

				if (kind != ClaimKind::Inline)
					return false;
				m_executing = true;
				return true;
			}
			if (kind == ClaimKind::Stale
			    && !isStale(m_pending.front().enqueuedUs, nowUs, maxStaleUs))
				return false;

			out.swap(m_pending);
			m_executing = true;
			if (kind == ClaimKind::Drain)
				m_stats.drained += out.size();
			else if (kind == ClaimKind::Stale)
				m_stats.flushedStale += out.size();
			syncHintLocked();
			return true;
		}

		void runBatch(const std::vector<Entry>& batch, const Runner& run)
		{

			std::lock_guard<std::recursive_mutex> ex(m_exec);
			for (const Entry& entry : batch)
			{
				bool threw = false;
				try
				{
					if (run)
						run(entry.cmd);
				}
				catch (...)
				{
					threw = true;
				}
				std::lock_guard<std::mutex> lk(m_mutex);
				if (threw)
					++m_stats.failed;
				else
					++m_stats.executed;
			}
		}

		void releaseExecuting()
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			m_executing = false;
		}

		void runClaimed(const std::vector<Entry>& batch, const Runner& run)
		{
			runBatch(batch, run);
			releaseExecuting();
		}

		void syncHintLocked()
		{
			m_depthHint.store(m_pending.size(), std::memory_order_relaxed);
		}

		mutable std::mutex       m_mutex;
		std::recursive_mutex     m_exec;
		std::atomic<std::size_t> m_depthHint{ 0 };
		std::vector<Entry>       m_pending;
		Stats                    m_stats;
		std::size_t              m_capacity;
		std::size_t              m_maxIds;
		bool                     m_executing = false;
		bool                     m_shuttingDown = false;
	};

	inline bool applyTeardown(PlacementGate& gate, Queue& queue)
	{
		if (!gate.claimTeardown())
			return false;
		queue.shutdown();
		return true;
	}
}

#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>

namespace SteamStub
{

	inline bool tryBeginWarmup(std::mutex& warmupMu,
	                           std::atomic<bool>& started,
	                           std::atomic<bool>& done,
	                           std::atomic<bool>& failed,
	                           std::uint64_t& generation)
	{
		std::lock_guard<std::mutex> lk(warmupMu);
		bool expected = false;
		if (!started.compare_exchange_strong(expected, true,
		                                     std::memory_order_acq_rel))
		{
			return false;
		}
		++generation;
		done.store(false, std::memory_order_release);
		failed.store(false, std::memory_order_release);
		return true;
	}

	inline bool tryBeginWarmup(std::mutex& warmupMu,
	                           std::atomic<bool>& started,
	                           std::atomic<bool>& done,
	                           std::atomic<bool>& failed)
	{
		std::uint64_t generation = 0;
		return tryBeginWarmup(warmupMu, started, done, failed, generation);
	}

	inline void recordWarmupResult(std::atomic<bool>& failed, int exitCode) noexcept
	{
		if (exitCode != 0)
			failed.store(true, std::memory_order_release);
	}

	inline void finishWarmup(std::mutex& warmupMu,
	                         std::atomic<bool>& started,
	                         std::atomic<bool>& done,
	                         std::atomic<bool>& failed)
	{
		std::lock_guard<std::mutex> lk(warmupMu);
		if (failed.load(std::memory_order_acquire))
			started.store(false, std::memory_order_release);
		done.store(true, std::memory_order_release);
	}

	inline bool shouldRetryWarmup(bool started, bool done, bool failed) noexcept
	{
		return !started && done && failed;
	}
}

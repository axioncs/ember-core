#pragma once

#include <memory>
#include <thread>
#include <utility>
namespace ThreadStart
{
	struct NoopDetachFailure
	{
		void operator()() const noexcept {}
	};

	struct NoopRecoveryFailure
	{
		void operator()() const noexcept {}
	};

	struct StdThreadDetacher
	{
		void operator()(std::thread& worker) const
		{
			worker.detach();
		}
	};

	struct StdThreadJoiner
	{
		void operator()(std::thread& worker) const
		{
			worker.join();
		}
	};

	template <typename Callback>
	void invokeNoThrow(Callback&& callback) noexcept
	{
		try
		{
			callback();
		}
		catch (...)
		{

		}
	}

	template <typename Worker, typename OnFailure,
	          typename OnDetachFailure = NoopDetachFailure,
	          typename Detacher = StdThreadDetacher,
	          typename Joiner = StdThreadJoiner,
	          typename OnRecoveryFailure = NoopRecoveryFailure>
	bool startDetached(Worker&& worker, OnFailure&& onFailure,
	                   OnDetachFailure&& onDetachFailure = OnDetachFailure{},
	                   Detacher&& detacher = Detacher{},
	                   Joiner&& joiner = Joiner{},
	                   OnRecoveryFailure&& onRecoveryFailure = OnRecoveryFailure{}) noexcept
	{
		std::unique_ptr<std::thread> workerThread;
		try
		{
			workerThread = std::make_unique<std::thread>(std::forward<Worker>(worker));
		}
		catch (...)
		{
			invokeNoThrow(std::forward<OnFailure>(onFailure));
			return false;
		}

		try
		{
			detacher(*workerThread);
			workerThread.release();
			return true;
		}
		catch (...)
		{
			invokeNoThrow(std::forward<OnDetachFailure>(onDetachFailure));

			bool joined = false;
			try
			{
				if (workerThread && workerThread->joinable())
				{
					std::forward<Joiner>(joiner)(*workerThread);
					joined = !workerThread->joinable();
				}
			}
			catch (...)
			{

			}

			if (joined)
			{
				invokeNoThrow(std::forward<OnFailure>(onFailure));
				return false;
			}

			bool detached = false;
			try
			{
				if (workerThread && workerThread->joinable())
				{
					detacher(*workerThread);
					detached = !workerThread->joinable();
				}
			}
			catch (...)
			{

			}

			if (!detached && workerThread && workerThread->joinable())
				workerThread.release();
			invokeNoThrow(std::forward<OnRecoveryFailure>(onRecoveryFailure));
			return false;
		}
	}

	template <typename Starter, typename OnFailure>
	bool tryStart(Starter&& starter, OnFailure&& onFailure) noexcept
	{
		try
		{
			starter();
			return true;
		}
		catch (...)
		{
			try
			{
				onFailure();
			}
			catch (...)
			{

			}
			return false;
		}
	}

	template <typename Worker, typename OnFailure, typename OnExit>
	void runGuarded(Worker&& worker, OnFailure&& onFailure,
	                OnExit&& onExit) noexcept
	{
		try
		{
			worker();
		}
		catch (...)
		{
			try
			{
				onFailure();
			}
			catch (...)
			{

			}
		}

		try
		{
			onExit();
		}
		catch (...)
		{

		}
	}
}
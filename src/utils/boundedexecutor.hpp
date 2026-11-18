

#pragma once

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

class BoundedExecutor
{
public:
	explicit BoundedExecutor(std::size_t workerCount,
	                         std::size_t maxQueue = 0)
		: m_maxQueue(maxQueue)
	{
		try
		{
			m_workers.reserve(workerCount);
			for (std::size_t i = 0; i < workerCount; ++i)
			{
				m_workers.emplace_back([this] { run(); });
			}
		}
		catch (...)
		{
			stopAndJoin();
		}
	}

	~BoundedExecutor()
	{
		stopAndJoin();
	}

	BoundedExecutor(const BoundedExecutor&) = delete;
	BoundedExecutor& operator=(const BoundedExecutor&) = delete;

	bool submit(std::function<void()> task)
	{
		if (!task) return false;
		{
			std::lock_guard<std::mutex> lk(m_lock);
			if (m_stopping || m_workers.empty()) return false;
			if (m_maxQueue != 0 && m_tasks.size() >= m_maxQueue) return false;
			try
			{
				m_tasks.push_back(std::move(task));
			}
			catch (...)
			{
				return false;
			}
		}
		m_ready.notify_one();
		return true;
	}

	std::size_t workerCount() const noexcept
	{
		return m_workers.size();
	}

private:
	void run()
	{
		for (;;)
		{
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lk(m_lock);
				m_ready.wait(lk, [this]
				{
					return m_stopping || !m_tasks.empty();
				});
				if (m_stopping && m_tasks.empty()) return;
				task = std::move(m_tasks.front());
				m_tasks.pop_front();
			}

			try { task(); } catch (...) {}
		}
	}

	void stopAndJoin() noexcept
	{
		{
			std::lock_guard<std::mutex> lk(m_lock);
			m_stopping = true;
		}
		m_ready.notify_all();
		for (auto& worker : m_workers)
		{
			if (worker.joinable()) worker.join();
		}
		m_workers.clear();
	}

	mutable std::mutex m_lock;
	std::condition_variable m_ready;
	std::deque<std::function<void()>> m_tasks;
	std::vector<std::thread> m_workers;
	std::size_t m_maxQueue = 0;
	bool m_stopping = false;
};

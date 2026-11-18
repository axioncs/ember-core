

#pragma once

#include <mutex>
#include <utility>

namespace AppInfoProvision
{

class ProvisionPassCoordinator
{
public:
	explicit ProvisionPassCoordinator(std::mutex& mutex) : mutex_(mutex) {}

	template <typename Fn>
	decltype(auto) snapshot(Fn&& fn)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return std::forward<Fn>(fn)();
	}

	template <typename Fn>
	decltype(auto) commit(Fn&& fn)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return std::forward<Fn>(fn)();
	}

	template <typename Fn>
	decltype(auto) network(Fn&& fn)
	{
		return std::forward<Fn>(fn)();
	}

private:
	std::mutex& mutex_;
};

}

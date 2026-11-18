

#pragma once

#include <mutex>

namespace Updater
{
namespace cache
{

class RefreshGate
{
public:
	bool tryStart() noexcept
	{
		std::lock_guard<std::mutex> lk(m_lock);
		if (m_started) return false;
		m_started = true;
		m_succeeded = false;
		return true;
	}

	void markSucceeded() noexcept
	{
		std::lock_guard<std::mutex> lk(m_lock);
		m_succeeded = true;
	}

	void finish() noexcept
	{
		std::lock_guard<std::mutex> lk(m_lock);
		if (!m_succeeded) m_started = false;
	}

private:
	std::mutex m_lock;
	bool m_started = false;
	bool m_succeeded = false;
};

inline bool isCacheFresh(bool cacheExists, long long mtimeSecs,
                         long long nowSecs, long long ttlSecs)
{
	if (!cacheExists)  return false;
	if (ttlSecs <= 0)  return false;

	const long long age = nowSecs - mtimeSecs;
	if (age < 0)       return false;
	return age < ttlSecs;
}

inline bool mustFetchSynchronously(bool safeMode, bool warnHashMissmatch)
{
	return safeMode || warnHashMissmatch;
}

inline bool mustVerifyClientHash(bool safeMode, bool warnHashMissmatch,
                                 bool extendedLogging)
{
	return safeMode || warnHashMissmatch || extendedLogging;
}

}
}

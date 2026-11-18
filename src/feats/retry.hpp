

#pragma once

#include <functional>

namespace AppInfoProvision
{

inline bool retryWithBackoff(const std::function<bool()>& op,
                             int maxAttempts,
                             int baseDelayMs,
                             const std::function<void(int)>& sleepMs)
{
	if (maxAttempts < 1) maxAttempts = 1;
	for (int attempt = 1; attempt <= maxAttempts; ++attempt)
	{
		if (op())
		{
			return true;
		}
		if (attempt < maxAttempts)
		{
			sleepMs(baseDelayMs * attempt);
		}
	}
	return false;
}

}

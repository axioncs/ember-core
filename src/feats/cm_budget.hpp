

#pragma once

#include <algorithm>

namespace CmClient
{

inline long remainingBudgetSeconds(long long elapsedMs, long long budgetMs,
                                   long operationCapSecs)
{
	if (budgetMs <= 0 || operationCapSecs <= 0 || elapsedMs >= budgetMs)
		return 0;
	const long long remainingMs = budgetMs - std::max(0LL, elapsedMs);
	const long long roundedSecs = (remainingMs + 999) / 1000;
	return static_cast<long>(std::min<long long>(roundedSecs, operationCapSecs));
}

}

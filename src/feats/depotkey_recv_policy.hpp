

#pragma once

namespace DepotKey
{
	enum class RecvAction
	{
		None,
		CacheObserved,
		SubstituteCached,
		SynthZero,
	};

	inline RecvAction classifyRecv(bool eresultOk, bool okKeyIs32,
	                               bool haveCachedKey, bool isAddedDepot) noexcept
	{
		if (eresultOk)
		{
			return okKeyIs32 ? RecvAction::CacheObserved : RecvAction::None;
		}
		if (haveCachedKey)
		{
			return RecvAction::SubstituteCached;
		}
		if (isAddedDepot)
		{
			return RecvAction::SynthZero;
		}
		return RecvAction::None;
	}
}

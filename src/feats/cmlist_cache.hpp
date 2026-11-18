

#pragma once

namespace CmClient
{
namespace cache
{

inline bool isCmListReusable(bool listExists, long long mtimeSecs,
                             long long nowSecs, long long ttlSecs)
{
	if (!listExists) return false;
	if (ttlSecs <= 0) return false;

	if (mtimeSecs > nowSecs) return false;
	const long long age = nowSecs - mtimeSecs;
	return age < ttlSecs;
}

}
}

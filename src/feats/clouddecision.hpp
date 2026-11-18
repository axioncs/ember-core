

#pragma once

namespace Apps
{

inline bool cloudDisableDecision(bool disableCloudEnabled,
                                 bool managedApp,
                                 bool playNotOwnedGames,
                                 bool steamReportsOwned)
{
	if (!disableCloudEnabled)
	{
		return false;
	}
	if (managedApp)
	{
		return true;
	}
	if (!playNotOwnedGames)
	{
		return false;
	}
	return !steamReportsOwned;
}

}

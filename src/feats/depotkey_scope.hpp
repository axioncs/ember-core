

#pragma once

#include <cstdint>

namespace DepotKey
{

	inline bool depotInManifestScope(bool appIsAddedApp,
	                                 bool depotIsAddedApp,
	                                 bool depotKeyIsManaged,
	                                 bool depotHasPin,
	                                 bool contentHasNativeLicense = false)
	{
		if (contentHasNativeLicense) return false;
		if (depotHasPin) return true;
		return appIsAddedApp || depotIsAddedApp || depotKeyIsManaged;
	}

	inline uint32_t manifestContentAppId(uint32_t baseAppId,
	                                    uint32_t requestedAppId,
	                                    uint32_t catalogAppId,
	                                    uint32_t depotDlcAppId)
	{
		if (depotDlcAppId) return depotDlcAppId;
		if (requestedAppId && requestedAppId != baseAppId)
			return requestedAppId;
		return baseAppId ? baseAppId : catalogAppId;
	}

	inline bool mergeManagedFlag(bool existingManaged, bool incomingManaged)
	{
		return existingManaged || incomingManaged;
	}
}

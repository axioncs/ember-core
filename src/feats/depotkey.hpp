// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include "../sdk/sdk.hpp"

namespace DepotKey
{
	// Rewrites a denied GetDepotDecryptionKey response with an imported key.
	void recvMsg(CNetPacket* pkt);
}

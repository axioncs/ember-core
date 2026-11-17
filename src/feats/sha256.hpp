// SPDX-License-Identifier: AGPL-3.0-only
//
// Self-contained SHA-256. The client's mount namespace has sha256sum, but a
// per-file subprocess on the upload path is a fork per save, and the store has
// to be testable on the host where no such binary is guaranteed.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace Ember
{
	// Lowercase hex SHA-256 over a byte range.
	std::string sha256Hex(const uint8_t* data, const size_t len);

	inline std::string sha256Hex(const std::string& data)
	{
		return sha256Hex(reinterpret_cast<const uint8_t*>(data.data()), data.size());
	}
}
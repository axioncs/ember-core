

#pragma once

#include <cstdint>

namespace LibraryRemoval
{

	bool setup() noexcept;

	void remove() noexcept;

	bool ready() noexcept;

	void queue(std::uint32_t appId) noexcept;
	void cancel(std::uint32_t appId) noexcept;

	void restore(std::uint32_t appId) noexcept;
}

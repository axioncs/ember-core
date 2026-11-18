#pragma once

#include <cstdint>

namespace AppInfoStatePolicy
{
enum class Action : std::uint8_t
{
	None,
	MarkSkip,
	SignalResolved,
	ClearSkipAndSignalResolved,
};

constexpr Action decide(
	bool managed,
	[[maybe_unused]] bool authoritative,
	bool create,
	bool shaEmpty,
	bool skipSet
) noexcept
{
	if (!managed || create)
		return Action::None;

	if (!shaEmpty)
		return skipSet ? Action::ClearSkipAndSignalResolved
		               : Action::SignalResolved;

	return skipSet ? Action::None : Action::MarkSkip;
}
}



#pragma once

namespace LicenseRefreshPolicy
{
enum class Action
{
	NoChange,
	Defer,
	Process,
};

constexpr bool unresolvedStateSafe(
	bool hasPendingUnresolvedAddition,
	bool guardReady
) noexcept
{
	return !hasPendingUnresolvedAddition || guardReady;
}

constexpr Action decide(
	bool markReady,
	bool processReady,
	bool unresolvedStateSafe,
	bool changed
) noexcept
{
	if (!changed)
		return Action::NoChange;
	if (!markReady || !processReady || !unresolvedStateSafe)
		return Action::Defer;
	return Action::Process;
}
}

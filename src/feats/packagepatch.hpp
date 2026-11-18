

#pragma once

#include "hotreload_types.hpp"

#include <cstdint>
#include <vector>

namespace PackagePatch
{

	bool isInjectedAppId(uint32_t appId);

	bool setup();

	void remove();

	bool injectIntoPackage0(const std::vector<uint32_t>& appIds);

	void setExtraAppIds(const std::vector<uint32_t>& appIds);

	void tryReconcileLicenses();

	void forceReconcileLicenses();

	void synchronizePackage0(const PackageSnapshot& snapshot);

	bool runtimeRefreshReady();

	bool runtimeRefreshPending() noexcept;

	void reprocessCurrentState() noexcept;
}

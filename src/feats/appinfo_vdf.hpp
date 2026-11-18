

#pragma once

#include <cstdint>
#ifdef APPINFO_VDF_TESTING
#include <functional>
#endif
#include <string>
#include <unordered_set>
#include <vector>

#include "synthmark.hpp"

namespace AppInfoVdf
{
	struct MetadataApp
	{
		uint32_t appid = 0;
		uint32_t changeNumber = 0;
		std::string sha;
		std::string wireBuffer;
	};
	using MetadataCommitGuard = bool (*)(void*) noexcept;

	std::string findExistingPath();

	bool injectApp(const std::string& path,
	               uint32_t appid,
	               uint32_t changeNumber,
	               const std::string& sha,
	               const std::string& wireBuffer);

	int injectAllCached(const std::string& path);

	int injectCachedApps(const std::string& path,
	                     const std::unordered_set<uint32_t>& requestedApps);

	int injectValidatedMetadataApps(
		const std::string& path,
		const std::vector<MetadataApp>& metadataApps);

	int injectValidatedMetadataAppsGuarded(
		const std::string& path,
		const std::vector<MetadataApp>& metadataApps,
		void* context,
		MetadataCommitGuard guard);

#ifdef APPINFO_VDF_TESTING
	void setBeforeScopedPublishHook(std::function<void()> hook);
#endif
}

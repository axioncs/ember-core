
#pragma once

#include <cstdint>
#include <map>
#include <shared_mutex>
#include <string>
#include <unordered_set>

namespace LibraryDates
{

class Store
{
public:
	bool refresh(const std::string& configDir, const std::string& scriptDir,
	             const std::unordered_set<uint32_t>& appIds, uint32_t now,
	             std::string& error);
	uint32_t get(uint32_t appId) const;

private:
	mutable std::shared_mutex mutex_;
	std::map<uint32_t, uint32_t> times_;
	std::string path_;
	bool initialized_ = false;
};
}

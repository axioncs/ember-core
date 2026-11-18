

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace CmClient
{

enum class FetchResult
{
	Success,
	NetworkUnavailable,
	Failed,
};

bool prepareForThreadedFetch();

FetchResult fetchProductInfoDetailed(
    const std::vector<uint32_t>& appids,
    std::unordered_map<uint32_t, std::string>& out,
    std::unordered_map<uint32_t, uint32_t>* changesOut = nullptr);

bool fetchProductInfo(const std::vector<uint32_t>& appids,
                      std::unordered_map<uint32_t, std::string>& out,
                      std::unordered_map<uint32_t, uint32_t>* changesOut = nullptr);

}

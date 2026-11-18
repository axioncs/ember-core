#pragma once

#include <string>
#include <type_traits>
#include <cstdint>
#include <vector>

namespace Utils
{
	double calculateEntropy(const std::vector<uint8_t>& bytes);
	bool isNumber(const char* str);

	template<typename T>
	bool tryConvertToNumber(const char* str, T& out)
	{
		if constexpr (std::is_same_v<T, int32_t>)
		{
			if (!isNumber(str))
			{
				return false;
			}

			out = std::stoi(str);
			return true;
		}

		else if constexpr (std::is_same_v<T, uint32_t>)
		{
			if (!isNumber(str))
			{
				return false;
			}

			out = std::stoul(str);
			return true;
		}

		return false;
	}

	std::vector<std::string> strsplit(const char* str, const char* delimeter);
	std::string getFileSHA256(const char* filePath);

	std::string getBuildId(const char* filePath);
}

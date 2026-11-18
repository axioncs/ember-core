#pragma once

#include <cstddef>
#include <string>

namespace CompatTool
{

	inline std::size_t matchBrace(const std::string& s, std::size_t open)
	{
		if (open >= s.size() || s[open] != '{') return std::string::npos;
		int depth = 0;
		for (std::size_t i = open; i < s.size(); ++i)
		{
			if (s[i] == '{') ++depth;
			else if (s[i] == '}')
			{
				if (--depth == 0) return i;
			}
		}
		return std::string::npos;
	}

	inline std::string readName(const std::string& s, std::size_t open, std::size_t close)
	{
		std::size_t namePos = s.find("\"name\"", open);
		if (namePos == std::string::npos || namePos >= close) return {};
		std::size_t q1 = s.find('"', namePos + 6);
		if (q1 == std::string::npos || q1 >= close) return {};
		std::size_t q2 = s.find('"', q1 + 1);
		if (q2 == std::string::npos || q2 >= close) return {};
		return s.substr(q1 + 1, q2 - q1 - 1);
	}

	inline std::string parseDefaultTool(const std::string& content)
	{
		const std::size_t mapPos = content.find("\"CompatToolMapping\"");
		if (mapPos == std::string::npos) return {};
		const std::size_t mapBrace = content.find('{', mapPos);
		if (mapBrace == std::string::npos) return {};
		const std::size_t mapEnd = matchBrace(content, mapBrace);
		if (mapEnd == std::string::npos) return {};

		static const std::string zeroKey = "\"0\"";
		std::size_t k = mapBrace;
		while ((k = content.find(zeroKey, k)) != std::string::npos && k < mapEnd)
		{

			std::size_t b = content.find_first_not_of(" \t\r\n", k + zeroKey.size());
			if (b != std::string::npos && b < mapEnd && content[b] == '{')
			{
				std::size_t objEnd = matchBrace(content, b);
				if (objEnd == std::string::npos) objEnd = mapEnd;
				return readName(content, b, objEnd);
			}
			k += zeroKey.size();
		}
		return {};
	}
}

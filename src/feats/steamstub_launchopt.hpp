

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace SteamlessLaunchOpt
{

	inline std::string parseAppLaunchOptions(const std::string& vdf, std::uint32_t appId)
	{
		static const std::string appsKey = "\"apps\"";
		std::size_t apps = vdf.find(appsKey);
		if (apps == std::string::npos) return {};

		std::size_t appsOpen = vdf.find('{', apps + appsKey.size());
		if (appsOpen == std::string::npos) return {};

		const std::string needle = "\"" + std::to_string(appId) + "\"";

		std::size_t pos = appsOpen + 1;
		int depth = 1;
		std::size_t appOpen = std::string::npos;
		while (pos < vdf.size() && depth >= 1)
		{
			const char c = vdf[pos];
			if (c == '{') { ++depth; ++pos; continue; }
			if (c == '}') { if (--depth == 0) break; ++pos; continue; }
			if (c == '"' && depth == 1
			    && vdf.compare(pos, needle.size(), needle) == 0)
			{
				appOpen = vdf.find('{', pos + needle.size());
				break;
			}
			++pos;
		}
		if (appOpen == std::string::npos) return {};

		static const std::string loKey = "\"LaunchOptions\"";
		pos = appOpen + 1;
		depth = 1;
		while (pos < vdf.size() && depth >= 1)
		{
			const char c = vdf[pos];
			if (c == '{') { ++depth; ++pos; continue; }
			if (c == '}') { if (--depth == 0) break; ++pos; continue; }
			if (c == '"' && vdf.compare(pos, loKey.size(), loKey) == 0)
			{
				std::size_t vOpen = vdf.find('"', pos + loKey.size());
				if (vOpen == std::string::npos) return {};
				std::string val;
				for (std::size_t i = vOpen + 1; i < vdf.size(); ++i)
				{
					const char ch = vdf[i];
					if (ch == '\\' && i + 1 < vdf.size()) { val += vdf[++i]; continue; }
					if (ch == '"') break;
					val += ch;
				}
				return val;
			}
			++pos;
		}
		return {};
	}

	inline bool requestsSteamless(const std::string& opts)
	{
		static const std::string tok = "-steamless";
		std::size_t p = 0;
		while ((p = opts.find(tok, p)) != std::string::npos)
		{
			const bool leftOk = (p == 0)
				|| opts[p - 1] == ' ' || opts[p - 1] == '\t' || opts[p - 1] == '-';
			const std::size_t end = p + tok.size();
			const bool rightOk = (end == opts.size())
				|| opts[end] == ' ' || opts[end] == '\t';
			if (leftOk && rightOk) return true;
			p = end;
		}
		return false;
	}
}

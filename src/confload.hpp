#pragma once

#include "confnormalize.hpp"
#include "yaml-cpp/yaml.h"

#include <string>

namespace ConfLoad
{
	enum class Outcome
	{
		ParsedAsIs,
		Repaired,
		Failed,
	};

	namespace detail
	{
		inline bool tryLoad(const std::string& text, YAML::Node& out)
		{
			try
			{
				out = YAML::Load(text);
				return true;
			}
			catch (...)
			{
				return false;
			}
		}
	}

	inline Outcome parseWithRepair(const std::string& raw, YAML::Node& node,
	                               std::string& repaired)
	{
		bool changed = false;
		repaired = ConfNormalize::repairSeqIndent(raw, &changed);

		if (changed)
		{

			if (detail::tryLoad(repaired, node)) return Outcome::Repaired;

			if (detail::tryLoad(raw, node)) return Outcome::ParsedAsIs;
			node = YAML::Node();
			return Outcome::Failed;
		}

		if (detail::tryLoad(raw, node)) return Outcome::ParsedAsIs;
		node = YAML::Node();
		return Outcome::Failed;
	}
}

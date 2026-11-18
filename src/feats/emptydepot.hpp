

#pragma once

#include "yaml-cpp/yaml.h"

#include <string>

namespace AppInfoProvision
{

	inline bool depotPublicManifestIsEmpty(const YAML::Node& depotNode)
	{
		if (!depotNode || !depotNode.IsMap()) return false;

		YAML::Node manifests = depotNode["manifests"];
		if (!manifests || !manifests.IsMap()) return false;

		YAML::Node pub = manifests["public"];
		if (!pub || !pub.IsMap()) return false;

		YAML::Node size = pub["size"];
		if (!size || !size.IsScalar()) return false;

		std::string s;
		try { s = size.as<std::string>(); }
		catch (...) { return false; }

		const auto first = s.find_first_not_of(" \t\r\n");
		if (first == std::string::npos) return false;
		const auto last = s.find_last_not_of(" \t\r\n");
		const std::string trimmed = s.substr(first, last - first + 1);

		return trimmed == "0";
	}
}

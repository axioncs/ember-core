

#pragma once

#include "yaml-cpp/yaml.h"

#include <cstdint>
#include <string>

namespace AppInfoProvision
{

inline bool hasUsableContentDepot(const YAML::Node& body)
{
	if (!body || !body.IsMap()) return false;

	const YAML::Node depots = body["depots"];
	if (!depots || !depots.IsMap()) return false;

	for (auto it = depots.begin(); it != depots.end(); ++it)
	{
		std::string key;
		try { key = it->first.as<std::string>(); }
		catch (...) { continue; }

		if (key.empty()) continue;
		bool numeric = true;
		for (char c : key)
		{
			if (c < '0' || c > '9') { numeric = false; break; }
		}
		if (!numeric) continue;

		const YAML::Node depot = it->second;
		if (!depot || !depot.IsMap()) continue;
		const YAML::Node manifests = depot["manifests"];
		if (!manifests || !manifests.IsMap()) continue;

		for (auto manifest = manifests.begin(); manifest != manifests.end();
		     ++manifest)
		{
			const YAML::Node branch = manifest->second;
			if (!branch || !branch.IsMap() || !branch["gid"]) continue;
			try
			{
				if (branch["gid"].as<uint64_t>() != 0) return true;
			}
			catch (...) {}
		}
	}

	return false;
}

}



#include "yaml-cpp/yaml.h"

#include <cstdio>
#include <string>

int main()
{
	YAML::Emitter emitter;
	emitter << YAML::BeginMap
	        << YAML::Key << "appid" << YAML::Value << 42u
	        << YAML::Key << "normalized" << YAML::Value << false
	        << YAML::EndMap;
	if (!emitter.good())
	{
		std::fprintf(stderr, "yaml emitter failed: %s\n",
		             emitter.GetLastError().c_str());
		return 1;
	}

	const YAML::Node parsed = YAML::Load(emitter.c_str());
	if (!parsed.IsMap() || parsed["appid"].as<unsigned int>() != 42u ||
	    parsed["normalized"].as<bool>())
	{
		std::fputs("yaml round-trip mismatch\n", stderr);
		return 1;
	}

	std::puts("yaml runtime ABI smoke test passed");
	return 0;
}



#include "../src/feats/usabledepot.hpp"
#include "yaml-cpp/yaml.h"

#include <cstdio>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

using AppInfoProvision::hasUsableContentDepot;

int main()
{

	{
		YAML::Node body;
		body["depots"]["branches"]["public"]["buildid"] = "123";
		CHECK(!hasUsableContentDepot(body),
		      "metadata-only depots -> not provisionable");
	}

	{
		YAML::Node body;
		body["depots"]["4556380"]["dlcappid"] = "4556380";
		CHECK(!hasUsableContentDepot(body),
		      "virtual DLC only -> not provisionable");
	}

	{
		YAML::Node body;
		body["depots"]["1229491"]["manifests"] = YAML::Node(YAML::NodeType::Map);
		CHECK(!hasUsableContentDepot(body),
		      "empty manifests -> not provisionable");
		body["depots"]["1229491"]["manifests"]["public"]["size"] = "123";
		CHECK(!hasUsableContentDepot(body),
		      "manifest without gid -> not provisionable");
	}

	{
		YAML::Node body;
		body["depots"]["1229491"]["manifests"]["public"]["gid"] = "456";
		CHECK(hasUsableContentDepot(body),
		      "content depot -> provisionable");
	}

	{
		YAML::Node body;
		body["depots"]["4229451"]["dlcappid"] = "4229450";
		body["depots"]["4229451"]["manifests"]["public"]["gid"] = "789";
		CHECK(hasUsableContentDepot(body),
		      "content DLC depot -> provisionable");
	}

	{
		YAML::Node missing;
		CHECK(!hasUsableContentDepot(missing), "missing depots -> not provisionable");
		YAML::Node scalar = YAML::Load("depots: invalid");
		CHECK(!hasUsableContentDepot(scalar), "scalar depots -> not provisionable");
	}

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}

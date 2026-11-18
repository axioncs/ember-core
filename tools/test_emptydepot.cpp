

#include "../src/feats/emptydepot.hpp"
#include "yaml-cpp/yaml.h"

#include <cstdio>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

using AppInfoProvision::depotPublicManifestIsEmpty;

int main()
{

	{
		YAML::Node d;
		d["dlcappid"] = "4394810";
		d["manifests"]["public"]["gid"]  = "6080006835337499181";
		d["manifests"]["public"]["size"] = "0";
		CHECK(depotPublicManifestIsEmpty(d), "size \"0\" -> empty (drop)");
	}

	{
		YAML::Node d;
		d["manifests"]["public"]["gid"]  = "4277352553098425670";
		d["manifests"]["public"]["size"] = "71790521";
		CHECK(!depotPublicManifestIsEmpty(d), "non-zero size -> not empty");
	}

	{
		YAML::Node d;
		d["dlcappid"] = "2463870";
		CHECK(!depotPublicManifestIsEmpty(d), "no manifests block -> not empty");
	}

	{
		YAML::Node d;
		d["manifests"]["beta"]["size"] = "0";
		CHECK(!depotPublicManifestIsEmpty(d), "no public branch -> not empty");
	}

	{
		YAML::Node d;
		d["manifests"]["public"]["gid"] = "123";
		CHECK(!depotPublicManifestIsEmpty(d), "public without size -> not empty");
	}

	{
		YAML::Node undef;
		CHECK(!depotPublicManifestIsEmpty(undef), "undefined node -> not empty");
		YAML::Node scalar = YAML::Load("just-a-string");
		CHECK(!depotPublicManifestIsEmpty(scalar), "scalar node -> not empty");
	}

	{
		YAML::Node d;
		d["manifests"]["public"]["size"] = " 0 ";
		CHECK(depotPublicManifestIsEmpty(d), "whitespace-padded \"0\" -> empty");
	}

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}

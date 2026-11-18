

#include "../src/feats/compattool.hpp"

#include <cstdio>
#include <string>

static int g_failures = 0;
static int g_checks = 0;

#define CHECK_EQ(got, want, msg)                                              \
	do {                                                                      \
		++g_checks;                                                           \
		std::string g = (got), w = (want);                                    \
		if (g != w) {                                                         \
			std::printf("FAIL: %s (got \"%s\", want \"%s\")\n",               \
			            msg, g.c_str(), w.c_str());                           \
			++g_failures;                                                     \
		}                                                                     \
	} while (0)

int main()
{

	const std::string full =
		"\"InstallConfigStore\"\n{\n\t\"Software\"\n\t{\n\t\t\"Valve\"\n"
		"\t\t{\n\t\t\t\"Steam\"\n\t\t\t{\n"
		"\t\t\t\t\"CompatToolMapping\"\n\t\t\t\t{\n"
		"\t\t\t\t\t\"3949040\"\n\t\t\t\t\t{\n"
		"\t\t\t\t\t\t\"name\"\t\t\"proton_experimental\"\n"
		"\t\t\t\t\t\t\"config\"\t\t\"\"\n"
		"\t\t\t\t\t\t\"priority\"\t\t\"250\"\n\t\t\t\t\t}\n"
		"\t\t\t\t\t\"0\"\n\t\t\t\t\t{\n"
		"\t\t\t\t\t\t\"name\"\t\t\"proton-cachyos\"\n"
		"\t\t\t\t\t\t\"config\"\t\t\"\"\n"
		"\t\t\t\t\t\t\"priority\"\t\t\"75\"\n\t\t\t\t\t}\n"
		"\t\t\t\t}\n\t\t\t}\n\t\t}\n\t}\n}\n";
	CHECK_EQ(CompatTool::parseDefaultTool(full), "proton-cachyos",
	         "reads the default tool from the \"0\" key");

	const std::string defaultFirst =
		"\"CompatToolMapping\"\n{\n"
		"\t\"0\"\n\t{\n\t\t\"name\"\t\t\"proton_9\"\n\t\t\"priority\"\t\t\"75\"\n\t}\n"
		"\t\"638510\"\n\t{\n\t\t\"name\"\t\t\"proton_experimental\"\n\t}\n}\n";
	CHECK_EQ(CompatTool::parseDefaultTool(defaultFirst), "proton_9",
	         "default-first layout");

	const std::string noDefault =
		"\"CompatToolMapping\"\n{\n"
		"\t\"284160\"\n\t{\n\t\t\"name\"\t\t\"proton_experimental\"\n\t}\n}\n";
	CHECK_EQ(CompatTool::parseDefaultTool(noDefault), "",
	         "no default key -> empty");

	CHECK_EQ(CompatTool::parseDefaultTool("\"CompatToolMapping\"\n{\n}\n"), "",
	         "empty mapping -> empty");

	CHECK_EQ(CompatTool::parseDefaultTool("\"Steam\"\n{\n}\n"), "",
	         "no mapping block -> empty");

	const std::string trickyIds =
		"\"CompatToolMapping\"\n{\n"
		"\t\"100\"\n\t{\n\t\t\"name\"\t\t\"a\"\n\t}\n"
		"\t\"284160\"\n\t{\n\t\t\"name\"\t\t\"b\"\n\t}\n"
		"\t\"3949040\"\n\t{\n\t\t\"name\"\t\t\"c\"\n\t}\n}\n";
	CHECK_EQ(CompatTool::parseDefaultTool(trickyIds), "",
	         "ids with 0 digits are not the default key");

	CHECK_EQ(CompatTool::parseDefaultTool("\"CompatToolMapping\"\n{\n\t\"0\"\n\t{"), "",
	         "unbalanced object -> empty (no crash)");

	if (g_failures == 0)
		std::printf("OK: %d checks passed\n", g_checks);
	else
		std::printf("FAILED: %d/%d checks failed\n", g_failures, g_checks);
	return g_failures == 0 ? 0 : 1;
}

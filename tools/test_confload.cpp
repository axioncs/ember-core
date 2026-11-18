

#include "confload.hpp"
#include "yaml-cpp/yaml.h"

#include <cstdio>
#include <string>

static int failures = 0;
static void check(bool cond, const char* msg) {
  if (cond) printf("OK:   %s\n", msg);
  else { printf("FAIL: %s\n", msg); failures++; }
}

int main() {
  using ConfLoad::Outcome;

  {
    const std::string bad =
        "API: false\n"
        "AdditionalApps:\n"
        "  - 4149320   # added via LuaTools\n"
        "- 381210\n"
        "- 489980\n"
        "LogLevel: 2\n";
    YAML::Node node;
    std::string repaired;
    auto outcome = ConfLoad::parseWithRepair(bad, node, repaired);
    check(outcome == Outcome::Repaired, "malformed config -> Repaired");
    check(node["AdditionalApps"].IsSequence() && node["AdditionalApps"].size() == 3,
          "malformed config -> all 3 ids parsed");
    check(node["LogLevel"].as<int>(-1) == 2, "malformed config -> rest intact");

    bool reparse = true;
    try { YAML::Load(repaired); } catch (...) { reparse = false; }
    check(reparse, "malformed config -> persisted repair re-parses");
  }

  {
    const std::string good =
        "AdditionalApps:\n"
        "  - 111\n"
        "  - 222\n"
        "LogLevel: 2\n";
    YAML::Node node;
    std::string repaired;
    auto outcome = ConfLoad::parseWithRepair(good, node, repaired);
    check(outcome == Outcome::ParsedAsIs, "clean config -> ParsedAsIs");
    check(node["AdditionalApps"].size() == 2, "clean config -> parsed");
  }

  {
    const std::string garbage = "foo: [1, 2\nbar: {\n";
    YAML::Node node;
    std::string repaired;
    auto outcome = ConfLoad::parseWithRepair(garbage, node, repaired);
    check(outcome == Outcome::Failed, "garbage config -> Failed");
    check(!node["foo"], "garbage config -> node empty/defaulted");
  }

  {
    const std::string bad =
        "AdditionalApps:\n"
        "- 111\n"
        "  - 222\n"
        "- 333\n"
        "LogLevel: 2\n";
    YAML::Node node;
    std::string repaired;
    auto outcome = ConfLoad::parseWithRepair(bad, node, repaired);
    check(outcome == Outcome::Repaired, "silent-fold config -> Repaired");
    check(node["AdditionalApps"].IsSequence() && node["AdditionalApps"].size() == 3,
          "silent-fold config -> 3 clean ids (not the folded 2)");
  }

  {
    YAML::Node node;
    std::string repaired;
    auto outcome = ConfLoad::parseWithRepair("", node, repaired);
    check(outcome == Outcome::ParsedAsIs, "empty config -> ParsedAsIs");
  }

  if (failures) { printf("\n%d check(s) failed\n", failures); return 1; }
  printf("\nall checks passed\n");
  return 0;
}

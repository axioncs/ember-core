

#include "confnormalize.hpp"
#include "yaml-cpp/yaml.h"

#include <cstdio>
#include <string>

static int failures = 0;

static void check(bool cond, const char* msg) {
  if (cond) {
    printf("OK:   %s\n", msg);
  } else {
    printf("FAIL: %s\n", msg);
    failures++;
  }
}

static bool parses(const std::string& text) {
  try {
    YAML::Load(text);
    return true;
  } catch (...) {
    return false;
  }
}

int main() {

  {
    const std::string bad =
        "API: false\n"
        "AdditionalApps:\n"
        "  - 4149320   # added via LuaTools\n"
        "- 381210\n"
        "- 489980\n"
        "AppIds: null\n"
        "LogLevel: 2\n";

    check(!parses(bad), "precondition: the malformed config does NOT parse");

    bool changed = false;
    const std::string fixed = ConfNormalize::repairSeqIndent(bad, &changed);

    check(changed, "repair: reports it changed the malformed config");
    check(parses(fixed), "repair: the result parses");

    if (parses(fixed)) {
      YAML::Node n = YAML::Load(fixed);
      auto apps = n["AdditionalApps"];
      check(apps.IsSequence() && apps.size() == 3,
            "repair: all three ids survive the repair");
      bool has4149320 = false, has381210 = false, has489980 = false;
      for (auto e : apps) {
        const auto v = e.as<long long>(-1);
        if (v == 4149320) has4149320 = true;
        if (v == 381210) has381210 = true;
        if (v == 489980) has489980 = true;
      }
      check(has4149320, "repair: the newly-added id (4149320) is preserved");
      check(has381210 && has489980, "repair: the pre-existing ids are preserved");
      check(n["LogLevel"].as<int>(-1) == 2, "repair: the rest of the doc is intact");
    }
  }

  {
    const std::string good =
        "AdditionalApps:\n"
        "- 111\n"
        "- 222\n"
        "LogLevel: 2\n";
    bool changed = true;
    const std::string out = ConfNormalize::repairSeqIndent(good, &changed);
    check(!changed, "idempotent: uniform 0-indent list reports no change");
    check(out == good, "idempotent: uniform 0-indent list is byte-identical");
  }

  {
    const std::string good =
        "AdditionalApps:\n"
        "  - 111\n"
        "  - 222\n"
        "LogLevel: 2\n";
    bool changed = true;
    const std::string out = ConfNormalize::repairSeqIndent(good, &changed);
    check(!changed, "idempotent: uniform 2-indent list reports no change");
    check(out == good, "idempotent: uniform 2-indent list is byte-identical");
  }

  {
    const std::string nested =
        "DlcData:\n"
        "  1234:\n"
        "    - name: a\n"
        "      id: 1\n"
        "    - name: b\n"
        "      id: 2\n"
        "LogLevel: 2\n";
    bool changed = true;
    const std::string out = ConfNormalize::repairSeqIndent(nested, &changed);
    check(!changed, "nested seq-of-maps: reports no change");
    check(out == nested, "nested seq-of-maps: byte-identical");
  }

  {
    const std::string denuvo =
        "DenuvoGames:\n"
        "  76561198000000000:\n"
        "    - 12345\n"
        "    - 67890\n"
        "LogLevel: 2\n";
    bool changed = true;
    const std::string out = ConfNormalize::repairSeqIndent(denuvo, &changed);
    check(!changed, "denuvo nested list: reports no change");
    check(out == denuvo, "denuvo nested list: byte-identical");
  }

  {
    const std::string bad =
        "AdditionalApps:\n"
        "- 111\n"
        "  - 222\n"
        "- 333\n"
        "LogLevel: 2\n";

    {
      auto apps = YAML::Load(bad)["AdditionalApps"];
      check(!(apps.IsSequence() && apps.size() == 3),
            "precondition: mid-list odd indent is silently mis-parsed");
    }
    bool changed = false;
    const std::string fixed = ConfNormalize::repairSeqIndent(bad, &changed);
    check(changed && parses(fixed), "repair: mid-list odd indent repaired");
    if (parses(fixed)) {
      auto apps = YAML::Load(fixed)["AdditionalApps"];
      check(apps.IsSequence() && apps.size() == 3,
            "repair: all three mid-list ids survive as clean scalars");
    }
  }

  if (failures) {
    printf("\n%d check(s) failed\n", failures);
    return 1;
  }
  printf("\nall checks passed\n");
  return 0;
}

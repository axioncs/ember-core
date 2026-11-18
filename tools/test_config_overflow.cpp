

#include "yaml-cpp/yaml.h"

#include <cstdint>
#include <cstdio>

template <typename T>
static T getSetting_fixed(YAML::Node& node, const char* name, T defVal) {
  if (!node[name]) return defVal;
  return node[name].as<T>(defVal);
}

int main() {
  int failures = 0;

  YAML::Node node = YAML::Load(
      "FakeWalletBalance: 500000000000000000\n"
      "GoodWallet: 4999\n"
  );

  bool threw = false;
  int32_t over = 0;
  try {
    over = getSetting_fixed<int32_t>(node, "FakeWalletBalance", 0);
  } catch (...) {
    threw = true;
  }
  if (threw) {
    printf("FAIL: getSetting threw on the out-of-range value.\n");
    failures++;
  } else if (over != 0) {
    printf("FAIL: out-of-range value did not fall back to the default "
           "(got %d).\n", over);
    failures++;
  } else {
    printf("OK: out-of-range FakeWalletBalance falls back to 0 without "
           "throwing.\n");
  }

  int32_t good = getSetting_fixed<int32_t>(node, "GoodWallet", 0);
  if (good != 4999) {
    printf("FAIL: in-range value not read (got %d, want 4999).\n", good);
    failures++;
  } else {
    printf("OK: in-range value read correctly (4999).\n");
  }

  int32_t missing = getSetting_fixed<int32_t>(node, "Absent", 7);
  if (missing != 7) {
    printf("FAIL: missing key did not return the default (got %d).\n", missing);
    failures++;
  } else {
    printf("OK: missing key returns the default.\n");
  }

  if (failures) {
    printf("\n%d check(s) failed\n", failures);
    return 1;
  }
  printf("\nall checks passed\n");
  return 0;
}



#include "../src/config_path.hpp"

#include <cstdio>

static int g_failures = 0;

static void check(bool condition, const char* message)
{
    if (!condition)
    {
        std::printf("FAIL: %s\n", message);
        ++g_failures;
    }
    else
    {
        std::printf("ok:   %s\n", message);
    }
}

int main()
{
    using ConfigPath::emberConfigDir;

    check(emberConfigDir(nullptr, "/home/tester") ==
              "/home/tester/.config/ember",
          "unset XDG_CONFIG_HOME uses the HOME config directory");
    check(emberConfigDir("", "/home/tester") ==
              "/home/tester/.config/ember",
          "empty XDG_CONFIG_HOME uses the HOME config directory");
    check(emberConfigDir("/tmp/xdg", "/home/tester") ==
              "/tmp/xdg/ember",
          "nonempty XDG_CONFIG_HOME is used as the config directory base");

    if (g_failures == 0) std::puts("all config-path checks passed");
    else                 std::printf("%d config-path check(s) FAILED\n", g_failures);
    return g_failures == 0 ? 0 : 1;
}

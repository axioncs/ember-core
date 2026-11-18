

#include "../src/feats/appinfo_vdf.hpp"
#include "../src/log.hpp"
#include "../src/config.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    if (argc < 3) { std::cerr << "usage: " << argv[0] << " <appinfo.vdf> <picsbuffer.bin>\n"; return 1; }

    g_pLog = std::unique_ptr<CLog>(new CLog("/tmp/test_inject.log"));
    setenv("HOME", "/tmp", 1);
    std::system("mkdir -p /tmp/.config/ember/cache");

    std::ifstream wf(argv[2], std::ios::binary | std::ios::ate);
    if (!wf) { std::cerr << "no wire buffer\n"; return 1; }
    auto sz = wf.tellg();
    std::string wire; wire.resize(sz); wf.seekg(0); wf.read(wire.data(), sz);

    const char* hexSha = "7da9fe5518638f1c121bee087f0de0dc0c5dc988";
    std::string sha; sha.assign(20, '\0');
    for (int i = 0; i < 20; ++i) {
        auto v = [](char c) { return (c >= '0' && c <= '9') ? c - '0' : 10 + (c - 'a'); };
        sha[i] = (v(hexSha[2*i]) << 4) | v(hexSha[2*i+1]);
    }

    bool ok = AppInfoVdf::injectApp(argv[1], 638510, 36310070u, sha, wire);
    std::cout << "inject ok=" << ok << "\n";
    return ok ? 0 : 2;
}

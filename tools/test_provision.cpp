

#include "yaml-cpp/yaml.h"
#include <fstream>
#include <iostream>
#include <set>
#include <string>

int main(int argc, char** argv)
{
    if (argc < 2) { std::cerr << "usage: test_provision <json>\n"; return 1; }
    std::ifstream f(argv[1]);
    std::string json((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    YAML::Node root = YAML::Load(json);
    const std::string appid = argv[2] ? argv[2] : "638510";
    YAML::Node appNode = root["data"][appid];

    YAML::Node body;
    for (auto it = appNode.begin(); it != appNode.end(); ++it) {
        std::string k = it->first.as<std::string>();
        if (!k.empty() && k.front() == '_') continue;
        body[k] = it->second;
    }

    std::set<std::string> haveKeys = {"638511"};
    YAML::Node depots = body["depots"];
    YAML::Node newDepots(YAML::NodeType::Map);
    for (auto it = depots.begin(); it != depots.end(); ++it) {
        std::string key = it->first.as<std::string>();
        bool numeric = !key.empty();
        for (char c : key) if (c < '0' || c > '9') numeric = false;
        if (!numeric) { newDepots[key] = YAML::Clone(it->second); continue; }
        if (haveKeys.count(key)) newDepots[key] = YAML::Clone(it->second);
    }
    body["depots"] = newDepots;

    YAML::Emitter em;
    em << body;
    std::cout << em.c_str() << "\n";
    return 0;
}

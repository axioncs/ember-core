

#include "../src/utils.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

static int g_failures = 0;
#define CHECK(cond, msg)                                                      \
    do {                                                                      \
        if (cond) {                                                           \
            std::printf("ok   %s\n", (msg));                                   \
        } else {                                                              \
            std::printf("FAIL %s\n", (msg));                                   \
            g_failures++;                                                      \
        }                                                                     \
    } while (0)

int main()
{

	{

		auto out = Utils::strsplit("", "|");
		CHECK(out.empty(), "empty input yields no fields");
	}
	{

		auto out = Utils::strsplit("|", "|");
		CHECK(out.empty(), "a lone delimiter yields no fields");
		out = Utils::strsplit("|||", "|");
		CHECK(out.empty(), "repeated delimiters yield no fields");
	}
	{
		auto out = Utils::strsplit(nullptr, "|");
		CHECK(out.empty(), "null input yields no fields");
	}
	{
		auto out = Utils::strsplit("a|b", nullptr);
		CHECK(out.size() == 1 && out[0] == "a|b",
		      "a null delimiter set leaves the input whole");
		out = Utils::strsplit("a|b", "");
		CHECK(out.size() == 1 && out[0] == "a|b",
		      "an empty delimiter set leaves the input whole");
	}

	{
		auto out = Utils::strsplit("install|480|0", "|");
		CHECK(out.size() == 3 && out[0] == "install" && out[1] == "480"
		          && out[2] == "0",
		      "three fields split in order");
	}
	{

		auto out = Utils::strsplit("|a||b|", "|");
		CHECK(out.size() == 2 && out[0] == "a" && out[1] == "b",
		      "empty fields are collapsed, as strtok did");
	}
	{

		auto out = Utils::strsplit("rax, qword ptr [rip + 0x10]", ",");
		CHECK(out.size() == 2 && out[0] == "rax"
		          && out[1] == " qword ptr [rip + 0x10]",
		      "only the delimiter splits; spaces are preserved");
	}
	{
		auto out = Utils::strsplit("a:b,c", ":,");
		CHECK(out.size() == 3 && out[0] == "a" && out[1] == "b" && out[2] == "c",
		      "every character of the delimiter set splits");
	}
	{
		auto out = Utils::strsplit("single", "|");
		CHECK(out.size() == 1 && out[0] == "single",
		      "input without a delimiter is one field");
	}

	{

		char buffer[] = "one:two:three";
		char original[sizeof(buffer)];
		std::memcpy(original, buffer, sizeof(buffer));
		auto out = Utils::strsplit(buffer, ":");
		CHECK(out.size() == 3, "a mutable buffer still splits");
		CHECK(std::memcmp(buffer, original, sizeof(buffer)) == 0,
		      "the caller's buffer is left byte-identical");
	}

	{

		constexpr int kThreads = 8;
		constexpr int kIterations = 2000;
		std::vector<std::thread> threads;
		std::vector<int> bad(kThreads, 0);
		for (int t = 0; t < kThreads; t++) {
			threads.emplace_back([t, &bad]() {
				const std::string input =
				    "field" + std::to_string(t) + "|480|0";
				for (int i = 0; i < kIterations; i++) {
					auto out = Utils::strsplit(input.c_str(), "|");
					if (out.size() != 3 || out[0] != "field" + std::to_string(t)
					    || out[1] != "480" || out[2] != "0") {
						bad[t]++;
					}
				}
			});
		}
		for (auto& th : threads) th.join();
		int total = 0;
		for (int b : bad) total += b;
		CHECK(total == 0, "concurrent splits do not interfere");
	}

	{
		std::string input;
		for (int i = 0; i < 500; i++) {
			if (i) input += ":";
			input += std::to_string(i);
		}
		auto out = Utils::strsplit(input.c_str(), ":");
		CHECK(out.size() == 500 && out.front() == "0" && out.back() == "499",
		      "500 fields all survive");
	}

	if (g_failures == 0) {
		std::printf("test_strsplit: ALL PASS\n");
		return 0;
	}
	std::printf("test_strsplit: %d FAILURE(S)\n", g_failures);
	return 1;
}



#include "../src/feats/retry.hpp"

#include <chrono>
#include <cstdio>
#include <thread>

int main()
{
	using clk = std::chrono::steady_clock;

	int attempts = 0;
	const auto t0 = clk::now();
	const bool ok = AppInfoProvision::retryWithBackoff(
		[&] { ++attempts; return attempts >= 3; },
		5, 50,
		[](int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); });
	const auto elapsedMs =
		std::chrono::duration_cast<std::chrono::milliseconds>(clk::now() - t0).count();

	std::printf("ok=%d attempts=%d elapsed=%lldms (expect >=150, <300)\n",
	            ok, attempts, static_cast<long long>(elapsedMs));

	const bool pass = ok && attempts == 3 && elapsedMs >= 150 && elapsedMs < 300;
	std::printf("%s\n", pass ? "PASS" : "FAIL");
	return pass ? 0 : 1;
}

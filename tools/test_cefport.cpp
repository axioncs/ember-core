

#include "../src/feats/cefport.hpp"

#include <cstdio>
#include <cstdint>
#include <fstream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>

static int g_failures = 0;
static int g_checks = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		++g_checks;                                                          \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
	} while (0)

static std::string readLine(const std::string& p)
{
	std::ifstream f(p);
	std::string s;
	std::getline(f, s);
	return s;
}

static void test_rewrite()
{
	using CefPort::rewritePortArg;

	{
		auto [o, c] = rewritePortArg("--remote-debugging-port=8080", 12345);
		CHECK(c && o == "--remote-debugging-port=12345", "exact element");
	}
	{
		auto [o, c] = rewritePortArg("--remote-debugging-port=8080", 1024);
		CHECK(c && o == "--remote-debugging-port=1024", "shorter replacement port");
	}
	{
		auto [o, c] = rewritePortArg("--remote-debugging-port=8080", 65535);
		CHECK(c && o == "--remote-debugging-port=65535", "max port");
	}

	{
		const std::string in =
			"exec '/x/steamwebhelper.sh' '-nocrashdialog' "
			"'--remote-debugging-port=8080' '--enable-smooth-scrolling'";
		auto [o, c] = rewritePortArg(in, 49777);
		CHECK(c, "sh -c: changed");
		CHECK(o.find("'--remote-debugging-port=49777'") != std::string::npos, "sh -c: digits replaced, quotes intact");
		CHECK(o.find("8080") == std::string::npos, "sh -c: no stale 8080");
		CHECK(o.find("'--enable-smooth-scrolling'") != std::string::npos, "sh -c: neighbours intact");
	}

	{
		auto [o, c] = rewritePortArg("--remote-debugging-port=8080", 2000);
		CHECK(c && o == "--remote-debugging-port=2000", "digits end the string");
	}

	{
		auto [o, c] = rewritePortArg("--remote-debugging-port=8080 --foo", 2000);
		CHECK(c && o == "--remote-debugging-port=2000 --foo", "digits followed by space");
	}

	{
		auto [o, c] = rewritePortArg("x='--remote-debugging-port=8080'", 2000);
		CHECK(c && o == "x='--remote-debugging-port=2000'", "digits followed by quote");
	}

	{
		auto [o, c] = rewritePortArg(
			"--remote-debugging-port=8080 then --remote-debugging-port=8080", 4444);
		CHECK(c, "multi: changed");
		CHECK(o == "--remote-debugging-port=4444 then --remote-debugging-port=4444", "multi: all replaced");
	}

	{
		auto [o, c] = rewritePortArg("-cachedir=/home/u/.cache", 12345);
		CHECK(!c && o == "-cachedir=/home/u/.cache", "no switch -> verbatim");
	}

	{
		auto [o, c] = rewritePortArg("", 12345);
		CHECK(!c && o.empty(), "empty string -> unchanged");
	}

	{
		auto [o, c] = rewritePortArg("--remote-debugging-port=9090", 22222);
		CHECK(c && o == "--remote-debugging-port=22222", "non-8080 default matched");
	}

	{
		auto [o, c] = rewritePortArg("--remote-debugging-port=49777", 49777);
		CHECK(o == "--remote-debugging-port=49777", "idempotent same-port");
	}

	{
		auto [o, c] = rewritePortArg("--remote-debugging-address=127.0.0.1", 12345);
		CHECK(!c && o == "--remote-debugging-address=127.0.0.1", "address not matched");
	}
	{
		auto [o, c] = rewritePortArg("--remote-debugging-pipe", 12345);
		CHECK(!c, "pipe not matched");
	}

	{
		auto [o, c] = rewritePortArg("--remote-debugging-port=", 12345);
		CHECK(!c && o == "--remote-debugging-port=", "prefix with no digits untouched");
	}
	{
		auto [o, c] = rewritePortArg("--remote-debugging-port=abc", 12345);
		CHECK(!c && o == "--remote-debugging-port=abc", "prefix with non-digit untouched");
	}
}

static void test_ports()
{

	{
		const uint16_t p = CefPort::pickFreePort();
		CHECK(p >= 1024, "pickFreePort in unprivileged range");
		CHECK(CefPort::isBindable(p), "pickFreePort is bindable");
		CHECK(!CefPort::isBindable(0), "port 0 is not bindable");
	}

	{
		int s = ::socket(AF_INET, SOCK_STREAM, 0);
		sockaddr_in a{}; a.sin_family = AF_INET; a.sin_addr.s_addr = htonl(INADDR_LOOPBACK); a.sin_port = 0;
		::bind(s, (sockaddr*)&a, sizeof(a)); ::listen(s, 1);
		socklen_t al = sizeof(a); ::getsockname(s, (sockaddr*)&a, &al);
		const uint16_t held = ntohs(a.sin_port);
		CHECK(!CefPort::isBindable(held), "occupied port not bindable");
		::close(s);
		CHECK(CefPort::isBindable(held), "released port bindable again");
	}

	const std::string tag = std::to_string(getpid());

	{
		const std::string path = "/tmp/cefport_missing_" + tag;
		::unlink(path.c_str());
		const uint16_t p = CefPort::resolveSessionPort(path);
		CHECK(p >= 1024 && CefPort::isBindable(p), "missing-file -> fresh bindable");
		CHECK(readLine(path) == std::to_string(p), "missing-file -> persisted");
		::unlink(path.c_str());
	}

	{
		const std::string path = "/tmp/cefport_reuse_" + tag;
		const uint16_t free = CefPort::pickFreePort();
		{ std::ofstream(path) << free << "\n"; }
		CHECK(CefPort::resolveSessionPort(path) == free, "valid free port reused");
		::unlink(path.c_str());
	}

	for (const char* bad : { "not-a-port", "70000", "1023", "-5", "" })
	{
		const std::string path = "/tmp/cefport_bad_" + tag;
		{ std::ofstream(path) << bad << "\n"; }
		const uint16_t p = CefPort::resolveSessionPort(path);
		CHECK(p >= 1024 && CefPort::isBindable(p), (std::string("garbage '") + bad + "' -> fresh").c_str());
		::unlink(path.c_str());
	}

	{
		const std::string path = "/tmp/cefport_busy_" + tag;
		int s = ::socket(AF_INET, SOCK_STREAM, 0);
		sockaddr_in a{}; a.sin_family = AF_INET; a.sin_addr.s_addr = htonl(INADDR_LOOPBACK); a.sin_port = 0;
		::bind(s, (sockaddr*)&a, sizeof(a)); ::listen(s, 1);
		socklen_t al = sizeof(a); ::getsockname(s, (sockaddr*)&a, &al);
		const uint16_t busy = ntohs(a.sin_port);
		{ std::ofstream(path) << busy << "\n"; }
		const uint16_t p = CefPort::resolveSessionPort(path);
		CHECK(p != busy && p >= 1024 && CefPort::isBindable(p), "occupied port rotates");
		::close(s);
		::unlink(path.c_str());
	}

	{
		const std::string dir = "/tmp/cefport_dir_" + tag + "/a/b";
		const std::string path = dir + "/cef_port";
		std::error_code ec; std::filesystem::remove_all("/tmp/cefport_dir_" + tag, ec);
		const uint16_t p = CefPort::resolveSessionPort(path);
		CHECK(p >= 1024, "nested-dir resolve yields a port");
		CHECK(readLine(path) == std::to_string(p), "nested-dir resolve persisted (dir created)");
		std::filesystem::remove_all("/tmp/cefport_dir_" + tag, ec);
	}

	{
		const uint16_t p = CefPort::resolveSessionPort("");
		CHECK(p >= 1024 && CefPort::isBindable(p), "empty path -> port without persistence");
	}

	{
		const std::string path = "/tmp/cefport_nopersist_" + tag;
		::unlink(path.c_str());
		const uint16_t p = CefPort::resolveSessionPortNoPersist(path);
		CHECK(p >= 1024 && CefPort::isBindable(p), "no-persist -> fresh bindable port");
		CHECK(!std::ifstream(path).good(), "no-persist -> contract file NOT written");
		::unlink(path.c_str());
	}

	{
		const std::string path = "/tmp/cefport_nopersist_reuse_" + tag;
		const uint16_t free = CefPort::pickFreePort();
		{ std::ofstream(path) << free << "\n"; }
		CHECK(CefPort::resolveSessionPortNoPersist(path) == free, "no-persist reuses valid free port");
		CHECK(readLine(path) == std::to_string(free), "no-persist leaves file unchanged");
		::unlink(path.c_str());
	}

	{
		const std::string path = "/tmp/cefport_nopersist_busy_" + tag;
		int s = ::socket(AF_INET, SOCK_STREAM, 0);
		sockaddr_in a{}; a.sin_family = AF_INET; a.sin_addr.s_addr = htonl(INADDR_LOOPBACK); a.sin_port = 0;
		::bind(s, reinterpret_cast<sockaddr*>(&a), sizeof(a));
		socklen_t len = sizeof(a); ::getsockname(s, reinterpret_cast<sockaddr*>(&a), &len);
		::listen(s, 1);
		const uint16_t busy = ntohs(a.sin_port);
		{ std::ofstream(path) << busy << "\n"; }
		const uint16_t p = CefPort::resolveSessionPortNoPersist(path);
		CHECK(p != busy && p >= 1024 && CefPort::isBindable(p), "no-persist rotates off occupied port");
		CHECK(readLine(path) == std::to_string(busy), "no-persist leaves occupied file unchanged");
		::close(s);
		::unlink(path.c_str());
	}
}

static void test_decky()
{
	const std::string tag = std::to_string(getpid());
	const std::string home = "/tmp/cefport_decky_" + tag;
	std::error_code ec;
	std::filesystem::remove_all(home, ec);

	std::filesystem::create_directories(home, ec);
	CHECK(!CefPort::deckyPresent(home), "decky: absent when no homebrew tree");

	std::filesystem::create_directories(home + "/homebrew/services", ec);
	CHECK(!CefPort::deckyPresent(home), "decky: absent when services dir empty");

	{ std::ofstream(home + "/homebrew/services/PluginLoader") << "x"; }
	CHECK(CefPort::deckyPresent(home), "decky: present when PluginLoader exists");

	CHECK(!CefPort::deckyPresent(std::string("")), "decky: empty home -> false");

	std::filesystem::remove_all(home, ec);

	{
		const std::string path = "/tmp/cefport_contract_" + tag;
		{ std::ofstream(path) << "12345\n"; }
		CHECK(std::ifstream(path).good(), "removeContract: file exists pre-remove");
		CefPort::removeContract(path);
		CHECK(!std::ifstream(path).good(), "removeContract: file gone post-remove");
		CefPort::removeContract(path);
		CefPort::removeContract("");
		CHECK(true, "removeContract: idempotent + empty-path safe");
	}
}

static void test_contract_owner()
{
	using CefPort::formatContract;
	using CefPort::parseStatStartTicks;

	CHECK(formatContract(49777, 4242, 987654) == "49777\nowner 4242 987654\n",
	      "contract: port line then owner line");
	CHECK(formatContract(8080, 0, 0) == "8080\n",
	      "contract: no owner -> port only (legacy shape)");
	CHECK(formatContract(8080, 4242, 0) == "8080\n",
	      "contract: unusable start time -> no owner line");
	CHECK(formatContract(8080, 0, 987654) == "8080\n",
	      "contract: unusable pid -> no owner line");

	{
		const std::string path = "/tmp/cefport_owner_" + std::to_string(getpid());
		CHECK(CefPort::writePortFile(path, 51515, 4242, 987654), "contract: written");
		CHECK(readLine(path) == "51515", "contract: first line is the port");
		CHECK(CefPort::readPortFile(path) == 51515, "contract: readPortFile still works");
		::unlink(path.c_str());
	}

	CHECK(parseStatStartTicks(
	          "1234 (steam) S 1 1234 1234 0 -1 4194560 100 0 0 0 1 2 0 0 20 0 3 0 "
	          "555666 100 200 300") == 555666,
	      "stat: plain comm");
	CHECK(parseStatStartTicks(
	          "1234 (we ird) na:me) S 1 1234 1234 0 -1 4194560 100 0 0 0 1 2 0 0 20 0 3 0 "
	          "777888 100 200 300") == 777888,
	      "stat: comm with spaces and parens");
	CHECK(parseStatStartTicks("") == 0, "stat: empty line -> 0");
	CHECK(parseStatStartTicks("1234 (steam) S 1 2 3") == 0,
	      "stat: truncated line -> 0");
	CHECK(parseStatStartTicks("no parens here") == 0, "stat: malformed -> 0");

	CHECK(CefPort::readProcStartTicks(getpid()) > 0, "stat: own start time readable");
	CHECK(CefPort::readProcStartTicks(0) == 0, "stat: pid 0 -> 0");
	CHECK(CefPort::readProcStartTicks(-1) == 0, "stat: negative pid -> 0");
}

int main()
{
	test_rewrite();
	test_ports();
	test_decky();
	test_contract_owner();

	if (g_failures == 0) { std::printf("test_cefport: ALL PASS (%d checks)\n", g_checks); return 0; }
	std::printf("test_cefport: %d/%d CHECK(S) FAILED\n", g_failures, g_checks);
	return 1;
}

#include "../src/feats/cefport.hpp"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

static std::filesystem::path g_sandbox;

static std::string markerPath()
{
	return (g_sandbox / ".local/share/Steam/.cef-enable-remote-debugging").string();
}

static void removeMarker()
{
	std::error_code ec;
	std::filesystem::remove(markerPath(), ec);
}

static void runCase(const char* label, bool preCreateParent, bool preCreateMarker)
{
	g_sandbox = std::filesystem::temp_directory_path() /
		("ember-cef-marker-" + std::to_string(std::rand() % 1000000));
	std::error_code ec;
	std::filesystem::remove_all(g_sandbox, ec);
	std::filesystem::create_directories(g_sandbox, ec);
	setenv("HOME", g_sandbox.c_str(), 1);

	if (preCreateParent)
	{
		std::filesystem::create_directories(
			g_sandbox / ".local/share/Steam", ec);
	}
	if (preCreateMarker)
	{
		std::ofstream f(markerPath(), std::ios::trunc);
		f << "";
	}

	const bool before = CefPort::remoteDebuggingMarkerPresent();

	const bool created = CefPort::enableRemoteDebuggingMarker();
	const bool after = CefPort::remoteDebuggingMarkerPresent();

	std::printf("-- %s\n", label);
	CHECK(before == preCreateMarker, "present() agrees with the fixture");
	CHECK(created, "enableRemoteDebuggingMarker reports success");
	CHECK(after, "the marker file exists afterwards");
	CHECK(CefPort::remoteDebuggingMarkerPath().find("/.local/share/Steam/") != std::string::npos,
	      "the marker lands in the Steam root");

	const bool again = CefPort::enableRemoteDebuggingMarker();
	CHECK(again, "a second call is idempotent, not an error");

	if (auto* raw = std::getenv("HOME"))
	{
		(void)raw;
	}
	std::filesystem::remove_all(g_sandbox, ec);
}

int main()
{
	std::srand(20261119);

	// The failing case on this machine: Steam root exists, marker absent.
	runCase("marker absent, Steam root present", true, false);
	// Neither exists: the helper has to create both.
	runCase("marker absent, Steam root absent", false, false);
	// Already enabled: must not fail or duplicate.
	runCase("marker already present", true, true);

	// HOME unset is a hard error, not a crash.
	unsetenv("HOME");
	CHECK(CefPort::remoteDebuggingMarkerPath().empty(), "no HOME -> empty marker path");
	CHECK(!CefPort::enableRemoteDebuggingMarker(), "no HOME -> enable reports failure");
	CHECK(!CefPort::remoteDebuggingMarkerPresent(), "no HOME -> present is false");

	removeMarker();

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}
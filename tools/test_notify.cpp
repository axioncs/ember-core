

#include "../src/notify.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

static bool contains(const std::string& hay, const std::string& needle)
{
	return hay.find(needle) != std::string::npos;
}

int main()
{

	{
		const Notify::Spec s = Notify::specForLevel(LogLevel::Warn);
		CHECK(s.enabled, "Warn produces a desktop notification");
		CHECK(s.timeoutMs > 0, "Warn has an explicit expire timeout");
		CHECK(std::string(s.urgency) != "critical",
		      "Warn urgency is not critical (so it auto-dismisses)");
	}

	{
		const Notify::Spec sShort = Notify::specForLevel(LogLevel::NotifyShort);
		CHECK(sShort.enabled && sShort.timeoutMs == 10000 &&
		      std::string(sShort.urgency) == "normal",
		      "NotifyShort = {enabled, 10000ms, normal}");

		const Notify::Spec sLong = Notify::specForLevel(LogLevel::NotifyLong);
		CHECK(sLong.enabled && sLong.timeoutMs == 30000 &&
		      std::string(sLong.urgency) == "normal",
		      "NotifyLong = {enabled, 30000ms, normal}");
	}

	{
		CHECK(!Notify::specForLevel(LogLevel::Once).enabled,  "Once does not notify");
		CHECK(!Notify::specForLevel(LogLevel::Debug).enabled, "Debug does not notify");
		CHECK(!Notify::specForLevel(LogLevel::Info).enabled,  "Info does not notify");
		CHECK(!Notify::specForLevel(LogLevel::None).enabled,  "None does not notify");
	}

	{
		const std::string warnCmd = Notify::buildCommand(LogLevel::Warn, "boom");
		CHECK(contains(warnCmd, "notify-send"), "Warn cmd invokes notify-send");
		CHECK(contains(warnCmd, "-t "), "Warn cmd carries an explicit -t timeout");
		CHECK(!contains(warnCmd, "critical"), "Warn cmd is not critical urgency");
		CHECK(contains(warnCmd, "boom"), "Warn cmd carries the message body");

		const std::string shortCmd = Notify::buildCommand(LogLevel::NotifyShort, "hi");
		CHECK(contains(shortCmd, "-t 10000"), "NotifyShort cmd uses 10000ms");
	}

	{
		CHECK(Notify::buildCommand(LogLevel::Info, "x").empty(),
		      "Info builds no command");
		CHECK(Notify::buildCommand(LogLevel::Debug, "x").empty(),
		      "Debug builds no command");
	}

	{
		const std::string cmd = Notify::buildCommand(LogLevel::Warn, "a\"b");
		CHECK(contains(cmd, "a\\\"b"), "double-quote in body is backslash-escaped");

		const std::string inj = Notify::buildCommand(LogLevel::Warn, "x`id`$(id)");
		CHECK(contains(inj, "\\`id\\`"), "backticks in body are escaped");
		CHECK(contains(inj, "\\$(id)"), "dollar in body is escaped");
	}

	{
		std::string withNull = "Loaded successfully";
		withNull.push_back('\0');
		const std::string cmd = Notify::buildCommand(LogLevel::NotifyShort, withNull);

		const std::string asExecuted(cmd.c_str());
		CHECK(asExecuted == cmd,
		      "command carries no embedded null (would truncate in system())");
		CHECK(!asExecuted.empty() && asExecuted.back() == '"',
		      "command ends with the closing quote (not truncated mid-word)");
	}

	{
		using Notify::shouldRaiseNotification;

		CHECK(shouldRaiseNotification(LogLevel::Warn, true, false),
		      "Warn pops up when enabled and not suppressed");

		CHECK(!shouldRaiseNotification(LogLevel::Warn, true, true),
		      "Warn does NOT pop up on a suppressed thread");
		CHECK(!shouldRaiseNotification(LogLevel::NotifyShort, true, true),
		      "NotifyShort does NOT pop up on a suppressed thread");

		CHECK(!shouldRaiseNotification(LogLevel::Warn, false, false),
		      "Warn does NOT pop up when notifications are disabled");

		CHECK(!shouldRaiseNotification(LogLevel::Info, true, false),
		      "Info never pops up");
		CHECK(!shouldRaiseNotification(LogLevel::Debug, true, false),
		      "Debug never pops up");
	}

	{
		const std::string cmd =
			Notify::buildCommandRaw("ember-moon", "servers busy", 30000, "normal");
		CHECK(contains(cmd, "notify-send"),  "raw cmd invokes notify-send");
		CHECK(contains(cmd, "-t 30000"),     "raw cmd carries the given timeout");
		CHECK(contains(cmd, "-u \"normal\""),"raw cmd carries the given urgency");
		CHECK(contains(cmd, "ember-moon"),"raw cmd carries the title");
		CHECK(contains(cmd, "servers busy"), "raw cmd carries the body");

		const std::string inj =
			Notify::buildCommandRaw("t", "x`id`$(id)\"q", 1000, "normal");
		CHECK(contains(inj, "\\`id\\`") && contains(inj, "\\$(id)") &&
		      contains(inj, "\\\"q"),
		      "raw cmd escapes shell metacharacters in the body");
	}

	{
		Notify::NotifyThrottle thr;
		thr.cooldownMs = 60000;
		int suppressed = -1;

		CHECK(thr.allow(1, 0, suppressed) && suppressed == 0,
		      "first occurrence emits (suppressed=0)");

		CHECK(!thr.allow(1, 1000, suppressed),  "burst #1 suppressed");
		CHECK(!thr.allow(1, 2000, suppressed),  "burst #2 suppressed");
		CHECK(!thr.allow(1, 3000, suppressed),  "burst #3 suppressed");

		CHECK(thr.allow(2, 3000, suppressed) && suppressed == 0,
		      "distinct category emits independently");

		CHECK(thr.allow(1, 61000, suppressed) && suppressed == 3,
		      "post-cooldown emit reports the 3 suppressed occurrences");

		CHECK(!thr.allow(1, 61500, suppressed), "next burst suppressed again");
		CHECK(thr.allow(1, 122000, suppressed) && suppressed == 1,
		      "counter reset: only 1 suppressed since last emit");
	}

	{
		const std::string payload = Notify::buildGamepadPayload(
			"ember-moon", "quoted \"body\"\nsecond line", 30000, 123456789);
		CHECK(contains(payload, "\"version\":1"),
		      "gamepad payload carries its schema version");
		CHECK(contains(payload, "\"created_ms\":123456789"),
		      "gamepad payload carries its creation time");
		CHECK(contains(payload, "quoted \\\"body\\\"\\nsecond line"),
		      "gamepad payload JSON-escapes quotes and newlines");
		CHECK(contains(payload, "\"timeout_ms\":30000"),
		      "gamepad payload carries the requested timeout");

		const auto base = std::filesystem::temp_directory_path() /
			("sls-notify-test-" + std::to_string(Notify::processId()));
		std::error_code ec;
		std::filesystem::remove_all(base, ec);
		CHECK(Notify::enqueueGamepadEvent(base.string(), "ember-moon",
		      "ready", 10000, 123456790),
		      "gamepad event is queued atomically");

		int jsonFiles = 0;
		int temporaryFiles = 0;
		std::string queued;
		for (const auto& entry : std::filesystem::directory_iterator(base))
		{
			if (entry.path().extension() == ".json")
			{
				++jsonFiles;
				std::ifstream in(entry.path());
				queued.assign(std::istreambuf_iterator<char>(in), {});
			}
			if (entry.path().extension() == ".tmp") ++temporaryFiles;
		}
		CHECK(jsonFiles == 1, "queue publishes exactly one JSON event");
		CHECK(temporaryFiles == 0, "queue leaves no partially-written event");
		CHECK(contains(queued, "\"body\":\"ready\""),
		      "published event preserves its body");
		std::filesystem::remove_all(base, ec);
	}

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}

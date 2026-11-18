#include "log.hpp"

#include "config.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <dlfcn.h>
#include <memory>

namespace
{
	bool sendDbusNotification(const char* appName, const char* summary,
	                          const char* body, int32_t timeoutMs)
	{
		void* lib = dlopen("libdbus-1.so.3", RTLD_LAZY | RTLD_NOLOAD);
		if (!lib)
		{
			lib = dlopen("libdbus-1.so.3", RTLD_LAZY);
		}
		if (!lib)
		{
			return false;
		}

		using BusGetFn = void*(*)(int, void*);
		using MsgNewFn = void*(*)(const char*, const char*, const char*, const char*);
		using IterInitFn = void(*)(void*, void*);
		using IterAppendFn = int(*)(void*, int, const void*);
		using IterOpenFn = int(*)(void*, int, const char*, void*);
		using IterCloseFn = int(*)(void*, void*);
		using ConnSendFn = int(*)(void*, void*, uint32_t*);
		using ConnFlushFn = void(*)(void*);
		using MsgUnrefFn = void(*)(void*);

		auto busGet = reinterpret_cast<BusGetFn>(dlsym(lib, "dbus_bus_get"));
		auto msgNew = reinterpret_cast<MsgNewFn>(dlsym(lib, "dbus_message_new_method_call"));
		auto iterInit = reinterpret_cast<IterInitFn>(dlsym(lib, "dbus_message_iter_init_append"));
		auto iterAppend = reinterpret_cast<IterAppendFn>(dlsym(lib, "dbus_message_iter_append_basic"));
		auto iterOpen = reinterpret_cast<IterOpenFn>(dlsym(lib, "dbus_message_iter_open_container"));
		auto iterClose = reinterpret_cast<IterCloseFn>(dlsym(lib, "dbus_message_iter_close_container"));
		auto connSend = reinterpret_cast<ConnSendFn>(dlsym(lib, "dbus_connection_send"));
		auto connFlush = reinterpret_cast<ConnFlushFn>(dlsym(lib, "dbus_connection_flush"));
		auto msgUnref = reinterpret_cast<MsgUnrefFn>(dlsym(lib, "dbus_message_unref"));

		if (!busGet || !msgNew || !iterInit || !iterAppend || !iterOpen ||
		    !iterClose || !connSend || !connFlush || !msgUnref)
		{
			return false;
		}

		void* conn = busGet(0 , nullptr);
		if (!conn)
		{
			return false;
		}

		void* msg = msgNew("org.freedesktop.Notifications",
		                   "/org/freedesktop/Notifications",
		                   "org.freedesktop.Notifications",
		                   "Notify");
		if (!msg)
		{
			return false;
		}

		alignas(8) unsigned char iter[128]{};
		alignas(8) unsigned char sub[128]{};
		iterInit(msg, iter);

		const uint32_t replacesId = 0;
		const char* icon = "";
		bool ok = iterAppend(iter, 's', &appName)
			&& iterAppend(iter, 'u', &replacesId)
			&& iterAppend(iter, 's', &icon)
			&& iterAppend(iter, 's', &summary)
			&& iterAppend(iter, 's', &body)
			&& iterOpen(iter, 'a', "s", sub)
			&& iterClose(iter, sub)
			&& iterOpen(iter, 'a', "{sv}", sub)
			&& iterClose(iter, sub)
			&& iterAppend(iter, 'i', &timeoutMs);

		if (ok && connSend(conn, msg, nullptr))
		{
			connFlush(conn);
			msgUnref(msg);
			return true;
		}

		msgUnref(msg);
		return false;
	}
}

using std::chrono::duration_cast;

CLog::CLog(const char* path) : path(path)
{

	ofstream = std::ofstream(path, std::ios::out | std::ios::app);
	if (!ofstream.is_open())
	{
		throw std::runtime_error("Unable to open logfile!");
	}

	const char* loc = getenv("LC_ALL");
	if (!loc || !loc[0]) loc = getenv("LC_MESSAGES");
	if (!loc || !loc[0]) loc = getenv("LANG");
	if (!loc || !loc[0]) loc = getenv("LANGUAGE");
	m_lang = detectLang(loc);
}

CLog::~CLog()
{
	if (ofstream.is_open())
	{
		ofstream.close();
	}
}

LogLevel CLog::getMinLevel()
{
	return static_cast<LogLevel>(g_config.logLevel.get());
}

bool CLog::shouldNotify()
{
	return g_config.notifications.get();
}

void CLog::notifyUser(UserMsg msg, const std::string& detail)
{
	const UiMessage ui = messageFor(msg, m_lang);
	const std::string body = substituteDetail(ui.body, detail);

	info("notifyUser: %s\n", body.c_str());

	if (!shouldNotify() || t_suppressNotify)
	{
		return;
	}

	int timeoutMs = 30000;
	if (ui.severity == Severity::Info)
	{
		timeoutMs = 10000;
	}

	int suppressed = 0;
	bool emit = false;
	{
		const long long nowMs = duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count();
		std::lock_guard<std::mutex> lk(m_notifyMu);
		emit = m_notifyThrottle.allow(static_cast<int>(msg), nowMs, suppressed);
	}

	if (!emit)
	{
		return;
	}
	if (suppressed > 0)
	{
		info("notifyUser: (%d earlier occurrence(s) of this message were "
		     "suppressed)\n", suppressed);
	}

	std::string shown = body;
	const std::string suffix = suppressedSuffix(m_lang, suppressed);
	if (!suffix.empty())
	{
		shown += " " + suffix;
	}

	const std::string cmd =
		Notify::buildCommandRaw("ember", shown, timeoutMs, "normal");

	(void)Notify::enqueueGamepadEventForUser("ember", shown, timeoutMs);
	const int rc = system((cmd + " >/dev/null 2>&1").c_str());
	if (rc != 0)
	{

		(void)sendDbusNotification("ember", "ember",
		                           shown.c_str(), timeoutMs);
	}
}

CLog* CLog::createDefaultLog()
{
	const char* home = getenv("HOME");
	if (home)
	{
		std::stringstream ss;
		ss << home << "/.ember.log";

		return new CLog(ss.str().c_str());
	}

	return nullptr;
}

std::unique_ptr<CLog> g_pLog;

#pragma once

#include "notify.hpp"
#include "usermsg.hpp"

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <mutex>
#include <openssl/sha.h>
#include <shared_mutex>
#include <sstream>
#include <unordered_set>

inline thread_local bool t_suppressNotify = false;

struct ScopedNotifySuppression
{
	bool m_prev;
	ScopedNotifySuppression() : m_prev(t_suppressNotify) { t_suppressNotify = true; }
	~ScopedNotifySuppression() { t_suppressNotify = m_prev; }
};

class CLog
{
	std::ofstream ofstream;
	std::unordered_set<std::string> msgHist {};
	std::shared_mutex mutex;

	Lang m_lang = Lang::English;
	Notify::NotifyThrottle m_notifyThrottle {};
	std::mutex m_notifyMu;

	constexpr const char* logLvlToStr(LogLevel& lvl)
	{
		switch(lvl)
		{
			case LogLevel::Once:
				return "Once";
			case LogLevel::Debug:
				return "Debug";
			case LogLevel::Info:
				return "Info";
			case LogLevel::NotifyShort:
			case LogLevel::NotifyLong:
				return "Notify";
			case LogLevel::Warn:
				return "Warn";

			default:
				return "Unknown";
		}
	}

	template<typename ...Args>
	__attribute__((hot))
	void __log(LogLevel lvl, const char* msg, Args... args)
	{
		if (lvl < getMinLevel())
		{
			return;
		}

		size_t size = snprintf(nullptr, 0, msg, args...) + 1;
		std::string formatted;
		formatted.resize(size);
		snprintf(formatted.data(), size, msg, args...);

		const auto lock = std::unique_lock(mutex);

		if (lvl == LogLevel::Once)
		{
			for(const auto& oldMsg : msgHist)
			{
				if (oldMsg == formatted)
				{
					return;
				}
			}

			msgHist.emplace(formatted);
		}

		ofstream << "[" << logLvlToStr(lvl) << "] " << formatted.c_str();
		if (lvl == LogLevel::NotifyShort || lvl == LogLevel::NotifyLong)
		{
			ofstream << "\n";
		}

		ofstream.flush();
	}

	std::unordered_set<std::string> dedupHist {};

	bool seenBefore(LogLevel lvl, const std::string& formatted)
	{
		std::unique_lock lock(mutex);
		const std::string key =
			std::string(logLvlToStr(lvl)) + "|" + formatted;
		auto [_, inserted] = dedupHist.emplace(key);
		return !inserted;
	}

public:
	std::string path;

	CLog(const char* path);
	~CLog();

	template<typename ...Args>
	constexpr void once(const char* msg, Args... args)
	{
		__log(LogLevel::Once, msg, args...);
	}

	template<typename ...Args>
	constexpr void debug(const char* msg, Args... args)
	{
		__log(LogLevel::Debug, msg, args...);
	}

	template<typename ...Args>
	constexpr void info(const char* msg, Args... args)
	{
		__log(LogLevel::Info, msg, args...);
	}

	template<typename ...Args>
	void infoOnce(const char* msg, Args... args)
	{
		if (LogLevel::Info < getMinLevel()) return;
		size_t size = snprintf(nullptr, 0, msg, args...) + 1;
		std::string formatted;
		formatted.resize(size);
		snprintf(formatted.data(), size, msg, args...);
		if (seenBefore(LogLevel::Info, formatted)) return;
		__log(LogLevel::Info, "%s", formatted.c_str());
	}

	template<typename ...Args>
	void debugOnce(const char* msg, Args... args)
	{
		if (LogLevel::Debug < getMinLevel()) return;
		size_t size = snprintf(nullptr, 0, msg, args...) + 1;
		std::string formatted;
		formatted.resize(size);
		snprintf(formatted.data(), size, msg, args...);
		if (seenBefore(LogLevel::Debug, formatted)) return;
		__log(LogLevel::Debug, "%s", formatted.c_str());
	}

	template<typename ...Args>
	constexpr void notify(const char* msg, Args... args)
	{
		__log(LogLevel::NotifyShort, msg, args...);
	}

	template<typename ...Args>
	constexpr void notifyLong(const char* msg, Args... args)
	{
		__log(LogLevel::NotifyLong, msg, args...);
	}

	template<typename ...Args>
	constexpr void warn(const char* msg, Args... args)
	{
		__log(LogLevel::Warn, msg, args...);
	}

	void notifyUser(UserMsg msg, const std::string& detail = "");

	static LogLevel getMinLevel();
	static bool shouldNotify();
	static CLog* createDefaultLog();
};

extern std::unique_ptr<CLog> g_pLog;

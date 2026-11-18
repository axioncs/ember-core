#pragma once

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unistd.h>

enum class LogLevel : unsigned int
{

	Once,
	Debug,
	Info,
	NotifyShort,
	NotifyLong,
	Warn,
	None
};

namespace Notify
{
	inline long long epochMilliseconds()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();
	}

	inline long long processId()
	{
		return static_cast<long long>(::getpid());
	}

	inline std::string jsonEscape(const std::string& value)
	{
		static constexpr char hex[] = "0123456789abcdef";
		std::string out;
		out.reserve(value.size() + 8);
		for (unsigned char c : value)
		{
			switch (c)
			{
				case '\0': break;
				case '"': out += "\\\""; break;
				case '\\': out += "\\\\"; break;
				case '\b': out += "\\b"; break;
				case '\f': out += "\\f"; break;
				case '\n': out += "\\n"; break;
				case '\r': out += "\\r"; break;
				case '\t': out += "\\t"; break;
				default:
					if (c < 0x20)
					{
						out += "\\u00";
						out.push_back(hex[(c >> 4) & 0x0f]);
						out.push_back(hex[c & 0x0f]);
					}
					else
					{
						out.push_back(static_cast<char>(c));
					}
			}
		}
		return out;
	}

	inline std::string buildGamepadPayload(const std::string& title,
	                                      const std::string& body,
	                                      int timeoutMs,
	                                      long long createdMs)
	{
		return "{\"version\":1,\"created_ms\":" + std::to_string(createdMs)
			+ ",\"title\":\"" + jsonEscape(title)
			+ "\",\"body\":\"" + jsonEscape(body)
			+ "\",\"timeout_ms\":" + std::to_string(timeoutMs) + "}";
	}

	inline bool enqueueGamepadEvent(const std::string& queueDir,
	                                const std::string& title,
	                                const std::string& body,
	                                int timeoutMs,
	                                long long createdMs = epochMilliseconds())
	{
		static std::atomic<unsigned long long> sequence{0};
		try
		{
			const std::filesystem::path dir(queueDir);
			std::error_code ec;
			std::filesystem::create_directories(dir, ec);
			if (ec) return false;
			std::filesystem::permissions(dir,
				std::filesystem::perms::owner_all,
				std::filesystem::perm_options::replace, ec);
			if (ec) return false;

			const std::string stem = "event-" + std::to_string(processId()) + "-"
				+ std::to_string(createdMs) + "-"
				+ std::to_string(sequence.fetch_add(1, std::memory_order_relaxed));
			const std::filesystem::path temporary = dir / ("." + stem + ".tmp");
			const std::filesystem::path published = dir / (stem + ".json");

			{
				std::ofstream out(temporary, std::ios::binary | std::ios::trunc);
				if (!out) return false;
				out << buildGamepadPayload(title, body, timeoutMs, createdMs);
				out.flush();
				if (!out) return false;
			}
			std::filesystem::rename(temporary, published, ec);
			if (ec)
			{
				std::filesystem::remove(temporary);
				return false;
			}
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	inline bool enqueueGamepadEventForUser(const std::string& title,
	                                       const std::string& body,
	                                       int timeoutMs)
	{
		const char* home = ::getenv("HOME");
		if (!home || !home[0]) return false;
		return enqueueGamepadEvent(std::string(home)
			+ "/.config/ember/notifications", title, body, timeoutMs);
	}

	struct Spec
	{
		bool        enabled;
		int         timeoutMs;
		const char* urgency;
	};

	inline Spec specForLevel(LogLevel lvl)
	{
		switch (lvl)
		{
			case LogLevel::NotifyShort: return { true, 10000, "normal" };
			case LogLevel::NotifyLong:  return { true, 30000, "normal" };

			case LogLevel::Warn:        return { true, 30000, "normal" };
			default:                    return { false, 0, "normal" };
		}
	}

	inline bool shouldRaiseNotification(LogLevel lvl, bool notificationsEnabled,
	                                    bool threadSuppressed)
	{
		if (!notificationsEnabled) return false;
		if (threadSuppressed) return false;
		return specForLevel(lvl).enabled;
	}

	inline std::string shellEscapeDoubleQuoted(const std::string& s)
	{
		std::string out;
		out.reserve(s.size() + 8);
		for (char c : s)
		{

			if (c == '\0')
			{
				continue;
			}
			if (c == '\\' || c == '"' || c == '$' || c == '`')
			{
				out.push_back('\\');
			}
			out.push_back(c);
		}
		return out;
	}

	inline std::string buildCommand(LogLevel lvl, const std::string& body)
	{
		const Spec s = specForLevel(lvl);
		if (!s.enabled)
		{
			return {};
		}

		std::string cmd = "notify-send -t ";
		cmd += std::to_string(s.timeoutMs);
		cmd += " -u \"";
		cmd += s.urgency;
		cmd += "\" \"ember\" \"";
		cmd += shellEscapeDoubleQuoted(body);
		cmd += "\"";
		return cmd;
	}

	inline std::string buildCommandRaw(const std::string& title,
	                                   const std::string& body,
	                                   int timeoutMs,
	                                   const char* urgency)
	{
		std::string cmd = "notify-send -t ";
		cmd += std::to_string(timeoutMs);
		cmd += " -u \"";
		cmd += urgency;
		cmd += "\" \"";
		cmd += shellEscapeDoubleQuoted(title);
		cmd += "\" \"";
		cmd += shellEscapeDoubleQuoted(body);
		cmd += "\"";
		return cmd;
	}

	struct NotifyThrottle
	{
		long long cooldownMs = 60000;

		struct Entry { long long lastEmitMs; int suppressedSince; };
		std::unordered_map<int, Entry> entries;

		bool allow(int category, long long nowMs, int& suppressedOut)
		{
			auto it = entries.find(category);
			if (it == entries.end())
			{
				entries.emplace(category, Entry{ nowMs, 0 });
				suppressedOut = 0;
				return true;
			}
			Entry& e = it->second;
			if (nowMs - e.lastEmitMs >= cooldownMs)
			{
				suppressedOut = e.suppressedSince;
				e.lastEmitMs = nowMs;
				e.suppressedSince = 0;
				return true;
			}
			e.suppressedSince += 1;
			return false;
		}
	};
}

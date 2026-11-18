#pragma once

#include "../ascii.hpp"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace CefPort
{
	inline constexpr const char* kSwitchPrefix = "--remote-debugging-port=";

	inline std::pair<std::string, bool> rewritePortArg(const std::string& arg, uint16_t port)
	{
		const std::string prefix = kSwitchPrefix;
		const std::string portStr = std::to_string(port);
		std::string out;
		out.reserve(arg.size() + 8);
		bool changed = false;

		size_t i = 0;
		while (i < arg.size())
		{
			if (arg.compare(i, prefix.size(), prefix) == 0)
			{
				out += prefix;
				i += prefix.size();

				size_t start = i;
				while (i < arg.size() && Ascii::isDigit(
					static_cast<unsigned char>(arg[i])))
				{
					++i;
				}

				if (i > start)
				{
					out += portStr;
					changed = true;
				}

			}
			else
			{
				out += arg[i];
				++i;
			}
		}

		return {out, changed};
	}

	inline bool isBindable(uint16_t port)
	{
		if (port == 0)
		{
			return false;
		}

		const int s = ::socket(AF_INET, SOCK_STREAM, 0);
		if (s < 0)
		{
			return false;
		}

		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		addr.sin_port = htons(port);

		const bool ok = ::bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0;
		::close(s);
		return ok;
	}

	inline uint16_t pickFreePort()
	{
		const int s = ::socket(AF_INET, SOCK_STREAM, 0);
		if (s < 0)
		{
			return 0;
		}

		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		addr.sin_port = 0;

		uint16_t port = 0;
		if (::bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0)
		{
			socklen_t len = sizeof(addr);
			if (::getsockname(s, reinterpret_cast<sockaddr*>(&addr), &len) == 0)
			{
				port = ntohs(addr.sin_port);
			}
		}

		::close(s);
		return port;
	}

	inline uint16_t readPortFile(const std::string& path)
	{
		std::ifstream f(path);
		if (!f)
		{
			return 0;
		}
		long v = 0;
		f >> v;
		if (v < 1024 || v > 65535)
		{
			return 0;
		}
		return static_cast<uint16_t>(v);
	}

	inline std::string formatContract(uint16_t port, long ownerPid,
	                                  unsigned long long ownerStartTicks)
	{
		std::string out = std::to_string(port);
		out += "\n";
		if (ownerPid > 0 && ownerStartTicks > 0)
		{
			out += "owner ";
			out += std::to_string(ownerPid);
			out += " ";
			out += std::to_string(ownerStartTicks);
			out += "\n";
		}
		return out;
	}

	inline unsigned long long parseStatStartTicks(const std::string& stat)
	{
		const size_t close = stat.rfind(')');
		if (close == std::string::npos)
		{
			return 0;
		}

		size_t i = close + 1;
		for (int field = 3; field <= 22; ++field)
		{
			while (i < stat.size() && stat[i] == ' ')
			{
				++i;
			}
			const size_t start = i;
			while (i < stat.size() && stat[i] != ' ' && stat[i] != '\n')
			{
				++i;
			}
			if (start == i)
			{
				return 0;
			}
			if (field == 22)
			{
				try
				{
					return std::stoull(stat.substr(start, i - start));
				}
				catch (...)
				{
					return 0;
				}
			}
		}
		return 0;
	}

	inline unsigned long long readProcStartTicks(long pid)
	{
		if (pid <= 0)
		{
			return 0;
		}
		std::ifstream f("/proc/" + std::to_string(pid) + "/stat");
		if (!f)
		{
			return 0;
		}
		std::string line;
		std::getline(f, line);
		return parseStatStartTicks(line);
	}

	inline bool writePortFile(const std::string& path, uint16_t port, long ownerPid,
	                          unsigned long long ownerStartTicks)
	{
		std::error_code ec;
		const auto parent = std::filesystem::path(path).parent_path();
		if (!parent.empty())
		{
			std::filesystem::create_directories(parent, ec);
		}
		std::ofstream f(path, std::ios::trunc);
		if (!f)
		{
			return false;
		}
		f << formatContract(port, ownerPid, ownerStartTicks);
		return static_cast<bool>(f);
	}

	inline bool writePortFile(const std::string& path, uint16_t port)
	{
		return writePortFile(path, port, 0, 0);
	}

	inline uint16_t resolveSessionPortNoPersist(const std::string& path)
	{
		const uint16_t existing = readPortFile(path);
		if (existing != 0 && isBindable(existing))
		{
			return existing;
		}
		return pickFreePort();
	}

	inline uint16_t resolveSessionPort(const std::string& path)
	{
		const uint16_t existing = readPortFile(path);
		if (existing != 0 && isBindable(existing))
		{
			return existing;
		}

		const uint16_t fresh = pickFreePort();
		if (fresh != 0)
		{
			writePortFile(path, fresh);
		}
		return fresh;
	}

	inline std::string contractPath()
	{
		const char* home = std::getenv("HOME");
		if (!home || !*home)
		{
			return "";
		}
		return std::string(home) + "/.config/ember/cef_port";
	}

	inline bool deckyPresent(const std::string& home)
	{
		if (home.empty())
		{
			return false;
		}
		std::error_code ec;
		return std::filesystem::exists(home + "/homebrew/services/PluginLoader", ec);
	}

	inline bool deckyPresent()
	{
		const char* home = std::getenv("HOME");
		if (!home || !*home)
		{
			return false;
		}
		return deckyPresent(std::string(home));
	}

	inline void removeContract(const std::string& path)
	{
		if (path.empty())
		{
			return;
		}
		std::error_code ec;
		std::filesystem::remove(path, ec);
	}
}

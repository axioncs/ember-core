#pragma once

#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace RuntimeDir
{

	inline const char* kSubdir = "ember";

	inline std::string resolveBase(const char* xdgRuntimeDir, const char* home)
	{
		if (xdgRuntimeDir != nullptr && xdgRuntimeDir[0] == '/')
		{
			return std::string(xdgRuntimeDir) + "/" + kSubdir;
		}
		if (home != nullptr && home[0] == '/')
		{
			return std::string(home) + "/.cache/" + kSubdir;
		}
		return std::string();
	}

	inline bool isPrivateDir(mode_t mode, uid_t uid, uid_t euid)
	{
		if (!S_ISDIR(mode))
		{
			return false;
		}
		if (uid != euid)
		{
			return false;
		}
		return (mode & (S_IRWXG | S_IRWXO)) == 0;
	}

	inline bool isPrivateFile(mode_t mode, uid_t uid, uid_t euid)
	{
		if (!S_ISREG(mode))
		{
			return false;
		}
		if (uid != euid)
		{
			return false;
		}
		return (mode & (S_IRWXG | S_IRWXO)) == 0;
	}

	inline bool ensureDir(const std::string& path)
	{
		if (path.empty())
		{
			return false;
		}
		struct stat st{};
		if (lstat(path.c_str(), &st) == 0)
		{
			return isPrivateDir(st.st_mode, st.st_uid, geteuid());
		}
		if (errno != ENOENT)
		{
			return false;
		}

		const std::size_t slash = path.rfind('/');
		if (slash != std::string::npos && slash > 0)
		{
			mkdir(path.substr(0, slash).c_str(), 0700);
		}
		if (mkdir(path.c_str(), 0700) != 0)
		{
			return false;
		}

		if (chmod(path.c_str(), 0700) != 0)
		{
			return false;
		}
		struct stat verify{};
		if (lstat(path.c_str(), &verify) != 0)
		{
			return false;
		}
		return isPrivateDir(verify.st_mode, verify.st_uid, geteuid());
	}

	inline bool descriptorIsPrivate(int fd)
	{
		if (fd < 0)
		{
			return false;
		}
		struct stat st{};
		if (fstat(fd, &st) != 0)
		{
			return false;
		}
		return isPrivateFile(st.st_mode, st.st_uid, geteuid());
	}
}

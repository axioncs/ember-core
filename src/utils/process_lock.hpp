

#pragma once

#include "../runtimedir.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <string>
#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>

namespace ProcessLock
{

class FileLock
{
public:
	explicit FileLock(const std::string& path, bool nonBlocking = true)
		: path_(path)
	{
		if (path.empty()) return;

		fd_ = ::open(path.c_str(), O_CREAT | O_RDWR | O_NOFOLLOW | O_CLOEXEC, 0600);
		if (fd_ < 0) return;

		if (!RuntimeDir::descriptorIsPrivate(fd_))
		{
			::close(fd_);
			fd_ = -1;
			return;
		}

		usable_ = true;

		const int mode = LOCK_EX | (nonBlocking ? LOCK_NB : 0);
		if (::flock(fd_, mode) != 0)
		{
			::close(fd_);
			fd_ = -1;
		}
	}

	~FileLock()
	{
		if (fd_ >= 0) ::close(fd_);
	}

	FileLock(const FileLock&) = delete;
	FileLock& operator=(const FileLock&) = delete;

	FileLock(FileLock&& other) noexcept
		: path_(std::move(other.path_)), fd_(other.fd_), usable_(other.usable_)
	{
		other.fd_ = -1;
		other.usable_ = false;
	}

	FileLock& operator=(FileLock&& other) noexcept
	{
		if (this == &other) return *this;
		if (fd_ >= 0) ::close(fd_);
		path_ = std::move(other.path_);
		fd_ = other.fd_;
		usable_ = other.usable_;
		other.fd_ = -1;
		other.usable_ = false;
		return *this;
	}

	bool acquired() const { return fd_ >= 0; }

	bool usable() const { return usable_; }

	bool heldByAnother() const { return !acquired() && usable(); }
	int fd() const { return fd_; }
	const std::string& path() const { return path_; }

private:
	std::string path_;
	int fd_ = -1;
	bool usable_ = false;
};

inline std::string perProcessPath(const char* prefix, pid_t pid = ::getpid())
{
	const std::string dir = RuntimeDir::resolveBase(::getenv("XDG_RUNTIME_DIR"),
	                                                ::getenv("HOME"));
	if (dir.empty() || !RuntimeDir::ensureDir(dir)) return std::string();
	char name[128];
	std::snprintf(name, sizeof(name), "/%s.%d", prefix,
	              static_cast<int>(pid));
	return dir + name;
}

}

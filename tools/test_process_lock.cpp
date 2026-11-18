

#include "../src/utils/process_lock.hpp"

#include <cassert>
#include <fcntl.h>
#include <filesystem>
#include <string>
#include <unistd.h>

int main()
{
	std::error_code ec;
	const std::string path =
		"/tmp/ember-test-process-lock." + std::to_string(getpid());
	std::filesystem::remove(path, ec);

	{
		ProcessLock::FileLock first(path);
		assert(first.acquired());
		assert(!first.heldByAnother());

		ProcessLock::FileLock second(path);
		assert(!second.acquired());

		assert(second.usable());
		assert(second.heldByAnother());
	}

	ProcessLock::FileLock afterRelease(path);
	assert(afterRelease.acquired());
	assert(!afterRelease.heldByAnother());
	std::filesystem::remove(path, ec);

	{
		const int fd = ::open(path.c_str(), O_CREAT | O_RDWR, 0666);
		assert(fd >= 0);
		::close(fd);
		ProcessLock::FileLock shared(path);
		assert(!shared.acquired());
		assert(!shared.usable());
		assert(!shared.heldByAnother());
		std::filesystem::remove(path, ec);
	}

	{
		ProcessLock::FileLock missingDir(
			"/tmp/ember-test-process-lock-absent-dir." +
			std::to_string(getpid()) + "/lock");
		assert(!missingDir.acquired());
		assert(!missingDir.usable());
		assert(!missingDir.heldByAnother());
	}

	return 0;
}

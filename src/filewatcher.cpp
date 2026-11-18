#include "filewatcher.hpp"

#include "filewatcher_burst.hpp"

#include "log.hpp"
#include "ownerwork.hpp"

#include <atomic>
#include <cstdlib>
#include <mutex>
#include <poll.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>

namespace
{

	uint32_t maskForPath(const char* path)
	{
		struct stat st{};
		if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
		{
			return IN_CREATE | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM
			     | IN_CLOSE_WRITE;
		}
		return IN_MODIFY;
	}

	std::atomic<bool>       g_watcherStopping{false};
	std::mutex              g_watcherThreadsMu;
	std::vector<pthread_t>  g_watcherThreads;
	std::once_flag          g_watcherAtexitOnce;
}

void* watchLoop(void* args)
{
	auto watcher = reinterpret_cast<CFileWatcher*>(args);
	g_pLog->debug("Started FileWatcher %u\n", watcher->notifyFd);

	for(;;)
	{

		if (g_watcherStopping.load(std::memory_order_relaxed))
		{
			return nullptr;
		}

		g_pLog->debug("Watching for changes...\n");

		struct pollfd pfd{};
		pfd.fd = watcher->notifyFd;
		pfd.events = POLLIN;
		const int ready = poll(&pfd, 1, OwnerWork::kIdleTickMs);
		if (ready <= 0 || (pfd.revents & POLLIN) == 0)
		{

			OwnerWork::idleTick();
			continue;
		}

		alignas(inotify_event) char buf[4096];
		ssize_t size = read(watcher->notifyFd, buf, sizeof(buf));
		if (size <= 0)
		{
			continue;
		}
		size += static_cast<ssize_t>(
			FileWatcherBurst::drainAdditionalReadable(watcher->notifyFd));

		if (g_watcherStopping.load(std::memory_order_relaxed))
		{
			return nullptr;
		}

		g_pLog->debug("inotify batch bytes=%zd\n", size);

		watcher->rearm();

		watcher->onModify();
	}

	return nullptr;
}

CFileWatcher::CFileWatcher(FileModifyEvent_t onModify)
{
	this->onModify = onModify;

	notifyFd = inotify_init();
	g_pLog->debug("Created notify fd %i\n", notifyFd);
}

CFileWatcher::~CFileWatcher()
{
	if (watchThread)
	{
		stop();
	}

	if (notifyFd != -1)
	{
		close(notifyFd);
	}
}

bool CFileWatcher::addFile(const char* path)
{
	int fd = inotify_add_watch(notifyFd, path, maskForPath(path));
	if (fd == -1)
	{
		return false;
	}

	watchedPaths.emplace_back(path);
	fileFdMap[fd] = path;
	g_pLog->debug("Added %s to FileWatcher %i\n", path, notifyFd);
	return true;
}

void CFileWatcher::rearm()
{
	for (const auto& path : watchedPaths)
	{
		int fd = inotify_add_watch(notifyFd, path.c_str(), maskForPath(path.c_str()));
		if (fd != -1)
		{
			fileFdMap[fd] = path;
		}
	}
}

bool CFileWatcher::start()
{
	int code = pthread_create(&watchThread, nullptr, &watchLoop, this);
	if (code != 0)
	{
		return false;
	}

	{
		std::lock_guard<std::mutex> lock(g_watcherThreadsMu);
		g_watcherThreads.push_back(watchThread);
	}
	std::call_once(g_watcherAtexitOnce, []
	{
		std::atexit(&CFileWatcher::stopAllForShutdown);
	});
	return true;
}

void CFileWatcher::stop()
{
	pthread_cancel(watchThread);
}

void CFileWatcher::stopAllForShutdown()
{
	g_watcherStopping.store(true, std::memory_order_relaxed);

	std::vector<pthread_t> threads;
	{
		std::lock_guard<std::mutex> lock(g_watcherThreadsMu);
		threads.swap(g_watcherThreads);
	}

	for (const pthread_t thread : threads)
	{
		pthread_join(thread, nullptr);
	}
}

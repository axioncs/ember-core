#include "api.hpp"

#include "sdk/IClientAppManager.hpp"

#include "afftrace.hpp"
#include "config.hpp"
#include "filewatcher.hpp"
#include "ownerwork.hpp"
#include "runtimedir.hpp"
#include "utils.hpp"

#include <cstdlib>
#include <fcntl.h>
#include <ios>
#include <unistd.h>

namespace EMBERAPI
{

	std::string contractPath;
	const char* path = "";
	std::fstream fstream;
	CFileWatcher* watcher;
}

bool EMBERAPI::isEnabled()
{
	return g_config.api.get() && fstream.is_open();
}

void EMBERAPI::onFileChange()
{

	auto watchSpan = AffTrace::watchSpan(AffTrace::Src::Api);

	if (!isEnabled())
	{
		return;
	}

	fstream.close();
	{
		const int probe = open(path, O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
		if (probe < 0)
		{
			g_pLog->info("API contract file could not be reopened -> channel closed\n");
			return;
		}
		const bool ok = RuntimeDir::descriptorIsPrivate(probe);
		close(probe);
		if (!ok)
		{
			g_pLog->info("API contract file is no longer private -> channel closed\n");
			return;
		}
	}
	fstream.open(path);
	if (!fstream.is_open())
	{
		g_pLog->info("API contract file could not be reopened for IO -> channel closed\n");
		return;
	}

	char cmd[128];
	fstream.getline(cmd, sizeof(cmd));

	g_pLog->debug("API Running %s\n", cmd);

	auto split = Utils::strsplit(cmd, "|");

	if (split.size() > 2 && split[0] == "install")
	{
		try
		{
			uint32_t appId = std::strtoul(split[1].c_str(), nullptr, 10);
			uint32_t library = std::strtoul(split[2].c_str(), nullptr, 10);

			if (!g_pClientAppManager)
			{
				g_pLog->info("API g_pClientAppManager is nullptr! Aborting...\n");
				return;
			}

			g_pLog->info("API Installing %s to %s\n", split[1].c_str(), split[2].c_str());

			const auto mode = OwnerWork::submitInstallApp(appId, library);
			g_pLog->debug("API install request dispatched %s\n", OwnerWork::modeName(mode));
		}
		catch(...)
		{
			g_pLog->info("API Failed to parse %s or %s!\n", split[1].c_str(), split[2].c_str());
		}
	}
}

void EMBERAPI::init()
{

	const std::string dir = RuntimeDir::resolveBase(getenv("XDG_RUNTIME_DIR"),
	                                                getenv("HOME"));
	if (dir.empty() || !RuntimeDir::ensureDir(dir))
	{
		g_pLog->info("API could not prepare a private runtime directory -> API unavailable\n");
		return;
	}

	contractPath = dir + "/api";
	path = contractPath.c_str();

	int fd = open(path, O_RDWR | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
	if (fd < 0)
	{
		fd = open(path, O_RDWR | O_NOFOLLOW | O_CLOEXEC);
		if (fd >= 0 && !RuntimeDir::descriptorIsPrivate(fd))
		{
			g_pLog->info("API contract file is not private -> replacing it\n");
			close(fd);
			unlink(path);
			fd = open(path, O_RDWR | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
		}
	}
	if (fd < 0)
	{
		g_pLog->info("API could not open its contract file -> API unavailable\n");
		return;
	}
	close(fd);

	fstream = std::fstream(path, std::ios::in | std::ios::out);
	if (!fstream.is_open())
	{
		g_pLog->info("API contract file could not be opened for IO -> API unavailable\n");
		return;
	}

	watcher = new CFileWatcher(onFileChange);
	watcher->addFile(path);
	watcher->start();

	g_pLog->debug("ember API initialized at %s\n", path);
}

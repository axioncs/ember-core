#include "runtime_dependencies.hpp"

#include "log.hpp"

#include <cstdlib>
#include <mutex>
#include <string_view>
#include <sys/stat.h>
#include <unistd.h>

namespace RuntimeDependencies
{
namespace
{
	bool executableOnPath(const char* path, const char* name)
	{
		if (path == nullptr || path[0] == '\0') return false;

		std::string_view entries(path);
		while (true)
		{
			const auto separator = entries.find(':');
			const auto directory = entries.substr(0, separator);
			const std::string candidate = std::string(
				directory.empty() ? std::string_view{"."} : directory) + "/" + name;
			struct stat st{};
			if (stat(candidate.c_str(), &st) == 0 && S_ISREG(st.st_mode)
			    && access(candidate.c_str(), X_OK) == 0)
			{
				return true;
			}
			if (separator == std::string_view::npos) break;
			entries.remove_prefix(separator + 1);
		}
		return false;
	}
}

std::string missingToolsOnPath(const char* path)
{
	return missingTools(
		executableOnPath(path, "unzip"),
		executableOnPath(path, "gzip"),
		executableOnPath(path, "timeout")
	);
}

bool check()
{
	static std::once_flag once;
	static bool available = false;
	std::call_once(once, []
	{
		const char* pathEnv = std::getenv("PATH");
		std::string effectivePath = pathEnv ? pathEnv : "";

		const auto prepend = [&effectivePath](const char* directory) {
			if (access(directory, F_OK) != 0) return;
			if (!effectivePath.empty()) effectivePath += ':';
			effectivePath += directory;
		};

		prepend("/run/host/usr/bin");
		prepend("/run/host/bin");

		// On NixOS the client runs in an FHS container whose PATH is
		// /run/wrappers/bin:/usr/bin plus whatever the wrapper passed in. The
		// user's tools live in the system profile, which the container does not
		// mount at /usr/bin, so a check that reads only the inherited PATH
		// reports every tool as missing while the command line works. The
		// profile directories are where those tools actually are.
		prepend("/run/current-system/sw/bin");
		prepend("/etc/profiles/per-user/current/bin");

		const std::string missing = missingToolsOnPath(effectivePath.c_str());
		available = missing.empty();
		if (!available && g_pLog)
		{
			g_pLog->warn("Runtime dependencies missing: %s\n", missing.c_str());
			g_pLog->notifyUser(UserMsg::RuntimeDependencyMissing, missing);
		}
	});
	return available;
}

}

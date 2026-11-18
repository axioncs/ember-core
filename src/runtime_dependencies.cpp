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

		if (access("/run/host/usr/bin", F_OK) == 0)
		{
			if (!effectivePath.empty()) effectivePath += ':';
			effectivePath += "/run/host/usr/bin:/run/host/bin";
		}
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

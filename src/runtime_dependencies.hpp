#pragma once

#include <string>

namespace RuntimeDependencies
{

	inline std::string missingTools(bool hasUnzip, bool hasGzip,
	                                bool hasTimeout)
	{
		std::string missing;
		if (!hasUnzip) missing = "unzip";
		if (!hasGzip)
		{
			if (!missing.empty()) missing += ", ";
			missing += "gzip";
		}
		if (!hasTimeout)
		{
			if (!missing.empty()) missing += ", ";
			missing += "timeout";
		}
		return missing;
	}

	std::string missingToolsOnPath(const char* path);

	bool check();

}

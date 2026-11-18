#pragma once

#include <string>

namespace ConfigPath
{

inline std::string emberConfigDir(const char* xdgConfigHome,
                                    const char* home)
{
	if (xdgConfigHome != nullptr && *xdgConfigHome != '\0')
		return std::string(xdgConfigHome) + "/ember";
	return std::string(home != nullptr ? home : "") + "/.config/ember";
}

}

#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace ManifestZip
{

	bool extractSingleFile(std::string_view archive,
	                       std::vector<unsigned char>& output,
	                       std::string* diagnostic = nullptr);
}

#pragma once

#include <fstream>
#include <string>

class CFileWatcher;

namespace EMBERAPI
{

	extern std::string contractPath;
	extern const char* path;
	extern std::fstream fstream;
	extern CFileWatcher* watcher;

	bool isEnabled();
	void onFileChange();
	void init();
}

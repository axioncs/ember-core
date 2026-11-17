// SPDX-License-Identifier: AGPL-3.0-only
//
// Link-time stand-in for src/log.cpp in host tests. The cloud layers log through
// the LOG_* macros, which dereference g_pLog; pulling in the real log.cpp drags
// in the whole SDK (config -> sdk.hpp -> generated protobuf headers), which a
// host test has no business compiling.
//
// g_pLog stays null, so LOG_IF_EXISTS short-circuits and these bodies never run.
// They exist only to satisfy the linker.

#include "../src/log.hpp"

#include <cstdarg>
#include <cstdio>

std::unique_ptr<CLog> g_pLog;

namespace
{
	void sink(const char* file, const char* function, const int line, const char* msg, va_list& args)
	{
		std::fprintf(stderr, "%s:%s:%d ", file, function, line);
		std::vfprintf(stderr, msg, args);
	}
}

void CLog::error(const char* file, const char* function, const int line, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	sink(file, function, line, msg, args);
	va_end(args);
}

// The unique_ptr in globals.hpp needs a definition for the deleter; without it
// the link fails even though g_pLog is never constructed.
CLog::~CLog() = default;

void CLog::warn(const char* file, const char* function, const int line, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	sink(file, function, line, msg, args);
	va_end(args);
}

void CLog::info(const char* file, const char* function, const int line, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	sink(file, function, line, msg, args);
	va_end(args);
}
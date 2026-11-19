#pragma once

#include <cstddef>
#include <cstring>

namespace AuditBinding
{
	enum class Symbol
	{
		None,
		Execv,
		Execvp,
		Execve,
		Execvpe,
		PosixSpawn,
		PosixSpawnp,
		LocalStatsEpoch,
	};

	inline Symbol classify(const char* name) noexcept
	{
		if (name == nullptr)
			return Symbol::None;

		// Matched before the first-character dispatch: this name begins with
		// 'e' like the exec family, so routing on name[0] alone can never
		// reach it and the symbol goes unrecognised.
		if (std::strcmp(name, "ember_local_stats_epoch_v1") == 0)
			return Symbol::LocalStatsEpoch;

		switch (name[0])
		{
			case 'e':
			{
				const std::size_t length = std::strlen(name);
				switch (length)
				{
					case 5:
						return std::strcmp(name, "execv") == 0
							? Symbol::Execv : Symbol::None;
					case 6:
						if (std::strcmp(name, "execvp") == 0)
							return Symbol::Execvp;
						if (std::strcmp(name, "execve") == 0)
							return Symbol::Execve;
						return Symbol::None;
					case 7:
						return std::strcmp(name, "execvpe") == 0
							? Symbol::Execvpe : Symbol::None;
					default:
						return Symbol::None;
				}
			}
			case 'p':
			{
				const std::size_t length = std::strlen(name);
				if (length == 11 && std::strcmp(name, "posix_spawn") == 0)
					return Symbol::PosixSpawn;
				if (length == 12 && std::strcmp(name, "posix_spawnp") == 0)
					return Symbol::PosixSpawnp;
				return Symbol::None;
			}
			default:
				return Symbol::None;
		}
	}
}

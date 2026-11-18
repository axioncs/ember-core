

#include "steamstub.hpp"

#include "../ascii.hpp"
#include "../config.hpp"
#include "../globals.hpp"
#include "../log.hpp"
#include "../thread_start.hpp"
#include "steamstub_warmup.hpp"
#include "steamstub_launchopt.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <signal.h>
#include <thread>
#include <unistd.h>
#include <unordered_set>
#include <vector>

namespace
{

	constexpr uint8_t kVlvSig[4] = { 'V', 'L', 'V', 0x00 };
	constexpr off_t   kVlvSigOff = 0x40;

	std::string g_helperScript;
	std::string g_steamlessHome;
	std::atomic<bool> g_enabled{false};

	std::mutex g_processedMu;
	std::unordered_set<std::string> g_processedExes;

	std::atomic<bool> g_warmupStarted{false};
	std::atomic<bool> g_warmupDone{false};
	std::atomic<bool> g_warmupFailed{false};
	std::uint64_t g_warmupGeneration = 0;
	std::mutex g_warmupMu;
	std::condition_variable g_warmupCV;

	bool fileHasStubMarker(const std::string& path)
	{
		std::ifstream f(path, std::ios::binary);
		if (!f.is_open()) return false;

		f.seekg(kVlvSigOff, std::ios::beg);
		uint8_t buf[4] = {};
		f.read(reinterpret_cast<char*>(buf), sizeof(buf));
		if (f.gcount() == static_cast<std::streamsize>(sizeof(buf))
		    && std::memcmp(buf, kVlvSig, sizeof(buf)) == 0)
		{
			return true;
		}

		f.clear();
		f.seekg(0, std::ios::beg);

		uint8_t mz[2] = {};
		f.read(reinterpret_cast<char*>(mz), sizeof(mz));
		if (f.gcount() != 2 || mz[0] != 'M' || mz[1] != 'Z') return false;

		uint32_t e_lfanew = 0;
		f.seekg(0x3c, std::ios::beg);
		f.read(reinterpret_cast<char*>(&e_lfanew), sizeof(e_lfanew));
		if (f.gcount() != sizeof(e_lfanew)) return false;
		if (e_lfanew == 0 || e_lfanew > 0x10000) return false;

		uint8_t peSig[4] = {};
		f.seekg(e_lfanew, std::ios::beg);
		f.read(reinterpret_cast<char*>(peSig), sizeof(peSig));
		if (f.gcount() != 4
		    || peSig[0] != 'P' || peSig[1] != 'E'
		    || peSig[2] != 0   || peSig[3] != 0)
		{
			return false;
		}

		uint16_t nsec = 0, optsize = 0;
		f.seekg(e_lfanew + 6, std::ios::beg);
		f.read(reinterpret_cast<char*>(&nsec), sizeof(nsec));
		f.seekg(e_lfanew + 0x14, std::ios::beg);
		f.read(reinterpret_cast<char*>(&optsize), sizeof(optsize));
		if (!f.good() || nsec == 0 || nsec > 96) return false;

		const std::streamoff secTable = static_cast<std::streamoff>(e_lfanew)
		                              + 0x18 + optsize;
		for (uint16_t i = 0; i < nsec; ++i)
		{
			char name[8] = {};
			f.seekg(secTable + i * 40, std::ios::beg);
			f.read(name, sizeof(name));
			if (!f.good()) return false;

			if (std::strncmp(name, ".bind", 5) == 0 && (name[5] == 0 || name[5] == ' '))
			{
				return true;
			}
		}
		return false;
	}

	std::string findInstallDir(uint32_t appId)
	{
		const char* home = std::getenv("HOME");
		if (!home) return {};

		static const char* steamRoots[] = {
			"/.steam/steam",
			"/.steam/debian-installation",
			"/.local/share/Steam",
		};

		std::vector<std::filesystem::path> libraryRoots;
		for (const char* suffix : steamRoots)
		{
			auto root = std::filesystem::path(home) / (std::string(suffix).substr(1));
			if (std::filesystem::exists(root))
			{
				libraryRoots.push_back(root / "steamapps");

				const auto libVdf = root / "steamapps" / "libraryfolders.vdf";
				if (std::filesystem::exists(libVdf))
				{
					std::ifstream f(libVdf);
					std::string line;
					while (std::getline(f, line))
					{

						const auto pos = line.find("\"path\"");
						if (pos == std::string::npos) continue;
						const auto qOpen = line.find('"', pos + 6);
						if (qOpen == std::string::npos) continue;
						const auto qClose = line.find('"', qOpen + 1);
						if (qClose == std::string::npos) continue;
						auto extra = line.substr(qOpen + 1, qClose - qOpen - 1);
						libraryRoots.push_back(std::filesystem::path(extra) / "steamapps");
					}
				}
			}
		}

		for (const auto& libroot : libraryRoots)
		{
			const auto manifest = libroot / ("appmanifest_" + std::to_string(appId) + ".acf");
			if (!std::filesystem::exists(manifest)) continue;

			std::ifstream f(manifest);
			std::string line;
			std::string installdir;
			while (std::getline(f, line))
			{
				const auto pos = line.find("\"installdir\"");
				if (pos == std::string::npos) continue;
				const auto qOpen = line.find('"', pos + 12);
				if (qOpen == std::string::npos) continue;
				const auto qClose = line.find('"', qOpen + 1);
				if (qClose == std::string::npos) continue;
				installdir = line.substr(qOpen + 1, qClose - qOpen - 1);
				break;
			}
			if (!installdir.empty())
			{
				return (libroot / "common" / installdir).string();
			}
		}
		return {};
	}

	constexpr auto kHelperTimeout = std::chrono::seconds(180);

	enum class HelperWait { Exited, Reaped, Failed };

	HelperWait waitForHelper(pid_t pid, int& status, const char* label)
	{
		const auto deadline = std::chrono::steady_clock::now() + kHelperTimeout;
		bool autoReaped = false;
		for (;;)
		{
			if (!autoReaped)
			{
				const pid_t waited = waitpid(pid, &status, WNOHANG);
				if (waited == pid) return HelperWait::Exited;
				if (waited < 0 && errno == ECHILD)
				{

					autoReaped = true;
				}
				else if (waited < 0 && errno != EINTR)
				{
					g_pLog->warn("SteamStub: %s waitpid failed (errno=%d)\n",
					             label, errno);
					break;
				}
			}
			else if (kill(pid, 0) != 0 && errno == ESRCH)
			{
				return HelperWait::Reaped;
			}

			if (std::chrono::steady_clock::now() >= deadline)
			{
				g_pLog->warn("SteamStub: %s timed out after %llds; terminating process group\n",
				             label,
				             static_cast<long long>(kHelperTimeout.count()));
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}

		if (kill(-pid, SIGKILL) != 0) (void)kill(pid, SIGKILL);
		while (waitpid(pid, &status, 0) < 0 && errno == EINTR) {}
		return HelperWait::Failed;
	}

	int runHelper(const std::string& exePath)
	{
		const pid_t pid = fork();
		if (pid < 0)
		{
			g_pLog->warn("SteamStub: fork() failed (errno=%d)\n", errno);
			return -1;
		}
		if (pid == 0)
		{
			(void)setpgid(0, 0);

			setenv("STEAMLESS_HOME", g_steamlessHome.c_str(), 1);

			setenv("QUIET", "1", 1);
			execlp("/bin/bash", "bash", g_helperScript.c_str(),
			       exePath.c_str(), nullptr);

			_exit(127);
		}

		(void)setpgid(pid, pid);

		int status = 0;
		switch (waitForHelper(pid, status, "Steamless helper"))
		{
			case HelperWait::Exited:
				if (WIFEXITED(status)) return WEXITSTATUS(status);
				g_pLog->warn("SteamStub: helper terminated abnormally (status=%d)\n", status);
				return -1;
			case HelperWait::Reaped:

				if (!fileHasStubMarker(exePath))
				{
					g_pLog->debug("SteamStub: helper auto-reaped; stub removed from %s -> success\n",
					              exePath.c_str());
					return 0;
				}
				g_pLog->warn("SteamStub: helper auto-reaped but stub remains in %s\n",
				             exePath.c_str());
				return -1;
			case HelperWait::Failed:
			default:
				return -1;
		}
	}

	int runPrewarm()
	{
		const pid_t pid = fork();
		if (pid < 0)
		{
			g_pLog->warn("SteamStub: prewarm fork() failed (errno=%d)\n", errno);
			return -1;
		}
		if (pid == 0)
		{
			(void)setpgid(0, 0);
			setenv("STEAMLESS_HOME", g_steamlessHome.c_str(), 1);
			setenv("QUIET", "1", 1);
			execlp("/bin/bash", "bash", g_helperScript.c_str(),
			       "--prewarm", nullptr);
			_exit(127);
		}
		(void)setpgid(pid, pid);
		int status = 0;
		switch (waitForHelper(pid, status, "Steamless prewarm"))
		{
			case HelperWait::Exited:
				return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
			case HelperWait::Reaped:

				return 0;
			case HelperWait::Failed:
			default:
				return -1;
		}
	}

	bool steamlessRequested(uint32_t appId)
	{
		const char* home = std::getenv("HOME");
		if (!home) return false;

		static const char* steamRoots[] = {
			"/.steam/steam",
			"/.steam/debian-installation",
			"/.local/share/Steam",
		};

		for (const char* suffix : steamRoots)
		{
			const auto userdata = std::filesystem::path(home)
				/ (std::string(suffix).substr(1)) / "userdata";
			std::error_code ec;
			if (!std::filesystem::is_directory(userdata, ec) || ec) continue;

			for (std::filesystem::directory_iterator it(userdata, ec), end;
			     it != end && !ec; it.increment(ec))
			{
				std::error_code fileEc;
				if (!it->is_directory(fileEc) || fileEc) continue;

				const auto vdf = it->path() / "config" / "localconfig.vdf";
				std::error_code exEc;
				if (!std::filesystem::exists(vdf, exEc) || exEc) continue;

				std::ifstream f(vdf, std::ios::binary);
				if (!f.is_open()) continue;
				std::stringstream ss;
				ss << f.rdbuf();

				const std::string opts =
					SteamlessLaunchOpt::parseAppLaunchOptions(ss.str(), appId);
				if (SteamlessLaunchOpt::requestsSteamless(opts))
				{
					g_pLog->debug("SteamStub: --steamless launch option set for %u\n", appId);
					return true;
				}
			}
		}
		return false;
	}

	void restoreOriginals(const std::string& installDir)
	{
		auto endsWith = [](const std::string& s, const char* suffix) {
			const size_t n = std::strlen(suffix);
			return s.size() >= n
			    && std::strncmp(s.data() + s.size() - n, suffix, n) == 0;
		};

		std::error_code walkEc;
		for (std::filesystem::recursive_directory_iterator it(installDir, walkEc), end;
		     it != end && !walkEc;
		     it.increment(walkEc))
		{
			if (it.depth() > 3)
			{
				it.disable_recursion_pending();
				continue;
			}
			std::error_code fileEc;
			if (!it->is_regular_file(fileEc) || fileEc) continue;

			const auto& backup = it->path();
			if (!endsWith(backup.filename().string(), ".original.exe")) continue;

			const std::string backupStr = backup.string();
			const std::string liveStr =
				backupStr.substr(0, backupStr.size() - std::strlen(".original.exe"));

			{
				std::ifstream bf(backupStr, std::ios::binary);
				char mz[2] = {};
				if (!bf.is_open() || !bf.read(mz, sizeof(mz))
				    || mz[0] != 'M' || mz[1] != 'Z')
				{
					g_pLog->warn("SteamStub: skipping restore, backup not a valid PE: %s\n",
					             backupStr.c_str());
					continue;
				}
			}

			std::error_code renEc;
			std::filesystem::rename(backup, liveStr, renEc);
			if (renEc)
			{
				g_pLog->warn("SteamStub: failed to restore %s (%s)\n",
				             liveStr.c_str(), renEc.message().c_str());
				continue;
			}

			std::error_code rmEc;
			std::filesystem::remove(liveStr + ".steamless_done", rmEc);

			{
				std::lock_guard<std::mutex> lk(g_processedMu);
				g_processedExes.erase(liveStr);
			}

			g_pLog->info("SteamStub: restored original executable (--steamless not set): %s\n",
			             liveStr.c_str());
		}
	}
}

namespace SteamStub
{

void setup(const char* installRoot)
{
	if (!installRoot || !installRoot[0])
	{
		g_pLog->debug("SteamStub: install root missing; feature disabled\n");
		return;
	}

	std::filesystem::path root(installRoot);

	const std::filesystem::path scriptCandidates[] = {
		root / "tools" / "steamstub-bypass" / "run-steamless.sh",
		std::filesystem::path(std::getenv("HOME") ? std::getenv("HOME") : "/")
			/ ".local" / "share" / "ember" / "steamstub-bypass"
			/ "run-steamless.sh",
	};
	const std::filesystem::path binCandidates[] = {
		root / "tools" / "steamless-bin",
		std::filesystem::path(std::getenv("HOME") ? std::getenv("HOME") : "/")
			/ ".local" / "share" / "ember" / "steamless-bin",
	};

	bool haveScript = false;
	for (const auto& cand : scriptCandidates)
	{
		if (std::filesystem::exists(cand))
		{
			g_helperScript = cand.string();
			haveScript = true;
			break;
		}
	}
	bool haveBinary = false;
	for (const auto& cand : binCandidates)
	{
		if (std::filesystem::exists(cand / "Steamless.CLI.exe"))
		{
			g_steamlessHome = cand.string();
			haveBinary = true;
			break;
		}
	}

	if (!haveScript || !haveBinary)
	{
		g_pLog->warn
		(
			"SteamStub: helper missing (script=%d binary=%d, root=%s); "
			"DRM removal DISABLED — games with Steam DRM will fail with "
			"\"Application load error 6\". Reinstall, or run "
			"tools/steamstub-bypass/install-steamless.sh to enable.\n",
			haveScript, haveBinary, root.c_str()
		);
		return;
	}

	g_enabled.store(true, std::memory_order_release);
	g_pLog->debug
	(
		"SteamStub: enabled (script=%s, steamless=%s)\n",
		g_helperScript.c_str(), g_steamlessHome.c_str()
	);
}

void warmupAsync()
{
	if (!g_enabled.load(std::memory_order_acquire)) return;

	if (!SteamStub::tryBeginWarmup(g_warmupMu,
	                               g_warmupStarted,
	                               g_warmupDone,
	                               g_warmupFailed,
	                               g_warmupGeneration))
	{
		return;
	}
	const bool started = ThreadStart::startDetached(
		[]
		{
			ThreadStart::runGuarded(
				[]
				{
					g_pLog->debug("SteamStub: prewarming Wine prefix in background\n");
					const int rc = runPrewarm();
					if (rc == 0)
					{
						g_pLog->debug("SteamStub: prewarm complete\n");
					}
					else
					{
						SteamStub::recordWarmupResult(g_warmupFailed, rc);
						g_pLog->debug
						(
							"SteamStub: prewarm exited with rc=%d "
							"(launch-time unpack will pay the cost)\n",
							rc
						);
					}
				},
				[]
				{
					g_warmupFailed.store(true, std::memory_order_release);
				},
				[]
				{
					SteamStub::finishWarmup(g_warmupMu,
					                        g_warmupStarted,
					                        g_warmupDone,
					                        g_warmupFailed);
					g_warmupCV.notify_all();
				});
		},
		[]
		{
			std::lock_guard<std::mutex> lk(g_warmupMu);
			g_warmupFailed.store(false, std::memory_order_release);
			g_warmupStarted.store(false, std::memory_order_release);
			g_warmupDone.store(true, std::memory_order_release);
			g_warmupCV.notify_all();
		},
		[]
		{
			g_pLog->warn("SteamStub: prewarm detach failed; joining worker\n");
		});
	if (!started)
	{
		g_pLog->warn("SteamStub: unable to start prewarm worker; launch will warm inline\n");
	}

}

void onLaunchApp(uint32_t appId)
{
	if (!appId) return;
	if (!g_config.isAddedAppId(appId)) return;

	const auto installDir = findInstallDir(appId);
	if (installDir.empty() || !std::filesystem::exists(installDir))
	{
		g_pLog->debug("SteamStub: no install dir resolved for %u; skip\n", appId);
		return;
	}

	if (!steamlessRequested(appId))
	{
		restoreOriginals(installDir);
		return;
	}

	if (!g_enabled.load(std::memory_order_acquire))
	{
		g_pLog->warn("SteamStub: --steamless set for %u but the Steamless kit is "
		             "unavailable; leaving the executable untouched\n", appId);
		return;
	}

	g_pLog->infoOnce("SteamStub: --steamless opt-in active for %u\n", appId);

	g_pLog->debug("SteamStub: scanning %s for %u\n", installDir.c_str(), appId);

	for (auto it = std::filesystem::recursive_directory_iterator(installDir);
	     it != std::filesystem::recursive_directory_iterator();
	     ++it)
	{
		if (it.depth() > 3)
		{
			it.disable_recursion_pending();
			continue;
		}
		if (!it->is_regular_file()) continue;

		const auto& p = it->path();

		auto ext = p.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(),
			[](unsigned char c){ return Ascii::toLower(c); });
		if (ext != ".exe") continue;

		const auto pathStr = p.string();

		const auto fname = p.filename().string();
		auto endsWith = [](const std::string& s, const char* suffix) {
			const size_t n = std::strlen(suffix);
			return s.size() >= n
			    && std::strncmp(s.data() + s.size() - n, suffix, n) == 0;
		};
		if (endsWith(fname, ".original.exe") || endsWith(fname, ".unpacked.exe"))
		{
			continue;
		}

		const auto markerPath = pathStr + ".steamless_done";
		if (std::filesystem::exists(markerPath))
		{
			std::error_code ec1, ec2;
			const auto exeMtime    = std::filesystem::last_write_time(p, ec1);
			const auto markerMtime = std::filesystem::last_write_time(markerPath, ec2);
			if (!ec1 && !ec2 && markerMtime >= exeMtime)
			{
				continue;
			}

			std::error_code rmEc;
			std::filesystem::remove(markerPath, rmEc);
		}

		{
			std::lock_guard<std::mutex> lk(g_processedMu);
			if (g_processedExes.count(pathStr))
			{
				continue;
			}
		}

		if (!fileHasStubMarker(pathStr))
		{
			continue;
		}

		if (!g_warmupStarted.load(std::memory_order_acquire))
		{
			warmupAsync();
		}

		std::uint64_t observedGeneration = 0;
		bool retriedWarmup = false;
		for (;;)
		{
			bool retryWarmup = false;
			{
				std::unique_lock<std::mutex> lk(g_warmupMu);
				if (observedGeneration == 0)
					observedGeneration = g_warmupGeneration;

				if (g_warmupGeneration != observedGeneration)
				{
					observedGeneration = g_warmupGeneration;
					retriedWarmup = true;
				}

				const auto waitingGeneration = observedGeneration;
				if (g_warmupStarted.load(std::memory_order_acquire)
				    && !g_warmupDone.load(std::memory_order_acquire))
				{
					g_pLog->debug("SteamStub: waiting for prewarm generation %llu\n",
					             static_cast<unsigned long long>(waitingGeneration));
					g_warmupCV.wait(lk, [waitingGeneration]
					{
						return g_warmupGeneration != waitingGeneration ||
						       g_warmupDone.load(std::memory_order_acquire);
					});
					if (g_warmupGeneration != waitingGeneration)
					{
						observedGeneration = g_warmupGeneration;
						retriedWarmup = true;
						continue;
					}
				}

				if (SteamStub::shouldRetryWarmup(
						g_warmupStarted.load(std::memory_order_acquire),
						g_warmupDone.load(std::memory_order_acquire),
						g_warmupFailed.load(std::memory_order_acquire))
				    && !retriedWarmup)
				{
					retriedWarmup = true;
					retryWarmup = true;
				}
				else
				{
					break;
				}
			}

			if (retryWarmup)
			{
				g_pLog->debug("SteamStub: retrying failed prewarm before launch\n");
				warmupAsync();
			}
		}

		g_pLog->info("SteamStub: processing %s\n", pathStr.c_str());
		const int rc = runHelper(pathStr);
		switch (rc)
		{
			case 0:
				g_pLog->info("SteamStub: processed (%s)\n", pathStr.c_str());
				break;
			case 2:

				g_pLog->debug("SteamStub: helper reported nothing to do (%s)\n", pathStr.c_str());
				break;
			default:
				g_pLog->warn
				(
					"SteamStub: helper failed for %s (rc=%d)\n",
					pathStr.c_str(), rc
				);
				g_pLog->notifyUser(UserMsg::DrmRemovalFailed);
				break;
		}

		std::lock_guard<std::mutex> lk(g_processedMu);
		g_processedExes.insert(pathStr);
	}
}

}

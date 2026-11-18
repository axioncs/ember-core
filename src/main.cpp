#include <dlfcn.h>
#include "afftrace.hpp"
#include "ascii.hpp"
#include "sdk/steam.hpp"
#include "api.hpp"
#include "runtimedir.hpp"
#include "audit_log.hpp"
#include "audit_policy.hpp"
#include "audit_symbols.hpp"
#include "config.hpp"
#include "globals.hpp"
#include "hooks.hpp"
#include "log.hpp"
#include "bootprof.hpp"
#include "ownerwork.hpp"
#include "patterns.hpp"
#include "runtime_attestation.hpp"
#include "runtime_dependencies.hpp"
#include "update.hpp"
#include "utils.hpp"
#include "utils/process_lock.hpp"

#include "feats/appinfo_provision.hpp"
#include "feats/provision_schedule.hpp"
#include "feats/appinfo_vdf.hpp"
#include "feats/cmclient.hpp"
#include "feats/apps.hpp"
#include "feats/cefport.hpp"
#include "feats/depotkey.hpp"
#include "feats/hotreload.hpp"
#include "feats/manifestid.hpp"
#include "feats/manifestdonor.hpp"
#include "feats/manifestcode.hpp"
#include "feats/steamstub.hpp"
#include "feats/stats_policy.hpp"

#include "libmem/libmem.h"

#include <cerrno>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

#include <chrono>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>

#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

static bool cleanEnvVar(const char* varName, const char* endsWith)
{
	char* var = getenv(varName);
	if (var == NULL)
		return false;

	auto splits = Utils::strsplit(var, ":");
	auto newEnv = std::string();

	for(unsigned int i = 0; i < splits.size(); i++)
	{
		auto split = splits.at(i);
		if (split.ends_with(endsWith))
		{
			g_pLog->debug("Removed %s from $%s\n", endsWith, varName);
			continue;
		}

		if(newEnv.size() > 0)
		{
			newEnv.append(":");
		}
		newEnv.append(split);
	}

	if(newEnv.size())
	{
		setenv(varName, newEnv.c_str(), true);
	}
	else
	{
		unsetenv(varName);
	}

	return true;
}

static void unload()
{

	if (!g_pLog) return;
	ManifestDonor::stop();
	ManifestCode::resetSession();
	HotReload::shutdown();
	Hooks::remove();

}

static bool setupSuccess = false;

static std::unique_ptr<ProcessLock::FileLock> g_setupLock;

static uint16_t g_cefSessionPort = 0;

static bool g_cefKeepDefaultPort = false;

static long g_clientPid = 0;
static unsigned long long g_clientStartTicks = 0;

static int g_rawLogFd = -1;
static char g_rawLogPath[4096] = {};
static std::atomic<bool> g_auditDiagnosticsEnabled{false};
static std::atomic<bool> g_auditPolicyReady{false};
static std::atomic<bool> g_auditBindAll{true};

static void setup()
{
	lm_process_t proc {};
	if (!LM_GetProcess(&proc))
	{
		unload();
		return;
	}

	if (strcmp(proc.name, "steam") != 0)
	{
		unload();
		return;
	}

	g_pLog = std::unique_ptr<CLog>(CLog::createDefaultLog());
	if (!g_pLog)
	{
		unload();
		return;
	}
	BootProf::Span setupProfile(g_pLog.get(), "setup");

	if (g_rawLogPath[0] == '\0' && !g_pLog->path.empty())
	{
		std::snprintf(g_rawLogPath, sizeof(g_rawLogPath), "%s", g_pLog->path.c_str());
	}
	if (g_rawLogFd < 0 && g_rawLogPath[0] != '\0')
	{
		g_rawLogFd = open(g_rawLogPath, O_WRONLY | O_APPEND | O_CLOEXEC);
	}

	const bool narrowAudit = AuditBinding::narrowValueEnabled(
		std::getenv("EMBER_AUDIT_NARROW"));
	const bool bindAllAudit = !narrowAudit
		|| AuditBinding::bindAllValueEnabled(
			std::getenv("EMBER_AUDIT_BINDALL"));
	g_auditBindAll.store(bindAllAudit, std::memory_order_release);
	g_auditPolicyReady.store(true, std::memory_order_release);

	g_setupLock = std::make_unique<ProcessLock::FileLock>(
		ProcessLock::perProcessPath("setup"));
	if (!g_setupLock->acquired())
	{

		if (g_setupLock->usable())
		{
			g_pLog->info("setup: another auditor namespace already initialized this process -> skipping\n");
			g_setupLock.reset();
			unload();
			return;
		}
		g_pLog->info("setup: no usable setup lock -> continuing without it\n");
		g_setupLock.reset();
	}

	g_pLog->debug("ember loading in %s\n", proc.name);

	if (AffTrace::init())
	{
		g_pLog->info("Affinity trace enabled -> %s (bounded, 0600, no identifiers)\n",
		             AffTrace::defaultPath().c_str());
	}

	OwnerWork::init();

	g_clientPid = static_cast<long>(getpid());
	g_clientStartTicks = CefPort::readProcStartTicks(g_clientPid);

	if(!g_config.init())
	{
		unload();
		return;
	}

	DepotKey::disableShaderCache();

	g_auditDiagnosticsEnabled.store(g_config.extendedLogging.get(),
	                               std::memory_order_release);

	RuntimeDependencies::check();

	if (CefPort::deckyPresent())
	{
		g_cefKeepDefaultPort = true;
		g_cefSessionPort = 0;
		CefPort::removeContract(CefPort::contractPath());
		g_pLog->info("CEF: Decky Loader detected -> keeping debug port on 8080 (not rewriting); Lumen + Decky share it\n");
	}
	else
	{
		g_cefSessionPort = CefPort::resolveSessionPortNoPersist(CefPort::contractPath());
		if (g_cefSessionPort != 0)
		{
			g_pLog->info("CEF: remote-debugging port -> %u (frees 8080); published when this client launches the webhelper\n",
			             g_cefSessionPort);
		}
		else
		{
			g_pLog->warn("CEF: could not pick a debug port; Steam will keep 8080\n");
		}
	}

	{
		const bool asyncProvision = AppInfoProvision::asyncProvisioningEnabled();
		const bool warmCurl = AppInfoProvision::shouldWarmCurlBeforePics(
		    !g_config.managedAppIds.get().empty());

		if (warmCurl && !CmClient::prepareForThreadedFetch())
		{
			g_pLog->warn("CmClient: unable to preload libcurl before PICS worker\n");
		}

		AppInfoProvision::flushPendingProtonMappings();

		const auto candidate = AppInfoVdf::findExistingPath();
		if (candidate.empty())
		{

			g_pLog->warn(
			    "AppInfoProvision: no Steam appinfo cache found; managed games "
			    "cannot be prepared this boot (open Steam once to create it)\n");
		}
		else
		{
			const auto preinitAction =
			    AppInfoProvision::preinitProvisionAction(asyncProvision);
			if (preinitAction ==
			    AppInfoProvision::PreinitProvisionAction::SynchronousProvision)
			{

				DepotKey::importLuaScripts();
				ManifestId::importLuaScripts();
				BootProf::Span provisionProfile(g_pLog.get(), "provision.sync_preinit");
				AppInfoProvision::provisionAllAddedApps(candidate);
			}
			else
			{

				DepotKey::importLuaScripts();
				ManifestId::importLuaScripts();
				BootProf::Span provisionProfile(g_pLog.get(), "provision.cold_preinit");
				const auto startupCandidates = g_config.managedAppIds.get();
				AppInfoProvision::provisionColdStartApps(
				    candidate,
				    startupCandidates,
				    AppInfoProvision::ColdStartMode::StartupRequireUsablePair,
				    nullptr,
				    true);
			}

			BootProf::Span spliceProfile(g_pLog.get(), "appinfo.splice_preinit");
			AppInfoVdf::injectAllCached(candidate);
			std::vector<AppInfoVdf::MetadataApp> dlcMetadataApps;
			std::unordered_set<std::uint32_t> seenDlcMetadataApps;
			for (const std::uint32_t baseAppId :
				g_config.managedAppIds.get())
			{
				DlcMetadata::CacheRecord record;
				if (!AppInfoProvision::readValidatedDlcMetadataCache(
					baseAppId, 0, record)) continue;
				for (const auto& app : record.apps)
				{
					if (!seenDlcMetadataApps.insert(app.appid).second) continue;
					dlcMetadataApps.push_back({
						.appid = app.appid,
						.changeNumber = app.changeNumber,
						.sha = app.sha,
						.wireBuffer = app.wireBuffer,
					});
				}
			}
			if (!dlcMetadataApps.empty())
			{
				(void)AppInfoVdf::injectValidatedMetadataApps(
					candidate, dlcMetadataApps);
			}
		}
	}

	{
		const char* cur = getenv("LD_LIBRARY_PATH");
		std::string ldLibPath = cur ? cur : "";
		ldLibPath.append(":/usr/lib:/usr/lib32");
		setenv("LD_LIBRARY_PATH", ldLibPath.c_str(), true);
	}

	Updater::init();

	setupSuccess = true;
}

static void load()
{
	if (!setupSuccess)
	{
		return;
	}

	try
	{

	static bool loadDone = false;
	if (loadDone)
	{
		return;
	}

	if (!LM_FindModule("steamclient.so", &g_modSteamClient))
	{
		unload();
		return;
	}
	if (!LM_FindModule("steamui.so", &g_modSteamUI))
	{
		unload();
		return;
	}

	{

		const std::string lockDir =
			RuntimeDir::resolveBase(getenv("XDG_RUNTIME_DIR"), getenv("HOME"));
		if (!lockDir.empty() && RuntimeDir::ensureDir(lockDir))
		{
			const std::string lockPath =
				lockDir + "/load." + std::to_string(getpid());
			const int lockFd = open(lockPath.c_str(),
			                        O_CREAT | O_RDWR | O_NOFOLLOW | O_CLOEXEC, 0600);
			if (lockFd >= 0 && RuntimeDir::descriptorIsPrivate(lockFd))
			{
				if (flock(lockFd, LOCK_EX | LOCK_NB) != 0)
				{

					g_pLog->info("load: another instance already claimed the hooking pass (guard %p) -> skipping\n",
					             static_cast<void*>(&loadDone));
					close(lockFd);
					return;
				}

			}
			else if (lockFd >= 0)
			{
				g_pLog->info("load: load lock is not a private file -> proceeding without it\n");
				close(lockFd);
			}
		}

	}
	g_pLog->info("load: claimed hooking pass (guard %p, pid %d)\n",
	             static_cast<void*>(&loadDone), getpid());
	loadDone = true;

	if (const char* attestationPath = getenv("EMBER_ATTESTATION_FILE");
	    attestationPath != nullptr && attestationPath[0] != '\0')
	{
		lm_module_t selfModule {};
		if (!LM_FindModule("ember-core.so", &selfModule))
		{
			g_pLog->warn("attestation: unable to identify loaded ember-core.so\n");
		}
		else
		{
			const std::string candidateId = Utils::getFileSHA256(selfModule.path);
			if (candidateId.empty()
			    || !RuntimeAttestation::initialize(candidateId))
			{
				g_pLog->warn("attestation: failed to initialize runtime evidence\n");
			}
		}
	}

	cleanEnvVar("LD_AUDIT", "ember-core.so");
	cleanEnvVar("LD_AUDIT", "ember-library-inject.so");
	cleanEnvVar("LD_AUDIT", "libember-core.so");
	cleanEnvVar("LD_AUDIT", "libember-ember-library-inject.so");

	auto path = std::filesystem::path(g_modSteamClient.path);
	auto dir = path.parent_path();

	g_pLog->info
	(
		"steamclient.so loaded from %s/%s at %p to %p\n",
		dir.filename().c_str(),
		path.filename().c_str(),
		g_modSteamClient.base,
		g_modSteamClient.end
	);
	g_pLog->info
	(
		"steamui.so loaded at %p to %p\n",
		g_modSteamUI.base,
		g_modSteamUI.end
	);

	{
		const auto logBuildId = [](const char* modName, const char* component)
		{
			lm_module_t mod {};
			if (!LM_FindModule(modName, &mod))
			{
				g_pLog->info("buildid: %-18s (not loaded)\n", modName);
				return;
			}
			const std::string id = Utils::getBuildId(mod.path);
			g_pLog->info("buildid: %-18s %s\n", modName,
			             id.empty() ? "(no build-id)" : id.c_str());
			if (RuntimeAttestation::enabled())
			{
				RuntimeAttestation::emit
				(
					"module-loaded",
					{
						RuntimeAttestation::Field::text("module", component),
						RuntimeAttestation::Field::text("module_file", modName),
						RuntimeAttestation::Field::text("build_id", id),
						RuntimeAttestation::Field::text
						(
							"sha256", Utils::getFileSHA256(mod.path)
						),
						RuntimeAttestation::Field::number("base", mod.base),
						RuntimeAttestation::Field::number("size", mod.size),
					}
				);
			}
		};
		logBuildId("ember-core.so", "candidate");
		logBuildId("ember-library-inject.so", "audit-loader");
		logBuildId("steamclient.so", "steamclient");
		logBuildId("steamui.so", "steamui");
		logBuildId("cloud_redirect.so", "cloud-redirect");
	}

	if (!Updater::verifySafeModeHash())
	{
		if (g_config.safeMode.get())
		{
			g_pLog->warn("Unknown steamclient.so hash! Aborting...");
			g_pLog->notifyUser(UserMsg::SteamVersionUnsupported);
			unload();
			return;
		}
		else if (g_config.warnHashMissmatch.get())
		{
			g_pLog->warn("steamclient.so hash missmatch! Please update :)");
			g_pLog->notifyUser(UserMsg::SteamVersionMismatch);
		}
	}

	if (!Steam::init())
	{
		g_pLog->warn("Failed to find steam exports!\n");
		return;
	}

	{
		BootProf::Span patternProfile(g_pLog.get(), "pattern_scan");
		if (!Patterns::init())
		{
			g_pLog->warn("Failed to find all patterns! Aborting...");
			g_pLog->notifyUser(UserMsg::InitializationFailed);
			return;
		}
	}

	if (!Hooks::setup())
	{

		g_pLog->warn("Hooks::setup() reported failure; aborting injection and "
		             "running unmodified\n");
		unload();
		return;
	}

	EMBERAPI::init();

	{
		lm_module_t selfMod {};
		const char* candidates[] = {
			"ember-core.so", "libember-core.so", nullptr
		};
		bool found = false;
		for (const char** name = candidates; *name; ++name)
		{
			if (LM_FindModule(*name, &selfMod))
			{
				found = true;
				break;
			}
		}
		if (found)
		{
			auto root = std::filesystem::path(selfMod.path).parent_path().string();
			SteamStub::setup(root.c_str());

		}
		else
		{
			g_pLog->debug("SteamStub: could not resolve ember-core.so path; feature disabled\n");
		}
	}

	{
		BootProf::Span importProfile(g_pLog.get(), "lua_imports");
		DepotKey::onStartup();
		ManifestId::importLuaScripts();
	}

	{
		std::lock_guard<std::mutex> passLock(
		    AppInfoProvision::provisioningPassMutex());
		bool dlcCollectionComplete = false;
		const auto dlcIds = AppInfoProvision::collectDlcAppIdsForAddedApps(
		    &dlcCollectionComplete);
		if (dlcCollectionComplete)
		{
			Apps::setDiscoveredAppDlcIds(dlcIds.appDlc);
		}
	}

	HotReload::initialize();

	if (g_config.notifyInit.get())
	{
		const auto now = std::chrono::time_point{std::chrono::system_clock::now()};
		const auto ymd = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(now)};

		if (static_cast<unsigned int>(ymd.month()) == 2 && static_cast<unsigned int>(ymd.day()) == 22)
		{
			g_pLog->notify("Happy birthday ember!");
			g_pLog->notifyUser(UserMsg::BirthdayGreeting);
		}
		else
		{
			g_pLog->notify("Loaded successfully");
			g_pLog->notifyUser(UserMsg::LoadSuccess);
		}
	}
	}
	catch (const std::exception& e)
	{

		if (g_pLog) g_pLog->warn("setup() aborted by exception: %s\n", e.what());
		try
		{
			unload();
		}
		catch (...)
		{
		}
		return;
	}
	catch (...)
	{

		if (g_pLog) g_pLog->warn("setup() aborted by a non-std exception\n");
		try
		{
			unload();
		}
		catch (...)
		{

		}
		return;
	}
}

#include <thread>
#include <chrono>
#include <link.h>
#include <spawn.h>

extern "C" unsigned int la_version(unsigned int)
{
	return LAV_CURRENT;
}

namespace
{
	using execv_t  = int (*)(const char*, char* const[]);
	using execve_t = int (*)(const char*, char* const[], char* const[]);
	using spawn_t  = int (*)(pid_t*, const char*, const posix_spawn_file_actions_t*,
	                         const posix_spawnattr_t*, char* const[], char* const[]);

	execv_t  g_realExecv     = nullptr;
	execv_t  g_realExecvp    = nullptr;
	execve_t g_realExecve    = nullptr;
	execve_t g_realExecvpe   = nullptr;
	spawn_t  g_realSpawn     = nullptr;
	spawn_t  g_realSpawnp    = nullptr;

	std::atomic<unsigned int> g_auditDiagnosticMask {0};

	const char* auditSymbolName(AuditBinding::Symbol symbol) noexcept
	{
		switch (symbol)
		{
			case AuditBinding::Symbol::Execv: return "execv";
			case AuditBinding::Symbol::Execvp: return "execvp";
			case AuditBinding::Symbol::Execve: return "execve";
			case AuditBinding::Symbol::Execvpe: return "execvpe";
			case AuditBinding::Symbol::PosixSpawn: return "posix_spawn";
			case AuditBinding::Symbol::PosixSpawnp: return "posix_spawnp";
			case AuditBinding::Symbol::LocalStatsEpoch: return "ember_local_stats_epoch_v1";
			default: return "<unknown>";
		}
	}
	const char* auditObjectName(uintptr_t* cookie) noexcept
	{
		if (cookie == nullptr || *cookie == 0)
			return "<unknown>";
		const auto* map = reinterpret_cast<const link_map*>(*cookie);
		if (map == nullptr || map->l_name == nullptr || map->l_name[0] == '\0')
			return "<main executable>";
		return map->l_name;
	}

	void writeRawLog(const char* text) noexcept
	{
		AuditLog::write(g_rawLogFd, g_rawLogPath, text);
	}

	void writeRawUnsigned(unsigned int value) noexcept
	{
		char digits[11] = {};
		size_t pos = sizeof(digits) - 1;
		do
		{
			digits[--pos] = static_cast<char>('0' + (value % 10));
			value /= 10;
		}
		while (value != 0);
		writeRawLog(digits + pos);
	}

	void logAuditBinding(AuditBinding::Symbol symbol,
	                     uintptr_t* refcook,
	                     uintptr_t* defcook) noexcept
	{

		if (!g_auditDiagnosticsEnabled.load(std::memory_order_acquire))
			return;
		const unsigned int bit = 1u << static_cast<unsigned int>(symbol);
		if ((g_auditDiagnosticMask.fetch_or(bit, std::memory_order_relaxed) & bit) != 0)
			return;
		writeRawLog("[Info] audit: ");
		writeRawLog(auditSymbolName(symbol));
		writeRawLog(" referenced by ");
		writeRawLog(auditObjectName(refcook));
		writeRawLog(" (definition ");
		writeRawLog(auditObjectName(defcook));
		writeRawLog(")\n");
	}

	void logCefRewrite(uint16_t port) noexcept
	{

		writeRawLog("[Info] CEF: rewrote --remote-debugging-port to ");
		writeRawUnsigned(port);
		writeRawLog(" (8080 freed)\n");
	}

	char** cefRewriteArgv(char* const argv[])
	{
		if (!argv)
		{
			return nullptr;
		}

		if (g_cefKeepDefaultPort)
		{
			return nullptr;
		}

		int n = 0;
		bool any = false;
		for (; argv[n]; ++n)
		{
			const char* p = std::strstr(argv[n], CefPort::kSwitchPrefix);
			if (p && Ascii::isDigit(static_cast<unsigned char>(
				p[std::strlen(CefPort::kSwitchPrefix)])))
			{
				any = true;
			}
		}
		if (!any)
		{
			return nullptr;
		}

		static uint16_t port = 0;
		if (port == 0)
		{

			port = g_cefSessionPort != 0
			           ? g_cefSessionPort
			           : CefPort::resolveSessionPortNoPersist(CefPort::contractPath());
		}
		if (port == 0)
		{
			return nullptr;
		}

		static bool published = false;
		if (!published && CefPort::writePortFile(CefPort::contractPath(), port,
		                                        g_clientPid, g_clientStartTicks))
		{
			published = true;
			if (g_pLog)
			{
				g_pLog->info("CEF: published debug port %u to %s (owner pid %ld, start %llu)\n",
				             port, CefPort::contractPath().c_str(), g_clientPid,
				             g_clientStartTicks);
			}
		}

		char** out = static_cast<char**>(std::malloc(sizeof(char*) * (n + 1)));
		if (!out)
		{
			return nullptr;
		}
		for (int i = 0; i < n; ++i)
		{
			auto [s, changed] = CefPort::rewritePortArg(argv[i], port);
			out[i] = changed ? strdup(s.c_str()) : argv[i];
		}
		out[n] = nullptr;

		logCefRewrite(port);
		return out;
	}

	int cefExecv(const char* path, char* const argv[])
	{
		char** rw = cefRewriteArgv(argv);
		return g_realExecv(path, rw ? rw : argv);
	}
	int cefExecvp(const char* file, char* const argv[])
	{
		char** rw = cefRewriteArgv(argv);
		return g_realExecvp(file, rw ? rw : argv);
	}
	int cefExecve(const char* path, char* const argv[], char* const envp[])
	{
		char** rw = cefRewriteArgv(argv);
		return g_realExecve(path, rw ? rw : argv, envp);
	}
	int cefExecvpe(const char* file, char* const argv[], char* const envp[])
	{
		char** rw = cefRewriteArgv(argv);
		return g_realExecvpe(file, rw ? rw : argv, envp);
	}
	int cefSpawn(pid_t* pid, const char* path, const posix_spawn_file_actions_t* fa,
	             const posix_spawnattr_t* attr, char* const argv[], char* const envp[])
	{
		char** rw = cefRewriteArgv(argv);
		return g_realSpawn(pid, path, fa, attr, rw ? rw : argv, envp);
	}
	int cefSpawnp(pid_t* pid, const char* file, const posix_spawn_file_actions_t* fa,
	              const posix_spawnattr_t* attr, char* const argv[], char* const envp[])
	{
		char** rw = cefRewriteArgv(argv);
		return g_realSpawnp(pid, file, fa, attr, rw ? rw : argv, envp);
	}
}

#if defined(__x86_64__)
extern "C" uintptr_t la_symbind64(Elf64_Sym* sym,
                                  __attribute__((unused)) unsigned int ndx,
                                  uintptr_t* refcook,
                                  uintptr_t* defcook,
                                  __attribute__((unused)) unsigned int* flags,
                                  const char* symname)
#else
extern "C" uintptr_t la_symbind32(Elf32_Sym* sym,
                                  __attribute__((unused)) unsigned int ndx,
                                  uintptr_t* refcook,
                                  uintptr_t* defcook,
                                  __attribute__((unused)) unsigned int* flags,
                                  const char* symname)
#endif
{
	if (sym == nullptr)
		return 0;

	const AuditBinding::Symbol symbol = AuditBinding::classify(symname);
	if (symbol == AuditBinding::Symbol::None)
		return sym->st_value;

	logAuditBinding(symbol, refcook, defcook);
	const auto orig = static_cast<uintptr_t>(sym->st_value);
	switch (symbol)
	{
		case AuditBinding::Symbol::LocalStatsEpoch:
			return reinterpret_cast<uintptr_t>(&ember_local_stats_epoch_v1);
		case AuditBinding::Symbol::Execv:
			if (!g_realExecv) g_realExecv = reinterpret_cast<execv_t>(orig);
			return reinterpret_cast<uintptr_t>(&cefExecv);
		case AuditBinding::Symbol::Execvp:
			if (!g_realExecvp) g_realExecvp = reinterpret_cast<execv_t>(orig);
			return reinterpret_cast<uintptr_t>(&cefExecvp);
		case AuditBinding::Symbol::Execve:
			if (!g_realExecve) g_realExecve = reinterpret_cast<execve_t>(orig);
			return reinterpret_cast<uintptr_t>(&cefExecve);
		case AuditBinding::Symbol::Execvpe:
			if (!g_realExecvpe) g_realExecvpe = reinterpret_cast<execve_t>(orig);
			return reinterpret_cast<uintptr_t>(&cefExecvpe);
		case AuditBinding::Symbol::PosixSpawn:
			if (!g_realSpawn) g_realSpawn = reinterpret_cast<spawn_t>(orig);
			return reinterpret_cast<uintptr_t>(&cefSpawn);
		case AuditBinding::Symbol::PosixSpawnp:
			if (!g_realSpawnp) g_realSpawnp = reinterpret_cast<spawn_t>(orig);
			return reinterpret_cast<uintptr_t>(&cefSpawnp);
		case AuditBinding::Symbol::None:
			break;
	}
	return sym->st_value;
}

extern "C" unsigned int la_objopen(struct link_map* map,
                                   __attribute__((unused)) Lmid_t lmid,
                                   __attribute__((unused)) uintptr_t* cookie)
{
	if (map && map->l_name &&
	    (AuditBinding::hasSuffix(map->l_name, "/steamclient.so") ||
	     AuditBinding::hasSuffix(map->l_name, "/steamui.so")))
	{
		if (!setupSuccess)
		{
			setup();
		}
		load();
	}

	const bool bindAll = !g_auditPolicyReady.load(std::memory_order_acquire)
		|| g_auditBindAll.load(std::memory_order_acquire);
	return AuditBinding::flagsForObject(map ? map->l_name : nullptr, bindAll);
}

extern "C" void la_preinit(__attribute__((unused)) uintptr_t* cookie)
{
	setup();
}

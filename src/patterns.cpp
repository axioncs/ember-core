#include "patterns.hpp"

#include "globals.hpp"
#include "memhlp.hpp"
#include "feats/ipcframe.hpp"
#include "pattern_catalog.hpp"
#include "pattern_cache.hpp"
#include "runtime_attestation.hpp"
#include "config.hpp"
#include "utils.hpp"

#include "libmem/libmem.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <optional>
#include <system_error>

#include <sys/stat.h>

namespace
{
	std::optional<PatternCatalog::Catalog> g_steamClientCatalog;
	std::optional<PatternCatalog::Catalog> g_steamUiCatalog;

	struct LocalCatalogState
	{
		std::optional<PatternCache::ModuleIdentity> identity;
		std::optional<PatternCache::Catalog> catalog;
		bool writeEligible = false;
		bool dirty = false;
	};

	LocalCatalogState g_steamClientLocal;
	LocalCatalogState g_steamUiLocal;

	std::optional<std::filesystem::path> patternRoot();

	bool localCacheEnabled()
	{
		return PatternCache::enabled(
			g_config.patternCache.get(), std::getenv("EMBER_PATTERN_CACHE")
		);
	}

	std::optional<PatternCache::ModuleIdentity> moduleIdentity(
		const char* component,
		const char* moduleName,
		const lm_module_t& module
	)
	{
		struct stat value {};
		if (stat(module.path, &value) != 0 || !S_ISREG(value.st_mode) || value.st_size <= 0)
			return std::nullopt;
		const std::string buildId = Utils::getBuildId(module.path);
		if (buildId.empty())
			return std::nullopt;
		return PatternCache::ModuleIdentity
		{
			component,
			moduleName,
			buildId,
			static_cast<std::uint64_t>(value.st_size),
			static_cast<std::int64_t>(value.st_mtim.tv_sec),
			static_cast<std::int64_t>(value.st_mtim.tv_nsec),
		};
	}

	std::filesystem::path localCatalogPath(
		const std::filesystem::path& root,
		const PatternCache::ModuleIdentity& identity
	)
	{
		const std::string key = identity.gnuBuildId + "-"
			+ std::to_string(identity.size) + "-"
			+ std::to_string(identity.mtimeSeconds) + "-"
			+ std::to_string(identity.mtimeNanoseconds) + ".cache";
		return root / "local" / identity.component / key;
	}

	LocalCatalogState loadLocalCatalog(
		const char* component,
		const char* moduleName,
		const lm_module_t& module
	)
	{
		LocalCatalogState state;
		if (!localCacheEnabled())
			return state;
		state.identity = moduleIdentity(component, moduleName, module);
		const auto root = patternRoot();
		if (!state.identity || !root)
			return state;

		std::string error;
		state.catalog = PatternCache::load(
			localCatalogPath(*root, *state.identity), *state.identity, &error
		);
		if (!state.catalog)
			state.writeEligible = true;
		return state;
	}

	LocalCatalogState& localStateFor(const Pattern_t& pattern)
	{
		return pattern.module == &g_modSteamUI ? g_steamUiLocal : g_steamClientLocal;
	}

	const PatternCatalog::Catalog* catalogFor(const Pattern_t& pattern)
	{
		const auto& selected = pattern.module == &g_modSteamUI
			? g_steamUiCatalog
			: g_steamClientCatalog;
		return selected ? &*selected : nullptr;
	}

	const char* followModeName(MemHlp::SigFollowMode mode)
	{
		switch (mode)
		{
			case MemHlp::SigFollowMode::Relative: return "Relative";
			case MemHlp::SigFollowMode::PrologueUpwards: return "PrologueUpwards";
			default: return "None";
		}
	}

	std::optional<lm_address_t> localCatalogAddress(
		const Pattern_t& pattern,
		bool logInvalid,
		lm_address_t* matchAddressOut
	)
	{
		LocalCatalogState& state = localStateFor(pattern);
		if (!state.catalog)
			return std::nullopt;

		const auto reject = [&](const char* message) -> std::optional<lm_address_t>
		{

			if (logInvalid)
			{
				state.catalog.reset();
				state.writeEligible = true;
				state.dirty = true;
			}
			if (logInvalid)
				g_pLog->warn("Local pattern cache for '%s' rejected: %s; using embedded resolver\n",
				             pattern.name.c_str(), message);
			return std::nullopt;
		};
		const PatternCache::Locator* entry = state.catalog->entry(pattern.symbol);
		if (entry == nullptr)
		{

			if (pattern.optional)
				return std::nullopt;
			return reject("locator is missing from the complete cache policy");
		}
		if (entry->required != !pattern.optional)
			return reject("compiled policy mismatch");
		if (entry->followMode != followModeName(pattern.followMode)
		    || entry->signature != pattern.pattern)
			return reject("compiled resolver metadata mismatch");

		const lm_module_t& module = pattern.module ? *pattern.module : g_modSteamClient;
		if (entry->targetRva >= module.size
		    || entry->targetRva > static_cast<std::uint64_t>(LM_ADDRESS_BAD - module.base))
			return reject("target RVA is outside the module");
		const lm_address_t candidate = module.base
			+ static_cast<lm_address_t>(entry->targetRva);
		lm_segment_t targetSegment {};
		const bool targetInside = candidate >= module.base && candidate < module.end;
		const bool targetExecutable = targetInside && LM_FindSegment(candidate, &targetSegment)
			&& (targetSegment.prot & LM_PROT_XR) == LM_PROT_XR;
		if (!targetExecutable)
			return reject("target RVA is not executable");

		const auto signatureBytes = PatternCache::signatureSize(entry->signature);
		if (!signatureBytes || entry->matchRva >= module.size
		    || *signatureBytes > module.size - entry->matchRva
		    || entry->matchRva > static_cast<std::uint64_t>(LM_ADDRESS_BAD - module.base))
			return reject("match RVA is outside the module");
		const lm_address_t match = module.base
			+ static_cast<lm_address_t>(entry->matchRva);
		lm_segment_t matchSegment {};
		if (!LM_FindSegment(match, &matchSegment)
		    || (matchSegment.prot & LM_PROT_XR) != LM_PROT_XR
		    || match < matchSegment.base
		    || match >= matchSegment.end
		    || matchSegment.end - match < *signatureBytes)
			return reject("signature range is not executable");
		if (!PatternCache::signatureMatches(
			entry->signature,
			std::span<const std::uint8_t>(
				reinterpret_cast<const std::uint8_t*>(match), *signatureBytes
			)))
			return reject("signature bytes changed");

		lm_address_t derivedTarget = match;
		switch (pattern.followMode)
		{
			case MemHlp::SigFollowMode::Relative:
				derivedTarget = MemHlp::getJmpTarget(match);
				break;
			case MemHlp::SigFollowMode::PrologueUpwards:
				derivedTarget = MemHlp::findPrologue(
					match,
					MemHlp::prologueLowerBound(module.base, matchSegment.base),
					pattern.prologue.empty() ? nullptr : pattern.prologue.data(),
					pattern.prologue.size()
				);
				break;
			case MemHlp::SigFollowMode::None:
				break;
		}
		if (derivedTarget == LM_ADDRESS_BAD
		    || derivedTarget < module.base || derivedTarget >= module.end)
			return reject("followed target is outside the module");
		const auto derivedTargetRva = static_cast<std::uint64_t>(
			derivedTarget - module.base
		);
		if (!PatternCache::targetRvaMatches(
			entry->followMode, entry->matchRva, entry->targetRva,
			derivedTargetRva
		))
			return reject("target RVA does not match the matched signature");

		if (matchAddressOut != nullptr)
			*matchAddressOut = match;
		return candidate;
	}

	std::optional<lm_address_t> catalogAddress(
		const Pattern_t& pattern,
		bool logInvalid,
		lm_address_t* matchAddressOut = nullptr
	)
	{
		if (const auto local = localCatalogAddress(pattern, logInvalid, matchAddressOut))
			return local;
		const PatternCatalog::Catalog* catalog = catalogFor(pattern);
		if (catalog == nullptr)
			return std::nullopt;
		const PatternCatalog::Locator* entry = catalog->entry(pattern.symbol);
		if (entry == nullptr)
			return std::nullopt;
		if (entry->required != !pattern.optional)
		{
			if (logInvalid)
				g_pLog->warn("Pattern catalog policy mismatch for '%s'; using embedded resolver\n",
				             pattern.name.c_str());
			return std::nullopt;
		}
		if (entry->signature != pattern.pattern ||
			entry->followMode != followModeName(pattern.followMode))
		{
			if (logInvalid)
				g_pLog->warn(
					"Pattern catalog resolver mismatch for '%s'; using embedded resolver\n",
					pattern.name.c_str());
			return std::nullopt;
		}

		const lm_module_t& module = pattern.module ? *pattern.module : g_modSteamClient;
		const lm_address_t rva = static_cast<lm_address_t>(entry->targetRva);
		if (rva >= module.size || module.base > LM_ADDRESS_BAD - rva)
			return std::nullopt;
		const lm_address_t candidate = module.base + rva;
		const bool inside = candidate >= module.base && candidate < module.end;
		lm_segment_t segment {};
		const bool executable = inside && LM_FindSegment(candidate, &segment)
		                     && (segment.prot & LM_PROT_XR) == LM_PROT_XR;
		if (!inside || !executable)
		{
			if (logInvalid)
				g_pLog->warn("Pattern catalog RVA for '%s' is not executable; using embedded resolver\n",
				             pattern.name.c_str());
			return std::nullopt;
		}
		return candidate;
	}

	std::optional<std::filesystem::path> patternRoot()
	{
		if (const char* xdg = std::getenv("XDG_CONFIG_HOME"); xdg != nullptr && xdg[0] != '\0')
		{
			std::filesystem::path root(xdg);
			if (root.is_absolute())
				return root / "ember" / "patterns";
			return std::nullopt;
		}
		if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0')
		{
			std::filesystem::path root(home);
			if (root.is_absolute())
				return root / ".config" / "ember" / "patterns";
		}
		return std::nullopt;
	}

	std::optional<PatternCatalog::Catalog> loadCatalog(
		const char* component,
		const char* moduleName,
		const lm_module_t& module
	)
	{
		const auto root = patternRoot();
		if (!root)
			return std::nullopt;
		const std::filesystem::path componentRoot = *root / component;
		std::error_code error;
		if (!std::filesystem::is_directory(componentRoot, error) || error)
			return std::nullopt;

		try
		{
			const std::string sha256 = Utils::getFileSHA256(module.path);
			const std::string buildId = Utils::getBuildId(module.path);
			if (sha256.size() != 64 || buildId.empty())
				return std::nullopt;
			const auto moduleSize = std::filesystem::file_size(module.path, error);
			if (error || moduleSize == 0)
				return std::nullopt;
			const std::filesystem::path path = componentRoot / (sha256 + ".toml");
			const auto status = std::filesystem::symlink_status(path, error);
			if (error || status.type() != std::filesystem::file_type::regular)
				return std::nullopt;
			const auto bodySize = std::filesystem::file_size(path, error);
			if (error || bodySize == 0 || bodySize > PatternCatalog::kMaximumBodySize)
				return std::nullopt;
			std::ifstream stream(path, std::ios::binary);
			if (!stream)
				return std::nullopt;
			std::string body(static_cast<std::size_t>(bodySize), '\0');
			stream.read(body.data(), static_cast<std::streamsize>(body.size()));
			if (!stream || stream.gcount() != static_cast<std::streamsize>(body.size()))
				return std::nullopt;

			std::string parseError;
			auto catalog = PatternCatalog::parseCanonical(
				body,
				{
					component,
					moduleName,
					sha256,
					moduleSize,
					buildId,
				},
				&parseError
			);
			if (!catalog)
			{
				g_pLog->warn("Pattern catalog for %s rejected: %s\n",
				             component, parseError.c_str());
				return std::nullopt;
			}
			return catalog;
		}
		catch (const std::exception&)
		{
			g_pLog->warn("Pattern catalog for %s could not be read; using embedded resolvers\n",
			             component);
			return std::nullopt;
		}
	}

	std::vector<PatternCatalog::CompiledLocator> compiledPolicy(bool steamUi)
	{
		std::vector<PatternCatalog::CompiledLocator> result;
		for (const Pattern_t* pattern : Patterns::patterns())
		{
			if ((pattern->module == &g_modSteamUI) != steamUi)
				continue;
			result.push_back({
				pattern->symbol,
				!pattern->optional,
				pattern->pattern,
				followModeName(pattern->followMode),
			});
		}
		return result;
	}

	std::vector<PatternCache::CompiledLocator> compiledLocalPolicy(bool steamUi)
	{
		std::vector<PatternCache::CompiledLocator> result;
		for (const Pattern_t* pattern : Patterns::patterns())
		{
			if ((pattern->module == &g_modSteamUI) != steamUi)
				continue;
			result.push_back({
				pattern->symbol,
				!pattern->optional,
				pattern->pattern,
				followModeName(pattern->followMode),
			});
		}
		return result;
	}

	void loadActiveCatalogs()
	{
		g_steamClientLocal = loadLocalCatalog(
			"steamclient", "steamclient.so", g_modSteamClient
		);
		g_steamUiLocal = loadLocalCatalog("steamui", "steamui.so", g_modSteamUI);
		g_steamClientCatalog.reset();
		g_steamUiCatalog.reset();

		const auto clientPolicy = compiledPolicy(false);
		const auto localClientPolicy = compiledLocalPolicy(false);
		if (g_steamClientLocal.catalog
		    && !PatternCache::policyMatches(g_steamClientLocal.catalog.value(), localClientPolicy))
		{
			g_pLog->warn("Local pattern cache for steamclient changed compiled locator policy; ignoring it\n");
			g_steamClientLocal.catalog.reset();
			g_steamClientLocal.writeEligible = true;
			g_steamClientLocal.dirty = true;
		}
		const auto uiPolicy = compiledPolicy(true);
		const auto localUiPolicy = compiledLocalPolicy(true);
		if (g_steamUiLocal.catalog
		    && !PatternCache::policyMatches(g_steamUiLocal.catalog.value(), localUiPolicy))
		{
			g_pLog->warn("Local pattern cache for steamui changed compiled locator policy; ignoring it\n");
			g_steamUiLocal.catalog.reset();
			g_steamUiLocal.writeEligible = true;
			g_steamUiLocal.dirty = true;
		}

		if (!g_steamClientLocal.catalog)
			g_steamClientCatalog = loadCatalog("steamclient", "steamclient.so", g_modSteamClient);
		if (!g_steamUiLocal.catalog)
			g_steamUiCatalog = loadCatalog("steamui", "steamui.so", g_modSteamUI);

		if (g_steamClientCatalog && !g_steamClientCatalog->validatePolicy(clientPolicy))
		{
			g_pLog->warn("Pattern catalog for steamclient changed compiled locator policy; ignoring it\n");
			g_steamClientCatalog.reset();
		}
		if (g_steamUiCatalog && !g_steamUiCatalog->validatePolicy(uiPolicy))
		{
			g_pLog->warn("Pattern catalog for steamui changed compiled locator policy; ignoring it\n");
			g_steamUiCatalog.reset();
		}
		if (g_steamClientLocal.catalog)
			g_pLog->info("Pattern cache: loaded steamclient local catalog\n");
		if (g_steamUiLocal.catalog)
			g_pLog->info("Pattern cache: loaded steamui local catalog\n");
		if (g_steamClientCatalog)
			g_pLog->info("Pattern catalog: loaded steamclient revision %llu\n",
			             static_cast<unsigned long long>(g_steamClientCatalog->revision()));
		if (g_steamUiCatalog)
			g_pLog->info("Pattern catalog: loaded steamui revision %llu\n",
			             static_cast<unsigned long long>(g_steamUiCatalog->revision()));
	}

	std::optional<PatternCache::Catalog> buildLocalCatalog(bool steamUi)
	{
		LocalCatalogState& state = steamUi ? g_steamUiLocal : g_steamClientLocal;
		if (!state.identity)
			return std::nullopt;
		const lm_module_t& module = steamUi ? g_modSteamUI : g_modSteamClient;
		PatternCache::Catalog result {*state.identity, {}};
		for (const Pattern_t* pattern : Patterns::patterns())
		{
			if ((pattern->module == &g_modSteamUI) != steamUi)
				continue;
			if (pattern->address == LM_ADDRESS_BAD || pattern->matchAddress == LM_ADDRESS_BAD)
			{
				if (!pattern->optional)
					return std::nullopt;
				continue;
			}
			if (pattern->address < module.base || pattern->matchAddress < module.base
			    || pattern->address >= module.end || pattern->matchAddress >= module.end)
				return std::nullopt;
			const lm_address_t targetRva = pattern->address - module.base;
			const lm_address_t matchRva = pattern->matchAddress - module.base;
			if (targetRva >= module.size || matchRva >= module.size)
				return std::nullopt;
			result.locators.push_back(
				{
					pattern->symbol,
					static_cast<std::uint64_t>(targetRva),
					static_cast<std::uint64_t>(matchRva),
					pattern->pattern,
					followModeName(pattern->followMode),
					!pattern->optional,
				}
			);
		}
		return result.locators.empty() ? std::nullopt : std::optional<PatternCache::Catalog>(std::move(result));
	}

	void writeLocalCatalog(bool steamUi)
	{
		LocalCatalogState& state = steamUi ? g_steamUiLocal : g_steamClientLocal;
		if ((!state.writeEligible && !state.dirty)
		    || (steamUi ? g_steamUiCatalog.has_value() : g_steamClientCatalog.has_value()))
			return;
		const auto root = patternRoot();
		const auto catalog = buildLocalCatalog(steamUi);
		if (!root || !catalog)
			return;
		const std::string body = PatternCache::serialize(*catalog);
		if (body.empty())
			return;
		if (PatternCache::writeAtomic(
			localCatalogPath(*root, catalog->identity), body
		))
		{
			g_pLog->info("Pattern cache: wrote %s local catalog (%zu locators)\n",
			             steamUi ? "steamui" : "steamclient", catalog->locators.size());
			state.writeEligible = false;
			state.dirty = false;
		}
	}

	void writeLocalCatalogs()
	{
		writeLocalCatalog(false);
		writeLocalCatalog(true);
	}
}

Pattern_t::Pattern_t(const char* name, const char* pattern,
	MemHlp::SigFollowMode followMode, lm_module_t* module, const char* symbol)
	:
	Pattern_t(name, pattern, followMode, std::vector<int16_t>(), module, symbol)
{
}

Pattern_t::Pattern_t(const char* name, const char* pattern,
	MemHlp::SigFollowMode followMode, std::vector<uint8_t> prologue,
	lm_module_t* module, const char* symbol)
	:
	name(name),
	symbol(symbol != nullptr ? symbol : std::string("Patterns::") + name),
	pattern(pattern),
	followMode(followMode),
	prologue(prologue.begin(), prologue.end()),
	module(module)
{
	Patterns::patterns().emplace_back(this);
}

Pattern_t::Pattern_t(const char* name, const char* pattern,
	MemHlp::SigFollowMode followMode, std::vector<int16_t> prologue,
	lm_module_t* module, const char* symbol)
	:
	name(name),
	symbol(symbol != nullptr ? symbol : std::string("Patterns::") + name),
	pattern(pattern),
	followMode(followMode),
	prologue(std::move(prologue)),
	module(module)
{
	Patterns::patterns().emplace_back(this);
}

bool Pattern_t::find()
{
	lm_module_t& targetModule = module ? *module : g_modSteamClient;
	matchAddress = LM_ADDRESS_BAD;
	lm_address_t cachedMatch = LM_ADDRESS_BAD;
	if (const auto trusted = catalogAddress(*this, true, &cachedMatch))
	{
		address = *trusted;
		matchAddress = cachedMatch;
	}
	else
	{
		const auto resolved = MemHlp::searchSignatureDetailed
		(
			name.c_str(), pattern.c_str(), targetModule, followMode,
			prologue.empty() ? nullptr : prologue.data(), prologue.size()
		);
		matchAddress = resolved.match;
		address = resolved.target;

		if (address != LM_ADDRESS_BAD)
		{
			lm_segment_t targetSegment {};
			const bool inside = address >= targetModule.base
			                 && address < targetModule.base + targetModule.size;
			const bool executable = LM_FindSegment(address, &targetSegment)
			                     && (targetSegment.prot & LM_PROT_XR) == LM_PROT_XR;
			if (!inside || !executable)
			{
				g_pLog->warn(
					"Resolved target for '%s' is not executable module memory; "
					"refusing to use it\n", name.c_str());
				address = LM_ADDRESS_BAD;
				matchAddress = LM_ADDRESS_BAD;
			}
		}
	}
	const bool resolved = address != LM_ADDRESS_BAD;

	if (RuntimeAttestation::enabled())
	{
		const char* moduleName = module == &g_modSteamUI ? "steamui" : "steamclient";
		if (!resolved)
		{
			RuntimeAttestation::emit
			(
				"locator-missing",
				{
					RuntimeAttestation::Field::text("locator", name),
					RuntimeAttestation::Field::text("module", moduleName),
					RuntimeAttestation::Field::boolean("optional", optional),
				}
			);
		}
		else
		{
			lm_segment_t segment {};
			const bool executable = LM_FindSegment(address, &segment)
			                     && (segment.prot & LM_PROT_XR) == LM_PROT_XR;
			const bool insideModule = address >= targetModule.base
			                       && address < targetModule.base + targetModule.size;
			RuntimeAttestation::emit
			(
				"locator-resolved",
				{
					RuntimeAttestation::Field::text("locator", name),
					RuntimeAttestation::Field::text("module", moduleName),
					RuntimeAttestation::Field::number
					(
						"target_rva", insideModule ? address - targetModule.base : 0
					),
					RuntimeAttestation::Field::boolean("inside_module", insideModule),
					RuntimeAttestation::Field::boolean("executable", executable),
				}
			);
		}
	}

	return resolved;
}

static void autoResolveIpcFrameRoots()
{
	const std::array<Pattern_t*, 6> targets =
	{
		&Patterns::IClientApps::RunIPCFrame,
		&Patterns::IClientAppManager::RunIPCFrame,
		&Patterns::IClientRemoteStorage::RunIPCFrame,
		&Patterns::IClientUGC::RunIPCFrame,
		&Patterns::IClientUserStats::RunIPCFrame,
		&Patterns::IClientUser::RunIPCFrame,
	};
	std::array<bool, targets.size()> catalogResolved {};
	for (size_t i = 0; i < targets.size(); ++i)
		catalogResolved[i] = catalogAddress(*targets[i], false).has_value();

	struct Ctx
	{
		std::vector<IpcFrame::Cand> cands;
	} ctx;

	const auto enumSegments = [](lm_segment_t* seg, lm_void_t* arg) -> lm_bool_t
	{
		auto* c = reinterpret_cast<Ctx*>(arg);
		if ((seg->prot & LM_PROT_XR) != LM_PROT_XR)
			return LM_TRUE;

		if (seg->base + seg->size <= g_modSteamClient.base)
			return LM_TRUE;
		if (seg->base >= g_modSteamClient.base + g_modSteamClient.size)
			return LM_TRUE;

		const auto local = IpcFrame::scan(reinterpret_cast<const uint8_t*>(seg->base), seg->size);
		for (const auto& cand : local)
			c->cands.push_back({
				static_cast<size_t>(seg->base) + cand.offset,
				cand.root,
				cand.available,
			});
		return LM_TRUE;
	};
	if (!IpcFrame::scanWhenCatalogIncomplete(catalogResolved, [&]
	{
		LM_EnumSegments(enumSegments, &ctx);
	}))
	{
		return;
	}

	if (ctx.cands.empty())
	{
		g_pLog->debug("IpcFrame: no RunIPCFrame candidates found; keeping embedded roots\n");
		return;
	}

	for (size_t i = 0; i < targets.size(); ++i)
	{
		Pattern_t* p = targets[i];

		if (catalogResolved[i])
			continue;
		const uint32_t seed = IpcFrame::parseTrailingRoot(p->pattern);
		size_t idx = IpcFrame::resolveConfident(ctx.cands, seed, IpcFrame::kMaxRootDrift);

		const auto resolveByPivots =
			[&](const uint32_t* pivots, size_t count, size_t minMatches) -> size_t
		{
			size_t hit = SIZE_MAX;
			size_t qualifying = 0;
			for (size_t i = 0; i < ctx.cands.size(); ++i)
			{
				const auto& cand = ctx.cands[i];
				if (IpcFrame::countHighPivots(
					reinterpret_cast<const uint8_t*>(cand.offset), cand.available,
					pivots, count) >= minMatches)
				{
					hit = i;
					++qualifying;
				}
			}
			return (qualifying == 1) ? hit : SIZE_MAX;
		};

		if (idx == SIZE_MAX && p == &Patterns::IClientRemoteStorage::RunIPCFrame)
		{
			static constexpr uint32_t remoteStoragePivots[] =
			{
				0x396376, 0x5DB472, 0x84692E, 0x8694E9, 0x8712DD,
				0xC0F5C7, 0xDB2410, 0xE82BD7, 0xF5841E, 0xFD3CA9,
			};
			idx = resolveByPivots(
				remoteStoragePivots, std::size(remoteStoragePivots), 4);
		}
		if (idx == SIZE_MAX && p == &Patterns::IClientUserStats::RunIPCFrame)
		{
			static constexpr uint32_t userStatsPivots[] =
			{
				0x84EDDC, 0x85DE33, 0x85E5D6, 0xF7452C,
				0xF991AC, 0xF9B9E3, 0xFF7DFB, 0xFF94F2,
			};
			idx = resolveByPivots(
				userStatsPivots, std::size(userStatsPivots), 4);
		}
		if (idx == SIZE_MAX)
		{

			continue;
		}

		const uint32_t found = ctx.cands[idx].root;
		if (found != seed)
		{

			if (g_config.extendedLogging.get())
				g_pLog->debug(
					"IpcFrame: %s root drifted 0x%08X -> 0x%08X; auto-resolved\n",
					p->name.c_str(), seed, found);
			IpcFrame::setTrailingRoot(p->pattern, found);
		}
	}
}

bool Patterns::init()
{
	bool found = true;

	CAPIJob::GetPlayerStats.optional = true;

	CUser::MarkLicenseAsChanged.optional = true;
	CUser::ProcessPendingLicenseUpdates.optional = true;
	CUser::NotifyLicensesUpdated.optional = true;
	CUser::Offset_CompatManager.optional = true;
	CCMInterface::RecvPkt.optional = true;
	CAppInfoCache::GetOrAddAppData.optional = true;
	CAppInfoCache::ThreadedReadFromDisk.optional = true;
	CAppInfoCache::SkipFlagReference.optional = true;
	CAppInfoCache::ShaReference.optional = true;
	CDepotDownloadMgr::ProcessDepotManifest.optional = true;
	CDepotDownloadMgr::PrepareDepotDownload.optional = true;
	CDepotDownloadMgr::BuildDepotDependency.optional = true;
	CDepotDownloadMgr::EvaluateConfigChanges.optional = true;
	CDepotDownloadMgr::OnChunkUnpackedStack.optional = true;
	CDepotDownloadMgr::OnChunkUnpackedReg.optional = true;
	ParentalSignatureCheck.optional = true;
	ParentalSettingsReceived.optional = true;
	SteamUI::AppControllerRunFrame.optional = true;
	SteamUI::GetAppByID.optional = true;
	SteamUI::MarkAppChange.optional = true;
	SteamUI::BuildCompleteAppOverviewChange.optional = true;
	SteamUI::OwnershipFlagsReference.optional = true;

	loadActiveCatalogs();

#if !defined(__x86_64__)
	autoResolveIpcFrameRoots();
#endif
	for(auto& pattern : patterns())
	{
		if (!pattern->find())
		{
			if (pattern->optional)
			{

				g_pLog->warn
				(
					"Optional pattern '%s' not found; dependent feature disabled\n",
					pattern->name.c_str()
				);
				continue;
			}

			g_pLog->warn
			(
				"Required pattern '%s' not found\n",
				pattern->name.c_str()
			);
			found = false;
		}
	}

	if (found)
		writeLocalCatalogs();
	return found;
}

using SigFollowMode = MemHlp::SigFollowMode;

namespace Patterns
{
#if defined(__x86_64__)
	Pattern_t ParentalSignatureCheck
	{
		"ParentalSignatureCheck",
		"84 C0 75 68 8B 95 42 02 00 00 4C 8D A4 24 40 01 00 00 48 8D 35 ? ? ? ?",
		SigFollowMode::None
	};
	Pattern_t ParentalSettingsReceived
	{
		"ParentalSettingsReceived",
		"41 57 41 56 45 89 C6 41 55 49 89 F5 41 54 41 89 D4 55 48 89 FD",
		SigFollowMode::None
	};

	Pattern_t TraceIPC
	{
		"TraceIPC",
		"41 57 41 56 41 55 41 54 55 53 48 81 EC 28 01 00 00 48 89 3C 24",
		SigFollowMode::None
	};

	namespace SteamUI
	{
		Pattern_t AppControllerRunFrame
		{
			"SteamUI::AppControllerRunFrame",
			"41 57 41 56 41 55 41 54 55 53 48 83 EC 38 64 48 8B 1C 25 28 00 00 00 48 89 5C 24 28 48 89 FB E8 ? ? ? ? 66 0F EF C9",
			SigFollowMode::None,
			&g_modSteamUI,
			"Patterns::SteamUI::AppControllerRunFrame"
		};

		Pattern_t GetAppByID
		{
			"SteamUI::GetAppByID",
			"41 57 41 56 49 89 FE 41 55 41 54 55 53 48 83 EC 38 89 74 24 0C",
			SigFollowMode::None,
			&g_modSteamUI,
			"Patterns::SteamUI::GetAppByID"
		};

		Pattern_t MarkAppChange
		{
			"SteamUI::MarkAppChange",
			"41 57 41 89 F7 41 56 41 55 41 54 55 48 89 FD 53 48 83 EC 28",
			SigFollowMode::None,
			&g_modSteamUI,
			"Patterns::SteamUI::MarkAppChange"
		};

		Pattern_t BuildCompleteAppOverviewChange
		{
			"SteamUI::BuildCompleteAppOverviewChange",
			"41 57 41 56 41 55 41 54 55 48 89 F5 53 48 89 FB 48 81 EC D8 00 00 00 48 89 54 24 40",
			SigFollowMode::None,
			&g_modSteamUI,
			"Patterns::SteamUI::BuildCompleteAppOverviewChange"
		};

		Pattern_t OwnershipFlagsReference
		{
			"SteamUI::OwnershipFlagsReference",
			"41 8B 56 1C 48 8B BD B8 E6 FF FF 48 8D 35 ? ? ? ? E8 ? ? ? ?",
			SigFollowMode::None,
			&g_modSteamUI,
			"Patterns::SteamUI::OwnershipFlagsReference"
		};
	}

	namespace CAPIJob
	{
		Pattern_t GetPlayerStats
		{
			"CAPIJob::GetPlayerStats",
			"41 57 31 F6 41 56 41 55 41 54 55 53 48 81 EC 18 02 00 00 64 48 8B 1C 25 28 00 00 00",
			SigFollowMode::None
		};
	}

	namespace CCMInterface
	{
		Pattern_t RecvPkt
		{
			"CCMInterface::RecvPkt",
			"41 57 41 56 41 55 41 54 49 89 FC 55 53 48 89 F3 48 81 EC 28 05 00 00 48 8B 2D ? ? ? ? 64 48 8B 04 25 28 00 00 00",
			SigFollowMode::None,
			nullptr,
			"Patterns::CCMInterface::RecvPkt"
		};
	}

	namespace CProtoBufMsgBase
	{
		Pattern_t InitFromPacket
		{
			"CProtoBufMsgBase::InitFromPacket",
			"41 55 41 54 55 53 48 89 FB 48 83 EC 48 48 8B 05 ? ? ? ? 64 4C 8B 24 25 28 00 00 00",
			SigFollowMode::None
		};
		Pattern_t Send
		{
			"CProtoBufMsgBase::Send",
			"55 53 48 89 FB 48 83 EC 08 80 3D ? ? ? ? 00 48 8B 7F 30",
			SigFollowMode::None
		};
	};

	namespace CSteamEngine
	{
		Pattern_t GetServerPipe
		{
			"CSteamEngine::GetServerPipe",
			"85 F6 74 2C 0F B7 CE 31 D2 3B 4F 68 73 18 48 63 C1 48 C1 E0 04 48 03 47 48",
			SigFollowMode::None,
			nullptr,
			"Patterns::CSteamEngine::GetServerPipe"
		};
		Pattern_t Init
		{
			"CSteamEngine::Init",
			"41 54 55 53 48 81 EC 10 01 00 00 64 48 8B 04 25 28 00 00 00 48 89 84 24 08 01 00 00 8B 87 ? ? 00 00",
			SigFollowMode::None
		};
		Pattern_t SetAppIdForCurrentPipe
		{
			"CSteamEngine::SetAppIdForCurrentPipe",
			"41 57 41 56 41 55 49 89 FD 41 54 41 89 D4 55 53 48 83 EC 38",
			SigFollowMode::None
		};
		Pattern_t ProcessIPCFrame
		{
			"CSteamEngine::ProcessIPCFrame",
			"41 55 49 89 CD 41 54 41 89 F4 55 48 89 FD 48 89 D7 53 48 89 D3",
			SigFollowMode::None,
			nullptr,
			"Patterns::CSteamEngine::ProcessIPCFrame"
		};
		Pattern_t Offset_ClientUtils
		{
			"CSteamEngine::m_ClientUtils",
			"48 89 87 ? ? 00 00 48 8D 87 ? ? 00 00 48 89 C7 48 89 44 24 18",
			SigFollowMode::None,
			nullptr,
			"Patterns::CSteamEngine::Offset_ClientUtils"
		};
		Pattern_t Offset_User
		{
			"CSteamEngine::m_pUser",
			"48 8B 8D ? ? 00 00 48 89 D8 48 C1 E0 04 48 8B 7C 01 08 48 85 FF 75 D1 48 83 C3 01 39 DA 7F E7 48 83 C4 08 5B 5D C3 CC",
			SigFollowMode::None,
			nullptr,
			"Patterns::CSteamEngine::Offset_User"
		};
	}

	namespace CSteamMatchmakingServers
	{
		Pattern_t GetServerDetails
		{
			"CSteamMatchmakingServers::GetServerDetails",
			"41 56 41 55 41 54 55 89 F5 53 80 BF 88 01 00 00 00 48 89 FB",
			SigFollowMode::None
		};
		Pattern_t RequestInternetServerList
		{
			"CSteamMatchmakingServers::RequestInternetServerList",
			"55 48 89 E5 41 57 4C 8D 7F 68 41 56 41 55 45 89 C5 41 54 41 89 F4",
			SigFollowMode::None
		};
	}

	namespace CUser
	{
		Pattern_t Offset_CompatManager
		{
			"CUser::m_CompatManager",
			"48 8D 83 58 4B 00 00 48 89 DE 48 89 C7 48 89 84 24 F0 00 00 00",
			SigFollowMode::None,
			nullptr,
			"Patterns::CUser::Offset_CompatManager"
		};

		Pattern_t CheckAppOwnership
		{
			"CUser::CheckAppOwnership",
			"41 57 41 56 49 89 D6 41 55 41 54 55 53 48 89 FB 48 81 EC B8 00 00 00",
			SigFollowMode::None
		};

		Pattern_t GetSubscribedApps
		{
			"CUser::GetSubscribedApps",
			"41 57 41 56 41 55 41 54 55 53 48 83 EC 78 66 0F 6F 05 ? ? ? ?",
			SigFollowMode::None
		};
		Pattern_t PostCallback
		{
			"CSteamEngine::PostCallback",
			"E8 ? ? ? ? 48 81 C3 E8 23 00 00 48 89 DF E8 ? ? ? ? 84 C0",
			SigFollowMode::Relative,
			nullptr,
			"Patterns::CUser::PostCallback"
		};
		Pattern_t PostCallbackToAppId
		{
			"CUser::PostCallbackToAppId",
			"41 57 41 89 F7 41 56 41 55 45 89 C5 41 54 49 89 CC 55 89 D5",
			SigFollowMode::None
		};
		Pattern_t UpdateAppOwnershipTicket
		{
			"IClientUser::UpdateAppOwnershipTicket",
			"55 48 89 D5 48 89 CA 53 48 89 FB 48 83 EC 08 E8 ? ? ? ? 48 83 C4 08",
			SigFollowMode::None,
			nullptr,
			"Patterns::CUser::UpdateAppOwnershipTicket"
		};

		Pattern_t MarkLicenseAsChanged
		{
			"CUser::MarkLicenseAsChanged",
			"41 57 48 8D 87 ? ? 00 00 41 89 F7 41 56 41 55 41 54 55 53",
			SigFollowMode::None
		};

		Pattern_t ProcessPendingLicenseUpdates
		{
			"CUser::ProcessPendingLicenseUpdates",
			"41 54 49 89 FC 55 53 8B 9F C8 24 00 00 83 EB 01 78 66 48 63 C3 48 8D 2C 80 48 C1 E5 04",
			SigFollowMode::None
		};

		Pattern_t NotifyLicensesUpdated
		{
			"CUser::NotifyLicensesUpdated",
			"41 57 41 56 41 55 41 54 55 53 48 81 EC 08 02 00 00 44 8B 8F C4 23 00 00",
			SigFollowMode::None
		};
	}

	namespace CAppInfoCache
	{
		Pattern_t GetOrAddAppData
		{
			"CAppInfoCache::GetOrAddAppData",
			"41 57 41 56 41 55 41 54 49 89 FC 55 89 F5 53 48 83 EC 18 48 8B 05 ? ? ? ?",
			SigFollowMode::None
		};

		Pattern_t ThreadedReadFromDisk
		{
			"CAppInfoCache::ThreadedReadFromDisk",
			"55 48 89 E5 41 57 41 56 41 55 41 54 53 48 81 EC 88 11 00 00 48 89 BD 90 EE FF FF",
			SigFollowMode::None
		};

		Pattern_t SkipFlagReference
		{
			"CAppInfoCache::SkipFlagReference",
			"0F B6 40 14 74 2D 84 C0 75 18 8B 53 0C 85 D2 74 22 3B 53 34 72 1D",
			SigFollowMode::None
		};

		Pattern_t ShaReference
		{
			"CAppInfoCache::ShaReference",
			"48 39 4B 20 74 7B 48 8B 44 24 08 64 48 2B 04 25 28 00 00 00",
			SigFollowMode::None
		};
	}

	namespace IClientAppManager
	{
		Pattern_t RunIPCFrame
		{
			"IClientAppManager::RunIPCFrame",
			"55 48 89 E5 41 57 41 56 41 55 41 54 53 48 89 F3 48 8D B5 D8 FC FF FF 48 81 EC 38 03 00 00",
			SigFollowMode::None
		};
		Pattern_t BCanRemotePlayTogether
		{
			"IClientAppManager::BCanRemotePlayTogether",
			"41 55 41 54 49 89 FC 55 89 F5 53 48 83 EC 08 4C 8D 2D ? ? ? ?",
			SigFollowMode::None
		};
	}

	namespace IClientApps
	{
		Pattern_t RunIPCFrame
		{
			"IClientApps::RunIPCFrame",
			"55 48 89 E5 41 57 41 56 49 89 F6 41 55 48 8D B5 F0 FE FF FF 41 54 49 89 FC 4C 89 F7",
			SigFollowMode::None
		};
	}

	namespace IClientRemoteStorage
	{
		Pattern_t RunIPCFrame
		{
			"IClientRemoteStorage::RunIPCFrame",
			"55 48 89 E5 41 57 41 56 41 55 49 89 F5 41 54 48 8D B5 00 FF FF FF 53 48 81 EC 28 01 00 00",
			SigFollowMode::None
		};
	}

	namespace IClientUser
	{
		Pattern_t RunIPCFrame
		{
			"IClientUser::RunIPCFrame",
			"55 48 89 E5 41 57 41 56 49 89 FE 41 55 41 54 49 89 D4 BA 04 00 00 00 53 48 89 F3 48 81 EC 88 01 00 00",
			SigFollowMode::None
		};

		Pattern_t BLoggedOn
		{
			"IClientUser::BLoggedOn",
			"8B 47 48 C3 CC CC CC CC CC CC CC CC CC CC CC CC 8B 87 40 06 00 00",
			SigFollowMode::None
		};
		Pattern_t BUpdateAppOwnershipTicket
		{
			"IClientUser::BUpdateAppOwnershipTicket",
			"41 57 41 56 41 55 41 54 4C 8D 67 50 55 48 89 F5 31 F6 53 48 89 FB",
			SigFollowMode::None
		};
		Pattern_t GetAppOwnershipTicketExtendedData
		{
			"IClientUser::GetAppOwnershipTicketExtendedData",
			"41 56 41 55 49 89 D5 41 54 41 89 F4 55 48 89 FD 48 89 CF 53 48 83 EC 10",
			SigFollowMode::None
		};
		Pattern_t IsUserSubscribedAppInTicket
		{
			"IClientUser::IsUserSubscribedAppInTicket",
			"8B 87 1C 26 00 00 85 C0 0F 84 ? ? ? ? 41 57 49 89 FF 41 56 41 89 F6 41 55",
			SigFollowMode::None
		};
		Pattern_t RequiresLegacyCDKey
		{
			"IClientUser::RequiresLegacyCDKey",
			"41 55 49 89 FD 41 54 55 89 F5 53 48 81 EC 48 02 00 00 48 8D 05 ? ? ? ? 64 4C 8B 24 25 28 00 00 00",
			SigFollowMode::None
		};
	}

	namespace IClientUGC
	{
		Pattern_t RunIPCFrame
		{
			"IClientUGC::RunIPCFrame",
			"55 48 89 E5 41 57 41 56 49 89 FE 41 55 41 54 49 89 D4 BA 04 00 00 00 53 48 89 F3 48 81 EC 18 01 00 00",
			SigFollowMode::None
		};
	}

	namespace IClientUserStats
	{
		Pattern_t RunIPCFrame
		{
			"IClientUserStats::RunIPCFrame",
			"55 48 89 E5 41 57 41 56 41 55 49 89 FD 41 54 53 48 89 F3 48 81 EC D8 00 00 00",
			SigFollowMode::None
		};
	}

	namespace CPackageInfoCache
	{
		Pattern_t LoadPackage
		{
			"CPackageInfoCache::LoadPackage",
			"41 57 41 56 49 89 F6 41 55 41 54 55 53 48 89 FB 48 81 EC 18 01 00 00",
			SigFollowMode::None
		};
	}

	namespace CUtlMemory
	{
		Pattern_t Grow
		{
			"CUtlMemory::Grow",
			"E8 ? ? ? ? 8B 45 10 8D 70 01 EB 95 66 0F 1F 44 00 00 29 C6",
			SigFollowMode::Relative
		};
	}

	namespace CDepotDownloadMgr
	{
		Pattern_t ProcessDepotManifest
		{
			"CDepotDownloadMgr::ProcessDepotManifest",
			"41 57 41 56 41 89 CE 41 55 41 54 49 89 FC 55 53 4C 89 C3 48 83 EC 38",
			SigFollowMode::None
		};
		Pattern_t PrepareDepotDownload
		{
			"CDepotDownloadMgr::PrepareDepotDownload",
			"41 57 41 56 41 55 41 54 55 53 48 89 FB 48 83 EC 78 48 89 74 24 08",
			SigFollowMode::None
		};
		Pattern_t BuildDepotDependency
		{
			"CDepotDownloadMgr::BuildDepotDependency",
			"41 57 4D 89 CF 41 56 49 89 FE 41 55 41 54 55 53 48 89 D3 48 81 EC 88 02 00 00",
			SigFollowMode::None
		};
		Pattern_t OnChunkUnpackedStack
		{
			"CDepotDownloadMgr::OnChunkUnpackedStack",
			"55 48 89 E5 41 57 41 56 49 89 D6 41 55 41 89 CD 41 54 4D 89 C4",
			SigFollowMode::None,
			nullptr,
			"Patterns::CDepotDownloadMgr::OnChunkUnpackedStack"
		};
		Pattern_t OnChunkUnpackedReg
		{
			"CDepotDownloadMgr::OnChunkUnpackedReg",
			"55 48 89 E5 41 57 41 56 49 89 F6 41 55 49 89 D5 41 54 41 89 CC",
			SigFollowMode::None,
			nullptr,
			"Patterns::CDepotDownloadMgr::OnChunkUnpackedReg"
		};
		Pattern_t EvaluateConfigChanges
		{
			"CDepotDownloadMgr::EvaluateConfigChanges",
			"55 48 89 E5 41 57 41 56 41 55 41 54 53 48 81 EC 38 01 00 00 89 95 CC FE FF FF",
			SigFollowMode::None
		};
	}

	namespace IClientFriends
	{
		Pattern_t GetFriendGamePlayed
		{
			"IClientFriends::GetFriendGamePlayed",
			"48 89 F0 41 56 48 C1 E8 30 41 55 89 C2 41 54 83 E2 F0 55 80 FA A0",
			SigFollowMode::None
		};
	}

	namespace IClientUtils
	{
		Pattern_t RunIPCFrame
		{
			"IClientUtils::RunIPCFrame",
			"55 48 89 E5 41 57 41 56 41 55 41 54 49 89 FC 53 48 89 F3 48 81 EC F8 00 00 00",
			SigFollowMode::None
		};
		Pattern_t Offset_GetPipeIndex
		{
			"IClientUtils::m_PipeIndex",
			"8B B7 D8 EF FF FF 83 F8 FF 74 32 48 8B 8F ? ? 00 00 EB 0D",
			SigFollowMode::None,
			nullptr,
			"Patterns::IClientUtils::Offset_GetPipeIndex"
		};
	}

	namespace ISteamMatchmakingPingResponse
	{
		Pattern_t ServerResponded
		{
			"ISteamMatchmakingPingResponse::ServerResponded",
			"C7 47 08 FF FF FF FF 0F B7 0E 48 89 F8 48 89 F2 66 89 4F 0C",
			SigFollowMode::None,
			&g_modSteamUI
		};
	}

	namespace CWebSocketConnection
	{
		Pattern_t BBuildAndAsyncSendFrame
		{
			"CWebSocketConnection::BBuildAndAsyncSendFrame",
			"41 57 41 56 41 55 41 89 F5 41 54 49 89 FC 55 53 48 89 D3 48 81 EC C8 00 00 00",
			SigFollowMode::None
		};
	}

	namespace CRemoteClientManager
	{
		Pattern_t RecvPkt
		{
			"CRemoteClientManager::RecvPkt",
			"41 54 55 48 89 FD 53 48 83 EC 10 48 8B 05 ? ? ? ? 64 4C 8B 24 25 28 00 00 00 4C 89 64 24 08 49 89 F4 8B 10",
			SigFollowMode::None
		};
	}

	namespace CJobMgr
	{
		Pattern_t BRouteMsgToJob
		{
			"CJobMgr::BRouteMsgToJob",
			"55 48 89 E5 41 57 41 56 41 55 49 89 D5 41 54 53 48 81 EC 98 00 00 00 48 89 BD 70 FF FF FF",
			SigFollowMode::None
		};
	}

	namespace CDepotDownloadMgr
	{
		Pattern_t BYldRequestDepotManifest
		{
			"CDepotDownloadMgr::BYldRequestDepotManifest",
			"41 57 48 8D 41 FF 41 89 D7 41 56 41 55 41 54 4D 89 C4 55 48 89 CD",
			SigFollowMode::None
		};
	}
#else

	Pattern_t ParentalSignatureCheck
	{
		"ParentalSignatureCheck",
		"84 C0 75 27 8B 85 ? ? ? ? 8D 9D ? ? ? ? 83 EC 04 FF B0 ? ? 00 00 8D 86 ? ? ? ? 50 53 E8 ? ? ? ?",
		SigFollowMode::None
	};
	Pattern_t ParentalSettingsReceived
	{
		"ParentalSettingsReceived",
		"55 89 E5 57 56 E8 ? ? ? ? 81 C6 ? ? ? ? 53 81 EC 00 02 00 00 8B 45 08 8B 55 1C 8B 7D 0C",
		SigFollowMode::None
	};

	Pattern_t TraceIPC
	{
		"TraceIPC",
		"E8 ? ? ? ? 83 C4 10 85 FF 74 ? 8B 07 83 EC 04 FF B5 ? ? ? ? FF B5 ? ? ? ? 57 FF 10 83 C4 10 8D 45 ? 83 EC 04 89 F3 6A 04 50 FF 75",
		SigFollowMode::Relative
	};

	namespace SteamUI
	{

		Pattern_t AppControllerRunFrame
		{
			"CSteamUIAppController::RunFrame",
			"55 57 56 53 E8 ? ? ? ? 81 C3 ? ? ? ? 83 EC 7C E8 ? ? ? ? 8B 84 24 90 00 00 00 D9 5C 24 0C D9 44 24 0C",
			SigFollowMode::None,
			&g_modSteamUI,
			"Patterns::SteamUI::AppControllerRunFrame"
		};

		Pattern_t GetAppByID
		{
			"CSteamUIAppController::GetAppByID",

			"E8 ? ? ? ? 05 ? ? ? ? 55 89 E5 57 56 53 83 EC 4C 8B 5D 10 8B 75 08 89 45 C8 8B 80 ? ? 00 00",
			SigFollowMode::None,
			&g_modSteamUI,
			"Patterns::SteamUI::GetAppByID"
		};

		Pattern_t MarkAppChange
		{
			"CUpdateManager::MarkAppChange",

			"E8 ? ? ? ? 05 ? ? ? ? 55 89 E5 57 56 53 83 EC 3C 8B 75 0C 8B 5D 10 89 45 D4 8B 80 ? ? 00 00",
			SigFollowMode::None,
			&g_modSteamUI,
			"Patterns::SteamUI::MarkAppChange"
		};

		Pattern_t BuildCompleteAppOverviewChange
		{
			"BuildCompleteAppOverviewChange",
			"55 89 E5 57 56 53 E8 ? ? ? ? 81 C3 ? ? ? ? 81 EC 8C 00 00 00 8B 7D 08 89 5D A4 E8 ? ? ? ? 84 C0",
			SigFollowMode::None,
			&g_modSteamUI,
			"Patterns::SteamUI::BuildCompleteAppOverviewChange"
		};

		Pattern_t OwnershipFlagsReference
		{
			"CSteamApp::OwnershipFlagsReference",
			"8B ? 18 8B 9D ? ? ? ? 83 EC 04 50 8D 83 ? ? ? ? 50 FF B5 ? ? ? ? E8 ? ? ? ?",
			SigFollowMode::None,
			&g_modSteamUI,
			"Patterns::SteamUI::OwnershipFlagsReference"
		};
	}

	namespace CAPIJob
	{

		Pattern_t GetPlayerStats
		{
			"CAPIJob::GetPlayerStats",
			"55 89 E5 57 E8 ? ? ? ? 81 C7 ? ? ? ? 56 53 81 EC ? ? 00 00 8B 75 08 89 B5 ? ? FF FF 89 FB 65 A1 14 00 00 00 89 45 E4 31 C0 8D 85 ? ? FF FF 6A 00 50",
			SigFollowMode::None
		};
	}

	namespace CCMInterface
	{
		Pattern_t RecvPkt
		{
			"CCMInterface::RecvPkt",

			"55 89 E5 57 56 E8 ? ? ? ? 81 C6 ? ? ? ? 53 81 EC ? ? 00 00 8B 45 08 8B 7D 0C 89 85 ? ? FF FF 65 A1 14 00 00 00 89 45 E4 31 C0 8B 86",
			SigFollowMode::None,
			nullptr,
			"Patterns::CCMInterface::RecvPkt"
		};
	}

	namespace CProtoBufMsgBase
	{
		Pattern_t InitFromPacket
		{
			"CProtoBufMsgBase::InitFromPacket",
			"E8 ? ? ? ? 58 8B 45 ? 8B 8D",
			SigFollowMode::Relative
		};
		Pattern_t Send
		{
			"CProtoBufMsgBase::Send",
			"E8 ? ? ? ? 59 5A 50 56 E8 ? ? ? ? 83 C4 0C",
			SigFollowMode::Relative
		};
	};

	namespace CSteamEngine
	{
		Pattern_t GetServerPipe
		{
			"CSteamEngine::GetServerPipe",
			"0F B7 CA 31 C0 3B 4B",
			SigFollowMode::PrologueUpwards,
			std::vector<int16_t> { 0x53, 0x56, 0x57 },
			nullptr,
			"Patterns::CSteamEngine::GetServerPipe"
		};
		Pattern_t Init
		{
			"CSteamEngine::Init",
			"E8 ? ? ? ? 83 C4 10 8D 83 ? ? ? ? 83 EC 0C 89 AB",
			SigFollowMode::Relative
		};
		Pattern_t SetAppIdForCurrentPipe
		{
			"CSteamEngine::SetAppIdForCurrentPipe",
			"E8 ? ? ? ? E9 ? ? ? ? ? ? ? ? ? 8B 85 ? ? ? ? 83 EC 08 FF B5",
			SigFollowMode::Relative
		};
		Pattern_t ProcessIPCFrame
		{
			"CSteamEngine::ProcessIPCFrame",
			"5E C3 FF 74 24",
			SigFollowMode::PrologueUpwards,
			std::vector<int16_t> { 0xC3, 0x81, -1, -1, -1, -1, 0xE8, 0x53, 0x56 },
			nullptr,
			"Patterns::CSteamEngine::ProcessIPCFrame"
		};
		Pattern_t Offset_ClientUtils
		{
			"CSteamEngine::m_ClientUtils",
			"89 86 ? ? ? ? 8D 86 ? ? ? ? 89 44 24 ? 50 E8 ? ? ? ? 83 C4",
			SigFollowMode::None,
			nullptr,
			"Patterns::CSteamEngine::Offset_ClientUtils"
		};
		Pattern_t Offset_User
		{
			"CSteamEngine::m_pUser",
			"8B 80 ? ? ? ? FF 75 ? 8D 34",
			SigFollowMode::None,
			nullptr,
			"Patterns::CSteamEngine::Offset_User"
		};
	}

	namespace CSteamMatchmakingServers
	{
		Pattern_t GetServerDetails
		{
			"CSteamMatchmakingServers::GetServerDetails",
			"89 45 ? 83 C4 10 83 EC 0C 89 F3",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x56, 0x57, 0xe5, 0x89, 0x55 }
		};
		Pattern_t RequestInternetServerList
		{
			"CSteamMatchmakingServers::RequestInternetServerList",
			"C7 04 24 ? ? 00 00 E8 ? ? ? ? 5A 89 45 ? 59 FF B6 ? ? ? ? FF B6 ? ? ? ? FF B6 ? ? ? ? FF B6 ? ? ? ? FF B6 ? ? ? ? 6A 01",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0xe8, 0x57, 0xe5, 0x89, 0x55 }
		};
	}

	namespace CUser
	{

		Pattern_t Offset_CompatManager
		{
			"CUser::m_CompatManager",
			"8D 8E ? ? ? ? 58 56 51 89 8D ? ? ? ? E8 ? ? ? ? 58 5A C7 86 ? ? ? ? FF FF FF FF",
			SigFollowMode::None,
			nullptr,
			"Patterns::CUser::Offset_CompatManager"
		};
		Pattern_t CheckAppOwnership
		{
			"CUser::CheckAppOwnership",
			"E8 ? ? ? ? 88 45 ? 83 C4 10 84 C0 0F 84 ? ? ? ? 8B 45 ? 80 7D ? 00",
			SigFollowMode::Relative
		};

		Pattern_t GetSubscribedApps
		{
			"CUser::GetSubscribedApps",
			"55 89 E5 57 56 E8 ? ? ? ? 81 C6 ? ? ? ? 53 83 C4 80 0F B6 45 14 6A 00 6A 00 88 45 8F 8D 45 D0 66 0F 6F 86 ? ? ? ? 50 89 45 80 0F 11 45 BC 66 0F 6F 86 ? ? ? ? 89 75 94 0F 29 45 A8",
			SigFollowMode::None
		};
		Pattern_t PostCallback
		{
			"CSteamEngine::PostCallback",
			"E8 ? ? ? ? 8D 86 ? ? ? ? 83 C4 18 68 F6 01 00 00",
			SigFollowMode::Relative,
			nullptr,
			"Patterns::CUser::PostCallback"
		};
		Pattern_t PostCallbackToAppId
		{
			"CUser::PostCallbackToAppId",
			"84 C0 0F 45 F8 89 F8",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0xE8, 0x53, 0x56, 0x57, 0x55 }
		};
		Pattern_t UpdateAppOwnershipTicket
		{
			"IClientUser::UpdateAppOwnershipTicket",
			"E8 ? ? ? ? E9 ? ? ? ? ? ? ? ? ? ? 8D 45 ? 89 45 ? EB",
			SigFollowMode::Relative,
			nullptr,
			"Patterns::CUser::UpdateAppOwnershipTicket"
		};

		Pattern_t MarkLicenseAsChanged
		{
			"CUser::MarkLicenseAsChanged",
			"55 57 56 53 E8 ? ? ? ? 81 C3 ? ? ? ? 83 EC 2C 8B 74 24 40 8B 44 24 48 8B BE ? ? 00 00 88 44 24 1C 8D 86 ? ? 00 00 89 44 24 0C 85 FF 0F 84 ? ? ? ?",
			SigFollowMode::None
		};

		Pattern_t ProcessPendingLicenseUpdates
		{
			"CUser::ProcessPendingLicenseUpdates",

			"55 57 56 53 E8 ? ? ? ? 81 C3 ? ? ? ? 83 EC 2C 8B 44 24 40 8B 88 ? 1C 00 00 85 C9 0F 8E ? ? ? ? 05 ? 1C 00 00 89 44 24 18 8D 83 ? ? ? ? 8B 30",
			SigFollowMode::None
		};

		Pattern_t NotifyLicensesUpdated
		{
			"CUser::NotifyLicensesUpdated",
			"55 89 E5 57 56 53 E8 ? ? ? ? 81 C3 ? ? ? ? 81 EC BC 01 00 00 8B 45 08 8B ? ? 1B 00 00 89 9D 54 FE FF FF 85 ? 0F 8E ? ? ? ? 8D 83 ? ? ? ? 83 EC 08 6A 7D 50",
			SigFollowMode::None
		};
	}

	namespace CAppInfoCache
	{

		Pattern_t GetOrAddAppData
		{
			"CAppInfoCache::GetOrAddAppData",
			"E8 ? ? ? ? 05 ? ? ? ? 55 89 E5 57 56 53 83 EC 2C 8B 75 0C 8B 7D 10 89 45 D0 8B 80 ? ? 00 00 8B 00 85 C0 0F 85 ? ? ? ?",
			SigFollowMode::None
		};

		Pattern_t ThreadedReadFromDisk
		{
			"CAppInfoCache::ThreadedReadFromDisk",
			"55 89 E5 57 56 E8 ? ? ? ? 81 C6 ? ? ? ? 53 81 EC 0C 11 00 00 8B 45 08 89 85 0C EF FF FF",
			SigFollowMode::None
		};

		Pattern_t SkipFlagReference
		{
			"CAppInfoCache::SkipFlagReference",
			"80 7E 10 00 0F 44 C8 88 4C 24 1E 8B 44 24 10 83 C7 01 3B 7D 44 8B 30 7C 9E",
			SigFollowMode::None
		};

		Pattern_t ShaReference
		{
			"CAppInfoCache::ShaReference",
			"8D 40 1C 50 E8 ? ? ? ? 83 C4 10 85 C0 75 10 0F B6 4C 24 1E 80 7E 10 00 0F 44 C8 88 4C 24 1E",
			SigFollowMode::None
		};
	}

	namespace IClientAppManager
	{
		Pattern_t RunIPCFrame
		{
			"IClientAppManager::RunIPCFrame",
			"E8 ? ? ? ? 8B 85 ? ? ? ? 83 C4 10 3D B7 85 0A 7A",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x56, 0x57, 0xe5, 0x89, 0x55 }
		};
		Pattern_t BCanRemotePlayTogether
		{
			"IClientAppManager::BCanRemotePlayTogether",
			"58 5A FF 74 24 ? 56 E8 ? ? ? ? 83 C4 10 85 C0 74",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0xe8, 0x53, 0x56, 0x57 }
		};
	}

	namespace IClientApps
	{
		Pattern_t RunIPCFrame
		{
			"IClientApps::RunIPCFrame",
			"E8 ? ? ? ? 8B 85 ? ? ? ? 83 C4 10 3D 37 9C 88 A6",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x56, 0x57, 0xe5, 0x89, 0x55 }
		};
	}

	namespace IClientRemoteStorage
	{
		Pattern_t RunIPCFrame
		{
			"IClientRemoteStorage::RunIPCFrame",

			"E8 ? ? ? ? 8B 85 ? ? ? ? 83 C4 10 3D 65 DD 12 87",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x56, 0x57, 0xe5, 0x89, 0x55 }
		};
	}

	namespace IClientUser
	{
		Pattern_t RunIPCFrame
		{
			"IClientUser::RunIPCFrame",
			"E8 ? ? ? ? 8B 85 ? ? ? ? 83 C4 10 3D 10 A3 86 73",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x56, 0x57, 0xe5, 0x89, 0x55 }
		};

		Pattern_t BLoggedOn
		{
			"IClientUser::BLoggedOn",
			"E9 ? ? ? ? ? ? ? ? ? ? 5B 5E 5F FF E0",
			SigFollowMode::Relative
		};
		Pattern_t BUpdateAppOwnershipTicket
		{
			"IClientUser::BUpdateAppOwnershipTicket",
			"83 EC 0C 89 F3 8B 7D ? FF 30 E8 ? ? ? ? 83 C4 10 83 FF 01 77 ? 84 C0 75 ? 80 7D ? 00 74 ? 80 7D ? 00 0F 84 ? ? ? ? 8D 65",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x56, 0x57, 0xe5, 0x89, 0x55 }
		};
		Pattern_t GetAppOwnershipTicketExtendedData
		{
			"IClientUser::GetAppOwnershipTicketExtendedData",
			"83 EC 24 FF 74 24 ? 8B 44 24",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x53, 0x56, 0x57, 0x55 }
		};
		Pattern_t IsUserSubscribedAppInTicket
		{
			"IClientUser::IsUserSubscribedAppInTicket",
			"E8 ? ? ? ? 89 C3 83 C4 20 8B ? ? ? ? ? 8B",
			SigFollowMode::Relative
		};

		Pattern_t RequiresLegacyCDKey
		{
			"IClientUser::RequiresLegacyCDKey",
			"75 ? 83 C4 1C 31 C0 5B 5E 5F 5D C3 ? ? ? ? ? 8B 44 24 ? 83 C4 1C 89 F9 89 F2 5B 5E 5F 5D 2D ? ? 00 00",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x53, 0x56, 0x57, 0x55 }
		};
	}

	namespace IClientUGC
	{
		Pattern_t RunIPCFrame
		{
			"IClientUGC::RunIPCFrame",
			"E8 ? ? ? ? 8B 85 ? ? ? ? 83 C4 10 3D 62 0C D2 71",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x56, 0x57, 0xe5, 0x89, 0x55 }
		};
	}

	namespace IClientUserStats
	{
		Pattern_t RunIPCFrame
		{
			"IClientUserStats::RunIPCFrame",
			"E8 ? ? ? ? 8B 85 ? ? ? ? 83 C4 10 3D 8F 65 6D 87",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x56, 0x57, 0xe5, 0x89, 0x55 }
		};
	}

	namespace CPackageInfoCache
	{
		Pattern_t LoadPackage
		{
			"CPackageInfoCache::LoadPackage",
			"E8 ? ? ? ? 83 C4 10 84 C0 0F 84 ? ? ? ? 8B 95 ? ? FF FF 8B 7A 18 83 FF FF",
			SigFollowMode::Relative
		};
	}

	namespace CUtlMemory
	{
		Pattern_t Grow
		{
			"CUtlMemory::Grow",
			"E8 ? ? ? ? 8B 85 ? ? FF FF 83 C4 10 8B 40 44 89 85 ? ? FF FF 83 C0 01 E9",
			SigFollowMode::Relative
		};
	}

	namespace CDepotDownloadMgr
	{

		Pattern_t ProcessDepotManifest
		{
			"CDepotDownloadMgr::ProcessDepotManifest",
			"E8 ? ? ? ? 05 ? ? ? ? 55 89 E5 57 56 53 83 EC 4C 8B 55 1C 89 45 C0 8B 45 18 89 55 CC 89 45 C8",
			SigFollowMode::None
		};

		Pattern_t PrepareDepotDownload
		{
			"CDepotDownloadMgr::PrepareDepotDownload",
			"55 89 E5 57 56 E8 ? ? ? ? 81 C6 ? ? ? ? 53 83 EC 60 8B 7D 08 8B 45 18 8B 55 1C FF 75 20 89 45 98 52 50 FF 75 14 89 55 9C FF 75 10 FF 75 0C 57 E8 ? ? ? ? 8B 47 4C 83 C4 20 83 F8 FF",
			SigFollowMode::None
		};

		Pattern_t BuildDepotDependency
		{
			"CDepotDownloadMgr::BuildDepotDependency",
			"E8 ? ? ? ? 05 ? ? ? ? 55 89 E5 57 56 53 81 EC 3C 04 00 00 8B 5D 08 8B 75 14 89 85 ? ? FF FF 8B 45 0C",
			SigFollowMode::None
		};

		Pattern_t OnChunkUnpackedStack
		{
			"CDepotDownloadMgr::OnChunkUnpacked[cdecl]",
			"55 89 E5 57 56 E8 ? ? ? ? 81 C6 ? ? ? ? 53 81 EC 7C 04 00 00 8B 45 0C 8B 7D 08 89 85 90 FB FF FF 8B 45 10 89 85 8C FB FF FF",
			SigFollowMode::None,
			nullptr,
			"Patterns::CDepotDownloadMgr::OnChunkUnpackedStack"
		};

		Pattern_t OnChunkUnpackedReg
		{
			"CDepotDownloadMgr::OnChunkUnpacked[regparm3]",
			"55 89 E5 57 E8 ? ? ? ? 81 C7 ? ? ? ? 56 89 C6 53 81 EC 7C 04 00 00 8B 45 0C 89 95 90 FB FF FF 89 8D 8C FB FF FF 89 85 94 FB FF FF",
			SigFollowMode::None,
			nullptr,
			"Patterns::CDepotDownloadMgr::OnChunkUnpackedReg"
		};

		Pattern_t EvaluateConfigChanges
		{
			"CDepotDownloadMgr::EvaluateConfigChanges",
			"55 89 E5 57 56 53 81 EC DC 00 00 00 89 85 50 FF FF FF 8B 45 10 89 85 40 FF FF FF 8B 45 08 8B 40 04",
			SigFollowMode::None
		};
	}

	namespace IClientFriends
	{
		Pattern_t GetFriendGamePlayed
		{
			"IClientFriends::GetFriendGamePlayed",
			"C7 45 ? A2 F1 5B 33 6A 04 50 57 E8 ? ? ? ?",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x56, 0x57, 0xE5, 0x89, 0x55 }
		};
	}

	namespace IClientUtils
	{
		Pattern_t RunIPCFrame
		{
			"IClientUtils::RunIPCFrame",
			"83 EC 08 89 F3 50 57 E8 ? ? ? ? 58 FF B5 ? ? ? ? E8 ? ? ? ? 58 8D 45",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x56, 0x57, 0xe5, 0x89, 0x55 }
		};
		Pattern_t Offset_GetPipeIndex
		{
			"IClientUtils::m_PipeIndex",
			"8B 91 ? ? ? ? 83 F8 FF 74 ? 8B 89 ? ? ? ? EB ? ? ? ? 8B 00 83 F8 FF 74 ? 8D 04 ? 8D 04 ? 3B 50",
			SigFollowMode::None,
			nullptr,
			"Patterns::IClientUtils::Offset_GetPipeIndex"
		};
	}

	namespace ISteamMatchmakingPingResponse
	{
		Pattern_t ServerResponded
		{
			"ISteamMatchmakingPingResponse::ServerResponded",
			"8B 85 ? ? ? ? 8B 40 ? 85 C0 0F 84 ? ? ? ? 39 46",
			SigFollowMode::PrologueUpwards,
			std::vector<uint8_t> { 0x57, 0xe5, 0x89, 0x55 },
			&g_modSteamUI
		};
	}

	namespace CWebSocketConnection
	{
		Pattern_t BBuildAndAsyncSendFrame
		{
			"CWebSocketConnection::BBuildAndAsyncSendFrame",
			"55 89 E5 57 E8 ? ? ? ? 81 C7 ? ? ? ? 56 53 81 EC AC 00 00 00 "
			"8B 45 10 8B 55 08 89 85 ? ? FF FF 89 95 ? ? FF FF "
			"65 A1 14 00 00 00",
			SigFollowMode::None
		};
	}

	namespace CRemoteClientManager
	{
		Pattern_t RecvPkt
		{
			"CRemoteClientManager::RecvPkt",
			"55 89 E5 57 56 E8 ? ? ? ? 81 C6 ? ? ? ? 53 83 EC 1C "

			"8B 86 ? ? 00 00 8B 00 85 C0 0F 85 ? ? ? ? "
			"C7 45 E4 00 00 00 00 83 EC 08 89 F3 6A 01 FF 75 0C "
			"E8 ? ? ? ? 89 C7 83 C4 10 85 C0 0F 84 ? ? ? ? 83 EC 0C 50",
			SigFollowMode::None
		};
	}

	namespace CJobMgr
	{
		Pattern_t BRouteMsgToJob
		{
			"CJobMgr::BRouteMsgToJob",
			"55 89 E5 57 56 E8 ? ? ? ? 81 C6 ? ? ? ? 53 83 EC 7C "
			"8B 45 08 8B 4D 14 89 45 90 8B 45 0C 89 4D A0 89 45 88 "
			"8B 45 10 89 45 A4 65 8B 0D 14 00 00",
			SigFollowMode::None
		};
	}

	namespace CDepotDownloadMgr
	{
		Pattern_t BYldRequestDepotManifest
		{
			"CDepotDownloadMgr::BYldRequestDepotManifest",
			"55 B9 FD FF FF FF 89 E5 57 E8 ? ? ? ? 81 C7 ? ? ? ? 56 53 83 EC 7C 8B 45 14 8B 55 18",
			SigFollowMode::None
		};
	}
#endif

	std::vector<Pattern_t*>& patterns()
	{

		static std::vector<Pattern_t*> instance;
		return instance;
	}
}

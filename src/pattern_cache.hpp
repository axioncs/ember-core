#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace PatternCache
{
	inline constexpr std::uint32_t kSchema = 1;
	inline constexpr std::size_t kMaximumBodySize = 64 * 1024;
	inline constexpr std::size_t kMaximumLocators = 128;

	struct ModuleIdentity
	{
		std::string component;
		std::string moduleName;
		std::string gnuBuildId;
		std::uint64_t size = 0;
		std::int64_t mtimeSeconds = 0;
		std::int64_t mtimeNanoseconds = 0;
	};

	struct Locator
	{
		std::string symbol;
		std::uint64_t targetRva = 0;
		std::uint64_t matchRva = 0;
		std::string signature;
		std::string followMode;
		bool required = true;
	};

	struct Catalog
	{
		ModuleIdentity identity;
		std::vector<Locator> locators;

		const Locator* entry(std::string_view symbol) const noexcept;
	};

	struct CompiledLocator
	{
		std::string_view symbol;
		bool required = true;
		std::string_view signature;
		std::string_view followMode;
	};

	bool identityMatches(
		const ModuleIdentity& cached,
		const ModuleIdentity& current
	) noexcept;

	std::optional<std::size_t> signatureSize(std::string_view signature) noexcept;
	bool signatureMatches(
		std::string_view signature,
		std::span<const std::uint8_t> bytes
	) noexcept;

	bool targetRvaMatches(
		std::string_view followMode,
		std::uint64_t matchRva,
		std::uint64_t cachedTargetRva,
		std::uint64_t derivedTargetRva
	) noexcept;

	bool policyMatches(
		const Catalog& catalog,
		std::span<const CompiledLocator> compiled
	) noexcept;
	bool validate(
		const Catalog& catalog,
		const ModuleIdentity& current,
		std::span<const CompiledLocator> compiled,
		std::span<const std::uint8_t> moduleBytes
	) noexcept;

	std::string serialize(const Catalog& catalog);
	std::optional<Catalog> parse(
		std::string_view body,
		std::string* error = nullptr
	);

	std::optional<Catalog> load(
		const std::filesystem::path& path,
		const ModuleIdentity& current,
		std::string* error = nullptr
	);

	bool writeAtomic(
		const std::filesystem::path& path,
		std::string_view body
	);

	bool enabled(bool configEnabled, const char* environmentOverride) noexcept;
}



#include "feats/compatlive.hpp"
#include "feats/libraryremoval_policy.hpp"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace
{
int failures = 0;

void expect(bool condition, std::string_view message)
{
	if (condition)
		return;
	std::cerr << "FAIL: " << message << '\n';
	++failures;
}

struct Byte
{
	std::uint8_t value;
	bool wildcard;
};

std::vector<Byte> parse(std::string_view pattern)
{
	std::vector<Byte> bytes;
	std::size_t cursor = 0;
	while (cursor < pattern.size())
	{
		while (cursor < pattern.size() && pattern[cursor] == ' ')
			++cursor;
		if (cursor >= pattern.size())
			break;
		const std::size_t end = pattern.find(' ', cursor);
		const std::string token(pattern.substr(
			cursor, end == std::string_view::npos ? end : end - cursor));
		bytes.push_back({
			static_cast<std::uint8_t>(
				token == "?" ? 0 : std::stoul(token, nullptr, 16)),
			token == "?",
		});
		cursor = end == std::string_view::npos ? pattern.size() : end;
	}
	return bytes;
}

std::vector<std::size_t> findMatches(
	const std::string& image, const std::vector<Byte>& pattern)
{
	std::vector<std::size_t> matches;
	if (pattern.empty() || image.size() < pattern.size())
		return matches;
	for (std::size_t offset = 0; offset + pattern.size() <= image.size(); ++offset)
	{
		bool match = true;
		for (std::size_t index = 0; index < pattern.size(); ++index)
		{
			if (!pattern[index].wildcard
			    && static_cast<std::uint8_t>(image[offset + index])
			           != pattern[index].value)
			{
				match = false;
				break;
			}
		}
		if (match)
			matches.push_back(offset);
	}
	return matches;
}

std::string readFile(const char* path)
{
	std::ifstream input(path, std::ios::binary);
	if (!input)
	{
		std::cerr << "could not open " << path << '\n';
		std::exit(2);
	}
	return std::string{std::istreambuf_iterator<char>(input),
	                   std::istreambuf_iterator<char>()};
}

std::size_t resolveUnique(
	const std::string& image,
	std::string_view name,
	std::string_view signature,
	const std::string& source)
{
	expect(source.find(signature) != std::string::npos,
	       std::string(name) + ": signature is not the one compiled in "
	                           "src/patterns.cpp");
	const auto matches = findMatches(image, parse(signature));
	if (matches.size() == 1)
		return matches.front();
	std::cerr << "FAIL: " << name << ": expected exactly one match, found "
	          << matches.size() << '\n';
	++failures;
	return std::string::npos;
}
}

int main(int argc, char** argv)
{
	if (argc < 3 || argc > 4)
	{
		std::cerr << "usage: test_optional_locators <steamclient.so> "
		             "<steamui.so> [src/patterns.cpp]\n";
		return 2;
	}
	const std::string client = readFile(argv[1]);
	const std::string ui = readFile(argv[2]);
	const std::string source = readFile(argc == 4 ? argv[3] : "src/patterns.cpp");

#if defined(__x86_64__)
	(void)resolveUnique(
		client, "CUser::ProcessPendingLicenseUpdates",
		"41 54 49 89 FC 55 53 8B 9F C8 24 00 00 83 EB 01 78 66 48 63 C3 48 8D 2C 80 48 C1 E5 04",
		source);

	(void)resolveUnique(
		client, "CCMInterface::RecvPkt",
		"41 57 41 56 41 55 41 54 49 89 FC 55 53 48 89 F3 48 81 EC 28 05 00 00 48 8B 2D ? ? ? ? 64 48 8B 04 25 28 00 00 00",
		source);

	const std::size_t ownership = resolveUnique(
		ui, "CSteamApp::OwnershipFlagsReference",
		"41 8B 56 1C 48 8B BD B8 E6 FF FF 48 8D 35 ? ? ? ? E8 ? ? ? ?",
		source);
	if (ownership != std::string::npos)
	{
		const auto* bytes =
			reinterpret_cast<const std::uint8_t*>(ui.data()) + ownership;
		const auto offset = LibraryRemovalPolicy::deriveOwnershipOffset(
			std::span<const std::uint8_t>(bytes, 4));
		expect(offset.has_value(),
		       "CSteamApp::OwnershipFlagsReference: the matched instruction does "
		       "not decode to a usable ownership-flags offset");
		if (offset)
		{
			std::cout << "CSteamApp::OwnershipFlags offset = 0x" << std::hex
			          << *offset << std::dec << '\n';
		}
	}

	const std::size_t parental = resolveUnique(
		client, "ParentalSignatureCheck",
		"84 C0 75 68 8B 95 42 02 00 00 4C 8D A4 24 40 01 00 00 48 8D 35 ? ? ? ?",
		source);
	if (parental != std::string::npos)
	{
		expect(static_cast<std::uint8_t>(client[parental + 2]) == 0x75,
		       "ParentalSignatureCheck: the patched byte is not the jne");
	}

	(void)resolveUnique(
		client, "CAPIJob::GetPlayerStats",
		"41 57 31 F6 41 56 41 55 41 54 55 53 48 81 EC 18 02 00 00 64 48 8B 1C 25 28 00 00 00",
		source);

	const std::size_t compat = resolveUnique(
		client, "CUser::m_CompatManager",
		"48 8D 83 58 4B 00 00 48 89 DE 48 89 C7 48 89 84 24 F0 00 00 00",
		source);
	if (compat != std::string::npos)
	{
		const auto* bytes =
			reinterpret_cast<const std::uint8_t*>(client.data()) + compat;
		expect(CompatLive::decodeManagerOffset(
			       std::span<const std::uint8_t>(bytes, 7)).has_value(),
		       "CUser::m_CompatManager: the matched LEA does not decode");
	}

	(void)resolveUnique(
		client, "CDepotDownloadMgr::BuildDepotDependency",
		"41 57 4D 89 CF 41 56 49 89 FE 41 55 41 54 55 53 48 89 D3 48 81 EC 88 02 00 00",
		source);
#else

	(void)resolveUnique(
		client, "CUser::ProcessPendingLicenseUpdates",
		"55 57 56 53 E8 ? ? ? ? 81 C3 ? ? ? ? 83 EC 2C 8B 44 24 40 8B 88 ? 1C "
		"00 00 85 C9 0F 8E ? ? ? ? 05 ? 1C 00 00 89 44 24 18 8D 83 ? ? ? ? 8B 30",
		source);

	(void)resolveUnique(
		client, "CCMInterface::RecvPkt",
		"55 89 E5 57 56 E8 ? ? ? ? 81 C6 ? ? ? ? 53 81 EC ? ? 00 00 8B 45 08 "
		"8B 7D 0C 89 85 ? ? FF FF 65 A1 14 00 00 00 89 45 E4 31 C0 8B 86",
		source);

	const std::size_t ownership = resolveUnique(
		ui, "CSteamApp::OwnershipFlagsReference",
		"8B ? 18 8B 9D ? ? ? ? 83 EC 04 50 8D 83 ? ? ? ? 50 FF B5 ? ? ? ? "
		"E8 ? ? ? ?",
		source);
	if (ownership != std::string::npos)
	{
		const auto* bytes =
			reinterpret_cast<const std::uint8_t*>(ui.data()) + ownership;
		const auto offset = LibraryRemovalPolicy::deriveOwnershipOffset(
			std::span<const std::uint8_t>(bytes, 3));
		expect(offset.has_value(),
		       "CSteamApp::OwnershipFlagsReference: the matched instruction does "
		       "not decode to a usable ownership-flags offset");
		if (offset)
		{
			std::cout << "CSteamApp::OwnershipFlags offset = 0x" << std::hex
			          << *offset << std::dec << '\n';
		}
	}

	const std::size_t parental = resolveUnique(
		client, "ParentalSignatureCheck",
		"84 C0 75 27 8B 85 ? ? ? ? 8D 9D ? ? ? ? 83 EC 04 FF B0 ? ? 00 00 "
		"8D 86 ? ? ? ? 50 53 E8 ? ? ? ?",
		source);
	if (parental != std::string::npos)
	{
		expect(static_cast<std::uint8_t>(client[parental + 2]) == 0x75,
		       "ParentalSignatureCheck: the patched byte is not the jne");
	}

	(void)resolveUnique(
		client, "CAPIJob::GetPlayerStats",
		"55 89 E5 57 E8 ? ? ? ? 81 C7 ? ? ? ? 56 53 81 EC ? ? 00 00 8B 75 08 "
		"89 B5 ? ? FF FF 89 FB 65 A1 14 00 00 00 89 45 E4 31 C0 8D 85 ? ? FF "
		"FF 6A 00 50",
		source);

	const std::size_t compat = resolveUnique(
		client, "CUser::m_CompatManager",
		"8D 8E ? ? ? ? 58 56 51 89 8D ? ? ? ? E8 ? ? ? ? 58 5A C7 86 ? ? ? ? "
		"FF FF FF FF",
		source);
	if (compat != std::string::npos)
	{
		const auto* bytes =
			reinterpret_cast<const std::uint8_t*>(client.data()) + compat;
		expect(CompatLive::decodeManagerOffset(
			       std::span<const std::uint8_t>(bytes, 6)).has_value(),
		       "CUser::m_CompatManager: the matched LEA does not decode");
	}

	(void)resolveUnique(
		client, "CDepotDownloadMgr::BuildDepotDependency",
		"E8 ? ? ? ? 05 ? ? ? ? 55 89 E5 57 56 53 81 EC 3C 04 00 00 8B 5D 08 "
		"8B 75 14 89 85 ? ? FF FF 8B 45 0C",
		source);
#endif

	if (failures != 0)
	{
		std::cerr << failures << " optional locator check(s) failed\n";
		return 1;
	}
	std::cout << "optional runtime-capability locators resolve uniquely\n";
	return 0;
}

#include "memhlp.hpp"

#include "log.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "pattern_scan.hpp"

#include "libmem/libmem.h"

#include <map>
#include <optional>
#include <vector>

std::vector<int16_t> MemHlp::patternToBytes(const char* pattern)
{
	auto bytes = std::vector<int16_t>();

	char* start = const_cast<char*>(pattern);
	char* end = start + strlen(pattern);

	while (start < end)
	{
		if (*start == '?')
		{
			bytes.emplace_back(-1);
		}
		else if (*start != ' ')
		{
			bytes.emplace_back(std::strtoul(start, &start, 16));
		}

		start++;
	}

	return bytes;
}

lm_address_t MemHlp::patternScan(const char* pattern, lm_module_t targetModule,
	std::size_t* matchesOut, std::vector<uintptr_t>* allMatches)
{
	const auto bytes = patternToBytes(pattern);

	auto codeSegments = std::map<lm_address_t, lm_address_t>();
	const static auto enumSegments = [](lm_segment_t* seg, lm_void_t* arg) -> lm_bool_t
	{
		auto rSegments = reinterpret_cast<std::map<lm_address_t, lm_address_t>*>(arg);

		if((seg->prot & LM_PROT_XR) == LM_PROT_XR)
		{
			(*rSegments)[seg->base] = seg->base + seg->size;

		}

		return LM_TRUE;
	};

	LM_EnumSegments(enumSegments, &codeSegments);

	if (bytes.empty())
	{
		return LM_ADDRESS_BAD;
	}

	PatternScanTotal total;
	for(const auto& itm : codeSegments)
	{
		if (targetModule.base > itm.second)
		{
			continue;
		}
		if (targetModule.base + targetModule.size < itm.first)
		{
			continue;
		}

		accumulateScan(total,
			scanPatternRange(bytes, itm.first, itm.second, true,
				allMatches, kMaxConvergenceCandidates));
	}

	if (matchesOut != nullptr)
	{
		*matchesOut = total.matches;
	}
	if (!total.resolved())
	{
		return LM_ADDRESS_BAD;
	}
	return static_cast<lm_address_t>(total.address);
}

lm_address_t MemHlp::patternScan(const char* pattern, lm_module_t targetModule)
{
	return patternScan(pattern, targetModule, nullptr, nullptr);
}

MemHlp::SignatureSearchResult MemHlp::searchSignatureDetailed(
	const char* name,
	const char* signature,
	lm_module_t module,
	SigFollowMode mode,
	const int16_t* extraData,
	size_t extraDataSize
)
{

	SignatureSearchResult result;
	std::size_t matches = 0;
	std::vector<uintptr_t> allMatches;
	result.match = patternScan(signature, module, &matches, &allMatches);
	result.target = result.match;
	result.matches = matches;
	if (matches > 1)
	{

		std::optional<lm_address_t> converged;
		if (mode == SigFollowMode::Relative && matches <= kMaxConvergenceCandidates
		    && allMatches.size() == matches)
		{
			std::vector<uintptr_t> targets;
			targets.reserve(allMatches.size());
			for (const uintptr_t candidate : allMatches)
			{
				const lm_address_t followed =
					MemHlp::getJmpTarget(static_cast<lm_address_t>(candidate));
				if (followed == LM_ADDRESS_BAD)
				{
					targets.clear();
					break;
				}
				targets.push_back(static_cast<uintptr_t>(followed));
			}
			if (const auto single = convergedTarget(targets))
			{
				converged = static_cast<lm_address_t>(*single);
			}
		}

		if (converged)
		{

			result.match = static_cast<lm_address_t>(allMatches.front());
			result.target = *converged;
			g_pLog->info("Signature for '%s' matched %zu call sites converging on %p\n",
			             name, matches, result.target);
			return result;
		}

		g_pLog->warn("Signature for '%s' matched %zu times without converging; "
		             "refusing to resolve it\n", name, matches);
		result.match = LM_ADDRESS_BAD;
		result.target = LM_ADDRESS_BAD;
		return result;
	}
	if (result.match == LM_ADDRESS_BAD)
	{
		g_pLog->debug("Unable to find signature for %s!\n", name);
	}
	else
	{
		switch (mode)
		{
			case SigFollowMode::Relative:
				g_pLog->debug("Resolving relative of %s at %p\n", name, result.match);
				result.target = MemHlp::getJmpTarget(result.match);
				break;

			case SigFollowMode::PrologueUpwards:
				g_pLog->debug("Searching function prologue of %s from %p\n", name, result.match);
				{
					lm_segment_t matchSegment{};
					if (!LM_FindSegment(result.match, &matchSegment))
					{
						g_pLog->warn("Unable to find matched segment for %s\n", name);
						result.target = LM_ADDRESS_BAD;
						break;
					}
					result.target = MemHlp::findPrologue(
						result.match,
						MemHlp::prologueLowerBound(module.base, matchSegment.base),
						extraData, extraDataSize
					);
				}
				break;

			default:
				break;
		}

		g_pLog->debug("%s at %p\n", name, result.target);
	}

	return result;
}

lm_address_t MemHlp::searchSignature(
	const char* name,
	const char* signature,
	lm_module_t module,
	SigFollowMode mode,
	const int16_t* extraData,
	size_t extraDataSize
)
{
	return searchSignatureDetailed(name, signature, module, mode, extraData, extraDataSize).target;
}

lm_address_t MemHlp::searchSignature(const char* name, const char* signature, lm_module_t module, SigFollowMode mode)
{
	return MemHlp::searchSignature(name, signature, module, mode, nullptr, 0);
}

lm_address_t MemHlp::searchSignature(const char* name, const char* signature, lm_module_t module)
{
	return searchSignature(name, signature, module, SigFollowMode::None);
}

lm_address_t MemHlp::getJmpTarget(lm_address_t address)
{
	lm_inst_t inst;
	if (!LM_Disassemble(address, &inst))
	{
		g_pLog->debug("Failed to disassemble code at %p!");
		return LM_ADDRESS_BAD;
	}

	g_pLog->debug("Resolved to %s %s\n", inst.mnemonic, inst.op_str);

	if (strcmp(inst.mnemonic, "jmp") != 0 && strcmp(inst.mnemonic, "call") != 0)
		return LM_ADDRESS_BAD;

	const lm_address_t target = parseJumpTargetOperand(inst.op_str);
	if (target == LM_ADDRESS_BAD)
	{
		g_pLog->debug("Unsupported jump/call operand '%s'\n", inst.op_str);
		return LM_ADDRESS_BAD;
	}
	return target;
}

lm_address_t MemHlp::findPrologue(lm_address_t address, lm_address_t lowerBound,
                                   const int16_t* prologueBytes, lm_size_t prologueSize)
{
	constexpr unsigned int scanSize = 0x10000;

	for(unsigned int i = 0u; i < scanSize; i++)
	{
		if (!prologueWindowWithin(address, lowerBound, i, prologueSize))
			break;

		bool found = true;
		for(unsigned int j = 0u; j < prologueSize; j++)
		{
			if (!prologueByteMatches(
				prologueBytes[j], *reinterpret_cast<lm_byte_t*>(address - i - j)))
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			lm_address_t prol = address - i - prologueSize + 1;
			g_pLog->debug("Prologue found at %p\n", prol);
			return prol;
		}
	}

	g_pLog->debug("Unable to find prologue after going up %p bytes!\n", scanSize);
	return LM_ADDRESS_BAD;
}

bool MemHlp::fixPICThunkCall(const char* name, lm_address_t fn, lm_address_t tramp)
{
#if defined(__x86_64__)

	(void)name;
	(void)fn;
	(void)tramp;
	return true;
#else
	g_pLog->debug("Fixing PIC thunks for %s's trampoline\n", name);
	constexpr unsigned int maxBytes = 0x5;

	lm_inst_t inst;
	for(unsigned int curTrampOffset = 0; curTrampOffset <= maxBytes; )
	{
		lm_address_t startAddress = tramp + curTrampOffset;

		if (!LM_Disassemble(startAddress, &inst))
		{
			g_pLog->debug("Unable to dissassemble code at %p\n", tramp + curTrampOffset);
			return false;
		}

		curTrampOffset += inst.size;
		g_pLog->debug("%p: %s %s\n", inst.address, inst.mnemonic, inst.op_str);

		if (strcmp(inst.mnemonic, "call") != 0)
			continue;

		lm_address_t followAddress = fn + curTrampOffset + *reinterpret_cast<lm_address_t*>(startAddress + 1);
		bool isIPCThunk = true;
		char newInstr[256] = {};

		for(unsigned int i = 0; i < 2; i++)
		{
			if (!LM_Disassemble(followAddress, &inst))
			{
				g_pLog->debug("Unable to dissassemble code at %p\n", followAddress);
				return false;
			}

			followAddress += inst.size;

			g_pLog->debug("%p: %s %s\n", inst.address, inst.mnemonic, inst.op_str);

			auto splits = std::vector<std::string>();
			lm_address_t retAddress = LM_ADDRESS_BAD;
			switch(i)
			{
				case 0:
					if (strcmp(inst.mnemonic, "mov") != 0)
						isIPCThunk = false;

					if (inst.op_str[0] == '\0')
					{
						isIPCThunk = false;
						break;
					}
					splits = Utils::strsplit(inst.op_str, ",");
					if (splits.empty() || splits.front().empty())
					{
						isIPCThunk = false;
						break;
					}
					retAddress = fn + curTrampOffset;
					if (!formatPICThunkInstruction(
						newInstr, sizeof(newInstr), inst.mnemonic,
						splits.front().c_str(), retAddress))
					{
						g_pLog->debug("Unable to format PIC thunk instruction\n");
						return false;
					}
					break;

				case 1:
					if (strcmp(inst.mnemonic, "ret") != 0)
						isIPCThunk = false;
					break;
			}

			if (!isIPCThunk)
				break;
		}

		if (!isIPCThunk)
			continue;

		if(!LM_Assemble(newInstr, &inst))
		{
			printf("Unable to assemble instruction %s!\n", newInstr);
			return false;
		}

		lm_prot_t oldProt;
		LM_ProtMemory(startAddress, inst.size, LM_PROT_XRW, &oldProt);
		LM_WriteMemory(startAddress, inst.bytes, inst.size);
		LM_ProtMemory(startAddress, inst.size, oldProt, nullptr);
		g_pLog->debug("Replaced PIC thunk call for %s at %p with %s\n", name, followAddress, newInstr);
		return true;
	}

	return false;
#endif
}

const char* MemHlp::getTypeName(void* pClass)
{
	const lm_address_t vft = *reinterpret_cast<lm_address_t*>(pClass);
	const lm_address_t typeInfo = *reinterpret_cast<lm_address_t*>(vft - sizeof(lm_address_t));
	const char* name = *reinterpret_cast<const char**>(typeInfo + sizeof(lm_address_t));

	return name;
}

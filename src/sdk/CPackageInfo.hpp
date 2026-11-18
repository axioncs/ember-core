

#pragma once

#include "CUtl.hpp"

#include <cstdint>

enum class EPackageStatus : uint32_t
{
	Available = 0,
	Preorder = 1,
	Unavailable = 2,
	Invalid = 3
};

enum class EBillingType : uint32_t {};
enum class ELicenseType : uint32_t {};

#if defined(__i386__)
static_assert(sizeof(CUtlMemory<uint32_t>) == 12, "CUtlMemory layout drift");
static_assert(sizeof(CUtlVector<uint32_t>) == 16, "CUtlVector layout drift");
#else
static_assert(sizeof(CUtlMemory<uint32_t>) == 16, "CUtlMemory x86_64 layout drift");
static_assert(sizeof(CUtlVector<uint32_t>) == 24, "CUtlVector x86_64 layout drift");
#endif

struct PackageInfo
{
	uint32_t PackageId;
	int32_t ChangeNumber;
	uint64_t PICS_token;
	EBillingType BillingType;
	ELicenseType LicenseType;
	EPackageStatus Status;
	uint8_t SHA_1_Hash[20];
	void* pPackageInfoNodeBegin;
	void* pExtendNodeBegin;
	CUtlVector<uint32_t> AppIdVec;
	CUtlVector<uint32_t> DepotIdVec;

};
static_assert(offsetof(PackageInfo, Status)     == 0x18, "PackageInfo::Status drift");
static_assert(offsetof(PackageInfo, SHA_1_Hash) == 0x1C, "PackageInfo::SHA_1_Hash drift");
#if defined(__i386__)
static_assert(offsetof(PackageInfo, AppIdVec)   == 0x38, "PackageInfo::AppIdVec drift");
static_assert(offsetof(PackageInfo, DepotIdVec) == 0x48, "PackageInfo::DepotIdVec drift");
#else

static_assert(offsetof(PackageInfo, AppIdVec)   == 0x40, "PackageInfo::AppIdVec x86_64 drift");
static_assert(offsetof(PackageInfo, DepotIdVec) == 0x58, "PackageInfo::DepotIdVec x86_64 drift");
#endif

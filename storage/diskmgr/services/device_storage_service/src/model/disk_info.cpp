/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "model/disk_info.h"
#include "device_storage_service_hilog.h"
#include "model/device_storage_constants.h"
#include "model/parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace DS {
DiskInfo::DiskInfo(const std::string &id, int32_t flags)
{
    mId = id;
    mFlags = flags;
}
DiskInfo::DiskInfo()
{
    SSLOG_D("instance is created without parameter");
}

DiskInfo::~DiskInfo() {}
bool DiskInfo::IsUsb()
{
    return (mFlags & Flags::kUsb) != 0;
}
bool DiskInfo::IsSd()
{
    return (mFlags & Flags::kSd) != 0;
}
bool DiskInfo::IsAdoptable()
{
    return (mFlags & Flags::kAdoptable) != 0;
}

void DiskInfo::SetSize(uint64_t size)
{
    mSize = size;
}
void DiskInfo::SetSysPath(const std::string &sysPath)
{
    mSysPath = sysPath;
}
void DiskInfo::SetLabel(const std::string &label)
{
    mLabel = label;
}

const std::string &DiskInfo::GetDescription()
{
    if (IsSd()) {
        return DS::Constants::SDCARD;
    } else if (IsUsb()) {
        return DS::Constants::USB;
    }
    return mLabel;
}

bool DiskInfo::ReadFromParcel(Parcel &parcel)
{
    mId = Str16ToStr8(parcel.ReadString16());
    mSysPath = Str16ToStr8(parcel.ReadString16());
    mSize = parcel.ReadUint64();
    mLabel = Str16ToStr8(parcel.ReadString16());
    mFlags = parcel.ReadInt32();
    return true;
}

DiskInfo *DiskInfo::Unmarshalling(Parcel &parcel)
{
    DiskInfo *info = new (std::nothrow) DiskInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        SSLOG_W("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool DiskInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(mId));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(mSysPath));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint64, parcel, mSize);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(mLabel));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, mFlags);
    return true;
}
} // namespace DS
} // namespace OHOS

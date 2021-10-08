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
#include "device_storage_service_hilog.h"
#include "model/volume_info.h"
#include "model/parcel_macro.h"
#include "model/device_storage_constants.h"
#include "string_ex.h"

namespace OHOS {
namespace DS {
VolumeInfo::VolumeInfo(const std::string &id,
                       int32_t type,
                       const std::string &diskId,
                       const std::string &partGuid)
{
    mId = id;
    mType = type;
    mDiskId = diskId;
    mPartGuid = partGuid;
}
VolumeInfo::~VolumeInfo() {}
VolumeInfo::VolumeInfo()
{
    SSLOG_D("instance is created without parameter");
}

void VolumeInfo::SetDiskId(const std::string &diskId)
{
    mDiskId = diskId;
}
void VolumeInfo::SetType(int32_t type)
{
    mType = type;
}
void VolumeInfo::SetPartGuid(const std::string &partGuid)
{
    mPartGuid = partGuid;
}
void VolumeInfo::SetMountFlags(int mountFlags)
{
    mMountFlags = mountFlags;
}
void VolumeInfo::SetMountUserId(int mountUserId)
{
    mMountUserId = mountUserId;
}

void VolumeInfo::SetId(const std::string &id)
{
    mId = id;
}
void VolumeInfo::SetPath(const std::string &path)
{
    mPath = path;
}
void VolumeInfo::SetInternalPath(const std::string &internalPath)
{
    mInternalPath = internalPath;
}

void VolumeInfo::SetFsType(const std::string &fsType)
{
    mFsType = fsType;
}
void VolumeInfo::SetFsUuid(const std::string &fsUuid)
{
    mFsUuid = fsUuid;
}
void VolumeInfo::SetFsLabel(const std::string &fsLabel)
{
    mFsLabel = fsLabel;
}

void VolumeInfo::SetState(int32_t state)
{
    mState = state;
}

const std::string &VolumeInfo::GetDescription()
{
    if (mId.substr(0, DS::Constants::ID_EMULATED_INTERNAL.size()) == DS::Constants::ID_EMULATED_INTERNAL) {
        return DS::Constants::STORAGE_INTERNEL;
    }
    if (DS::Constants::ID_PRIVATE_INTERNAL == mId) {
        return DS::Constants::STORAGE_INTERNEL;
    } else {
        return mFsLabel;
    }
}

bool VolumeInfo::IsVisibleForUser(int32_t userId)
{
    if (mMountUserId == userId && mType == (int32_t)Type::kPublic) {
        return (mMountFlags & kVisible) != 0;
    } else if (mType == (int32_t)Type::kEmulated) {
        return (mMountFlags & kVisible) != 0;
    }
    return false;
}

bool VolumeInfo::IsPrimary()
{
    return (mMountFlags & MountFlags::kPrimary) != 0;
}

bool VolumeInfo::IsPrimaryEmulatedForUser(int32_t userId)
{
    return mId == (DS::Constants::ID_EMULATED_INTERNAL + ";" + std::to_string(userId));
}

bool VolumeInfo::IsRemovable(int32_t userId)
{
    if (IsPrimaryEmulatedForUser(userId)) {
        return false;
    }
    if (mDiskId == "") {
        return false;
    }
    return true;
}
bool VolumeInfo::ReadFromParcel(Parcel &parcel)
{
    mId = Str16ToStr8(parcel.ReadString16());
    mDiskId = Str16ToStr8(parcel.ReadString16());
    mPartGuid = Str16ToStr8(parcel.ReadString16());
    mType = parcel.ReadInt32();
    mMountFlags = parcel.ReadInt32();
    mMountUserId = parcel.ReadInt32();
    mState = parcel.ReadInt32();
    mPath = Str16ToStr8(parcel.ReadString16());
    mInternalPath = Str16ToStr8(parcel.ReadString16());
    mFsUuid = Str16ToStr8(parcel.ReadString16());
    mFsLabel = Str16ToStr8(parcel.ReadString16());
    return true;
}

VolumeInfo *VolumeInfo::Unmarshalling(Parcel &parcel)
{
    VolumeInfo *info = new (std::nothrow) VolumeInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        SSLOG_W("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool VolumeInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(mId));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(mDiskId));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(mPartGuid));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, mType);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, mMountFlags);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, mMountUserId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, mState);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(mPath));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(mInternalPath));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(mFsUuid));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(mFsLabel));
    return true;
}
} // namespace DS
} // namespace OHOS
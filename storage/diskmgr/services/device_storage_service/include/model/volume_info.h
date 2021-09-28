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
#ifndef STORAGE_VOLUMEINFO_H
#define STORAGE_VOLUMEINFO_H
#include <list>
#include <string>
#include "parcel.h"

namespace OHOS {
namespace DS {
class VolumeInfo : public Parcelable {
public:
    VolumeInfo(const std::string &id, int32_t type, const std::string &diskId, const std::string &partGuid);
    VolumeInfo();
    virtual ~VolumeInfo();

    enum class Type {
        kPublic = 0,
        kPrivate,
        kEmulated,
        kAsec,
        kObb,
        kStub,
    };

    enum MountFlags {
        kPrimary = 1 << 0,
        kVisible = 1 << 1,
    };

    enum class State {
        kUnmounted = 0,
        kChecking,
        kMounted,
        kMountedReadOnly,
        kFormatting,
        kEjecting,
        kUnmountable,
        kRemoved,
        kBadRemoval,
    };

    const std::string &GetId() const
    {
        return mId;
    }
    const std::string &GetDiskId() const
    {
        return mDiskId;
    }
    const std::string &GetPartGuid() const
    {
        return mPartGuid;
    }

    const std::string &GetFsUuid() const
    {
        return mFsUuid;
    }
    int GetType() const
    {
        return mType;
    }
    int32_t GetMountFlags() const
    {
        return mMountFlags;
    }
    int GetMountUserId() const
    {
        return mMountUserId;
    }
    int GetState() const
    {
        return mState;
    }
    const std::string &GetPath() const
    {
        return mPath;
    }
    const std::string &GetInternalPath() const
    {
        return mInternalPath;
    }
    const std::string &GetFsLabel() const
    {
        return mFsLabel;
    }
    const std::string &GetDescription();
    bool IsVisibleForUser(int32_t userId);

    void SetDiskId(const std::string &diskId);
    void SetType(int32_t type);
    void SetPartGuid(const std::string &partGuid);
    void SetMountFlags(int mountFlags);
    void SetMountUserId(int mountUserId);
    void SetFsType(const std::string &fsType);
    void SetFsUuid(const std::string &fsUuid);
    void SetFsLabel(const std::string &fsLabel);
    void SetPath(const std::string &path);
    void SetState(int32_t state);
    bool IsEmulated()
    {
        return mType == static_cast<int32_t>(Type::kEmulated);
    }
    bool IsPrimaryEmulatedForUser(int32_t userId);
    bool IsRemovable(int32_t userId);
    bool ReadFromParcel(Parcel &parcel);

    virtual bool Marshalling(Parcel &parcel) const override;
    static VolumeInfo *Unmarshalling(Parcel &parcel);
    void SetId(const std::string &id);
    void SetInternalPath(const std::string &internalPath);
    bool IsPrimary();

private:
    std::string mId;
    std::string mDiskId;
    /* Partition GUID of this volume */
    std::string mPartGuid;
    /* Volume type */
    int32_t mType;
    /* Flags used when mounting this volume */
    int32_t mMountFlags = 0;
    /* User that owns this volume, otherwise -1 */
    int32_t mMountUserId;
    /* Current state of volume */
    int32_t mState = 0;
    /* Path to mounted volume */
    std::string mPath;
    /* Path to internal backing storage */
    std::string mInternalPath;
    /* Filesystem type */
    std::string mFsType;
    /* Filesystem UUID */
    std::string mFsUuid;
    /* User-visible filesystem label */
    std::string mFsLabel;
};
} // namespace DS
} // namespace OHOS
#endif // STORAGE_VOLUMEINFO_H

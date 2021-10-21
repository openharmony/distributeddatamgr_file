/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef STORAGEBASE_H
#define STORAGEBASE_H

#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <sys/types.h>

#include "storage_constant.h"
#include "storage_service.h"
#include "storage_utils.h"

namespace OHOS {
namespace StorageService {
class StorageBase {
public:
    virtual ~StorageBase();
    enum class Type {
        baseTypeIsPublic = 0,
        baseTypeIsPrivate,
        baseTypeIsEmulated,
        baseTypeIsAsec,
        baseTypeIsObb,
        baseTypeIsStub,
    };

    enum MountFlags {
        baseMountFlagsIskPrimary = 1 << 0,
        baseMountFlagsIsVisible = 1 << 1,
    };

    enum class State {
        baseStateIsError = -1,
        baseStateIsUnMounted = 0,
        baseStateIsChecking,
        baseStateIsMounted,
        baseStateIsMountedReadOnly,
        baseStateIsFormatting,
        baseStateIsEjecting,
        baseStateIsUnMountable,
        baseStateIsRemoved,
        baseStateIsBadRemoval,
    };

    enum SendInfoFlags {
        SendInfoFlagsIsPath = 1,
        SendInfoFlagsIsInternalPath,
        SendInfoFlagsIsState,
    };

    enum SetInfoFlags {
        SetInfoFlagsIsDiskId = 1,
        SetInfoFlagsIsPartGuid,
        setInfoFlagsIsId,
        SetInfoFlagsIsMountUserId,
        SetInfoFlagsIsMountFlags,
        SetInfoFlagsIsSetSilent,
        SetInfoFlagsIsDiskPathId,
    };

    enum BaseSendFlags {
        BaseSendFlagsisCreate = 0,
        BaseSendFlagsisDestroy,
    };

    int Create();
    int Destroy();
    int Mount();
    int UnMount();
    int Format(const std::string &fsType);
    int SetInfo(const std::string &diskId,
                const std::string &partGuid,
                const std::string &id,
                int32_t MountUserId,
                int32_t MountFlags,
                bool silent,
                int setInfoFlags);
    int SetMountFlags(int MountFlags);
    void HandleVolume(const std::shared_ptr<StorageBase> &volume, int handleVolumeflags);

    const std::list<std::shared_ptr<StorageBase>> &getVolumes() const
    {
        return mVolumes;
    }

    Type GetType() const
    {
        return baseType;
    }

    const std::string &getId() const
    {
        return baseId;
    }

    const std::string &getDiskPathId() const
    {
        return baseDiskPathId;
    }

    int GetMountFlags() const
    {
        return baseMountFlags;
    }

    int GetMountUserId() const
    {
        return baseMountUserId;
    }

    const std::string &GetInternalPath() const
    {
        return baseInternalPath;
    }

    State GetState() const
    {
        return baseState;
    }

    const std::string &getPath() const
    {
        return basePath;
    }

    std::shared_ptr<StorageBase> FindVolume(const std::string &id);

    bool IsEmulated()
    {
        return baseType == Type::baseTypeIsEmulated;
    }

protected:
    explicit StorageBase(Type type);

    virtual int DoCreate();
    virtual int DoDestroy();
    virtual int DoMount() = 0;
    virtual void DoPostMount();
    virtual int DoUnMount() = 0;
    virtual int DoFormat(const std::string &fsType);

    int SendInfo(const std::string &path, const std::string &internalPath, State state, int sendInfoFlags);

    OHOS::sptr<OHOS::IStorageServiceCallback> GetListener() const;

private:
    void BaseSend(int baseSendFlags);
    int BoolStateIsUnMount();
    int BaseMount();
    int BaseUnMount();
    int BaseCreate();
    int BaseDestroy();
    void BaseDestroyToUnMount();
    int BaseFormat(const std::string &fsType);
    int32_t baseMountFlags;
    int32_t baseMountUserId;
    bool baseSilent;
    bool baseCreated;
    State baseState;
    Type baseType;

    std::string baseId;
    std::string baseDiskPathId;
    std::string baseDiskId;
    std::string basePartGuid;
    std::string basePath;
    std::string baseInternalPath;

    std::list<std::shared_ptr<StorageBase>> mVolumes;
    OHOS::sptr<OHOS::IStorageServiceCallback> mMountCallback;
};
} // namespace StorageService
} // namespace OHOS
#endif // STORAGEBASE_H

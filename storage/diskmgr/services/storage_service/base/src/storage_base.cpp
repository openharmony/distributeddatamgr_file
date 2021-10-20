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
#include "storage_base.h"

#include "interface/storage_service_callback_proxy.h"
#include "storage_hilog.h"
#include "storage_manager.h"
#include "storage_constant.h"

using namespace OHOS::StorageService::Constants;

namespace OHOS {
namespace StorageService {
static constexpr int SWITCH_FLAGS_SENDSTATE = 3;

StorageBase::StorageBase(Type type)
    : baseMountFlags(0)
    , baseMountUserId(USER_UNKNOWN)
    , baseSilent(false)
    , baseCreated(false)
    , baseState(State::baseStateIsUnMounted)
    , baseType(type)
{}

StorageBase::~StorageBase() {}

void StorageBase::HandleVolume(const std::shared_ptr<StorageBase> &volume, int handleVolumeflags)
{
    if (handleVolumeflags) {
        mVolumes.push_back(volume);
    } else {
        mVolumes.remove(volume);
    }
}

int StorageBase::SetInfo(const std::string &diskId,
                         const std::string &partGuid,
                         const std::string &id,
                         int32_t MountUserId,
                         int32_t MountFlags,
                         bool silent,
                         int setInfoFlags)
{
    if (baseCreated) {
        return -EBUSY;
    }

    switch (setInfoFlags) {
        case SetInfoFlagsIsDiskId:
            baseDiskId = diskId;
            break;
        case SetInfoFlagsIsPartGuid:
            basePartGuid = partGuid;
            break;
        case setInfoFlagsIsId:
            baseId = id;
            break;
        case SetInfoFlagsIsMountUserId:
            if (BoolStateIsUnMount() != 0) {
                return -EBUSY;
            }
            baseMountUserId = MountUserId;
            break;
        case SetInfoFlagsIsMountFlags:
            if (BoolStateIsUnMount() != 0) {
                return -EBUSY;
            }
            baseMountFlags = MountFlags;
            break;
        case SetInfoFlagsIsSetSilent:
            baseSilent = silent;
            break;
        case SetInfoFlagsIsDiskPathId:
            baseDiskPathId = id;
            break;
        default: SSLOG_I("error"); break;
    }
    return OK;
}

int StorageBase::SendInfo(const std::string &path,
                          const std::string &internalPath,
                          State state,
                          int sendInfoFlags)
{
    auto listener = StorageManager::Instance()->GetListener();
    if (!listener) {
        return -1;
    }

    switch (sendInfoFlags) {
        case SendInfoFlagsIsPath:
            basePath = path;
            listener->OnVolumePathChanged(getId(), basePath);
            break;
        case SendInfoFlagsIsInternalPath:
            baseInternalPath = internalPath;
            listener->OnVolumeInternalPathChanged(getId(), baseInternalPath);
            break;
        case SendInfoFlagsIsState:
            baseState = state;
            listener->OnVolumeStateChanged(getId(), static_cast<int32_t>(baseState));
            break;
        default: break;
    }
    return OK;
}

std::shared_ptr<StorageBase> StorageBase::FindVolume(const std::string &id)
{
    for (auto vol : mVolumes) {
        if (vol->getId() == id) {
            return vol;
        }
    }
    return nullptr;
}

void StorageBase::BaseSend(int baseSendFlags)
{
    auto listener = StorageManager::Instance()->GetListener();
    if (listener) {
        switch (baseSendFlags) {
            case BaseSendFlagsisCreate:
                listener->OnVolumeCreated(getId(), static_cast<int32_t>(baseType), baseDiskId, basePartGuid);
                break;
            case BaseSendFlagsisDestroy:
                listener->OnVolumeDestroyed(getId());
                break;
            default: break;
        }
    }
}

int StorageBase::Create()
{
    baseCreated = true;
    return BaseCreate();
}

int StorageBase::DoCreate()
{
    return OK;
}

int StorageBase::Mount()
{
    if (BoolStateIsUnMount() != 0) {
        return -EBUSY;
    }
    return BaseMount();
}

int StorageBase::UnMount()
{
    SendInfo("", "", State::baseStateIsEjecting, SWITCH_FLAGS_SENDSTATE);
    for (const auto &vol : mVolumes) {
        vol->Destroy();
    }
    mVolumes.clear();
    return BaseUnMount();
}

int StorageBase::DoDestroy()
{
    return OK;
}

int StorageBase::Destroy()
{
    if (baseState == State::baseStateIsMounted) {
        BaseDestroyToUnMount();
    }
    return BaseDestroy();
}

void StorageBase::DoPostMount() {}

int StorageBase::DoFormat(const std::string &fsType)
{
    return -ENOTSUP;
}

int StorageBase::Format(const std::string &fsType)
{
    if (baseState == State::baseStateIsMounted) {
        UnMount();
    }
    return BaseFormat(fsType);
}

int StorageBase::BaseMount()
{
    SendInfo("", "", State::baseStateIsChecking, SWITCH_FLAGS_SENDSTATE);
    int mountState = DoMount();
    SendInfo("", "", (mountState == OK) ? State::baseStateIsMounted : State::baseStateIsUnMountable,
             SWITCH_FLAGS_SENDSTATE);

    if (mountState == OK) {
        DoPostMount();
    }
    return mountState;
}

int StorageBase::BaseUnMount()
{
    int unMountState = DoUnMount();
    SendInfo("", "", State::baseStateIsUnMounted, SWITCH_FLAGS_SENDSTATE);
    return unMountState;
}

int StorageBase::BaseCreate()
{
    int creatState = DoCreate();
    BaseSend(BaseSendFlagsisCreate);
    SendInfo("", "", State::baseStateIsUnMounted, SWITCH_FLAGS_SENDSTATE);
    return creatState;
}

int StorageBase::BaseDestroy()
{
    SendInfo("", "", State::baseStateIsRemoved, SWITCH_FLAGS_SENDSTATE);
    BaseSend(BaseSendFlagsisDestroy);
    int destroyState = DoDestroy();
    baseCreated = false;
    return destroyState;
}

void StorageBase::BaseDestroyToUnMount()
{
    UnMount();
    SendInfo("", "", State::baseStateIsBadRemoval, SWITCH_FLAGS_SENDSTATE);
}

int StorageBase::BaseFormat(const std::string &fsType)
{
    if (BoolStateIsUnMount() == 0) {
        SendInfo("", "", State::baseStateIsFormatting, SWITCH_FLAGS_SENDSTATE);
        int formatState = DoFormat(fsType);
        SendInfo("", "", State::baseStateIsUnMounted, SWITCH_FLAGS_SENDSTATE);
        return formatState;
    } else {
        return -EBUSY;
    }
}

int StorageBase::BoolStateIsUnMount()
{
    if ((baseState != State::baseStateIsUnMounted) && (baseState != State::baseStateIsUnMountable)) {
        return -1;
    }
    return 0;
}
} // namespace StorageService
} // namespace OHOS
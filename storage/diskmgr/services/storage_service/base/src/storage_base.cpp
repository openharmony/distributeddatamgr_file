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
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (handleVolumeflags) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        mVolumes.push_back(volume);
    } else {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
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
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (baseCreated) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return -EBUSY;
    }

    switch (setInfoFlags) {
        case SetInfoFlagsIsDiskId:
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            baseDiskId = diskId;
            break;
        case SetInfoFlagsIsPartGuid:
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            basePartGuid = partGuid;
            break;
        case setInfoFlagsIsId:
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            baseId = id;
            SSLOG_I("dugl %{public}s %{public}s %{public}d baseId=%{public}s", __FILE__, __func__, __LINE__,
                    baseId.c_str());
            break;
        case SetInfoFlagsIsMountUserId:
            if (BoolStateIsUnMount() != 0) {
                SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
                return -EBUSY;
            }
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            baseMountUserId = MountUserId;
            break;
        case SetInfoFlagsIsMountFlags:
            if (BoolStateIsUnMount() != 0) {
                SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
                return -EBUSY;
            }
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            baseMountFlags = MountFlags;
            break;
        case SetInfoFlagsIsSetSilent:
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            baseSilent = silent;
            break;
        case SetInfoFlagsIsDiskPathId:
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            baseDiskPathId = id;
            SSLOG_I("dugl %{public}s %{public}s %{public}d baseDiskPathId=%{public}s", __FILE__, __func__,
                    __LINE__, baseDiskPathId.c_str());
            break;
        default: SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__); break;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return OK;
}

int StorageBase::SendInfo(const std::string &path,
                          const std::string &internalPath,
                          State state,
                          int sendInfoFlags)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto listener = StorageManager::Instance()->GetListener();
    if (!listener) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return -1;
    }

    switch (sendInfoFlags) {
        case SendInfoFlagsIsPath:
            basePath = path;
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            listener->OnVolumePathChanged(getId(), basePath);
            break;
        case SendInfoFlagsIsInternalPath:
            baseInternalPath = internalPath;
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            listener->OnVolumeInternalPathChanged(getId(), baseInternalPath);
            break;
        case SendInfoFlagsIsState:
            baseState = state;
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            listener->OnVolumeStateChanged(getId(), static_cast<int32_t>(baseState));
            break;
        default: break;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return OK;
}

std::shared_ptr<StorageBase> StorageBase::FindVolume(const std::string &id)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d baseId=%{public}s id=%{public}s", __FILE__, __func__,
            __LINE__, baseId.c_str(), id.c_str());
    for (auto vol : mVolumes) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        if (vol->getId() == id) {
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            return vol;
        }
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return nullptr;
}

void StorageBase::BaseSend(int baseSendFlags)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto listener = StorageManager::Instance()->GetListener();
    if (listener) {
        switch (baseSendFlags) {
            case BaseSendFlagsisCreate:
                SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
                listener->OnVolumeCreated(getId(), static_cast<int32_t>(baseType), baseDiskId, basePartGuid);
                break;
            case BaseSendFlagsisDestroy:
                SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
                listener->OnVolumeDestroyed(getId());
                break;
            default: break;
        }
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
}

int StorageBase::Create()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    baseCreated = true;
    return BaseCreate();
}

int StorageBase::DoCreate()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return OK;
}

int StorageBase::Mount()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (BoolStateIsUnMount() != 0) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return -EBUSY;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return BaseMount();
}

int StorageBase::UnMount()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    SendInfo("", "", State::baseStateIsEjecting, SWITCH_FLAGS_SENDSTATE);
    for (const auto &vol : mVolumes) {
        vol->Destroy();
    }
    mVolumes.clear();
    return BaseUnMount();
}

int StorageBase::DoDestroy()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return OK;
}

int StorageBase::Destroy()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (baseState == State::baseStateIsMounted) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        BaseDestroyToUnMount();
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return BaseDestroy();
}

void StorageBase::DoPostMount() {}

int StorageBase::DoFormat(const std::string &fsType)
{
    return -ENOTSUP;
}

int StorageBase::Format(const std::string &fsType)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (baseState == State::baseStateIsMounted) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        UnMount();
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return BaseFormat(fsType);
}

int StorageBase::BaseMount()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    SendInfo("", "", State::baseStateIsChecking, SWITCH_FLAGS_SENDSTATE);
    int mountState = DoMount();
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    SendInfo("", "", (mountState == OK) ? State::baseStateIsMounted : State::baseStateIsUnMountable,
             SWITCH_FLAGS_SENDSTATE);

    if (mountState == OK) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        DoPostMount();
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return mountState;
}

int StorageBase::BaseUnMount()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    int unMountState = DoUnMount();
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    SendInfo("", "", State::baseStateIsUnMounted, SWITCH_FLAGS_SENDSTATE);
    return unMountState;
}

int StorageBase::BaseCreate()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    int creatState = DoCreate();
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    BaseSend(BaseSendFlagsisCreate);
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    SendInfo("", "", State::baseStateIsUnMounted, SWITCH_FLAGS_SENDSTATE);
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return creatState;
}

int StorageBase::BaseDestroy()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    SendInfo("", "", State::baseStateIsRemoved, SWITCH_FLAGS_SENDSTATE);
    BaseSend(BaseSendFlagsisDestroy);
    int destroyState = DoDestroy();
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    baseCreated = false;
    return destroyState;
}

void StorageBase::BaseDestroyToUnMount()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    UnMount();
    SendInfo("", "", State::baseStateIsBadRemoval, SWITCH_FLAGS_SENDSTATE);
}

int StorageBase::BaseFormat(const std::string &fsType)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (BoolStateIsUnMount() == 0) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        SendInfo("", "", State::baseStateIsFormatting, SWITCH_FLAGS_SENDSTATE);
        int formatState = DoFormat(fsType);
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        SendInfo("", "", State::baseStateIsUnMounted, SWITCH_FLAGS_SENDSTATE);
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return formatState;
    } else {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return -EBUSY;
    }
}

int StorageBase::BoolStateIsUnMount()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if ((baseState != State::baseStateIsUnMounted) && (baseState != State::baseStateIsUnMountable)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return -1;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return 0;
}

} // namespace StorageService
} // namespace OHOS

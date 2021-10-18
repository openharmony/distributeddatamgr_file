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
#include "device_storage_event_handler.h"

#include "common_event_manager.h"
#include "common_event_support.h"
#include "device_storage_service.h"
#include "device_storage_service_hilog.h"
#include "ohos/aafwk/content/want.h"

namespace OHOS {
DeviceStorageEventHandler::DeviceStorageEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
                                                     const sptr<DeviceStorageService> &service)
    :EventHandler(runner)
    ,service_(service)
{
    SSLOG_I("DeviceStorageEventHandler instance is created");
}

DeviceStorageEventHandler::~DeviceStorageEventHandler()
{
    SSLOG_I("DeviceStorageEventHandler instance is destroyed");
}

void DeviceStorageEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    switch (event->GetInnerEventId()) {
        case VOLUME_MOUNT: {
            SSLOG_I("DeviceStorageEventHandler handle volume_mount");
            std::shared_ptr<DS::VolumeInfo> param = event->GetSharedObject<DS::VolumeInfo>();
            // (vol.id, vol.mountFlags, vol.mountUserId)
            service_->Mount(param->GetId());
            break;
        }
        case VOLUME_BROADCAST: {
            SSLOG_I("DeviceStorageEventHandler handle volume broadcast");
            HandleVolumeBroadcast(event);
            break;
        }
        case DAEMON_CONNECTED: {
            service_->ResetIfBootedAndConnected();
            break;
        }
        case VOLUME_UNMOUNT: {
            SSLOG_I("DeviceStorageEventHandler handle volume_Unmount");
            std::shared_ptr<DS::VolumeInfo> param = event->GetSharedObject<DS::VolumeInfo>();
            service_->UnMount(param->GetId());
            break;
        }
        case COMPLETE_UNLOCK_USER: {
            SSLOG_I("DeviceStorageEventHandler handle COMPLETE_UNLOCK_USER");
            auto object = event->GetUniqueObject<int32_t>();
            if (object == nullptr) {
                SSLOG_I("object is nullptr!");
                return;
            }
            int32_t userId = *object;
            SSLOG_I("DeviceStorageEventHandler handle COMPLETE_UNLOCK_USER user Id is %{public}d", userId);
            service_->GetDeviceStorageDataMgr()->CompleteUnlockUser(userId);
            break;
        }
        case SS_RECONNECT: {
            SSLOG_I("DeviceStorageEventHandler handle SS_RECONNECT");
            service_->Init();
            break;
        }
        default: SSLOG_E("the eventId is not supported"); break;
    }
}

void DeviceStorageEventHandler::HandleVolumeBroadcast(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<DS::VolumeInfo> param = event->GetSharedObject<DS::VolumeInfo>();
    int32_t state = param->GetState();
    std::string eventData = [state]() -> std::string {
        switch (state) {
            case (int32_t)DS::VolumeInfo::State::kMounted:
                return EventFwk::CommonEventSupport::COMMON_EVENT_DISK_MOUNTED;
            case (int32_t)DS::VolumeInfo::State::kUnmounted:
                return EventFwk::CommonEventSupport::COMMON_EVENT_DISK_UNMOUNTED;
            case (int32_t)DS::VolumeInfo::State::kRemoved:
                return EventFwk::CommonEventSupport::COMMON_EVENT_DISK_REMOVED;
            case (int32_t)DS::VolumeInfo::State::kBadRemoval:
                return EventFwk::CommonEventSupport::COMMON_EVENT_DISK_BAD_REMOVAL;
            case (int32_t)DS::VolumeInfo::State::kEjecting:
                return EventFwk::CommonEventSupport::COMMON_EVENT_DISK_EJECT;
            case (int32_t)DS::VolumeInfo::State::kChecking: return "usual.event.data.DISK_CHECKING";
            default: return "usual.event.data.DISK_CHANGED";
        }
    }();
    AAFwk::Want want;
    want.SetAction(eventData);
    want.SetParam("volId", param->GetId());
    want.SetParam("state", param->GetState());
    SSLOG_I("DeviceStorageEventHandler handle volume broadcast volId and state and  action is");
    SSLOG_I("%{public}s, %{public}d,%{public}s", param->GetId().c_str(), state, eventData.c_str());
    EventFwk::CommonEventData data;
    data.SetWant(want);
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        SSLOG_E("DeviceStorageService::sendbroadcastfail");
    }
    SSLOG_I("DeviceStorageService::sendbroadcastsuccess");
}
} // namespace OHOS
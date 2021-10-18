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
#ifndef STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_EVENT_HANDLER_H
#define STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_EVENT_HANDLER_H

#include <refbase.h>
#include "event_handler.h"

namespace OHOS {
class DeviceStorageService;
class DeviceStorageEventHandler : public AppExecFwk::EventHandler {
public:
    explicit DeviceStorageEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
                                       const sptr<DeviceStorageService> &service);
    virtual ~DeviceStorageEventHandler() override;

    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;

    enum {
        VOLUME_MOUNT = 1,
        VOLUME_BROADCAST = 2,
        DAEMON_CONNECTED = 3,
        VOLUME_UNMOUNT = 4,
        COMPLETE_UNLOCK_USER = 5,
        SS_RECONNECT = 6,
    };

    void HandleVolumeBroadcast(const AppExecFwk::InnerEvent::Pointer &event);

private:
    sptr<DeviceStorageService> service_;
};
} // namespace OHOS

#endif // STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_EVENT_HANDLER_H
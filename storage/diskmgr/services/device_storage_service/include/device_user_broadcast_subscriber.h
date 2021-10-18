/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef STORAGE_DSS_INCLUDE_DEVICE_USER_BROADCAST_SUBSCRIBER_H
#define STORAGE_DSS_INCLUDE_DEVICE_USER_BROADCAST_SUBSCRIBER_H

#include <string>
#include <vector>
#include "common_event.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "device_storage_service.h"

namespace OHOS {
class DeviceUserBroadcastSubscriber : public EventFwk::CommonEventSubscriber {
public:
    explicit DeviceUserBroadcastSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo,
                                           const sptr<DeviceStorageService> &service);
    ~DeviceUserBroadcastSubscriber() = default;
    virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data);

private:
    sptr<DeviceStorageService> service_;
};
} // namespace OHOS
#endif
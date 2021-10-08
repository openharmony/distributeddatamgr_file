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

#include "device_user_broadcast_subscriber.h"
#include "device_storage_service_hilog.h"

namespace OHOS {
using namespace OHOS::EventFwk;
DeviceUserBroadcastSubscriber::DeviceUserBroadcastSubscriber(
    const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo,
    const sptr<DeviceStorageService> &service): CommonEventSubscriber(subscriberInfo), service_(service)
{}

void DeviceUserBroadcastSubscriber::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    AAFwk::Want want = data.GetWant();
    std::string action = data.GetWant().GetAction();
    int32_t userId = want.GetIntParam("userId", 0);
    SSLOG_D("Subscriber::OnReceiveEvent action = %{public}s\n", action.c_str());
    int msgCode = GetCode();
    std::string msgData = GetData();
    SSLOG_D("Subscriber::OnReceiveEvent msgData = %{public}s,msgCode = %{public}d\n", msgData.c_str(),
            msgCode);

    if (OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_USER_ADDED == action) {
        // todo：user getSerial
        int32_t userSerial = 0;
        if (service_ != nullptr) {
            service_->UserAdded(userId, userSerial);
        }
    } else if (OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED == action) {
        if (service_ != nullptr) {
            service_->UserRemoved(userId);
        }
    }
}
} // namespace OHOS
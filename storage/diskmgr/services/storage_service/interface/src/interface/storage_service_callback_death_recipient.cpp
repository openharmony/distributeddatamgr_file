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
#include "interface/storage_service_callback_death_recipient.h"
#include "interface/storage_service_callback_interface.h"
#include "storage_hilog.h"
#include "storage_manager.h"
#include "storage_service.h"

namespace OHOS {
StorageServiceCallbackDeathRecipient::StorageServiceCallbackDeathRecipient()
{
    SSLOG_I("Create StorageServiceCallbackDeathRecipient instance");
}

StorageServiceCallbackDeathRecipient::~StorageServiceCallbackDeathRecipient()
{
    SSLOG_I("Destroy StorageServiceCallbackDeathRecipient instance");
}

void StorageServiceCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    SSLOG_I("device storage service died, remove the proxy object");
    sptr<IStorageServiceCallback> callback = iface_cast<IStorageServiceCallback>(remote.promote());
    SSLOG_I("device storage service died");
    if (!callback) {
        SSLOG_E("callback is nullptr");
        return;
    }
    StorageManager::Instance()->ClearListener();
}
} // namespace OHOS
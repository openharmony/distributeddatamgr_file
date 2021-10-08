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
#include "ipc/storage_service_death_recipient.h"
#include "storage_hilog.h"
#include "storage_service.h"

namespace OHOS {
StorageServiceDeathRecipient::StorageServiceDeathRecipient(const sptr<DeviceStorageService> &service)
    : service_(service)
{
    SSLOG_I("create StorageServiceDeathRecipient instance");
}

StorageServiceDeathRecipient::~StorageServiceDeathRecipient()
{
    SSLOG_I("destroy StorageServiceDeathRecipient instance");
}

void StorageServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    SSLOG_I("storage service died, reconnect the storage service");
    service_->ResetProxy(remote);
    service_->Init();
}
} // namespace OHOS
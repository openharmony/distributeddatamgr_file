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
#include "ipc/storage_service_task_proxy.h"

#include "device_storage_service_hilog.h"
#include "ipc_types.h"
#include "parcel.h"
#include "string_ex.h"

namespace OHOS {
StorageServiceTaskProxy::StorageServiceTaskProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IStorageServiceTask>(impl)
{}

StorageServiceTaskProxy::~StorageServiceTaskProxy() {}

void StorageServiceTaskProxy::OnStatus(int32_t status, const AppExecFwk::PacMap &map)
{
    SSLOG_I("OnStatus status is %{public}d", status);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceTaskProxy::GetDescriptor())) {
        SSLOG_E("StorageServiceTaskProxy::%{public}s write descriptor failed!", __func__);
        return;
    }
    if (!data.WriteInt32(status)) {
        SSLOG_E("fail to call OnStatus, for write status failed");
        return;
    }
    if (!data.WriteParcelable(&map)) {
        SSLOG_E("fail to call OnStatus, for write map failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to call OnStatus, for Remote() is nullptr");
        return;
    }
    int32_t ret = remote->SendRequest(static_cast<int32_t>(IStorageServiceTask::Message::TASK_ON_STATUS),
                                      data, reply, option);
    if (ret != NO_ERROR) {
        SSLOG_W("fail to call OnStatus, for transact is failed, error code is: %{public}d", ret);
    }
}

void StorageServiceTaskProxy::OnFinished(int32_t status, const AppExecFwk::PacMap &map)
{
    SSLOG_I("OnFinished status is %{public}d", status);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceTaskProxy::GetDescriptor())) {
        SSLOG_E("StorageServiceTaskProxy::%{public}s write descriptor failed!", __func__);
        return;
    }
    if (!data.WriteInt32(status)) {
        SSLOG_E("fail to call OnFinished, for write status failed");
        return;
    }
    if (!data.WriteParcelable(&map)) {
        SSLOG_E("fail to call OnFinished, for write map failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to call OnFinished, for Remote() is nullptr");
        return;
    }
    int32_t ret = remote->SendRequest(static_cast<int32_t>(IStorageServiceTask::Message::TASK_ON_FINISHED),
                                      data, reply, option);
    if (ret != NO_ERROR) {
        SSLOG_W("fail to call OnFinished, for transact is failed, error code is: %{public}d", ret);
    }
}
} // namespace OHOS
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
#include "ipc/storage_service_task_stub.h"

#include "device_storage_service_hilog.h"
#include "ipc_types.h"
#include "string_ex.h"

namespace OHOS {
int32_t StorageServiceTaskStub::OnRemoteRequest(uint32_t code,
                                                MessageParcel &data,
                                                MessageParcel &reply,
                                                MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    SSLOG_D("storage service Task Stub onReceived message, the message code is %{public}u", code);
    switch (code) {
        case static_cast<uint32_t>(IStorageServiceTask::Message::TASK_ON_STATUS): {
            int32_t status = data.ReadInt32();
            SSLOG_D("storage service Task Stub onReceived message,status is %{public}d", status);
            std::shared_ptr<AppExecFwk::PacMap> map(data.ReadParcelable<AppExecFwk::PacMap>());
            if (!map) {
                SSLOG_E("readParcelableInfo PacMap failed");
                return false;
            }
            AppExecFwk::PacMap mapInfo = *map;
            OnStatus(status, mapInfo);
            SSLOG_I("Destroy storage service Task Stub instance");
            break;
        }
        case static_cast<uint32_t>(IStorageServiceTask::Message::TASK_ON_FINISHED): {
            int32_t status = data.ReadInt32();
            SSLOG_D("storage service Task Stub onReceived message,status is %{public}d", status);
            std::shared_ptr<AppExecFwk::PacMap> map(data.ReadParcelable<AppExecFwk::PacMap>());
            if (!map) {
                SSLOG_E("readParcelableInfo PacMap failed");
                return false;
            }
            AppExecFwk::PacMap mapInfo = *map;
            OnFinished(status, mapInfo);
            SSLOG_I("Destroy storage service Task Stub instance");
            break;
        }
        default: return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NO_ERROR;
}
} // namespace OHOS
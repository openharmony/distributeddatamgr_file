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
#include "ipc/storage_service_task_stub_impl.h"

#include "device_storage_service_hilog.h"
#include "ipc/storage_service_task_proxy.h"
#include "ipc_types.h"
#include "parcel.h"
#include "string_ex.h"

namespace OHOS {
StorageServiceTaskStubImpl::StorageServiceTaskStubImpl(sptr<DeviceStorageService> &service)
    :service_(service)
{}
StorageServiceTaskStubImpl::~StorageServiceTaskStubImpl() {}
void StorageServiceTaskStubImpl::OnStatus(int32_t status, const AppExecFwk::PacMap &map)
{
    SSLOG_I("call back OnStatus Finished status is %{public}d", status);
    service_->GetDeviceStorageDataMgr()->MoveStatusUpdate(status);
}

void StorageServiceTaskStubImpl::OnFinished(int32_t status, const AppExecFwk::PacMap &map)
{
    SSLOG_I("call back  OnFinished result status is %{public}d", status);
}
} // namespace OHOS
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

#ifndef SERVICES_STORAGE_SERVICE_TASK_STUB_IMPL_H
#define SERVICES_STORAGE_SERVICE_TASK_STUB_IMPL_H

#include "device_storage_service.h"
#include "ipc/storage_service_task_stub.h"
#include "nocopyable.h"

namespace OHOS {
class StorageServiceTaskStubImpl : public StorageServiceTaskStub {
public:
    StorageServiceTaskStubImpl(sptr<DeviceStorageService> &service);
    virtual ~StorageServiceTaskStubImpl();
    virtual void OnStatus(int32_t status, const AppExecFwk::PacMap &map) override;
    virtual void OnFinished(int32_t status, const AppExecFwk::PacMap &map) override;

private:
    sptr<DeviceStorageService> service_;
};
} // namespace OHOS

#endif // SERVICES_STORAGE_SERVICE_TASK_STUB_IMPL_H
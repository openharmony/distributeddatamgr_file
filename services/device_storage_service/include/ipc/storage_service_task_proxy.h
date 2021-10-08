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

#ifndef SERVICES_STORAGE_SERVICE_TASK_PROXY_H
#define SERVICES_STORAGE_SERVICE_TASK_PROXY_H

#include <nocopyable.h>
#include "i_storage_service_task.h"

namespace OHOS {
class StorageServiceTaskProxy : public IRemoteProxy<IStorageServiceTask> {
public:
    StorageServiceTaskProxy(const sptr<IRemoteObject> &impl);
    ~StorageServiceTaskProxy();
    DISALLOW_COPY_AND_MOVE(StorageServiceTaskProxy);
    virtual void OnStatus(int32_t status, const AppExecFwk::PacMap &map) override;
    virtual void OnFinished(int32_t status, const AppExecFwk::PacMap &map) override;

private:
    static inline BrokerDelegator<StorageServiceTaskProxy> delegator_;
};
} // namespace OHOS

#endif // SERVICES_STORAGE_SERVICE_TASK_PROXY_H
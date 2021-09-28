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

#ifndef SERVICES_STORAGE_SERVICE_TASK_INTERFACE_H
#define SERVICES_STORAGE_SERVICE_TASK_INTERFACE_H

#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "ohos/aafwk/content/pac_map.h"

namespace OHOS {
class IStorageServiceTask : public IRemoteBroker {
public:
    enum class Message {
        TASK_ON_STATUS,
        TASK_ON_FINISHED,
    };
    virtual void OnStatus(int32_t status, const AppExecFwk::PacMap &map) = 0;
    virtual void OnFinished(int32_t status, const AppExecFwk::PacMap &map) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"IStorageServiceTask");
};
} // namespace OHOS

#endif // SERVICES_STORAGE_SERVICE_TASK_INTERFACE_H
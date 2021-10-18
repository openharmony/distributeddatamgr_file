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
#ifndef STORAGE_DISKMGR_SERVICES_IDLE_MAINTAIN_H
#define STORAGE_DISKMGR_SERVICES_IDLE_MAINTAIN_H

#include "i_storage_service_task.h"

namespace OHOS {
static const int DIRTY_SEGMENTS_LIMIT = 100;
static const int GC_TIMEOUT_LIMIT = 420; // 7min

void IdleFsTrim(const sptr<IStorageServiceTask> &storageServiceTask);
int RunIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask);
int AbortIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask);
} // namespace OHOS

#endif // STORAGE_DISKMGR_SERVICES_IDLE_MAINTAIN_H
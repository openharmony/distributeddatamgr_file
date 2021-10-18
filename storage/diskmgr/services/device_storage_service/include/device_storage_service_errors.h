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

#ifndef STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_ERRORS_H
#define STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_ERRORS_H
#include <errors.h>

namespace OHOS {
enum {
    E_GET_SYSTEM_ABILITY_MANAGER_FAILED = 1,
    E_GET_STORAGE_SERVICE_FAILED,
    E_ADD_DEATH_RECIPIENT_FAILED,
    E_REGISTER_CALLBACK_FAILED,
    E_READ_PARCEL_ERROR,
    E_WRITE_PARCEL_ERROR,
};
} // namespace OHOS

#endif // STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_ERRORS_H
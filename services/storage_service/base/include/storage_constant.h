/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _STORAGE_SERVICE_CONSTANT_H_
#define _STORAGE_SERVICE_CONSTANT_H_
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <cstring>
#include <memory>
#include <vector>
#include <stddef.h>
namespace OHOS {
namespace StorageService {
namespace Constants {
constexpr uint32_t AID_MEDIA_RW = 1023;
constexpr uint32_t AID_EXT_DATA_RW = 1078;
constexpr uint32_t AID_EXT_OBB_RW = 1079;
constexpr uint32_t AID_ROOT = 0;
constexpr uint32_t AID_SHELL = 2000;
constexpr uint32_t AID_CACHE = 2001;
constexpr uint32_t AID_DIAG = 2002;
constexpr uint32_t AID_USER = 10000;
constexpr uint32_t AID_NOBODY = 9999;
constexpr uint32_t AID_SYSTEM = 1000;
constexpr uint32_t AID_EXTERNAL_STORAGE = 1077;
constexpr uint32_t AID_SDCARD_RW = 1015;
constexpr uint32_t AID_EVERYBODY = 9997;
constexpr uint32_t AID_USER_OFFSET = 10000;
constexpr uint32_t AID_MISC = 9998;
constexpr uint32_t OK = 0;
constexpr uid_t USER_UNKNOWN = ((uid_t)-1);
} // namespace Constants
} // namespace StorageService
} // namespace OHOS

#endif // _STORAGE_SERVICE_CONSTANT_H_
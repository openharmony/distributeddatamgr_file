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

#ifndef STORAGE_DEVICE_STORAGE_CONSTANTS_H
#define STORAGE_DEVICE_STORAGE_CONSTANTS_H

#include <string>

namespace OHOS {
namespace DS {
namespace Constants {
const std::string EMPTY_STRING = "";
const std::string ID_PRIVATE_INTERNAL = "private";
const std::string ID_EMULATED_INTERNAL = "emulated";
const std::string UUID_PRIVATE_INTERNAL = "";
const int32_t USER_SYSTEM = 0;
const int32_t SUB_LENGTH = 8; // emulated length
const int64_t DELAY_TIME = 100;
const int32_t MOVE_STATUS_COPY_FINISHED = 82;
const int32_t SUB_PRI_LENGTH = 8; // private length
const std::string STORAGE_INTERNEL = "Internal shared storage";
const std::string SDCARD = " SD card";
const std::string USB = " Usb drive";
} // namespace Constants
} // namespace DS
} // namespace OHOS
#endif // STORAGE_DEVICE_STORAGE_CONSTANTS_H
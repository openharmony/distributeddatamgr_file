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
#ifndef STORAGE_DISKMGR_SERVICES_MOUNT_INFO_H
#define STORAGE_DISKMGR_SERVICES_MOUNT_INFO_H

#include <list>
#include <string>

namespace OHOS {
static const std::string JOBS_ARR_NAME_IN_JSON = "jobs";
static const std::string CMDS_ARR_NAME_IN_JSON = "cmds";
static const int MAX_JOBS_COUNT = 100;
static const int MAX_JSON_FILE_LEN = 102400;

void ParseInitCfg(const char *configFile, std::list<std::string> &mountList);
} // namespace OHOS

#endif // STORAGE_DISKMGR_SERVICES_MOUNT_INFO_H

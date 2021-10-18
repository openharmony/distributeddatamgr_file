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

#ifndef STORAGE_SERVICE_SS_MOUNT_TABLE_MGR_H
#define STORAGE_SERVICE_SS_MOUNT_TABLE_MGR_H

#include <string>
#include <unistd.h>
#include <vector>
#include "fs_mount_tab.h"

namespace OHOS {
class SsTabMgr {
public:
    bool InsertEntry(const FsMountTab::FsMountEntry &entry);
    bool RemoveEntry(const std::string &mountPoint);
    bool GetEntry(int32_t index, FsMountTab::FsMountEntry &entry);
    bool GetEntry(const std::string &mountPoint, FsMountTab::FsMountEntry &entry);
    size_t EntryCount();

private:
    static std::vector<FsMountTab::FsMountEntry> ssMountTab;
};
} // namespace OHOS
#endif // STORAGE_SERVICE_SS_MOUNT_TABLE_MGR_H
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
#include "ss_tab_mgr.h"

#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <algorithm>
#include <array>
#include <utility>
#include <vector>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include "storage_utils.h"
#include "ss_tab_mgr.h"

using namespace OHOS::FsMountTab;
namespace OHOS {
std::vector<FsMountEntry> SsTabMgr::ssMountTab = {};
bool SsTabMgr::InsertEntry(const FsMountEntry &entry)
{
    if (entry.mountPoint.empty()) {
        return false;
    }
    RemoveEntry(entry.mountPoint);
    ssMountTab.emplace_back(std::move(entry));
    return true;
}
bool SsTabMgr::RemoveEntry(const std::string &mountPoint)
{
    std::vector<FsMountEntry>::iterator it;
    for (it = ssMountTab.begin(); it != ssMountTab.end(); it++) {
        if (it->mountPoint == mountPoint) {
            ssMountTab.erase(it);
            return true;
        }
    }
    return false;
}
bool SsTabMgr::GetEntry(int32_t index, FsMountEntry &entry)
{
    if (index >= ssMountTab.size()) {
        return false;
    }
    entry = ssMountTab.at(index);
    return true;
}
bool SsTabMgr::GetEntry(const std::string &mountPoint, FsMountEntry &entry)
{
    std::vector<FsMountEntry>::iterator it;
    for (it = ssMountTab.begin(); it != ssMountTab.end(); it++) {
        if (it->mountPoint == mountPoint) {
            entry = *it;
            return true;
        }
    }
    return false;
}
size_t SsTabMgr::EntryCount()
{
    return ssMountTab.size();
}

} // namespace OHOS
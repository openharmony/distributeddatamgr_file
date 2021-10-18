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
#include "f2fsmgr.h"

#include <string>
#include <sys/mount.h>
#include <vector>

#include "storage_hilog.h"
#include "storage_utils.h"
#include "utils_string.h"
using namespace OHOS::SsUtils;
namespace OHOS {
namespace StorageService {
const std::string F2fsMgr::kMkfsPath = "/system/bin/make_f2fs";
const std::string F2fsMgr::kFsckPath = "/system/bin/fsck_f2fs";
bool F2fsMgr::IsSupported()
{
    return (access(kMkfsPath.c_str(), X_OK) == 0 && access(kFsckPath.c_str(), X_OK) == 0);
}

int F2fsMgr::Check(const std::string &source)
{
    std::vector<std::string> cmd;
    cmd.push_back("-a");
    cmd.push_back(source);
    return ExecuteCmd(kFsckPath.c_str(), cmd);
}

int F2fsMgr::Mount(const std::string &source, const std::string &target)
{
    unsigned long flags = MS_NOATIME | MS_NODEV | MS_NOSUID | MS_DIRSYNC;

    int res = mount(source.c_str(), target.c_str(), "f2fs", flags, nullptr);
    if (res != 0) {
        SSLOGFE("Failed to mount %s", source.c_str());
        if (errno == EROFS) {
            res = mount(source.c_str(), target.c_str(), "f2fs", flags | MS_RDONLY, nullptr);
            if (res != 0) {
                SSLOGFE("Failed to mount read-only %s", source.c_str());
            }
        }
    }
    return res;
}

int F2fsMgr::Format(const std::string &source)
{
    std::vector<std::string> cmd;
    cmd.push_back(kMkfsPath);
    cmd.push_back(source);
    int rc = ExecuteCmd(kMkfsPath.c_str(), cmd);
    if (rc != 0) {
        SSLOGFE("Filesystem Format failed due to logwrap error");
        errno = EIO;
        return -1;
    }

    SSLOGFI("Filesystem Formatted OK");
    return 0;
}
} // namespace StorageService
} // namespace OHOS

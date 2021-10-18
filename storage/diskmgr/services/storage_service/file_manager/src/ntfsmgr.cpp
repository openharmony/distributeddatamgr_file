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
#include "ntfsmgr.h"

#include <string>
#include <sys/mount.h>
#include <vector>

#include "storage_hilog.h"
#include "storage_utils.h"
#include "utils_string.h"
using namespace OHOS::SsUtils;
namespace OHOS {
namespace StorageService {
const std::string NtfsMgr::kMkfsPath = "/system/bin/mkfs.ntfs";
const std::string NtfsMgr::kFsckPath = "/system/bin/fsck.ntfs";
bool NtfsMgr::IsSupported()
{
    return (access(kMkfsPath.c_str(), X_OK) == 0 && access(kFsckPath.c_str(), X_OK) == 0);
}

int NtfsMgr::Check(const std::string &source)
{
    std::vector<std::string> cmd;
    cmd.push_back("-a");
    cmd.push_back(source);
    return ExecuteCmd(kFsckPath.c_str(), cmd);
}

int NtfsMgr::Mount(const std::string &source, const std::string &target)
{
    unsigned long flags = MS_NOATIME | MS_NODEV | MS_NOSUID | MS_DIRSYNC;

    int res = mount(source.c_str(), target.c_str(), "ntfs", flags, nullptr);
    if (res != 0) {
        SSLOGFE("Failed to mount %s", source.c_str());
        if (errno == EROFS) {
            res = mount(source.c_str(), target.c_str(), "ntfs", flags | MS_RDONLY, nullptr);
            if (res != 0) {
                SSLOGFE("Failed to mount read-only %s", source.c_str());
            }
        }
    }
    return res;
}

int NtfsMgr::Format(const std::string &source)
{
    std::vector<std::string> cmd;

    cmd.push_back(kMkfsPath);
    cmd.push_back(source);
    return ExecuteCmd(kMkfsPath.c_str(), cmd);
}
} // namespace StorageService
} // namespace OHOS

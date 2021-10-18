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
#include "ext4mgr.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <linux/kdev_t.h>

#include "storage_hilog.h"
#include "storage_utils.h"
#include "utils_string.h"

using namespace OHOS::SsUtils;
namespace OHOS {
namespace StorageService {
const std::string Ext4Mgr::kResizefsPath = "/system/bin/resize2fs";
const std::string Ext4Mgr::kMkfsPath = "/system/bin/mke2fs";
const std::string Ext4Mgr::kFsckPath = "/system/bin/e2fsck";

bool Ext4Mgr::IsSupported()
{
    return (access(kMkfsPath.c_str(), X_OK) == 0 && access(kFsckPath.c_str(), X_OK) == 0);
}

int Ext4Mgr::Check(const std::string &source, const std::string &target)
{
    return 0;
}

int Ext4Mgr::Mount(
    const std::string &source, const std::string &target, bool ro, bool remount, bool executable)
{
    unsigned long flags;

    flags = MS_NOATIME | MS_NODEV | MS_NOSUID | MS_DIRSYNC;

    flags |= (executable ? 0 : MS_NOEXEC);
    flags |= (ro ? MS_RDONLY : 0);
    flags |= (remount ? MS_REMOUNT : 0);

    int rc;
    rc = mount(source.c_str(), target.c_str(), "ext4", flags, nullptr);
    if (rc && errno == EROFS) {
        SSLOGFE("%s appears to be a read only filesystem - retrying mount RO", source.c_str());
        flags |= MS_RDONLY;
        rc = mount(source.c_str(), target.c_str(), "ext4", flags, nullptr);
    }
    return rc;
}

int Ext4Mgr::Format(const std::string &source)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    constexpr int blockSize = 4096;
    std::vector<std::string> cmd;

    cmd.push_back(kMkfsPath);
    cmd.push_back("-F");
    cmd.push_back("-t");
    cmd.push_back("ext4");
    cmd.push_back("-q");
    cmd.push_back("-b");
    cmd.push_back(std::to_string(blockSize));
    cmd.push_back(source);
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    int rc = ExecuteCmd(kMkfsPath.c_str(), cmd);
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (rc != 0) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        SSLOGFE("Ext4 Format error !");
        errno = EIO;
        return -1;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return 0;
}
} // namespace StorageService
} // namespace OHOS

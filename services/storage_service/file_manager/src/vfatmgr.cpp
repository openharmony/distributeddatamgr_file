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
#include "vfatmgr.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/kdev_t.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "storage_hilog.h"
#include "storage_utils.h"
#include "utils_string.h"

static constexpr int DIGITAL3 = 3;
static constexpr int DIGITAL4 = 4;
static constexpr int DIGITAL0755 = 755;
using namespace OHOS::SsUtils;
namespace OHOS {
namespace StorageService {
const std::string VfatMgr::kMkfsPath = "/system/bin/newfs_msdos";
const std::string VfatMgr::kFsckPath = "/system/bin/fsck_msdos";
bool VfatMgr::IsSupported()
{
    return (access(kMkfsPath.c_str(), X_OK) == 0 && access(kFsckPath.c_str(), X_OK) == 0);
}

int VfatMgr::Check(const std::string &source)
{
    if (access(kFsckPath.c_str(), X_OK)) {
        SSLOGFW("Skipping fs checks\n");
        return 0;
    }
    int pass = 1;
    int rc = 0;
    do {
        std::vector<std::string> cmd;
        cmd.push_back("-p");
        cmd.push_back("-f");
        cmd.push_back("-y");
        cmd.push_back(source);

        // Fat devices are currently always untrusted
        rc = ExecuteCmd(kFsckPath.c_str(), cmd);
        if (rc == DIGITAL4 && pass++ <= DIGITAL3) {
            SSLOGFW("Filesystem modified - rechecking (pass %d)", pass);
            continue;
        }
        if (rc == 0) {
            SSLOGFI("Filesystem check completed OK");
            return 0;
        } else {
            SSLOGFE("Filesystem check failed due to error");
            return -1;
        }
    } while (0);
    // unexpect loop end
    return -1;
}

int VfatMgr::Mount(const std::string &source,
                   const std::string &target,
                   bool ro,
                   bool remount,
                   bool executable,
                   int ownerUid,
                   int ownerGid,
                   int permMask,
                   bool createLost)
{
    int rc;
    unsigned long flags;
    flags = MS_NODEV | MS_NOSUID | MS_DIRSYNC | MS_NOATIME;
    flags |= (executable ? 0 : MS_NOEXEC);
    flags |= (ro ? MS_RDONLY : 0);
    flags |= (remount ? MS_REMOUNT : 0);
    auto mountData = SsUtils::StringPrintf("utf8,uid=%d,gid=%d,fmask=%o,dmask=%o,shortname=mixed", ownerUid,
                                           ownerGid, permMask, permMask);
    rc = mount(source.c_str(), target.c_str(), "vfat", flags, mountData.c_str());
    if (rc && errno == EROFS) {
        SSLOGFE(" %s appears to be a read only filesystem - retrying mount RO", source.c_str());
        flags |= 1;
        rc = mount(source.c_str(), target.c_str(), "vfat", flags, mountData.c_str());
    }

    if (rc == 0 && createLost) {
        auto lostPath = SsUtils::StringPrintf("%s/LOST.DIR", target.c_str());
        if (access(lostPath.c_str(), F_OK)) {
            if (mkdir(lostPath.c_str(), DIGITAL0755)) {
                SSLOGFE("Unable to Create LOST.DIR");
            }
        }
    }
    return rc;
}

int VfatMgr::Format(const std::string &source)
{
    std::vector<std::string> cmd;

    cmd.push_back(kMkfsPath);
    cmd.push_back("-p"); // Fat type
    cmd.push_back("-f");
    cmd.push_back("-y");
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

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
#include "exfatmgr.h"

#include <sys/mount.h>

#include "storage_hilog.h"
#include "storage_utils.h"
#include "utils_string.h"
using namespace OHOS::SsUtils;
namespace OHOS {
namespace StorageService {
const std::string ExfatMgr::kMkfsPath = "/system/bin/mkfs.exfat";
const std::string ExfatMgr::kFsckPath = "/system/bin/fsck.exfat";
bool ExfatMgr::IsSupported()
{
    return (access(kMkfsPath.c_str(), X_OK) == 0 && access(kFsckPath.c_str(), X_OK) == 0);
}

int ExfatMgr::Check(const std::string &source)
{
    std::vector<std::string> cmd;
    cmd.push_back("-a");
    cmd.push_back(source);
    int rc = ExecuteCmd(kFsckPath.c_str(), cmd);
    if (rc == 0) {
        SSLOGFI("Exfat check OK!");
        return 0;
    } else {
        SSLOGFE("Exfat check failed with error code %d", rc);
        errno = EIO;
        return -1;
    }
}

int ExfatMgr::Mount(
    const std::string &source, const std::string &target, int ownerUid, int ownerGid, int permMask)
{
    int mountFlags = MS_NODEV | MS_NOSUID | MS_DIRSYNC | MS_NOATIME | MS_NOEXEC;
    auto mountData =
        SsUtils::StringPrintf("uid=%d,gid=%d,fmask=%o,dmask=%o", ownerUid, ownerGid, permMask, permMask);
    if (mount(source.c_str(), target.c_str(), "exfat", mountFlags, mountData.c_str()) == 0) {
        SSLOGFI("Exfat mount successful!");
        return 0;
    }

    SSLOGFE("Mount failed; attempting read-only");
    mountFlags |= MS_RDONLY;
    if (mount(source.c_str(), target.c_str(), "exfat", mountFlags, mountData.c_str()) == 0) {
        SSLOGFI("Exfat mount read only successful!");
        return 0;
    }
    SSLOGFE("Exfat Mount Read Only failed");
    return -1;
}

int ExfatMgr::Format(const std::string &source)
{
    std::vector<std::string> cmd;

    cmd.push_back(kMkfsPath);
    cmd.push_back("-n");
    cmd.push_back("ohos"); // Volume label marked as ohos
    cmd.push_back(source);

    int rc = ExecuteCmd(kMkfsPath.c_str(), cmd);
    if (rc != 0) {
        SSLOGFE("Format failed (code %d)", rc);
        errno = EIO;
        return -1;
    }
    SSLOGFI("Format OK");
    return 0;
}
} // namespace StorageService
} // namespace OHOS

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd. 2021-2021.
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
#ifndef STORAGE_SERVICE_FS_MGR_H
#define STORAGE_SERVICE_FS_MGR_H

#include "ext4.h"
#include "fs_mount_tab.h"
namespace OHOS {
class FsMgr {
public:
    static ext4_fsblk_t Ext4BlocksCount(const struct ext4_super_block *es);
    static bool Tune2FsAvailable(void);
    static bool RunTune2fs(const std::vector<std::string> argv);
    static ext4_fsblk_t Ext4rBlocksCount(const struct ext4_super_block *es);
    static void TuneReservedSize(const std::string blkDev,
                                 FsMountTab::FsMountEntry rec,
                                 const struct ext4_super_block *sb,
                                 int &fsStat);
    static bool IsEXTFS(std::string &fsType);
    static bool ReadExt4Superblock(std::string blkDev, ext4_super_block *sb, int &fsStat);
    static int FsMgrIsQuota(const struct FsMountTab::FsMountEntry entry);
    static bool FsMgrIsFileEncrypted(const struct FsMountTab::FsMountEntry fstab,
                                     const struct ext4_super_block *sb);
    static void TuneEncrypt(const std::string blkDev,
                            const struct FsMountTab::FsMountEntry fstab,
                            const struct ext4_super_block *sb,
                            int &fsStat);
    static bool shouldForceCheck(int fsStat);
	static void CheckExtFsMountAndUnmount(const std::string blkDev,
                                          std::string fsType,
                                          std::string target,
                                          std::string tmpmntOpts,
                                          int &fsStat);
    static void CheckFs(const std::string blkDev, std::string fsType, std::string target, int &fsStat);
    int PrepareFsForMount(std::string blkDev, FsMountTab::FsMountEntry &rec);
};
} // namespace OHOS

#endif // STORAGE_SERVICE_FS_MGR_H
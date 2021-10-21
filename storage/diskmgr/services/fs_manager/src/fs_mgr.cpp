/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "fs_mgr.h"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <pthread.h>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/magic.h>

#include "filesystem_log.h"
#include "storage_utils.h"

#define LOG_KLOG 2
#define LOG_FILE 4
#define MF_QUOTA 0x400000
#define MF_CHECK 0x2
#define MF_RESERVEDSIZE 0x200000
#define MF_FILEENCRYPTION 0x2000
#define TUNE2FS_BIN "/system/bin/tune2fs"
#define E2FSCK_BIN "/system/bin/e2fsck"
#define F2FS_FSCK_BIN "/system/bin/fsck.f2fs"

#define PRIu64 "llu"
namespace OHOS {
ext4_fsblk_t FsMgr::Ext4BlocksCount(const struct ext4_super_block *es)
{
    ext4_fsblk_t reservedBlocks;
    reservedBlocks =
        ((ext4_fsblk_t)le32_to_cpu(es->s_blocks_count_hi) << 32) | le32_to_cpu(es->s_blocks_count_lo);
    return reservedBlocks;
}

bool FsMgr::Tune2FsAvailable(void)
{
    return access(TUNE2FS_BIN, X_OK) == 0;
}

bool FsMgr::RunTune2fs(const std::vector<std::string> argv)
{
    int ret;
    ret = OHOS::SsUtils::ExecuteCmd(argv[1].c_str(), argv, nullptr);
    return ret == 0;
}

ext4_fsblk_t FsMgr::Ext4rBlocksCount(const struct ext4_super_block *es)
{
    ext4_fsblk_t ext4rBlocksCount;
    ext4rBlocksCount =
        ((ext4_fsblk_t)le32_to_cpu(es->s_r_blocks_count_hi) << 32) | le32_to_cpu(es->s_r_blocks_count_lo);
    return ext4rBlocksCount;
}
// Resize the reserved block and set the reserved block
void FsMgr::TuneReservedSize(const std::string blkDev,
                             FsMountTab::FsMountEntry rec,
                             const struct ext4_super_block *sb,
                             int &fsStat)
{
    // maximum reserved block size of% 2.
    // maximum number of blocks directly reserved
    const uint64_t reservedBlocks = FsMgr::Ext4BlocksCount(sb) * 0.02;
    // Check the type of equipment and whether the reserved device block is correct
    if ((!(rec.mountFlags & MF_RESERVEDSIZE)) && (FsMgr::Ext4rBlocksCount(sb) == reservedBlocks)) {
        return;
    }
    // Check if there is / system/bin/tune2fs function
    if (!FsMgr::Tune2FsAvailable()) {
        SSLOG_E("Unable to set the number of reserved blocks on because %s is missing\n", TUNE2FS_BIN);
        std::cout << "Unable to set the number of reserved blocks on " << blkDev
                  << " because " TUNE2FS_BIN " is missing";
        return;
    }
    char buf[32];
    std::vector<std::string> argv = { TUNE2FS_BIN, "-r", buf, blkDev };
    snprintf(buf, sizeof(buf), "%" PRIu64, reservedBlocks); // Related to the above
    std::cout << "Setting reserved block count on " << blkDev << " to " << reservedBlocks;
    // Set reserved blocks to adjust system reserved space
    if (!FsMgr::RunTune2fs(argv)) {
        std::cout << "Failed to run " TUNE2FS_BIN " to set the number of reserved blocks on " << blkDev;
        fsStat |= FS_STAT_SET_RESERVED_BLOCKS_FAILED;
    }
}
// Determine whether the hard drive to be mounted is ext4 or ext3 or ext2
bool FsMgr::IsEXTFS(std::string &fsType)
{
    if (fsType == "ext4" || fsType == "ext3" || fsType == "ext2") {
        return true;
    }
    return false;
}

bool FsMgr::ReadExt4Superblock(std::string blkDev, ext4_super_block *sb, int &fsStat)
{
    // O_RDONLY opens the device in read-only mode the file descriptor opened in O_CLOEXEC mode is closed
    // during the execution of the exec call to the new program
    int fd;
    fd = open(blkDev.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        std::cout << "Failed to open '" << blkDev << "'";
        return false;
    }
    // From the same address in the virtual memory, read the data with the same location of the actual address
    // into the sb
    if (pread(fd, sb, sizeof(*sb), 1024) != sizeof(*sb)) {
        std::cout << "Can't read '" << blkDev << "' superblock";
        return false;
    }
    // The detected fs is used to determine whether the mount will fail.
    if (sb->s_magic != EXT4_SUPER_MAGIC) {
        std::cout << "Invalid ext4 magic:0x" << std::hex << sb->s_magic << " "
                  << "on '" << blkDev << "'";
        fsStat |= FS_STAT_EXT4_INVALID_MAGIC;
        return false;
    }
    fsStat |= FS_STAT_IS_EXT4;
    std::cout << "superblock s_max_mnt_count:" << sb->s_max_mnt_count << "," << blkDev;
    if (sb->s_max_mnt_count == 0xffff) {
        fsStat |= FS_STAT_NEW_IMAGE_VERSION;
    }
    return true;
}

int FsMgr::FsMgrIsQuota(const struct FsMountTab::FsMountEntry entry)
{
    return entry.mountFlags & MF_QUOTA;
}

static void TuneQuota(std::string blkDev,
                      const struct FsMountTab::FsMountEntry rec,
                      const struct ext4_super_block *sb,
                      int &fsStat)
{
    bool hasQuota = (sb->s_feature_ro_compat & cpu_to_le32(EXT4_FEATURE_RO_COMPAT_QUOTA)) != 0;
    bool wantQuota = FsMgr::FsMgrIsQuota(rec) != 0;
    if (hasQuota == wantQuota) {
        return;
    }
    // Check if the command is available
    if (!FsMgr::Tune2FsAvailable()) {
        std::cout << "Unable to " << (wantQuota ? "enable" : "disable") << " quotas on " << blkDev
                  << " because " TUNE2FS_BIN " is missing";
        return;
    }
    std::vector<std::string> argv;
    if (wantQuota) {
        std::cout << "Enabling quotas on " << blkDev;
        argv = { TUNE2FS_BIN, "-Oquota", "-Qusrquota,grpquota", blkDev.c_str() };
        fsStat |= FS_STAT_QUOTA_ENABLED;
    } else {
        std::cout << "Disabling quotas on " << blkDev;
        argv = { TUNE2FS_BIN, "-O^quota", "-Q^usrquota,^grpquota", blkDev.c_str() };
    }
    if (!FsMgr::RunTune2fs(argv)) {
        std::cout << "Failed to run " TUNE2FS_BIN " to " << (wantQuota ? "enable" : "disable")
                  << " quotas on " << blkDev;
        fsStat |= FS_STAT_TOGGLE_QUOTAS_FAILED;
    }
}

bool FsMgr::FsMgrIsFileEncrypted(const struct FsMountTab::FsMountEntry fstab,
                                 const struct ext4_super_block *sb)
{
    if ((sb->s_feature_incompat & cpu_to_le32(EXT4_FEATURE_INCOMPAT_ENCRYPT)) != 0) {
        return true;
    }
    if ((fstab.mountFlags & MF_FILEENCRYPTION) == 0) {
        return true;
    }
    return false;
}

void FsMgr::TuneEncrypt(const std::string blkDev,
                        const struct FsMountTab::FsMountEntry fstab,
                        const struct ext4_super_block *sb,
                        int &fsStat)
{
    bool wantEncrypt = FsMgr::FsMgrIsFileEncrypted(fstab, sb);
    if (wantEncrypt) {
        return;
    }
    // Check if there is / system/bin/tune2fs function
    if (!FsMgr::Tune2FsAvailable()) {
        std::cout << "Unable to enable ext4 encryption on " << blkDev
                  << " because " TUNE2FS_BIN " is missing";
        return;
    }

    const std::vector<std::string> argv = { TUNE2FS_BIN, "-Oencrypt", blkDev };

    std::cout << "Enabling ext4 encryption on " << blkDev;
    if (!FsMgr::RunTune2fs(argv)) {
        std::cout << "Failed to run " TUNE2FS_BIN " to enable "
                  << "ext4 encryption on " << blkDev;
        fsStat |= FS_STAT_ENABLE_ENCRYPTION_FAILED;
    }
}

bool FsMgr::shouldForceCheck(int fsStat)
{
    if (fsStat & (FS_STAT_E2FSCK_F_ALWAYS | FS_STAT_UNCLEAN_SHUTDOWN | FS_STAT_QUOTA_ENABLED |
                  FS_STAT_RO_MOUNT_FAILED | FS_STAT_RO_UNMOUNT_FAILED | FS_STAT_FULL_MOUNT_FAILED |
                  FS_STAT_E2FSCK_FAILED | FS_STAT_TOGGLE_QUOTAS_FAILED | FS_STAT_SET_RESERVED_BLOCKS_FAILED |
                  FS_STAT_ENABLE_ENCRYPTION_FAILED)) {
        return true;
    }
    return false;
}

void FsMgr::CheckExtFsMountAndUnmount(const std::string blkDev,
                                      std::string fsType,
                                      std::string target,
                                      std::string tmpmntOpts,
                                      int &fsStat)
{
    long tmpmntFlags = MS_NOATIME | MS_NOEXEC | MS_NOSUID;
    int ret = mount(blkDev.c_str(), target.c_str(), fsType.c_str(), tmpmntFlags, tmpmntOpts.c_str());
    std::cout << __FUNCTION__ << "(): mount(" << blkDev << "," << target << "," << fsType
                << ")=" << ret;
    if (!ret) {
        bool umounted = false;
        int retry_count = 5;
        while (retry_count-- > 0) {
            umounted = umount(target.c_str()) == 0;
            if (umounted) {
                std::cout << __FUNCTION__ << "(): unmount(" << target << ") succeeded";
                break;
            }
            std::cout << __FUNCTION__ << "(): umount(" << target << ") failed";
            if (retry_count) {
                sleep(1);
            }
        }
        if (!umounted) {
            // boot may fail but continue and leave it to later stage for now.
            std::cout << __FUNCTION__ << "(): umount(" << target << ") timed out";
            fsStat |= FS_STAT_RO_UNMOUNT_FAILED;
        }
    } else {
        fsStat |= FS_STAT_RO_MOUNT_FAILED;
    }
}

// Check the file system
void FsMgr::CheckFs(const std::string blkDev, std::string fsType, std::string target, int &fsStat)
{
    int ret;
    std::string tmpmntOpts = "errors=remount-ro";
    const std::vector<std::string> e2fsck_argv = { E2FSCK_BIN, "-y", blkDev };
    const std::vector<std::string> e2fsck_forced_argv = { E2FSCK_BIN, "-f", "-y", blkDev };
    // Determine whether the hard drive to be mounted is ext4 or ext3 or ext2
    if (FsMgr::IsEXTFS(fsType)) {
        if (fsStat & FS_STAT_EXT4_INVALID_MAGIC) {
            return;
        }
        if (!(fsStat & FS_STAT_FULL_MOUNT_FAILED)) { 
            errno = 0;
            if (!strcmp(fsType.c_str(), "ext4")) {
                // This option is only valid with ext4
                tmpmntOpts.append(",nomblk_io_submit");
            }
            FsMgr::CheckExtFsMountAndUnmount(blkDev, fsType, target, tmpmntOpts, fsStat);
        }
        // Check if there is / system/bin/e2fsck
        if (access(E2FSCK_BIN, X_OK)) {
            std::cout << "Not running " << E2FSCK_BIN << " on " << blkDev
                      << " (executable not in system image)";
        } else {
            std::cout << "Running " << E2FSCK_BIN << " on " << blkDev;
            if (FsMgr::shouldForceCheck(fsStat)) {
                //  Disk check-y: preset all checking questions to be answered
                ret = OHOS::SsUtils::ExecuteCmd(e2fsck_forced_argv[1].c_str(), e2fsck_forced_argv, nullptr);
            } else {
                //  Disk forced check-f: mandatory check
                ret = OHOS::SsUtils::ExecuteCmd(e2fsck_argv[1].c_str(), e2fsck_argv, nullptr);
            }

            if (ret < 0) {
                std::cout << "Failed trying to run " << E2FSCK_BIN;
                //  I don't know if this else is needed.
                fsStat |= FS_STAT_E2FSCK_FAILED;
            } else {
                // As long as the ret is not less than 0, it is calculated according to the repair success.
                fsStat |= FS_STAT_E2FSCK_FS_FIXED;
            }
        }
    } else if (!strcmp(fsType.c_str(), "f2fs")) {
        const std::vector<std::string> f2fs_fsck_argv = { F2FS_FSCK_BIN, "-a", blkDev.c_str() };
        std::cout << "Running " << F2FS_FSCK_BIN << " -a " << blkDev;
        // Check the f2fs file system
        ret = OHOS::SsUtils::ExecuteCmd(f2fs_fsck_argv[1].c_str(), f2fs_fsck_argv, nullptr);
        if (ret < 0) {
            std::cout << "Failed trying to run " << F2FS_FSCK_BIN;
        }
    }
    return;
}

int FsMgr::PrepareFsForMount(std::string blkDev, FsMountTab::FsMountEntry &rec)
{
    int fsStat = 0;
    // First of all, determine whether it is ext4 or ext3 or ext2.
    if (FsMgr::IsEXTFS(rec.fsType)) {
        // First of all, determine whether it is ext4 or ext3 or ext2.
        struct ext4_super_block sb;
        // Read the super block information of the ext4 device that needs to be mounted
        if (FsMgr::ReadExt4Superblock(blkDev, &sb, fsStat)) {
            // Determine whether the open device is completely closed
            if ((sb.s_feature_incompat & EXT4_FEATURE_INCOMPAT_RECOVER) != 0 ||
                (sb.s_state & EXT4_VALID_FS) == 0) {
                std::cout << "Filesystem on " << blkDev << " was not cleanly shutdown; "
                          << "state flags: 0x" << std::hex << sb.s_state << ", "
                          << "incompat feature flags: 0x" << std::hex << sb.s_feature_incompat;
                fsStat |= FS_STAT_UNCLEAN_SHUTDOWN;
            }
            // Assign quotas to devices before running fsck
            TuneQuota(blkDev, rec, &sb, fsStat);
        } else {
            return fsStat;
        }
    }

    if ((rec.mountFlags & MF_CHECK) || (fsStat & (FS_STAT_UNCLEAN_SHUTDOWN | FS_STAT_QUOTA_ENABLED))) {
        FsMgr::CheckFs(blkDev, rec.fsType, rec.mountPoint, fsStat);
    }

    if (FsMgr::IsEXTFS(rec.fsType) && (rec.mountFlags & (MF_RESERVEDSIZE | MF_FILEENCRYPTION))) {
        struct ext4_super_block sb;
        // Read the super block information of the ext4 device that needs to be mounted
        if (FsMgr::ReadExt4Superblock(blkDev, &sb, fsStat)) {
            // Resize the reserved block and set the reserved block
            FsMgr::TuneReservedSize(blkDev, rec, &sb, fsStat);

            FsMgr::TuneEncrypt(blkDev, rec, &sb, fsStat);
        }
    }

    return fsStat;
}
} // namespace OHOS

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
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/inotify.h>
#include <sys/mount.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <linux/fuse.h>

#include "storage_utils.h"
#include "storage_constant.h"
#include "utils_string.h"
#include "storage_hilog.h"

using namespace OHOS::SsUtils;
using namespace OHOS::StorageService::Constants;
using userid_t = uid_t;
namespace OHOS {
namespace SdcardFs {
static constexpr int BASE_FLAGS = 10;
static constexpr int RLIM_FLAGS = 8192;
struct SdcardFsOption {
    uid_t uid;
    gid_t gid;
    userid_t userid;
    bool multiUser;
    bool fullWrite;
    bool deriveGid;
    bool defaultNormal;
    bool unsharedObb;
    std::string sourcePath;
    std::string label;
} fsOption = {};

void InitOption()
{
    fsOption.uid = 0;
    fsOption.gid = 0;
    fsOption.userid = 0;
    fsOption.label.clear();
    fsOption.sourcePath.clear();
    fsOption.multiUser = false;
    fsOption.fullWrite = false;
    fsOption.deriveGid = false;
    fsOption.defaultNormal = false;
    fsOption.unsharedObb = false;
}

bool ValidOption()
{
    if ((fsOption.gid == 0) || (fsOption.uid == 0)) {
        SSLOGFE("uid and gid must be nonzero");
        return false;
    }
    if (fsOption.sourcePath.empty()) {
        SSLOGFE("source path must be specified");
        return false;
    }
    if (fsOption.label.empty()) {
        SSLOGFE("label must be specified");
        return false;
    }
    return true;
}

bool IsEsdFsSupported(void)
{
    std::string filesystems;
    if (!ReadFile("/proc/filesystems", &filesystems)) {
        SSLOGFE("Could not read /proc/filesystems");
        return false;
    }

    for (const auto &fs : Split(filesystems, "\n")) {
        if (fs.find("esdfs") != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool Setup(const std::string &destPath, gid_t gid, mode_t mask, bool useEsdfs)
{
    // Add new options at the end of the vector.
    std::vector<std::string> optsList;
    if (fsOption.multiUser) {
        optsList.push_back("multiuser,");
    }
    if (fsOption.deriveGid) {
        optsList.push_back("derive_gid,");
    }
    if (fsOption.defaultNormal) {
        optsList.push_back("default_normal,");
    }
    if (fsOption.unsharedObb) {
        optsList.push_back("unshared_obb,");
    }

    for (size_t i = 0; i <= optsList.size(); ++i) {
        std::string newOpts;
        for (size_t j = 0; j < optsList.size() - i; ++j) {
            newOpts += optsList[j];
        }

        auto opts = StringPrintf("fsuid=%d,fsgid=%d,%smask=%d,userid=%d,gid=%d", fsOption.uid, fsOption.gid,
                                 newOpts.c_str(), mask, fsOption.userid, gid);
        if (mount(fsOption.sourcePath.c_str(), destPath.c_str(), useEsdfs ? "esdfs" : "sdcardfs",
                  MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME, opts.c_str()) == -1) {
            SSLOGFE("Failed to mount sdcardfs with options %s", opts.c_str());
        } else {
            return true;
        }
    }

    return false;
}

bool SetupRemount(const std::string &srcPath, const std::string &destPath, gid_t gid, mode_t mask)
{
    std::string opts = StringPrintf("mask=%d,gid=%d", mask, gid);

    if (mount(srcPath.c_str(), destPath.c_str(), nullptr, MS_BIND, nullptr) != 0) {
        SSLOGFE("failed to bind mount sdcardfs filesystem");
        return false;
    }

    if (mount(srcPath.c_str(), destPath.c_str(), "none",
              MS_REMOUNT | MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME, opts.c_str()) != 0) {
        SSLOGFE("failed to mount sdcardfs filesystem");
        if (umount2(destPath.c_str(), MNT_DETACH))
            SSLOGFW("Failed to unmount bind");
        return false;
    }

    return true;
}

void Run()
{
    std::string pathDefault = "/mnt/runtime/default/" + fsOption.label;
    std::string pathRead = "/mnt/runtime/read/" + fsOption.label;
    std::string pathWrite = "/mnt/runtime/write/" + fsOption.label;
    std::string pathFull = "/mnt/runtime/full/" + fsOption.label;
    bool useEsdfs = IsEsdFsSupported();
    umask(0);
    bool ret = false;
    if (fsOption.multiUser) {
        ret |= Setup(pathDefault, AID_SDCARD_RW, 0006, useEsdfs);
        if (useEsdfs) {
            ret |= Setup(pathRead, AID_EVERYBODY, 0027, useEsdfs);
            ret |= Setup(pathWrite, AID_EVERYBODY, fsOption.fullWrite ? 0007 : 0027, useEsdfs);
            ret |= Setup(pathFull, AID_EVERYBODY, 0007, useEsdfs);
        } else {
            ret |= SetupRemount(pathDefault, pathRead, AID_EVERYBODY, 0027);
            ret |= SetupRemount(pathDefault, pathWrite, AID_EVERYBODY, fsOption.fullWrite ? 0007 : 0027);
            ret |= SetupRemount(pathDefault, pathFull, AID_EVERYBODY, 0007);
        }
        if (!ret) {
            SSLOG_F("failed to sdcardfs_setup");
        }
    } else {
        bool ret = false;
        ret |= Setup(pathDefault, AID_SDCARD_RW, 0006, useEsdfs);
        if (useEsdfs) {
            ret |= Setup(pathRead, AID_EVERYBODY, fsOption.fullWrite ? 0027 : 0022, useEsdfs);
            ret |= Setup(pathWrite, AID_EVERYBODY, fsOption.fullWrite ? 0007 : 0022, useEsdfs);
            ret |= Setup(pathFull, AID_EVERYBODY, 0007, useEsdfs);
        } else {
            ret |= SetupRemount(pathDefault, pathRead, AID_EVERYBODY, fsOption.fullWrite ? 0027 : 0022);
            ret |= SetupRemount(pathDefault, pathWrite, AID_EVERYBODY, fsOption.fullWrite ? 0007 : 0022);
            ret |= SetupRemount(pathDefault, pathFull, AID_EVERYBODY, 0007);
        }
    }
    if (!ret) {
        SSLOG_F("failed to sdcardfs_setup");
    }
    // Will abort if priv-dropping fails.

    if (fsOption.multiUser) {
        std::string obbPath = fsOption.sourcePath + "/obb";
        PrepareDir(obbPath, 0775, fsOption.uid, fsOption.gid);
    }

    exit(0);
}
} // namespace SdcardFs
} // namespace OHOS

static int usage()
{
    fprintf(stdout, "usage: sdcard [OPTIONS] <source_path> <label>"
                    "    -u: specify UID to run as"
                    "    -g: specify GID to run as"
                    "    -U: specify user ID that owns device"
                    "    -m: source_path is multi-user"
                    "    -w: runtime write mount has full write access"
                    "    -P: preserve owners on the lower file system"
                    "    -o: obb dir doesn't need to be shared between users");
    return 1;
}

int main(int argc, char **argv)
{
    int i;
    struct rlimit rlim;
    int opt;
    OHOS::SdcardFs::InitOption();
    while ((opt = getopt(argc, argv, "u:g:U:mwGio")) != -1) {
        switch (opt) {
            case 'u': OHOS::SdcardFs::fsOption.uid = strtoul(optarg, NULL, OHOS::SdcardFs::BASE_FLAGS); break;
            case 'g': OHOS::SdcardFs::fsOption.gid = strtoul(optarg, NULL, OHOS::SdcardFs::BASE_FLAGS); break;
            case 'U': OHOS::SdcardFs::fsOption.userid = strtoul(optarg, NULL, OHOS::SdcardFs::BASE_FLAGS); break;
            case 'm': OHOS::SdcardFs::fsOption.multiUser = true; break;
            case 'w': OHOS::SdcardFs::fsOption.fullWrite = true; break;
            case 'G': OHOS::SdcardFs::fsOption.deriveGid = true; break;
            case 'i': OHOS::SdcardFs::fsOption.defaultNormal = true; break;
            case 'o': OHOS::SdcardFs::fsOption.unsharedObb = true; break;
            case '?':
            default: fprintf(stderr, "Unknown option: '%c'", opt); return usage();
        }
    }

    for (i = optind; i < argc; i++) {
        char *arg = argv[i];
        if (OHOS::SdcardFs::fsOption.sourcePath.empty()) {
            OHOS::SdcardFs::fsOption.sourcePath.append(arg);
        } else if (OHOS::SdcardFs::fsOption.label.empty()) {
            OHOS::SdcardFs::fsOption.label.append(arg);
        } else {
            fprintf(stderr, "too many arguments");
            return usage();
        }
    }
    if (!OHOS::SdcardFs::ValidOption()) {
        fprintf(stderr, "Option incorrect");
        return usage();
    }

    rlim.rlim_cur = OHOS::SdcardFs::RLIM_FLAGS;
    rlim.rlim_max = OHOS::SdcardFs::RLIM_FLAGS;
    if (setrlimit(RLIMIT_NOFILE, &rlim) == -1) {
        fprintf(stderr, "setting RLIMIT_NOFILE failed");
    }

    OHOS::SdcardFs::Run();
    return 1;
}

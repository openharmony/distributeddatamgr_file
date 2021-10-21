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
#include "storage_utils.h"

#include <cctype>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <list>
#include <memory>
#include <mntent.h>
#include <mutex>
#include <numeric>
#include <poll.h>
#include <pwd.h>
#include <regex>
#include <string>
#include <sys/sysmacros.h>
#include <sys/statvfs.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/xattr.h>
#include <thread>
#include <unistd.h>
#include <unordered_set>
#include <utility>

#include <linux/fs.h>
#include <linux/posix_acl.h>
#include <linux/posix_acl_xattr.h>

#include "utils_file.h"
#include "utils_string.h"
#include "storage_hilog.h"

namespace OHOS {
namespace SsUtils {
bool storageSleepOnUnMount = true;

static const std::string toyBoxPath = "/system/bin/toybox";
static const std::string keyPath = "/data/misc/storage";
static const std::string procFilesystems = "/proc/filesystems";
static const std::string PROC_DEVICES = "/proc/devices";

static constexpr int OK = 0;
static pid_t localForkPid = -1;
static int STORAGE_FIVE;
static int numberSixteen = 16;
static int numberNine = 9;
#define CMDNUM 100

bool IsSdcardfsUsed()
{
    return IsFilesystemSupported("sdcardfs");
}

bool IsFilesystemSupported(const std::string &fsType)
{
    std::string supported;
    if (!ReadFile(procFilesystems, &supported)) {
        return false;
    }
    for (auto &type : Split(supported, "\n")) {
        if (type == fsType) {
            return true;
        }
    }
    return false;
}

int MountForBind(const std::string &source, const std::string &target)
{
    if (TEMP_FAILURE_RETRY(umount2(target.c_str(), MNT_DETACH)) < 0 && errno != EINVAL && errno != ENOENT) {
        SSLOGFE("target %{public}s Unmount failed", target.c_str());
        return -1;
    }
    if (TEMP_FAILURE_RETRY(mount(source.c_str(), target.c_str(), nullptr, MS_BIND, nullptr)) < 0) {
        SSLOGFE("Failed to bind Mount %{public}s to %{public}s", source.c_str(), target.c_str());
        return -1;
    }
    return 0;
}

bool CheckMaps(const std::string &path, const std::string &prefix)
{
    std::string maps;
    if (!ReadFile(path, &maps)) {
        SSLOGFE("read map string error!");
        return false;
    }
    for (auto &item : Split(maps, "\n")) {
        size_t n = item.find('/');
        if (n == std::string::npos) {
            continue;
        }
        if (StartsWith(item.substr(n), prefix)) {
            return true;
        }
    }
    return false;
}

int GetPid(const char *s)
{
    int tenPid = 10;
    int pid = 0;
    while (*s) {
        if (!isdigit(*s)) {
            return -1;
        }
        pid = tenPid * pid + ((*s) - '0');
        s++;
    }
    return pid;
}

int KillProcWithFiles(const std::string &prefix, int signal)
{
    auto pd = std::unique_ptr<DIR, int (*)(DIR *)>(opendir("/proc"), closedir);
    if (pd == nullptr) {
        SSLOGFE("Failed to open proc");
        return -1;
    }
    struct dirent *procEntry = nullptr;
    while ((procEntry = readdir(pd.get())) != nullptr) {
        pid_t pid;
        if (procEntry->d_type != DT_DIR)
            continue;
        pid = GetPid(procEntry->d_name);
        if (pid == -1)
            continue;

        bool needKill = false;
        auto path = StringPrintf("/proc/%d/", pid);
        needKill |= CheckMaps(path + "maps", prefix);
        needKill |= CheckSymlink(path + "cwd", prefix);
        needKill |= CheckSymlink(path + "root", prefix);
        needKill |= CheckSymlink(path + "exe", prefix);

        auto fdPath = path + "fd";
        auto fdd = std::unique_ptr<DIR, int (*)(DIR *)>(opendir(fdPath.c_str()), closedir);
        if (!fdd) {
            SSLOGFE("Failed to open %{public}s", fdPath.c_str());
            continue;
        }
        struct dirent *fdEntry;
        while ((fdEntry = readdir(fdd.get())) != nullptr) {
            if (fdEntry->d_type != DT_LNK) {
                continue;
            }
            needKill |= CheckSymlink(fdPath + "/" + fdEntry->d_name, prefix);
        }
        if (needKill && signal != 0) {
            if (pid != localForkPid) {
                kill(pid, signal);
            }
        }
    }
    return 0;
}

int KillProcForPath(const std::string &path)
{
    if (KillProcWithFiles(path, SIGINT) == 0) {
        return 0;
    }
    if (storageSleepOnUnMount) {
        sleep(STORAGE_FIVE);
    }

    if (KillProcWithFiles(path, SIGTERM) == 0) {
        return 0;
    }
    if (storageSleepOnUnMount) {
        sleep(STORAGE_FIVE);
    }

    if (KillProcWithFiles(path, SIGKILL) == 0) {
        return 0;
    }
    if (storageSleepOnUnMount) {
        sleep(STORAGE_FIVE);
    }

    if (KillProcWithFiles(path, SIGKILL) == 0) {
        return 0;
    }
    SSLOGFE("Failed to kill processes using %{public}s", path.c_str());
    return -1;
}

int ForceUnMount(const std::string &path)
{
    if (!umount2(path.c_str(), UMOUNT_NOFOLLOW) || errno == EINVAL || errno == ENOENT) {
        return OK;
    }
    if (storageSleepOnUnMount) {
        sleep(STORAGE_FIVE);
    }

    KillProcWithFiles(path, SIGINT);
    if (storageSleepOnUnMount) {
        sleep(STORAGE_FIVE);
    }
    if (!umount2(path.c_str(), UMOUNT_NOFOLLOW) || errno == EINVAL || errno == ENOENT) {
        return OK;
    }

    KillProcWithFiles(path, SIGTERM);
    if (storageSleepOnUnMount) {
        sleep(STORAGE_FIVE);
    }
    if (!umount2(path.c_str(), UMOUNT_NOFOLLOW) || errno == EINVAL || errno == ENOENT) {
        return OK;
    }

    KillProcWithFiles(path, SIGKILL);
    if (storageSleepOnUnMount) {
        sleep(STORAGE_FIVE);
    }
    if (!umount2(path.c_str(), UMOUNT_NOFOLLOW) || errno == EINVAL || errno == ENOENT) {
        return OK;
    }
    SSLOGFE("ForceUnMount failed: %{public}s", path.c_str());
    return -errno;
}

uint64_t GetNanoTime()
{
    auto nanoNow = std::chrono::steady_clock::now().time_since_epoch();
    return nanoNow.count();
}

int ExecuteCmd(const char *execveHandlepath,
               const std::vector<std::string> &args,
               std::vector<std::string> *output)
{
    constexpr int BUFSIZE_KBYTES = 1024;
    char cmd[CMDNUM] = "";
    for (int i = 0; i < args.size(); i++) {
        strcat(cmd, const_cast<char *>(args[i].c_str()));
        strcat(cmd, " ");
    }
    output->clear();
    FILE *fp = popen(cmd, "r"); // NOLINT
    if (!fp) {
        SSLOGFE("Failed to execute shell cmd %{public}s ", cmd);
        return -1;
    }
    char buf[BUFSIZE_KBYTES] = { 0 };
    while (fgets(buf, BUFSIZE_KBYTES, fp) != nullptr) {
        output->push_back(std::string(buf));
    }

    if (pclose(fp) == -1) {
        return -1;
    }
    return 0;
}

int ExecuteCmd(const char *execveHandlepath, const std::vector<std::string> &args)
{
    uint64_t NS_PER_SECOND = 1000000000;
    pid_t pid = fork();
    if (pid < 0) {
        localForkPid = pid;
        return -1;
    } else if (pid == 0) {
        // Redirect the stdout to /dev/null
        int fd = open("/dev/null", O_WRONLY);
        // follow standard, although dup2 may handle the case of invalid oldfd
        if (fd >= 0) {
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        std::vector<char *> argv;
        for (const auto &arg : args) {
            argv.push_back(const_cast<char *>(arg.c_str()));
        }
        argv.push_back(nullptr);
#ifdef OHOS_LITE
        execve(execveHandlepath, &argv[0]), NULL);
#else
        execv(execveHandlepath, &argv[0]);
#endif
    }
    constexpr uint64_t maxWaitingTime = 60; // 60 seconds
    uint64_t remainedTime = maxWaitingTime * NS_PER_SECOND;
    while (remainedTime > 0) {
        uint64_t startTime = GetNanoTime();
        int status = 0;
        waitpid(pid, &status, WNOHANG);
        if (WIFEXITED(status)) {
            return 0;
        }
        sleep(1);
        uint64_t duration = GetNanoTime() - startTime;
        remainedTime = (remainedTime > duration) ? (remainedTime - duration) : 0;
    }
    return -1;
}

int ReadDevInfo(const std::string &path, std::string *fsType, std::string *fsUuid, std::string *fsLabel)
{
    fsType->clear();
    fsUuid->clear();
    fsLabel->clear();

    std::vector<std::string> cmd;
    cmd.push_back(toyBoxPath);
    cmd.push_back("blkid");
    cmd.push_back(path);
    std::vector<std::string> output;
    int res = ExecuteCmd(toyBoxPath.c_str(), cmd, &output);
    if (res != OK) {
        return res;
    }
    for (const auto &line : output) {
        FindValue(line, "TYPE", fsType);
        FindValue(line, "UUID", fsUuid);
        FindValue(line, "LABEL", fsLabel);
    }
    return OK;
}

std::string BuildKeyPath(const std::string &partGuid)
{
    return StringPrintf("%s/expand_%s.key", keyPath.c_str(), partGuid.c_str());
}

int ReadRandom(size_t bytes, std::string &out)
{
    out.clear();
    out.resize(bytes);
    auto buf = reinterpret_cast<char *>(out.data());
    return ReadRandom(bytes, buf);
}

int ReadRandom(size_t bytes, char *buf)
{
    int MAX_RANDOM_RETRY_COUNT = 16;
    struct stat sb;
    if (stat("/dev/urandom", &sb) != 0) {
        SSLOGFE("not exist urandom node");
        return -1;
    }
    int fd = TEMP_FAILURE_RETRY(open("/dev/urandom", O_RDONLY));
    if (fd == -1) {
        SSLOGFE("Error random get failure");
        return -1;
    }
    ssize_t n = bytes;
    int loseCount = 0;
    while (n > 0) {
        ssize_t x = read(fd, buf, n);
        if (x <= 0) {
            if (loseCount++ > MAX_RANDOM_RETRY_COUNT) {
                break;
            }
            continue;
        }
        n -= x;
        buf += x;
        loseCount = 0;
    }
    close(fd);
    if (n == 0) {
        return OK;
    } else {
        return -1;
    }
}

int GenerateRandomUuid(std::string &out)
{
    int numberSix = 6;
    int numberEight = 8;
    int res = ReadRandom(numberSixteen, out);
    if (res == OK) {
        out[numberSix] &= 0x0f;
        out[numberSix] |= 0x40;
        out[numberEight] &= 0x3f;
        out[numberEight] |= 0x80;
    }
    return res;
}

pid_t ExecuteCmdAync(const char *execvpHandlepath, const std::vector<std::string> &args)
{
    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    } else if (pid == 0) {
        // Redirect the stdout to /dev/null
        int fd = open("/dev/null", O_WRONLY);
        // follow standard, although dup2 may handle the case of invalid oldfd
        if (fd >= 0) {
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        std::vector<char *> argv;
        for (const auto &arg : args) {
            argv.push_back(const_cast<char *>(arg.c_str()));
        }
        argv.push_back(0);
#ifdef OHOS_LITE
        execve(execveHandlepath, &argv[0]), nullptr);
#else
        execv(execvpHandlepath, &argv[0]);
#endif
        _exit(EXIT_FAILURE);
    }
    return pid;
}

int64_t GetDirSize(const std::string &dirName)
{
    int64_t size = 0;
    DIR *dp = opendir(dirName.c_str());
    struct dirent *entry = nullptr;
    struct stat statbuf = {};
    if (dp == nullptr) {
        SSLOGFE("Cannot open dir: %{public}s", dirName.c_str());
        return 0;
    }
    chdir(dirName.c_str());
    while ((entry = readdir(dp)) != nullptr) {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
                continue;
            }
            size += GetDirSize(entry->d_name);
        } else {
            size += (statbuf.st_size >> numberNine);
        }
    }
    chdir("..");
    closedir(dp);
    return size;
}

uint64_t GetFreeSize(const std::string &path)
{
    struct statvfs sb = {};
    if (statvfs(path.c_str(), &sb) == 0) {
        return (uint64_t)sb.f_bavail * sb.f_frsize;
    }
    return -1;
}
} // namespace SsUtils
} // namespace OHOS
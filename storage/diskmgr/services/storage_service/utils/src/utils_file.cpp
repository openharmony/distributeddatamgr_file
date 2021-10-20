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
#include "utils_file.h"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/dm-ioctl.h>
#include <linux/fs.h>

#include "storage_hilog.h"
#include "storage_utils.h"
#include "utils_string.h"
namespace OHOS {
namespace SsUtils {
constexpr uint32_t ALL_PERMS = (S_ISUID | S_ISGID | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO);
constexpr int MAX_READ_BUFF_SIZE = 4096;
static int numberTwo = 2;
bool IsDirEmpty(const std::string &path)
{
    auto dirp = std::unique_ptr<DIR, int (*)(DIR *)>(opendir(path.c_str()), closedir);
    if (!dirp) {
        SSLOGFE("Unable to read directory: %{public}s", path.c_str());
        return false;
    }
    dirent *dp = nullptr;
    while ((dp = readdir(dirp.get())) != nullptr) {
        if ((strcmp(dp->d_name, ".") == 0) || (strcmp(dp->d_name, "..")) == 0) {
            continue;
        }
        if (strcmp(dp->d_name, "lost+found") == 0) {
            continue;
        } else {
            return false;
        }
    }
    return true;
}

int EnsureDir(const std::string &path, mode_t mode, uid_t uId, gid_t gId)
{
    if (TEMP_FAILURE_RETRY(chmod(path.c_str(), mode)) != 0) {
        return -1;
    }
    if (TEMP_FAILURE_RETRY(chown(path.c_str(), uId, gId)) != 0) {
        return -1;
    }
    return 0;
}
int PrepareDir(const std::string &path, mode_t mode, uid_t uId, gid_t gId)
{
    int ownerMatch;
    int modeMatch;
    struct stat sb;
    int fd = -1;

    if (TEMP_FAILURE_RETRY(lstat(path.c_str(), &sb)) == -1) {
        if (errno != ENOENT) {
            return -1;
        }
    } else {
        if (!S_ISDIR(sb.st_mode)) {
            return -1;
        }

        ownerMatch = ((sb.st_uid == uId) && (sb.st_gid == gId));
        modeMatch = ((sb.st_mode & ALL_PERMS) == mode);
        if (ownerMatch && modeMatch) {
            return 0;
        } else {
            return EnsureDir(path, mode, uId, gId);
        }
    }
    fd = TEMP_FAILURE_RETRY(mkdir(path.c_str(), mode));
    if (fd == -1) {
        if (errno != EEXIST) {
            return -1;
        }
    }
    return EnsureDir(path, mode, uId, gId);
}

int64_t GetSysTime()
{
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_BOOTTIME, &t);
    return static_cast<int64_t>((t.tv_sec) * 1000000000LL + t.tv_nsec);
}

bool ReadFd(int fd, std::string *content)
{
    content->clear();
    struct stat sb;
    if (fstat(fd, &sb) != -1 && sb.st_size > 0) {
        content->resize(sb.st_size);
    }
    auto buf = reinterpret_cast<char *>(content->data());
    ssize_t n;
    size_t left = content->size();
    while ((n = TEMP_FAILURE_RETRY(read(fd, buf, left))) > 0) {
        buf += n;
        left -= n;
    }
    return (n == 0) ? true : false;
}

bool ReadFile(const std::string &path, std::string *content)
{
    bool ret = true;

    int flags = O_RDONLY | O_CLOEXEC | O_BINARY;
    int fd = TEMP_FAILURE_RETRY(open(path.c_str(), flags));
    if (fd == -1) {
        return false;
    }
    ret = ReadFd(fd, content);
    close(fd);
    return ret;
}

bool WriteFd(const std::string &content, int fd)
{
    const char *buf = content.data();
    size_t size = content.size();
    ssize_t n = 0;
    while ((n = TEMP_FAILURE_RETRY(write(fd, buf, size))) > 0) {
        buf += n;
        size -= n;
    }
    return ((n == 0) ? true : false);
}
bool WriteFile(const std::string &content, const std::string &path)
{
    int flags = O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC | O_BINARY;
    int fd = TEMP_FAILURE_RETRY(open(path.c_str(), flags, 0666));
    if (fd == -1) {
        return false;
    }
    if (WriteFd(content, fd)) {
        close(fd);
        unlink(path.c_str());
        return false;
    }
    close(fd);
    return true;
}
bool Readlink(const std::string &path, std::string *result)
{
    result->clear();
    std::vector<char> buf(MAX_READ_BUFF_SIZE);
    ssize_t n;
    while ((n = readlink(path.c_str(), &buf[0], buf.size())) > 0) {
        if (static_cast<size_t>(n) <= buf.size()) {
            result->assign(&buf[0], n);
            return true;
        }
        buf.resize(buf.size() * numberTwo);
    }
    return false;
}
int SetAttrs(const std::string &path, unsigned int attrs)
{
    unsigned long flags;
    int fd = TEMP_FAILURE_RETRY(open(path.c_str(), O_RDONLY | O_NONBLOCK | O_CLOEXEC));
    if (fd == -1) {
        SSLOGFE("Open path %s error", path.c_str());
        return -1;
    }
    if (ioctl(fd, FS_IOC_GETFLAGS, (void *)&flags)) {
        SSLOGFE("Get path flag failed");
        close(fd);
        return -1;
    }
    if ((flags & attrs) == attrs) {
        SSLOG_I("Flag same as preset !");
        close(fd);
        return 0;
    }
    flags |= attrs;
    if (ioctl(fd, FS_IOC_SETFLAGS, (void *)&flags)) {
        SSLOGFE("Failed to set flags for path %s ", path.c_str());
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}
int Symlink(const std::string &target, const std::string &linkpath)
{
    if (TEMP_FAILURE_RETRY(unlink(linkpath.c_str())) < 0 && errno != EINVAL && errno != ENOENT) {
        SSLOGFE("Failed to unlink %s", linkpath.c_str());
        return -1;
    }
    if (TEMP_FAILURE_RETRY(symlink(target.c_str(), linkpath.c_str())) < 0) {
        SSLOGFE("Failed to Create symlink %s to %s", linkpath.c_str(), target.c_str());
        return -1;
    }
    return 0;
}

bool CheckSymlink(const std::string &path, const std::string &prefix)
{
    std::string res;
    if (Readlink(path, &res)) {
        if (StartsWith(res, prefix)) {
            SSLOGFI("Found symlink %s referencing %s", path.c_str(), res.c_str());
            return true;
        }
    }
    return false;
}
} // namespace SsUtils
} // namespace OHOS

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
#include "idle_maintain.h"

#include <chrono>
#include <climits>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <filesystem_manager.h>
#include <libgen.h>
#include <memory>
#include <mntent.h>
#include <mutex>
#include <refbase.h>
#include <sstream>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <linux/fs.h>

#include "fs_mount_tab.h"
#include "i_storage_service_task.h"
#include "ss_tab_mgr.h"
#include "storage_disk.h"
#include "storage_hilog.h"
#include "storage_manager.h"
#include "storage_private_volume.h"
#include "storage_utils.h"
#include "utils_file.h"
#include "utils_string.h"

using namespace OHOS::StorageService;
using namespace OHOS::SsUtils;

namespace OHOS {
/* Disk path type */
enum class PathTypes {
    PATH_TYPES_IS_MOUNT_POINT = 1,
    PATH_TYPES_IS_BLK_DEVICE,
};

/* Idlemaintain function state */
enum class IdleMaintainStates {
    IM_STATE_IS_STOPPED = 1,
    IM_STAT_IS_RUNNING,
    IM_STATE_IS_ABORT,
} idleMaintainState(IdleMaintainStates::IM_STATE_IS_STOPPED);

static std::mutex conditionVariableMutex;
static std::condition_variable conditionVariableAbort;
static std::condition_variable conditionVariableStop;

/* Calculate time difference */
class TimeSpan {
public:
    TimeSpan()
    {
        startTime = std::clock();
    }
    ~TimeSpan() {}
    int Elapsed() const
    {
        return int(std::clock() - startTime) / CLOCKS_PER_SEC;
    }

private:
    std::clock_t startTime;
};

/* Convert relative path to absolute path */
static bool GetRealPath(const std::string &path, std::string &result)
{
    result = "";
    char buff[PATH_MAX];
    memset(buff, 0, sizeof(buff));
    if (realpath(path.c_str(), buff)) {
        result = buff;
        return true;
    } else {
        SSLOG_I("convert realpath failed on %{public}s", path.c_str());
        return false;
    }
}

/* Get basename(context following the last "/") */
static std::string GetBaseName(const std::string &path)
{
    std::string result(path);
    std::string name = basename(&result[0]);
    result = name;
    return result;
}

static std::string BlkDevForPath(const std::string &path)
{
    std::unique_ptr<FILE, int (*)(FILE *)> mnts(setmntent("/proc/mounts", "re"), endmntent);
    if (!mnts) {
        SSLOG_I("Unable to open /proc/mounts");
        return "";
    }
    std::string result;
    size_t bestLength = 0;
    struct mntent *mnt;
    while ((mnt = getmntent(mnts.get())) != nullptr) {
        auto l = strlen(mnt->mnt_dir);
        if (l > bestLength && path.size() > l && path[l] == '/' && path.compare(0, l, mnt->mnt_dir) == 0) {
            result = mnt->mnt_fsname;
            bestLength = l;
        }
    }
    if (result.empty()) {
        SSLOG_I("Didn't find a mountpoint to match path on %{public}s", path.c_str());
        return "";
    }
    SSLOG_I("For path %{public}s block device is %{public}s", path.c_str(), result.c_str());
    return result;
}

static void GetPathsFromStorageManager(std::list<std::string> &pathList, PathTypes pathType)
{
    StorageManager *sm = StorageManager::Instance();
    std::list<std::string> storagePrivateVols;
    sm->ListVolumes(OHOS::StorageService::StorageBase::Type::baseTypeIsPrivate, storagePrivateVols);
    for (const std::string &id : storagePrivateVols) {
        OHOS::StorageService::StoragePrivateVolume *spvol =
            static_cast<OHOS::StorageService::StoragePrivateVolume *>(sm->FindVolume(id).get());
        if (spvol != nullptr &&
            spvol->GetState() == OHOS::StorageService::StorageBase::State::baseStateIsMounted) {
            if (pathType == PathTypes::PATH_TYPES_IS_MOUNT_POINT) {
                pathList.push_back(spvol->getPath());
            } else if (pathType == PathTypes::PATH_TYPES_IS_BLK_DEVICE) {
                std::string diskPath;
                const std::string &mFsType = spvol->GetFsType();
                if (mFsType == "f2fs" && GetRealPath(spvol->GetRawDevPath(), diskPath)) {
                    pathList.push_back(std::string("/sys/fs/") + mFsType + "/" + GetBaseName(diskPath));
                }
            }
        }
    }
}

static void GetPathsFromFstab(std::list<std::string> &pathList, PathTypes pathType)
{
    std::string previousMountPoint;
    auto sm = StorageManager::Instance();
    int count = sm->ssTabMgr->EntryCount();
    SSLOG_I("count is %{public}d", count);
    FsMountTab::FsMountEntry entry = {};

    for (int i = 0; i < count; i++) {
        if (!sm->ssTabMgr->GetEntry(i, entry)) {
            continue;
        }
        if (entry.fsType == "emmc" || entry.fsType == "mtd") {
            continue;
        }
        if (entry.mountFlags & MS_RDONLY) {
            continue;
        }
        if (entry.fsMgrFlags.noTrim) {
            continue;
        }
        if (entry.mountPoint == previousMountPoint) {
            continue;
        }
        if (pathType == PathTypes::PATH_TYPES_IS_MOUNT_POINT) {
            pathList.push_back(entry.mountPoint);
        } else if (pathType == PathTypes::PATH_TYPES_IS_BLK_DEVICE) {
            std::string diskPath;
            if (entry.fsType == "f2fs" && GetRealPath(BlkDevForPath(entry.mountPoint + "/"), diskPath)) {
                pathList.push_back("/sys/fs/" + entry.fsType + "/" + GetBaseName(diskPath));
            }
        }
        previousMountPoint = entry.mountPoint;
    }
}

void IdleFsTrim(const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("Start IdleFsTrim");
    std::list<std::string> pathList;
    GetPathsFromFstab(pathList, PathTypes::PATH_TYPES_IS_MOUNT_POINT);
    GetPathsFromStorageManager(pathList, PathTypes::PATH_TYPES_IS_MOUNT_POINT);
    if (pathList.empty()) {
        SSLOG_I("path list is empty!");
    }

    for (const auto &path : pathList) {
        SSLOG_I("Start fstrim on  %{public}s", path.c_str());

        OHOS::AppExecFwk::PacMap map;
        map.PutStringValue("path", path);

        int fd = open(path.c_str(), O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
        if (fd < 0) {
            SSLOG_I("Failed to open %{public}s", path.c_str());
            if (storageServiceTask) {
                storageServiceTask->OnStatus(-1, map);
            }
            continue;
        }
        struct fstrim_range range;
        memset(&range, 0, sizeof(range));
        range.len = ULLONG_MAX;
        if (ioctl(fd, FITRIM, &range)) {
            SSLOG_I("Trim failed on %{public}s", path.c_str());
            if (storageServiceTask) {
                storageServiceTask->OnStatus(-1, map);
            }
        } else {
            SSLOG_I("Trimed %{public}llu bytes on %{public}s", range.len, path.c_str());
            map.PutLongValue("bytes", range.len);
            if (storageServiceTask) {
                storageServiceTask->OnStatus(0, map);
            }
        }
        close(fd);
    }
    if (storageServiceTask) {
        OHOS::AppExecFwk::PacMap map;
        storageServiceTask->OnFinished(0, map);
    }
}

static void ControlGC(const std::list<std::string> &pathList, std::string dgContext, std::string guContext)
{
    for (const auto &path : pathList) {
        SSLOG_I("Set on %{public}s", path.c_str());
        if (!WriteFile(dgContext, StringPrintf("%s/discard_granularity", path.c_str()))) {
            SSLOG_I("Failed set on %{public}s/discard_gralunarity", path.c_str());
        } else {
            SSLOG_I("Successful set on %{public}s/discard_gralunarity", path.c_str());
        }
        if (!WriteFile(guContext, StringPrintf("%s/gc_urgent", path.c_str()))) {
            SSLOG_I("Failed set on %{public}s/gc_urgent", path.c_str());
        } else {
            SSLOG_I("Successful set on %{public}s/gc_urgent", path.c_str());
        }
    }
}

static bool WaitGC(const std::list<std::string> &pathList)
{
    std::unique_lock<std::mutex> uniqueLock(conditionVariableMutex, std::defer_lock);
    bool isReachLimit = false;
    bool isStateIsAbort = false;
    static const int WAIT_TIME = 10000;
    TimeSpan t;
    while (!isReachLimit && !isStateIsAbort) {
        isReachLimit = true;
        for (const auto &path : pathList) {
            std::string dirtySegments;
            if (!ReadFile(path + "/dirty_segments", &dirtySegments)) {
                SSLOG_I("Reading dirty_segments failed on %{public}s", path.c_str());
                continue;
            } else {
                SSLOG_I("Reading dirty_segments successful on %{public}s", path.c_str());
            }
            if (std::stoi(dirtySegments) > OHOS::DIRTY_SEGMENTS_LIMIT) {
                isReachLimit = false;
                break;
            }
        }
        if (isReachLimit) {
            break;
        }
        if (t.Elapsed() >= OHOS::GC_TIMEOUT_LIMIT) {
            SSLOG_I("GC timeout");
            break;
        }
        uniqueLock.lock();
        isStateIsAbort =
            conditionVariableAbort.wait_for(uniqueLock, std::chrono::milliseconds(WAIT_TIME), [] {
                return idleMaintainState == IdleMaintainStates::IM_STATE_IS_ABORT;
            });
        uniqueLock.unlock();
    }
    return isStateIsAbort;
}

int RunIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask)
{
    std::unique_lock<std::mutex> uniqueLock(conditionVariableMutex);
    SSLOG_I("Start RunIdleMaintain");

    std::list<std::string> pathList;
    const std::string TurnOnDG = "1";
    const std::string TurnOnGU = "1";
    const std::string TurnOffDG = "16";
    const std::string TurnOffGU = "0";

    GetPathsFromFstab(pathList, PathTypes::PATH_TYPES_IS_BLK_DEVICE);
    GetPathsFromStorageManager(pathList, PathTypes::PATH_TYPES_IS_BLK_DEVICE);
    if (pathList.empty()) {
        SSLOG_I("path list is empty!");
    }

    if (idleMaintainState != IdleMaintainStates::IM_STATE_IS_STOPPED) {
        SSLOG_I("idle maintenance is already running");
        if (storageServiceTask) {
            OHOS::AppExecFwk::PacMap map;
            storageServiceTask->OnFinished(0, map);
        }
        return 0;
    }
    idleMaintainState = IdleMaintainStates::IM_STAT_IS_RUNNING;

    uniqueLock.unlock();

    SSLOG_I("Turn on GC");
    ControlGC(pathList, TurnOnDG, TurnOnGU);

    bool gcAborted = WaitGC(pathList);

    SSLOG_I("Turn off GC");
    ControlGC(pathList, TurnOffDG, TurnOffGU);

    uniqueLock.lock();
    idleMaintainState = IdleMaintainStates::IM_STATE_IS_STOPPED;
    uniqueLock.unlock();
    conditionVariableStop.notify_one();
    if (!gcAborted) {
        IdleFsTrim(nullptr);
    }
    if (storageServiceTask) {
        OHOS::AppExecFwk::PacMap map;
        storageServiceTask->OnFinished(0, map);
    }
    SSLOG_I("Complete IdleMaintenance");
    return 0;
}

int AbortIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("Start AbortIdleMaintain");
    std::unique_lock<std::mutex> uniqueLock(conditionVariableMutex);
    if (idleMaintainState != IdleMaintainStates::IM_STATE_IS_STOPPED) {
        idleMaintainState = IdleMaintainStates::IM_STATE_IS_ABORT;
        uniqueLock.unlock();
        conditionVariableAbort.notify_one();
        uniqueLock.lock();
        SSLOG_I("Abort IdleMaintenance");
        conditionVariableStop.wait(
            uniqueLock, [] { return idleMaintainState == IdleMaintainStates::IM_STATE_IS_STOPPED; });
    }
    uniqueLock.unlock();
    if (storageServiceTask) {
        OHOS::AppExecFwk::PacMap map;
        storageServiceTask->OnFinished(0, map);
    }
    SSLOG_I("Stop IdleMaintenance");
    return 0;
}
} // namespace OHOS
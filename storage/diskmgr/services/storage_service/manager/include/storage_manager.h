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
#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <cstdlib>
#include <fnmatch.h>
#include <list>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "fs_mount_tab.h"
#include "local_event_packet.h"
#include "refbase.h"
#include "ss_tab_mgr.h"
#include "storage_base.h"
#include "storage_disk.h"

namespace OHOS {
class StorageManager {
public:
    virtual ~StorageManager();

    enum UserHandleFlags {
        UserFlagsisAdd = 1,
        UserFlagsisRemoved,
        UserFlagsisStarted,
        UserFlagsisStopped,
        UserFlagsiscreateStorageEmulatedVolumes,
        UserFlagsisdestroyStorageEmulatedVolumes,
    };

    enum HandleDiskFlags {
        HandleDiskFlagsAddDisk = 1,
        HandleDiskFlagsisRemovedDisk,
        HandleDiskFlagsisAddDiskSource,
    };

    int UnMountAll();
    int Start();
    static StorageManager *Instance();
    void KernelEventHandle(LocalEventPacket *evt);

    void setListener(OHOS::sptr<OHOS::IStorageServiceCallback> listener)
    {
        managerListener = listener;
    }
    void ClearListener();

    OHOS::sptr<OHOS::IStorageServiceCallback> GetListener() const
    {
        return managerListener;
    }
    void GetMountInfo();

    class DiskSource {
    public:
        DiskSource(const std::string &sysPattern, const std::string &nickname, int flags)
            : mSysPattern(sysPattern), diskNickname(nickname), mFlags(flags)
        {}

        bool Matches(const std::string &sysPath)
        {
            return !fnmatch(mSysPattern.c_str(), sysPath.c_str(), 0);
        }

        const std::string &GetNickname() const
        {
            return diskNickname;
        }
        int GetFlags() const
        {
            return mFlags;
        }

    private:
        std::string mSysPattern;
        std::string diskNickname;
        int mFlags;
    };

    std::shared_ptr<OHOS::StorageService::Disk> FindDisk(const std::string &id);
    std::shared_ptr<OHOS::StorageService::StorageBase> FindVolume(const std::string &id);
    const std::set<int> &getStartedUsers() const
    {
        return managerStartedUsers;
    }
    int Reset();
    int OnUserHandle(int userId, int userSerialNumber, int userHandleFlags);
    void ListVolumes(OHOS::StorageService::StorageBase::Type type, std::list<std::string> &list) const;
    int HandlePrimary(int userId, const std::shared_ptr<OHOS::StorageService::StorageBase> &vol, int isSet);
    void HandleDiskVolume(const std::shared_ptr<OHOS::StorageService::Disk> &disk,
                          dev_t device,
                          const std::shared_ptr<DiskSource> &diskSource,
                          int handleDiskFlags);
    void CreateEmulatedVolumesForUser(int userId);
    void DestroyEmulatedVolumesForUser(int userId);
    SsTabMgr *ssTabMgr;

private:
    StorageManager();
    static StorageManager *managerHasInstance;
    bool managerDebug;
    void DoHandle(LocalEventPacket *evt, int major, int minor, std::string &eventPath);
    int DoNullVirtualDiskHandler();
    void DoDestroyPrivateVolForUser(int userId);
    void DoResetEmulated();
    void DoResetDisk();
    void DoResetVirtualDisk();
    void DoResetUnLink();
    int DoUnMountHandlePath();
    void DoUnMountHandleEmulated();
    void DoUnMountHandleDisk();

    OHOS::sptr<OHOS::IStorageServiceCallback> managerListener;

    std::list<std::shared_ptr<DiskSource>> managerDiskSources;
    std::list<std::shared_ptr<OHOS::StorageService::Disk>> managerDisks;
    std::list<std::shared_ptr<OHOS::StorageService::Disk>> managerPendingDisks;
    std::list<std::shared_ptr<OHOS::StorageService::StorageBase>> managerObbVolumes;
    std::list<std::shared_ptr<OHOS::StorageService::StorageBase>> managerInternalEmulatedVolumes;

    std::unordered_map<int, int> managerAddedUsers;
    std::set<int> managerStartedUsers;
    std::shared_ptr<OHOS::StorageService::StorageBase> managerPrimary;
    std::string managerVirtualDiskPath;
    std::shared_ptr<OHOS::StorageService::Disk> managerVirtualDisk;
};
} // namespace OHOS
#endif // STORAGEMANAGER_H

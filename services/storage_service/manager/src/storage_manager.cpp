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
#include "storage_manager.h"

#include <cstdlib>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <memory>
#include <mntent.h>
#include <string>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "block_device.h"
#include "interface/storage_service_callback_proxy.h"
#include "storage_emulated_volume.h"
#include "storage_hilog.h"
#include "storage_private_volume.h"
#include "storage_public_volume.h"
#include "storage_service.h"
#include "storage_utils.h"
#include "utils_file.h"
#include "utils_string.h"

#include "mount_info.h"
#include <vector>
#include "fs_mount_tab.h"

using namespace OHOS::StorageService;
using namespace OHOS::SsUtils;
using namespace OHOS::StorageService::Constants;

namespace OHOS {
static constexpr int PERMISSION_FLAGS = 755;

static const char *managerPathUserMount = "/mnt/user";
static const std::string kPathVirtualDisk = "/data/misc/storage/virtual_disk";
static const std::string initConfigurationFile = "/etc/init.cfg";

StorageManager *StorageManager::managerHasInstance = NULL;

StorageManager *StorageManager::Instance()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (!managerHasInstance) {
        managerHasInstance = new StorageManager();
    }
    return managerHasInstance;
}

StorageManager::StorageManager()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    managerDebug = false;
    ssTabMgr = new SsTabMgr();
}

StorageManager::~StorageManager() {}

void StorageManager::KernelEventHandle(LocalEventPacket *evt)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    int major, minor;
    std::string eventPath(evt->FindParam("DEVPATH") ? evt->FindParam("DEVPATH") : "");
    std::string devType(evt->FindParam("DEVTYPE") ? evt->FindParam("DEVTYPE") : "");
    if (devType != "disk") {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (evt->FindParam("MAJOR") != nullptr && evt->FindParam("MINOR") != nullptr) {
        major = std::stoi(evt->FindParam("MAJOR"));
        minor = std::stoi(evt->FindParam("MINOR"));
    } else {
        return;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d major=%{public}d minor=%{public}d eventPath=%{public}s",
            __FILE__, __func__, __LINE__, major, minor, eventPath.c_str());
    DoHandle(evt, major, minor, eventPath);
}

void StorageManager::HandleDiskVolume(const std::shared_ptr<OHOS::StorageService::Disk> &disk,
                                      dev_t device,
                                      const std::shared_ptr<DiskSource> &diskSource,
                                      int handleDiskFlags)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    switch (handleDiskFlags) {
        case HandleDiskFlagsAddDisk:
            managerDisks.push_back(disk);
            disk->Create();
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            break;
        case HandleDiskFlagsisRemovedDisk: {
            auto i = managerDisks.begin();
            while (i != managerDisks.end()) {
                if ((*i)->GetDevice() == device) {
                    (*i)->Destroy();
                    i = managerDisks.erase(i);
                } else {
                    ++i;
                }
            }
            auto j = managerPendingDisks.begin();
            while (j != managerPendingDisks.end()) {
                if ((*j)->GetDevice() == device) {
                    j = managerPendingDisks.erase(j);
                } else {
                    ++j;
                }
            }
            break;
        }
        case HandleDiskFlagsisAddDiskSource: managerDiskSources.push_back(diskSource); break;
        default: break;
    }
}

int StorageManager::HandlePrimary(int userId,
                                  const std::shared_ptr<OHOS::StorageService::StorageBase> &vol,
                                  int isSet)
{
    if (isSet) {
        std::string source(managerPrimary->getPath());
        if (managerPrimary->IsEmulated()) {
            source = StringPrintf("%s/%d", source.c_str(), userId);
            PrepareDir(source.c_str(), PERMISSION_FLAGS, AID_ROOT, AID_ROOT);
        }
        std::string target(StringPrintf("/mnt/user/%d/primary", userId));
        Symlink(source, target);
    } else {
        managerPrimary = vol;
        for (int userId : managerStartedUsers) {
            HandlePrimary(userId, nullptr, 0);
        }
    }
    return 0;
}

int StorageManager::Start()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    UnMountAll();
    auto vol = std::shared_ptr<OHOS::StorageService::StorageBase>(
        new OHOS::StorageService::StorageEmulatedVolume("/data/media", 0));
    vol->SetInfo("", "", "", 0, -1, true, SWITCH_FLAGS_SETMOUNTUSERID);
    vol->Create();
    managerInternalEmulatedVolumes.push_back(vol);
    if (access((char *)kPathVirtualDisk.c_str(), F_OK) != 0) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (managerVirtualDisk == nullptr) {
        if (DoNullVirtualDiskHandler()) {
            return -1;
        }
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return 0;
}

int StorageManager::UnMountAll()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    DoUnMountHandleEmulated();
    DoUnMountHandleDisk();
    if (DoUnMountHandlePath()) {
        return -errno;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return 0;
}

std::shared_ptr<OHOS::StorageService::Disk> StorageManager::FindDisk(const std::string &id)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    for (auto disk : managerDisks) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        if (disk->getId() == id) {
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            return disk;
        }
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return nullptr;
}

std::shared_ptr<OHOS::StorageService::StorageBase> StorageManager::FindVolume(const std::string &id)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    for (const auto &vol : managerInternalEmulatedVolumes) {
        if (vol->getId() == id) {
            SSLOG_I("dugl %{public}s %{public}s %{public}d vol->getId()=%{public}s", __FILE__, __func__,
                    __LINE__, vol->getId().c_str());
            return vol;
        }
    }
    for (const auto &disk : managerDisks) {
        auto diskVol = disk->FindVolume(id);
        if (diskVol != nullptr) {
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            return diskVol;
        }
    }
    for (const auto &mgrObbVol : managerObbVolumes) {
        if (mgrObbVol->getId() == id) {
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            return mgrObbVol;
        }
    }
    return nullptr;
}

int StorageManager::Reset()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    DoResetEmulated();
    DoResetDisk();
    DoResetVirtualDisk();
    DoResetUnLink();
    managerStartedUsers.clear();
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return 0;
}

void StorageManager::ClearListener()
{
    if (managerListener != nullptr) {
        managerListener.clear();
    }
}

int StorageManager::OnUserHandle(int userId, int userSerialNumber, int userHandleFlags)
{
    switch (userHandleFlags) {
        case UserFlagsisAdd: managerAddedUsers[userId] = userSerialNumber; break;
        case UserFlagsisRemoved:
            OnUserHandle(userId, -1, UserFlagsisStopped);
            managerAddedUsers.erase(userId);
            break;
        case UserFlagsisStarted: {
            if (managerStartedUsers.find(userId) == managerStartedUsers.end()) {
                OnUserHandle(userId, -1, UserFlagsiscreateStorageEmulatedVolumes);
            }
            std::string path(StringPrintf("%s/%d", managerPathUserMount, userId));
            if (managerPrimary) {
                HandlePrimary(userId, nullptr, 0);
            }
            managerStartedUsers.insert(userId);
            break;
        }
        case UserFlagsisStopped: {
            if (managerStartedUsers.find(userId) != managerStartedUsers.end()) {
                OnUserHandle(userId, -1, UserFlagsiscreateStorageEmulatedVolumes);
            }
            managerStartedUsers.erase(userId);
            break;
        }
        case UserFlagsiscreateStorageEmulatedVolumes: CreateEmulatedVolumesForUser(userId); break;
        case UserFlagsisdestroyStorageEmulatedVolumes: DestroyEmulatedVolumesForUser(userId); break;
        default: break;
    }
    return 0;
}

int StorageManager::DoNullVirtualDiskHandler()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    struct stat buf;
    if (stat(managerVirtualDiskPath.c_str(), &buf) < 0) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d \n managerVirtualDiskPath%{public}s", __FILE__,
                __func__, __LINE__, managerVirtualDiskPath.c_str());
        return 1;
    }

    auto disk = new OHOS::StorageService::Disk("virtual", buf.st_rdev, "virtual",
                                               OHOS::StorageService::Disk::Flags::diskFlagIsAdoptable |
                                                   OHOS::StorageService::Disk::Flags::diskFlagIsSd);
    managerVirtualDisk = std::shared_ptr<OHOS::StorageService::Disk>(disk);
    HandleDiskVolume(managerVirtualDisk, 1, nullptr, 1);
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return 0;
}

void StorageManager::DoDestroyPrivateVolForUser(int userId)
{
    std::list<std::string> storagePrivateVols;
    ListVolumes(OHOS::StorageService::StorageBase::Type::baseTypeIsPrivate, storagePrivateVols);
    for (const std::string &id : storagePrivateVols) {
        OHOS::StorageService::StoragePrivateVolume *spvol =
            static_cast<OHOS::StorageService::StoragePrivateVolume *>(FindVolume(id).get());
        std::list<std::shared_ptr<OHOS::StorageService::StorageBase>> vols_to_remove;
        if (spvol->GetState() == OHOS::StorageService::StorageBase::State::baseStateIsMounted) {
            for (const auto &vol : spvol->getVolumes()) {
                if (vol->GetMountUserId() == userId) {
                    vols_to_remove.push_back(vol);
                }
            }
            for (const auto &vol : vols_to_remove) {
                vol->Destroy();
                spvol->HandleVolume(vol, 0);
            }
        }
    }
}

int StorageManager::DoUnMountHandlePath()
{
    FILE *fp = setmntent("/proc/mounts", "re");
    if (fp == NULL) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return 1;
    }

    std::list<std::string> toUnMount;
    mntent *mEntry;
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    while ((mEntry = getmntent(fp)) != NULL) {
        auto test = std::string(mEntry->mnt_dir);
        if ((StartsWith(test, "/mnt/") && !StartsWith(test, "/mnt/vendor") &&
             !StartsWith(test, "/mnt/product") && !StartsWith(test, "/mnt/installer") &&
             !StartsWith(test, "/mnt/ohoswritable")) ||
            StartsWith(test, "/storage/")) {
            toUnMount.push_front(test);
        }
    }
    endmntent(fp);
    for (const auto &path : toUnMount) {
        SsUtils::ForceUnMount(path);
    }
    return 0;
}

void StorageManager::DoHandle(LocalEventPacket *evt, int major, int minor, std::string &eventPath)
{
    dev_t device = makedev(major, minor);
    switch (evt->getAction()) {
        case LocalEventPacket::Action::LOCALEVENT_ACTION_ADD: {
            for (const auto &source : managerDiskSources) {
                SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
                int flags = source->GetFlags();
                if (SsBlockDevice::IsMmcDev(major) || SsBlockDevice::IsVirtioBlk(major)) {
                    flags |= OHOS::StorageService::Disk::Flags::diskFlagIsSd;
                } else {
                    flags |= OHOS::StorageService::Disk::Flags::diskFlagIsUsb;
                }
                auto disk = new OHOS::StorageService::Disk(eventPath, device, source->GetNickname(), flags);
                HandleDiskVolume(std::shared_ptr<OHOS::StorageService::Disk>(disk), device, nullptr, 1);
                break;
            }
            break;
        }
        case LocalEventPacket::Action::LOCALEVENT_ACTION_CHANGE:
            for (const auto &disk : managerDisks) {
                SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
                if (disk->GetDevice() == device) {
                    disk->DiskReadMetadata();
                    disk->ReadPartitions();
                }
            }
            break;
        case LocalEventPacket::Action::LOCALEVENT_ACTION_REMOVE: {
            int handleDiskFlag = 2;
            HandleDiskVolume(nullptr, device, nullptr, handleDiskFlag);
            break;
        }
        default: break;
    }
}

void StorageManager::DoResetEmulated()
{
    for (const auto &vol : managerInternalEmulatedVolumes) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        vol->Destroy();
    }
    managerInternalEmulatedVolumes.clear();
}

void StorageManager::DoResetDisk()
{
    for (const auto &disk : managerDisks) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        disk->Destroy();
        disk->Create();
    }
}

void StorageManager::DoResetVirtualDisk()
{
    if (managerVirtualDisk == nullptr) {
        return;
    }
    dev_t device = managerVirtualDisk->GetDevice();
    int handleDiskFlag = 2;
    HandleDiskVolume(nullptr, device, nullptr, handleDiskFlag);
    managerVirtualDisk = nullptr;
}

void StorageManager::DoResetUnLink()
{
    if (access((char *)kPathVirtualDisk.c_str(), F_OK) == 0) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        unlink((char *)kPathVirtualDisk.c_str());
    }
}

void StorageManager::DoUnMountHandleEmulated()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    for (const auto &sb_instance : managerInternalEmulatedVolumes) {
        sb_instance->UnMount();
    }
}

void StorageManager::DoUnMountHandleDisk()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    for (const auto &disk : managerDisks) {
        disk->UnMountAll();
    }
}

void StorageManager::ListVolumes(OHOS::StorageService::StorageBase::Type type,
                                 std::list<std::string> &list) const
{
    list.clear();
    for (const auto &disk : managerDisks) {
        disk->ListVolumes(type, list);
    }
}

void StorageManager::CreateEmulatedVolumesForUser(int userId)
{
    auto vol = std::shared_ptr<OHOS::StorageService::StorageBase>(
        new OHOS::StorageService::StorageEmulatedVolume("/data/media", userId));
    vol->SetInfo("", "", "", userId, -1, true, SWITCH_FLAGS_SETMOUNTUSERID);
    managerInternalEmulatedVolumes.push_back(vol);
    vol->Create();
    DoDestroyPrivateVolForUser(userId);
}

void StorageManager::DestroyEmulatedVolumesForUser(int userId)
{
    auto i = managerInternalEmulatedVolumes.begin();
    while (i != managerInternalEmulatedVolumes.end()) {
        auto vol = *i;
        if (vol->GetMountUserId() == userId) {
            vol->Destroy();
            i = managerInternalEmulatedVolumes.erase(i);
        } else {
            i++;
        }
    }
    DoDestroyPrivateVolForUser(userId);
}

void StorageManager::GetMountInfo()
{
    StorageManager *sm = StorageManager::Instance();
    SSLOG_I("start GetMountInfo");
    std::list<std::string> mountList;
    OHOS::ParseInitCfg(initConfigurationFile.c_str(), mountList);
    SSLOG_I("size of mountlist is %{public}d", mountList.size());
    FsMountTab::FsMountEntry entry = {};
    for (const auto &mount : mountList) {
        SSLOG_I("main.cpp--mount is %{public}s", mount.c_str());
        if (FsMountTab::ParseMountEntry(mount, entry)) {
            SSLOG_I("main.cpp--After parse, mount is %{public}s", mount.c_str());
            sm->ssTabMgr->InsertEntry(entry);
        }
    }
}

} // namespace OHOS

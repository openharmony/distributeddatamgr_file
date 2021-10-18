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

#include "device_storage_data_mgr.h"
#include "device_storage_service.h"
#include "device_storage_service_hilog.h"
#include "preferences.h"
#include "preferences_errno.h"
#include "preferences_helper.h"

namespace OHOS {
DeviceStorageDataMgr::DeviceStorageDataMgr(const sptr<DeviceStorageService> &service)
    : service_(service)
{
    SSLOG_I("DeviceStorageDataMgr instance is created");
}

DeviceStorageDataMgr::~DeviceStorageDataMgr()
{
    SSLOG_I("DeviceStorageDataMgr instance is destroyed");
    diskInfos_.clear();
    volumeInfos_.clear();
}

bool DeviceStorageDataMgr::AddDiskInfo(const std::string &diskId, int32_t flags)
{
    std::lock_guard<std::mutex> lock(mLock);
    SSLOG_I("DeviceStorageDataMgr AddDiskInfo flag and diskId is %{public}d, %{public}s", flags,
            diskId.c_str());
    flags |= OHOS::DS::DiskInfo::Flags::kAdoptable;
    auto infoItem = diskInfos_.find(diskId);
    if (infoItem != diskInfos_.end()) {
        SSLOG_E("disk info already exist");
        return false;
    }
    auto diskInfo = std::make_shared<DS::DiskInfo>(diskId, flags);
    diskInfos_.emplace(diskId, diskInfo);
    SSLOG_D("disk info added successfully");
    return true;
}

bool DeviceStorageDataMgr::SetMetaDataforDisk(const std::string &diskId,
                                              uint64_t sizeBytes,
                                              const std::string &label,
                                              const std::string &sysPath)
{
    SSLOG_I("DeviceStorageDataMgr SetMetaDataforDisk diskId is %{public}s", diskId.c_str());
    std::lock_guard<std::mutex> lock(mLock);
    auto infoItem = diskInfos_.find(diskId);
    if (infoItem != diskInfos_.end()) {
        infoItem->second->SetLabel(label);
        infoItem->second->SetSize(sizeBytes);
        infoItem->second->SetSysPath(sysPath);
        SSLOG_D("disk info metadata update  successfully");
        return true;
    }
    SSLOG_D("disk info does not exist ");
    return false;
}

bool DeviceStorageDataMgr::DeleteDiskInfo(const std::string &diskId)
{
    std::lock_guard<std::mutex> lock(mLock);
    SSLOG_I("DeviceStorageDataMgr DeleteDiskInfo diskId is %{public}s", diskId.c_str());
    auto infoItem = diskInfos_.find(diskId);
    if (infoItem != diskInfos_.end()) {
        diskInfos_.erase(diskId);
        SSLOG_D("delete disk info successfully");
        return true;
    }
    SSLOG_D("disk info does not exist ");
    // todo: need amend callback destory */
    return false;
}

bool DeviceStorageDataMgr::AddVolumeInfo(const std::string &volId,
                                         int32_t type,
                                         const std::string &diskId,
                                         const std::string &partGuid)
{
    std::lock_guard<std::mutex> lock(mLock);
    SSLOG_I("DeviceStorageDataMgr AddVolumeInfo volId is %{public}s", volId.c_str());
    auto infoItem = volumeInfos_.find(volId);
    if (infoItem != volumeInfos_.end()) {
        SSLOG_E("volume info already exist");
        return false;
    }
    std::shared_ptr<DS::VolumeInfo> vol = std::make_shared<DS::VolumeInfo>(volId, type, diskId, partGuid);
    if (type == (static_cast<int32_t>(DS::VolumeInfo::Type::kPublic))) {
        int32_t flags = vol->GetMountFlags();
        SSLOG_I("DeviceStorageDataMgr AddVolumeInfo flags before is %{public}d", flags);
        flags |= DS::VolumeInfo::kVisible;
        SSLOG_I("DeviceStorageDataMgr AddVolumeInfo flags is %{public}d", flags);
        vol->SetMountFlags(flags);
    } else if (type == (static_cast<int32_t>(DS::VolumeInfo::Type::kEmulated))) {
        SSLOG_I("DeviceStorageDataMgr AddVolumeInfo type is Emulated");
        auto privateVol = FindPrivateForEmulate(vol);
        if (privateVol == nullptr) {
            SSLOG_I("DeviceStorageDataMgr AddVolumeInfo  FindPrivateVol is nullptr");
        } else {
            int32_t flags = vol->GetMountFlags();
            SSLOG_I("DeviceStorageDataMgr AddVolumeInfo flags before is %{public}d", flags);
            SSLOG_I("DeviceStorageDataMgr AddVolumeInfo flags is %{public}d and primaryStorageUuid_ is  "
                    "%{public}s  and privateVol Id is %{public}s ",
                    flags, primaryStorageUuid_.c_str(), privateVol->GetId().c_str());
            if (DS::Constants::UUID_PRIVATE_INTERNAL == primaryStorageUuid_ &&
                DS::Constants::ID_PRIVATE_INTERNAL == privateVol->GetId()) {
                flags |= DS::VolumeInfo::kPrimary;
                flags |= DS::VolumeInfo::kVisible;
            } else if (primaryStorageUuid_ == privateVol->GetFsUuid()) {
                flags |= DS::VolumeInfo::kPrimary;
                flags |= DS::VolumeInfo::kVisible;
            }
            SSLOG_I("DeviceStorageDataMgr AddVolumeInfo flags is %{public}d", flags);
            vol->SetMountFlags(flags);
        }
    }

    vol->SetMountUserId(currentUserId_);
    volumeInfos_.emplace(volId, vol);
    SSLOG_D("volume info added successfully");
    service_->GetDeviceStorageServiceEventHandler()->SendEvent(DeviceStorageEventHandler::VOLUME_MOUNT, vol);
    return true;
}

bool DeviceStorageDataMgr::SetMetaDataforVolume(const std::string &volId,
                                                const std::string &fsType,
                                                const std::string &fsUuid,
                                                const std::string &fsLabel)
{
    std::lock_guard<std::mutex> lock(mLock);
    SSLOG_I("DeviceStorageDataMgr SetMetaDataforVolume volId is %{public}s", volId.c_str());
    auto infoItem = volumeInfos_.find(volId);
    if (infoItem != volumeInfos_.end()) {
        infoItem->second->SetFsType(fsType);
        infoItem->second->SetFsUuid(fsUuid);
        infoItem->second->SetFsLabel(fsLabel);
        SSLOG_D("volume info metadata update successfully");
        return true;
    }
    SSLOG_E("volume info does not  exist");
    return false;
}

bool DeviceStorageDataMgr::SetPathforVolume(const std::string &volId, const std::string &path)
{
    std::lock_guard<std::mutex> lock(mLock);
    SSLOG_I("DeviceStorageDataMgr SetPathforVolume volId is %{public}s", volId.c_str());
    auto infoItem = volumeInfos_.find(volId);
    if (infoItem != volumeInfos_.end()) {
        infoItem->second->SetPath(path);
        SSLOG_D("volume info path update  successfully");
        return true;
    }
    SSLOG_E("volume info does not  exist");
    return false;
}

bool DeviceStorageDataMgr::SetInternalPathforVolume(const std::string &volId, const std::string &internalPath)
{
    std::lock_guard<std::mutex> lock(mLock);
    SSLOG_I("DeviceStorageDataMgr SetInternalPathforVolume volId and internalPath is %{public}s,%{public}s",
            volId.c_str(), internalPath.c_str());
    auto infoItem = volumeInfos_.find(volId);
    if (infoItem != volumeInfos_.end()) {
        infoItem->second->SetInternalPath(internalPath);
        SSLOG_D("volume info internalpath update  successfully");
        return true;
    }
    SSLOG_E("volume info does not  exist");
    return false;
}

bool DeviceStorageDataMgr::DeleteVolumeInfo(const std::string &volId)
{
    std::lock_guard<std::mutex> lock(mLock);
    auto infoItem = volumeInfos_.find(volId);
    if (infoItem != volumeInfos_.end()) {
        volumeInfos_.erase(volId);
        SSLOG_D("delete volume info successfully");
        return true;
    }
    return false;
}

bool DeviceStorageDataMgr::NotifyVolumeState(const std::string &volId, int32_t state)
{
    std::lock_guard<std::mutex> lock(mLock);
    SSLOG_D("NotifyVolumeState");
    auto infoItem = volumeInfos_.find(volId);
    if (infoItem != volumeInfos_.end()) {
        int32_t oldState = infoItem->second->GetState();
        // todo: need amend mCallbacks.notifyVolumeStateChanged */
        SSLOG_D("NotifyVolumeState  old state and new state is   %{public}d, %{public}d", oldState, state);
        infoItem->second->SetState(state);
        if (oldState != state) {
            service_->GetDeviceStorageServiceEventHandler()->SendEvent(
                DeviceStorageEventHandler::VOLUME_BROADCAST, infoItem->second);
            return true;
        }
    }
    SSLOG_E("volume info does not exist");
    return false;
}

bool DeviceStorageDataMgr::GetVolumeInfos(std::vector<std::shared_ptr<DS::VolumeInfo>> &infos)
{
    std::lock_guard<std::mutex> lock(mLock);
    SSLOG_I("DeviceStorageDataMgr::GetVolumeInfos");
    if (volumeInfos_.empty()) {
        SSLOG_I("volumeInfos_ data is empty");
        return false;
    }
    SSLOG_I("DeviceStorageDataMgr::volumeInfos_ size  is %{public}d ", volumeInfos_.size());
    for (const auto &item : volumeInfos_) {
        infos.emplace_back(item.second);
    }
    return true;
}

bool DeviceStorageDataMgr::GetDiskInfos(std::vector<std::shared_ptr<DS::DiskInfo>> &infos)
{
    std::lock_guard<std::mutex> lock(mLock);
    SSLOG_I("DeviceStorageDataMgr::GetDiskInfos");
    if (diskInfos_.empty()) {
        SSLOG_I("diskInfos_ data is empty");
        return false;
    }
    for (const auto &item : diskInfos_) {
        infos.emplace_back(item.second);
    }
    return true;
}

bool DeviceStorageDataMgr::FindVolumeById(std::shared_ptr<DS::VolumeInfo> &vol, const std::string &volId)
{
    auto infoItem = volumeInfos_.find(volId);
    if (infoItem != volumeInfos_.end()) {
        vol = infoItem->second;
        SSLOG_D("find volume by Id successfully");
        return true;
    }
    SSLOG_D("find volume by Id Unsuccessfully");
    return false;
}

void DeviceStorageDataMgr::InitData()
{
    SSLOG_I("DeviceStorageDataMgr::InitData");
    std::lock_guard<std::mutex> lock(mLock);
    diskInfos_.clear();
    volumeInfos_.clear();
    auto info = std::make_shared<DS::VolumeInfo>(
        DS::Constants::ID_PRIVATE_INTERNAL, static_cast<int32_t>(DS::VolumeInfo::Type::kPrivate), "", "");
    info->SetState((int32_t)DS::VolumeInfo::State::kMounted);
    info->SetPath("data");
    info->SetMountUserId(currentUserId_);
    volumeInfos_.emplace(info->GetId(), info);
    SSLOG_I("DeviceStorageDataMgr::VolumeInfo anded successfully");
}

void DeviceStorageDataMgr::SetDefaultPrimaryUuid()
{
    SSLOG_I("DeviceStorageDataMgr::SetDefaultPrimaryUuid");
    std::lock_guard<std::mutex> lock(mLock);

    std::string file("/data/accounts/account_0/appdata/com.ohos.settings/sharedPreference");
    int errCode = NativePreferences::E_OK;
    std::shared_ptr<NativePreferences::Preferences> pref =
        NativePreferences::PreferencesHelper::GetPreferences(file, errCode);
    if (pref == nullptr) {
        SSLOG_I("DeviceStorageDataMgr::SetDefaultPrimaryUuid  pref nullptr %{public}d", errCode);
        return;
    }

    primaryStorageUuid_ = pref->GetString("primaryUuid", "");
    SSLOG_D("primaryStorageUuid_ read in Preferences  is   %{public}s", primaryStorageUuid_.c_str());
    if (primaryStorageUuid_ == "") {
        SSLOG_D("primaryStorageUuid_ read in Preferences  is empty,set to default primary");
        primaryStorageUuid_ = DS::Constants::UUID_PRIVATE_INTERNAL;
        pref->PutString("primaryUuid", primaryStorageUuid_);
        pref->FlushSync();
    }
}

void DeviceStorageDataMgr::SetPrimaryUuid(const std::string &primaryUuid,
                                          std::string &fromVolId,
                                          std::string &toVolId)
{
    SSLOG_I("DeviceStorageDataMgr::SetPrimaryUuid");
    std::lock_guard<std::mutex> lock(mLock);
    if (primaryUuid == primaryStorageUuid_) {
        SSLOG_I("DeviceStorageDataMgr::Primary storage already at %{public}s ", primaryStorageUuid_.c_str());
        return;
    } else {
        targetStorageUuid_ = primaryUuid;
        SSLOG_I("DeviceStorageDataMgr::Primary storage  uuid  and target move storage uuid is   %{public}s , "
                "%{public}s ",
                primaryStorageUuid_.c_str(), targetStorageUuid_.c_str());
        auto fromVol = FindVolumeByUuid(primaryStorageUuid_);
        if (fromVol == nullptr) {
            SSLOG_I("DeviceStorageDataMgr::Failing move due to missing from volume  %{public}s ",
                    primaryStorageUuid_.c_str());
            return;
        }
        auto toVol = FindVolumeByUuid(primaryUuid);
        if (toVol == nullptr) {
            SSLOG_I("DeviceStorageDataMgr::Failing move due to missing to volume  %{public}s ",
                    primaryUuid.c_str());
            return;
        }
        fromVolId = fromVol->GetId();
        toVolId = toVol->GetId();
    }
}

std::shared_ptr<DS::VolumeInfo> DeviceStorageDataMgr::FindVolumeByUuid(const std::string &volumeUuid)
{
    if (volumeUuid == DS::Constants::UUID_PRIVATE_INTERNAL) {
        std::shared_ptr<DS::VolumeInfo> vol;
        FindVolumeById(vol, DS::Constants::ID_EMULATED_INTERNAL + ";" + std::to_string(currentUserId_));
        return vol;
    } else {
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        for (const auto &item : volumeInfos_) {
            auto info = item.second;
            if (info->GetFsUuid() == volumeUuid && info->IsEmulated()) {
                return info;
            }
        }
        return nullptr;
    }
}

std::shared_ptr<DS::VolumeInfo> DeviceStorageDataMgr::FindPrivateForEmulate(
    std::shared_ptr<DS::VolumeInfo> &vol)
{
    if (vol != nullptr) {
        std::shared_ptr<DS::VolumeInfo> privateVol;
        SSLOG_I("DeviceStorageDataMgr::FindPrivateForEmulate GetId is  %{public}s ", vol->GetId().c_str());
        std::string subvolId = vol->GetId().substr(DS::Constants::SUB_LENGTH, vol->GetId().length());
        if (subvolId == ";" + std::to_string(currentUserId_)) {
            subvolId = "";
        }
        SSLOG_I("DeviceStorageDataMgr::FindPrivateForEmulate subvolId is  %{public}s ", subvolId.c_str());
        std::string id = "private" + subvolId;
        SSLOG_I("DeviceStorageDataMgr::FindPrivateForEmulate id is  %{public}s ", id.c_str());
        FindVolumeById(privateVol, id);
        return privateVol;
    } else {
        return nullptr;
    }
}

std::shared_ptr<DS::VolumeInfo> DeviceStorageDataMgr::FindEmulateForPrivate(
    std::shared_ptr<DS::VolumeInfo> &vol)
{
    if (vol != nullptr) {
        std::shared_ptr<DS::VolumeInfo> emuVol;
        SSLOG_I("DeviceStorageDataMgr::FindEmulateForPrivate GetId is  %{public}s ", vol->GetId().c_str());
        SSLOG_I("DeviceStorageDataMgr::FindEmulateForPrivate GetId is  %{public}d ", vol->GetId().length());
        std::string subvolId;
        if (vol->GetId() == DS::Constants::ID_PRIVATE_INTERNAL) {
            SSLOG_I("DeviceStorageDataMgr::FindEmulateForPrivate Is Equal ");
            subvolId = ";" + std::to_string(currentUserId_);
            SSLOG_I("DeviceStorageDataMgr::FindEmulateForPrivate subvolId is  %{public}s ", subvolId.c_str());
        } else if (vol->GetId().length() > DS::Constants::SUB_PRI_LENGTH) {
            SSLOG_I("DeviceStorageDataMgr::FindEmulateForPrivate Is greater than ");
            subvolId = vol->GetId().substr(DS::Constants::SUB_PRI_LENGTH, vol->GetId().length());
            SSLOG_I("DeviceStorageDataMgr::FindEmulateForPrivate subvolId is  %{public}s ", subvolId.c_str());
        }
        SSLOG_I("DeviceStorageDataMgr::FindEmulateForPrivate subvolId is  %{public}s ", subvolId.c_str());
        std::string id = "emulated" + subvolId;
        SSLOG_I("DeviceStorageDataMgr::FindEmulateForPrivate id is  %{public}s ", id.c_str());
        FindVolumeById(emuVol, id);
        return emuVol;
    } else {
        return nullptr;
    }
}

bool DeviceStorageDataMgr::GetPrimaryUuid(std::string &primaryUuid)
{
    std::lock_guard<std::mutex> lock(mLock);
    SSLOG_I("DeviceStorageDataMgr::GetPrimaryUuid");
    primaryUuid = primaryStorageUuid_;
    return true;
}

void DeviceStorageDataMgr::MoveStatusUpdate(int32_t status)
{
    SSLOG_I("DeviceStorageDataMgr::MoveStatusUpdate");
    std::lock_guard<std::mutex> lock(mLock);
    if (status == DS::Constants::MOVE_STATUS_COPY_FINISHED) {
        SSLOG_I("DeviceStorageDataMgr Move to %{public}s  copy phase finshed; persisting ",
                targetStorageUuid_.c_str());
        primaryStorageUuid_ = targetStorageUuid_;
        std::string file("/data/accounts/account_0/appdata/com.ohos.settings/sharedPreference");
        int errCode = NativePreferences::E_OK;
        std::shared_ptr<NativePreferences::Preferences> pref =
            NativePreferences::PreferencesHelper::GetPreferences(file, errCode);
        if (pref == nullptr) {
            SSLOG_I("DeviceStorageDataMgr::SetDefaultPrimaryUuid  pref nullptr %{public}d", errCode);
            return;
        }
        pref->PutString("primaryUuid", primaryStorageUuid_);
        pref->FlushSync();
    }

    targetStorageUuid_ = "null";
}

void DeviceStorageDataMgr::UnmountVolForUser(int32_t userId)
{
    SSLOG_I("DeviceStorageDataMgr::UnmountVolForUser  %{public}d", userId);
    std::lock_guard<std::mutex> lock(mLock);
    std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
    for (const auto &item : volumeInfos_) {
        auto info = item.second;
        if (info->GetMountUserId() == userId) {
            service_->GetDeviceStorageServiceEventHandler()->SendEvent(
                DeviceStorageEventHandler::VOLUME_UNMOUNT, info);
        }
    }
}

void DeviceStorageDataMgr::SetCurrentUser(int32_t userId)
{
    SSLOG_I("DeviceStorageDataMgr::SetCurrentUser  %{public}d", userId);
    std::lock_guard<std::mutex> lock(mLock);
    currentUserId_ = userId;
}

void DeviceStorageDataMgr::RemoveunlockUser(int32_t userId)
{
    SSLOG_I("DeviceStorageDataMgr::RemoveunlockUser  %{public}d", userId);
    std::lock_guard<std::mutex> lock(mLock);
    auto i = systemUnlockedUsers_.begin();
    while (i != systemUnlockedUsers_.end()) {
        if ((*i) == userId) {
            i = systemUnlockedUsers_.erase(i);
        } else {
            ++i;
        }
    }
}

void DeviceStorageDataMgr::CompleteUnlockUser(int32_t userId)
{
    SSLOG_I("DeviceStorageDataMgr::CompleteUnlockUser  %{public}d", userId);
    std::lock_guard<std::mutex> lock(mLock);
    for (const auto &item : volumeInfos_) {
        auto vol = item.second;
        if (vol->IsVisibleForUser(userId) && vol->GetState() == (int32_t)DS::VolumeInfo::State::kMounted) {
            service_->GetDeviceStorageServiceEventHandler()->SendEvent(
                DeviceStorageEventHandler::VOLUME_MOUNT, item.second);
        }
    }
    systemUnlockedUsers_.push_back(userId);
}
} // namespace OHOS
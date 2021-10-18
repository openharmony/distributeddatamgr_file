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
#include "device_storage_manager.h"

#include <cstring>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "device_storage_manager_errors.h"
#include "device_storage_manager_hilog.h"
#include "device_storage_service.h"
#include "ipc/device_storage_service_proxy.h"

namespace OHOS {
DeviceStorageManager::DeviceStorageManager()
{
    SSLOG_I("DEBUG DeviceStorageManager constructer");
    deviceStorageService_ = nullptr;
}

DeviceStorageManager::~DeviceStorageManager()
{
    SSLOG_I("DEBUG ~DeviceStorageManager ~");
}

int DeviceStorageManager::Connect()
{
    if (deviceStorageService_ != nullptr) {
        return ERR_OK;
    }
    int errCode = 0;
    SSLOG_E("DeviceStorageManager::Connect start");
    if (deviceStorageService_ == nullptr) {
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            SSLOG_E("DeviceStorageManager::Connect samgr == nullptr");
            return E_GET_SYSTEM_ABILITY_MANAGER_FAILED;
        }
        auto object = sam->GetSystemAbility(DEVICE_STORAGE_MANAGER_SERVICE_ID);
        if (object == nullptr) {
            SSLOG_E("DeviceStorageManager::Connect object == nullptr");
            return E_GET_DEVICE_STORAGE_SERVICE_FAILED;
        }
        deviceStorageService_ = iface_cast<IDeviceStorageService>(object);
        if (deviceStorageService_ == nullptr) {
            SSLOG_E("DeviceStorageManager::Connect service == nullptr");
            return E_GET_DEVICE_STORAGE_SERVICE_FAILED;
        }
    }
    SSLOG_E("DeviceStorageManager::Connect end");
    return errCode;
}

bool DeviceStorageManager::Mount(const std::string &volId)
{
    SSLOG_I("DeviceStorageManager::Mount Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::Mount end");
        return false;
    } else {
        return deviceStorageService_->Mount(volId);
    }
}
bool DeviceStorageManager::UnMount(const std::string &volId)
{
    SSLOG_I("DeviceStorageManager::UnMount Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::UnMount end");
        return false;
    } else {
        return deviceStorageService_->UnMount(volId);
    }
}

bool DeviceStorageManager::Format(const std::string &volId)
{
    SSLOG_I("DeviceStorageManager::Format Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::Format end");
        return false;
    } else {
        return deviceStorageService_->Format(volId);
    }
}

bool DeviceStorageManager::IsEncrypted(const std::string &filePath)
{
    SSLOG_I("DeviceStorageManager::IsEncrypted Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::IsEncrypted end");
        return false;
    } else {
        return deviceStorageService_->IsEncrypted(filePath);
    }
}

bool DeviceStorageManager::GetVolumes(std::vector<std::shared_ptr<DS::VolumeInfo>> &volumeInfos)
{
    SSLOG_I("DeviceStorageManager::GetVolumes Start");
    if (Connect() != ERR_OK) {
        return false;
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
    } else {
        return deviceStorageService_->GetVolumes(volumeInfos);
    }
}

bool DeviceStorageManager::PartitionPublic(const std::string &diskId)
{
    SSLOG_I("DeviceStorageManager::PartitionPublic Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
        return false;
    } else {
        return deviceStorageService_->PartitionPublic(diskId);
    }
}

bool DeviceStorageManager::PartitionPrivate(const std::string &diskId)
{
    SSLOG_I("DeviceStorageManager::PartitionPrivate Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
        return false;
    } else {
        return deviceStorageService_->PartitionPrivate(diskId);
    }
}


bool DeviceStorageManager::GetDisks(std::vector<std::shared_ptr<DS::DiskInfo>> &diskInfos)
{
    SSLOG_I("DeviceStorageManager::getDisks Start");
    if (Connect() != ERR_OK) {
        return false;
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
    } else {
        return deviceStorageService_->GetDisks(diskInfos);
    }
}

bool DeviceStorageManager::SetPrimaryStorageUuid(const std::string &volumeUuid)
{
    SSLOG_I("DeviceStorageManager::SetPrimaryStorageUuid Start");
    if (Connect() != ERR_OK) {
        return false;
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
    } else {
        return deviceStorageService_->SetPrimaryStorageUuid(volumeUuid);
    }
}

bool DeviceStorageManager::BenchMark(const std::string &volId,
                                     const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("DeviceStorageManager::BenchMark Start");
    if (Connect() != ERR_OK) {
        return false;
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
    } else {
        return deviceStorageService_->BenchMark(volId, storageServiceTask);
    }
}

bool DeviceStorageManager::FindVolumeById(std::shared_ptr<DS::VolumeInfo> &vol, const std::string &volId)
{
    SSLOG_I("DeviceStorageManager::FindVolumeById Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
        return false;
    } else {
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        bool ret = deviceStorageService_->GetVolumes(infos);
        if (ret) {
            SSLOG_I("size is  %{public}d\n", infos.size());
            for (const auto &iter : infos) {
                if (iter->GetId() == volId) {
                    vol = iter;
                    return true;
                }
            }
            return false;
        } else {
            SSLOG_I("volumeInfos_ data is empty");
            return false;
        }
    }
}

bool DeviceStorageManager::FindVolumeByUuid(std::shared_ptr<DS::VolumeInfo> &vol,
                                            const std::string &volumeUuid)
{
    SSLOG_I("DeviceStorageManager::FindVolumeByUuid Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
        return false;
    } else {
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        bool ret = deviceStorageService_->GetVolumes(infos);
        if (ret) {
            SSLOG_I("size is  %{public}d\n", infos.size());
            for (const auto &iter : infos) {
                if (iter->GetFsUuid() == volumeUuid) {
                    vol = iter;
                    return true;
                }
            }
            return false;
        } else {
            SSLOG_I("volumeInfos_ data is empty");
            return false;
        }
    }
}

bool DeviceStorageManager::GetWritableVolumes(std::vector<std::shared_ptr<DS::VolumeInfo>> &volumeInfos)
{
    SSLOG_I("DeviceStorageManager::GetWritableVolumes Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
        return false;
    } else {
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        bool ret = deviceStorageService_->GetVolumes(infos);
        if (ret) {
            SSLOG_I("size is  %{public}d\n", infos.size());
            for (const auto &iter : infos) {
                if (iter->GetType() == static_cast<int32_t>(DS::VolumeInfo::Type::kPrivate) &&
                    iter->GetState() == static_cast<int32_t>(DS::VolumeInfo::State::kMounted)) {
                    volumeInfos.emplace_back(iter);
                }
            }
            return true;
        } else {
            SSLOG_I("volumeInfos_ data is empty");
            return false;
        }
    }
}

bool DeviceStorageManager::FindDiskById(std::shared_ptr<DS::DiskInfo> &disk, const std::string &dsikId)
{
    SSLOG_I("DeviceStorageManager::FindDiskById Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
        return false;
    } else {
        std::vector<std::shared_ptr<DS::DiskInfo>> infos;
        bool ret = deviceStorageService_->GetDisks(infos);
        if (ret) {
            SSLOG_I("size is  %{public}d\n", infos.size());
            for (const auto &iter : infos) {
                if (iter->GetId() == dsikId) {
                    disk = iter;
                    return true;
                }
            }
            return false;
        } else {
            SSLOG_I("diskInfos_ data is empty");
            return false;
        }
    }
}

bool DeviceStorageManager::GetPrimaryStorageUuid(std::string &primaryUuid)
{
    SSLOG_I("DeviceStorageManager::GetPrimaryStorageUuid Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
        return false;
    } else {
        return deviceStorageService_->GetPrimaryStorageUuid(primaryUuid);
    }
}

bool DeviceStorageManager::FindPrivateForEmulate(std::shared_ptr<DS::VolumeInfo> &priVol,
                                                 std::shared_ptr<DS::VolumeInfo> &emuVol)
{
    SSLOG_I("DeviceStorageManager::FindPrivateForEmulate Start");
    if (emuVol == nullptr || !emuVol->IsEmulated()) {
        SSLOG_I("DeviceStorageManager::FindPrivateForEmulate input type must be emulated");
        return false;
    }
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
        return false;
    } else {
        return deviceStorageService_->FindPrivateForEmulate(priVol, emuVol);
    }
}

bool DeviceStorageManager::FindEmulateForPrivate(std::shared_ptr<DS::VolumeInfo> &emuVol,
                                                 std::shared_ptr<DS::VolumeInfo> &priVol)
{
    SSLOG_I("DeviceStorageManager::FindEmulateForPrivate Start");
    if (priVol == nullptr || priVol->GetType() != static_cast<int32_t>(DS::VolumeInfo::Type::kPrivate)) {
        return false;
    }
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
        return false;
    } else {
        return deviceStorageService_->FindEmulateForPrivate(emuVol, priVol);
    }
}

bool DeviceStorageManager::GetBestVolumeDescription(std::shared_ptr<DS::VolumeInfo> &vol,
                                                    std::string &desCription)
{
    SSLOG_I("DeviceStorageManager::GetBestVolumeDescription Start");
    if (Connect() != ERR_OK) {
        SSLOG_I("DeviceStorageManager::can not get DeviceStorageService");
        return false;
    } else {
        if (vol == nullptr) {
            return false;
        }
        if (vol->GetDescription() != "") {
            desCription = vol->GetDescription();
            SSLOG_I("DeviceStorageManager::desCrip is %{public}s", desCription.c_str());
            return true;
        }
        std::string diskId = vol->GetDiskId();
        std::shared_ptr<DS::DiskInfo> diskInfo;
        FindDiskById(diskInfo, diskId);
        if (diskInfo != nullptr) {
            desCription = diskInfo->GetDescription();
            SSLOG_I("DeviceStorageManager::desCripdisk is %{public}s", desCription.c_str());
            return true;
        }
        return false;
    }
}
} // namespace OHOS
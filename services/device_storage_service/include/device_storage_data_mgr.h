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

#ifndef STORAGE_DSS_INCLUDE_DEVICE_STORAGE_DATA_MGR_H
#define STORAGE_DSS_INCLUDE_DEVICE_STORAGE_DATA_MGR_H

#include <algorithm>
#include <map>
#include <mutex>
#include <string>
#include "model/device_storage_constants.h"
#include "model/disk_info.h"
#include "model/volume_info.h"

namespace OHOS {
class DeviceStorageService;
class DeviceStorageDataMgr {
public:
    DeviceStorageDataMgr(const sptr<DeviceStorageService> &service);
    ~DeviceStorageDataMgr();
    std::mutex &getLock()
    {
        return mLock;
    }
    bool AddDiskInfo(const std::string &diskId, int32_t flags);
    bool SetMetaDataforDisk(const std::string &diskId,
                            uint64_t sizeBytes,
                            const std::string &label,
                            const std::string &sysPath);
    bool DeleteDiskInfo(const std::string &diskId);
    bool AddVolumeInfo(const std::string &volId,
                       int32_t type,
                       const std::string &diskId,
                       const std::string &partGuid);
    bool SetMetaDataforVolume(const std::string &volId,
                              const std::string &fsType,
                              const std::string &fsUuid,
                              const std::string &fsLabel);
    bool SetPathforVolume(const std::string &volId, const std::string &path);
    bool SetInternalPathforVolume(const std::string &volId, const std::string &internalPath);
    bool DeleteVolumeInfo(const std::string &volId);
    bool NotifyVolumeState(const std::string &volId, int32_t state);
    bool GetVolumeInfos(std::vector<std::shared_ptr<DS::VolumeInfo>> &volumeInfos);
    bool FindVolumeById(std::shared_ptr<DS::VolumeInfo> &vol, const std::string &volId);
    void InitData();
    bool GetDiskInfos(std::vector<std::shared_ptr<DS::DiskInfo>> &diskInfos);
    void SetDefaultPrimaryUuid();
    void SetPrimaryUuid(const std::string &primaryUuid, std::string &fromVolId, std::string &toVolId);
    const std::string &GetPrimaryStorageUuid() const
    {
        return primaryStorageUuid_;
    }
    std::shared_ptr<DS::VolumeInfo> FindVolumeByUuid(const std::string &volumeUuid);
    std::shared_ptr<DS::VolumeInfo> FindPrivateForEmulate(std::shared_ptr<DS::VolumeInfo> &vol);
    std::shared_ptr<DS::VolumeInfo> FindEmulateForPrivate(std::shared_ptr<DS::VolumeInfo> &vol);
    bool GetPrimaryUuid(std::string &primaryUuid);
    void MoveStatusUpdate(int32_t status);
    void UnmountVolForUser(int32_t userId);
    void SetCurrentUser(int32_t userId);
    void RemoveunlockUser(int32_t userId);
    void CompleteUnlockUser(int32_t userId);

private:
    std::map<std::string, std::shared_ptr<OHOS::DS::DiskInfo>> diskInfos_;
    std::map<std::string, std::shared_ptr<OHOS::DS::VolumeInfo>> volumeInfos_;
    std::string primaryStorageUuid_;
    std::string targetStorageUuid_;
    int32_t currentUserId_ = DS::Constants::USER_SYSTEM;
    std::vector<int32_t> systemUnlockedUsers_;
    std::mutex mLock;
    sptr<DeviceStorageService> service_;
};
} // namespace OHOS
#endif // STORAGE_DSS_INCLUDE_DEVICE_STORAGE_DATA_MGR_H

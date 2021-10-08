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

#ifndef STORAGE_DISKMGR_FRAMEWORKS_DEVICE_STORAGE_MANAGER_H
#define STORAGE_DISKMGR_FRAMEWORKS_DEVICE_STORAGE_MANAGER_H

#include <cstdio>
#include <memory>
#include <refbase.h>
#include <singleton.h>
#include "ipc/device_storage_service_stub.h"
#include "model/volume_info.h"
#include "system_ability.h"
#include "model/disk_info.h"
#include "ipc/i_storage_service_task.h"

namespace OHOS {
class DeviceStorageManager : public RefBase {
    DECLARE_DELAYED_SINGLETON(DeviceStorageManager);

public:
    int Connect();
    bool Mount(const std::string &volId);
    bool UnMount(const std::string &volId);
    bool Format(const std::string &volId);
    bool PartitionPublic(const std::string &diskId);
    bool PartitionPrivate(const std::string &diskId);
    bool GetVolumes(std::vector<std::shared_ptr<DS::VolumeInfo>> &volumeInfos);
    bool GetDisks(std::vector<std::shared_ptr<DS::DiskInfo>> &diskInfos);
    bool SetPrimaryStorageUuid(const std::string &volumeUuid);
    bool BenchMark(const std::string &volId, const sptr<IStorageServiceTask> &storageServiceTask);
    bool GetPrimaryStorageUuid(std::string &primaryUuid);
    bool FindVolumeById(std::shared_ptr<DS::VolumeInfo> &vol, const std::string &volId);
    bool FindVolumeByUuid(std::shared_ptr<DS::VolumeInfo> &vol, const std::string &volumeUuid);
    bool FindDiskById(std::shared_ptr<DS::DiskInfo> &disk, const std::string &dsikId);
    bool FindPrivateForEmulate(std::shared_ptr<DS::VolumeInfo> &priVol,
                               std::shared_ptr<DS::VolumeInfo> &emuVol);
    bool FindEmulateForPrivate(std::shared_ptr<DS::VolumeInfo> &emuVol,
                               std::shared_ptr<DS::VolumeInfo> &priVol);
    bool GetWritableVolumes(std::vector<std::shared_ptr<DS::VolumeInfo>> &volumeInfos);
    bool GetBestVolumeDescription(std::shared_ptr<DS::VolumeInfo> &vol, std::string &desCription);
    bool IsEncrypted(const std::string &filePath);
private:
    DISALLOW_COPY_AND_MOVE(DeviceStorageManager);
    sptr<IDeviceStorageService> deviceStorageService_;
};
} // namespace OHOS
#endif // STORAGE_DISKMGR_FRAMEWORKS_DEVICE_STORAGE_MANAGER_H
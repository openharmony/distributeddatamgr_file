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
#ifndef DISK_H
#define DISK_H

#include <vector>

#include "utils_file.h"
#include "interface/storage_service_callback_interface.h"
#include "storage_base.h"
#include "block_device.h"
namespace OHOS {
namespace StorageService {
class StorageBase;
class Disk {
public:
    Disk(const std::string &eventPath, dev_t device, const std::string &nickname, int flags);

    enum Flags {
        diskFlagIsAdoptable = 1 << 0,
        diskFlagIsDefaultPrimary = 1 << 1,
        diskFlagIsSd = 1 << 2,
        diskFlagIsUsb = 1 << 3,
        diskFlagIsEmmc = 1 << 4,
        diskFlagIsStub = 1 << 5,
    };

    enum PartitionHandleFlags {
        PartitionHandleFlagsIsPublic = 1,
        PartitionHandleFlagsIsPrivate,
    };

    enum CreateVolumeFlags {
        CreateVolumeFlagsIsPublic = 1,
        CreateVolumeFlagsIsPrivate,
    };

    int Create();
    int Destroy();
    int UnMountAll();
    int ReadPartitionsHandler();
    int DiskReadMetadata();
    int ReadPartitions();
    int PartitionHandle(int partitionHandleFlags, int8_t ratio);
    std::shared_ptr<StorageBase> FindVolume(const std::string &id);

    dev_t GetDevice() const
    {
        return diskDevice;
    }

    const std::string &getEventPath() const
    {
        return diskEventPath;
    }

    const std::string &getId() const
    {
        return diskId;
    }

    void ListVolumes(StorageBase::Type type, std::list<std::string> &list) const;
    std::vector<std::shared_ptr<StorageBase>> GetVolumes() const;

private:
    dev_t diskDevice;
    int diskSize;
    int diskFlags;
    bool diskCreated;
    bool diskJustPartitioned;

    std::string diskId;
    std::string diskEventPath;
    std::string diskSysPath;
    std::string diskDevPath;
    std::string diskNickname;
    std::string diskLabel;
    sptr<SsBlockDevice> diskBlkDev;
    void CreateVolume(dev_t device, const std::string &partGuid, bool createVolumeIsPrivate);
    std::vector<std::shared_ptr<StorageBase>> mVolumes;
    int GetMaxMinors();
};
} // namespace StorageService
} // namespace OHOS

#endif // DISK_H

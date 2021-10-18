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
#ifndef STORAGE_DISKMGR_SERVICES_I_DEVICE_STORAGE_SERVICE_H
#define STORAGE_DISKMGR_SERVICES_I_DEVICE_STORAGE_SERVICE_H

#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "model/volume_info.h"
#include "model/disk_info.h"
#include "ipc/i_storage_service_task.h"

namespace OHOS {
class IDeviceStorageService : public IRemoteBroker {
public:
    enum {
        DSS_INIT = 1,
        DSS_DOMOUNT,
        DSS_UNMOUNT,
        DSS_FORMAT,
        DSS_GETVOLUMES,
        DSS_PARTITIONPUBLIC,
        DSS_PARTITIONPRIVATE,
        DSS_GETDISKS,
        DSS_SETPRIMARYSTORAGE,
        DSS_BENCHMARK,
        DSS_GETPRIMARYUUID,
        DSS_FINDPRIVATEFOREMULATE,
        DSS_FINDEMULATEFORPRIVATE,
        DSS_ISENCRYPTED
    };
    virtual bool Mount(const std::string &volId) = 0;
    virtual int Init() = 0;
    virtual bool UnMount(const std::string &volId) = 0;
    virtual bool Format(const std::string &volId) = 0;
    virtual bool GetVolumes(std::vector<std::shared_ptr<DS::VolumeInfo>> &volumeInfos) = 0;
    virtual bool PartitionPrivate(const std::string &diskId) = 0;
    virtual bool PartitionPublic(const std::string &diskId) = 0;
    virtual bool GetDisks(std::vector<std::shared_ptr<DS::DiskInfo>> &diskInfos) = 0;
    virtual bool SetPrimaryStorageUuid(const std::string &volumeUuid) = 0;
    virtual bool BenchMark(const std::string &volId, const sptr<IStorageServiceTask> &storageServiceTask) = 0;
    virtual bool GetPrimaryStorageUuid(std::string &primaryUuid) = 0;
    virtual bool FindPrivateForEmulate(std::shared_ptr<DS::VolumeInfo> &priVol,
                                       std::shared_ptr<DS::VolumeInfo> &emuVol) = 0;
    virtual bool FindEmulateForPrivate(std::shared_ptr<DS::VolumeInfo> &emuVol,
                                       std::shared_ptr<DS::VolumeInfo> &priVol) = 0;
    virtual bool IsEncrypted(const std::string &filePath) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"IDeviceStorageService");
};
} // namespace OHOS
#endif // STORAGE_DISKMGR_SERVICES_I_DEVICE_STORAGE_SERVICE_H

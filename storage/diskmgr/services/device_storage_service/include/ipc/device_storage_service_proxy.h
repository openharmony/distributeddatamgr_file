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

#ifndef STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_PROXY_H
#define STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_PROXY_H

#include "ipc/idevice_storage_service.h"
#include "model/volume_info.h"

namespace OHOS {
class DeviceStorageServiceProxy : public IRemoteProxy<IDeviceStorageService> {
public:
    DeviceStorageServiceProxy(const sptr<IRemoteObject> &impl);
    ~DeviceStorageServiceProxy();

    virtual bool Mount(const std::string &volId) override;
    virtual bool UnMount(const std::string &volId) override;
    virtual bool Format(const std::string &volId) override;
    virtual bool IsEncrypted(const std::string &filePath) override;
    virtual int Init() override;
    virtual bool GetVolumes(std::vector<std::shared_ptr<DS::VolumeInfo>> &volumeInfos) override;
    virtual bool PartitionPrivate(const std::string &diskId) override;
    virtual bool PartitionPublic(const std::string &diskId) override;
    virtual bool GetDisks(std::vector<std::shared_ptr<DS::DiskInfo>> &diskInfos) override;
    virtual bool SetPrimaryStorageUuid(const std::string &volumeUuid) override;
    virtual bool BenchMark(const std::string &volId,
                           const sptr<IStorageServiceTask> &storageServiceTask) override;
    virtual bool GetPrimaryStorageUuid(std::string &primaryUuid) override;
    virtual bool FindPrivateForEmulate(std::shared_ptr<DS::VolumeInfo> &priVol,
                                       std::shared_ptr<DS::VolumeInfo> &emuVol) override;
    virtual bool FindEmulateForPrivate(std::shared_ptr<DS::VolumeInfo> &emuVol,
                                       std::shared_ptr<DS::VolumeInfo> &priVol) override;

private:
    static inline BrokerDelegator<DeviceStorageServiceProxy> delegator_;
    bool SendTransactCmd(uint32_t code, MessageParcel &reply);
    template<typename T>
    bool GetParcelableInfos(uint32_t code, std::vector<std::shared_ptr<T>> &parcelableInfos);
};
} // namespace OHOS

#endif // STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_PROXY_H

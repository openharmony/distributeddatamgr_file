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

#ifndef STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_H
#define STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_H

#include <cstdio>
#include <memory>
#include <mutex>
#include <refbase.h>

#include "storage_service_stub.h"
#include "interface/storage_service_callback_stub.h"

#include "device_storage_data_mgr.h"
#include "device_storage_event_handler.h"
#include "ipc/device_storage_service_stub.h"
#include "model/volume_info.h"
#include "model/disk_info.h"
#include "system_ability.h"
namespace OHOS {
class StorageServiceCallbackStubImpl;

class DeviceStorageService final : public SystemAbility, public DeviceStorageServiceStub {
    DECLARE_SYSTEM_ABILITY(DeviceStorageService);

public:
    explicit DeviceStorageService(int32_t systemAbilityId, bool runOnCreate);
    DeviceStorageService();
    virtual ~DeviceStorageService();

    bool Mount(const std::string &volId) override;
    bool UnMount(const std::string &volId) override;
    bool Format(const std::string &volId) override;
    void OnDump() override;
    void OnStart() override;
    void OnStop() override;
    int Init() override;
    int RegisterStorageServiceCallback(sptr<IStorageServiceCallback> &istorageServiceCallback);
    bool GetVolumes(std::vector<std::shared_ptr<DS::VolumeInfo>> &volumeInfos) override;
    bool PartitionPublic(const std::string &diskId) override;
    bool PartitionPrivate(const std::string &diskId) override;
    bool GetDisks(std::vector<std::shared_ptr<DS::DiskInfo>> &diskInfos) override;
    bool SetPrimaryStorageUuid(const std::string &volumeUuid) override;
    bool GetPrimaryStorageUuid(std::string &primaryUuid) override;
    bool BenchMark(const std::string &volId, const sptr<IStorageServiceTask> &storageServiceTask) override;
    bool FindPrivateForEmulate(std::shared_ptr<DS::VolumeInfo> &priVol,
                               std::shared_ptr<DS::VolumeInfo> &emuVol) override;
    bool FindEmulateForPrivate(std::shared_ptr<DS::VolumeInfo> &emuVol,
                               std::shared_ptr<DS::VolumeInfo> &priVol) override;
    const std::shared_ptr<DeviceStorageDataMgr> GetDeviceStorageDataMgr() const;
    const std::shared_ptr<DeviceStorageEventHandler> GetDeviceStorageServiceEventHandler() const;
    void ResetProxy(const wptr<IRemoteObject> &remote);
    void OnDaemonConnected();
    void ResetIfBootedAndConnected();
    void UserAdded(int32_t userId, int32_t userSerial);
    void UserRemoved(int32_t userId);
    void SwitchUser(int32_t userId);
    void CleanupUser(int32_t userId);
    void UnlockUser(int32_t userId);
    bool SubscribeUserEvent();
    bool ConnectToStorageService();
    void IdleFsTrim(const sptr<IStorageServiceTask> &storageServiceTask);
    void RunIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask);
    void AbortIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask);
    bool IsEncrypted(const std::string &filePath)  override;
    bool IsServiceReady() const
    {
        return ready_;
    }

private:
    sptr<IStorageService> storageService_;
    sptr<IStorageServiceCallback> ssCallback_;
    mutable std::mutex mLock;
    std::shared_ptr<DeviceStorageDataMgr> dataMgr_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<DeviceStorageEventHandler> handler_;
    sptr<DeviceStorageService> instance;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_{ nullptr };
    bool ready_{ false };
};

class StorageServiceCallbackStubImpl : public StorageServiceCallbackStub {
public:
    StorageServiceCallbackStubImpl(sptr<DeviceStorageService> &service);
    virtual ~StorageServiceCallbackStubImpl();
    virtual void OnDiskCreated(const std::string &diskId, int32_t flags) override;
    virtual void OnDiskMetadataChanged(const std::string &diskId,
                                       uint64_t sizeBytes,
                                       const std::string &label,
                                       const std::string &sysPath) override;
    virtual void OnDiskDestroyed(const std::string &diskId) override;
    virtual void OnVolumeCreated(const std::string &volId,
                                 int32_t type,
                                 const std::string &diskId,
                                 const std::string &partGuid) override;
    virtual void OnVolumeStateChanged(const std::string &volId, int32_t state) override;
    virtual void OnVolumeMetadataChanged(const std::string &volId,
                                         const std::string &fsType,
                                         const std::string &fsUuid,
                                         const std::string &fsLabel) override;
    virtual void OnVolumePathChanged(const std::string &volId, const std::string &path) override;
    virtual void OnVolumeInternalPathChanged(const std::string &volId,
                                             const std::string &internalPath) override;
    virtual void OnVolumeDestroyed(const std::string &volId) override;

private:
    sptr<DeviceStorageService> service_;
};
} // namespace OHOS

#endif // STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_H

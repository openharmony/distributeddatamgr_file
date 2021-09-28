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
#include "device_storage_service.h"

#include <cstring>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "common_event_manager.h"
#include "common_event_support.h"
#include "device_storage_service_errors.h"
#include "device_storage_service_hilog.h"
#include "device_user_broadcast_subscriber.h"
#include "ipc/storage_service_death_recipient.h"
#include "ipc/storage_service_task_stub_impl.h"
#include "model/device_storage_constants.h"
#include "storage_service_proxy.h"

namespace OHOS {
REGISTER_SYSTEM_ABILITY_BY_ID(DeviceStorageService, DEVICE_STORAGE_MANAGER_SERVICE_ID, true)

DeviceStorageService::DeviceStorageService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate)
{
    SSLOG_I("DEBUG DeviceStorageService");
}

DeviceStorageService::DeviceStorageService()
{
    SSLOG_I("instance created with no para");
}
DeviceStorageService::~DeviceStorageService()
{
    SSLOG_I("DEBUG ~DeviceStorageService");
    if (dataMgr_) {
        dataMgr_.reset();
    }
    if (handler_) {
        handler_.reset();
    }
    if (runner_) {
        runner_.reset();
    }
    if (storageService_ != nullptr) {
        auto remoteObject = storageService_->AsObject();
        if (remoteObject != nullptr) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
        }
    }
}

void DeviceStorageService::OnStart()
{
    SSLOG_E("DeviceStorageService::OnStart");
    if (ready_) {
        SSLOG_E("OnStart is ready, nothing to do.");
        return;
    }
    storageService_ = nullptr;
    instance = this;
    if (!dataMgr_) {
        SSLOG_I("Create DeviceStorageDataMgr");
        dataMgr_ = std::make_shared<DeviceStorageDataMgr>(instance);
    }
    dataMgr_->SetDefaultPrimaryUuid();
    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create("DeviceStorageService");
        if (!runner_) {
            SSLOG_E("create runner fail");
        }
    }
    SSLOG_D("create runner success");
    if (!handler_) {
        handler_ = std::make_shared<DeviceStorageEventHandler>(runner_, instance);
        if (!handler_) {
            SSLOG_E("create DS event handler fail");
        }
    }
    SubscribeUserEvent();
    bool res = Publish(this);
    if (res) {
        SSLOG_E("DeviceStorageService::OnStart start Success!");
    }
    Init();
    ready_ = true;
}

void DeviceStorageService::OnDump()
{
    SSLOG_E("DeviceStorageService::OnDump start");
}
void DeviceStorageService::OnStop()
{
    SSLOG_E("DeviceStorageService::OnStop start");
    if (!ready_) {
        return;
    }
    if (dataMgr_) {
        dataMgr_.reset();
    }
    if (handler_) {
        handler_.reset();
    }
    if (runner_) {
        runner_.reset();
    }
    ready_ = false;
}
int DeviceStorageService::Init()
{
    SSLOG_E("DeviceStorageService::Init start lli.wangok17");
    int errorCode = ERR_OK;
    if (!ConnectToStorageService()) {
        errorCode = E_GET_STORAGE_SERVICE_FAILED;
        goto NEXT;
    }
    if (storageService_ != nullptr) {
        if (ssCallback_ == nullptr) {
            ssCallback_ = new StorageServiceCallbackStubImpl(instance);
        }
        int result = RegisterStorageServiceCallback(ssCallback_);
        if (result != ERR_OK) {
            storageService_ = nullptr;
            errorCode = E_REGISTER_CALLBACK_FAILED;
            goto NEXT;
        }
    }
NEXT:
    SSLOG_E("DeviceStorageService::Init NEXT start!!!");
    if (storageService_ == nullptr) {
        SSLOG_E("DeviceStorageService::OnStart storageService is nullptr!");
        if (handler_ != nullptr) {
            SSLOG_E("DeviceStorageService::OnStart Reconnect the SS!");
            handler_->SendEvent(DeviceStorageEventHandler::SS_RECONNECT, 0, DS::Constants::DELAY_TIME);
        }
        return errorCode;
    } else {
        SSLOG_E("DeviceStorageService:: OnDaemonConnected!");
        OnDaemonConnected();
    }
    SSLOG_E("DeviceStorageService::Init end");
    return ERR_OK;
}

void DeviceStorageService::ResetProxy(const wptr<IRemoteObject> &remote)
{
    if (storageService_ == nullptr) {
        return;
    }
    auto serviceRemote = storageService_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        storageService_ = nullptr;
    }
}
const std::shared_ptr<DeviceStorageDataMgr> DeviceStorageService::GetDeviceStorageDataMgr() const
{
    return dataMgr_;
}

const std::shared_ptr<DeviceStorageEventHandler> DeviceStorageService::GetDeviceStorageServiceEventHandler() const
{
    return handler_;
}
bool DeviceStorageService::Mount(const std::string &volId)
{
    SSLOG_I("DeviceStorageService::Mount Start");
    // todo:add permissionCheck;
    std::shared_ptr<DS::VolumeInfo> info;
    GetDeviceStorageDataMgr()->FindVolumeById(info, volId);
    if (storageService_ != nullptr && info != nullptr) {
        return storageService_->Mount(info->GetId(), info->GetMountFlags(), info->GetMountUserId());
    }
    SSLOG_I("DeviceStorageService::Mount id::%{public}s", volId.c_str());
    SSLOG_I("DeviceStorageService::Mount end");
    return false;
}

bool DeviceStorageService::UnMount(const std::string &volId)
{
    SSLOG_I("DeviceStorageService::UnMount Start");
    // todo:add permissionCheck
    if (storageService_ != nullptr) {
        return storageService_->UnMount(volId);
    }
    SSLOG_I("DeviceStorageService::UnMount id::%{public}s", volId.c_str());
    SSLOG_I("DeviceStorageService::UnMount end");
    return false;
}
bool DeviceStorageService::Format(const std::string &volId)
{
    SSLOG_I("DeviceStorageService::Format Start");
    // todo:add permissionCheck
    if (storageService_ != nullptr) {
        return storageService_->Format(volId, "auto");
    }
    SSLOG_I("DeviceStorageService::Format id::%{public}s", volId.c_str());
    SSLOG_I("DeviceStorageService::Format end");
    return false;
}
bool DeviceStorageService::IsEncrypted(const std::string &filePath)
{
    SSLOG_I("DeviceStorageService::IsEncrypted Start");
    // todo:add permissionCheck
    if (storageService_ != nullptr) {
        return storageService_->IsEncrypted(filePath);
    }
    SSLOG_I("DeviceStorageService::IsEncrypted id::%{public}s", filePath.c_str());
    SSLOG_I("DeviceStorageService::IsEncrypted end");
    return false;
}

int DeviceStorageService::RegisterStorageServiceCallback(
    sptr<IStorageServiceCallback> &istorageServiceCallback)
{
    SSLOG_I("DeviceStorageService::RegisterStorageServiceCallback Start");
    if (storageService_ != nullptr) {
        bool result = storageService_->RegisterStorageServiceCallback(istorageServiceCallback);
        if (!result) {
            return E_REGISTER_CALLBACK_FAILED;
        }
        SSLOG_I("DeviceStorageService::RegisterStorageServiceCallback end");
    }
    return ERR_OK;
}
bool DeviceStorageService::GetVolumes(std::vector<std::shared_ptr<DS::VolumeInfo>> &volumeInfos)
{
    SSLOG_I("DeviceStorageService::GetVolumes");
    GetDeviceStorageDataMgr()->GetVolumeInfos(volumeInfos);
    if (volumeInfos.empty()) {
        SSLOG_I("volumeInfos_ data is empty");
        return false;
    }
    SSLOG_I("DeviceStorageService::getvolumeInfos size is %{public}d", volumeInfos.size());
    SSLOG_I("get volumes success");
    return true;
}

bool DeviceStorageService::GetPrimaryStorageUuid(std::string &primaryUuid)
{
    SSLOG_I("DeviceStorageService::GetPrimaryStorageUuid");
    if (dataMgr_ == nullptr) {
        return false;
    }
    return dataMgr_->GetPrimaryUuid(primaryUuid);
}

bool DeviceStorageService::FindPrivateForEmulate(std::shared_ptr<DS::VolumeInfo> &priVol,
                                                 std::shared_ptr<DS::VolumeInfo> &emuVol)
{
    SSLOG_I("DeviceStorageService::FindPrivateForEmulate");
    if (dataMgr_ == nullptr) {
        return false;
    }
    priVol = dataMgr_->FindPrivateForEmulate(emuVol);
    return true;
}

bool DeviceStorageService ::FindEmulateForPrivate(std::shared_ptr<DS::VolumeInfo> &emuVol,
                                                  std::shared_ptr<DS::VolumeInfo> &priVol)
{
    SSLOG_I("DeviceStorageService::FindEmulateForPrivate");
    if (dataMgr_ == nullptr) {
        return false;
    }
    emuVol = dataMgr_->FindEmulateForPrivate(priVol);
    return true;
}

void DeviceStorageService::UserAdded(int32_t userId, int32_t userSerial)
{
    SSLOG_I("DeviceStorageService::UserAdded userId is %{public}d and userSerial is %{public}d", userId,
            userSerial);
    if (storageService_ != nullptr) {
        storageService_->UserAdded(userId, userSerial);
    }
}

void DeviceStorageService::UserRemoved(int32_t userId)
{
    SSLOG_I("DeviceStorageService::UserRemoved userId is %{public}d ", userId);

    if (storageService_ != nullptr) {
        dataMgr_->UnmountVolForUser(userId);
        storageService_->UserRemoved(userId);
    }
}

void DeviceStorageService::SwitchUser(int32_t userId)
{
    SSLOG_I("DeviceStorageService::SwitchUser userId  is %{public}d ", userId);
    if (dataMgr_ != nullptr) {
        dataMgr_->SetCurrentUser(userId);
    }
}

void DeviceStorageService::CleanupUser(int32_t userId)
{
    SSLOG_I("DeviceStorageService::CleanupUser userId  is %{public}d ", userId);
    if (storageService_ != nullptr) {
        storageService_->UserStoped(userId);
    }
    dataMgr_->RemoveunlockUser(userId);
}

void DeviceStorageService::UnlockUser(int32_t userId)
{
    SSLOG_I("DeviceStorageService::UnlockUser userId  is %{public}d ", userId);
    if (storageService_ != nullptr) {
        storageService_->UserStarted(userId);
    }
    if (handler_ != nullptr) {
        handler_->SendEvent(DeviceStorageEventHandler::COMPLETE_UNLOCK_USER, userId, 0);
    }
}

bool DeviceStorageService::SetPrimaryStorageUuid(const std::string &volumeUuid)
{
    SSLOG_I("DeviceStorageService::SetPrimaryStorageUuid Start");
    // todo:add permissionCheck
    std::string fromVolId;
    std::string toVolId;
    dataMgr_->SetPrimaryUuid(volumeUuid, fromVolId, toVolId);
    if (storageService_ != nullptr) {
        sptr<IStorageServiceTask> ssTask = new StorageServiceTaskStubImpl(instance);
        return storageService_->MoveStorage(fromVolId, toVolId, ssTask);
    }
    return false;
}

bool DeviceStorageService::BenchMark(const std::string &volId,
                                     const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("DeviceStorageService::BenchMark Start");
    // todo:add permissionCheck
    if (storageService_ != nullptr) {
        return storageService_->BenchMark(volId, storageServiceTask);
    }
    return false;
}

bool DeviceStorageService::GetDisks(std::vector<std::shared_ptr<DS::DiskInfo>> &diskInfos)
{
    GetDeviceStorageDataMgr()->GetDiskInfos(diskInfos);
    if (diskInfos.empty()) {
        SSLOG_I("diskInfos_ data is empty");
        return false;
    }
    SSLOG_I("get disks success");
    return true;
}

bool DeviceStorageService::PartitionPublic(const std::string &diskId)
{
    SSLOG_I("DeviceStorageService::PartitionPublic Start");
    // todo:add permissionCheck
    if (storageService_ != nullptr) {
        return storageService_->Partition(diskId, 0, -1);
    }
    return false;
    SSLOG_I("DeviceStorageService::PartitionPublic End");
}

bool DeviceStorageService::PartitionPrivate(const std::string &diskId)
{
    SSLOG_I("DeviceStorageService::PartitionPrivate Start");
    // todo:add permissionCheck
    if (storageService_ != nullptr) {
        return storageService_->Partition(diskId, 1, -1);
    }
    return false;
    SSLOG_I("DeviceStorageService::PartitionPrivate End");
}

void DeviceStorageService::OnDaemonConnected()
{
    SSLOG_I("DeviceStorageService::onDaemonConnected");
    handler_->SendEvent(DeviceStorageEventHandler::DAEMON_CONNECTED);
}

void DeviceStorageService::ResetIfBootedAndConnected()
{
    SSLOG_I("DeviceStorageService::ResetIfBootedAndConnected");
    dataMgr_->InitData();
    if (storageService_ != nullptr) {
        storageService_->Reset();
        storageService_->UserStarted(DS::Constants::USER_SYSTEM);
    }
}

bool DeviceStorageService::SubscribeUserEvent()
{
    SSLOG_D("RegistSubscriber start.\n");
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_ADDED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<DeviceUserBroadcastSubscriber> subscriberTest =
        std::make_shared<DeviceUserBroadcastSubscriber>(subscriberInfo, instance);
    if (subscriberTest == nullptr) {
        return false;
    }
    bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberTest);
    SSLOG_I("RegisterSubscriber end###subscribeResult = %{public}d\n", subscribeResult);
    return subscribeResult;
}

bool DeviceStorageService::ConnectToStorageService()
{
    SSLOG_I("ConnectToStorageService");
    if (storageService_ == nullptr) {
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            SSLOG_E(" DeviceStorageService::Init sam == nullptr");
            return false;
        }
        auto object = sam->GetSystemAbility(STORAGE_SERVICE_ID);
        if (object == nullptr) {
            SSLOG_E("DeviceStorageService::Init object == nullptr");
            return false;
        }
        deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new StorageServiceDeathRecipient(instance));
        if (deathRecipient_ == nullptr) {
            SSLOG_E("Fail to create StorageServiceDeathRecipient");
            return false;
        }
        if ((object->IsProxyObject()) && (!object->AddDeathRecipient(deathRecipient_))) {
            SSLOG_E("Add death recipient to DeviceStorageService failed.");
            return false;
        }
        storageService_ = iface_cast<IStorageService>(object);
        if (storageService_ == nullptr) {
            SSLOG_E("failed to iface_cast object to storageService_");
            return false;
        }
        return true;
    }
    return true;
}

StorageServiceCallbackStubImpl::StorageServiceCallbackStubImpl(sptr<DeviceStorageService> &service)
    : service_(service)
{}
StorageServiceCallbackStubImpl::~StorageServiceCallbackStubImpl() {}
void StorageServiceCallbackStubImpl::OnDiskCreated(const std::string &diskId, int32_t flags)
{
    SSLOG_I("call back OnDiskCreated OnFinished result flag and diskId is %{public}d, %{public}s", flags,
            diskId.c_str());
    service_->GetDeviceStorageDataMgr()->AddDiskInfo(diskId, flags);
}

void StorageServiceCallbackStubImpl::OnDiskMetadataChanged(const std::string &diskId,
                                                           uint64_t sizeBytes,
                                                           const std::string &label,
                                                           const std::string &sysPath)
{
    SSLOG_I("call back OnDiskMetadataChanged OnFinished result diskId and sizebytes and label and "
            "sysPath is ");
    SSLOG_I("sysPath is %{public}s, %{public}llu,%{public}s, %{public}s", diskId.c_str(), sizeBytes,
            label.c_str(), sysPath.c_str());
    service_->GetDeviceStorageDataMgr()->SetMetaDataforDisk(diskId, sizeBytes, label, sysPath);
}

void StorageServiceCallbackStubImpl::OnDiskDestroyed(const std::string &diskId)
{
    SSLOG_I("call back OnDiskDestroyed OnFinished result diskId is%{public}s", diskId.c_str());
    service_->GetDeviceStorageDataMgr()->DeleteDiskInfo(diskId);
}

void StorageServiceCallbackStubImpl::OnVolumeCreated(const std::string &volId,
                                                     int32_t type,
                                                     const std::string &diskId,
                                                     const std::string &partGuid)
{
    SSLOG_I("call back OnVolumeCreated OnFinished result volId and type  and diskId and partGuid is ");
    SSLOG_I("%{public}s, %{public}d,%{public}s, %{public}s", volId.c_str(), type, diskId.c_str(),
            partGuid.c_str());
    service_->GetDeviceStorageDataMgr()->AddVolumeInfo(volId, type, diskId, partGuid);
}

void StorageServiceCallbackStubImpl::OnVolumeStateChanged(const std::string &volId, int32_t state)
{
    SSLOG_I("call back OnVolumeStateChanged OnFinished result state and volId is %{public}d, %{public}s",
            state, volId.c_str());
    service_->GetDeviceStorageDataMgr()->NotifyVolumeState(volId, state);
}

void StorageServiceCallbackStubImpl::OnVolumeMetadataChanged(const std::string &volId,
                                                             const std::string &fsType,
                                                             const std::string &fsUuid,
                                                             const std::string &fsLabel)
{
    SSLOG_I("call back OnVolumeMetadataChanged OnFinished result volId and fsType  and fsUuid and "
            "fsLabel is ");
    SSLOG_I("%{public}s, %{public}s,%{public}s, %{public}s", volId.c_str(), fsType.c_str(), fsUuid.c_str(),
            fsLabel.c_str());
    service_->GetDeviceStorageDataMgr()->SetMetaDataforVolume(volId, fsType, fsUuid, fsLabel);
}

void StorageServiceCallbackStubImpl::OnVolumePathChanged(const std::string &volId, const std::string &path)
{
    SSLOG_I("call back OnVolumePathChanged OnFinished result path and volId is %{public}s, %{public}s",
            path.c_str(), volId.c_str());
    service_->GetDeviceStorageDataMgr()->SetPathforVolume(volId, path);
}

void StorageServiceCallbackStubImpl::OnVolumeInternalPathChanged(const std::string &volId,
                                                                 const std::string &internalPath)
{
    SSLOG_I("call back OnVolumeInternalPathChanged OnFinished result internalPath and volId is ");
    SSLOG_I("%{public}s, %{public}s", internalPath.c_str(), volId.c_str());
    service_->GetDeviceStorageDataMgr()->SetInternalPathforVolume(volId, internalPath);
}
void StorageServiceCallbackStubImpl::OnVolumeDestroyed(const std::string &volId)
{
    SSLOG_I("call back OnVolumeDestroyed OnFinished result  volId is %{public}s ", volId.c_str());
    service_->GetDeviceStorageDataMgr()->DeleteVolumeInfo(volId);
}

void DeviceStorageService::IdleFsTrim(const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("DeviceStorageService::IdleFsTrim Start");
    if (storageService_ != nullptr) {
        storageService_->IdleFsTrim(storageServiceTask);
    } 
}

void DeviceStorageService::RunIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("DeviceStorageService::RunIdleMaintain Start");
    if (storageService_ != nullptr) {
        storageService_->RunIdleMaintain(storageServiceTask);
    }
}

void DeviceStorageService::AbortIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("DeviceStorageService::AbortIdleMaintain Start");
    if (storageService_ != nullptr) {
        storageService_->AbortIdleMaintain(storageServiceTask);
    }
}
} // namespace OHOS

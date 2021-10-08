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
#include "storage_service.h"

#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <thread>
#include <unistd.h>

#include "fbe_crypto.h"
#include "fs_mount_tab.h"
#include "idle_maintain.h"
#include "interface/storage_service_callback_death_recipient.h"
#include "interface/storage_service_callback_proxy.h"
#include "ipc_object_stub.h"
#include "ss_tab_mgr.h"
#include "storage_constant.h"
#include "storage_hilog.h"
#include "storage_manager.h"
#include "storage_service_proxy.h"

using namespace OHOS::StorageService::Constants;

namespace OHOS {
bool StorageNativeService::Mount(std::string volId, int32_t MountFlags, int32_t MountUserId)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto callingUid = OHOS::IPCSkeleton::GetCallingUid();
    int callFlag = 10000;
    if (callingUid > callFlag) {
        return -1;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d volId=%{public}s", __FILE__, __func__, __LINE__,
            volId.c_str());
    auto vol = StorageManager::Instance()->FindVolume(volId);
    if (vol == nullptr) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    vol->SetInfo("", "", "", -1, MountFlags, true, OHOS::SWITCH_FLAGS_SETMOUNTFLAGS);
    vol->SetInfo("", "", "", MountUserId, -1, true, OHOS::SWITCH_FLAGS_SETMOUNTUSERID);
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    int res = vol->Mount();
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (res != OK) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if ((MountFlags & MOUNT_FLAG_PRIMARY) != 0) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        res = StorageManager::Instance()->HandlePrimary(0, vol, 1);
        if (res != OK) {
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            return false;
        }
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return true;
}

bool StorageNativeService::UnMount(std::string volId)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto callingUid = OHOS::IPCSkeleton::GetCallingUid();
    int callFlag = 10000;
    if (callingUid > callFlag) {
        return -1;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto vol = StorageManager::Instance()->FindVolume(volId);
    if (vol == nullptr) {
        return false;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    vol->UnMount();
    std::cout << "Storage Service UnMount end!!!!!!" << std::endl;
    return true;
}

bool StorageNativeService::Format(std::string volId, const std::string &fsType)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto callingUid = OHOS::IPCSkeleton::GetCallingUid();
    int callFlag = 10000;
    if (callingUid > callFlag) {
        return -1;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto vol = StorageManager::Instance()->FindVolume(volId);
    if (vol == nullptr) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    std::string fstype = "ext4";
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    vol->Format(fstype);
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    std::cout << "Storage Service Format end!!!!!!" << std::endl;
    return true;
}

bool StorageNativeService::PersistFstab(std::string fstab)
{
    SSLOG_I("dugl enter PersistFstab");
    FsMountTab::FsMountEntry entry = {};
    if (FsMountTab::ParseMountEntry(fstab, entry)) {
        auto sm = StorageManager::Instance();
        sm->ssTabMgr->InsertEntry(entry);
    } else {
        SSLOGFE("PersistFstab insert entry fail");
        return false;
    }
    // persist fstab from fs_manager.
    return true;
}

bool StorageNativeService::IsEncrypted(std::string filePath)
{
    SSLOG_I("dugl enter IsEncrypted");
    FsMountTab::FsMountEntry entry = {};
    auto sm = StorageManager::Instance();
    if (sm->ssTabMgr->GetEntry(filePath, entry)) {
        SSLOGFE("Read Mount option failed for filePath");
        return false;
    }
    if (!entry.IsEncryptable()) {
        SSLOGFE("Is not encrypt for filePath ");
        return false;
    }
    return true;
}

bool StorageNativeService::RegisterStorageServiceCallback(
    const sptr<IStorageServiceCallback> &storageServiceCallback)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    mStroageServiceCallback = storageServiceCallback;
    if (storageServiceCallback->AsObject() != nullptr) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        sptr<StorageServiceCallbackDeathRecipient> deathRecipient =
            new StorageServiceCallbackDeathRecipient();
        storageServiceCallback->AsObject()->AddDeathRecipient(deathRecipient);
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    StorageManager::Instance()->setListener(mStroageServiceCallback);
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return true;
}

bool StorageNativeService::Partition(const std::string &diskId, int32_t partitionType, int32_t ratio)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto callingUid = OHOS::IPCSkeleton::GetCallingUid();
    int callFlag = 10000;
    if (callingUid > callFlag) {
        return -1;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto disk = StorageManager::Instance()->FindDisk(diskId);
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (disk == nullptr) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    switch (partitionType) {
        case PARTITION_TYPE_PUBLIC: {
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            if (disk->PartitionHandle(1, 0)) {
                SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
                return true;
            }
        } break;
        case PARTITION_TYPE_PRIVATE: {
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            int handleFlag = 2;
            if (disk->PartitionHandle(handleFlag, 0)) {
                SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
                return true;
            }
        } break;
        case PARTITION_TYPE_MIXED: {
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            int handleFlag = 2;
            if (disk->PartitionHandle(handleFlag, ratio)) {
                SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
                return true;
            }
        } break;
        default: {
            SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
            return false;
        }
    }
    return false;
}

bool StorageNativeService::Reset()
{
    SSLOG_I("StorageNativeService::Reset");
    if (StorageManager::Instance()->Reset()) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::UserAdded(int32_t userId, int32_t userSerial)
{
    SSLOG_I("StorageNativeService::UserAdded");
    if (StorageManager::Instance()->OnUserHandle(userId, userSerial, 1)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::UserRemoved(int32_t userId)
{
    SSLOG_I("StorageNativeService::UserRemoved");
    int handleFlag = 2;
    if (StorageManager::Instance()->OnUserHandle(userId, -1, handleFlag)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::UserStarted(int32_t userId)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto callingUid = OHOS::IPCSkeleton::GetCallingUid();
    int callFlag = 10000;
    if (callingUid > callFlag) {
        return -1;
    }
    SSLOG_I("StorageNativeService::UserStarted");
    int handleFlag = 3;
    if (StorageManager::Instance()->OnUserHandle(userId, -1, handleFlag)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::UserStoped(int32_t userId)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto callingUid = OHOS::IPCSkeleton::GetCallingUid();
    int callFlag = 10000;
    if (callingUid > callFlag) {
        return -1;
    }
    SSLOG_I("StorageNativeService::UserStoped");
    int handleFlag = 4;
    if (StorageManager::Instance()->OnUserHandle(userId, -1, handleFlag)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::MoveStorage(std::string fromVolId,
                                       std::string toVolId,
                                       const sptr<IStorageServiceTask> &storageServiceTask)
{
    return true;
}

bool StorageNativeService::BenchMark(const std::string &volId,
                                     const sptr<IStorageServiceTask> &storageServiceTask)
{
    return true;
}

bool StorageNativeService::CryptoEnable()
{
    if (FbeCrypto::Enable()) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::CryptoInitialize()
{
    const sptr<IRemoteObject> impl;
    StorageServiceProxy StorageServiceProxy1(impl);
    if (FbeCrypto::Initialize()) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::CryptoCreateKey(int32_t userId)
{
    const sptr<IRemoteObject> impl;
    StorageServiceProxy StorageServiceProxy1(impl);
    if (FbeCrypto::CreateKey(userId)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::CryptoDeleteKey(int32_t userId)
{
    const sptr<IRemoteObject> impl;
    StorageServiceProxy StorageServiceProxy1(impl);
    if (FbeCrypto::DeleteKey(userId)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::CryptoAddAuthKey(int32_t userId,
                                            const std::string &token,
                                            const std::string &secret)
{
    const sptr<IRemoteObject> impl;
    StorageServiceProxy StorageServiceProxy1(impl);
    if (FbeCrypto::AddAuthKey(userId, token, secret)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::CryptoDelAuthKey(int32_t userId,
                                            const std::string &token,
                                            const std::string &secret)
{
    const sptr<IRemoteObject> impl;
    StorageServiceProxy StorageServiceProxy1(impl);
    if (FbeCrypto::DelAuthKey(userId, token, secret)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::CryptoUnlockKey(int32_t userId,
                                           const std::string &token,
                                           const std::string &secret)
{
    const sptr<IRemoteObject> impl;
    StorageServiceProxy StorageServiceProxy1(impl);
    if (FbeCrypto::UnLockKey(userId, token, secret)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::CryptoLockKey(int32_t userId)
{
    const sptr<IRemoteObject> impl;
    StorageServiceProxy StorageServiceProxy1(impl);
    if (FbeCrypto::LockKey(userId)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::CryptoInitUserSpace(const std::string &uuid, int32_t userId, int32_t flag)
{
    const sptr<IRemoteObject> impl;
    StorageServiceProxy StorageServiceProxy1(impl);
    if (FbeCrypto::InitUserSpace(uuid, userId, flag)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::CryptoRemoveUserSpace(const std::string &uuid, int32_t userId, int32_t flag)
{
    if (FbeCrypto::RemoveUserSpace(uuid, userId, flag)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::CryptoUpdateAuthKey(int32_t userId,
                                               const std::string &token,
                                               const std::string secret)
{
    if (FbeCrypto::UpdateAuthKey(userId, token, secret)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return false;
    }
    return true;
}

bool StorageNativeService::IdleFsTrim(const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("StorageNativeService::IdleFsTrim");
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    std::thread([=]() { OHOS::IdleFsTrim(storageServiceTask); }).detach();
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return true;
}

bool StorageNativeService::RunIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("StorageNativeService::RunIdleMaintain");
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    std::thread([=]() { OHOS::RunIdleMaintain(storageServiceTask); }).detach();
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return true;
}

bool StorageNativeService::AbortIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("StorageNativeService::AbortIdleMaintain");
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    std::thread([=]() { OHOS::AbortIdleMaintain(storageServiceTask); }).detach();
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return true;
}
} // namespace OHOS

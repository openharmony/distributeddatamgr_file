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

#ifndef SERVICES_SS_INCLUDE_SERVER_STORAGE_SERVICE_H
#define SERVICES_SS_INCLUDE_SERVER_STORAGE_SERVICE_H
#include <list>
#include <mutex>
#include "storage_service_stub.h"

namespace OHOS {
static constexpr int SWITCH_FLAGS_SETMOUNTUSERID = 4;
static constexpr int SWITCH_FLAGS_SETMOUNTFLAGS = 5;

static const int MOUNT_FLAG_PRIMARY = 1;
static const int PARTITION_TYPE_PUBLIC = 0;
static const int PARTITION_TYPE_PRIVATE = 1;
static const int PARTITION_TYPE_MIXED = 2;

class StorageNativeService : public StorageServiceStub {
public:
    virtual bool Mount(std::string volId, int32_t MountFlags, int32_t MountUserId) override;
    virtual bool UnMount(std::string volId) override;
    virtual bool Format(std::string volId, const std::string &fsType) override;
    virtual bool
    RegisterStorageServiceCallback(const sptr<IStorageServiceCallback> &storageServiceCallback) override;
    virtual bool Partition(const std::string &diskId, int32_t partitionType, int32_t ratio) override;
    virtual bool Reset() override;
    virtual bool UserAdded(int32_t userId, int32_t userSerial) override;
    virtual bool UserRemoved(int32_t userId) override;
    virtual bool UserStarted(int32_t userId) override;
    virtual bool UserStoped(int32_t userId) override;
    virtual bool MoveStorage(std::string fromVolId,
                             std::string toVolId,
                             const sptr<IStorageServiceTask> &storageServiceTask) override;
    virtual bool CryptoEnable() override;
    virtual bool CryptoInitialize() override;
    virtual bool CryptoCreateKey(int32_t userId) override;
    virtual bool CryptoDeleteKey(int32_t userId) override;
    virtual bool
    CryptoAddAuthKey(int32_t userId, const std::string &token, const std::string &secret) override;
    virtual bool
    CryptoDelAuthKey(int32_t userId, const std::string &token, const std::string &secret) override;
    virtual bool
    CryptoUnlockKey(int32_t userId, const std::string &token, const std::string &secret) override;
    virtual bool CryptoLockKey(int32_t userId) override;
    virtual bool CryptoInitUserSpace(const std::string &uuid, int32_t userId, int32_t flag) override;
    virtual bool CryptoRemoveUserSpace(const std::string &uuid, int32_t userId, int32_t flag) override;
    virtual bool
    CryptoUpdateAuthKey(int32_t userId, const std::string &token, const std::string secret) override;
    virtual bool BenchMark(const std::string &volId,
                           const sptr<IStorageServiceTask> &storageServiceTask) override;
    virtual bool IdleFsTrim(const sptr<IStorageServiceTask> &storageServiceTask) override;
    virtual bool RunIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask) override;
    virtual bool AbortIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask) override;
    virtual bool PersistFstab(std::string fstab) override;
    virtual bool IsEncrypted(std::string filePath) override;

private:
    std::mutex callbacks_mutex;
    std::condition_variable condition_;
    sptr<IStorageServiceCallback> mStroageServiceCallback;
};
} // namespace OHOS

#endif // SERVICES_SS_INCLUDE_SERVER_STORAGE_SERVICE_H

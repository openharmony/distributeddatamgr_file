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

#ifndef SERVICES_SS_INCLUDE_CLIENT_STORAGE_SERVICE_PROXY_H
#define SERVICES_SS_INCLUDE_CLIENT_STORAGE_SERVICE_PROXY_H

#include "istorage_service.h"

namespace OHOS {
class StorageServiceProxy : public IRemoteProxy<IStorageService> {
public:
    StorageServiceProxy(const sptr<IRemoteObject> &impl);
    ~StorageServiceProxy();
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
    virtual bool BenchMark(const std::string &volId,
                           const sptr<IStorageServiceTask> &storageServiceTask) override;
    bool CryptoEnable() override;
    bool CryptoInitialize() override;
    bool CryptoCreateKey(int32_t userId) override;
    bool CryptoDeleteKey(int32_t userId) override;
    bool CryptoAddAuthKey(int32_t userId, const std::string &token, const std::string &secret) override;
    bool CryptoDelAuthKey(int32_t userId, const std::string &token, const std::string &secret) override;
    bool CryptoUnlockKey(int32_t userId, const std::string &token, const std::string &secret) override;
    bool CryptoLockKey(int32_t userId) override;
    bool CryptoInitUserSpace(const std::string &uuid, int32_t userId, int32_t flag) override;
    bool CryptoRemoveUserSpace(const std::string &uuid, int32_t userId, int32_t flag) override;
    bool CryptoUpdateAuthKey(int32_t userId, const std::string &token, const std::string secret) override;
    virtual bool IdleFsTrim(const sptr<IStorageServiceTask> &storageServiceTask) override;
    virtual bool RunIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask) override;
    virtual bool AbortIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask) override;
    virtual bool PersistFstab(std::string fstab) override;
    virtual bool IsEncrypted(std::string filePath) override;

private:
    static inline BrokerDelegator<StorageServiceProxy> delegator_;
};
} // namespace OHOS

#endif // SERVICES_SS_INCLUDE_CLIENT_STORAGE_SERVICE_PROXY_H

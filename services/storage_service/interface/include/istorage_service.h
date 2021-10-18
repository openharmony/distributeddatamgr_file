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

#ifndef SERVICES_SS_INCLUDE_STORAGE_SERVICE_BASE_H
#define SERVICES_SS_INCLUDE_STORAGE_SERVICE_BASE_H

#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "interface/storage_service_callback_interface.h"
#include "ipc/i_storage_service_task.h"

namespace OHOS {
class IStorageService : public IRemoteBroker {
public:
    enum {
        SS_INIT = 1,
        SS_DOMOUNT,
        SS_UNMOUNT,
        SS_FORMAT,
        SS_REGISTERCALLBACK,
        SS_PARTITION,
        SS_RESET,
        SS_USERADDED,
        SS_USERREMOVED,
        SS_USERSTARTED,
        SS_USERSTOPED,
        SS_MOVESTORAGE,
        SS_BENCHMARK,
        SS_CRYPTO_ENABLE,
        SS_CRYPTO_INITIALIZE,
        SS_CRYPTO_CREATEKEY,
        SS_CRYPTO_DELETEKEY,
        SS_CRYPTO_ADDAUTHKEY,
        SS_CRYPTO_DELAUTHKEY,
        SS_CRYPTO_UNLOCKKEY,
        SS_CRYPTO_LOCKKEY,
        SS_CRYPTO_INITUSERSPACE,
        SS_CRYPTO_REMOVEUSERSPACE,
        SS_CRYPTO_UPDATEAUTHKEY,
        SS_IDLEFSTRIM = 25,
        SS_RUNIDLEMAINTAIN = 26,
        SS_ABORTIDLEMAINTAIN = 27,
        SS_ISENCRYPTED,
        SS_PERSISTFSTAB = 40
    };

    virtual bool Mount(std::string volId, int32_t MountFlags, int32_t MountUserId) = 0;
    virtual bool UnMount(std::string volId) = 0;
    virtual bool Format(std::string volId, const std::string &fsType) = 0;
    virtual bool Partition(const std::string &diskId, int32_t partitionType, int32_t ratio) = 0;
    virtual bool Reset() = 0;
    virtual bool UserAdded(int32_t userId, int32_t userSerial) = 0;
    virtual bool UserRemoved(int32_t userId) = 0;
    virtual bool UserStarted(int32_t userId) = 0;
    virtual bool UserStoped(int32_t userId) = 0;
    virtual bool MoveStorage(std::string fromVolId,
                             std::string toVolId,
                             const sptr<IStorageServiceTask> &storageServiceTask) = 0;
    virtual bool BenchMark(const std::string &volId, const sptr<IStorageServiceTask> &storageServiceTask) = 0;
    virtual bool
    RegisterStorageServiceCallback(const sptr<IStorageServiceCallback> &storageServiceCallback) = 0;
    virtual bool CryptoEnable() = 0;
    virtual bool CryptoInitialize() = 0;
    virtual bool CryptoCreateKey(int32_t userId) = 0;
    virtual bool CryptoDeleteKey(int32_t userId) = 0;
    virtual bool CryptoAddAuthKey(int32_t userId, const std::string &token, const std::string &secret) = 0;
    virtual bool CryptoDelAuthKey(int32_t userId, const std::string &token, const std::string &secret) = 0;
    virtual bool CryptoUnlockKey(int32_t userId, const std::string &token, const std::string &secret) = 0;
    virtual bool CryptoLockKey(int32_t userId) = 0;
    virtual bool CryptoInitUserSpace(const std::string &uuid, int32_t userId, int32_t flag) = 0;
    virtual bool CryptoRemoveUserSpace(const std::string &uuid, int32_t userId, int32_t flag) = 0;
    virtual bool CryptoUpdateAuthKey(int32_t userId, const std::string &token, const std::string secret) = 0;
    virtual bool IdleFsTrim(const sptr<IStorageServiceTask> &storageServiceTask) = 0;
    virtual bool RunIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask) = 0;
    virtual bool AbortIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask) = 0;
    virtual bool PersistFstab(std::string fstab) = 0;
    virtual bool IsEncrypted(std::string filePath) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"IStorageService");
};
} // namespace OHOS

#endif // SERVICES_SS_INCLUDE_STORAGE_SERVICE_BASE_H

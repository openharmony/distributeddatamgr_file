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
#include "fbe_crypto.h"
#include "interface/storage_service_callback_proxy.h"
#include "ipc/storage_service_task_proxy.h"
#include "storage_hilog.h"
#include "storage_service_stub.h"

namespace OHOS {
void RemoteReturnInt(MessageParcel &reply, int32_t errcode)
{
    reply.WriteInt32(errcode);
    if (errcode != ERR_NONE) {
        SSLOG_E("remote error code =%{public}d ", errcode);
    }
}

int32_t StorageServiceStub::OnRemoteRequest(uint32_t code,
                                            MessageParcel &data,
                                            MessageParcel &reply,
                                            MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    int32_t errCode = ERR_NONE;
    SSLOG_I("StorageServiceStub::OnRemoteRequest code : %{public}d", code);
    switch (code) {
        case SS_DOMOUNT: errCode = StorageServiceStub::HandleMount(data, reply); break;
        case SS_UNMOUNT: errCode = StorageServiceStub::HandleUnMount(data, reply); break;
        case SS_FORMAT: errCode = HandleFormat(data, reply); break;
        case SS_REGISTERCALLBACK: errCode = HandleRegisterStorageServiceCallback(data, reply); break;
        case SS_PARTITION: errCode = HandlePartition(data, reply); break;
        case SS_RESET: errCode = HandleReset(reply); break;
        case SS_USERADDED: errCode = HandleUserAdded(data, reply); break;
        case SS_USERREMOVED: errCode = HandleUserRemoved(data, reply); break;
        case SS_USERSTARTED: errCode = HandleUserStarted(data, reply); break;
        case SS_USERSTOPED: errCode = HandleUserStoped(data, reply); break;
        case SS_MOVESTORAGE: errCode = HandleMoveStorage(data, reply); break;
        case SS_BENCHMARK: errCode = HandleBenchMark(data, reply); break;
        case SS_CRYPTO_ENABLE: errCode = HandleCryptoEnable(reply); break;
        case SS_CRYPTO_INITIALIZE: errCode = HandleCryptoInitialize(reply); break;
        case SS_CRYPTO_CREATEKEY: errCode = HandleCryptoCreateKey(data, reply); break;
        case SS_CRYPTO_DELETEKEY: errCode = HandleCryptoDeleteKey(data, reply); break;
        case SS_CRYPTO_ADDAUTHKEY: errCode = HandleCryptoAddAuthKey(data, reply); break;
        case SS_CRYPTO_DELAUTHKEY: errCode = HandleCryptoDelAuthKey(data, reply); break;
        case SS_CRYPTO_UNLOCKKEY: errCode = HandleCryptoUnlockKey(data, reply); break;
        case SS_CRYPTO_LOCKKEY: errCode = HandleCryptoLockKey(data, reply); break;
        case SS_CRYPTO_INITUSERSPACE: errCode = HandleCryptoInitUserSpace(data, reply); break;
        case SS_CRYPTO_REMOVEUSERSPACE: errCode = HandleCryptoRemoveUserSpace(data, reply); break;
        case SS_CRYPTO_UPDATEAUTHKEY: errCode = HandleCryptoUpdateAuthKey(data, reply); break;
        case SS_IDLEFSTRIM: errCode = HandleIdleFsTrim(reply); break;
        case SS_RUNIDLEMAINTAIN: errCode = HandleRunIdleMaintain(reply); break;
        case SS_ABORTIDLEMAINTAIN: errCode = HandleAbortIdleMaintain(reply); break;
        case SS_PERSISTFSTAB: errCode = HandlePersistFstab(data, reply); break;
        case SS_ISENCRYPTED: errCode = HandleIsEncrypted(data, reply); break;
        default: {
            SSLOG_I("StorageServiceStub::OnRemoteRequest default");
            errCode = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }

    return errCode;
}
int32_t StorageServiceStub::HandleMount(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("StorageServiceStub::OnRemoteRequest SS_DOMount");
    bool ret = false;
    std::string volId = data.ReadString();
    int32_t mountFlags = data.ReadInt32();
    int32_t mountUserId = data.ReadInt32();
    SSLOG_I("StorageServiceStub::OnRemoteRequest: %{public}s  MountFlags  %{public}d  MountUserId  "
            "%{public}d",
            volId.c_str(), mountFlags, mountUserId);
    ret = Mount(volId, mountFlags, mountUserId);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleUnMount(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("StorageServiceStub::OnRemoteRequest SS_UNMOUNT");
    bool ret = false;
    std::string volId = data.ReadString();
    SSLOG_I("StorageServiceStub::OnRemoteRequest: %{public}s", volId.c_str());
    ret = UnMount(volId);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleFormat(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("StorageServiceStub::OnRemoteRequest SS_FORMAT");
    std::string volId = data.ReadString();
    std::string fsType = data.ReadString();
    SSLOG_I("StorageServiceStub::OnRemoteRequest:volId is  %{public}s,fsType is %{public}s", volId.c_str(),
            fsType.c_str());
    bool ret = false;
    ret = Format(volId, fsType);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandlePersistFstab(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("StorageServiceStub::OnRemoteRequest SS_PERSISTFSTAB");
    bool ret = false;
    std::string fstab = data.ReadString();
    SSLOG_I("StorageServiceStub::OnRemoteRequest: fstab::%{public}s ", fstab.c_str());
    ret = PersistFstab(fstab);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleIsEncrypted(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("StorageServiceStub::OnRemoteRequest SS_ISENCRYPTED");
    bool ret = false;
    std::string filePath = data.ReadString();
    SSLOG_I("StorageServiceStub::OnRemoteRequest: SS_ISENCRYPTED::%{public}s ", filePath.c_str());
    ret = IsEncrypted(filePath);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleRegisterStorageServiceCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> object = data.ReadRemoteObject();
    if (object != nullptr) {
        SSLOG_E("remote object is not null");
    }
    sptr<IStorageServiceCallback> storageServiceCallback = iface_cast<IStorageServiceCallback>(object);

    bool ret = false;
    if (!storageServiceCallback) {
        SSLOG_E("Get IStorageServiceCallback failed");
        return TRANSACTION_ERR;
    } else {
        ret = RegisterStorageServiceCallback(storageServiceCallback);
    }
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandlePartition(MessageParcel &data, MessageParcel &reply)
{
    bool ret = false;
    std::string diskId = data.ReadString();
    int32_t partitionType = data.ReadInt32();
    int32_t ratio = data.ReadInt32();
    ret = Partition(diskId, partitionType, ratio);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleUserAdded(MessageParcel &data, MessageParcel &reply)
{
    bool ret = false;
    int32_t userId = data.ReadInt32();
    int32_t userSerial = data.ReadInt32();
    ret = UserAdded(userId, userSerial);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleUserRemoved(MessageParcel &data, MessageParcel &reply)
{
    bool ret = false;
    int32_t userId = data.ReadInt32();
    ret = UserRemoved(userId);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleUserStarted(MessageParcel &data, MessageParcel &reply)
{
    bool ret = false;
    int32_t userId = data.ReadInt32();
    SSLOG_E("__func%{public}s, userId is %{public}d", __func__, userId);
    ret = UserStarted(userId);
    SSLOG_E(" __func%{public}s, ret is %{public}d", __func__, ret);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleUserStoped(MessageParcel &data, MessageParcel &reply)
{
    bool ret = false;
    int32_t userId = data.ReadInt32();
    ret = UserStoped(userId);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleMoveStorage(MessageParcel &data, MessageParcel &reply)
{
    bool ret = false;
    std::string fromVolId = data.ReadString();
    std::string toVolId = data.ReadString();
    sptr<IRemoteObject> object = data.ReadRemoteObject();
    sptr<IStorageServiceTask> storageServiceTask = iface_cast<IStorageServiceTask>(object);
    ret = MoveStorage(fromVolId, toVolId, storageServiceTask);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleBenchMark(MessageParcel &data, MessageParcel &reply)
{
    bool ret = false;
    std::string volId = data.ReadString();
    sptr<IRemoteObject> object = data.ReadRemoteObject();
    sptr<IStorageServiceTask> storageServiceTask = iface_cast<IStorageServiceTask>(object);
    ret = BenchMark(volId, storageServiceTask);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleReset(MessageParcel &reply)
{
    SSLOG_E("HandleReset");
    bool ret = false;
    ret = Reset();
    SSLOG_E("HandleReset: %{public}d", ret);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleCryptoEnable(MessageParcel &reply)
{
    SSLOG_I("HandleFbeEnable STARTING....（From gby）");
    bool ret = false;
    ret = CryptoEnable();
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    SSLOG_I("HandleFbeEnable END....（From gby）");
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleCryptoInitialize(MessageParcel &reply)
{
    SSLOG_I("HandleInitUser0 STARTING....（From gby）");
    bool ret = false;
    ret = CryptoInitialize();
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    SSLOG_I("HandleInitUser0 END....（From gby）");
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleCryptoCreateKey(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("HandleInitUser0 STARTING....（From gby）");
    bool ret = false;
    int32_t userId = data.ReadInt32();
    ret = CryptoCreateKey(userId);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    SSLOG_I("HandleInitUser0 END....（From gby）");
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleCryptoDeleteKey(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("HandleInitUser0 STARTING....（From gby）");
    bool ret = false;
    int32_t userId = data.ReadInt32();
    ret = CryptoDeleteKey(userId);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    SSLOG_I("HandleInitUser0 END....（From gby）");
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleCryptoAddAuthKey(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("HandleInitUser0 STARTING....（From gby）");
    bool ret = false;
    int32_t userId = data.ReadInt32();
    const std::string token = data.ReadString();
    const std::string secret = data.ReadString();
    ret = CryptoAddAuthKey(userId, token, secret);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    SSLOG_I("HandleInitUser0 END....（From gby）");
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleCryptoDelAuthKey(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("HandleInitUser0 STARTING....（From gby）");
    bool ret = false;
    int32_t userId = data.ReadInt32();
    const std::string token = data.ReadString();
    const std::string secret = data.ReadString();
    ret = CryptoDelAuthKey(userId, token, secret);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    SSLOG_I("HandleInitUser0 END....（From gby）");
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleCryptoUnlockKey(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("HandleInitUser0 STARTING....（From gby）");
    bool ret = false;
    int32_t userId = data.ReadInt32();
    const std::string token = data.ReadString();
    const std::string secret = data.ReadString();
    ret = CryptoUnlockKey(userId, token, secret);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    SSLOG_I("HandleInitUser0 END....（From gby）");
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleCryptoLockKey(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("HandleInitUser0 STARTING....（From gby）");
    bool ret = false;
    int32_t userId = data.ReadInt32();
    ret = CryptoLockKey(userId);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    SSLOG_I("HandleInitUser0 END....（From gby）");
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleCryptoUpdateAuthKey(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("HandleInitUser0 STARTING....（From gby）");
    bool ret = false;
    int32_t userId = data.ReadInt32();
    const std::string token = data.ReadString();
    const std::string secret = data.ReadString();
    ret = CryptoUpdateAuthKey(userId, token, secret);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    SSLOG_I("HandleInitUser0 END....（From gby）");
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleCryptoInitUserSpace(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("HandleInitUser0 STARTING....（From gby）");
    bool ret = false;
    const std::string uuid = data.ReadString();
    int32_t userId = data.ReadInt32();
    int32_t flag = data.ReadInt32();
    ret = CryptoInitUserSpace(uuid, userId, flag);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    SSLOG_I("HandleInitUser0 END....（From gby）");
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleCryptoRemoveUserSpace(MessageParcel &data, MessageParcel &reply)
{
    SSLOG_I("HandleInitUser0 STARTING....（From gby）");
    bool ret = false;
    const std::string uuid = data.ReadString();
    int32_t userId = data.ReadInt32();
    int32_t flag = data.ReadInt32();
    ret = CryptoRemoveUserSpace(uuid, userId, flag);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    SSLOG_I("HandleInitUser0 END....（From gby）");
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleIdleFsTrim(MessageParcel &reply)
{
    bool ret = false;
    sptr<IStorageServiceTask> storageServiceTask;
    ret = IdleFsTrim(storageServiceTask);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleRunIdleMaintain(MessageParcel &reply)
{
    bool ret = false;
    sptr<IStorageServiceTask> storageServiceTask;
    ret = RunIdleMaintain(storageServiceTask);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

int32_t StorageServiceStub::HandleAbortIdleMaintain(MessageParcel &reply)
{
    bool ret = false;
    sptr<IStorageServiceTask> storageServiceTask;
    ret = AbortIdleMaintain(storageServiceTask);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}
} // namespace OHOS

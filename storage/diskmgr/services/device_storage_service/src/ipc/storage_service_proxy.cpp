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
#include "storage_service_proxy.h"
#include "storage_hilog.h"

namespace OHOS {
StorageServiceProxy::StorageServiceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IStorageService>(impl)
{}

StorageServiceProxy::~StorageServiceProxy() {}

bool StorageServiceProxy::Mount(std::string volId, int32_t mountFlags, int32_t mountUserId)
{
    SSLOG_I("StorageServiceProxy::Mount start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
        return false;
    }

    if (!in.WriteString(volId)) {
        SSLOG_E("fail to write parcel parameter volId");
        return false;
    }
    if (!in.WriteInt32(mountFlags)) {
        SSLOG_E("fail to write parcel parameter mountFlags ");
        return false;
    }
    if (!in.WriteInt32(mountUserId)) {
        SSLOG_E("fail to write parcel parameter mountUserId ");
        return false;
    }
    int error = Remote()->SendRequest(SS_DOMOUNT, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool StorageServiceProxy::UnMount(std::string volId)
{
    SSLOG_I("StorageServiceProxy::UnMount start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
        return false;
    }

    if (!in.WriteString(volId)) {
        SSLOG_E("fail to write parcel parameter volId");
        return false;
    }

    int error = Remote()->SendRequest(SS_UNMOUNT, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    SSLOG_I("StorageServiceProxy::UnMount end");
    return true;
}
bool StorageServiceProxy::IsEncrypted(std::string filePath)
{
    SSLOG_I("StorageServiceProxy::IsEncrypted start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
        return false;
    }

    if (!in.WriteString(filePath)) {
        SSLOG_E("fail to write parcel parameter volId");
        return false;
    }

    int error = Remote()->SendRequest(SS_ISENCRYPTED, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    SSLOG_I("StorageServiceProxy::IsEncrypted end");
    return true;
}
bool StorageServiceProxy::Format(std::string volId, const std::string &fsType)
{
    SSLOG_I("StorageServiceProxy::Format start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
        return false;
    }

    if (!in.WriteString(volId)) {
        SSLOG_E("fail to write parcel for parameter volId ");
        return false;
    }
    if (!in.WriteString(fsType)) {
        SSLOG_E("fail to write parcel for parameter fsType");
        return false;
    }

    int error = Remote()->SendRequest(SS_FORMAT, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    SSLOG_I("StorageServiceProxy::Format end");
    return true;
}

bool StorageServiceProxy::PersistFstab(std::string fstab)
{
    SSLOG_I("StorageServiceProxy::PersistFstab start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
        return false;
    }

    if (!in.WriteString(fstab)) {
        SSLOG_E("fail to write parcel parameter volId");
        return false;
    }

    int error = Remote()->SendRequest(SS_PERSISTFSTAB, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    SSLOG_I("StorageServiceProxy::PersistFstab end");
    return true;
}

bool StorageServiceProxy::Reset()
{
    SSLOG_I("StorageServiceProxy::Reset start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
        return false;
    }

    int error = Remote()->SendRequest(SS_RESET, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    SSLOG_I("StorageServiceProxy::Reset end");
    return true;
}

bool StorageServiceProxy::Partition(const std::string &diskId, int32_t partitionType, int32_t ratio)
{
    SSLOG_I("begin to Partition");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to Partition due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(diskId)) {
        SSLOG_E("fail to Partition, for write diskId failed");
        return false;
    }
    if (!data.WriteInt32(partitionType)) {
        SSLOG_E("fail to Partition, write parcel parameter partitionType ");
    }
    if (!data.WriteInt32(ratio)) {
        SSLOG_E("fail to Partition, write parcel parameter ratio ");
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_PARTITION);
        return false;
    }
    int32_t result = remote->SendRequest(SS_PARTITION, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result, SS_PARTITION);
        return false;
    }
    if (!reply.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool StorageServiceProxy::UserAdded(int32_t userId, int32_t userSerial)
{
    SSLOG_I("begin to UserAdded");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to UserAdded due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to UserAdded, for write userId failed");
        return false;
    }
    if (!data.WriteInt32(userSerial)) {
        SSLOG_E("fail to UserAdded, write userSerial failed");
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_USERADDED);
        return false;
    }
    int32_t result = remote->SendRequest(SS_USERADDED, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result, SS_USERADDED);
        return false;
    }
    if (!reply.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool StorageServiceProxy::UserRemoved(int32_t userId)
{
    SSLOG_I("begin to UserRemoved");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to UserRemoved due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to UserRemoved, for write userId failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_USERREMOVED);
        return false;
    }
    int32_t result = remote->SendRequest(SS_USERREMOVED, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result, SS_USERREMOVED);
        return false;
    }
    if (!reply.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool StorageServiceProxy::UserStarted(int32_t userId)
{
    SSLOG_I("begin to UserStarted");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to UserStarted due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to UserStarted, for write userId failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_USERSTARTED);
        return false;
    }
    int32_t result = remote->SendRequest(SS_USERSTARTED, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result, SS_USERSTARTED);
        return false;
    }
    if (!reply.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool StorageServiceProxy::UserStoped(int32_t userId)
{
    SSLOG_I("begin to UserStoped");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to UserStoped due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to UserStoped, for write userId failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_USERSTOPED);
        return false;
    }
    int32_t result = remote->SendRequest(SS_USERSTOPED, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result, SS_USERSTOPED);
        return false;
    }
    if (!reply.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool StorageServiceProxy::MoveStorage(std::string fromVolId,
                                      std::string toVolId,
                                      const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("begin to moveStorage");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to moveStorage due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(fromVolId)) {
        SSLOG_E("fail to moveStorage, for write fromVolId failed");
        return false;
    }
    if (!data.WriteString(toVolId)) {
        SSLOG_E("fail to moveStorage, for write toVolId failed");
        return false;
    }
    if (!data.WriteRemoteObject(storageServiceTask->AsObject().GetRefPtr())) {
        SSLOG_E("fail to moveStorage, for write storageServiceTask failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_MOVESTORAGE);
        return false;
    }
    int32_t result = remote->SendRequest(SS_MOVESTORAGE, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result, SS_MOVESTORAGE);
        return false;
    }
    if (!reply.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool StorageServiceProxy::BenchMark(const std::string &volId,
                                    const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("begin to BenchMark");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to BenchMark due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(volId)) {
        SSLOG_E("fail to BenchMark, for write volId failed");
        return false;
    }
    if (!data.WriteRemoteObject(storageServiceTask->AsObject().GetRefPtr())) {
        SSLOG_E("fail to BenchMark, for write storageServiceTask failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_BENCHMARK);
        return false;
    }
    int32_t result = remote->SendRequest(SS_BENCHMARK, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result, SS_BENCHMARK);
        return false;
    }
    if (!reply.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool StorageServiceProxy::RegisterStorageServiceCallback(
    const sptr<IStorageServiceCallback> &storageServiceCallback)
{
    SSLOG_I("begin to RegisterStorageServiceCallback");
    if (!storageServiceCallback) {
        SSLOG_E("fail to RegisterStorageServiceCallback");
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to RegisterStorageServiceCallback due to write InterfaceToken fail");
        return false;
    }

    if (!data.WriteRemoteObject(storageServiceCallback->AsObject().GetRefPtr())) {
        SSLOG_E("fail to RegisterStorageServiceCallback, for write parcel failed");
        return false;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_REGISTERCALLBACK);
        return false;
    }
    int32_t result = remote->SendRequest(SS_REGISTERCALLBACK, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                SS_REGISTERCALLBACK);
        return false;
    }
    return true;
}

bool StorageServiceProxy::CryptoEnable()
{
    SSLOG_I("StorageServiceProxy::CryptoEnable start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
        return false;
    }

    int error = Remote()->SendRequest(SS_CRYPTO_ENABLE, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    SSLOG_I("StorageServiceProxy::CryptoEnable end");
    return true;
}

bool StorageServiceProxy::CryptoInitialize()
{
    SSLOG_I("StorageServiceProxy::CryptoInitialize start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
        return false;
    }

    int error = Remote()->SendRequest(SS_CRYPTO_INITIALIZE, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    SSLOG_I("StorageServiceProxy::CryptoInitialize end");
    return true;
}

bool StorageServiceProxy::CryptoCreateKey(int32_t userId)
{
    SSLOG_I("begin to StorageServiceProxy::CryptoCreateKey");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to CryptoCreateKey due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to CryptoCreateKey, for write userId failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_CRYPTO_CREATEKEY);
        return false;
    }
    int32_t result = remote->SendRequest(SS_CRYPTO_CREATEKEY, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                SS_CRYPTO_CREATEKEY);
        return false;
    }
    return true;
}

bool StorageServiceProxy::CryptoDeleteKey(int32_t userId)
{
    SSLOG_I("begin to CryptoDeleteKey");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to CryptoDeleteKey due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to CryptoDeleteKey, for write userId failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_CRYPTO_DELETEKEY);
        return false;
    }
    int32_t result = remote->SendRequest(SS_CRYPTO_DELETEKEY, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                SS_CRYPTO_DELETEKEY);
        return false;
    }
    return true;
}

bool StorageServiceProxy::CryptoAddAuthKey(int32_t userId,
                                           const std::string &token,
                                           const std::string &secret)
{
    SSLOG_I("begin to CryptoAddAuthKey");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to CryptoAddAuthKey due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to CryptoAddAuthKey, for write userId failed");
        return false;
    }
    if (!data.WriteString(token)) {
        SSLOG_E("fail to CryptoAddAuthKey, for write token failed");
        return false;
    }
    if (!data.WriteString(secret)) {
        SSLOG_E("fail to CryptoAddAuthKey, for write secret failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_CRYPTO_ADDAUTHKEY);
        return false;
    }
    int32_t result = remote->SendRequest(SS_CRYPTO_ADDAUTHKEY, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                SS_CRYPTO_ADDAUTHKEY);
        return false;
    }
    return true;
}

bool StorageServiceProxy::CryptoDelAuthKey(int32_t userId,
                                           const std::string &token,
                                           const std::string &secret)
{
    SSLOG_I("begin to CryptoDelAuthKey");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to CryptoDelAuthKey due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to CryptoDelAuthKey, for write userId failed");
        return false;
    }
    if (!data.WriteString(token)) {
        SSLOG_E("fail to CryptoDelAuthKey, for write token failed");
        return false;
    }
    if (!data.WriteString(secret)) {
        SSLOG_E("fail to CryptoDelAuthKey, for write secret failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_CRYPTO_DELAUTHKEY);
    }
    int32_t result = remote->SendRequest(SS_CRYPTO_DELAUTHKEY, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                SS_CRYPTO_DELAUTHKEY);
        return false;
    }
    return true;
}

bool StorageServiceProxy::CryptoUnlockKey(int32_t userId, const std::string &token, const std::string &secret)
{
    SSLOG_I("begin to CryptoUnlockKey");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to CryptoUnlockKey due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to CryptoUnlockKey, for write userId failed");
        return false;
    }
    if (!data.WriteString(token)) {
        SSLOG_E("fail to CryptoUnlockKey, for write token failed");
        return false;
    }
    if (!data.WriteString(secret)) {
        SSLOG_E("fail to CryptoUnlockKey, for write secret failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_CRYPTO_UNLOCKKEY);
        return false;
    }
    int32_t result = remote->SendRequest(SS_CRYPTO_UNLOCKKEY, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                SS_CRYPTO_UNLOCKKEY);
        return false;
    }
    return true;
}

bool StorageServiceProxy::CryptoLockKey(int32_t userId)
{
    SSLOG_I("begin to CryptoLockKey");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to CryptoLockKey due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to CryptoLockKey, for write userId failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_CRYPTO_LOCKKEY);
        return false;
    }
    int32_t result = remote->SendRequest(SS_CRYPTO_LOCKKEY, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                SS_CRYPTO_LOCKKEY);
        return false;
    }
    return true;
}

bool StorageServiceProxy::CryptoUpdateAuthKey(int32_t userId,
                                              const std::string &token,
                                              const std::string secret)
{
    SSLOG_I("begin to CryptoUpdateAuthKey");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to CryptoUpdateAuthKey due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to CryptoUpdateAuthKey, for write userId failed");
        return false;
    }
    if (!data.WriteString(token)) {
        SSLOG_E("fail to CryptoUpdateAuthKey, for write token failed");
        return false;
    }
    if (!data.WriteString(secret)) {
        SSLOG_E("fail to CryptoUpdateAuthKey, for write secret failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_CRYPTO_UPDATEAUTHKEY);
        return false;
    }
    int32_t result = remote->SendRequest(SS_CRYPTO_UPDATEAUTHKEY, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                SS_CRYPTO_UPDATEAUTHKEY);
        return false;
    }
    return true;
}

bool StorageServiceProxy::CryptoInitUserSpace(const std::string &uuid, int32_t userId, int32_t flag)
{
    SSLOG_I("begin to CryptoCreatePaths");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to CryptoCreatePaths due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(uuid)) {
        SSLOG_E("fail to CryptoCreatePaths, for write volumeUuid failed");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to CryptoCreatePaths, for write userId failed");
        return false;
    }
    if (!data.WriteInt32(flag)) {
        SSLOG_E("fail to CryptoCreatePaths, for write flags failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_CRYPTO_INITUSERSPACE);
        return false;
    }
    int32_t result = remote->SendRequest(SS_CRYPTO_INITUSERSPACE, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                SS_CRYPTO_INITUSERSPACE);
        return false;
    }
    return true;
}

bool StorageServiceProxy::CryptoRemoveUserSpace(const std::string &uuid, int32_t userId, int32_t flag)
{
    SSLOG_I("begin to CryptoRemovePaths");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("fail to CryptoRemovePaths due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(uuid)) {
        SSLOG_E("fail to CryptoRemovePaths, for write volumeUuid failed");
        return false;
    }
    if (!data.WriteInt32(userId)) {
        SSLOG_E("fail to CryptoRemovePaths, for write userId failed");
        return false;
    }
    if (!data.WriteInt32(flag)) {
        SSLOG_E("fail to CryptoRemovePaths, for write flags failed");
        return false;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", SS_CRYPTO_REMOVEUSERSPACE);
        return false;
    }
    int32_t result = remote->SendRequest(SS_CRYPTO_REMOVEUSERSPACE, data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                SS_CRYPTO_REMOVEUSERSPACE);
        return false;
    }
    return true;
}

bool StorageServiceProxy::IdleFsTrim(const sptr<IStorageServiceTask> &storageServiceTask)
{
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
        return false;
    }
    int error = Remote()->SendRequest(SS_IDLEFSTRIM, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool StorageServiceProxy::RunIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask)
{
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
        return false;
    }
    int error = Remote()->SendRequest(SS_RUNIDLEMAINTAIN, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool StorageServiceProxy::AbortIdleMaintain(const sptr<IStorageServiceTask> &storageServiceTask)
{
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
        return false;
    }
    int error = Remote()->SendRequest(SS_ABORTIDLEMAINTAIN, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}
} // namespace OHOS

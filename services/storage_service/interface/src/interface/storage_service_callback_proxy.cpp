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
#include "interface/storage_service_callback_proxy.h"
#include "ipc_types.h"
#include "parcel.h"
#include "storage_hilog.h"
#include "string_ex.h"
namespace OHOS {
StorageServiceCallbackProxy::StorageServiceCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IStorageServiceCallback>(impl)
{}

StorageServiceCallbackProxy::~StorageServiceCallbackProxy() {}

void StorageServiceCallbackProxy::OnDiskCreated(const std::string &diskId, int32_t flags)
{
    SSLOG_I("OnDiskCreated diskid is %{public}s", diskId.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        SSLOG_E("StorageServiceCallbackProxy::%{public}s write descriptor failed!", __func__);
        return;
    }
    if (!data.WriteString16(Str8ToStr16(diskId))) {
        SSLOG_E("fail to call OnDiskCreated, for write diskId failed");
        return;
    }
    if (!data.WriteInt32(flags)) {
        SSLOG_E("fail to call OnDiskCreated, for write flags failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to call OnDiskCreated, for Remote() is nullptr");
        return;
    }
    int32_t ret = remote->SendRequest(static_cast<int32_t>(IStorageServiceCallback::Message::ON_DISK_CREATED),
                                      data, reply, option);
    if (ret != NO_ERROR) {
        SSLOG_W("fail to call OnDiskCreated, for transact is failed, error code is: %{public}d", ret);
    }
}

void StorageServiceCallbackProxy::OnDiskMetadataChanged(const std::string &diskId,
                                                        uint64_t sizeBytes,
                                                        const std::string &label,
                                                        const std::string &sysPath)
{
    SSLOG_I("OnDiskMetadataChanged diskid is %{public}s", diskId.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        SSLOG_E("StorageServiceCallbackProxy::%{public}s write descriptor failed!", __func__);
        return;
    }
    if (!data.WriteString16(Str8ToStr16(diskId))) {
        SSLOG_E("fail to call OnDiskMetadataChanged, for write diskId failed");
        return;
    }
    if (!data.WriteUint64(sizeBytes)) {
        SSLOG_E("fail to call OnDiskMetadataChanged, for write sizeBytes failed");
        return;
    }
    if (!data.WriteString16(Str8ToStr16(label))) {
        SSLOG_E("fail to call OnDiskMetadataChanged, for write label failed");
        return;
    }
    if (!data.WriteString16(Str8ToStr16(sysPath))) {
        SSLOG_E("fail to call OnDiskMetadataChanged, for write sysPath failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to call OnDiskMetadataChanged, for Remote() is nullptr");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<int32_t>(IStorageServiceCallback::Message::ON_DISK_METADATA_CHANGED),
                            data, reply, option);
    if (ret != NO_ERROR) {
        SSLOG_W("fail to call OnDiskMetadataChanged, for transact is failed, error code is: %{public}d", ret);
    }
}

void StorageServiceCallbackProxy::OnDiskDestroyed(const std::string &diskId)
{
    SSLOG_I("OnDiskDestroyed diskid is %{public}s", diskId.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        SSLOG_E("StorageServiceCallbackProxy::%{public}s write descriptor failed!", __func__);
        return;
    }
    if (!data.WriteString16(Str8ToStr16(diskId))) {
        SSLOG_E("fail to call OnDiskDestroyed, for write diskId failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to call OnDiskDestroyed, for Remote() is nullptr");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(IStorageServiceCallback::Message::ON_DISK_DESTORYED), data, reply, option);
    if (ret != NO_ERROR) {
        SSLOG_W("fail to call OnDiskDestroyed, for transact is failed, error code is: %{public}d", ret);
    }
}

void StorageServiceCallbackProxy::OnVolumeCreated(const std::string &volId,
                                                  int32_t type,
                                                  const std::string &diskId,
                                                  const std::string &partGuid)
{
    SSLOG_I("OnVolumeCreated volId is %{public}s", volId.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        SSLOG_E("StorageServiceCallbackProxy::%{public}s write descriptor failed!", __func__);
        return;
    }
    if (!data.WriteString16(Str8ToStr16(volId))) {
        SSLOG_E("fail to call OnVolumeCreated, for write volId failed");
        return;
    }
    if (!data.WriteInt32(type)) {
        SSLOG_E("fail to call OnVolumeCreated, for write type failed");
        return;
    }
    if (!data.WriteString16(Str8ToStr16(diskId))) {
        SSLOG_E("fail to call OnVolumeCreated, for write diskId failed");
        return;
    }
    if (!data.WriteString16(Str8ToStr16(partGuid))) {
        SSLOG_E("fail to call OnVolumeCreated, for write partGuid failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to call OnVolumeCreated, for Remote() is nullptr");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(IStorageServiceCallback::Message::ON_VOLUME_CREATED), data, reply, option);
    if (ret != NO_ERROR) {
        SSLOG_W("fail to call OnVolumeCreated, for transact is failed, error code is: %{public}d", ret);
    }
}

void StorageServiceCallbackProxy::OnVolumeStateChanged(const std::string &volId, int32_t state)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d volId=%{public}s", __FILE__, __func__, __LINE__,
            volId.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        SSLOG_E("StorageServiceCallbackProxy::%{public}s write descriptor failed!", __func__);
        return;
    }
    if (!data.WriteString16(Str8ToStr16(volId))) {
        SSLOG_E("fail to call OnVolumeStateChanged, for write volId failed");
        return;
    }
    if (!data.WriteInt32(state)) {
        SSLOG_E("fail to call OnVolumeStateChanged, for write state failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to call OnVolumeStateChanged, for Remote() is nullptr");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(IStorageServiceCallback::Message::ON_VOLUME_STATE_CHANGED), data, reply, option);
    if (ret != NO_ERROR) {
        SSLOG_W("fail to call OnVolumeStateChanged, for transact is failed, error code is: %{public}d", ret);
    }
}

void StorageServiceCallbackProxy::OnVolumeMetadataChanged(const std::string &volId,
                                                          const std::string &fsType,
                                                          const std::string &fsUuid,
                                                          const std::string &fsLabel)
{
    SSLOG_I("OnVolumeMetadataChanged volId is %{public}s", volId.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        SSLOG_E("StorageServiceCallbackProxy::%{public}s write descriptor failed!", __func__);
        return;
    }
    if (!data.WriteString16(Str8ToStr16(volId))) {
        SSLOG_E("fail to call OnVolumeMetadataChanged, for write volId failed");
        return;
    }
    if (!data.WriteString16(Str8ToStr16(fsType))) {
        SSLOG_E("fail to call OnVolumeMetadataChanged, for write fsType failed");
        return;
    }
    if (!data.WriteString16(Str8ToStr16(fsUuid))) {
        SSLOG_E("fail to call OnVolumeMetadataChanged, for write fsUuid failed");
        return;
    }
    if (!data.WriteString16(Str8ToStr16(fsLabel))) {
        SSLOG_E("fail to call OnVolumeMetadataChanged, for write fsLabel failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to call OnVolumeMetadataChanged, for Remote() is nullptr");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(IStorageServiceCallback::Message::ON_VOLUME_METADATA_CHANGED), data, reply,
        option);
    if (ret != NO_ERROR) {
        SSLOG_W("fail to call OnVolumeMetadataChanged, for transact is failed, error code is: %{public}d",
                ret);
    }
}

void StorageServiceCallbackProxy::OnVolumePathChanged(const std::string &volId, const std::string &path)
{
    SSLOG_I("OnVolumePathChanged volId is %{public}s", volId.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        SSLOG_E("StorageServiceCallbackProxy::%{public}s write descriptor failed!", __func__);
        return;
    }
    if (!data.WriteString16(Str8ToStr16(volId))) {
        SSLOG_E("fail to call OnVolumePathChanged, for write volId failed");
        return;
    }
    if (!data.WriteString16(Str8ToStr16(path))) {
        SSLOG_E("fail to call OnVolumePathChanged, for write path failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to call OnVolumePathChanged, for Remote() is nullptr");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(IStorageServiceCallback::Message::ON_VOLUME_PATH_CHANGED), data, reply, option);
    if (ret != NO_ERROR) {
        SSLOG_W("fail to call OnVolumePathChanged, for transact is failed, error code is: %{public}d", ret);
    }
}

void StorageServiceCallbackProxy::OnVolumeInternalPathChanged(const std::string &volId,
                                                              const std::string &internalPath)
{
    SSLOG_I("OnVolumeInternalPathChanged volId is %{public}s", volId.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        SSLOG_E("StorageServiceCallbackProxy::%{public}s write descriptor failed!", __func__);
        return;
    }
    if (!data.WriteString16(Str8ToStr16(volId))) {
        SSLOG_E("fail to call OnVolumeInternalPathChanged, for write volId failed");
        return;
    }
    if (!data.WriteString16(Str8ToStr16(internalPath))) {
        SSLOG_E("fail to call OnVolumeInternalPathChanged, for write internalPath failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to call OnVolumeInternalPathChanged, for Remote() is nullptr");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(IStorageServiceCallback::Message::ON_VOLUME_INTERNAL_PATH_CHANGED), data, reply,
        option);
    if (ret != NO_ERROR) {
        SSLOG_W("fail to call OnVolumeInternalPathChanged, for transact is failed, error code is: %{public}d",
                ret);
    }
}

void StorageServiceCallbackProxy::OnVolumeDestroyed(const std::string &volId)
{
    SSLOG_I("OnVolumeDestroyed volId is %{public}s", volId.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        SSLOG_E("StorageServiceCallbackProxy::%{public}s write descriptor failed!", __func__);
        return;
    }
    if (!data.WriteString16(Str8ToStr16(volId))) {
        SSLOG_E("fail to call OnVolumeDestroyed, for write diskId volId");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to call OnVolumeDestroyed, for Remote() is nullptr");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(IStorageServiceCallback::Message::ON_VOLUME_DESTORYED), data, reply, option);
    if (ret != NO_ERROR) {
        SSLOG_W("fail to call OnDiskDestroyed, for transact is failed, error code is: %{public}d", ret);
    }
}
} // namespace OHOS
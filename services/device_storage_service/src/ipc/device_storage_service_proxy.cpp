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
#include "ipc/device_storage_service_proxy.h"
#include "device_storage_service_hilog.h"
namespace OHOS {
DeviceStorageServiceProxy::DeviceStorageServiceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IDeviceStorageService>(impl)
{}

DeviceStorageServiceProxy::~DeviceStorageServiceProxy() {}

bool DeviceStorageServiceProxy::Mount(const std::string &volId)
{
    SSLOG_I("DeviceStorageServiceProxy::Mount start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }

    if (!in.WriteString(volId)) {
        SSLOG_E("fail to write parcel");
    }

    int error = Remote()->SendRequest(DSS_DOMOUNT, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}
bool DeviceStorageServiceProxy::UnMount(const std::string &volId)
{
    SSLOG_I("DeviceStorageServiceProxy::UnMount start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }

    if (!in.WriteString(volId)) {
        SSLOG_E("fail to write parcel");
    }

    int error = Remote()->SendRequest(DSS_UNMOUNT, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    SSLOG_I("DeviceStorageServiceProxy::UnMount end");
    return true;
}

bool DeviceStorageServiceProxy::Format(const std::string &volId)
{
    SSLOG_I("DeviceStorageServiceProxy::Format start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }

    if (!in.WriteString(volId)) {
        SSLOG_E("fail to write parcel");
    }

    int error = Remote()->SendRequest(DSS_FORMAT, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    SSLOG_I("DeviceStorageServiceProxy::Format end");
    return true;
}
bool DeviceStorageServiceProxy::IsEncrypted(const std::string &filePath)
{
    SSLOG_I("DeviceStorageServiceProxy::IsEncrypted start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }

    if (!in.WriteString(filePath)) {
        SSLOG_E("fail to write parcel");
    }

    int error = Remote()->SendRequest(DSS_ISENCRYPTED, in, out, option);
    if (error != ERR_NONE) {
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    SSLOG_I("DeviceStorageServiceProxy::IsEncrypted end");
    return true;
}

int DeviceStorageServiceProxy::Init()
{
    SSLOG_I("DeviceStorageServiceProxy::Init start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }

    int error = Remote()->SendRequest(DSS_INIT, in, out, option);
    if (error != ERR_NONE) {
        SSLOG_E("DeviceStorageServiceProxy::Init Error code %d", error);
        return error;
    }
    return error;
}

bool DeviceStorageServiceProxy::PartitionPublic(const std::string &diskId)
{
    SSLOG_I("DeviceStorageServiceProxy::PartitionPublic start");

    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }
    if (!in.WriteString(diskId)) {
        SSLOG_E("fail to write parcel diskId");
    }
    int32_t result = Remote()->SendRequest(DSS_PARTITIONPUBLIC, in, out, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                DSS_PARTITIONPUBLIC);
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool DeviceStorageServiceProxy::PartitionPrivate(const std::string &diskId)
{
    SSLOG_I("DeviceStorageServiceProxy::PartitionPrivate start");

    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }
    if (!in.WriteString(diskId)) {
        SSLOG_E("fail to write parcel diskId");
    }
    int32_t result = Remote()->SendRequest(DSS_PARTITIONPRIVATE, in, out, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                DSS_PARTITIONPRIVATE);
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool DeviceStorageServiceProxy::SetPrimaryStorageUuid(const std::string &volumeUuid)
{
    SSLOG_I("DeviceStorageServiceProxy::SetPrimaryStorageUuid start");

    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }
    if (!in.WriteString(volumeUuid)) {
        SSLOG_E("fail to write parcel volumeUuid");
    }
    int32_t result = Remote()->SendRequest(DSS_SETPRIMARYSTORAGE, in, out, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                DSS_SETPRIMARYSTORAGE);
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool DeviceStorageServiceProxy::BenchMark(const std::string &volId,
                                          const sptr<IStorageServiceTask> &storageServiceTask)
{
    SSLOG_I("DeviceStorageServiceProxy::BenchMark start");

    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }
    if (!in.WriteString(volId)) {
        SSLOG_E("fail to BenchMark, for write volId failed");
        return false;
    }
    if (!in.WriteRemoteObject(storageServiceTask->AsObject().GetRefPtr())) {
        SSLOG_E("fail to volId, for write storageServiceTask failed");
        return false;
    }
    int32_t result = Remote()->SendRequest(DSS_BENCHMARK, in, out, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result, DSS_BENCHMARK);
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    return true;
}

bool DeviceStorageServiceProxy::GetPrimaryStorageUuid(std::string &primaryUuid)
{
    SSLOG_I("DeviceStorageServiceProxy::GetPrimaryStorageUuid start");

    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }
    int32_t result = Remote()->SendRequest(DSS_GETPRIMARYUUID, in, out, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                DSS_GETPRIMARYUUID);
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    primaryUuid = out.ReadString();
    return true;
}

bool DeviceStorageServiceProxy::FindPrivateForEmulate(std::shared_ptr<DS::VolumeInfo> &priVol,
                                                      std::shared_ptr<DS::VolumeInfo> &emuVol)
{
    SSLOG_I("DeviceStorageServiceProxy::FindPrivateForEmulate start");

    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }
    if (!in.WriteParcelable(emuVol.get())) {
        SSLOG_E("fail to FindPrivateForEmulate, for write emuVol failed");
        return false;
    }
    int32_t result = Remote()->SendRequest(DSS_FINDPRIVATEFOREMULATE, in, out, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                DSS_FINDPRIVATEFOREMULATE);
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    std::shared_ptr<DS::VolumeInfo> info(out.ReadParcelable<DS::VolumeInfo>());
    if (!info) {
        SSLOG_E("readParcelableInfo failed");
        return false;
    }
    priVol = info;
    SSLOG_D("get parcelable info success");
    return true;
}

bool DeviceStorageServiceProxy::FindEmulateForPrivate(std::shared_ptr<DS::VolumeInfo> &emuVol,
                                                      std::shared_ptr<DS::VolumeInfo> &priVol)
{
    SSLOG_I("DeviceStorageServiceProxy::FindEmulateForPrivate start");

    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }
    if (!in.WriteParcelable(priVol.get())) {
        SSLOG_E("fail to FindEmulateForPrivate, for write priVol failed");
        return false;
    }
    int32_t result = Remote()->SendRequest(DSS_FINDEMULATEFORPRIVATE, in, out, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result,
                DSS_FINDEMULATEFORPRIVATE);
        return false;
    }
    if (!out.ReadBool()) {
        SSLOG_E("reply result false");
        return false;
    }
    std::shared_ptr<DS::VolumeInfo> info(out.ReadParcelable<DS::VolumeInfo>());
    if (!info) {
        SSLOG_E("readParcelableInfo failed");
        return false;
    }
    emuVol = info;
    SSLOG_D("get parcelable info success");
    return true;
}

bool DeviceStorageServiceProxy::GetVolumes(std::vector<std::shared_ptr<DS::VolumeInfo>> &volumeInfos)
{
    SSLOG_I("DeviceStorageServiceProxy::GetVolumes start");

    if (!GetParcelableInfos<DS::VolumeInfo>(DSS_GETVOLUMES, volumeInfos)) {
        SSLOG_E("fail to GetVolumeInfos from server or volumeInfos is empty");
        return false;
    }
    return true;
}

bool DeviceStorageServiceProxy::GetDisks(std::vector<std::shared_ptr<DS::DiskInfo>> &diskInfos)
{
    SSLOG_I("DeviceStorageServiceProxy::GetDisks start");

    if (!GetParcelableInfos<DS::DiskInfo>(DSS_GETDISKS, diskInfos)) {
        SSLOG_E("fail to GetdiskInfos from server or diskInfos is empty");
        return false;
    }
    return true;
}

template<typename T>
bool DeviceStorageServiceProxy::GetParcelableInfos(uint32_t code,
                                                   std::vector<std::shared_ptr<T>> &parcelableInfos)
{
    MessageParcel reply;
    if (!SendTransactCmd(code, reply)) {
        return false;
    }

    if (!reply.ReadBool()) {
        SSLOG_E("readParcelableInfo failed");
        return false;
    }

    int32_t infoSize = reply.ReadInt32();
    SSLOG_I("DeviceStorageServiceProxy::info size is %{public}d", infoSize);

    for (int32_t i = 0; i < infoSize; i++) {
        std::shared_ptr<T> info(reply.ReadParcelable<T>());
        if (!info) {
            SSLOG_E("Read Parcelable infos failed");
            return false;
        }
        parcelableInfos.emplace_back(info);
    }
    SSLOG_D("get parcelable infos success");
    return true;
}

bool DeviceStorageServiceProxy::SendTransactCmd(uint32_t code, MessageParcel &reply)
{
    MessageOption option(MessageOption::TF_SYNC);

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        SSLOG_E("fail to send transact cmd %{public}d due to remote object", code);
        return false;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SSLOG_E("write interface token failed");
    }
    int32_t result = remote->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (result != NO_ERROR) {
        SSLOG_E("receive error transact code %{public}d in transact cmd %{public}d", result, code);
        return false;
    }
    return true;
}
} // namespace OHOS

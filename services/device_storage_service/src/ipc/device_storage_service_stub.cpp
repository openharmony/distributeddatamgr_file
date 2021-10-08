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

#include "ipc/device_storage_service_stub.h"

#include "device_storage_service_hilog.h"
#include "device_storage_service_errors.h"
#include "ipc/storage_service_task_proxy.h"

namespace OHOS {
int32_t DeviceStorageServiceStub::OnRemoteRequest(uint32_t code,
                                                  MessageParcel &data,
                                                  MessageParcel &reply,
                                                  MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    int32_t ret = 0;
    switch (code) {
        case DSS_DOMOUNT: {
            ret = DeviceStorageServiceStub::HandleMount(data.ReadString(), reply);
            break;
        }
        case DSS_INIT: ret = DeviceStorageServiceStub::HandleInit(); break;
        case DSS_UNMOUNT: ret = DeviceStorageServiceStub::HandleUnMount(data.ReadString(), reply); break;
        case DSS_FORMAT: ret = DeviceStorageServiceStub::HandleFormat(data.ReadString(), reply); break;
        case DSS_GETVOLUMES: ret = DeviceStorageServiceStub::HandlegetVolumes(reply); break;
        case DSS_PARTITIONPUBLIC:
            ret = DeviceStorageServiceStub::HandlePartitionPublic(data.ReadString(), reply);
            break;
        case DSS_PARTITIONPRIVATE:
            ret = DeviceStorageServiceStub::HandlePartitionPrivate(data.ReadString(), reply);
            break;
        case DSS_SETPRIMARYSTORAGE:
            ret = DeviceStorageServiceStub::HandleSetPrimaryStorage(data.ReadString(), reply);
            break;
        case DSS_BENCHMARK: ret = DeviceStorageServiceStub::HandleBenchMark(data, reply); break;
        case DSS_GETDISKS: ret = DeviceStorageServiceStub::HandlegetDisks(reply); break;
        case DSS_GETPRIMARYUUID: ret = DeviceStorageServiceStub::HandleGetPrimaryUuid(reply); break;
        case DSS_FINDPRIVATEFOREMULATE:
            ret = DeviceStorageServiceStub::HandleFindPrivateForEmulate(data, reply);
            break;
        case DSS_FINDEMULATEFORPRIVATE:
            ret = DeviceStorageServiceStub::HandleFindEmulateForPrivate(data, reply);
            break;
        case DSS_ISENCRYPTED:
            ret = DeviceStorageServiceStub::HandleIsEncrypted(data.ReadString(), reply);
            break;
        default: {
            SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest default");
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

int32_t DeviceStorageServiceStub::HandleMount(const std::string &volId, Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_DOMOUNT");
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest: %{public}s", volId.c_str());
    bool ret = Mount(volId);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_DOMOUNT End");
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandleUnMount(const std::string &volId, Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_UNMOUNT");
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest: %{public}s", volId.c_str());
    bool ret = UnMount(volId);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_UNMOUNT End");
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandleFormat(const std::string &volId, Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_FORMAT");
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest: %{public}s", volId.c_str());
    bool ret = Format(volId);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_FORMAT End");
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandleIsEncrypted(const std::string &filePath, Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_ISENCRYPTED");
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest: %{public}s", filePath.c_str());
    bool ret = IsEncrypted(filePath);
    if (!reply.WriteBool(ret)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_ISENCRYPTED End");
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandleInit()
{
    int ret = 0;
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_INIT");
    ret = Init();
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_INIT End");
    return ret;
}

int32_t DeviceStorageServiceStub::HandlePartitionPrivate(const std::string &diskId, Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_PARTITIONPRIVATE");
    bool partionPrivateRet = PartitionPrivate(diskId);
    if (!reply.WriteBool(partionPrivateRet)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_PARTITIONPRIVATE End");
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandlePartitionPublic(const std::string &diskId, Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequestDSS_PARTITIONPUBLIC");
    bool partionPublicRet = PartitionPublic(diskId);
    if (!reply.WriteBool(partionPublicRet)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_PARTITIONPUBLIC End");
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandleSetPrimaryStorage(const std::string &volumeUuid, Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest  DSS_SetPrimaryStorage");
    bool setPrimaryStorage = SetPrimaryStorageUuid(volumeUuid);
    if (!reply.WriteBool(setPrimaryStorage)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_SetPrimaryStorage End");
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandleGetPrimaryUuid(Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest  DSS_GETPRIMARYUUID");
    std::string volumeUuid;
    bool getPrimaryUuid = GetPrimaryStorageUuid(volumeUuid);
    if (!reply.WriteBool(getPrimaryUuid)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    if (getPrimaryUuid) {
        if (!reply.WriteString(volumeUuid)) {
            SSLOG_E("write failed");
            return E_WRITE_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandleFindPrivateForEmulate(MessageParcel &data, Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest  DSS_FINDPRIVATEFOREMULATE");
    std::shared_ptr<DS::VolumeInfo> info(data.ReadParcelable<DS::VolumeInfo>());
    std::shared_ptr<DS::VolumeInfo> priVol;
    bool find = FindPrivateForEmulate(priVol, info);
    if (!reply.WriteBool(find)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    if (find) {
        if (!reply.WriteParcelable((priVol.get()))) {
            SSLOG_E("write failed");
            return E_WRITE_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandleFindEmulateForPrivate(MessageParcel &data, Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest  DSS_FINDEMULATEFORPRIVATE");
    std::shared_ptr<DS::VolumeInfo> info(data.ReadParcelable<DS::VolumeInfo>());
    std::shared_ptr<DS::VolumeInfo> emuVol;
    bool find = FindEmulateForPrivate(emuVol, info);
    if (!reply.WriteBool(find)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    if (find) {
        if (!reply.WriteParcelable((emuVol.get()))) {
            SSLOG_E("write failed");
            return E_WRITE_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandleBenchMark(MessageParcel &data, Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest  DSS_HANDLEBENCHMARK");
    std::string volId = data.ReadString();
    sptr<IRemoteObject> object = data.ReadRemoteObject();
    sptr<IStorageServiceTask> storageServiceTask = iface_cast<IStorageServiceTask>(object);
    bool bench = BenchMark(volId, storageServiceTask);
    if (!reply.WriteBool(bench)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_HANDLEBENCHMARK End");
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandlegetVolumes(Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_GETVOLUMES");
    std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
    bool getVolumeRet = GetVolumes(infos);
    if (!reply.WriteBool(getVolumeRet)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    SSLOG_I("DeviceStorageServiceStub::getvolumeInfos size is %{public}d", infos.size());
    if (getVolumeRet) {
        if (!WriteParcelableVector(infos, reply)) {
            SSLOG_E("write failed");
            return E_WRITE_PARCEL_ERROR;
        }
    }
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_GETVOLUMES End");
    return ERR_OK;
}

int32_t DeviceStorageServiceStub::HandlegetDisks(Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_GETDISKS");
    std::vector<std::shared_ptr<DS::DiskInfo>> infos;
    bool getDiskRet = GetDisks(infos);
    if (!reply.WriteBool(getDiskRet)) {
        SSLOG_E("write failed");
        return E_WRITE_PARCEL_ERROR;
    }
    if (getDiskRet) {
        if (!WriteParcelableVector(infos, reply)) {
            SSLOG_E("write failed");
            return E_WRITE_PARCEL_ERROR;
        }
    }
    SSLOG_I("DeviceStorageServiceStub::OnRemoteRequest DSS_GETDISKS End");
    return ERR_OK;
}
template<typename T>
bool DeviceStorageServiceStub::WriteParcelableVector(std::vector<std::shared_ptr<T>> &parcelableVector,
                                                     Parcel &reply)
{
    SSLOG_I("DeviceStorageServiceStub::WriteParcelableVector size is %{public}d", parcelableVector.size());
    if (!reply.WriteInt32(parcelableVector.size())) {
        SSLOG_E("write ParcelableVector failed");
        return false;
    }
    for (auto &parcelable : parcelableVector) {
        if (!reply.WriteParcelable(parcelable.get())) {
            SSLOG_E("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}
} // namespace OHOS

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

#include "interface/storage_service_callback_stub.h"
#include "ipc_types.h"
#include "storage_hilog.h"
#include "string_ex.h"

namespace OHOS {
int32_t StorageServiceCallbackStub::OnRemoteRequest(uint32_t code,
                                                    MessageParcel &data,
                                                    MessageParcel &reply,
                                                    MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    SSLOG_D("storage service callback Stub onReceived message, the message code is %{public}u", code);
    switch (code) {
        case static_cast<uint32_t>(IStorageServiceCallback::Message::ON_DISK_CREATED): {
            std::string diskId = Str16ToStr8(data.ReadString16());
            int32_t flag = data.ReadInt32();
            OnDiskCreated(diskId, flag);
            SSLOG_I("Destroy storage service callback Stub instance");
            break;
        }
        case static_cast<uint32_t>(IStorageServiceCallback::Message::ON_DISK_METADATA_CHANGED): {
            std::string diskId = Str16ToStr8(data.ReadString16());
            uint64_t sizeBytes = data.ReadUint64();
            std::string label = Str16ToStr8(data.ReadString16());
            std::string sysPath = Str16ToStr8(data.ReadString16());
            OnDiskMetadataChanged(diskId, sizeBytes, label, sysPath);
            SSLOG_I("Destroy storage service callback Stub instance");
            break;
        }
        case static_cast<uint32_t>(IStorageServiceCallback::Message::ON_DISK_DESTORYED): {
            std::string diskId = Str16ToStr8(data.ReadString16());
            OnDiskDestroyed(diskId);
            SSLOG_I("Destroy storage service callback Stub instance");
            break;
        }
        case static_cast<uint32_t>(IStorageServiceCallback::Message::ON_VOLUME_CREATED): {
            std::string volId = Str16ToStr8(data.ReadString16());
            int32_t type = data.ReadInt32();
            std::string diskId = Str16ToStr8(data.ReadString16());
            std::string partGuid = Str16ToStr8(data.ReadString16());
            OnVolumeCreated(volId, type, diskId, partGuid);
            SSLOG_I("Destroy storage service callback Stub instance");
            break;
        }
        case static_cast<uint32_t>(IStorageServiceCallback::Message::ON_VOLUME_STATE_CHANGED): {
            std::string volId = Str16ToStr8(data.ReadString16());
            int32_t state = data.ReadInt32();
            OnVolumeStateChanged(volId, state);
            SSLOG_I("Destroy storage service callback Stub instance");
            break;
        }
        case static_cast<uint32_t>(IStorageServiceCallback::Message::ON_VOLUME_METADATA_CHANGED): {
            std::string volId = Str16ToStr8(data.ReadString16());
            std::string fsType = Str16ToStr8(data.ReadString16());
            std::string fsUuid = Str16ToStr8(data.ReadString16());
            std::string fsLabel = Str16ToStr8(data.ReadString16());
            OnVolumeMetadataChanged(volId, fsType, fsUuid, fsLabel);
            SSLOG_I("Destroy storage service callback Stub instance");
            break;
        }
        case static_cast<uint32_t>(IStorageServiceCallback::Message::ON_VOLUME_PATH_CHANGED): {
            std::string volId = Str16ToStr8(data.ReadString16());
            std::string path = Str16ToStr8(data.ReadString16());
            OnVolumePathChanged(volId, path);
            SSLOG_I("Destroy storage service callback Stub instance");
            break;
        }
        case static_cast<uint32_t>(IStorageServiceCallback::Message::ON_VOLUME_INTERNAL_PATH_CHANGED): {
            std::string volId = Str16ToStr8(data.ReadString16());
            std::string internalPath = Str16ToStr8(data.ReadString16());
            OnVolumeInternalPathChanged(volId, internalPath);
            SSLOG_I("Destroy storage service callback Stub instance");
            break;
        }
        case static_cast<uint32_t>(IStorageServiceCallback::Message::ON_VOLUME_DESTORYED): {
            std::string volId = Str16ToStr8(data.ReadString16());
            OnVolumeDestroyed(volId);
            SSLOG_I("Destroy storage service callback Stub instance");
            break;
        }
        default: return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NO_ERROR;
}
} // namespace OHOS
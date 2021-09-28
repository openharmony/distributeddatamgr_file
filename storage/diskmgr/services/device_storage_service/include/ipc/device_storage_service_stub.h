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

#ifndef STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_STUB_H
#define STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_STUB_H

#include "ipc/idevice_storage_service.h"

namespace OHOS {
class DeviceStorageServiceStub : public IRemoteStub<IDeviceStorageService> {
public:
    virtual int32_t
    OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t HandleInit();
    int32_t HandleMount(const std::string &volId, Parcel &reply);
    int32_t HandleUnMount(const std::string &volId, Parcel &reply);
    int32_t HandleFormat(const std::string &volId, Parcel &reply);
    int32_t HandlePartitionPrivate(const std::string &diskId, Parcel &reply);
    int32_t HandlePartitionPublic(const std::string &diskId, Parcel &reply);
    int32_t HandleSetPrimaryStorage(const std::string &volumeUuid, Parcel &reply);
    int32_t HandleBenchMark(MessageParcel &data, Parcel &reply);
    int32_t HandleGetPrimaryUuid(Parcel &reply);
    int32_t HandleFindPrivateForEmulate(MessageParcel &data, Parcel &reply);
    int32_t HandleFindEmulateForPrivate(MessageParcel &data, Parcel &reply);
    int32_t HandlegetVolumes(Parcel &reply);
    int32_t HandlegetDisks(Parcel &reply);
    int32_t HandleIsEncrypted(const std::string &filePath,Parcel &reply);
    template<typename T>
    bool WriteParcelableVector(std::vector<std::shared_ptr<T>> &parcelableVector, Parcel &reply);
};
} // namespace OHOS

#endif // STORAGE_DISKMGR_SERVICES_DEVICE_STORAGE_SERVICE_STUB_H
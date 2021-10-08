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

#ifndef SERVICES_STORAGE_SERVICE_INCLUDE_CALLBACK_H
#define SERVICES_STORAGE_SERVICE_INCLUDE_CALLBACK_H

#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
class IStorageServiceCallback : public IRemoteBroker {
public:
    enum class Message {
        ON_DISK_CREATED,
        ON_DISK_METADATA_CHANGED,
        ON_DISK_DESTORYED,
        ON_VOLUME_CREATED,
        ON_VOLUME_STATE_CHANGED,
        ON_VOLUME_METADATA_CHANGED,
        ON_VOLUME_PATH_CHANGED,
        ON_VOLUME_INTERNAL_PATH_CHANGED,
        ON_VOLUME_DESTORYED,
    };

    virtual void OnDiskCreated(const std::string &diskId, int32_t flags) = 0;
    virtual void OnDiskMetadataChanged(const std::string &diskId,
                                       uint64_t sizeBytes,
                                       const std::string &label,
                                       const std::string &sysPath) = 0;
    virtual void OnDiskDestroyed(const std::string &diskId) = 0;
    virtual void OnVolumeCreated(const std::string &volId,
                                 int32_t type,
                                 const std::string &diskId,
                                 const std::string &partGuid) = 0;
    virtual void OnVolumeStateChanged(const std::string &volId, int32_t state) = 0;
    virtual void OnVolumeMetadataChanged(const std::string &volId,
                                         const std::string &fsType,
                                         const std::string &fsUuid,
                                         const std::string &fsLabel) = 0;
    virtual void OnVolumePathChanged(const std::string &volId, const std::string &path) = 0;
    virtual void OnVolumeInternalPathChanged(const std::string &volId, const std::string &internalPath) = 0;
    virtual void OnVolumeDestroyed(const std::string &volId) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"IStorageServiceCallback");
};
} // namespace OHOS

#endif // SERVICES_STORAGE_SERVICE_INCLUDE_CALLBACK_H
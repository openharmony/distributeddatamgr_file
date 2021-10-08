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

#ifndef SERVICES_INCLUDE_STORAGE_SERVICE_CALLBACK_PROXY_H
#define SERVICES_INCLUDE_STORAGE_SERVICE_CALLBACK_PROXY_H

#include <nocopyable.h>

#include "storage_service_callback_interface.h"

namespace OHOS {
class StorageServiceCallbackProxy : public IRemoteProxy<IStorageServiceCallback> {
public:
    StorageServiceCallbackProxy(const sptr<IRemoteObject> &impl);
    ~StorageServiceCallbackProxy();
    DISALLOW_COPY_AND_MOVE(StorageServiceCallbackProxy);
    virtual void OnDiskCreated(const std::string &diskId, int32_t flags) override;
    virtual void OnDiskMetadataChanged(const std::string &diskId,
                                       uint64_t sizeBytes,
                                       const std::string &label,
                                       const std::string &sysPath) override;
    virtual void OnDiskDestroyed(const std::string &diskId) override;
    virtual void OnVolumeCreated(const std::string &volId,
                                 int32_t type,
                                 const std::string &diskId,
                                 const std::string &partGuid) override;
    virtual void OnVolumeStateChanged(const std::string &volId, int32_t state) override;
    virtual void OnVolumeMetadataChanged(const std::string &volId,
                                         const std::string &fsType,
                                         const std::string &fsUuid,
                                         const std::string &fsLabel) override;
    virtual void OnVolumePathChanged(const std::string &volId, const std::string &path) override;
    virtual void OnVolumeInternalPathChanged(const std::string &volId,
                                             const std::string &internalPath) override;
    virtual void OnVolumeDestroyed(const std::string &volId) override;

private:
    static inline BrokerDelegator<StorageServiceCallbackProxy> delegator_;
};
} // namespace OHOS

#endif // SERVICES_INCLUDE_STORAGE_SERVICE_CALLBACK_PROXY_H
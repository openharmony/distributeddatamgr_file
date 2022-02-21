/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef STORAGE_DISKMGR_INTERFACE_KITS_NAPI_DSM_DEVICE_SM_EXPORTER_H
#define STORAGE_DISKMGR_INTERFACE_KITS_NAPI_DSM_DEVICE_SM_EXPORTER_H

#pragma once

#include "../common/napi/n_exporter.h"

namespace OHOS {
namespace DistributedFS {
namespace ModuleDSMExpoter {
class DeviceSMExporter final : public NExporter {
public:
    inline static const std::string className_ = "DeviceSMgr";
    static napi_value Mount(napi_env env, napi_callback_info info);
    static napi_value UnMount(napi_env env, napi_callback_info info);
    static napi_value Format(napi_env env, napi_callback_info info);
    static napi_value PartitionPublic(napi_env env, napi_callback_info info);
    static napi_value PartitionPrivate(napi_env env, napi_callback_info info);
    static napi_value GetVolumes(napi_env env, napi_callback_info info);
    static napi_value GetDisks(napi_env env, napi_callback_info info);
    static napi_value SetPrimaryStorageUuid(napi_env env, napi_callback_info info);
    static napi_value FindVolumeById(napi_env env, napi_callback_info info);
    static napi_value FindVolumeByUuid(napi_env env, napi_callback_info info);
    static napi_value FindDiskById(napi_env env, napi_callback_info info);
    static napi_value GetPrimaryStorageUuid(napi_env env, napi_callback_info info);
    static napi_value FindPrivateForEmulate(napi_env env, napi_callback_info info);
    static napi_value FindEmulateForPrivate(napi_env env, napi_callback_info info);
    static napi_value GetWritableVolumes(napi_env env, napi_callback_info info);
    static napi_value GetBestVolumeDescription(napi_env env, napi_callback_info info);
    static napi_value IsEncrypted(napi_env env, napi_callback_info info);
    bool Export() override;

    std::string GetClassName() override;

    DeviceSMExporter(napi_env env, napi_value exports);
    ~DeviceSMExporter() override;
};
} // namespace ModuleDSMExpoter
} // namespace DistributedFS
} // namespace OHOS

#endif // STORAGE_DISKMGR_INTERFACE_KITS_NAPI_DSM_DEVICE_SM_EXPORTER_H
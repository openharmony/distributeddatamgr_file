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
#ifndef STORAGE_DISKMGR_INTERFACE_KITS_NAPI_DSM_FILE_PICKER_EXPORTER_H
#define STORAGE_DISKMGR_INTERFACE_KITS_NAPI_DSM_FILE_PICKER_EXPORTER_H

#pragma once

#include "ability.h"

#include "../common/napi/n_exporter.h"
#include "file_info.h"
#include "root_info.h"
#include "storage_ability.h"

namespace OHOS {
namespace DistributedFS {
namespace ModuleFPExpoter {
struct MkdirAsyncInfo {
    napi_async_work asyncWork = nullptr;
    napi_deferred defer = nullptr;
    napi_ref callback[3] = { nullptr };
    std::string path = "";
    int err = 0;
};

struct SaveFileAsyncInfo {
    napi_async_work asyncWork = nullptr;
    napi_deferred defer = nullptr;
    napi_ref callback[3] = { nullptr };
    std::string dstPath = "";
    size_t srcLen = 0;
    int err = 0;
    std::vector<std::string> srcPath;
    std::vector<FileManager::SaveFileInfo> saveFileList;
};

struct SearchFileAsyncInfo {
    napi_async_work asyncWork = nullptr;
    napi_deferred defer = nullptr;
    napi_ref callback[3] = { nullptr };
    std::string path = "";
    std::string name = "";
    int err = 0;
    std::vector<FileManager::FileInfo> fileInfoList;
};

struct ListAsyncInfo {
    napi_async_work asyncWork = nullptr;
    napi_deferred defer = nullptr;
    napi_ref callback[3] = { nullptr };
    std::string path = "";
    int err = 0;
    std::vector<FileManager::FileInfo> fileInfoList;
};

struct GetDeviceAsyncInfo {
    napi_async_work asyncWork = nullptr;
    napi_deferred defer = nullptr;
    napi_ref callback[3] = { nullptr };
    int err = 0;
    std::vector<FileManager::RootInfo> deviceInfoList;
};

class FilePickerExporter final : public NExporter {
public:
    inline static const std::string className_ = "FilePicker";
    static napi_value Mkdir(napi_env env, napi_callback_info info);
    static napi_value SaveFile(napi_env env, napi_callback_info info);
    static napi_value SearchFile(napi_env env, napi_callback_info info);
    static napi_value List(napi_env env, napi_callback_info info);
    static napi_value GetDeviceInfo(napi_env env, napi_callback_info info);

    bool Export() override;

    std::string GetClassName() override;

    FilePickerExporter(napi_env env, napi_value exports);
    ~FilePickerExporter() override;
};
} // namespace ModuleFMSExpoter
} // namespace DistributedFS
} // namespace OHOS

#endif // STORAGE_DISKMGR_INTERFACE_KITS_NAPI_DSM_FILE_PICKER_EXPORTER_H
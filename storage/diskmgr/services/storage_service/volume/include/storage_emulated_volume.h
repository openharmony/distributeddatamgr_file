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
#ifndef STORAGE_EMULATED_VOLUME_H
#define STORAGE_EMULATED_VOLUME_H

#include "storage_base.h"

namespace OHOS {
namespace StorageService {
class StorageEmulatedVolume : public StorageBase {
public:
    explicit StorageEmulatedVolume(const std::string &rawPath, int userId);
    StorageEmulatedVolume(const std::string &rawPath, dev_t device, const std::string &fsUuid, int userId);
    virtual ~StorageEmulatedVolume();

protected:
    int DoMount() override;
    int DoUnMount() override;

private:
    std::string emulatedVolRawPath;
    std::string emulatedVolLabel;
    std::string emulatedVolSdcardFsDefault;
    std::string emulatedVolSdcardFsRead;
    std::string emulatedVolSdcardFsWrite;
    std::string emulatedVolSdcardFsFull;

    bool emulatedVolUseSdcardFs;
    bool emulatedVolAppDataIsolationEnabled;

    bool PreareDirCheck();
    int ExeclFile(std::string label, dev_t before);
    void ForceAllUnMount();
    void RmdirAll();
    void ClearAll();
    void Init(std::string label);
};
} // namespace StorageService
} // namespace OHOS

#endif // STORAGE_EMULATED_VOLUME_H

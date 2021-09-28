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
#ifndef STORAGEPUBLICVOLUME_H
#define STORAGEPUBLICVOLUME_H

#include <map>
#include <string>
#include <iostream>
#include "block_device.h"
#include "storage_base.h"
#include "utils_file.h"
namespace OHOS {
namespace StorageService {
class StoragePublicVolume : public StorageBase {
public:
    explicit StoragePublicVolume(dev_t device);
    virtual ~StoragePublicVolume();

    enum publicVolType {
        publicVolNotDefined,
        publicVolAUTO,
        publicVolEXFAT,
        publicVolEXT4,
        publicVolF2FS,
        publicVolNTFS,
        publicVolVFAT
    };

    std::map<std::string, publicVolType> publicVolMapStringValues;

protected:
    int DoCreate() override;
    int DoDestroy() override;
    int DoMount() override;
    int DoUnMount() override;
    int DoFormat(const std::string &fsType) override;
    int InitAsecStage();

private:
    dev_t publicVolDevice;

    std::string publicVolFsType;
    std::string publicVolFsUuid;
    std::string publicVolFsLabel;
    std::string publicVolDevPath;
    std::string publicVolRawPath;
    std::string publicVolSdcardFsRead;
    std::string publicVolSdcardFsWrite;
    std::string publicVolSdcardFsFull;
    std::string publicVolSdcardFsDefault;
    sptr<SsBlockDevice> publicBlkDev;
    bool publicVolUseSdcardFs;
    void InitMapStringValues();
    void ForceUnmountAll();
    void ClearAll();
    void RmdirAll();
    void MountInit(std::string stableName);
    bool DoMountVisible();

    DISALLOW_COPY_AND_ASSIGN(StoragePublicVolume);
};
} // namespace StorageService
} // namespace OHOS

#endif // STORAGEPUBLICVOLUME_H

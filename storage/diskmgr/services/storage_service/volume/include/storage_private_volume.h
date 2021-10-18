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
#ifndef STORAGE_PRIVATE_VOLUME_H
#define STORAGE_PRIVATE_VOLUME_H

#include "storage_base.h"
#include "utils_file.h"
#include "block_device.h"
namespace OHOS {
namespace StorageService {
class StoragePrivateVolume : public StorageBase {
public:
    StoragePrivateVolume(dev_t device, const std::string &keyraw);
    virtual ~StoragePrivateVolume();
    enum privateVolType {
        privateVolNotDefined,
        privateVolAUTO,
        privatepublicVolEXFAT,
        privateVolEXT4,
        privateVolF2FS,
        privateVolNTFS,
        privateVolVFAT
    };

    std::map<std::string, privateVolType> privateVolMapStringValues;
    const std::string &GetFsType() const
    {
        return mFsType;
    };
    const std::string &GetRawDevPath() const
    {
        return mRawDevPath;
    };
    const std::string &GetRawDmDevPath() const
    {
        return mDmDevPath;
    };
    const std::string &GetFsUuid() const
    {
        return mFsUuid;
    };
    dev_t GetRawDevice() const
    {
        return mRawDevice;
    };

protected:
    int DoCreate() override;
    int DoDestroy() override;
    int DoMount() override;
    void DoPostMount() override;
    int DoUnMount() override;
    int DoFormat(const std::string &fsType) override;

private:
    dev_t mRawDevice;
    std::string mRawDevPath;
    std::string mDmDevPath;
    std::string mPath;
    std::string mFsType;
    std::string mFsUuid;
    std::string mFsLabel;
    std::string mKeyRaw;
    sptr<SsBlockDevice> privateBlkDev;
    bool PrepareAllDir();
    DISALLOW_COPY_AND_ASSIGN(StoragePrivateVolume);
};
} // namespace StorageService
} // namespace OHOS

#endif // STORAGE_PRIVATE_VOLUME_H

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
#ifndef _STORAGE_DM_CRYPT_H_
#define _STORAGE_DM_CRYPT_H_

#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <cstring>
#include <memory>
#include <vector>
#include <stddef.h>
#include <utility>
#include <algorithm>

#define MAX_CRYPT_TYPE_LEN 64
#define MAX_CRYPT_KEY_SIZE 64

namespace OHOS {
namespace StorageService {
namespace DmCrypt {
struct DmCryptInfo {
    uint64_t fsSize;
    int keySize;
    char cryptType[MAX_CRYPT_TYPE_LEN];
    std::string rawKey;
};
bool GetDmCryptVersion(int fd, const std::string &name, int *version);
int CreateExtVolume(const std::string &name,
                    const std::string &blkdev,
                    const std::string &rawkey,
                    std::string &outblkdev);
int RemoveExtVolume(const std::string &name);
void InitDmIoctl(struct dm_ioctl *io, size_t dataSize, const std::string &name, uint32_t flags);
int CreateCryptoDev(const struct DmCryptInfo &cryptinfo,
                    const std::string &blkdev,
                    std::string &outblkdev,
                    const std::string &name);
int LoadCryptTable(const struct DmCryptInfo &cryptinfo,
                   const std::string &blkdev,
                   const std::string &name,
                   int fd,
                   const std::string &extra_params);

void KeyToHexStr(const std::string &rawkey, std::string &keystr);
} // namespace DmCrypt
} // namespace StorageService
} // namespace OHOS
#endif // _STORAGE_DM_CRYPT_H
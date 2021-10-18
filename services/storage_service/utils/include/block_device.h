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
#ifndef STORAGE_SERVICE_BLOCK_DEVICE_H_
#define STORAGE_SERVICE_BLOCK_DEVICE_H_
#include <sys/types.h>
#include <refbase.h>
#include <unistd.h>
#include <stdint.h>
#include <string>
#include <stddef.h>
#include <linux/fiemap.h>

namespace OHOS {
class SsBlockDevice : public RefBase {
public:
    SsBlockDevice(const std::string &path);
    SsBlockDevice(const std::string &path, const dev_t &dev);
    ~SsBlockDevice();
    uint64_t GetNrSectors();
    bool CreateNode();
    bool DestroyNode();
    uint64_t GetSize();
    dev_t GetDevice();
    static dev_t GetDevice(const std::string &path);
    int Open(int flags);
    bool Wipe();
    bool ToBlockDevice(const std::string &path);
    static bool IsVirtioBlk(unsigned int majorId);
    static bool IsScsiDev(unsigned int majorId);
    static bool IsVirtualDev(unsigned int majorId);
    static bool IsMmcDev(unsigned int majorId);
    static bool IsNvmeDev(unsigned int majorId, const std::string &path);
    std::string GetNvmeLabel(const std::string &path);
    std::string GetMmcDevLabel(const std::string &path);
    std::string GetVirtualDevLabel();
    std::string GetScsiDevLabel(const std::string &path);
    std::string GetVirtioDevLabel();
    static uint32_t GetVirtioBlk();
    bool WipePath(const std::string &path);
    bool WriteZero(uint64_t start, uint64_t length);
    std::unique_ptr<struct fiemap> FiemapAllocate();
    std::unique_ptr<struct fiemap> GetDirFiemap(const std::string &path);

private:
    int devFd;
    std::string devPath;
    uint64_t nrSectors;
    dev_t device;
    uint64_t devSize;
    static const std::string procDevices;
};
} // namespace OHOS

#endif // STORAGE_SERVICE_BLOCK_DEVICE_H_
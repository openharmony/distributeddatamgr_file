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
#include "block_device.h"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <mntent.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/fs.h>

#include "storage_hilog.h"
#include "utils_file.h"
#include "utils_string.h"
using namespace OHOS::SsUtils;
namespace OHOS {
const std::string SsBlockDevice::procDevices = "/proc/devices";
constexpr int MAX_FM_EXTENTS = 32;
constexpr uint64_t MAX_ERASE_BUFF_SIZE = 4096;
static const unsigned int majorBlockLoop = 7;
static const unsigned int majorBlockScsiA = 8;
static const unsigned int majorBlockScsiB = 65;
static const unsigned int majorBlockScsiC = 66;
static const unsigned int majorBlockScsiD = 67;
static const unsigned int majorBlockScsiE = 68;
static const unsigned int majorBlockScsiF = 69;
static const unsigned int majorBlockScsiG = 70;
static const unsigned int majorBlockScsiH = 71;
static const unsigned int majorBlockScsiI = 128;
static const unsigned int majorBlockScsiJ = 129;
static const unsigned int majorBlockScsiK = 130;
static const unsigned int majorBlockScsiL = 131;
static const unsigned int majorBlockScsiM = 132;
static const unsigned int majorBlockScsiN = 133;
static const unsigned int majorBlockScsiO = 134;
static const unsigned int majorBlockScsiP = 135;
static const unsigned int majorBlockMmc = 179;
static const unsigned int majorBlockDynamicMin = 234;
static const unsigned int majorBlockDynamicMax = 512;

SsBlockDevice::SsBlockDevice(const std::string &path)
    : devPath(path)
{
    nrSectors = 0;
    devSize = 0;
    device = GetDevice(path);
    devFd = -1;
}
SsBlockDevice::SsBlockDevice(const std::string &path, const dev_t &dev)
    : devPath(path)
    , device(dev)
{
    nrSectors = 0;
    devSize = 0;
    devFd = -1;
}
SsBlockDevice::~SsBlockDevice()
{
    if (devFd >= 0) {
        close(devFd);
    }
}

bool SsBlockDevice::IsScsiDev(unsigned int majorId)
{
    return (majorId == majorBlockScsiA || majorId == majorBlockScsiB || majorId == majorBlockScsiC ||
            majorId == majorBlockScsiD || majorId == majorBlockScsiE || majorId == majorBlockScsiF ||
            majorId == majorBlockScsiG || majorId == majorBlockScsiH || majorId == majorBlockScsiI ||
            majorId == majorBlockScsiJ || majorId == majorBlockScsiK || majorId == majorBlockScsiL ||
            majorId == majorBlockScsiM || majorId == majorBlockScsiN || majorId == majorBlockScsiO ||
            majorId == majorBlockScsiP);
}

bool SsBlockDevice::IsVirtualDev(unsigned int majorId)
{
    return (majorId == majorBlockLoop);
}
bool SsBlockDevice::IsMmcDev(unsigned int majorId)
{
    return (majorId == majorBlockMmc);
}
bool SsBlockDevice::IsNvmeDev(unsigned int majorId, const std::string &path)
{
    return ((path.find("nvme") != std::string::npos) && (majorId >= majorBlockDynamicMin) &&
            (majorId <= majorBlockDynamicMax));
}
std::string SsBlockDevice::GetNvmeLabel(const std::string &path)
{
    std::string model(path + "/device/model");
    std::string label("");
    ReadFile(model, &label);
    return label;
}
std::string SsBlockDevice::GetMmcDevLabel(const std::string &path)
{
    std::string mmcPath(path + "/device/manfid");
    std::string label("");
    std::string idStr("");
    if (!ReadFile(mmcPath, &idStr)) {
        return label;
    }
    int64_t manfid = GetPid(Trim(idStr).c_str());
    if (manfid == -1) {
        return label;
    }
    switch (manfid) {
        case 0x000003: label = "SanDisk"; break;
        case 0x00001b: label = "Samsung"; break;
        case 0x000028: label = "Lexar"; break;
        case 0x000074: label = "Transcend"; break;
        default: label = "Other"; break;
    }
    return label;
}
std::string SsBlockDevice::GetVirtualDevLabel()
{
    std::string label("Virtual");
    return label;
}
std::string SsBlockDevice::GetScsiDevLabel(const std::string &path)
{
    std::string vendor(path + "/device/vendor");
    std::string label("");
    if (!ReadFile(vendor, &label)) {
        return label;
    }
    return Trim(label);
}
std::string SsBlockDevice::GetVirtioDevLabel()
{
    std::string label("Virtual");
    return label;
}
int SsBlockDevice::Open(int flags)
{
    if (devFd >= 0) {
        close(devFd);
        devFd = -1;
    }
    return (TEMP_FAILURE_RETRY(open(devPath.c_str(), flags)));
}

uint64_t SsBlockDevice::GetNrSectors()
{
    if (nrSectors != 0) {
        return nrSectors;
    }
    uint64_t nrSec64 = 0;
    uint32_t nrSec32 = 0;
    devFd = Open(O_RDONLY | O_CLOEXEC);
    if (devFd < 0) {
        SSLOGFE("Open Blk Dev %s failed ", devPath.c_str());
        return 0;
    }
    if (ioctl(devFd, BLKGETSIZE64, &nrSec64) == 0) {
        SSLOGFI("Get BLK64 Size Success");
        nrSec64 = nrSec64 >> 9;
        nrSectors = nrSec64;
        return nrSectors;
    }
    if (ioctl(devFd, BLKGETSIZE, &nrSec32) == 0) {
        SSLOGFI("Get BLK Sectors Size Success");
        nrSectors = static_cast<uint64_t>(nrSec32);
    }
    return nrSectors;
}
bool SsBlockDevice::CreateNode()
{
    if (device == 0) {
        device = GetDevice();
    }
    if (mknod(devPath.c_str(), 0660 | S_IFBLK, device) < 0) {
        if (errno != EEXIST) {
            SSLOGFE("Failed to Create device node for %d:%d ast %s", major(device), minor(device),
                    devPath.c_str());
            return false;
        }
    }
    return true;
}

uint64_t SsBlockDevice::GetSize()
{
    if (devSize > 0) {
        return devSize;
    }
    devFd = Open(O_RDONLY | O_CLOEXEC);
    if (devFd < 0) {
        SSLOGFE("Open Blk Dev %s failed ", devPath.c_str());
        devSize = 0;
        return devSize;
    }
    if (ioctl(devFd, BLKGETSIZE64, &devSize) == 0) {
        SSLOGFE("Get BLK Size 64 success");
        return devSize;
    }
    uint32_t nrSec = 0;
    devSize = 0;
    if (ioctl(devFd, BLKGETSIZE, &nrSec) == 0) {
        devSize = static_cast<uint64_t>(nrSec);
        devSize = devSize << 9;
    }
    return devSize;
}
dev_t SsBlockDevice::GetDevice(const std::string &path)
{
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0) {
        return sb.st_dev;
    } else {
        return 0;
    }
}
dev_t SsBlockDevice::GetDevice()
{
    return device;
}

bool SsBlockDevice::DestroyNode()
{
    if (TEMP_FAILURE_RETRY(unlink(devPath.c_str())) != 0) {
        return false;
    }
    return true;
}
bool SsBlockDevice::Wipe()
{
    uint64_t range[2] = { 0, 0 };
    int devFd = Open(O_RDWR | O_CLOEXEC);
    if (devFd == -1) {
        SSLOGFE("Failed to open %s", devPath.c_str());
        return false;
    }
    range[1] = GetSize();
    SSLOGFI("About to discard %llu on %s", range[1], devPath.c_str());
    if (ioctl(devFd, BLKDISCARD, &range) == 0) {
        SSLOGFI("Discard success on %s", devPath.c_str());
        return true;
    }
    return false;
}

bool SsBlockDevice::WriteZero(uint64_t start, uint64_t length)
{
    if (devFd == -1) {
        return false;
    }
    if (lseek64(devFd, start, SEEK_SET) != static_cast<loff_t>(start)) {
        SSLOGFE("Seek failed for zero overwrite");
        return false;
    }
    char buf[MAX_ERASE_BUFF_SIZE];
    memset(buf, 0, MAX_ERASE_BUFF_SIZE);
    while (length > 0) {
        size_t wlen = static_cast<size_t>(std::min(MAX_ERASE_BUFF_SIZE, length));
        auto n = write(devFd, buf, wlen);
        if (n < 1) {
            SSLOGFE("Write of zeroes failed");
            return false;
        }
        length -= n;
    }
    return true;
}
bool SsBlockDevice::WipePath(const std::string &path)
{
    auto fiemap = GetDirFiemap(path);
    if (!fiemap) {
        return false;
    }
    if (!ToBlockDevice(path)) {
        return false;
    }
    int devFd = Open(O_RDWR | O_CLOEXEC);
    if (devFd == -1) {
        SSLOGFE("Failed to open %s", devPath.c_str());
        return false;
    }
    int count = fiemap->fm_mapped_extents;
    for (int i = 0; i < count; i++) {
        uint64_t range[2] = { 0, 0 };
        range[0] = fiemap->fm_extents[i].fe_physical;
        range[1] = fiemap->fm_extents[i].fe_length;
        if (ioctl(devFd, BLKDISCARD, &range) == 0) {
            SSLOGFI("Discard success on %s", devPath.c_str());
            continue;
        } else {
            if (!WriteZero(range[0], range[1])) {
                return false;
            }
        }
    }
    return true;
}
bool SsBlockDevice::ToBlockDevice(const std::string &path)
{
    FILE *mntFp = setmntent("/proc/mounts", "re");
    if (mntFp == nullptr) {
        SSLOGFE("Unable to open /proc/mounts");
        return false;
    }
    std::string matchPath;
    size_t maxDirLen = 0;
    struct mntent *mnt = nullptr;
    while ((mnt = getmntent(mntFp)) != nullptr) {
        auto dirLen = strlen(mnt->mnt_dir);
        if ((dirLen > maxDirLen) && (path.size() > dirLen) && (path[dirLen] == '/') &&
            (path.compare(0, dirLen, mnt->mnt_dir) == 0)) {
            matchPath = mnt->mnt_fsname;
            maxDirLen = dirLen;
        }
    }
    endmntent(mntFp);
    if (matchPath.empty()) {
        SSLOGFE("Didn't find a mountpoint to match path ");
        return false;
    }
    devPath = matchPath;
    return true;
}
uint32_t SsBlockDevice::GetVirtioBlk()
{
    uint32_t major = 0;
    std::string devInfo;
    if (!ReadFile(procDevices, &devInfo)) {
        SSLOGFE("Unable to open /proc/devices");
        return 0;
    }
    bool isBlkDev = false;
    for (auto item : Split(devInfo, "\n")) {
        if (item == "Block devices:") {
            isBlkDev = true;
            continue;
        } else if (item == "Character devices:") {
            isBlkDev = false;
            continue;
        } else {
            if (isBlkDev) {
                auto infos = Split(item, " ");
                if ((infos.size() == 2) && (infos[1] == "virtblk")) {
                    major = std::stoul(infos[0]);
                    break;
                }
            }
        }
    }
    return major;
}

bool SsBlockDevice::IsVirtioBlk(unsigned int major)
{
    unsigned int majorBlk = GetVirtioBlk();
    return (majorBlk && major) == majorBlk;
}

std::unique_ptr<struct fiemap> SsBlockDevice::FiemapAllocate()
{
    size_t allocsize = offsetof(struct fiemap, fm_extents[MAX_FM_EXTENTS]);
    std::unique_ptr<struct fiemap> res(new (::operator new(allocsize)) struct fiemap);
    memset(res.get(), 0, allocsize);
    res->fm_start = 0;
    res->fm_length = UINT64_MAX;
    res->fm_flags = 0;
    res->fm_extent_count = MAX_FM_EXTENTS;
    res->fm_mapped_extents = 0;
    return res;
}
std::unique_ptr<struct fiemap> SsBlockDevice::GetDirFiemap(const std::string &path)
{
    int fd = TEMP_FAILURE_RETRY(open(path.c_str(), O_RDONLY | O_CLOEXEC, 0));
    if (fd == -1) {
        SSLOGFE("Unable to open %s", path.c_str());
        return nullptr;
    }
    auto fiemap = FiemapAllocate();
    if (ioctl(fd, FS_IOC_FIEMAP, fiemap.get()) != 0) {
        SSLOGFE("Unable to FIEMAP %s", path.c_str());
        close(fd);
        return nullptr;
    }
    close(fd);
    auto count = fiemap->fm_mapped_extents;
    if (count < 1 || count > MAX_FM_EXTENTS) {
        SSLOGFE("Extent count not in bounds 1 <= %d <=   in %s", fiemap->fm_mapped_extents, path.c_str());
        return nullptr;
    }
    for (uint32_t i = 0; i < count; i++) {
        if (fiemap->fm_extents[i].fe_flags &
            (FIEMAP_EXTENT_UNKNOWN | FIEMAP_EXTENT_DELALLOC | FIEMAP_EXTENT_NOT_ALIGNED)) {
            SSLOGFE("Extent has unexpected flags ");
            return nullptr;
        }
    }
    return fiemap;
}
} // namespace OHOS
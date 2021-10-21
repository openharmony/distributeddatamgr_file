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
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/dm-ioctl.h>

#include "storage_utils.h"
#include "storage_hilog.h"
#include "storage_dm_crypt.h"
#include "utils_file.h"
#include "utils_string.h"
#include "block_device.h"

using namespace OHOS::SsUtils;
namespace OHOS {
namespace StorageService {
namespace DmCrypt {
static int numberTwo = 2;
static int numberFour = 4;
static int numberSeven = 7;
constexpr int DM_BUFF_SIZE = 4096;
constexpr int DM_SLEEP_SIZE = 500000;
void InitDmIoctl(struct dm_ioctl *io, size_t dataSize, const std::string &name, unsigned flags)
{
    memset(io, 0, dataSize);
    io->data_size = dataSize;
    io->data_start = sizeof(struct dm_ioctl);
    io->version[0] = numberFour;
    io->version[1] = 0;
    io->version[numberTwo] = 0;
    io->flags = flags;
    if (!name.empty()) {
        strlcpy(io->name, name.c_str(), sizeof(io->name));
    }
}

int RemoveExtVolume(const std::string &name)
{
    int fd;
    char buffer[DM_BUFF_SIZE];
    struct dm_ioctl *io;

    if ((fd = open("/dev/device-mapper", O_RDWR | O_CLOEXEC)) < 0) {
        SSLOGFE("Cannot open device-mapper\n");
        return -1;
    }

    io = (struct dm_ioctl *)buffer;

    InitDmIoctl(io, DM_BUFF_SIZE, name, 0);
    if (ioctl(fd, DM_DEV_REMOVE, io)) {
        SSLOGFE("Cannot remove dm-crypt device\n");
        close(fd);
        return -1;
    }
    close(fd); /* If fd is <0 from a failed open call, it's safe to just ignore the close error */
    return 0;
}

int CreateExtVolume(const std::string &name,
                    const std::string &blkdev,
                    const std::string &rawkey,
                    std::string &outblkdev)
{
    uint64_t nrSec = 0;
    SsBlockDevice *blkDev = new SsBlockDevice(blkdev);
    nrSec = blkDev->GetSize();

    if (nrSec == 0) {
        SSLOGFE("Failed to get size of %s: %s", blkdev.c_str(), strerror(errno));
        return -1;
    }
    struct DmCryptInfo cryptInfo;
    cryptInfo.fsSize = nrSec;
    cryptInfo.keySize = rawkey.size();
    strlcpy(cryptInfo.cryptType, "aes-cbc-essiv:sha256", MAX_CRYPT_TYPE_LEN);
    cryptInfo.rawKey = rawkey;
    return CreateCryptoDev(cryptInfo, blkdev, outblkdev, name);
}

bool GetDmCryptVersion(int fd, const std::string &name, int *version)
{
    char buffer[DM_BUFF_SIZE];
    struct dm_ioctl *io;
    struct dm_target_versions *v;

    io = (struct dm_ioctl *)buffer;

    InitDmIoctl(io, DM_BUFF_SIZE, name, 0);

    if (ioctl(fd, DM_LIST_VERSIONS, io)) {
        return false;
    }

    /* Iterate over the returned versions, looking for name of "crypt".
     * When found, get and return the version.
     */
    v = (struct dm_target_versions *)&buffer[sizeof(struct dm_ioctl)];
    while (v->next) {
        if (!strcmp(v->name, "crypt")) {
            /* We found the crypt driver, return the version, and get out */
            version[0] = v->version[0];
            version[1] = v->version[1];
            version[numberTwo] = v->version[numberTwo];
            return true;
        }
        v = (struct dm_target_versions *)(((char *)v) + v->next);
    }

    return false;
}

int CreateCryptoDev(const struct DmCryptInfo &cryptinfo,
                    const std::string &blkdev,
                    std::string &outblkdev,
                    const std::string &name)
{
    char buffer[DM_BUFF_SIZE];
    struct dm_ioctl *io;
    unsigned int minor;
    int fd = 0;
    int version[3];
    std::string extraParams;
    int loadCount;

    if ((fd = open("/dev/device-mapper", O_RDWR | O_CLOEXEC)) < 0) {
        return -1;
    }
    io = (struct dm_ioctl *)buffer;

    InitDmIoctl(io, DM_BUFF_SIZE, name, 0);
    if (ioctl(fd, DM_DEV_CREATE, io)) {
        close(fd);
        return -1;
    }
    InitDmIoctl(io, DM_BUFF_SIZE, name, 0);
    if (ioctl(fd, DM_DEV_STATUS, io)) {
        close(fd);
        return -1;
    }
    minor = (io->dev & 0xff) | ((io->dev >> 12) & 0xfff00);
    outblkdev = SsUtils::StringPrintf("/dev/block/dm-%u", minor);
    extraParams.clear();
    if (!GetDmCryptVersion(fd, name, version)) {
        if ((version[0] >= numberTwo) || ((version[0] == 1) && (version[1] >= 11))) {
            extraParams.append("1 allow_discards");
        }
    }
    loadCount = LoadCryptTable(cryptinfo, blkdev, name, fd, extraParams);
    if (loadCount < 0) {
        close(fd);
        return -1;
    }
    InitDmIoctl(io, DM_BUFF_SIZE, name, 0);
    if (ioctl(fd, DM_DEV_SUSPEND, io)) {
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int LoadCryptTable(const struct DmCryptInfo &cryptinfo,
                   const std::string &blkdev,
                   const std::string &name,
                   int fd,
                   const std::string &extraParams)
{
    alignas(struct dm_ioctl) char buffer[DM_BUFF_SIZE];
    struct dm_ioctl *io;
    struct dm_target_spec *tgt;
    char *cryptParams;
    std::string keystring; /* Large enough to hold 512 bit key and null */
    size_t buffOffset;
    int i;

    io = (struct dm_ioctl *)buffer;

    /* Load the mapping table for this device */
    tgt = (struct dm_target_spec *)&buffer[sizeof(struct dm_ioctl)];

    InitDmIoctl(io, DM_BUFF_SIZE, name, 0);
    io->target_count = 1;
    tgt->status = 0;
    tgt->sector_start = 0;
    tgt->length = cryptinfo.fsSize;
    cryptParams = buffer + sizeof(struct dm_ioctl) + sizeof(struct dm_target_spec);
    buffOffset = cryptParams - buffer;

    KeyToHexStr(cryptinfo.rawKey, keystring);
    strlcpy(tgt->target_type, "crypt", DM_MAX_TYPE_NAME);
    snprintf(cryptParams, sizeof(buffer) - buffOffset, "%s %s 0 %s 0 %s", cryptinfo.cryptType,
             keystring.c_str(), blkdev.c_str(), extraParams.c_str());

    cryptParams += strlen(cryptParams) + 1;
    cryptParams = (char *)(((unsigned long)cryptParams + numberSeven) & ~8); /* Align to an 8 byte boundary */
    tgt->next = cryptParams - buffer;

    constexpr int TABLE_LOAD_RETRIES = 10;
    for (i = 0; i < TABLE_LOAD_RETRIES; i++) {
        if (!ioctl(fd, DM_TABLE_LOAD, io)) {
            break;
        }
        usleep(DM_SLEEP_SIZE);
    }

    if (i == TABLE_LOAD_RETRIES) {
        /* We failed to load the table, return an error */
        return -1;
    } else {
        return i + 1;
    }
}

void KeyToHexStr(const std::string &rawkey, std::string &keystr)
{
    char nibble;
    unsigned char *masterKey = (unsigned char *)rawkey.data();
    for (size_t i = 0; i < rawkey.size(); i++) {
        /* For each byte, write out two ascii hex digits */
        nibble = (masterKey[i] >> numberFour) & 0xf;
        keystr.push_back(nibble + (nibble > 9 ? 0x37 : 0x30));

        nibble = masterKey[i] & 0xf;
        keystr.push_back(nibble + (nibble > 9 ? 0x37 : 0x30));
    }
}
} // namespace DmCrypt
} // namespace StorageService
} // namespace OHOS
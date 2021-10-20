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
#include "storage_disk.h"

#include <list>
#include <memory>
#include <numeric>
#include <string>
#include <sys/sysmacros.h>

#include "interface/storage_service_callback_proxy.h"
#include "storage_base.h"
#include "storage_constant.h"
#include "storage_hilog.h"
#include "storage_manager.h"
#include "storage_private_volume.h"
#include "storage_public_volume.h"
#include "storage_utils.h"
#include "utils_file.h"
#include "utils_string.h"

using namespace OHOS::SsUtils;
using namespace OHOS::StorageService::Constants;

namespace OHOS {
namespace StorageService {
static constexpr int SWITCH_FLAGS_SETDISKID = 1;
static constexpr int SWITCH_FLAGS_SETPARTGUID = 2;
static constexpr int SWITCH_FLAGS_SETSILENT = 6;
static constexpr int MAJORID_FLAGS_FIFTEEN = 15;
static constexpr int MAJORID_FLAGS_PARTITION = 127;
static constexpr int NUM_FLAGS_ISTEN = 10;
static constexpr int NUM_FLAGS_ISNINETY = 90;
static constexpr int NUM_FLAGS_ISHUNDRED = 100;
static constexpr int NUM_FLAGS_ISBITESIZE = 1024;

static const char *diskSysfsMmcMaxMinorsDeprecated = "/sys/module/mmcblk/parameters/perdev_minors";
static const char *diskSysfsMmcMaxMinors = "/sys/module/mmc_block/parameters/perdev_minors";
static const char *diskSysfsLoopMaxMinors = "/sys/module/loop/parameters/max_part";
static const std::string diskSgDiskPath = "/system/bin/sgdisk";

Disk::Disk(const std::string &eventPath, dev_t device, const std::string &nickname, int flags)
    : diskDevice(device)
    , diskSize(-1)
    , diskFlags(flags)
    , diskCreated(false)
    , diskJustPartitioned(false)
    , diskNickname(nickname)
{
    diskId = StringPrintf("disk:%u,%u", major(device), minor(device));
    diskEventPath = eventPath;
    diskSysPath = StringPrintf("/sys/%s", eventPath.c_str());
    diskDevPath = StringPrintf("/dev/block/platform/%s", diskId.c_str());
    diskBlkDev = new SsBlockDevice(diskDevPath, diskDevice);
    if (diskBlkDev != nullptr) {
        diskBlkDev->CreateNode();
    }
}

void Disk::CreateVolume(dev_t device, const std::string &partGuid, bool createVolumeIsPrivate)
{
    auto ohosVol = std::shared_ptr<StorageBase>(new StoragePublicVolume(device));
    if (createVolumeIsPrivate) {
        std::string normalizedGuid;
        std::string keyRaw;
        if (NormalizeHex(partGuid, normalizedGuid) && !ReadFile(BuildKeyPath(normalizedGuid), &keyRaw)) {
            return;
        }
        ohosVol = std::shared_ptr<StorageBase>(new StoragePrivateVolume(device, keyRaw));
    }
    if (diskJustPartitioned) {
        ohosVol->SetInfo(getId(), "", "", -1, -1, true, SWITCH_FLAGS_SETSILENT);
        ohosVol->Create();
        ohosVol->Format("auto");
        ohosVol->Destroy();
        ohosVol->SetInfo(getId(), "", "", -1, -1, false, SWITCH_FLAGS_SETSILENT);
    }
    mVolumes.push_back(ohosVol);
    ohosVol->SetInfo(getId(), "", "", -1, -1, true, SWITCH_FLAGS_SETDISKID);
    if (createVolumeIsPrivate) {
        ohosVol->SetInfo("", partGuid, "", -1, -1, true, SWITCH_FLAGS_SETPARTGUID);
    }
    ohosVol->Create();
}

std::shared_ptr<StorageBase> Disk::FindVolume(const std::string &id)
{
    for (auto vol : mVolumes) {
        if (vol->getId() == id) {
            return vol;
        }
        auto stackedVol = vol->FindVolume(id);
        if (stackedVol != nullptr) {
            return stackedVol;
        }
    }
    return nullptr;
}

int Disk::UnMountAll()
{
    for (const auto &vol : mVolumes) {
        vol->UnMount();
    }
    return OK;
}

int Disk::Create()
{
    auto listener = StorageManager::Instance()->GetListener();
    if (listener) {
        listener->OnDiskCreated(getId(), diskFlags);
    }

    if (diskFlags & diskFlagIsStub) {
        if (listener) {
            listener->OnDiskMetadataChanged(getId(), diskSize, diskLabel, diskSysPath);
        }
        mVolumes[0]->SetInfo(getId(), "", "", -1, -1, true, SWITCH_FLAGS_SETDISKID);
        mVolumes[0]->Create();
        return OK;
    }

    DiskReadMetadata();
    ReadPartitions();
    return 0;
}

int Disk::DiskReadMetadata()
{
    std::string tmp;
    diskSize = -1;
    diskLabel.clear();
    diskSize = diskBlkDev->GetSize();

    unsigned int majorId = major(diskDevice);
    if (diskBlkDev->IsVirtualDev(majorId)) {
        diskLabel = diskBlkDev->GetVirtualDevLabel();
    } else if (diskBlkDev->IsScsiDev(majorId)) {
        diskLabel = diskBlkDev->GetScsiDevLabel(diskSysPath);
    } else if (diskBlkDev->IsMmcDev(majorId)) {
        diskLabel = diskBlkDev->GetMmcDevLabel(diskSysPath);
    } else if (diskBlkDev->IsVirtioBlk(majorId)) {
        diskLabel = diskBlkDev->GetVirtioDevLabel();
    } else if (diskBlkDev->IsNvmeDev(majorId, diskSysPath)) {
        diskLabel = diskBlkDev->GetNvmeLabel(diskSysPath);
    } else {
        return -ENOTSUP;
    }
    auto listener = StorageManager::Instance()->GetListener();
    if (listener) {
        listener->OnDiskMetadataChanged(getId(), diskSize, diskLabel, diskSysPath);
    }
    return 0;
}

int Disk::ReadPartitionsHandler()
{
    dev_t partDevice = makedev(major(diskDevice), minor(diskDevice) + 1);
    CreateVolume(partDevice, "ok", false);
    return 0;
}

int Disk::ReadPartitions()
{
    if (GetMaxMinors() < 0) {
        return -ENOTSUP;
    }
    if (!mVolumes.empty()) {
        for (const auto &vol : mVolumes) {
            vol->Destroy();
        }
        mVolumes.clear();
    }
    ReadPartitionsHandler();
    diskJustPartitioned = false;
    return OK;
}

int Disk::Destroy()
{
    if (!mVolumes.empty()) {
        for (const auto &vol : mVolumes) {
            vol->Destroy();
        }
        mVolumes.clear();
    }
    diskCreated = false;
    auto listener = StorageManager::Instance()->GetListener();
    if (listener) {
        listener->OnDiskDestroyed(getId());
    }
    return OK;
}

int Disk::GetMaxMinors()
{
    unsigned int majorId = major(diskDevice);
    if (diskBlkDev->IsVirtualDev(majorId)) {
        std::string tmp("");
        if (!ReadFile(diskSysfsLoopMaxMinors, &tmp)) {
            return -1;
        }
        return std::stoi(tmp);
    } else if (diskBlkDev->IsScsiDev(majorId)) {
        return MAJORID_FLAGS_FIFTEEN;
    } else if (diskBlkDev->IsMmcDev(majorId)) {
        std::string tmp("");
        if (!ReadFile(diskSysfsMmcMaxMinors, &tmp) && !ReadFile(diskSysfsMmcMaxMinorsDeprecated, &tmp)) {
            return -1;
        }
        return std::stoi(tmp);
    } else if (diskBlkDev->IsVirtioBlk(majorId)) {
        return MAJORID_FLAGS_FIFTEEN;
    } else if (diskBlkDev->IsNvmeDev(majorId, diskSysPath)) {
        return MAJORID_FLAGS_PARTITION;
    } else {
        return -ENOTSUP;
    }
}

int Disk::PartitionHandle(int partitionHandleFlags, int8_t ratio)
{
    int res;
    if (!mVolumes.empty()) {
        for (const auto &vol : mVolumes) {
            vol->Destroy();
        }
        mVolumes.clear();
    }
    diskJustPartitioned = true;
    std::vector<std::string> cmd;
    cmd.push_back(diskSgDiskPath);
    cmd.push_back("--zap-all");
    cmd.push_back(diskDevPath);
    cmd.clear();
    cmd.push_back(diskSgDiskPath);
    if (partitionHandleFlags == PartitionHandleFlagsIsPublic) {
        cmd.push_back("--new=0:0:-0");
        cmd.push_back("--typecode=0:0c00");
        cmd.push_back("--gpttombr=1");
        cmd.push_back(diskDevPath);
        if ((res = ExecuteCmd(diskSgDiskPath.c_str(), cmd, nullptr)) != 0) {
            return res;
        }
    } else if (partitionHandleFlags == PartitionHandleFlagsIsPrivate) {
        if (ratio > 0) {
            if (ratio < NUM_FLAGS_ISTEN || ratio > NUM_FLAGS_ISNINETY) {
                return -EINVAL;
            }
            uint64_t splitMb =
                ((diskSize / NUM_FLAGS_ISHUNDRED) * ratio) / NUM_FLAGS_ISBITESIZE / NUM_FLAGS_ISBITESIZE;
            cmd.push_back(StringPrintf("--new=0:0:+%lldM", splitMb));
            cmd.push_back("--change-name=0:shared");
        }
        cmd.push_back("--new=0:0:+16M");
        cmd.push_back("--change-name=0:ohos_meta");
        cmd.push_back("--new=0:0:-0");
        cmd.push_back("--partition-guid=0:66oh-88os");
        cmd.push_back("--change-name=0:ohos_expand");
        cmd.push_back(diskDevPath);
        if ((res = ExecuteCmd(diskSgDiskPath.c_str(), cmd, nullptr)) != 0) {
            return res;
        }
    }
    return OK;
}

void Disk::ListVolumes(StorageBase::Type type, std::list<std::string> &list) const
{
    for (const auto &vol : mVolumes) {
        if (vol->GetType() == type) {
            list.push_back(vol->getId());
        }
    }
}

std::vector<std::shared_ptr<StorageBase>> Disk::GetVolumes() const
{
    std::vector<std::shared_ptr<StorageBase>> vols;
    for (const auto &vol : mVolumes) {
        vols.push_back(vol);
        auto stackedVolumes = vol->getVolumes();
        vols.insert(vols.end(), stackedVolumes.begin(), stackedVolumes.end());
    }
    return vols;
}
} // namespace StorageService
} // namespace OHOS
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
#include "storage_private_volume.h"

#include <cstdlib>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include "exfatmgr.h"
#include "ext4mgr.h"
#include "f2fsmgr.h"
#include "ntfsmgr.h"
#include "storage_dm_crypt.h"
#include "storage_emulated_volume.h"
#include "storage_hilog.h"
#include "storage_manager.h"
#include "storage_utils.h"
#include "utils_file.h"
#include "utils_string.h"
#include "vfatmgr.h"
static const unsigned int majorBlockLoop = 7;
static const unsigned int majorBlockMmc = 179;

using namespace OHOS::SsUtils;
using namespace OHOS::StorageService::Constants;
using namespace OHOS::StorageService::DmCrypt;

namespace OHOS {
namespace StorageService {
static constexpr int SWITCH_FLAGS_SENDPAth = 1;
static constexpr int SWITCH_FLAGS_SETID = 3;
static constexpr int SWITCH_FLAGS_SETMOUNTUSERID = 4;
static constexpr int PERMISSION_FLAGS = 700;
static constexpr int DIGITAL0771 = 771;
static constexpr int DIGITAL0711 = 711;
static constexpr int DIGITAL0770 = 770;
static constexpr int DIGITAL0751 = 751;

StoragePrivateVolume::StoragePrivateVolume(dev_t device, const std::string &keyraw)
    : StorageBase(Type::baseTypeIsPrivate)
    , mRawDevice(device)
    , mKeyRaw(keyraw)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    SetInfo("", "", StringPrintf("private:%u,%u", major(device), minor(device)), -1, -1, true,
            SWITCH_FLAGS_SETID);
    mRawDevPath = StringPrintf("/dev/block/platform/%s", getId().c_str());
    privateBlkDev = nullptr;
}

StoragePrivateVolume::~StoragePrivateVolume() {}

int StoragePrivateVolume::DoCreate()
{
    if (privateBlkDev == nullptr) {
        privateBlkDev = new SsBlockDevice(mRawDevPath, mRawDevice);
    }
    if (privateBlkDev != nullptr) {
        if (!privateBlkDev->CreateNode()) {
            return -EIO;
        }
    } else {
        return -1;
    }

    DmCrypt::RemoveExtVolume(getId());

    if (DmCrypt::CreateExtVolume(getId(), mRawDevPath, mKeyRaw, mDmDevPath) != 0) {
        return -EIO;
    }
    return OK;
}

int StoragePrivateVolume::DoDestroy()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (DmCrypt::RemoveExtVolume(getId()) != 0) {
        return -EIO;
    }
    if (privateBlkDev != nullptr) {
        if (privateBlkDev->DestroyNode()) {
            return 0;
        }
    }
    return -EIO;
}
int StoragePrivateVolume::DoMount()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);

    privateVolMapStringValues["ext4"] = privateVolEXT4;
    privateVolMapStringValues["f2fs"] = privateVolF2FS;

    int res = ReadDevInfo(mDmDevPath, &mFsType, &mFsUuid, &mFsLabel);
    auto listener = StorageManager::Instance()->GetListener();
    if (listener) {
        listener->OnVolumeMetadataChanged(getId(), mFsType, mFsUuid, mFsLabel);
    }

    if (res) {
        return -EIO;
    } else {
        mPath = StringPrintf("/mnt/expand/%s", mFsUuid.c_str());
        SendInfo(mPath, "", State::baseStateIsError, SWITCH_FLAGS_SENDPAth);

        if (PrepareDir(mPath, PERMISSION_FLAGS, AID_ROOT, AID_ROOT)) {
            return -EIO;
        }

        switch (privateVolMapStringValues[mFsType.c_str()]) {
            case privateVolEXT4: {
                int resTemp = Ext4Mgr::Check(mDmDevPath, mPath);
                if (resTemp == 0 || resTemp == 1) {
                    if (Ext4Mgr::Mount(mDmDevPath, mPath, false, false, true)) {
                        return -EIO;
                    }
                } else {
                    return -EIO;
                }
                break;
            }
            case privateVolF2FS: {
                if (F2fsMgr::Check(mDmDevPath) == 0) {
                    if (F2fsMgr::Mount(mDmDevPath, mPath)) {
                        return -EIO;
                    }
                } else {
                    return -EIO;
                }
                break;
            }
            default: return -EIO; break;
        }

        if (PrepareAllDir()) {
            return -EIO;
        }
        return OK;
    }
}

void StoragePrivateVolume::DoPostMount()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    auto vol_manager = StorageManager::Instance();
    std::string mediaPath(mPath + "/media");
    for (int user : vol_manager->getStartedUsers()) {
        auto vol =
            std::shared_ptr<StorageBase>(new StorageEmulatedVolume(mediaPath, mRawDevice, mFsUuid, user));
        vol->SetInfo("", "", "", user, -1, true, SWITCH_FLAGS_SETMOUNTUSERID);
        HandleVolume(vol, 1);
        vol->Create();
    }
}

int StoragePrivateVolume::DoUnMount()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    ForceUnMount(mPath);
    TEMP_FAILURE_RETRY(rmdir(mPath.c_str()));
    return OK;
}

int StoragePrivateVolume::DoFormat(const std::string &fsType)
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);

    privateVolMapStringValues["auto"] = privateVolAUTO;
    privateVolMapStringValues["ext4"] = privateVolEXT4;
    privateVolMapStringValues["f2fs"] = privateVolF2FS;

    switch (privateVolMapStringValues[fsType.c_str()]) {
        case privateVolAUTO: {
            if ((major(mRawDevice) == majorBlockMmc || major(mRawDevice) == majorBlockLoop ||
                 privateBlkDev->IsVirtioBlk(major(mRawDevice))) &&
                F2fsMgr::IsSupported()) {
                if (F2fsMgr::Format(mDmDevPath)) {
                    return -EIO;
                }
            } else {
                if (Ext4Mgr::Format(mDmDevPath)) {
                    return -EIO;
                }
            }
            break;
        }
        case privateVolEXT4:
            if (Ext4Mgr::Format(mDmDevPath)) {
                return -EIO;
            }
            break;
        case privateVolF2FS:
            if (F2fsMgr::Format(mDmDevPath)) {
                return -EIO;
            }
            break;
        default: return -EINVAL; break;
    }
    return OK;
}

bool StoragePrivateVolume::PrepareAllDir()
{
    return PrepareDir(mPath + "/app", DIGITAL0771, AID_SYSTEM, AID_SYSTEM) ||
           PrepareDir(mPath + "/user", DIGITAL0711, AID_SYSTEM, AID_SYSTEM) ||
           PrepareDir(mPath + "/user_de", DIGITAL0711, AID_SYSTEM, AID_SYSTEM) ||
           PrepareDir(mPath + "/media", DIGITAL0770, AID_MEDIA_RW, AID_MEDIA_RW) ||
           PrepareDir(mPath + "/media/0", DIGITAL0770, AID_MEDIA_RW, AID_MEDIA_RW) ||
           PrepareDir(mPath + "/local", DIGITAL0751, AID_ROOT, AID_ROOT) ||
           PrepareDir(mPath + "/local/tmp", DIGITAL0771, AID_SHELL, AID_SHELL);
}
} // namespace StorageService
} // namespace OHOS
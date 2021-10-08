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
#include <cstdlib>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "exfatmgr.h"
#include "ext4mgr.h"
#include "f2fsmgr.h"
#include "ntfsmgr.h"
#include "storage_public_volume.h"
#include "storage_base.h"
#include "storage_hilog.h"
#include "storage_manager.h"
#include "storage_utils.h"
#include "utils_file.h"
#include "utils_string.h"
#include "vfatmgr.h"
using namespace OHOS::SsUtils;
using namespace OHOS::StorageService::Constants;

namespace OHOS {
namespace StorageService {
static constexpr int SWITCH_FLAGS_SENDPAth = 1;
static constexpr int SWITCH_FLAGS_SENDInternalPath = 2;
static constexpr int SWITCH_FLAGS_SETID = 3;
static constexpr int SWITCH_FLAGS_SETDISKPATHID = 7;

static constexpr int PERMISSION_FLAGS = 700;
static const unsigned int MEMORY_UNIT = 1024;
static const std::string kAsecPath = "/mnt/secure/asec";

StoragePublicVolume::StoragePublicVolume(dev_t device)
    : StorageBase(Type::baseTypeIsPublic)
    , publicVolDevice(device)
{
    SetInfo("", "", StringPrintf("public:%u,%u", major(device), minor(device)), -1, -1, true,
            SWITCH_FLAGS_SETID);
    SetInfo("", "", StringPrintf("disk:%u,%u", major(device), minor(device) - 1), -1, -1, true,
            SWITCH_FLAGS_SETDISKPATHID);
    publicVolDevPath = StringPrintf("/dev/block/platform/%s", getDiskPathId().c_str());
    SSLOG_I(
        "dugl %{public}s %{public}s %{public}d \n publicVolDevPath=%{public}s \n getDiskPathId=%{public}s",
        __FILE__, __func__, __LINE__, publicVolDevPath.c_str(), getDiskPathId().c_str());
    publicVolUseSdcardFs = IsSdcardfsUsed();
    publicBlkDev = nullptr;
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
}

StoragePublicVolume::~StoragePublicVolume() {}

int StoragePublicVolume::InitAsecStage()
{
    std::string legacyPath(publicVolRawPath + "/ohos_secure");
    std::string securePath(publicVolRawPath + "/.ohos_secure");

    if (!access(legacyPath.c_str(), R_OK | X_OK) && access(securePath.c_str(), R_OK | X_OK)) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        rename(legacyPath.c_str(), securePath.c_str());
    }

    if (TEMP_FAILURE_RETRY(mkdir(securePath.c_str(), PERMISSION_FLAGS))) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        if (errno != EEXIST) {
            return -errno;
        }
    }
    MountForBind(securePath, kAsecPath);
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return OK;
}

int StoragePublicVolume::DoCreate()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (publicBlkDev == nullptr) {
        publicBlkDev = new SsBlockDevice(publicVolDevPath, publicVolDevice);
    }
    if (publicBlkDev != nullptr) {
        if (publicBlkDev->CreateNode()) {
            return 0;
        }
    }
    return -1;
}

int StoragePublicVolume::DoDestroy()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (publicBlkDev != nullptr) {
        if (publicBlkDev->DestroyNode()) {
            return 0;
        }
    }
    return -1;
}

int StoragePublicVolume::DoMount()
{
    int res = 0;
    bool isVisible = DoMountVisible();
    if (PrepareDir(publicVolRawPath.c_str(), PERMISSION_FLAGS, AID_ROOT, AID_ROOT)) {
        return -errno;
    }
    switch (publicVolMapStringValues[publicVolFsType.c_str()]) {
        case publicVolEXFAT:
            if (ExfatMgr::IsSupported() && !ExfatMgr::Check(publicVolDevPath)) {
                res = ExfatMgr::Mount(publicVolDevPath, publicVolRawPath, AID_ROOT,
                                      (isVisible ? AID_MEDIA_RW : AID_EXTERNAL_STORAGE), 0007);
            }
            break;
        case publicVolEXT4:
            if (Ext4Mgr::IsSupported() && !Ext4Mgr::Check(publicVolDevPath, publicVolRawPath)) {
                res = Ext4Mgr::Mount(publicVolDevPath, publicVolRawPath, false, false, false);
            }
            break;
        case publicVolF2FS:
            if (F2fsMgr::IsSupported() && !F2fsMgr::Check(publicVolDevPath)) {
                res = F2fsMgr::Mount(publicVolDevPath, publicVolRawPath);
            }
            break;
        case publicVolNTFS:
            if (NtfsMgr::IsSupported() && NtfsMgr::Check(publicVolDevPath)) {
                res = NtfsMgr::Mount(publicVolDevPath, publicVolRawPath);
            }
            break;
        case publicVolVFAT:
            if (VfatMgr::IsSupported() && !VfatMgr::Check(publicVolDevPath)) {
                res = VfatMgr::Mount(publicVolDevPath, publicVolRawPath, false, false, false, AID_ROOT,
                                     (isVisible ? AID_MEDIA_RW : AID_EXTERNAL_STORAGE), 0007, true);
            }
            break;
        default: SSLOGFE("failed to Mount Unsupported filesystem "); break;
    }
    if (res) {
        return -EIO;
    } else {
        if (GetMountFlags() & MountFlags::baseMountFlagsIskPrimary) {
            InitAsecStage();
        }
        return OK;
    }
}

bool StoragePublicVolume::DoMountVisible()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    InitMapStringValues();
    bool isVisible = GetMountFlags() & MountFlags::baseMountFlagsIsVisible;
    SSLOG_I("dugl %{public}s %{public}s %{public}d \n GetMountFlags()=%{public}d \n isVisible=%{public}d \n "
            "publicVolFsType=%{public}s",
            __FILE__, __func__, __LINE__, GetMountFlags(), isVisible, publicVolFsType.c_str());
    ReadDevInfo(publicVolDevPath, &publicVolFsType, &publicVolFsUuid, &publicVolFsLabel);
    auto listener = StorageManager::Instance()->GetListener();
    if (listener) {
        listener->OnVolumeMetadataChanged(getId(), publicVolFsType, publicVolFsUuid, publicVolFsLabel);
    }

    std::string stableName = getId();
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (!publicVolFsUuid.empty()) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        stableName = publicVolFsUuid;
    }
    MountInit(stableName);
    SendInfo("", publicVolRawPath, State::baseStateIsError, SWITCH_FLAGS_SENDInternalPath);
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (isVisible) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        SendInfo(StringPrintf("/storage/%s", stableName.c_str()), "", State::baseStateIsError,
                 SWITCH_FLAGS_SENDPAth);
    } else {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        SendInfo(publicVolRawPath, "", State::baseStateIsError, SWITCH_FLAGS_SENDPAth);
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return isVisible;
}

int StoragePublicVolume::DoUnMount()
{
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);

    if (publicVolUseSdcardFs) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        ForceUnmountAll();
        RmdirAll();
        ClearAll();
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d publicVolRawPath=%{public}s", __FILE__, __func__, __LINE__,
            publicVolRawPath.c_str());
    ForceUnMount(publicVolRawPath);
    rmdir(publicVolRawPath.c_str());
    publicVolRawPath.clear();
    return OK;
}

int StoragePublicVolume::DoFormat(const std::string &fsType)
{
    int publicVolUseType = 0;
    int res = 0;
    InitMapStringValues();
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    switch (publicVolMapStringValues[fsType.c_str()]) {
        case publicVolAUTO: {
            if (VfatMgr::IsSupported() && ExfatMgr::IsSupported() && Ext4Mgr::IsSupported() &&
                F2fsMgr::IsSupported() && NtfsMgr::IsSupported()) {
                uint64_t size = 0;
                if (publicBlkDev != nullptr) {
                    size = publicBlkDev->GetSize();
                } else {
                    return -1;
                }
                if (size > 32896LL * MEMORY_UNIT * MEMORY_UNIT) {
                    publicVolUseType = publicVolVFAT;
                } else {
                    publicVolUseType = publicVolEXFAT;
                }
            }
            break;
        }
        case publicVolEXFAT: publicVolUseType = publicVolEXFAT; break;
        case publicVolEXT4: publicVolUseType = publicVolEXT4; break;
        case publicVolF2FS: publicVolUseType = publicVolF2FS; break;
        case publicVolNTFS: publicVolUseType = publicVolNTFS; break;
        case publicVolVFAT: publicVolUseType = publicVolVFAT; break;
        default: SSLOGFE("Unsupported filesystem "); break;
    }
    if (publicBlkDev != nullptr) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        publicBlkDev->Wipe();
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    switch (publicVolUseType) {
        case publicVolEXFAT: res = ExfatMgr::Format(publicVolDevPath); break;
        case publicVolEXT4: res = Ext4Mgr::Format(publicVolDevPath); break;
        case publicVolF2FS: res = F2fsMgr::Format(publicVolDevPath); break;
        case publicVolNTFS: res = NtfsMgr::Format(publicVolDevPath); break;
        case publicVolVFAT: res = VfatMgr::Format(publicVolDevPath); break;
        default: break;
    }

    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    if (res != OK) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        res = -errno;
    }
    SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
    return res;
}

void StoragePublicVolume::MountInit(std::string stableName)
{
    publicVolRawPath = StringPrintf("/data/ss/%s", stableName.c_str());
    publicVolSdcardFsDefault = StringPrintf("/data/ss/runtime/default/%s", stableName.c_str());
    publicVolSdcardFsRead = StringPrintf("/data/ss/runtime/read/%s", stableName.c_str());
    publicVolSdcardFsWrite = StringPrintf("/data/ss/runtime/write/%s", stableName.c_str());
    publicVolSdcardFsFull = StringPrintf("/data/ss/runtime/full/%s", stableName.c_str());
}

void StoragePublicVolume::ForceUnmountAll()
{
    ForceUnMount(publicVolSdcardFsDefault);
    ForceUnMount(publicVolSdcardFsRead);
    ForceUnMount(publicVolSdcardFsWrite);
    ForceUnMount(publicVolSdcardFsFull);
}

void StoragePublicVolume::RmdirAll()
{
    rmdir(publicVolSdcardFsDefault.c_str());
    rmdir(publicVolSdcardFsRead.c_str());
    rmdir(publicVolSdcardFsWrite.c_str());
    rmdir(publicVolSdcardFsFull.c_str());
}

void StoragePublicVolume::ClearAll()
{
    publicVolSdcardFsDefault.clear();
    publicVolSdcardFsRead.clear();
    publicVolSdcardFsWrite.clear();
    publicVolSdcardFsFull.clear();
}
void StoragePublicVolume::InitMapStringValues()
{
    publicVolMapStringValues["auto"] = publicVolAUTO;
    publicVolMapStringValues["exfat"] = publicVolEXFAT;
    publicVolMapStringValues["ext4"] = publicVolEXT4;
    publicVolMapStringValues["f2fs"] = publicVolF2FS;
    publicVolMapStringValues["ntfs"] = publicVolNTFS;
    publicVolMapStringValues["vfat"] = publicVolVFAT;
}
} // namespace StorageService
} // namespace OHOS
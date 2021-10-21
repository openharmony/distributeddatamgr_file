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
#include <cstdlib>
#include <fcntl.h>
#include <list>
#include <memory>
#include <vector>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "block_device.h"
#include "storage_emulated_volume.h"
#include "storage_hilog.h"
#include "storage_manager.h"
#include "storage_utils.h"
#include "utils_file.h"
#include "utils_string.h"

using namespace OHOS::SsUtils;
using namespace OHOS::StorageService::Constants;

namespace OHOS {
namespace StorageService {
static constexpr int SWITCH_FLAGS_SENDPAth = 1;
static constexpr int SWITCH_FLAGS_SENDInternalPath = 2;
static constexpr int SWITCH_FLAGS_SETID = 3;

static constexpr int PERMISSION_FLAGS = 700;
static const unsigned int GET_DEVICE_INTERVAL_TIME = 50000;
static const unsigned int GET_DEVICE_UNIT_TIME = 1000000;
static const unsigned int GET_DEVICE_THRESHOLD_TIME = 5000;
static const std::string kSdcardFsPath = "/system/bin/sdcard";

StorageEmulatedVolume::StorageEmulatedVolume(const std::string &rawPath, int userId)
    : StorageBase(Type::baseTypeIsEmulated)
{
    SetInfo("", "", StringPrintf("emulated;%u", userId), -1, -1, true, SWITCH_FLAGS_SETID);
    emulatedVolRawPath = rawPath;
    emulatedVolLabel = "emulated";
    emulatedVolUseSdcardFs = IsSdcardfsUsed();
    emulatedVolAppDataIsolationEnabled = true;
}

StorageEmulatedVolume::StorageEmulatedVolume(const std::string &rawPath,
                                             dev_t device,
                                             const std::string &fsUuid,
                                             int userId)
    : StorageBase(Type::baseTypeIsEmulated)
{
    SetInfo("", "", StringPrintf("emulated:%u,%u;%u", major(device), minor(device), userId), -1, -1, true,
            SWITCH_FLAGS_SETID);
    emulatedVolRawPath = rawPath;
    emulatedVolLabel = fsUuid;
    emulatedVolUseSdcardFs = IsSdcardfsUsed();
    emulatedVolAppDataIsolationEnabled = true;
}

StorageEmulatedVolume::~StorageEmulatedVolume() {}

int StorageEmulatedVolume::DoMount()
{
    std::string label;
    if (GetMountFlags() & MountFlags::baseMountFlagsIskPrimary) {
        label = "emulated";
    } else {
        label = emulatedVolLabel;
    }
    Init(label);
    SendInfo("", emulatedVolRawPath, State::baseStateIsError, SWITCH_FLAGS_SENDInternalPath);
    SendInfo(StringPrintf("/storage/%s", label.c_str()), "", State::baseStateIsError, SWITCH_FLAGS_SENDPAth);
    if (PreareDirCheck()) {
        return -errno;
    }
    dev_t before = SsBlockDevice::GetDevice(emulatedVolSdcardFsFull);
    int ret = StorageEmulatedVolume::ExeclFile(label, before);
    return ret;
}

int StorageEmulatedVolume::DoUnMount()
{
    if (!emulatedVolUseSdcardFs || GetMountUserId() != 0) {
        return -errno;
    }
    ForceAllUnMount();
    RmdirAll();
    ClearAll();
    return OK;
}

bool StorageEmulatedVolume::PreareDirCheck()
{
    return PrepareDir(emulatedVolSdcardFsDefault.c_str(), PERMISSION_FLAGS, AID_ROOT, AID_ROOT) ||
           PrepareDir(emulatedVolSdcardFsRead.c_str(), PERMISSION_FLAGS, AID_ROOT, AID_ROOT) ||
           PrepareDir(emulatedVolSdcardFsWrite.c_str(), PERMISSION_FLAGS, AID_ROOT, AID_ROOT) ||
           PrepareDir(emulatedVolSdcardFsFull.c_str(), PERMISSION_FLAGS, AID_ROOT, AID_ROOT);
}

int StorageEmulatedVolume::ExeclFile(std::string label, dev_t before)
{
    if (emulatedVolUseSdcardFs && GetMountUserId() == 0) {
        int sdcardFsPid;
        if (!(sdcardFsPid = fork())) {
            if (execl((char *)kSdcardFsPath.c_str(), (char *)kSdcardFsPath.c_str(), "-u", "1023", "-g",
                      "1023", "-m", "-w", "-G", "-i", "-o", emulatedVolRawPath.c_str(), label.c_str(),
                      NULL)) {}
            _exit(1);
        }
        if (sdcardFsPid == -1) {
            return -errno;
        } else {
            int64_t Start = GetSysTime();
            while (before == SsBlockDevice::GetDevice(emulatedVolSdcardFsFull)) {
                usleep(GET_DEVICE_INTERVAL_TIME); // 50ms
                int64_t now = GetSysTime();
                if (((now - Start) / GET_DEVICE_UNIT_TIME) > GET_DEVICE_THRESHOLD_TIME) {
                    return -ETIMEDOUT;
                }
            }
            TEMP_FAILURE_RETRY(waitpid(sdcardFsPid, nullptr, 0));
            sdcardFsPid = 0;
        }
    }
    return OK;
}

void StorageEmulatedVolume::Init(std::string label)
{
    emulatedVolSdcardFsDefault = StringPrintf("/data/ss/default/%s", label.c_str());
    emulatedVolSdcardFsWrite = StringPrintf("/data/ss/write/%s", label.c_str());
    emulatedVolSdcardFsRead = StringPrintf("/data/ss/read/%s", label.c_str());
    emulatedVolSdcardFsFull = StringPrintf("/data/ss/full/%s", label.c_str());
}
void StorageEmulatedVolume::ForceAllUnMount()
{
    ForceUnMount(emulatedVolSdcardFsDefault);
    ForceUnMount(emulatedVolSdcardFsRead);
    ForceUnMount(emulatedVolSdcardFsWrite);
    ForceUnMount(emulatedVolSdcardFsFull);
}

void StorageEmulatedVolume::RmdirAll()
{
    rmdir(emulatedVolSdcardFsDefault.c_str());
    rmdir(emulatedVolSdcardFsRead.c_str());
    rmdir(emulatedVolSdcardFsWrite.c_str());
    rmdir(emulatedVolSdcardFsFull.c_str());
}

void StorageEmulatedVolume::ClearAll()
{
    emulatedVolSdcardFsDefault.clear();
    emulatedVolSdcardFsRead.clear();
    emulatedVolSdcardFsWrite.clear();
    emulatedVolSdcardFsFull.clear();
}
} // namespace StorageService
} // namespace OHOS

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
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <list>
#include <mutex>
#include <system_ability_definition.h>
#include <unistd.h>

#include "local_event_socket.h"
#include "refbase.h"
#include "storage_hilog.h"
#include "storage_manager.h"
#include "storage_service.h"
#include "fs_mount_tab.h"

using namespace std;
using namespace OHOS;

static int ParsingFstabConfig(StorageManager *sm)
{
    int flagsSd = 0;
    std::string sysPatternSd = "/system/sd";
    std::string nicknameSd("sd");
    flagsSd |= OHOS::StorageService::Disk::Flags::diskFlagIsAdoptable;
    sm->HandleDiskVolume(nullptr, 1,
                         std::shared_ptr<StorageManager::DiskSource>(
                             new StorageManager::DiskSource(sysPatternSd, nicknameSd, flagsSd)),
                         3);
    return 0;
}

static void StartStorageService(void)
{
    do {
        auto sm = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sm != nullptr) {
            OHOS::sptr<OHOS::IRemoteObject> service = new OHOS::StorageNativeService();
            sm->AddSystemAbility(STORAGE_SERVICE_ID, service);
            break;
        }
    } while (1);
    OHOS::IPCSkeleton::JoinWorkThread();
}

static void do_coldboot(DIR *d, int lvl)
{
    struct dirent *de;
    int dfd, fd;

    dfd = dirfd(d);

    fd = openat(dfd, "uevent", O_WRONLY | O_CLOEXEC);
    if (fd >= 0) {
        write(fd, "add\n", 4);
        close(fd);
    }

    while ((de = readdir(d))) {
        DIR *d2;

        if (de->d_name[0] == '.') {
            continue;
        }

        if (de->d_type != DT_DIR && lvl > 0) {
            continue;
        }

        fd = openat(dfd, de->d_name, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
        if (fd < 0) {
            continue;
        }

        d2 = fdopendir(fd);
        if (d2 == nullptr) {
            close(fd);
        } else {
            do_coldboot(d2, lvl + 1);
            closedir(d2);
        }
    }
}

static void coldboot(const char *path)
{
    DIR *d = opendir(path);
    if (d) {
        do_coldboot(d, 0);
        closedir(d);
    }
}

int main()
{
    LocalEventSocket *nm = nullptr;
    StorageManager *sm = nullptr;

    if (!(sm = StorageManager::Instance())) {}
    if (!(nm = LocalEventSocket::Instance())) {}
    if (sm->Start()) {}
    if (ParsingFstabConfig(sm)) {}
    sm->GetMountInfo();
    if (nm->Start()) {}

    StartStorageService();
    coldboot("/sys/block");
    while (1) {
        sleep(1);
    }
    return 0;
}
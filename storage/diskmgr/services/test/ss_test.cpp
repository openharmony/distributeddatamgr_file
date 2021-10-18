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
#include <cstring>
#include <device_storage_service.h>
#include <functional>
#include <hilog/log.h>
#include <iostream>
#include <iservice_registry.h>
#include <securec.h>
#include <system_ability_definition.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <zlib.h>

#include "device_storage_manager.h"
#include "fbe_crypto.h"
#include "istorage_service.h"
#include "i_storage_service_task.h"
#include "storage_service.h"

using namespace OHOS;
#define TESTLOG(func, fmt, ...) (void)func(LOG_LABEL, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define TESTLOG_F(fmt, ...) TESTLOG(HiviewDFX::HiLog::Fatal, fmt, ##__VA_ARGS__)
#define TESTLOG_E(fmt, ...) TESTLOG(HiviewDFX::HiLog::Error, fmt, ##__VA_ARGS__)
#define TESTLOG_W(fmt, ...) TESTLOG(HiviewDFX::HiLog::Warn, fmt, ##__VA_ARGS__)
#define TESTLOG_I(fmt, ...) TESTLOG(HiviewDFX::HiLog::Info, fmt, ##__VA_ARGS__)
#define TESTLOG_D(fmt, ...) TESTLOG(HiviewDFX::HiLog::Debug, fmt, ##__VA_ARGS__)

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "SS_TEST" };
sptr<DeviceStorageService> g_deviceStorage = nullptr;
sptr<IStorageService> g_cryptoProxy = nullptr;
void Test1()
{
    TESTLOG_I("Test1 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();

    if (dsm != nullptr) {
        TESTLOG_I("Test1 Connect Executed");
        dsm->Connect();
    } else {
        TESTLOG_E("Test1 Execute FAILED");
    }
    TESTLOG_I("Test1 End");
}
void Test2()
{
    TESTLOG_I("Test2 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        TESTLOG_I("Test2 Mount Executed ");
        dsm->Mount("public:179,25");
    } else {
        TESTLOG_E("Test2 Execute FAILED");
    }
    TESTLOG_I("Test2 End");
}
void Test3()
{
    TESTLOG_I("Test3 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        TESTLOG_I("Test3 UnMount Executed ");
        dsm->UnMount("public:179,25");
    } else {
        TESTLOG_E("Test3 Execute FAILED");
    }
    TESTLOG_I("Test3 End");
}
void Test4()
{
    TESTLOG_I("Test4 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        TESTLOG_I("Test4 Format Executed ");
        dsm->Format("public:179,25");
    } else {
        TESTLOG_E("Test4 Execute FAILED");
    }
    TESTLOG_I("Test4 End");
}

void Test5()
{
    TESTLOG_I("Test5 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();

    if (dsm != nullptr) {
        TESTLOG_I("Test5 GetVolumes Executed ");
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        bool ret = dsm->GetVolumes(infos);
        if (ret) {
            TESTLOG_I("size is  %{public}d\n", infos.size());
        } else {
            TESTLOG_I("volumeInfos_ data is empty");
            return;
        }

        for (const auto &iter : infos) {
            TESTLOG_I("volume id %{public}s\n", iter->GetId().c_str());
            TESTLOG_I("volume type %{public}d\n", iter->GetType());
            TESTLOG_I("diskid %{public}s\n", iter->GetDiskId().c_str());
            TESTLOG_I("partguid %{public}s\n", iter->GetPartGuid().c_str());
            TESTLOG_I("uuid %{public}s\n", iter->GetFsUuid().c_str());
            TESTLOG_I("mountFlag %{public}d\n", iter->GetMountFlags());
            TESTLOG_I("mountUserId %{public}d\n", iter->GetMountUserId());
            TESTLOG_I("path %{public}s\n", iter->GetPath().c_str());
            TESTLOG_I("InternelPath %{public}s\n", iter->GetInternalPath().c_str());
            TESTLOG_I("state is %{public}d\n", iter->GetState());
        }
    } else {
        TESTLOG_E("Test5 Execute FAILED");
    }
    TESTLOG_I("Test5 End");
}

void Test6()
{
    TESTLOG_I("Test6 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        TESTLOG_I("Test6 PartitionPublic Executed ");
        dsm->PartitionPublic("disk:179,24");
    } else {
        TESTLOG_E("Test3 PartitionPublic FAILED");
    }
    TESTLOG_I("PartitionPublic End");
}

void Test7()
{
    TESTLOG_I("Test7 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        TESTLOG_I("Test7 PartitionPrivate Executed ");
        dsm->PartitionPrivate("disk:179,24");
    } else {
        TESTLOG_E("Test7 PartitionPrivate FAILED");
    }
    TESTLOG_I("PartitionPrivate End");
}

void Test8()
{
    TESTLOG_I("Test8 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        TESTLOG_I("Test8 SetPrimaryStorageUuid Executed ");
        // set later
        dsm->SetPrimaryStorageUuid("123");
    } else {
        TESTLOG_E("Test8 SetPrimaryStorageUuid FAILED");
    }
    TESTLOG_I("SetPrimaryStorageUuid End");
}

void Test9()
{
    TESTLOG_I("Test9 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        std::vector<std::shared_ptr<DS::DiskInfo>> infos;
        bool ret = dsm->GetDisks(infos);
        if (ret) {
            TESTLOG_I("size is  %{public}d\n", infos.size());
        } else {
            TESTLOG_I("infos  data is empty");
            return;
        }

        for (const auto &iter : infos) {
            TESTLOG_I("disk id %{public}s\n", iter->GetId().c_str());
            TESTLOG_I("sysPath %{public}s\n", iter->GetSysPath().c_str());
            TESTLOG_I("label %{public}s\n", iter->GetLabel().c_str());
        }
    } else {
        TESTLOG_E("Test9 Execute FAILED");
    }
    TESTLOG_I("Test9 End");
}

void Test10()
{
    TESTLOG_I("Test10 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        std::string primaryUuid;
        bool ret = dsm->GetPrimaryStorageUuid(primaryUuid);
        if (ret) {
            TESTLOG_I("primaryUuid is %{public}s\n", primaryUuid.c_str());
        } else {
            TESTLOG_I("Get primaryUuid error");
            return;
        }
    } else {
        TESTLOG_E("Test10 Execute FAILED");
    }
    TESTLOG_I("Test10 End");
}

void Test11()
{
    TESTLOG_I("Test11 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        std::shared_ptr<DS::VolumeInfo> vol;
        bool ret = dsm->FindVolumeById(vol, "public:179,25");
        if (ret) {
            TESTLOG_I("volume id %{public}s\n", vol->GetId().c_str());
            TESTLOG_I("volume type %{public}d\n", vol->GetType());
            TESTLOG_I("diskid %{public}s\n", vol->GetDiskId().c_str());
            TESTLOG_I("partguid %{public}s\n", vol->GetPartGuid().c_str());
            TESTLOG_I("uuid %{public}s\n", vol->GetFsUuid().c_str());
            TESTLOG_I("mountFlag %{public}d\n", vol->GetMountFlags());
            TESTLOG_I("mountUserId %{public}d\n", vol->GetMountUserId());
            TESTLOG_I("path %{public}s\n", vol->GetPath().c_str());
            TESTLOG_I("InternelPath %{public}s\n", vol->GetInternalPath().c_str());
            TESTLOG_I("state is %{public}d\n", vol->GetState());
        } else {
            TESTLOG_I("FindVolumeById error");
            return;
        }
    } else {
        TESTLOG_E("Test11 Execute FAILED");
    }
    TESTLOG_I("Test11 End");
}
void Test12()
{
    TESTLOG_I("Test12 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        std::shared_ptr<DS::VolumeInfo> vol;
        bool ret = dsm->FindVolumeByUuid(vol, "");
        if (ret) {
            TESTLOG_I("volume id %{public}s\n", vol->GetId().c_str());
            TESTLOG_I("volume type %{public}d\n", vol->GetType());
            TESTLOG_I("diskid %{public}s\n", vol->GetDiskId().c_str());
            TESTLOG_I("partguid %{public}s\n", vol->GetPartGuid().c_str());
            TESTLOG_I("uuid %{public}s\n", vol->GetFsUuid().c_str());
            TESTLOG_I("mountFlag %{public}d\n", vol->GetMountFlags());
            TESTLOG_I("mountUserId %{public}d\n", vol->GetMountUserId());
            TESTLOG_I("path %{public}s\n", vol->GetPath().c_str());
            TESTLOG_I("InternelPath %{public}s\n", vol->GetInternalPath().c_str());
            TESTLOG_I("state is %{public}d\n", vol->GetState());
        } else {
            TESTLOG_I("FindVolumeByuuid error");
            return;
        }
    } else {
        TESTLOG_E("Test12 Execute FAILED");
    }
    TESTLOG_I("Test12 End");
}
void Test13()
{
    TESTLOG_I("Test13 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        std::shared_ptr<DS::DiskInfo> disk;
        bool ret = dsm->FindDiskById(disk, "disk:179,24");
        if (ret) {
            TESTLOG_I("disk id %{public}s\n", disk->GetId().c_str());
            TESTLOG_I("sysPath %{public}s\n", disk->GetSysPath().c_str());
            TESTLOG_I("label %{public}s\n", disk->GetLabel().c_str());
        } else {
            TESTLOG_I("FindDiskById error");
            return;
        }
    } else {
        TESTLOG_E("Test13 Execute FAILED");
    }
    TESTLOG_I("Test13 End");
}

void Test14()
{
    TESTLOG_I("Test14 Start...(gby)");
    if (g_cryptoProxy != nullptr) {
        TESTLOG_I("SS_request::Connect start...(gby)");
    }
    if (g_cryptoProxy == nullptr) {
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            TESTLOG_E("SS_request::Connect samgr == nullptr...(gby)");
            return;
        }
        auto object = sam->GetSystemAbility(STORAGE_SERVICE_ID);
        if (object == nullptr) {
            TESTLOG_E("SS_request::Connect object == nullptr...(gby)");
            return;
        }
        g_cryptoProxy = iface_cast<IStorageService>(object);
        if (g_cryptoProxy == nullptr) {
            TESTLOG_E("SS_request::Connect service == nullptr...(gby)");
            return;
        }
    }
    TESTLOG_I("DeviceStorageManager::Connect end...(gby)");
    if (g_cryptoProxy == nullptr) {
        TESTLOG_E("The g_cryptoProxy is null...(gby)");
        return;
    }
    bool ret = g_cryptoProxy->CryptoEnable();
    if (ret == OHOS::FbeCrypto::CRYPTO_SUCCESS) {
        TESTLOG_I("CryptoEnable PASS...(gby)");
    } else {
        TESTLOG_E("Test14 Execute FAILED...(gby)");
    }
    TESTLOG_I("Test14 End...(gby)");
}

void Test15()
{
    TESTLOG_I("Test15 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        TESTLOG_I("Test15 GetVolumes Executed ");
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        bool ret = dsm->GetVolumes(infos);
        if (ret) {
            TESTLOG_I("size is  %{public}d\n", infos.size());
        } else {
            TESTLOG_I("volumeInfos_ data is empty");
            return;
        }
        for (auto &iter : infos) {
            std::string desCrip;
            dsm->GetBestVolumeDescription(iter, desCrip);
            TESTLOG_I("desCrip %{public}s\n", desCrip.c_str());
        }
    } else {
        TESTLOG_E("Test15 Execute FAILED");
    }
    TESTLOG_I("Test15 End");
}
void Test16()
{
    TESTLOG_I("Test16 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        TESTLOG_I("Test16 GetVolumes Executed ");
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        bool ret = dsm->GetVolumes(infos);
        if (ret) {
            TESTLOG_I("size is %{public}d\n", infos.size());
        } else {
            TESTLOG_I("volumeInfos_ data is empty");
            return;
        }
        for (auto &iter : infos) {
            if (iter->IsEmulated()) {
                std::shared_ptr<DS::VolumeInfo> priVol;
                dsm->FindPrivateForEmulate(priVol, iter);
                TESTLOG_I("volume id %{public}s\n", priVol->GetId().c_str());
                TESTLOG_I("volume type %{public}d\n", priVol->GetType());
                TESTLOG_I("diskid %{public}s\n", priVol->GetDiskId().c_str());
                TESTLOG_I("partguid %{public}s\n", priVol->GetPartGuid().c_str());
                TESTLOG_I("uuid %{public}s\n", priVol->GetFsUuid().c_str());
                TESTLOG_I("mountFlag %{public}d\n", priVol->GetMountFlags());
                TESTLOG_I("mountUserId %{public}d\n", priVol->GetMountUserId());
                TESTLOG_I("path %{public}s\n", priVol->GetPath().c_str());
                TESTLOG_I("InternelPath %{public}s\n", priVol->GetInternalPath().c_str());
                TESTLOG_I("state is %{public}d\n", priVol->GetState());
            }
        }
    } else {
        TESTLOG_E("Test16 Execute FAILED");
    }
    TESTLOG_I("Test16 End");
}
void Test17()
{
    TESTLOG_I("Test17 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        TESTLOG_I("Test17 GetVolumes Executed ");
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        bool ret = dsm->GetVolumes(infos);
        if (ret) {
            TESTLOG_I("size is %{public}d\n", infos.size());
        } else {
            TESTLOG_I("volumeInfos_ data is empty");
            return;
        }
        for (auto &iter : infos) {
            if (iter->GetType() == 1) {
                std::shared_ptr<DS::VolumeInfo> emuVol;
                dsm->FindEmulateForPrivate(emuVol, iter);
                TESTLOG_I("volume id %{public}s\n", emuVol->GetId().c_str());
                TESTLOG_I("volume type %{public}d\n", emuVol->GetType());
                TESTLOG_I("diskid %{public}s\n", emuVol->GetDiskId().c_str());
                TESTLOG_I("partguid %{public}s\n", emuVol->GetPartGuid().c_str());
                TESTLOG_I("uuid %{public}s\n", emuVol->GetFsUuid().c_str());
                TESTLOG_I("mountFlag %{public}d\n", emuVol->GetMountFlags());
                TESTLOG_I("mountUserId %{public}d\n", emuVol->GetMountUserId());
                TESTLOG_I("path %{public}s\n", emuVol->GetPath().c_str());
                TESTLOG_I("InternelPath %{public}s\n", emuVol->GetInternalPath().c_str());
                TESTLOG_I("state is %{public}d\n", emuVol->GetState());
            }
        }
    } else {
        TESTLOG_E("Test17 Execute FAILED");
    }
    TESTLOG_I("Test17 End");
}

void Test18()
{
    TESTLOG_I("Test18 Start");
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    if (dsm != nullptr) {
        bool ret1 = dsm->IsEncrypted("/data/unencrypt");
        bool ret2 = dsm->IsEncrypted("/data/app");
        bool ret3 = dsm->IsEncrypted("/data/vendor/usd/");
        bool ret4 = dsm->IsEncrypted("/data/vendor/usc/");
        TESTLOG_I("Disk id %{public}d\n",ret1); 
        TESTLOG_I("Disk id %{public}d\n",ret2); 
        TESTLOG_I("Disk id %{public}d\n",ret3); 
  	    TESTLOG_I("Disk id %{public}d\n",ret4); 
    } else {
        TESTLOG_E("Test18 Execute FAILED");
    }
    TESTLOG_I("Test18 End");
}

#define ADD_TEST(tests, id_, desc_) tests.push_back({ .id = id_, .desc = desc_, .func = Test##id_ })

struct StorageServiceTest {
    int32_t id;
    const char *desc;
    void (*func)();
};

void InitTest(std::vector<struct StorageServiceTest> &tests)
{
    ADD_TEST(tests, 1, "connect");
    ADD_TEST(tests, 2, "mount");
    ADD_TEST(tests, 3, "unMount");
    ADD_TEST(tests, 4, "format");
    ADD_TEST(tests, 5, "getvolumes");
    ADD_TEST(tests, 6, "partitionPublic");
    ADD_TEST(tests, 7, "partitionPrivate");
    ADD_TEST(tests, 8, "setPrimaryUuid");
    ADD_TEST(tests, 9, "getDisks");
    ADD_TEST(tests, 10, "getPrimaryUuid");
    ADD_TEST(tests, 11, "findvolumebyId");
    ADD_TEST(tests, 12, "findvolumebyuuid");
    ADD_TEST(tests, 13, "findDiskbyid");
    ADD_TEST(tests, 14, "CryptoEnable");
    ADD_TEST(tests, 15, "GetBestVolumeDescrip");
    ADD_TEST(tests, 16, "FindPrivate");
    ADD_TEST(tests, 17, "FindEmulate");
    ADD_TEST(tests, 18, "IsEncrypted");
}
void usage(const char *argv0, std::vector<struct StorageServiceTest> &tests)
{
    printf("usage: %s test_id\n", argv0);
    for (auto it = tests.begin(); it != tests.end(); it++) {
        printf("test %d: %s\n", it->id, it->desc);
    }
}
} // namespace

int32_t main(int32_t argc, const char *const argv[])
{
    TESTLOG_I("ss-test start");

    std::vector<struct StorageServiceTest> tests;
    InitTest(tests);
    if (argc <= 1) {
        usage(argv[0], tests);
        return 0;
    }
    int32_t testcase = -1;
    int ret = sscanf_s(argv[1], "%d", &testcase);
    if (ret == 0) {
        usage(argv[0], tests);
        return 1;
    }
    auto condition = [testcase](auto &item) { return item.id == testcase; };
    auto test = std::find_if(tests.begin(), tests.end(), condition);
    if (test != tests.end()) {
        test->func();
        printf("%d %s run!\n", test->id, test->desc);
    } else {
        printf("not found test %d\n", testcase);
        return 0;
    }
    return 0;
}

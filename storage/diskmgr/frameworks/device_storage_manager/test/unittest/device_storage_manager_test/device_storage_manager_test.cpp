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

#include "device_storage_manager_test.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <iostream>
#include <iservice_registry.h>
#include <refbase.h>
#include <sys/time.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "device_storage_manager.h"
#include "device_storage_manager_errors.h"
#include "device_storage_manager_hilog.h"
#include "device_storage_service.h"
#include "ipc/device_storage_service_proxy.h"
#include "volume_info.h"

using namespace std;
namespace {
using namespace OHOS;
class DeviceStorageManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase() {}
    int mountstate = 2;
    int unmountstate = 0;
    int sdcard = 2;
    int timeus = 1000000;
    int performance = 1000;
};

/**
 * @tc.number SUB_STORAGE_DSM_Function_0000
 * @tc.name DSM_Function_0000
 * @tc.desc Test function of GetVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_0000, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_0000" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        bool result = dsm->GetVolumes(volumeInfos);
        EXPECT_TRUE(result);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            int32_t userid = volumeInfos[i]->GetMountUserId();
            cout << "mId:" << volumeInfos[i]->GetId() << endl;
            cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
            cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
            cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
            cout << "mType:" << volumeInfos[i]->GetType() << endl;
            cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
            cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
            cout << "mState:" << volumeInfos[i]->GetState() << endl;
            cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
            cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
            cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
            cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
            cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
            cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
            cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid) << endl;
            cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
            cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetVolumes" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_0000" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_0100
 * @tc.name DSM_Function_0100
 * @tc.desc Test function of GetVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_0100, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_0100" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        bool result = dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        EXPECT_TRUE(result);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            std::string nId = "public";
            if (strstr(mId.c_str(), nId.c_str())) {
                int32_t userid = volumeInfos[i]->GetMountUserId();
                cout << "mId:" << volumeInfos[i]->GetId() << endl;
                cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
                cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
                cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
                cout << "mType:" << volumeInfos[i]->GetType() << endl;
                cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
                cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
                cout << "mState:" << volumeInfos[i]->GetState() << endl;
                cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
                cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
                cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
                cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
                cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid)
                     << endl;
                cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
            } else {
                printf("No sdcard volume information");
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetVolumes" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_0100" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_0200
 * @tc.name DSM_Function_0200
 * @tc.desc Test function of GetVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_0200, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_0200" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        bool result = dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        EXPECT_TRUE(result);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            std::string nId = "public";
            if (strstr(mId.c_str(), nId.c_str())) {
                int32_t userid = volumeInfos[i]->GetMountUserId();
                cout << "mId:" << volumeInfos[i]->GetId() << endl;
                cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
                cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
                cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
                cout << "mType:" << volumeInfos[i]->GetType() << endl;
                cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
                cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
                cout << "mState:" << volumeInfos[i]->GetState() << endl;
                cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
                cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
                cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
                cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
                cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid)
                     << endl;
                cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
            } else {
                printf("No sdcard volume information");
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetVolumes" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_0200" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_0300
 * @tc.name DSM_Function_0300
 * @tc.desc Test function of UnMount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_0300, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_0300" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos1;
        std::string mId1;
        std::string nId = "public";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos[i]->GetState() == mountstate) {
                    printf("Mount already\n");
                } else {
                    printf("No sdcard Mounted\n");
                }
                mId1 = mId;
                bool result = dsm->UnMount(mId1);
                EXPECT_TRUE(result);
            }
        }
        dsm->GetVolumes(volumeInfos1);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos1[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos1[i]->GetState() == unmountstate) {
                    printf("UnMount success\n");
                } else {
                    printf("UnMount failed\n");
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_UnMount" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_0300" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_0400
 * @tc.name DSM_Function_0400
 * @tc.desc Test function of Mount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_0400, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_0400" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos1;
        std::string nId = "public";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos[i]->GetState() == mountstate) {
                    printf("Mount already\n");
                } else {
                    printf("No sdcard Mounted\n");
                }
                std::string mId1 = mId;
                bool result = dsm->Mount(mId1);
                EXPECT_TRUE(result);
                std::string mInternalpath1 = volumeInfos[i]->GetInternalPath();
                string fpath = mInternalpath1 + "/a.txt";
                int fd = open(fpath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
                if (fd == -1) {
                    cout << "Failed to create file: " << fpath << endl;
                }
                close(fd);
                int rmFileRes = remove(fpath.c_str());
                if (rmFileRes == -1) {
                    cout << "Failed to remove file: " << fpath << endl;
                }
            }
            dsm->GetVolumes(volumeInfos1);
            for (int i = 0; i < count; i++) {
                std::string mId = volumeInfos1[i]->GetId();
                if (strstr(mId.c_str(), nId.c_str())) {
                    if (volumeInfos1[i]->GetState() == mountstate) {
                        printf("Mount success\n");
                    } else {
                        printf("Mount failed\n");
                    }
                }
            }
            cout << "DeviceStorageManagerTest-end Parameter_Mount" << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_0400" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_0500
 * @tc.name DSM_Function_0500
 * @tc.desc Test function of Format interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_0500, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_0500" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string nId = "public";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                std::string mId1 = mId;
                bool result = dsm->Format(mId1);
                EXPECT_TRUE(result);
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Format" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_0500" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_0600
 * @tc.name DSM_Function_0600
 * @tc.desc Test function of Format interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_0600, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_0600" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string nId = "public";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                std::string mId1 = mId;
                bool result = dsm->Format(mId1);
                EXPECT_TRUE(result);
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Format" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_0600" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_0700
 * @tc.name DSM_Function_0700
 * @tc.desc Test function of GetDisks interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_0700, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_0700" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::DiskInfo>> diskInfos;
        bool result = dsm->GetDisks(diskInfos);
        EXPECT_TRUE(result);
        int count = diskInfos.size();
        for (int i = 0; i < count; i++) {
            cout << "mId:" << diskInfos[i]->GetId() << endl;
            cout << "mSysPath:" << diskInfos[i]->GetSysPath() << endl;
            cout << "mSize:" << diskInfos[i]->GetSize() << endl;
            cout << "mLabel:" << diskInfos[i]->GetLabel() << endl;
            cout << "mFlags:" << diskInfos[i]->GetFlags() << endl;
            cout << "IsUsb:" << diskInfos[i]->IsUsb() << endl;
            cout << "IsSd:" << diskInfos[i]->IsSd() << endl;
            cout << "IsAdoptable:" << diskInfos[i]->IsAdoptable() << endl;
            cout << "GetDescription:" << diskInfos[i]->GetDescription() << endl;
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetDisks" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_0700" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_0800
 * @tc.name DSM_Function_0800
 * @tc.desc Test function of FindVolumeById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_0800, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_0800" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::shared_ptr<DS::VolumeInfo> vol;
        std::string nId = "public";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int a = 0; a < count; a++) {
            std::string mId = volumeInfos[a]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                mId = volumeInfos[a]->GetId();
                bool isFindVolume = dsm->FindVolumeById(vol, mId);
                EXPECT_TRUE(isFindVolume);
                int32_t userid = vol->GetMountUserId();
                cout << "mId:" << vol->GetId() << endl;
                cout << "mDiskId:" << vol->GetDiskId() << endl;
                cout << "mPartGuid:" << vol->GetPartGuid() << endl;
                cout << "mFsUuid:" << vol->GetFsUuid() << endl;
                cout << "mType:" << vol->GetType() << endl;
                cout << "mMountFlags:" << vol->GetMountFlags() << endl;
                cout << "mMountUserId:" << vol->GetMountUserId() << endl;
                cout << "mState:" << vol->GetState() << endl;
                cout << "mPath:" << vol->GetPath() << endl;
                cout << "mInternalPath:" << vol->GetInternalPath() << endl;
                cout << "mFsLabel" << vol->GetFsLabel() << endl;
                cout << "GetDescription:" << vol->GetDescription() << endl;
                cout << "IsVisibleForUser:" << vol->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << vol->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << vol->IsPrimaryEmulatedForUser(userid) << endl;
                cout << "IsRemovable:" << vol->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << vol->IsPrimary() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_0800" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_0900
 * @tc.name DSM_Function_0900
 * @tc.desc Test function of FindVolumeById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_0900, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_0900" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::shared_ptr<DS::VolumeInfo> vol;
        std::string nId = "emulated";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int a = 0; a < count; a++) {
            std::string mId = volumeInfos[a]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                mId = volumeInfos[a]->GetId();
                cout << volumeInfos[a]->GetId() << endl;
                bool isFindVolume = dsm->FindVolumeById(vol, mId);
                EXPECT_TRUE(isFindVolume);
                int32_t userid = vol->GetMountUserId();
                cout << "mId:" << vol->GetId() << endl;
                cout << "mDiskId:" << vol->GetDiskId() << endl;
                cout << "mPartGuid:" << vol->GetPartGuid() << endl;
                cout << "mFsUuid:" << vol->GetFsUuid() << endl;
                cout << "mType:" << vol->GetType() << endl;
                cout << "mMountFlags:" << vol->GetMountFlags() << endl;
                cout << "mMountUserId:" << vol->GetMountUserId() << endl;
                cout << "mState:" << vol->GetState() << endl;
                cout << "mPath:" << vol->GetPath() << endl;
                cout << "mInternalPath:" << vol->GetInternalPath() << endl;
                cout << "mFsLabel" << vol->GetFsLabel() << endl;
                cout << "GetDescription:" << vol->GetDescription() << endl;
                cout << "IsVisibleForUser:" << vol->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << vol->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << vol->IsPrimaryEmulatedForUser(userid) << endl;
                cout << "IsRemovable:" << vol->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << vol->IsPrimary() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_0900" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_1000
 * @tc.name DSM_Function_1000
 * @tc.desc Test function of FindVolumeById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_1000, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_1000" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::shared_ptr<DS::VolumeInfo> vol;
        std::string nId = "private";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int a = 0; a < count; a++) {
            std::string mId = volumeInfos[a]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                mId = volumeInfos[a]->GetId();
                cout << volumeInfos[a]->GetId() << endl;
                bool isFindVolume = dsm->FindVolumeById(vol, mId);
                EXPECT_TRUE(isFindVolume);
                int32_t userid = vol->GetMountUserId();
                cout << "mId:" << vol->GetId() << endl;
                cout << "mDiskId:" << vol->GetDiskId() << endl;
                cout << "mPartGuid:" << vol->GetPartGuid() << endl;
                cout << "mFsUuid:" << vol->GetFsUuid() << endl;
                cout << "mType:" << vol->GetType() << endl;
                cout << "mMountFlags:" << vol->GetMountFlags() << endl;
                cout << "mMountUserId:" << vol->GetMountUserId() << endl;
                cout << "mState:" << vol->GetState() << endl;
                cout << "mPath:" << vol->GetPath() << endl;
                cout << "mInternalPath:" << vol->GetInternalPath() << endl;
                cout << "mFsLabel" << vol->GetFsLabel() << endl;
                cout << "GetDescription:" << vol->GetDescription() << endl;
                cout << "IsVisibleForUser:" << vol->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << vol->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << vol->IsPrimaryEmulatedForUser(userid) << endl;
                cout << "IsRemovable:" << vol->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << vol->IsPrimary() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_1000" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_1100
 * @tc.name DSM_Function_1100
 * @tc.desc Test function of FindVolumeByUuid interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_1100, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_1100" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::shared_ptr<DS::VolumeInfo> vol;
        std::string nId = "public";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int a = 0; a < count; a++) {
            std::string mId = volumeInfos[a]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                std::string uuid = volumeInfos[a]->GetFsUuid();
                cout << volumeInfos[a]->GetFsUuid() << endl;
                bool isFindVolume = dsm->FindVolumeByUuid(vol, uuid);
                EXPECT_TRUE(isFindVolume);
                int32_t userid = vol->GetMountUserId();
                cout << "mId:" << vol->GetId() << endl;
                cout << "mDiskId:" << vol->GetDiskId() << endl;
                cout << "mPartGuid:" << vol->GetPartGuid() << endl;
                cout << "mFsUuid:" << vol->GetFsUuid() << endl;
                cout << "mType:" << vol->GetType() << endl;
                cout << "mMountFlags:" << vol->GetMountFlags() << endl;
                cout << "mMountUserId:" << vol->GetMountUserId() << endl;
                cout << "mState:" << vol->GetState() << endl;
                cout << "mPath:" << vol->GetPath() << endl;
                cout << "mInternalPath:" << vol->GetInternalPath() << endl;
                cout << "mFsLabel" << vol->GetFsLabel() << endl;
                cout << "GetDescription:" << vol->GetDescription() << endl;
                cout << "IsVisibleForUser:" << vol->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << vol->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << vol->IsPrimaryEmulatedForUser(userid) << endl;
                cout << "IsRemovable:" << vol->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << vol->IsPrimary() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeByUuid" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_1100" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_1200
 * @tc.name DSM_Function_1200
 * @tc.desc Test function of GetWritableVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_1200, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_1200" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string nId = "public";
        bool isGetWritableVolumes = dsm->GetWritableVolumes(volumeInfos);
        EXPECT_TRUE(isGetWritableVolumes);
        dsm->GetVolumes(volumeInfos);
        int count1 = volumeInfos.size();
        for (int i = 0; i < count1; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                int32_t userid = volumeInfos[i]->GetMountUserId();
                cout << "mId:" << volumeInfos[i]->GetId() << endl;
                cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
                cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
                cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
                cout << "mType:" << volumeInfos[i]->GetType() << endl;
                cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
                cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
                cout << "mState:" << volumeInfos[i]->GetState() << endl;
                cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
                cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
                cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
                cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
                cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid)
                     << endl;
                cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetWritableVolumes" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_1200" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_1300
 * @tc.name DSM_Function_1300
 * @tc.desc Test function of FindDiskById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_1300, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_1300" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::DiskInfo>> diskInfos;
        std::shared_ptr<DS::DiskInfo> diskInfo;
        std::string nId = "disk";
        dsm->GetDisks(diskInfos);
        int count1 = diskInfos.size();
        for (int i = 0; i < count1; i++) {
            std::string mId = diskInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                std::string mId = diskInfos[i]->GetId();
                bool isFindDiskById = dsm->FindDiskById(diskInfo, mId);
                EXPECT_TRUE(isFindDiskById);
                cout << "mId:" << diskInfo->GetId() << endl;
                cout << "mSysPath:" << diskInfo->GetSysPath() << endl;
                cout << "mSize:" << diskInfo->GetSize() << endl;
                cout << "mLabel:" << diskInfo->GetLabel() << endl;
                cout << "mFlags:" << diskInfo->GetFlags() << endl;
                cout << "IsUsb:" << diskInfo->IsUsb() << endl;
                cout << "IsSd:" << diskInfo->IsSd() << endl;
                cout << "IsAdoptable:" << diskInfo->IsAdoptable() << endl;
                cout << "GetDescription:" << diskInfo->GetDescription() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_FindDiskById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_1300" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_1400
 * @tc.name DSM_Function_1400
 * @tc.desc Test function of GetPrimaryStorageUuid interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_1400, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_1400" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::string primaryUuid;
        bool isGetUuid = dsm->GetPrimaryStorageUuid(primaryUuid);
        EXPECT_TRUE(isGetUuid);
        printf("Uuid:%s\n", primaryUuid.c_str());
        cout << "DeviceStorageManagerTest-end Parameter_GetPrimaryStorageUuid" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_1400" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_1500
 * @tc.name DSM_Function_1500
 * @tc.desc Test function of FindPrivateForEmulate interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_1500, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_1500" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::shared_ptr<DS::VolumeInfo> priVol;
        std::shared_ptr<DS::VolumeInfo> emuVol;
        dsm->GetVolumes(volumeInfos);
        emuVol = volumeInfos[0];
        bool isFindPrivate = dsm->FindPrivateForEmulate(priVol, emuVol);
        EXPECT_TRUE(isFindPrivate);
        int32_t userid = priVol->GetMountUserId();
        cout << "mId:" << priVol->GetId() << endl;
        cout << "mDiskId:" << priVol->GetDiskId() << endl;
        cout << "mPartGuid:" << priVol->GetPartGuid() << endl;
        cout << "mFsUuid:" << priVol->GetFsUuid() << endl;
        cout << "mType:" << priVol->GetType() << endl;
        cout << "mMountFlags:" << priVol->GetMountFlags() << endl;
        cout << "mMountUserId:" << priVol->GetMountUserId() << endl;
        cout << "mState:" << priVol->GetState() << endl;
        cout << "mPath:" << priVol->GetPath() << endl;
        cout << "mInternalPath:" << priVol->GetInternalPath() << endl;
        cout << "mFsLabel:" << priVol->GetFsLabel() << endl;
        cout << "GetDescription:" << priVol->GetDescription() << endl;
        cout << "IsVisibleForUser:" << priVol->IsVisibleForUser(userid) << endl;
        cout << "IsEmulated:" << priVol->IsEmulated() << endl;
        cout << "IsPrimaryEmulatedForUser:" << priVol->IsPrimaryEmulatedForUser(userid) << endl;
        cout << "IsRemovable:" << priVol->IsRemovable(userid) << endl;
        cout << "IsPrimary:" << priVol->IsPrimary() << endl;
        cout << "DeviceStorageManagerTest-end Parameter_FindPrivateForEmulate" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_1500" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_1600
 * @tc.name DSM_Function_1600
 * @tc.desc Test function of FindEmulateForPrivate interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_1600, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_1600" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::shared_ptr<DS::VolumeInfo> priVol;
        std::shared_ptr<DS::VolumeInfo> emuVol;
        dsm->GetVolumes(volumeInfos);
        priVol = volumeInfos[1];
        bool isFindPrivate = dsm->FindEmulateForPrivate(emuVol, priVol);
        EXPECT_TRUE(isFindPrivate);
        int32_t userid = emuVol->GetMountUserId();
        cout << "mId:" << emuVol->GetId() << endl;
        cout << "mDiskId:" << emuVol->GetDiskId() << endl;
        cout << "mPartGuid:" << emuVol->GetPartGuid() << endl;
        cout << "mFsUuid:" << emuVol->GetFsUuid() << endl;
        cout << "mType:" << emuVol->GetType() << endl;
        cout << "mMountFlags:" << emuVol->GetMountFlags() << endl;
        cout << "mMountUserId:" << emuVol->GetMountUserId() << endl;
        cout << "mState:" << emuVol->GetState() << endl;
        cout << "mPath:" << emuVol->GetPath() << endl;
        cout << "mInternalPath:" << emuVol->GetInternalPath() << endl;
        cout << "mFsLabel:" << emuVol->GetFsLabel() << endl;
        cout << "GetDescription:" << emuVol->GetDescription() << endl;
        cout << "IsVisibleForUser:" << emuVol->IsVisibleForUser(userid) << endl;
        cout << "IsEmulated:" << emuVol->IsEmulated() << endl;
        cout << "IsPrimaryEmulatedForUser:" << emuVol->IsPrimaryEmulatedForUser(userid) << endl;
        cout << "IsRemovable:" << emuVol->IsRemovable(userid) << endl;
        cout << "IsPrimary:" << emuVol->IsPrimary() << endl;
        cout << "DeviceStorageManagerTest-end Parameter_FindEmulateForPrivate" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_1600" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_1700
 * @tc.name DSM_Function_1700
 * @tc.desc Test function of GetBestVolumeDescription interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_1700, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_1700" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string desCription;
        std::shared_ptr<DS::VolumeInfo> vol;
        dsm->GetVolumes(volumeInfos);
        vol = volumeInfos[sdcard];
        bool isGetBestVolumeDescription = dsm->GetBestVolumeDescription(vol, desCription);
        EXPECT_TRUE(isGetBestVolumeDescription);
        printf("description %s\n", desCription.c_str());
        cout << "DeviceStorageManagerTest-end Parameter_GetBestVolumeDescription" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_1700" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_1800
 * @tc.name DSM_Function_1800
 * @tc.desc Test function of GetBestVolumeDescription interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_1800, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_1800" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string desCription;
        std::shared_ptr<DS::VolumeInfo> vol;
        dsm->GetVolumes(volumeInfos);
        vol = volumeInfos[0];
        bool isGetBestVolumeDescription = dsm->GetBestVolumeDescription(vol, desCription);
        EXPECT_TRUE(isGetBestVolumeDescription);
        printf("description %s\n", desCription.c_str());
        cout << "DeviceStorageManagerTest-end Parameter_GetBestVolumeDescription" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_1800" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_1900
 * @tc.name DSM_Function_1900
 * @tc.desc Test function of GetBestVolumeDescription interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_1900, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_1900" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string desCription;
        std::shared_ptr<DS::VolumeInfo> vol;
        dsm->GetVolumes(volumeInfos);
        vol = volumeInfos[1];
        bool isGetBestVolumeDescription = dsm->GetBestVolumeDescription(vol, desCription);
        EXPECT_TRUE(isGetBestVolumeDescription);
        printf("description %s\n", desCription.c_str());
        cout << "DeviceStorageManagerTest-end Parameter_GetBestVolumeDescription" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_1900" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_2000
 * @tc.name DSM_Function_2000
 * @tc.desc Test function of GetVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_2000, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_2000" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        bool result = dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        EXPECT_TRUE(result);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            std::string nId = "public";
            if (strstr(mId.c_str(), nId.c_str())) {
                int32_t userid = volumeInfos[i]->GetMountUserId();
                cout << "mId:" << volumeInfos[i]->GetId() << endl;
                cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
                cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
                cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
                cout << "mType:" << volumeInfos[i]->GetType() << endl;
                cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
                cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
                cout << "mState:" << volumeInfos[i]->GetState() << endl;
                cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
                cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
                cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
                cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
                cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid)
                     << endl;
                cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
            } else {
                printf("No U volume information");
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetVolumes" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_2000" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_2100
 * @tc.name DSM_Function_2100
 * @tc.desc Test function of GetVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_2100, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_2100" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        bool result = dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        EXPECT_TRUE(result);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            std::string nId = "public";
            if (strstr(mId.c_str(), nId.c_str())) {
                int32_t userid = volumeInfos[i]->GetMountUserId();
                cout << "mId:" << volumeInfos[i]->GetId() << endl;
                cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
                cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
                cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
                cout << "mType:" << volumeInfos[i]->GetType() << endl;
                cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
                cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
                cout << "mState:" << volumeInfos[i]->GetState() << endl;
                cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
                cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
                cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
                cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
                cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid)
                     << endl;
                cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
            } else {
                printf("No U volume information");
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetVolumes" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_2100" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_2200
 * @tc.name DSM_Function_2200
 * @tc.desc Test function of GetVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_2200, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_2200" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        bool result = dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        EXPECT_TRUE(result);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            std::string nId = "public";
            if (strstr(mId.c_str(), nId.c_str())) {
                int32_t userid = volumeInfos[i]->GetMountUserId();
                cout << "mId:" << volumeInfos[i]->GetId() << endl;
                cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
                cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
                cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
                cout << "mType:" << volumeInfos[i]->GetType() << endl;
                cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
                cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
                cout << "mState:" << volumeInfos[i]->GetState() << endl;
                cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
                cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
                cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
                cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
                cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid)
                     << endl;
                cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
            } else {
                printf("No U volume information");
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetVolumes" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_2200" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_2300
 * @tc.name DSM_Function_2300
 * @tc.desc Test function of UnMount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_2300, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_2300" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos1;
        std::string mId1;
        std::string nId = "public";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos[i]->GetState() == mountstate) {
                    printf("Mount already\n");
                } else {
                    printf("No U Mounted\n");
                }
                mId1 = mId;
                bool result = dsm->UnMount(mId1);
                EXPECT_TRUE(result);
            }
        }
        dsm->GetVolumes(volumeInfos1);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos1[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos1[i]->GetState() == unmountstate) {
                    printf("UnMount success\n");
                } else {
                    printf("UnMount failed\n");
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_UnMount" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_2300" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_2400
 * @tc.name DSM_Function_2400
 * @tc.desc Test function of Mount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_2400, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_2400" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos1;
        std::string nId = "public";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos[i]->GetState() == mountstate) {
                    printf("Mount already\n");
                } else {
                    printf("No U Mounted\n");
                }
                std::string mId1 = mId;
                bool result = dsm->Mount(mId1);
                EXPECT_TRUE(result);
                std::string mInternalpath1 = volumeInfos[i]->GetInternalPath();
                string fpath = mInternalpath1 + "/a.txt";
                int fd = open(fpath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
                if (fd == -1) {
                    cout << "Failed to create file: " << fpath << endl;
                }
                close(fd);
                int rmFileRes = remove(fpath.c_str());
                if (rmFileRes == -1) {
                    cout << "Failed to remove file: " << fpath << endl;
                }
            }
        }
        dsm->GetVolumes(volumeInfos1);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos1[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos1[i]->GetState() == mountstate) {
                    printf("Mount success\n");
                } else {
                    printf("Mount failed\n");
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Mount" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_2400" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_2500
 * @tc.name DSM_Function_2500
 * @tc.desc Test function of Format interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_2500, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_2500" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string mId1;
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            std::string nId = "public";
            if (strstr(mId.c_str(), nId.c_str())) {
                mId1 = mId;
                bool result = dsm->Format(mId1);
                EXPECT_TRUE(result);
                int32_t userid = volumeInfos[i]->GetMountUserId();
                cout << "mId:" << volumeInfos[i]->GetId() << endl;
                cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
                cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
                cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
                cout << "mType:" << volumeInfos[i]->GetType() << endl;
                cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
                cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
                cout << "mState:" << volumeInfos[i]->GetState() << endl;
                cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
                cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
                cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
                cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
                cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid)
                     << endl;
                cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Format" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_2500" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_2600
 * @tc.name DSM_Function_2600
 * @tc.desc Test function of Format interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_2600, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_2600" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string mId1;
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            std::string nId = "public";
            if (strstr(mId.c_str(), nId.c_str())) {
                mId1 = volumeInfos[i]->GetId();
                cout << mId1 << endl;
                bool result = dsm->Format(mId1);
                EXPECT_TRUE(result);
                int32_t userid = volumeInfos[i]->GetMountUserId();
                cout << "mId:" << volumeInfos[i]->GetId() << endl;
                cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
                cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
                cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
                cout << "mType:" << volumeInfos[i]->GetType() << endl;
                cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
                cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
                cout << "mState:" << volumeInfos[i]->GetState() << endl;
                cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
                cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
                cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
                cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
                cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid)
                     << endl;
                cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Format" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_2600" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_2700
 * @tc.name DSM_Function_2700
 * @tc.desc Test function of GetDisks interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_2700, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_2700" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::DiskInfo>> diskInfos;
        bool result = dsm->GetDisks(diskInfos);
        EXPECT_TRUE(result);
        int count = diskInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = diskInfos[i]->GetId();
            std::string nId = "disk";
            if (strstr(mId.c_str(), nId.c_str())) {
                cout << "mId:" << diskInfos[i]->GetId() << endl;
                cout << "mSysPath:" << diskInfos[i]->GetSysPath() << endl;
                cout << "mSize:" << diskInfos[i]->GetSize() << endl;
                cout << "mLabel:" << diskInfos[i]->GetLabel() << endl;
                cout << "mFlags:" << diskInfos[i]->GetFlags() << endl;
                cout << "IsUsb:" << diskInfos[i]->IsUsb() << endl;
                cout << "IsSd:" << diskInfos[i]->IsSd() << endl;
                cout << "IsAdoptable:" << diskInfos[i]->IsAdoptable() << endl;
                cout << "GetDescription:" << diskInfos[i]->GetDescription() << endl;
            } else {
                printf("not disk");
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetDisks" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_2700" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_2800
 * @tc.name DSM_Function_2800
 * @tc.desc Test function of FindVolumeById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_2800, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_2800" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::shared_ptr<DS::VolumeInfo> vol;
        std::string nId = "public";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int a = 0; a < count; a++) {
            std::string mId = volumeInfos[a]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                mId = volumeInfos[a]->GetId();
                cout << volumeInfos[a]->GetId() << endl;
                bool isFindVolume = dsm->FindVolumeById(vol, mId);
                EXPECT_TRUE(isFindVolume);
                int32_t userid = vol->GetMountUserId();
                cout << "mId:" << vol->GetId() << endl;
                cout << "mDiskId:" << vol->GetDiskId() << endl;
                cout << "mPartGuid:" << vol->GetPartGuid() << endl;
                cout << "mFsUuid:" << vol->GetFsUuid() << endl;
                cout << "mType:" << vol->GetType() << endl;
                cout << "mMountFlags:" << vol->GetMountFlags() << endl;
                cout << "mMountUserId:" << vol->GetMountUserId() << endl;
                cout << "mState:" << vol->GetState() << endl;
                cout << "mPath:" << vol->GetPath() << endl;
                cout << "mInternalPath:" << vol->GetInternalPath() << endl;
                cout << "mFsLabel" << vol->GetFsLabel() << endl;
                cout << "GetDescription:" << vol->GetDescription() << endl;
                cout << "IsVisibleForUser:" << vol->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << vol->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << vol->IsPrimaryEmulatedForUser(userid) << endl;
                cout << "IsRemovable:" << vol->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << vol->IsPrimary() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_2800" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_2900
 * @tc.name DSM_Function_2900
 * @tc.desc Test function of FindVolumeByUuid interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_2900, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_2900" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::shared_ptr<DS::VolumeInfo> vol;
        std::string nId = "public";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int a = 0; a < count; a++) {
            std::string mId = volumeInfos[a]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                std::string uuid = volumeInfos[a]->GetFsUuid();
                cout << volumeInfos[a]->GetFsUuid() << endl;
                bool isFindVolume = dsm->FindVolumeByUuid(vol, uuid);
                EXPECT_TRUE(isFindVolume);
                int32_t userid = vol->GetMountUserId();
                cout << "mId:" << vol->GetId() << endl;
                cout << "mDiskId:" << vol->GetDiskId() << endl;
                cout << "mPartGuid:" << vol->GetPartGuid() << endl;
                cout << "mFsUuid:" << vol->GetFsUuid() << endl;
                cout << "mType:" << vol->GetType() << endl;
                cout << "mMountFlags:" << vol->GetMountFlags() << endl;
                cout << "mMountUserId:" << vol->GetMountUserId() << endl;
                cout << "mState:" << vol->GetState() << endl;
                cout << "mPath:" << vol->GetPath() << endl;
                cout << "mInternalPath:" << vol->GetInternalPath() << endl;
                cout << "mFsLabel" << vol->GetFsLabel() << endl;
                cout << "GetDescription:" << vol->GetDescription() << endl;
                cout << "IsVisibleForUser:" << vol->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << vol->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << vol->IsPrimaryEmulatedForUser(userid) << endl;
                cout << "IsRemovable:" << vol->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << vol->IsPrimary() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeByUuid" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_2900" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_3000
 * @tc.name DSM_Function_3000
 * @tc.desc Test function of GetWritableVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_3000, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_3000" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string nId = "public";
        bool isGetWritableVolumes = dsm->GetWritableVolumes(volumeInfos);
        EXPECT_TRUE(isGetWritableVolumes);
        dsm->GetVolumes(volumeInfos);
        int count1 = volumeInfos.size();
        for (int i = 0; i < count1; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                int32_t userid = volumeInfos[i]->GetMountUserId();
                cout << "mId:" << volumeInfos[i]->GetId() << endl;
                cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
                cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
                cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
                cout << "mType:" << volumeInfos[i]->GetType() << endl;
                cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
                cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
                cout << "mState:" << volumeInfos[i]->GetState() << endl;
                cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
                cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
                cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
                cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
                cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid)
                     << endl;
                cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetWritableVolumes" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_3000" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_3100
 * @tc.name DSM_Function_3100
 * @tc.desc Test function of FindDiskById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_3100, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_3100" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::DiskInfo>> diskInfos;
        std::shared_ptr<DS::DiskInfo> diskInfo;
        std::string nId = "disk";
        dsm->GetDisks(diskInfos);
        int count1 = diskInfos.size();
        for (int i = 0; i < count1; i++) {
            std::string mId = diskInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                std::string mId = diskInfos[i]->GetId();
                bool isFindDiskById = dsm->FindDiskById(diskInfo, mId);
                EXPECT_TRUE(isFindDiskById);
                cout << "mId:" << diskInfo->GetId() << endl;
                cout << "mSysPath:" << diskInfo->GetSysPath() << endl;
                cout << "mSize:" << diskInfo->GetSize() << endl;
                cout << "mLabel:" << diskInfo->GetLabel() << endl;
                cout << "mFlags:" << diskInfo->GetFlags() << endl;
                cout << "IsUsb:" << diskInfo->IsUsb() << endl;
                cout << "IsSd:" << diskInfo->IsSd() << endl;
                cout << "IsAdoptable:" << diskInfo->IsAdoptable() << endl;
                cout << "GetDescription:" << diskInfo->GetDescription() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_FindDiskById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_3100" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_3200
 * @tc.name DSM_Function_3200
 * @tc.desc Test function of GetBestVolumeDescription interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_3200, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_3200" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string desCription;
        std::shared_ptr<DS::VolumeInfo> vol;
        dsm->GetVolumes(volumeInfos);
        vol = volumeInfos[sdcard];
        bool isGetBestVolumeDescription = dsm->GetBestVolumeDescription(vol, desCription);
        EXPECT_TRUE(isGetBestVolumeDescription);
        printf("description %s\n", desCription.c_str());
        cout << "DeviceStorageManagerTest-end Parameter_GetBestVolumeDescription" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_3200" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_3300
 * @tc.name DSM_Function_3300
 * @tc.desc Test function of GetVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_3300, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_3300" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        bool result = dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        EXPECT_TRUE(result);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            std::string nId = "emulate";
            if (strstr(mId.c_str(), nId.c_str())) {
                int32_t userid = volumeInfos[i]->GetMountUserId();
                cout << "mId:" << volumeInfos[i]->GetId() << endl;
                cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
                cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
                cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
                cout << "mType:" << volumeInfos[i]->GetType() << endl;
                cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
                cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
                cout << "mState:" << volumeInfos[i]->GetState() << endl;
                cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
                cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
                cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
                cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
                cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid)
                     << endl;
                cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
            } else {
                printf("No emulate information");
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetVolumes" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_3300" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_3400
 * @tc.name DSM_Function_3400
 * @tc.desc Test function of Mount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_3400, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_3400" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos1;
        std::string nId = "emulate";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos[i]->GetState() == mountstate) {
                    printf("Mount already\n");
                } else {
                    printf("No Mounted\n");
                }
                std::string mId1 = mId;
                bool result = dsm->Mount(mId1);
                EXPECT_TRUE(result);
                std::string mInternalpath1 = volumeInfos[i]->GetInternalPath();
                string fpath = mInternalpath1 + "/a.txt";
                int fd = open(fpath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
                if (fd == -1) {
                    cout << "Failed to create file: " << fpath << endl;
                }
                close(fd);
                int rmFileRes = remove(fpath.c_str());
                if (rmFileRes == -1) {
                    cout << "Failed to remove file: " << fpath << endl;
                }
            }
        }
        dsm->GetVolumes(volumeInfos1);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos1[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos1[i]->GetState() == mountstate) {
                    printf("Mount success\n");
                } else {
                    printf("Mount failed\n");
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Mount" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_3400" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_3500
 * @tc.name DSM_Function_3500
 * @tc.desc Test function of UnMount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_3500, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_3500" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos1;
        std::string mId1;
        std::string nId = "emulate";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos[i]->GetState() == mountstate) {
                    printf("Mount already\n");
                } else {
                    printf("No U Mounted\n");
                }
                mId1 = mId;
                bool result = dsm->UnMount(mId1);
                EXPECT_TRUE(result);
            }
        }
        dsm->GetVolumes(volumeInfos1);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos1[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos1[i]->GetState() == unmountstate) {
                    printf("UnMount success\n");
                } else {
                    printf("UnMount failed\n");
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_UnMount" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_3500" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Function_3600
 * @tc.name DSM_Function_3600
 * @tc.desc Test function of Format interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Function_3600, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Function_3600" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string mId1;
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            std::string nId = "emulate";
            if (strstr(mId.c_str(), nId.c_str())) {
                mId1 = mId;
                bool result = dsm->Format(mId1);
                EXPECT_TRUE(result);
                int32_t userid = volumeInfos[i]->GetMountUserId();
                cout << "mId:" << volumeInfos[i]->GetId() << endl;
                cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
                cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
                cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
                cout << "mType:" << volumeInfos[i]->GetType() << endl;
                cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
                cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
                cout << "mState:" << volumeInfos[i]->GetState() << endl;
                cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
                cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
                cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
                cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
                cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
                cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
                cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid)
                     << endl;
                cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
                cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Format" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Function_3600" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_0100
 * @tc.name DSM_ERROR_0100
 * @tc.desc Test function of Mount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_0100, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_0100" << endl;
    try {
        std::string mId1;
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos1;
        std::string nId = "public";
        mId1 = "";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos[i]->GetState() == mountstate) {
                    printf("Mount already\n");
                } else {
                    printf("No sdcard Mounted\n");
                }
            }
        }
        bool result = dsm->Mount(mId1);
        EXPECT_FALSE(result);
        dsm->GetVolumes(volumeInfos1);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos1[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos1[i]->GetState() == mountstate) {
                    printf("Mount success\n");
                } else {
                    printf("Mount failed\n");
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Mount" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_0100" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_0200
 * @tc.name DSM_ERROR_0200
 * @tc.desc Test function of Mount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_0200, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_0200" << endl;
    try {
        std::string mId1;
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos1;
        std::string nId = "public";
        mId1 = "123";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos[i]->GetState() == mountstate) {
                    printf("Mount already\n");
                } else {
                    printf("No sdcard Mounted\n");
                }
            }
        }
        bool result = dsm->Mount(mId1);
        EXPECT_FALSE(result);
        dsm->GetVolumes(volumeInfos1);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos1[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos1[i]->GetState() == mountstate) {
                    printf("Mount success\n");
                } else {
                    printf("Mount failed\n");
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Mount" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_0200" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_0300
 * @tc.name DSM_ERROR_0300
 * @tc.desc Test function of UnMount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_0300, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_0300" << endl;
    try {
        std::string mId1;
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos1;
        std::string nId = "public";
        mId1 = "123";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos[i]->GetState() == mountstate) {
                    printf("Mount already\n");
                } else {
                    printf("No sdcard Mounted\n");
                }
            }
        }
        bool result = dsm->UnMount(mId1);
        EXPECT_FALSE(result);
        dsm->GetVolumes(volumeInfos1);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos1[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos1[i]->GetState() == unmountstate) {
                    printf("UnMount success\n");
                } else {
                    printf("UnMount failed\n");
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_UnMount" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_0300" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_0400
 * @tc.name DSM_ERROR_0400
 * @tc.desc Test function of UnMount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_0400, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_0400" << endl;
    try {
        std::string mId1;
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos1;
        std::string nId = "public";
        mId1 = "";
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos[i]->GetState() == mountstate) {
                    printf("Mount already\n");
                } else {
                    printf("No sdcard Mounted\n");
                }
            }
        }
        bool result = dsm->UnMount(mId1);
        EXPECT_FALSE(result);
        dsm->GetVolumes(volumeInfos1);
        for (int i = 0; i < count; i++) {
            std::string mId = volumeInfos1[i]->GetId();
            if (strstr(mId.c_str(), nId.c_str())) {
                if (volumeInfos1[i]->GetState() == unmountstate) {
                    printf("UnMount success\n");
                } else {
                    printf("UnMount failed\n");
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_UnMount" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_0400" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_0500
 * @tc.name DSM_ERROR_0500
 * @tc.desc Test function of Format interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_0500, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_0500" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string volId = "123";
        bool result = dsm->Format(volId);
        EXPECT_FALSE(result);
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            int32_t userid = volumeInfos[i]->GetMountUserId();
            cout << "mId:" << volumeInfos[i]->GetId() << endl;
            cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
            cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
            cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
            cout << "mType:" << volumeInfos[i]->GetType() << endl;
            cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
            cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
            cout << "mState:" << volumeInfos[i]->GetState() << endl;
            cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
            cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
            cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
            cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
            cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
            cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
            cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid) << endl;
            cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
            cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
        }
        cout << "DeviceStorageManagerTest-end Parameter_Format" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_0500" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_0600
 * @tc.name DSM_ERROR_0600
 * @tc.desc Test function of Format interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_0600, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_0600" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::string volId = "123";
        bool result = dsm->Format(volId);
        EXPECT_FALSE(result);
        dsm->GetVolumes(volumeInfos);
        int count = volumeInfos.size();
        for (int i = 0; i < count; i++) {
            int32_t userid = volumeInfos[i]->GetMountUserId();
            cout << "mId:" << volumeInfos[i]->GetId() << endl;
            cout << "mDiskId:" << volumeInfos[i]->GetDiskId() << endl;
            cout << "mPartGuid:" << volumeInfos[i]->GetPartGuid() << endl;
            cout << "mFsUuid:" << volumeInfos[i]->GetFsUuid() << endl;
            cout << "mType:" << volumeInfos[i]->GetType() << endl;
            cout << "mMountFlags:" << volumeInfos[i]->GetMountFlags() << endl;
            cout << "mMountUserId:" << volumeInfos[i]->GetMountUserId() << endl;
            cout << "mState:" << volumeInfos[i]->GetState() << endl;
            cout << "mPath:" << volumeInfos[i]->GetPath() << endl;
            cout << "mInternalPath:" << volumeInfos[i]->GetInternalPath() << endl;
            cout << "mFsLabel:" << volumeInfos[i]->GetFsLabel() << endl;
            cout << "GetDescription:" << volumeInfos[i]->GetDescription() << endl;
            cout << "IsVisibleForUser:" << volumeInfos[i]->IsVisibleForUser(userid) << endl;
            cout << "IsEmulated:" << volumeInfos[i]->IsEmulated() << endl;
            cout << "IsPrimaryEmulatedForUser:" << volumeInfos[i]->IsPrimaryEmulatedForUser(userid) << endl;
            cout << "IsRemovable:" << volumeInfos[i]->IsRemovable(userid) << endl;
            cout << "IsPrimary:" << volumeInfos[i]->IsPrimary() << endl;
        }
        cout << "DeviceStorageManagerTest-end Parameter_Format" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_0600" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_0700
 * @tc.name DSM_ERROR_0700
 * @tc.desc Test function of FindVolumeById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_0700, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_0700" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::shared_ptr<DS::VolumeInfo> vol;
        std::string mId = "";
        bool isFindVolume = dsm->FindVolumeById(vol, mId);
        EXPECT_FALSE(isFindVolume);
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_0700" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_0800
 * @tc.name DSM_ERROR_0800
 * @tc.desc Test function of FindVolumeById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_0800, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_0800" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::shared_ptr<DS::VolumeInfo> vol;
        std::string mId = "1234";
        bool isFindVolumeById = dsm->FindVolumeById(vol, mId);
        EXPECT_FALSE(isFindVolumeById);
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_0800" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_0900
 * @tc.name DSM_ERROR_0900
 * @tc.desc Test function of FindVolumeByUuid interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_0900, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_0900" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::shared_ptr<DS::VolumeInfo> vol;
        std::string uuid = "";
        bool isFindVolume = dsm->FindVolumeByUuid(vol, uuid);
        EXPECT_TRUE(isFindVolume);
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeByUuid" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_0900" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_1000
 * @tc.name DSM_ERROR_1000
 * @tc.desc Test function of FindVolumeByUuid interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_1000, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_1000" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::shared_ptr<DS::VolumeInfo> vol;
        std::string uuid = "321";
        bool isFindVolume = dsm->FindVolumeByUuid(vol, uuid);
        EXPECT_FALSE(isFindVolume);
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_1000" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_1100
 * @tc.name DSM_ERROR_1100
 * @tc.desc Test function of GetWritableVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_1100, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_1100" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        bool isGetWritableVolumes = dsm->GetWritableVolumes(volumeInfos);
        EXPECT_FALSE(isGetWritableVolumes);
        cout << "DeviceStorageManagerTest-end Parameter_GetWritableVolumes" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_1100" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_1200
 * @tc.name DSM_ERROR_1200
 * @tc.desc Test function of GetWritableVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_1200, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_1200" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::DiskInfo>> diskInfos;
        std::shared_ptr<DS::DiskInfo> diskInfo;
        std::string mId = "";
        bool isFindDiskById = dsm->FindDiskById(diskInfo, mId);
        EXPECT_FALSE(isFindDiskById);
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_1200" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_1300
 * @tc.name DSM_ERROR_1300
 * @tc.desc Test function of FindDiskById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_1300, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_1300" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::DiskInfo>> diskInfos;
        std::shared_ptr<DS::DiskInfo> diskInfo;
        std::string mId = "12345";
        bool isFindDiskById = dsm->FindDiskById(diskInfo, mId);
        EXPECT_FALSE(isFindDiskById);
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_1300" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_1400
 * @tc.name DSM_ERROR_1400
 * @tc.desc Test function of FindPrivateForEmulate interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_1400, testing::ext::TestSize.Level1)
{
    cout << "begin SUB_STORAGE_DSM_ERROR_1400" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::shared_ptr<DS::VolumeInfo> priVol;
        std::shared_ptr<DS::VolumeInfo> emuVol;
        dsm->GetVolumes(volumeInfos);
        priVol = volumeInfos[1];
        bool isFindPrivate = dsm->FindPrivateForEmulate(emuVol, priVol);
        EXPECT_FALSE(isFindPrivate);
        cout << "DeviceStorageManagerTest-end Parameter_FindPrivateForEmulate" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end SUB_STORAGE_DSM_ERROR_1400" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_1500
 * @tc.name DSM_ERROR_1500
 * @tc.desc Test function of FindEmulateForPrivate interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_1500, testing::ext::TestSize.Level1)
{
    cout << "begin SUB_STORAGE_DSM_ERROR_1500" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
        std::shared_ptr<DS::VolumeInfo> priVol;
        std::shared_ptr<DS::VolumeInfo> emuVol;
        dsm->GetVolumes(volumeInfos);
        priVol = volumeInfos[0];
        bool isFindEmulateForPrivate = dsm->FindEmulateForPrivate(emuVol, priVol);
        EXPECT_FALSE(isFindEmulateForPrivate);
        cout << "DeviceStorageManagerTest-end Parameter_FindEmulateForPrivate" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_1500" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_1600
 * @tc.name DSM_ERROR_1600
 * @tc.desc Test function of GetBestVolumeDescription interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_1600, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_1600" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> VolumeInfos;
        std::shared_ptr<DS::VolumeInfo> VolumeInfo;
        std::string mId = "";
        std::string mDiskId = "";
        std::string mPartGuid = "";
        std::string mFsUuid = "";
        int32_t mType = 0;
        int32_t mMountFlags = 0;
        int32_t mMountUserId = 0;
        int32_t mState = 0;
        std::string mPath = "";
        std::string mInternalPath = "";
        dsm->GetVolumes(VolumeInfos);
        int count = VolumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = VolumeInfos[i]->GetId();
            std::string nId = "public";
            if (strstr(mId.c_str(), nId.c_str())) {
                VolumeInfos[i]->SetId(mId);
                VolumeInfos[i]->SetDiskId(mDiskId);
                VolumeInfos[i]->SetPartGuid(mPartGuid);
                VolumeInfos[i]->SetFsUuid(mFsUuid);
                VolumeInfos[i]->SetType(mType);
                VolumeInfos[i]->SetMountFlags(mMountFlags);
                VolumeInfos[i]->SetMountUserId(mMountUserId);
                VolumeInfos[i]->SetState(mState);
                VolumeInfos[i]->SetPath(mPath);
                VolumeInfos[i]->SetInternalPath(mInternalPath);
            }
        }
        VolumeInfo = VolumeInfos[sdcard];
        std::string description;
        bool isGetBestVolumeDescription = dsm->GetBestVolumeDescription(VolumeInfo, description);
        EXPECT_FALSE(isGetBestVolumeDescription);
        printf("description:%s\n", description.c_str());
        cout << "DeviceStorageManagerTest-end Parameter_GetBestVolumeDescriptio" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_1600" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_ERROR_1700
 * @tc.name DSM_ERROR_1700
 * @tc.desc Test function of GetBestVolumeDescription interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_ERROR_1700, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_ERROR_1700" << endl;
    try {
        std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
        std::vector<std::shared_ptr<DS::VolumeInfo>> VolumeInfos;
        std::shared_ptr<DS::VolumeInfo> VolumeInfo;
        std::string mId = "public:177,25";
        std::string mDiskId = "disk:189,24";
        std::string mPartGuid = "";
        std::string mFsUuid = "d87ba2fe-828f-4237-8006-c6dc6c2910e7";
        int32_t mType = 0;
        int32_t mMountFlags = 2;
        int32_t mMountUserId = 0;
        int32_t mState = 2;
        std::string mPath = "/storage/d87ba2fe-828f-4237-8006-c6dc6c2910e7";
        std::string mInternalPath = "/data/ss/d87ba2fe-828f-4237-8006-c6dc6c2910e7";
        dsm->GetVolumes(VolumeInfos);
        int count = VolumeInfos.size();
        for (int i = 0; i < count; i++) {
            std::string mId = VolumeInfos[i]->GetId();
            std::string nId = "public";
            if (strstr(mId.c_str(), nId.c_str())) {
                VolumeInfos[i]->SetId(mId);
                VolumeInfos[i]->SetDiskId(mDiskId);
                VolumeInfos[i]->SetPartGuid(mPartGuid);
                VolumeInfos[i]->SetFsUuid(mFsUuid);
                VolumeInfos[i]->SetType(mType);
                VolumeInfos[i]->SetMountFlags(mMountFlags);
                VolumeInfos[i]->SetMountUserId(mMountUserId);
                VolumeInfos[i]->SetState(mState);
                VolumeInfos[i]->SetPath(mPath);
                VolumeInfos[i]->SetInternalPath(mInternalPath);
            }
        }
        VolumeInfo = VolumeInfos[sdcard];
        std::string description;
        bool isGetBestVolumeDescription = dsm->GetBestVolumeDescription(VolumeInfo, description);
        EXPECT_FALSE(isGetBestVolumeDescription);
        printf("description:%s\n", description.c_str());
        cout << "DeviceStorageManagerTest-end Parameter_GetBestVolumeDescription" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_ERROR_1700" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_0000
 * @tc.name DSM_Performance_0000
 * @tc.desc Test function of GetVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_0000, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_0000" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> VolumeInfos;
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            bool result = dsm->GetVolumes(VolumeInfos);
            gettimeofday(&tv1, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "DSM_Performance_0000 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i << endl;
            EXPECT_TRUE(result);
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetVolumes" << endl;
    } catch (...) {
        EXPECT_NE(nullptr, nullptr);
        cout << "DeviceStorageManagerTest-an exception occurred." << endl;
    }
    cout << "end DSM_Performance_0000" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_0100
 * @tc.name DSM_Performance_0100
 * @tc.desc Test function of Mount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_0100, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_0100" << endl;
    try {
        for (int a = 0; a < performance; a++) {
            std::string mId1;
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int i = 0; i < count; i++) {
                std::string mId = volumeInfos[i]->GetId();
                std::string nId = "public";
                if (strstr(mId.c_str(), nId.c_str())) {
                    mId1 = mId;
                    dsm->UnMount(mId1);
                    struct timeval tv;
                    struct timeval tv1;
                    gettimeofday(&tv, nullptr);
                    bool result = dsm->Mount(mId1);
                    gettimeofday(&tv1, nullptr);
                    std::cout << tv.tv_usec << "us" << endl;
                    std::cout << tv1.tv_usec << "us" << endl;
                    std::cout << "DSM_Performance_0100 Time use:"
                              << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << a
                              << endl;
                    EXPECT_TRUE(result);
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Mount" << endl;
    } catch (...) {
        EXPECT_NE(nullptr, nullptr);
        cout << "DeviceStorageManagerTest-an exception occurred." << endl;
    }
    cout << "end DSM_Performance_0100" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_0200
 * @tc.name DSM_Performance_0200
 * @tc.desc Test function of Unmount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_0200, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_0200" << endl;
    try {
        for (int a = 0; a < performance; a++) {
            std::string mId1;
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int i = 0; i < count; i++) {
                std::string mId = volumeInfos[i]->GetId();
                std::string nId = "public";
                if (strstr(mId.c_str(), nId.c_str())) {
                    mId1 = mId;
                    struct timeval tv;
                    struct timeval tv1;
                    gettimeofday(&tv, nullptr);
                    bool result = dsm->UnMount(mId1);
                    gettimeofday(&tv1, nullptr);
                    std::cout << tv.tv_usec << "us" << endl;
                    std::cout << tv1.tv_usec << "us" << endl;
                    std::cout << "DSM_Performance_0200 Time use:"
                              << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << a
                              << endl;
                    EXPECT_TRUE(result);
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_UnMount" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_0200" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_0300
 * @tc.name DSM_Performance_0300
 * @tc.desc Test function of Format interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_0300, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_0300" << endl;
    try {
        for (int a = 0; a < performance; a++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::string mId1;
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int i = 0; i < count; i++) {
                std::string mId = volumeInfos[i]->GetId();
                std::string nId = "public";
                if (strstr(mId.c_str(), nId.c_str())) {
                    mId1 = mId;
                    struct timeval tv;
                    struct timeval tv1;
                    gettimeofday(&tv, nullptr);
                    bool result = dsm->Format(mId1);
                    gettimeofday(&tv1, nullptr);
                    std::cout << tv.tv_usec << "us" << endl;
                    std::cout << tv1.tv_usec << "us" << endl;
                    std::cout << "DSM_Performance_0300 Time use:"
                              << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << a
                              << endl;
                    EXPECT_TRUE(result);
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Format" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_0300" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_0400
 * @tc.name DSM_Performance_0400
 * @tc.desc Test function of GetDisks interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_0400, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_0400" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::DiskInfo>> diskInfos;
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            bool result = dsm->GetDisks(diskInfos);
            gettimeofday(&tv1, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "DSM_Performance_0400 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i << endl;
            EXPECT_TRUE(result);
        }
        cout << "DeviceStorageManagerTest-end Parameter_GetDisks" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_0400" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_0500
 * @tc.name DSM_Performance_0500
 * @tc.desc Test function of FindVolumeById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_0500, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_0500" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::shared_ptr<DS::VolumeInfo> vol;
            std::string nId = "public";
            struct timeval tv;
            struct timeval tv1;
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int a = 0; a < count; a++) {
                std::string mId = volumeInfos[a]->GetId();
                if (strstr(mId.c_str(), nId.c_str())) {
                    mId = volumeInfos[a]->GetId();
                    gettimeofday(&tv, nullptr);
                    bool isFindVolume = dsm->FindVolumeById(vol, mId);
                    gettimeofday(&tv1, nullptr);
                    std::cout << tv.tv_usec << "us" << endl;
                    std::cout << tv1.tv_usec << "us" << endl;
                    std::cout << "Performance_0500 Time use:"
                              << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i
                              << endl;
                    EXPECT_TRUE(isFindVolume);
                }
            }
            cout << "DeviceStorageManagerTest-end Parameter_FindVolumeById" << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_0500" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_0600
 * @tc.name DSM_Performance_0600
 * @tc.desc Test function of FindVolumeByUuid interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_0600, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_0600" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::shared_ptr<DS::VolumeInfo> vol;
            std::string nId = "public";
            struct timeval tv;
            struct timeval tv1;
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int a = 0; a < count; a++) {
                std::string mId = volumeInfos[a]->GetId();
                if (strstr(mId.c_str(), nId.c_str())) {
                    std::string uuid = volumeInfos[a]->GetFsUuid();
                    gettimeofday(&tv, nullptr);
                    bool isFindVolume = dsm->FindVolumeByUuid(vol, uuid);
                    gettimeofday(&tv1, nullptr);
                    std::cout << tv.tv_usec << "us" << endl;
                    std::cout << tv1.tv_usec << "us" << endl;
                    std::cout << "Performance_0600 Time use:"
                              << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i
                              << endl;
                    EXPECT_TRUE(isFindVolume);
                }
            }
            cout << "DeviceStorageManagerTest-end Parameter_FindVolumeByUuid" << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_0600" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_0700
 * @tc.name DSM_Performance_0700
 * @tc.desc Test function of GetWritableVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_0700, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_0700" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            bool isGetWritableVolumes = dsm->GetWritableVolumes(volumeInfos);
            gettimeofday(&tv1, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Performance_0700 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i << endl;
            EXPECT_TRUE(isGetWritableVolumes);
            cout << "DeviceStorageManagerTest-end Parameter_GetWritableVolumes" << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_0700" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_0800
 * @tc.name DSM_Performance_0800
 * @tc.desc Test function of FindDiskById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_0800, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_0800" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::DiskInfo>> diskInfos;
            std::shared_ptr<DS::DiskInfo> diskInfo;
            std::string nId = "disk";
            struct timeval tv;
            struct timeval tv1;
            dsm->GetDisks(diskInfos);
            int count1 = diskInfos.size();
            for (int i = 0; i < count1; i++) {
                std::string mId = diskInfos[i]->GetId();
                if (strstr(mId.c_str(), nId.c_str())) {
                    std::string mId = diskInfos[i]->GetId();
                    gettimeofday(&tv, nullptr);
                    bool isFindDiskById = dsm->FindDiskById(diskInfo, mId);
                    gettimeofday(&tv1, nullptr);
                    EXPECT_TRUE(isFindDiskById);
                    std::cout << tv.tv_usec << "us" << endl;
                    std::cout << tv1.tv_usec << "us" << endl;
                    std::cout << "Performance_0800 Time use:"
                              << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i
                              << endl;
                }
            }
            cout << "DeviceStorageManagerTest-end Parameter_FindDiskById" << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_0800" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_0900
 * @tc.name DSM_Performance_0900
 * @tc.desc Test function of GetPrimaryStorageUuid interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_0900, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_0900" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::string primaryUuid = "";
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            bool isGetPrimaryStorageUuid = dsm->GetPrimaryStorageUuid(primaryUuid);
            gettimeofday(&tv1, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Performance_0900 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i << endl;
            EXPECT_TRUE(isGetPrimaryStorageUuid);
            cout << "DeviceStorageManagerTest-end Parameter_GetPrimaryStorageUuid" << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_0900" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_1000
 * @tc.name DSM_Performance_1000
 * @tc.desc Test function of FindPrivateForEmulate interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_1000, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_1000" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::shared_ptr<DS::VolumeInfo> priVol;
            std::shared_ptr<DS::VolumeInfo> emuVol;
            struct timeval tv;
            struct timeval tv1;
            dsm->GetVolumes(volumeInfos);
            emuVol = volumeInfos[0];
            gettimeofday(&tv, nullptr);
            bool isFindPrivate = dsm->FindPrivateForEmulate(priVol, emuVol);
            gettimeofday(&tv1, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Performance_1000 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i << endl;
            EXPECT_TRUE(isFindPrivate);
            cout << "DeviceStorageManagerTest-end Parameter_FindPrivateForEmulate" << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_1000" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_1100
 * @tc.name DSM_Performance_1100
 * @tc.desc Test function of FindEmulateForPrivate interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_1100, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_1100" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::shared_ptr<DS::VolumeInfo> priVol;
            std::shared_ptr<DS::VolumeInfo> emuVol;
            struct timeval tv;
            struct timeval tv1;
            dsm->GetVolumes(volumeInfos);
            priVol = volumeInfos[1];
            gettimeofday(&tv, nullptr);
            bool isFindEmulateForPrivate = dsm->FindEmulateForPrivate(emuVol, priVol);
            gettimeofday(&tv1, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Performance_1100 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i << endl;
            EXPECT_TRUE(isFindEmulateForPrivate);
            cout << "DeviceStorageManagerTest-end Parameter_FindEmulateForPrivate" << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_1100" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_1200
 * @tc.name DSM_Performance_1200
 * @tc.desc Test function of GetBestVolumeDescription interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_1200, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_1200" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::string desCription;
            std::shared_ptr<DS::VolumeInfo> vol;
            struct timeval tv;
            struct timeval tv1;
            dsm->GetVolumes(volumeInfos);
            vol = volumeInfos[sdcard];
            gettimeofday(&tv, nullptr);
            bool isGetBestVolumeDescription = dsm->GetBestVolumeDescription(vol, desCription);
            gettimeofday(&tv1, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Performance_1200 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i << endl;
            EXPECT_TRUE(isGetBestVolumeDescription);
            cout << "DeviceStorageManagerTest-end Parameter_GetBestVolumeDescription" << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_1200" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_1300
 * @tc.name DSM_Performance_1300
 * @tc.desc Test function of Mount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_1300, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_1300" << endl;
    try {
        for (int a = 0; a < performance; a++) {
            std::string mId1;
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int i = 0; i < count; i++) {
                std::string mId = volumeInfos[i]->GetId();
                std::string nId = "public";
                if (strstr(mId.c_str(), nId.c_str())) {
                    mId1 = mId;
                    dsm->UnMount(mId1);
                    struct timeval tv;
                    struct timeval tv1;
                    gettimeofday(&tv, nullptr);
                    bool result = dsm->Mount(mId1);
                    gettimeofday(&tv1, nullptr);
                    std::cout << tv.tv_usec << "us" << endl;
                    std::cout << tv1.tv_usec << "us" << endl;
                    std::cout << "DSM_Performance_1300 Time use:"
                              << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << a
                              << endl;
                    EXPECT_TRUE(result);
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Mount" << endl;
    } catch (...) {
        EXPECT_NE(nullptr, nullptr);
        cout << "DeviceStorageManagerTest-an exception occurred." << endl;
    }
    cout << "end DSM_Performance_1300" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_1400
 * @tc.name DSM_Performance_1400
 * @tc.desc Test function of UnMount interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_1400, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_1400" << endl;
    try {
        for (int a = 0; a < performance; a++) {
            std::string mId1;
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int i = 0; i < count; i++) {
                std::string mId = volumeInfos[i]->GetId();
                std::string nId = "public";
                if (strstr(mId.c_str(), nId.c_str())) {
                    mId1 = mId;
                    struct timeval tv;
                    struct timeval tv1;
                    gettimeofday(&tv, nullptr);
                    bool result = dsm->UnMount(mId1);
                    gettimeofday(&tv1, nullptr);
                    std::cout << tv.tv_usec << "us" << endl;
                    std::cout << tv1.tv_usec << "us" << endl;
                    std::cout << "DSM_Performance_1400 Time use:"
                              << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << a
                              << endl;
                    EXPECT_TRUE(result);
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_UnMount" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_1400" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_1500
 * @tc.name DSM_Performance_1500
 * @tc.desc Test function of Format interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_1500, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_1500" << endl;
    try {
        for (int a = 0; a < performance; a++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::string mId1;
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int i = 0; i < count; i++) {
                std::string mId = volumeInfos[i]->GetId();
                std::string nId = "public";
                if (strstr(mId.c_str(), nId.c_str())) {
                    mId1 = mId;
                    struct timeval tv;
                    struct timeval tv1;
                    gettimeofday(&tv, nullptr);
                    bool result = dsm->Format(mId1);
                    gettimeofday(&tv1, nullptr);
                    std::cout << tv.tv_usec << "us" << endl;
                    std::cout << tv1.tv_usec << "us" << endl;
                    std::cout << "DSM_Performance_1500 Time use:"
                              << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << a
                              << endl;
                    EXPECT_TRUE(result);
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_Format" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_1500" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_1600
 * @tc.name DSM_Performance_1600
 * @tc.desc Test function of FindVolumeById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_1600, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_1600" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::shared_ptr<DS::VolumeInfo> vol;
            std::string nId = "public";
            struct timeval tv;
            struct timeval tv1;
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int a = 0; a < count; a++) {
                std::string mId = volumeInfos[a]->GetId();
                if (strstr(mId.c_str(), nId.c_str())) {
                    mId = volumeInfos[a]->GetId();
                    gettimeofday(&tv, nullptr);
                    bool isFindVolumeById = dsm->FindVolumeById(vol, mId);
                    gettimeofday(&tv1, nullptr);
                    std::cout << tv.tv_usec << "us" << endl;
                    std::cout << tv1.tv_usec << "us" << endl;
                    std::cout << "DSM_Performance_1600 Time use:"
                              << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i
                              << endl;
                    EXPECT_TRUE(isFindVolumeById);
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_1600" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_1700
 * @tc.name DSM_Performance_1700
 * @tc.desc Test function of FindVolumeByUuid interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_1700, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_1700" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::shared_ptr<DS::VolumeInfo> vol;
            std::string nId = "public";
            struct timeval tv;
            struct timeval tv1;
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int a = 0; a < count; a++) {
                std::string mId = volumeInfos[a]->GetId();
                if (strstr(mId.c_str(), nId.c_str())) {
                    std::string uuid = volumeInfos[a]->GetFsUuid();
                    cout << volumeInfos[a]->GetFsUuid() << endl;
                    gettimeofday(&tv, nullptr);
                    bool isFindVolumeByUuid = dsm->FindVolumeByUuid(vol, uuid);
                    gettimeofday(&tv1, nullptr);
                    std::cout << tv.tv_usec << "us" << endl;
                    std::cout << tv1.tv_usec << "us" << endl;
                    std::cout << "DSM_Performance_1700 Time use:"
                              << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i
                              << endl;
                    EXPECT_TRUE(isFindVolumeByUuid);
                }
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_FindVolumeByUuid" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_1700" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_1800
 * @tc.name DSM_Performance_1800
 * @tc.desc Test function of GetWritableVolumes interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_1800, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_1800" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            struct timeval tv;
            struct timeval tv1;
            dsm->GetVolumes(volumeInfos);
            gettimeofday(&tv, nullptr);
            bool isGetWritableVolumes = dsm->GetWritableVolumes(volumeInfos);
            gettimeofday(&tv1, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "DSM_Performance_1800 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i << endl;
            EXPECT_TRUE(isGetWritableVolumes);
            cout << "DeviceStorageManagerTest-end Parameter_GetWritableVolumes" << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_1800" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_1900
 * @tc.name DSM_Performance_1900
 * @tc.desc Test function of FindDiskById interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_1900, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_1900" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::DiskInfo>> diskInfos;
            std::shared_ptr<DS::DiskInfo> diskInfo;
            struct timeval tv;
            struct timeval tv1;
            dsm->GetDisks(diskInfos);
            diskInfo = diskInfos[sdcard];
            int count1 = diskInfos.size();
            for (int i = 0; i < count1; i++) {
                std::string mId = diskInfos[i]->GetId();
                gettimeofday(&tv, nullptr);
                bool isFindDiskById = dsm->FindDiskById(diskInfo, mId);
                gettimeofday(&tv1, nullptr);
                std::cout << tv.tv_usec << "us" << endl;
                std::cout << tv1.tv_usec << "us" << endl;
                std::cout << "DSM_Performance_1900 Time use:"
                          << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i
                          << endl;
                EXPECT_TRUE(isFindDiskById);
            }
        }
        cout << "DeviceStorageManagerTest-end Parameter_FindDiskById" << endl;
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_1900" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_Performance_2000
 * @tc.name DSM_Performance_2000
 * @tc.desc Test function of GetBestVolumeDescription interface.
 */
HWTEST_F(DeviceStorageManagerTest, DSM_Performance_2000, testing::ext::TestSize.Level1)
{
    cout << "begin DSM_Performance_2000" << endl;
    try {
        for (int i = 0; i < performance; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::string desCription;
            std::shared_ptr<DS::VolumeInfo> vol;
            struct timeval tv;
            struct timeval tv1;
            dsm->GetVolumes(volumeInfos);
            gettimeofday(&tv, nullptr);
            bool isGetBestVolumeDescription = dsm->GetBestVolumeDescription(vol, desCription);
            gettimeofday(&tv1, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "DSM_Performance_2000 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * timeus + (tv1.tv_usec - tv.tv_usec) << "us" << i << endl;
            EXPECT_TRUE(isGetBestVolumeDescription);
            cout << "DeviceStorageManagerTest-end Parameter_GetBestVolumeDescription" << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
    cout << "end DSM_Performance_2000" << endl;
}

/**
 * @tc.number SUB_STORAGE_DSM_STABILITY
 * @tc.name DSM_STABILITY
 * @tc.desc Test of STABILITY
 */
void StabilityGetvolumesDisks()
{
    int stability = 100000;
    try {
        for (int i = 0; i < stability; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::vector<std::shared_ptr<DS::DiskInfo>> diskInfos;
            bool result = dsm->GetVolumes(volumeInfos);
            EXPECT_TRUE(result);
            cout << "Getvolumes success_" << i << endl;
            bool result1 = dsm->GetDisks(diskInfos);
            EXPECT_TRUE(result1);
            cout << "GetDisks success_" << i << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
}

void StabilityUnmountMount()
{
    int stability = 100000;
    try {
        for (int i = 0; i < stability; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::string mId1;
            std::string nId = "public";
            int mountstate = 2;
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int i = 0; i < count; i++) {
                std::string mId = volumeInfos[i]->GetId();
                if (strstr(mId.c_str(), nId.c_str())) {
                    if (volumeInfos[i]->GetState() == mountstate) {
                        printf("Mount already\n");
                    } else {
                        printf("No sdcard Mounted\n");
                    }
                    mId1 = mId;
                    bool result = dsm->UnMount(mId1);
                    EXPECT_TRUE(result);
                    bool result1 = dsm->Mount(mId1);
                    EXPECT_TRUE(result1);
                }
            }
            cout << "Unmount success_" << i << endl;
            cout << "Mount success_" << i << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
}

void StabilityFindVolumeByIdUuId()
{
    int stability = 100000;
    try {
        for (int i = 0; i < stability; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::shared_ptr<DS::VolumeInfo> vol;
            std::string nId = "public";
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int a = 0; a < count; a++) {
                std::string mId = volumeInfos[a]->GetId();
                if (strstr(mId.c_str(), nId.c_str())) {
                    mId = volumeInfos[a]->GetId();
                    std::string uuid = volumeInfos[a]->GetFsUuid();
                    bool isFindVolumeById = dsm->FindVolumeById(vol, mId);
                    EXPECT_TRUE(isFindVolumeById);
                    bool isFindVolumeByUuid = dsm->FindVolumeByUuid(vol, uuid);
                    EXPECT_TRUE(isFindVolumeByUuid);
                }
            }
            cout << "FindVolumeById success_" << i << endl;
            cout << "FindVolumeByUuId success_" << i << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
}

void StabilityGetWritableFindDiskById()
{
    int stability = 100000;
    try {
        for (int i = 0; i < stability; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::string nId = "public";
            bool isGetWritableVolumes = dsm->GetWritableVolumes(volumeInfos);
            EXPECT_TRUE(isGetWritableVolumes);
            cout << "GetWritable success_" << i << endl;
        }
        for (int i = 0; i < stability; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::DiskInfo>> diskInfos;
            std::shared_ptr<DS::DiskInfo> diskInfo;
            std::string nId = "disk";
            dsm->GetDisks(diskInfos);
            int count1 = diskInfos.size();
            for (int i = 0; i < count1; i++) {
                std::string mId = diskInfos[i]->GetId();
                if (strstr(mId.c_str(), nId.c_str())) {
                    std::string mId = diskInfos[i]->GetId();
                    bool isFindDiskById = dsm->FindDiskById(diskInfo, mId);
                    EXPECT_TRUE(isFindDiskById);
                }
            }
            cout << "FindDiskById success_" << i << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
}

void StabilityFindprivateEmulate()
{
    int stability = 100000;
    try {
        for (int i = 0; i < stability; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::shared_ptr<DS::VolumeInfo> priVol;
            std::shared_ptr<DS::VolumeInfo> emuVol;
            dsm->GetVolumes(volumeInfos);
            priVol = volumeInfos[1];
            bool isFindEmulateForPrivate = dsm->FindEmulateForPrivate(emuVol, priVol);
            EXPECT_TRUE(isFindEmulateForPrivate);
            cout << "FindEmulateForPrivate success_" << i << endl;
        }
        for (int i = 0; i < stability; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::shared_ptr<DS::VolumeInfo> priVol;
            std::shared_ptr<DS::VolumeInfo> emuVol;
            dsm->GetVolumes(volumeInfos);
            emuVol = volumeInfos[0];
            bool isFindPrivateForEmulate = dsm->FindPrivateForEmulate(priVol, emuVol);
            EXPECT_TRUE(isFindPrivateForEmulate);
            cout << "FindPrivateForEmulate success_" << i << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
}

void StabilityGetDescriptionFormat()
{
    int stability = 100000;
    try {
        for (int i = 0; i < stability; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::string desCription;
            std::shared_ptr<DS::VolumeInfo> vol;
            dsm->GetVolumes(volumeInfos);
            vol = volumeInfos[0];
            bool isGetBestVolumeDescription = dsm->GetBestVolumeDescription(vol, desCription);
            EXPECT_TRUE(isGetBestVolumeDescription);
            cout << "GetDescription success_" << i << endl;
        }
        for (int i = 0; i < stability; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::vector<std::shared_ptr<DS::VolumeInfo>> volumeInfos;
            std::string nId = "public";
            dsm->GetVolumes(volumeInfos);
            int count = volumeInfos.size();
            for (int i = 0; i < count; i++) {
                std::string mId = volumeInfos[i]->GetId();
                if (strstr(mId.c_str(), nId.c_str())) {
                    std::string mId1 = mId;
                    bool result = dsm->Format(mId1);
                    EXPECT_TRUE(result);
                }
            }
            cout << "Format success_" << i << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
}

void StabilityGetPrimaryStorageUuid()
{
    int stability = 100000;
    try {
        for (int i = 0; i < stability; i++) {
            std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
            std::string primaryUuid;
            bool isGetUuid = dsm->GetPrimaryStorageUuid(primaryUuid);
            EXPECT_TRUE(isGetUuid);
            cout << "GetPrimaryStorageUuid success_" << i << endl;
        }
    } catch (...) {
        cout << "catch(...)" << endl;
    }
}

HWTEST_F(DeviceStorageManagerTest, Stability, testing::ext::TestSize.Level1)
{
    StabilityGetvolumesDisks();
    StabilityUnmountMount();
    StabilityFindVolumeByIdUuId();
    StabilityGetWritableFindDiskById();
    StabilityFindprivateEmulate();
    StabilityGetDescriptionFormat();
    StabilityGetPrimaryStorageUuid();
}
} 

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
#include "device_storage_data_mgr_test.h"

#include <csignal>
#include <cstring>
#include <iostream>
#include <refbase.h>
#include "device_storage_service.h"
#include "model/disk_info.h"
#include "model/volume_info.h"
using namespace testing::ext;
using namespace OHOS;
using namespace std;
std::shared_ptr<OHOS::DeviceStorageDataMgr> DeviceStorageDataMgrTest::dataMgr_ = nullptr;

namespace {
const std::string DISKID = "disk:179,24";
const int32_t DISKFLAG = 12;
const uint64_t DISKSIZE = 100;
const std::string DISKSIZELABEL = "sandisk";
const std::string DISKSYSPATH =
    "/sys//devices/platform/soc/100f0000.himci.SD/mmc_host/mmc1/mmc1:59b4/block/mmcblk1";
} // namespace

void DeviceStorageDataMgrTest::SetUpTestCase(void)
{
    if (dataMgr_ == nullptr) {
        std::shared_ptr<DeviceStorageService> service = std::make_shared<DeviceStorageService>();
        service->OnStart();
        dataMgr_ = service->GetDeviceStorageDataMgr();
    }
}

void DeviceStorageDataMgrTest::TearDownTestCase(void) {}

void DeviceStorageDataMgrTest::SetUp(void) {}

void DeviceStorageDataMgrTest::TearDown(void) {}

/**
 * @tc.name: DeviceStorageDataMgr01
 * @tc.desc: Test DeviceStorageDataMgr add disk.
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStorageDataMgrTest, DeviceStorageDataMgr001, TestSize.Level0)
{
    bool ret1 = dataMgr_->AddDiskInfo(DISKID, DISKFLAG);
    EXPECT_TRUE(ret1);
}

/**
 * @tc.name: DeviceStorageDataMgr02
 * @tc.desc: Test DeviceStorageDataMgr SetMetaDataforDisk.
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStorageDataMgrTest, DeviceStorageDataMgr002, TestSize.Level0)
{
    bool ret1 = dataMgr_->SetMetaDataforDisk(DISKID, DISKSIZE, DISKSIZELABEL, DISKSYSPATH);
    EXPECT_TRUE(ret1);
}

/**
 * @tc.name: DeviceStorageDataMgr03
 * @tc.desc: Test DeviceStorageDataMgr DeleteDiskInfo.
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStorageDataMgrTest, DeviceStorageDataMgr003, TestSize.Level0)
{
    bool ret1 = dataMgr_->DeleteDiskInfo(DISKID);
    EXPECT_TRUE(ret1);
}

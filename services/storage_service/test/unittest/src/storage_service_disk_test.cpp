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
#include "storage_service_disk_test.h"

#include <csignal>
#include <cstring>
#include <iostream>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <refbase.h>
#include <sys/sysmacros.h>
#include <system_ability_definition.h>

#include "storage_disk.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::StorageService;
using namespace std;

void StorageServiceDiskTest::SetUpTestCase(void) {}
void StorageServiceDiskTest::TearDownTestCase(void) {}
void StorageServiceDiskTest::SetUp(void) {}
void StorageServiceDiskTest::TearDown(void) {}

/**
 * @tc.name: StorageServiceDisk001
 * @tc.desc: Test StorageServiceDisk Create
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceDiskTest, StorageServiceDisk001, TestSize.Level0)
{
    if (true) {
        dev_t device = makedev(179, 24);
        auto disk = new OHOS::StorageService::Disk("virtual", device, "virtual",
                                                   OHOS::StorageService::Disk::Flags::diskFlagIsAdoptable |
                                                   OHOS::StorageService::Disk::Flags::diskFlagIsSd);
        ASSERT_TRUE(disk->Create()) << "DiskService Start to Create disk is Ok";
    }
}

/**
 * @tc.name: StorageServiceDisk002
 * @tc.desc: Test StorageServiceDisk Destroy
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceDiskTest, StorageServiceDisk002, TestSize.Level0)
{
    if (true) {
        dev_t device = makedev(179, 24);
        auto disk = new OHOS::StorageService::Disk("virtual", device, "virtual",
                                                   OHOS::StorageService::Disk::Flags::diskFlagIsAdoptable |
                                                   OHOS::StorageService::Disk::Flags::diskFlagIsSd);
        ASSERT_TRUE(disk->Destroy()) << "DiskService Start to Destroy disk is Ok";
    }
}

/**
 * @tc.name: StorageServiceDisk003
 * @tc.desc: Test StorageServiceDisk DiskReadMetadata
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceDiskTest, StorageServiceDisk003, TestSize.Level0)
{
    if (true) {
        dev_t device = makedev(179, 24);
        auto disk = new OHOS::StorageService::Disk("virtual", device, "virtual",
                                                   OHOS::StorageService::Disk::Flags::diskFlagIsAdoptable |
                                                   OHOS::StorageService::Disk::Flags::diskFlagIsSd);
        ASSERT_TRUE(disk->DiskReadMetadata()) << "DiskService Start to DiskReadMetadata disk is Ok";
    }
}

/**
 * @tc.name: StorageServiceDisk004
 * @tc.desc: Test StorageServiceDisk ReadPartitions
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceDiskTest, StorageServiceDisk004, TestSize.Level0)
{
    if (true) {
        dev_t device = makedev(179, 24);
        auto disk = new OHOS::StorageService::Disk("virtual", device, "virtual",
                                                   OHOS::StorageService::Disk::Flags::diskFlagIsAdoptable |
                                                   OHOS::StorageService::Disk::Flags::diskFlagIsSd);
        ASSERT_TRUE(disk->ReadPartitions()) << "DiskService Start to ReadPartitions disk is Ok";
    }
}

/**
 * @tc.name: StorageServiceDisk005
 * @tc.desc: Test StorageServiceDisk PartitionHandle
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceDiskTest, StorageServiceDisk005, TestSize.Level0)
{
    if (true) {
        dev_t device = makedev(179, 24);
        auto disk = new OHOS::StorageService::Disk("virtual", device, "virtual",
                                                   OHOS::StorageService::Disk::Flags::diskFlagIsAdoptable |
                                                   OHOS::StorageService::Disk::Flags::diskFlagIsSd);
        ASSERT_TRUE(disk->PartitionHandle(1, 0)) << "DiskService Start to PartitionHandle disk mbr is Ok";
        ASSERT_TRUE(disk->PartitionHandle(2, 0)) << "DiskService Start to PartitionHandle disk gpt is Ok";
    }
}
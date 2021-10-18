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

#include "storage_service_base_test.h"
#include <csignal>
#include <cstring>
#include <iostream>
#include <refbase.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <sys/sysmacros.h>
#include <system_ability_definition.h>
#include "storage_base.h"
#include "storage_manager.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::StorageService;
using namespace std;

void StorageServiceBaseTest::SetUpTestCase(void) {}
void StorageServiceBaseTest::TearDownTestCase(void) {}
void StorageServiceBaseTest::SetUp(void) {}
void StorageServiceBaseTest::TearDown(void) {}

/**
 * @tc.name: StorageServiceBase001
 * @tc.desc: Test StorageServiceBase Create
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceBaseTest, StorageServiceBase001, TestSize.Level0)
{
    if (true) {
        auto vol = StorageManager::Instance()->FindVolume("public:179,25");
        ASSERT_TRUE(vol->Create()) << "BaseService Start to Create is Ok";
    }
}

/**
 * @tc.name: StorageServiceBase002
 * @tc.desc: Test StorageServiceBase Mount
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceBaseTest, StorageServiceBase002, TestSize.Level0)
{
    if (true) {
        auto vol = StorageManager::Instance()->FindVolume("public:179,25");
        ASSERT_TRUE(vol->Mount()) << "BaseService Start to Mount is Ok";
    }
}

/**
 * @tc.name: StorageServiceBase003
 * @tc.desc: Test StorageServiceBase UnMount
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceBaseTest, StorageServiceBase003, TestSize.Level0)
{
    if (true) {
        auto vol = StorageManager::Instance()->FindVolume("public:179,25");
        ASSERT_TRUE(vol->UnMount()) << "BaseService Start to UnMount is Ok";
    }
}

/**
 * @tc.name: StorageServiceBase004
 * @tc.desc: Test StorageServiceBase Destroy
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceBaseTest, StorageServiceBase004, TestSize.Level0)
{
    if (true) {
        auto vol = StorageManager::Instance()->FindVolume("public:179,25");
        ASSERT_TRUE(vol->Destroy()) << "BaseService Start to Destroy is Ok";
    }
}

/**
 * @tc.name: StorageServiceBase005
 * @tc.desc: Test StorageServiceBase Format
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceBaseTest, StorageServiceBase005, TestSize.Level0)
{
    if (true) {
        auto vol = StorageManager::Instance()->FindVolume("public:179,25");
        ASSERT_TRUE(vol->Format("ext4")) << "BaseService Start to Format ext4 is Ok";
        ASSERT_TRUE(vol->Format("f2fs")) << "BaseService Start to Format f2fs is Ok";
        ASSERT_TRUE(vol->Format("ntfs")) << "BaseService Start to Format ntfs is Ok";
        ASSERT_TRUE(vol->Format("exfat")) << "BaseService Start to Format exfat is Ok";
        ASSERT_TRUE(vol->Format("vfat")) << "BaseService Start to Format vfat is Ok";
    }
}
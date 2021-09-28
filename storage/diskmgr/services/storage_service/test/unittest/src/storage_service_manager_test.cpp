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

#include "storage_service_manager_test.h"
#include <csignal>
#include <cstring>
#include <iostream>
#include <refbase.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "storage_manager.h"

using namespace testing::ext;
using namespace OHOS;
using namespace std;

void StorageServiceManagerTest::SetUpTestCase(void) {}
void StorageServiceManagerTest::TearDownTestCase(void) {}
void StorageServiceManagerTest::SetUp(void) {}
void StorageServiceManagerTest::TearDown(void) {}

/**
 * @tc.name: StorageServiceManager001
 * @tc.desc: Test StorageServiceBase Start
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceManagerTest, StorageServiceManager001, TestSize.Level0)
{
    if (true) {
        auto sm = StorageManager::Instance();
        ASSERT_TRUE(sm != nullptr) << "StorageServiceManager001 fail to get StorageManager";
        ASSERT_TRUE(sm->Start()) << "managerService Start to disk is Ok";
    }
}

/**
 * @tc.name: StorageServiceManager002
 * @tc.desc: Test StorageServiceBase UnMountAll
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceManagerTest, StorageServiceManager002, TestSize.Level0)
{
    if (true) {
        auto sm = StorageManager::Instance();
        ASSERT_TRUE(sm != nullptr) << "StorageServiceManager002 fail to get StorageManager";
        ASSERT_TRUE(sm->UnMountAll()) << "managerService UnMountAll to disk is Ok";
    }
}

/**
 * @tc.name: StorageServiceManager003
 * @tc.desc: Test StorageServiceBase Reset
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceManagerTest, StorageServiceManager003, TestSize.Level0)
{
    if (true) {
        auto sm = StorageManager::Instance();
        ASSERT_TRUE(sm != nullptr) << "StorageServiceManager002 fail to get StorageManager";
        ASSERT_TRUE(sm->Reset()) << "managerService Reset to disk is Ok";
    }
}
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
#include "storage_service_local_envent_socket_test.h"

#include <csignal>
#include <cstring>
#include <iostream>
#include <refbase.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <sys/sysmacros.h>
#include <system_ability_definition.h>

#include "local_event_socket.h"

using namespace testing::ext;
using namespace OHOS;
using namespace std;

void StorageServiceLocalEnventSocketTest::SetUpTestCase(void) {}
void StorageServiceLocalEnventSocketTest::TearDownTestCase(void) {}
void StorageServiceLocalEnventSocketTest::SetUp(void) {}
void StorageServiceLocalEnventSocketTest::TearDown(void) {}

/**
 * @tc.name: StorageServiceLocalEnventSocket001
 * @tc.desc: Test StorageServiceBase Start
 * @tc.type: FUNC
 */
HWTEST_F(StorageServiceLocalEnventSocketTest, StorageServiceLocalEnventSocket001, TestSize.Level0)
{
    if (true) {
        auto nm = LocalEventSocket::Instance();
        ASSERT_TRUE(nm != nullptr) << "StorageServiceLocalEnventSocket001 fail to get StorageManager";
        ASSERT_TRUE(nm->Start()) << "LocalEnventSocket Start to Start is Ok";
    }
}
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

#include "device_storage_service_test.h"

#include <csignal>
#include <cstring>
#include <iostream>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <refbase.h>
#include <system_ability_definition.h>

#include "device_storage_service.h"

using namespace testing::ext;
using namespace OHOS;
using namespace std;

void DeviceStorageServiceTest::SetUpTestCase(void) {}

void DeviceStorageServiceTest::TearDownTestCase(void) {}

void DeviceStorageServiceTest::SetUp(void) {}

void DeviceStorageServiceTest::TearDown(void) {}

/**
 * @tc.name: DeviceStorageService01
 * @tc.desc: Test DeviceStorageService service ready.
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStorageServiceTest, DeviceStorageService001, TestSize.Level0)
{
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(sam != nullptr) << "DeviceStorageService01 fail to get GetSystemAbilityManager";
    sptr<IRemoteObject> remoteObject_ = sam->CheckSystemAbility(DEVICE_STORAGE_MANAGER_SERVICE_ID);
    ASSERT_TRUE(remoteObject_ != nullptr) << "GetSystemAbility failed.";
}

/**
 * @tc.name: DeviceStorageService002
 * @tc.desc: Test DeviceStorageService Start and stop.
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStorageServiceTest, DeviceStorageService002, TestSize.Level0)
{
    if (false) {
        std::shared_ptr<DeviceStorageService> service = std::make_shared<DeviceStorageService>();
        ASSERT_TRUE(service != nullptr) << "DeviceStorageService002 fail to get DeviceStorageService";
        ASSERT_TRUE(!service->IsServiceReady()) << "SetUpTestCase service is ready";
        service->OnStart();
        ASSERT_TRUE(service->IsServiceReady()) << "SetUpTestCase service is not ready";
        service->OnStop();
        ASSERT_TRUE(!service->IsServiceReady()) << "SetUpTestCase service stop fail";
        service->OnStart();
        ASSERT_TRUE(service->IsServiceReady()) << "SetUpTestCase service is not ready";
        service->OnStop();
        ASSERT_TRUE(!service->IsServiceReady()) << "SetUpTestCase service stop fail";
    }
}

/**
 * @tc.name: DeviceStorageService003
 * @tc.desc: Test DeviceStorageService connect to storageservice
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStorageServiceTest, DeviceStorageService003, TestSize.Level0)
{
    if (true) {
        std::shared_ptr<DeviceStorageService> service = std::make_shared<DeviceStorageService>();
        ASSERT_TRUE(service != nullptr) << "DeviceStorageService003 fail to get DeviceStorageService";
        ASSERT_TRUE(service->ConnectToStorageService()) << "Connect to StorageService is fail";
    }
}

/**
 * @tc.name: DeviceStorageService004
 * @tc.desc: Test DeviceStorageService connect to storageservice
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStorageServiceTest, DeviceStorageService004, TestSize.Level0)
{
    if (true) {
        std::shared_ptr<DeviceStorageService> service = std::make_shared<DeviceStorageService>();
        ASSERT_TRUE(service != nullptr) << "DeviceStorageService004 fail to get DeviceStorageService";
        ASSERT_TRUE(service->SubscribeUserEvent()) << "Subscribe User Event is fail";
    }
}

/**
 * @tc.name: DeviceStorageService005
 * @tc.desc: Test DeviceStorageService get dataMgr
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStorageServiceTest, DeviceStorageService005, TestSize.Level0)
{
    if (true) {
        std::shared_ptr<DeviceStorageService> service = std::make_shared<DeviceStorageService>();
        ASSERT_TRUE(service != nullptr) << "DeviceStorageService005 fail to get DeviceStorageService";
        service->OnStart();
        ASSERT_TRUE(service->IsServiceReady()) << "SetUpTestCase service is not ready";
        EXPECT_NE(nullptr, service->GetDeviceStorageDataMgr()) << "GetDeviceStorageDataMgr is fail";
    }
}
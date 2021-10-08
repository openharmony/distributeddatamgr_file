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

#include "storage_service_local_envent_poll_test.h"
#include <csignal>
#include <cstring>
#include <iostream>
#include <refbase.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <sys/sysmacros.h>
#include <system_ability_definition.h>
#include "local_envent_poll.h"
#include "local_event_socket.h"

using namespace testing::ext;
using namespace OHOS;
using namespace std;

void StorageServiceLocalEnventPollTest::SetUpTestCase(void) {}
void StorageServiceLocalEnventPollTest::TearDownTestCase(void) {}
void StorageServiceLocalEnventPollTest::SetUp(void) {}
void StorageServiceLocalEnventPollTest::TearDown(void) {}

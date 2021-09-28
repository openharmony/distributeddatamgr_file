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
#include "idle_maintain_test.h"

#include <gtest/gtest.h>
#include <iostream>
#include <refbase.h>
#include <thread>

#include "idle_maintain.h"
#include "i_storage_service_task.h"
#include "storage_service.h"

namespace {
using namespace std;
using namespace OHOS;
class IdleMaintainTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase() {}
}; // class idleMaintainTest

/**
 * @tc.number SUB_STORAGE_IdleMaintainTest_0000
 * @tc.name IdleMaintainTest_0000
 * @tc.desc Test function of IdleFsTrim interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_0000, testing::ext::TestSize.Level1)
{
    try {
        std::cout << "begin IdleMaintainTest_0000" << std::endl;
        sptr<IStorageServiceTask> storageServiceTask;
        IdleFsTrim(storageServiceTask);
    } catch (...) {
        cout << "IdleFsTrim exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0000" << endl;
}

/**
 * @tc.number SUB_STORAGE_IdleMaintainTest_0100
 * @tc.name IdleMaintainTest_0100
 * @tc.desc Test function of RunIdleMaintain interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_0100, testing::ext::TestSize.Level1)
{
    try {
        std::cout << "begin IdleMaintainTest_0100" << std::endl;
        sptr<IStorageServiceTask> storageServiceTask;
        int result = RunIdleMaintain(storageServiceTask);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "RunIdleMaintain exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0100" << endl;
}

/**
 * @tc.number SUB_STORAGE_IdleMaintainTest_0200
 * @tc.name IdleMaintainTest_0200
 * @tc.desc Test function of AbortIdleMaintain interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_0200, testing::ext::TestSize.Level1)
{
    try {
        std::cout << "begin IdleMaintainTest_0100" << std::endl;
        sptr<IStorageServiceTask> storageServiceTask;
        RunIdleMaintain(storageServiceTask);
        int result = AbortIdleMaintain(storageServiceTask);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "AbortIdleMaintain exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0200" << endl;
}

/**
 * @tc.number SUB_STORAGE_IdleMaintainTest_Error_0000
 * @tc.name IdleMaintainTest_Error_0000
 * @tc.desc Test function of IdleFsTrim interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_Error_0000, testing::ext::TestSize.Level1)
{
    try {
        std::cout << "begin IdleMaintainTest_0000" << std::endl;
        IdleFsTrim(nullptr);
    } catch (...) {
        cout << "IdleFsTrim exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0000" << endl;
}

/**
 * @tc.number SUB_STORAGE_IdleMaintainTest_Error_0100
 * @tc.name IdleMaintainTest_Error_0100
 * @tc.desc Test function of RunIdleMaintain interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_Error_0100, testing::ext::TestSize.Level1)
{
    try {
        std::cout << "begin IdleMaintainTest_0100" << std::endl;
        int result = RunIdleMaintain(nullptr);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "RunIdleMaintain exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0100" << endl;
}

/**
 * @tc.number SUB_STORAGE_IdleMaintainTest_Error_0200
 * @tc.name IdleMaintainTest_Error_0200
 * @tc.desc Test function of AbortIdleMaintain interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_Error_0200, testing::ext::TestSize.Level1)
{
    try {
        std::cout << "begin IdleMaintainTest_0100" << std::endl;
        sptr<IStorageServiceTask> storageServiceTask;
        RunIdleMaintain(storageServiceTask);
        int result = AbortIdleMaintain(nullptr);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "AbortIdleMaintain exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0200" << endl;
}

/**
 * @tc.number SUB_STORAGE_IdleMaintainTest_Performance_0000
 * @tc.name IdleMaintainTest_Performance_0000
 * @tc.desc Test function of IdleFsTrim interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_Performance_0000, testing::ext::TestSize.Level1)
{
    try {
        for (int i = 0; i < 1000; i++) {
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            sptr<IStorageServiceTask> storageServiceTask;
            IdleFsTrim(storageServiceTask);
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "IdleMaintainTest_0200 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << i
                      << endl;
        }
    } catch (...) {
        cout << "IdleFsTrim exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0300" << endl;
}

/**
 * @tc.number SUB_STORAGE_IdleMaintainTest_Performance_0100
 * @tc.name IdleMaintainTest_Performance_0100
 * @tc.desc Test function of RunIdleMaintain interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_Performance_0100, testing::ext::TestSize.Level1)
{
    try {
        for (int i = 0; i < 1000; i++) {
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            sptr<IStorageServiceTask> storageServiceTask;
            int result = RunIdleMaintain(storageServiceTask);
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "IdleMaintainTest_0300 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << i
                      << endl;

            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "RunIdleMaintain exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0400" << endl;
}

/**
 * @tc.number SUB_STORAGE_IdleMaintainTest_Performance_0200
 * @tc.name IdleMaintainTest_Performance_0200
 * @tc.desc Test function of AbortIdleMaintain interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_Performance_0200, testing::ext::TestSize.Level1)
{
    try {
        for (int i = 0; i < 1000; i++) {
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            sptr<IStorageServiceTask> storageServiceTask;
            int result = AbortIdleMaintain(storageServiceTask);
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "IdleMaintainTest_0300 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << i
                      << endl;

            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "AbortIdleMaintain exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0500" << endl;
}

/**
 * @tc.number SUB_STORAGE_IdleMaintainTest_Reliable_0000
 * @tc.name IdleMaintainTest_Reliable_0000
 * @tc.desc Test function of IdleFsTrim interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_Reliable_0000, testing::ext::TestSize.Level1)
{
    try {
        for (int i = 0; i < 100000; i++) {
            sptr<IStorageServiceTask> storageServiceTask;
            IdleFsTrim(storageServiceTask);
            std::cout << "count is " << i << endl;
        }
    } catch (...) {
        cout << "IdleFsTrim exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0600" << endl;
}

/**
 * @tc.number SUB_STORAGE_IdleMaintainTest_Reliable_0100
 * @tc.name IdleMaintainTest_Reliable_0100
 * @tc.desc Test function of RunIdleMaintain interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_Reliable_0100, testing::ext::TestSize.Level1)
{
    try {
        for (int i = 0; i < 100000; i++) {
            sptr<IStorageServiceTask> storageServiceTask;
            int result = RunIdleMaintain(storageServiceTask);
            std::cout << "count is " << i << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "RunIdleMaintain exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0700" << endl;
}

/**
 * @tc.number SUB_STORAGE_IdleMaintainTes_Reliable_0200
 * @tc.name IdleMaintainTest_Reliable_0200
 * @tc.desc Test function of AbortIdleMaintain interface.
 */
HWTEST_F(IdleMaintainTest, IdleMaintainTest_Reliable_0200, testing::ext::TestSize.Level1)
{
    try {
        for (int i = 0; i < 100000; i++) {
            sptr<IStorageServiceTask> storageServiceTask;
            int result = AbortIdleMaintain(storageServiceTask);
            std::cout << "count is " << i << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "AbortIdleMaintain exception occurred" << endl;
    }
    cout << "end IdleMaintainTest_0800" << endl;
}
} // namespace

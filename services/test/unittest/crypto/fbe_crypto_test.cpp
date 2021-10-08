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
#include "fbe_crypto_test.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <gtest/gtest.h>
#include <iostream>
#include <iservice_registry.h>
#include <refbase.h>
#include <system_ability_definition.h>
#include <sys/time.h>
#include <unistd.h>

#include "fbe_crypto.h"

namespace {
using namespace std;
using namespace OHOS;
class FbeCryptoTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "FBE encryption code test" << endl;
    }
    static void TearDownTestCase() {}
};

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Function_Enable_0000
 * @tc.name Fbe_crypto_Function_Enable_0000
 * @tc.desc Test function of Enable interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_Enable_0000, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_Enable_0000" << endl;
    try {
        int32_t result = FbeCrypto::Enable();
        printf("the result of Enable is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_Enable_0000" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Function_Enable_0100
 * @tc.name Fbe_crypto_Function_Initialize_0100
 * @tc.desc Test function of Initialize interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_Initialize_0100, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_Initialize_0100" << endl;
    try {
        FbeCrypto::Enable();
        int32_t result = FbeCrypto::Initialize();
        printf("the result of Initialize is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_Initialize_0100" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Function_CreateKey_0200
 * @tc.name Fbe_crypto_Function_CreateKey_0200
 * @tc.desc Test function of CreateKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_CreateKey_0200, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_CreateKey_0200" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        int32_t result = FbeCrypto::CreateKey(1);
        printf("the result of Initialize is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_CreateKey_0200" << endl;
}

HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_CreateKey_0250, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_CreateKey_0200" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        int32_t result = FbeCrypto::CreateKey(3);
        printf("the result of Initialize is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_CreateKey_0200" << endl;
}

HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_CreateKey_0300, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_CreateKey_0200" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        int32_t result = FbeCrypto::CreateKey(3);
        printf("the result of Initialize is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_CreateKey_0200" << endl;
}

HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_CreateKey_0350, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_CreateKey_0350" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        int32_t result = FbeCrypto::CreateKey(1);
        printf("the result of Initialize is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_CreateKey_0350" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Function_DeleteKey_0400
 * @tc.name Fbe_crypto_Function_DeleteKey_0400
 * @tc.desc Test function of DeleteKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_DeleteKey_0400, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_DeleteKey_0400" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(4);
        int32_t result = FbeCrypto::DeleteKey(4);
        printf("the result of Initialize is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_DeleteKey_0400" << endl;
}

HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_DeleteKey_0450, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_DeleteKey_0450" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        int32_t result = FbeCrypto::DeleteKey(1);
        printf("the result of Initialize is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_DeleteKey_0450" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Function_AddAuthKey_0500
 * @tc.name Fbe_crypto_Function_AddAuthKey_0500
 * @tc.desc Test function of AddAuthKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_AddAuthKey_0500, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_AddAuthKey_0500" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(5);
        int32_t result = FbeCrypto::AddAuthKey(5, "", "123");
        printf("the result of AddAuthKey is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_AddAuthKey_0500" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Function_DelAuthKey_0600
 * @tc.name Fbe_crypto_Function_DelAuthKey_0600
 * @tc.desc Test function of AddAuthKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_DelAuthKey_0600, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_DelAuthKey_0600" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        int32_t result = FbeCrypto::DelAuthKey(5, "", "123");
        printf("the result of AddAuthKey is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_DelAuthKey_0600" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Function_UnLockKey_0700
 * @tc.name Fbe_crypto_Function_UnLockKey_0700
 * @tc.desc Test function of UnLockKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_UnLockKey_0700, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_UnLockKey_0700" << endl;
    try {
        int32_t result = FbeCrypto::UnLockKey(0, "", "");
        printf("the result of UnlockKey is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_UnLockKey_0700" << endl;
}

HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_UnlockKey_0800, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_UnlockKey_0800" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(6);
        FbeCrypto::InitUserSpace("", 6, 1);
        FbeCrypto::InitUserSpace("", 6, 2);
        FbeCrypto::AddAuthKey(6, "", "123");
        std::vector<struct CryptoPath> zpaths = {
            { "/data/system/usc/6/crypto.cpp", 1000, 1000, 0770, false },
            { "/data/system/usd/6/crypto.cpp", 1000, 1000, 0770, false }
        };
        for (auto path : zpaths) {
            if (path.IsExist()) {
                continue;
            }
            path.Create();
        }
        FbeCrypto::LockKey(6);
        int32_t result = FbeCrypto::UnLockKey(6, "", "123");
        printf("the result of UnlockKey is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_UnlockKey_0800" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Function_LockKey_0900
 * @tc.name Fbe_crypto_Function_LockKey_0900
 * @tc.desc Test function of LockKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_LockKey_0900, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_LockKey_0900" << endl;
    try {
        int32_t result = FbeCrypto::LockKey(2);
        printf("the result of LockKey is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_LockKey_0900" << endl;
}

HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_LockKey_1000, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_LockKey_1000" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::InitUserSpace("", 0, 2);
        std::vector<struct CryptoPath> zpaths = {
            { "/data/system/usc/0/crypto.cpp", 1000, 1000, 0770, false },
            { "/data/system/usd/0/crypto.cpp", 1000, 1000, 0770, false }
        };
        for (auto path : zpaths) {
            if (path.IsExist()) {
                continue;
            }
            path.Create();
        }
        int32_t result = FbeCrypto::LockKey(0);
        printf("the result of LockKey is %d\n", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_LockKey_1000" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Function_InitUserSpace_1100
 * @tc.name Fbe_crypto_Function_InitUserSpace_1100
 * @tc.desc Test function of nitUserSpace interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_InitUserSpace_1100, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_InitUserSpace_1100" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(7);
        int32_t result = FbeCrypto::InitUserSpace("", 7, 1);
        printf("the result of InitUserSpace is %d", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_InitUserSpace_1100" << endl;
}

HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_InitUserSpace_1200, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_InitUserSpace_1200" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        int32_t result = FbeCrypto::InitUserSpace("", 7, 2);
        printf("the result of Enable is %d", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_InitUserSpace_1200" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Function_RemoveUserSpace_1300
 * @tc.name Fbe_crypto_Function_RemoveUserSpace_1300
 * @tc.desc Test function of RemoveUserSpace interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_RemoveUserSpace_1300, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_RemoveUserSpace_1300" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        int32_t result = FbeCrypto::RemoveUserSpace("", 7, 1);
        printf("the result of Enable is %d", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_RemoveUserSpace_1300" << endl;
}

HWTEST_F(FbeCryptoTest, Fbe_crypto_Function_RemoveUserSpace_1400, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Function_RemoveUserSpace_1400" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        int32_t result = FbeCrypto::RemoveUserSpace("", 7, 2);
        printf("the result of Enable is %d", result);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Function_RemoveUserSpace_1400" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Test_Error_0100
 * @tc.name Fbe_crypto_Test_Error_0100
 * @tc.desc Test function of DelAuthKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Test_Error_0100, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Test_Error_0100" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(15);
        FbeCrypto::AddAuthKey(15, "", "123");
        int32_t result = FbeCrypto::DelAuthKey(15, "", "234");
        printf("the result of Enable is %d", result);
        EXPECT_NE(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Test_Error_0100" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Test_Error_0200
 * @tc.name Fbe_crypto_Test_Error_0200
 * @tc.desc Test function of UnLockKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Test_Error_0200, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Test_Error_0200" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(16);
        FbeCrypto::InitUserSpace("", 16, 1);
        FbeCrypto::InitUserSpace("", 16, 2);
        std::vector<struct CryptoPath> zpaths = {
            { "/data/system/usc/6/crypto.cpp", 1000, 1000, 0770, false },
            { "/data/system/usd/6/crypto.cpp", 1000, 1000, 0770, false }
        };
        for (auto path : zpaths) {
            if (path.IsExist()) {
                continue;
            }
            path.Create();
        }
        FbeCrypto::AddAuthKey(16, "", "123");
        FbeCrypto::LockKey(16);
        int32_t result = FbeCrypto::UnLockKey(16, "", "");
        printf("the result of Enable is %d", result);
        EXPECT_NE(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Test_Error_0200" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Test_Error_0300
 * @tc.name Fbe_crypto_Test_Error_0300
 * @tc.desc Test function of InitUserSpace interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Test_Error_0300, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Test_Error_0300" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(17);
        int32_t result = FbeCrypto::InitUserSpace("", 17, 0);
        printf("the result of Enable is %d", result);
        EXPECT_NE(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Test_Error_0300" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Test_Error_0400
 * @tc.name Fbe_crypto_Test_Error_0400
 * @tc.desc Test function of RemoveUserSpace interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Test_Error_0400, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Test_Error_0400" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(8);
        FbeCrypto::InitUserSpace("", 18, 1);
        FbeCrypto::InitUserSpace("", 18, 2);
        int32_t result = FbeCrypto::RemoveUserSpace("", 8, 0);
        printf("the result of Enable is %d", result);
        EXPECT_NE(result, 0);
    } catch (...) {
        cout << "FbeCryptoTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Test_Error_0400" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Performance_0000
 * @tc.name Fbe_crypto_Performance_0000
 * @tc.desc Test function of Enable interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Performance_0000, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Performance_0000" << endl;
    try {
        for (int a = 0; a < 1000; a++) {
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            int result = FbeCrypto::Enable();
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Fbe_crypto_Performance_0000 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << a
                      << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Performance_0000" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Performance_0100
 * @tc.name Fbe_crypto_Performance_0100
 * @tc.desc Test function of Initialize interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Performance_0100, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Performance_0100" << endl;
    try {
        for (int a = 0; a < 1000; a++) {
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            FbeCrypto::Enable();
            int32_t result = FbeCrypto::Initialize();
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Fbe_crypto_Performance_0100 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << a
                      << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Performance_0100" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Performance_0400
 * @tc.name Fbe_crypto_Performance_0400
 * @tc.desc Test function of CreateKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Performance_0400, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Performance_0400" << endl;
    try {
        for (int a = 0; a < 1000; a++) {
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            FbeCrypto::Enable();
            FbeCrypto::Initialize();
            int32_t result = FbeCrypto::CreateKey(21);
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Fbe_crypto_Performance_0400 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << a
                      << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Performance_0400" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Performance_0500
 * @tc.name Fbe_crypto_Performance_0500
 * @tc.desc Test function of DeleteKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Performance_0500, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Performance_0500" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        for (int a = 0; a < 1000; a++) {
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            FbeCrypto::CreateKey(22);
            int32_t result = FbeCrypto::DeleteKey(22);
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Fbe_crypto_Performance_0500 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << a
                      << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Performance_0500" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Performance_0700
 * @tc.name Fbe_crypto_Performance_0700
 * @tc.desc Test function of DelAuthKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Performance_0700, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Performance_0700" << endl;
    try {
        for (int a = 0; a < 1000; a++) {
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            FbeCrypto::Enable();
            FbeCrypto::Initialize();
            FbeCrypto::CreateKey(24);
            FbeCrypto::AddAuthKey(24, "", "123");
            int32_t result = FbeCrypto::DelAuthKey(24, "", "123");
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Fbe_crypto_Performance_0700 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << a
                      << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Performance_0700" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Performance_0800
 * @tc.name Fbe_crypto_Performance_0800
 * @tc.desc Test function of UnLockKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Performance_0800, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Performance_0800" << endl;
    try {
        for (int a = 0; a < 1000; a++) {
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            FbeCrypto::Enable();
            FbeCrypto::Initialize();
            FbeCrypto::CreateKey(25);
            FbeCrypto::InitUserSpace("", 25, 1);
            FbeCrypto::InitUserSpace("", 25, 2);
            FbeCrypto::AddAuthKey(25, "", "123");
            std::vector<struct CryptoPath> zpaths = {
                { "/data/system/usc/25/crypto.cpp", 1000, 1000, 0770, false },
                { "/data/system/usd/25/crypto.cpp", 1000, 1000, 0770, false }
            };
            for (auto path : zpaths) {
                if (path.IsExist()) {
                    continue;
                }
                path.Create();
            }
            FbeCrypto::LockKey(25);
            int32_t result = FbeCrypto::UnLockKey(25, "", "123");
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Fbe_crypto_Performance_0800 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << a
                      << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Performance_0800" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Performance_0900
 * @tc.name Fbe_crypto_Performance_0900
 * @tc.desc Test function of LockKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Performance_0900, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Performance_0900" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(26);
        FbeCrypto::InitUserSpace("", 26, 1);
        FbeCrypto::InitUserSpace("", 26, 2);
        FbeCrypto::AddAuthKey(26, "", "123");
        std::vector<struct CryptoPath> zpaths = {
            { "/data/system/usc/26/crypto.cpp", 1000, 1000, 0770, false },
            { "/data/system/usd/26/crypto.cpp", 1000, 1000, 0770, false }
        };
        for (auto path : zpaths) {
            if (path.IsExist()) {
                continue;
            }
            path.Create();
        }
        for (int a = 0; a < 1000; a++) {
            struct timeval tv;
            struct timeval tv1;
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            int32_t result = FbeCrypto::LockKey(26);
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Fbe_crypto_Performance_0900 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << a
                      << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Performance_0900" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Performance_1000
 * @tc.name Fbe_crypto_Performance_1000
 * @tc.desc Test function of InitUserSpace interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Performance_1000, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Performance_1000" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(27);
        for (int a = 0; a < 1000; a++) {
            struct timeval tv;
            struct timeval tv1;
            FbeCrypto::InitUserSpace("", 27, 1);
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            int32_t result = FbeCrypto::InitUserSpace("", 27, 2);
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Fbe_crypto_Performance_1000 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << a
                      << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Performance_1000" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Performance_1100
 * @tc.name Fbe_crypto_Performance_1100
 * @tc.desc Test function of RemoveUserSpace interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Performance_1100, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Performance_1100" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(28);
        for (int a = 0; a < 1000; a++) {
            struct timeval tv;
            struct timeval tv1;
            FbeCrypto::InitUserSpace("", 28, 1);
            FbeCrypto::InitUserSpace("", 28, 2);
            FbeCrypto::RemoveUserSpace("", 28, 1);
            gettimeofday(&tv, nullptr);
            std::cout << tv.tv_usec << "us" << endl;
            int32_t result = FbeCrypto::RemoveUserSpace("", 28, 2);
            gettimeofday(&tv1, nullptr);
            std::cout << tv1.tv_usec << "us" << endl;
            std::cout << "Fbe_crypto_Performance_1100 Time use:"
                      << (tv1.tv_sec - tv.tv_sec) * 1000000 + (tv1.tv_usec - tv.tv_usec) << "us," << a
                      << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Performance_1100" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Reliable_0000
 * @tc.name Fbe_crypto_Reliable_0000
 * @tc.desc Test function of Enable interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Reliable_0000, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Reliable_0000" << endl;
    try {
        for (int a = 0; a < 100000; a++) {
            int result = FbeCrypto::Enable();
            std::cout << "count :" << a << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Reliable_0000" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Reliable_0100
 * @tc.name Fbe_crypto_Reliable_0100
 * @tc.desc Test function of Initialize interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Reliable_0100, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Reliable_0100" << endl;
    try {
        for (int a = 0; a < 100000; a++) {
            FbeCrypto::Enable();
            int32_t result = FbeCrypto::Initialize();
            std::cout << "count :" << a << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Reliable_0100" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Reliable_0200
 * @tc.name Fbe_crypto_Reliable_0200
 * @tc.desc Test function of CreateKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Reliable_0200, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Reliable_0200" << endl;
    try {
        for (int a = 0; a < 100000; a++) {
            FbeCrypto::Enable();
            FbeCrypto::Initialize();
            int32_t result = FbeCrypto::CreateKey(21);
            std::cout << "count :" << a << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Reliable_0200" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Reliable_0300
 * @tc.name Fbe_crypto_Reliable_0300
 * @tc.desc Test function of DeleteKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Reliable_0300, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Reliable_0300" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        for (int a = 0; a < 100000; a++) {
            FbeCrypto::CreateKey(22);
            int32_t result = FbeCrypto::DeleteKey(22);
            std::cout << "count :" << a << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Reliable_0300" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Reliable_0400
 * @tc.name Fbe_crypto_Reliable_0400
 * @tc.desc Test function of DelAuthKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Reliable_0400, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Reliable_0400" << endl;
    try {
        for (int a = 0; a < 100000; a++) {
            FbeCrypto::Enable();
            FbeCrypto::Initialize();
            FbeCrypto::CreateKey(24);
            FbeCrypto::AddAuthKey(24, "", "123");
            int32_t result = FbeCrypto::DelAuthKey(24, "", "123");
            std::cout << "count :" << a << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Reliable_0400" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Reliable_0500
 * @tc.name Fbe_crypto_Reliable_0500
 * @tc.desc Test function of UnLockKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Reliable_0500, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Reliable_0500" << endl;
    try {
        for (int a = 0; a < 100000; a++) {
            FbeCrypto::Enable();
            FbeCrypto::Initialize();
            FbeCrypto::CreateKey(25);
            FbeCrypto::InitUserSpace("", 25, 1);
            FbeCrypto::InitUserSpace("", 25, 2);
            FbeCrypto::AddAuthKey(25, "", "123");
            std::vector<struct CryptoPath> zpaths = {
                { "/data/system/usc/25/crypto.cpp", 1000, 1000, 0770, false },
                { "/data/system/usd/25/crypto.cpp", 1000, 1000, 0770, false }
            };
            for (auto path : zpaths) {
                if (path.IsExist()) {
                    continue;
                }
                path.Create();
            }
            FbeCrypto::LockKey(25);
            int32_t result = FbeCrypto::UnLockKey(25, "", "123");
            std::cout << "count :" << a << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Reliable_0500" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Reliable_0600
 * @tc.name Fbe_crypto_Reliable_0600
 * @tc.desc Test function of LockKey interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Reliable_0600, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Reliable_0600" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(26);
        FbeCrypto::InitUserSpace("", 26, 1);
        FbeCrypto::InitUserSpace("", 26, 2);
        FbeCrypto::AddAuthKey(26, "", "123");
        std::vector<struct CryptoPath> zpaths = {
            { "/data/system/usc/26/crypto.cpp", 1000, 1000, 0770, false },
            { "/data/system/usd/26/crypto.cpp", 1000, 1000, 0770, false }
        };
        for (auto path : zpaths) {
            if (path.IsExist()) {
                continue;
            }
            path.Create();
        }
        for (int a = 0; a < 100000; a++) {
            int32_t result = FbeCrypto::LockKey(26);
            std::cout << "count :" << a << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Reliable_0600" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Reliable_0700
 * @tc.name Fbe_crypto_Reliable_0700
 * @tc.desc Test function of InitUserSpace interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Reliable_0700, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Reliable_0700" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(27);
        for (int a = 0; a < 100000; a++) {
            FbeCrypto::InitUserSpace("", 27, 1);
            int32_t result = FbeCrypto::InitUserSpace("", 27, 2);
            std::cout << "count :" << a << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Reliable_0700" << endl;
}

/**
 * @tc.number SUB_STORAGE_Fbe_crypto_Reliable_0800
 * @tc.name Fbe_crypto_Reliable_0800
 * @tc.desc Test function of RemoveUserSpace interface.
 */
HWTEST_F(FbeCryptoTest, Fbe_crypto_Reliable_0800, testing::ext::TestSize.Level1)
{
    cout << "FbeCryptoTest-begin Fbe_crypto_Reliable_0800" << endl;
    try {
        FbeCrypto::Enable();
        FbeCrypto::Initialize();
        FbeCrypto::CreateKey(28);
        for (int a = 0; a < 100000; a++) {
            FbeCrypto::InitUserSpace("", 28, 1);
            FbeCrypto::InitUserSpace("", 28, 2);
            FbeCrypto::RemoveUserSpace("", 28, 1);
            int32_t result = FbeCrypto::RemoveUserSpace("", 28, 2);
            std::cout << "count :" << a << endl;
            EXPECT_EQ(result, 0);
        }
    } catch (...) {
        cout << "FmsFileShareAbilityTest-an exception occurred." << endl;
    }
    cout << "FbeCryptoTest-end Fbe_crypto_Reliable_0800" << endl;
}
} // namespace

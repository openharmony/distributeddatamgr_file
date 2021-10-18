/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "ss_crypto_api.h"

#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "fbe_crypto.h"
#include "istorage_service.h"
#include "storage_hilog.h"
#include "storage_service_proxy.h"

namespace OHOS {
const int SCA_ERR_NONE = 0;

sptr<IStorageService> g_cryptoProxy = nullptr;

bool GetCryptoProxy()
{
    if (g_cryptoProxy != nullptr) {
        return true;
    }
    SSLOG_E("SS_request::Connect start");
    if (g_cryptoProxy == nullptr) {
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            SSLOG_E("SS_request::Connect samgr == nullptr");
            return false;
        }
        auto object = sam->GetSystemAbility(STORAGE_SERVICE_ID);
        if (object == nullptr) {
            SSLOG_E("SS_request::Connect object == nullptr");
            return false;
        }
        g_cryptoProxy = iface_cast<IStorageService>(object);
        if (g_cryptoProxy == nullptr) {
            SSLOG_E("SS_request::Connect service == nullptr");
            return false;
        }
    }
    SSLOG_E("DeviceStorageManager::Connect end");
    return true;
}

int32_t SsCryptoEnable()
{
    SSLOG_I("CryptoEnable starting....(from gby)");
    if (g_cryptoProxy == nullptr) {
        SSLOG_E("The mSsProxy is null..(from gby)");
        return SCA_ERR_NONE;
    }
    g_cryptoProxy->CryptoEnable();
    SSLOG_I("CryptoEnable end....(from gby)");
    return 0;
}

int32_t SsCryptoInitialize()
{
    SSLOG_I("CryptoInitialize starting....(from gby)");
    if (g_cryptoProxy == nullptr) {
        SSLOG_E("The mSsProxy is null..");
        return SCA_ERR_NONE;
    }
    g_cryptoProxy->CryptoInitialize();
    SSLOG_I("CryptoInitialize end....(from gby)");
    return 0;
}

int32_t SsCryptoCreateKey(int32_t userId)
{
    SSLOG_I("CryptoCreateKey starting....(from gby)");
    if (g_cryptoProxy == nullptr) {
        SSLOG_E("The mSsProxy is null..");
        return SCA_ERR_NONE;
    }
    if (userId <= 0) {
        SSLOG_E("The input userId is illegal");
        return SCA_ERR_NONE;
    }
    g_cryptoProxy->CryptoCreateKey(userId);
    SSLOG_I("CryptoCreateKey end....(from gby)");
    return 0;
}

int32_t SsCryptoDeleteKey(int32_t userId)
{
    SSLOG_I("CryptoDeleteKey starting....(from gby)");
    if (g_cryptoProxy == nullptr) {
        SSLOG_E("The mSsProxy is null..");
        return SCA_ERR_NONE;
    }
    if (userId <= 0) {
        SSLOG_E("The input userId is illegal");
        return SCA_ERR_NONE;
    }
    g_cryptoProxy->CryptoDeleteKey(userId);
    SSLOG_I("CryptoDeleteKey end....(from gby)");
    return 0;
}

int32_t SsCryptoAddAuthKey(int32_t userId, const std::string &token, const std::string &secret)
{
    SSLOG_I("CryptoAddAuthKey starting....(from gby)");
    if (g_cryptoProxy == nullptr) {
        SSLOG_E("The mSsProxy is null..");
        return SCA_ERR_NONE;
    }
    if (userId <= 0) {
        SSLOG_E("The input userId is illegal");
        return SCA_ERR_NONE;
    }
    g_cryptoProxy->CryptoAddAuthKey(userId, token, secret);
    SSLOG_I("CryptoAddAuthKey end....(from gby)");
    return 0;
}

int32_t SsCryptoDelAuthKey(int32_t userId, const std::string &token, const std::string &secret)
{
    SSLOG_I("CryptoDelAuthKey starting....(from gby)");
    if (g_cryptoProxy == nullptr) {
        SSLOG_E("The mSsProxy is null..");
        return SCA_ERR_NONE;
    }
    if (userId <= 0) {
        SSLOG_E("The input userId is illegal");
        return SCA_ERR_NONE;
    }
    g_cryptoProxy->CryptoDelAuthKey(userId, token, secret);
    SSLOG_I("CryptoDelAuthKey end....(from gby)");
    return 0;
}

int32_t SsCryptoUnlockKey(int32_t userId, const std::string &token, const std::string &secret)
{
    SSLOG_I("CryptoUnlockKey starting....(from gby)");
    if (g_cryptoProxy == nullptr) {
        SSLOG_E("The mSsProxy is null..");
        return SCA_ERR_NONE;
    }
    if (userId <= 0) {
        SSLOG_E("The input userId is illegal");
        return SCA_ERR_NONE;
    }
    g_cryptoProxy->CryptoUnlockKey(userId, token, secret);
    SSLOG_I("CryptoUnlockKey end....(from gby)");
    return 0;
}

int32_t SsCryptoLockKey(int32_t userId)
{
    SSLOG_I("CryptoLockKey starting....(from gby)");
    if (g_cryptoProxy == nullptr) {
        SSLOG_E("The mSsProxy is null..");
        return SCA_ERR_NONE;
    }
    if (userId <= 0) {
        SSLOG_E("The input userId is illegal");
        return SCA_ERR_NONE;
    }
    g_cryptoProxy->CryptoLockKey(userId);
    SSLOG_I("CryptoLockKey end....(from gby)");
    return 0;
}

int32_t SsCryptoUpdateAuthKey(int32_t userId, const std::string &token, const std::string secret)
{
    SSLOG_I("CryptoUpdateAuthKey starting....(from gby)");
    if (g_cryptoProxy == nullptr) {
        SSLOG_E("The mSsProxy is null..");
        return SCA_ERR_NONE;
    }
    if (userId <= 0) {
        SSLOG_E("The input userId is illegal");
        return SCA_ERR_NONE;
    }
    g_cryptoProxy->CryptoUpdateAuthKey(userId, token, secret);
    SSLOG_I("CryptoUpdateAuthKey end....(from gby)");
    return 0;
}

int32_t SsCryptoInitUserSpace(const std::string &uuid, int32_t userid, int32_t flag)
{
    SSLOG_I("CryptoCreatePaths starting....(from gby)");
    if (g_cryptoProxy == nullptr) {
        SSLOG_E("The mSsProxy is null..");
        return SCA_ERR_NONE;
    }
    if (userid <= 0) {
        SSLOG_E("The input userId is illegal");
        return SCA_ERR_NONE;
    }
    g_cryptoProxy->CryptoInitUserSpace(uuid, userid, flag);
    SSLOG_I("CryptoCreatePaths end....(from gby)");
    return 0;
}

int32_t SsCryptoRemoveUserSpace(const std::string &uuid, int32_t userid, int32_t flag)
{
    SSLOG_I("CryptoRemovePaths starting....(from gby)");
    if (g_cryptoProxy == nullptr) {
        SSLOG_E("The mSsProxy is null..");
        return SCA_ERR_NONE;
    }
    if (userid <= 0) {
        SSLOG_E("The input userId is illegal");
        return SCA_ERR_NONE;
    }
    g_cryptoProxy->CryptoRemoveUserSpace(uuid, userid, flag);
    SSLOG_I("CryptoRemovePaths end....(from gby)");
    return 0;
}
} // namespace OHOS
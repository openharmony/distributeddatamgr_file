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
#ifndef STORAGE_SERVICE_SSCRYPT_FBE_CRYPTO_H
#define STORAGE_SERVICE_SSCRYPT_FBE_CRYPTO_H
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <map>
#include <ctype.h>
#include <stdarg.h>
#include "crypto_policy.h"
#include "crypto_path.h"

namespace OHOS {
class FbeCrypto {
public:
    typedef enum {
        CRYPTO_SUCCESS,
        CRYPTO_ERROR_FAIL,
        CRYPTO_ERROR_KEY_INVALID,
        CRYPTO_ERROR_GENERATE_KEY,
        CRYPTO_ERROR_ENCRYPT_KEY,
        CRYPTO_ERROR_DECRYPT_KEY,
        CRYPTO_ERROR_INSTALL_KEY,
        CRYPTO_ERROR_READ_KEY,
        CRYPTO_ERROR_WRITE_KEY,
        CRYPTO_ERROR_PARAM,
        CRYPTO_ERROR_UNKNOWN
    } CryptoErrorType;
    static int32_t Enable();
    static int32_t Initialize();
    static int32_t CreateKey(int32_t userId);
    static int32_t DeleteKey(int32_t userId);
    static int32_t AddAuthKey(int32_t userId, const std::string &token, const std::string &secret);
    static int32_t DelAuthKey(int32_t userId, const std::string &token, const std::string &secret);
    static int32_t UnLockKey(int32_t userId, const std::string &token, const std::string &secret);
    static int32_t LockKey(int32_t userId);
    static int32_t InitUserSpace(const std::string &uuid, int32_t userId, int32_t flag);
    static int32_t RemoveUserSpace(const std::string &uuid, int32_t userId, int32_t flag);
    static int32_t UpdateAuthKey(int32_t userId, const std::string &token, const std::string secret);

private:
    static int32_t InstallUserKey(int32_t userid);
    static int32_t DoMkDir(const std::vector<CryptoPath> &dirs);
    static int32_t DoMkUnencryptDir();
    static int32_t DoMkSsdEncryptDir();
    static int32_t DoMkUsdEncryptDir(int32_t userid, const std::string uuid = "");
    static int32_t DoMkUscEncryptDir(int32_t userid, const std::string uuid = "");
    static int32_t DoRmUsdEncryptDir(int32_t userid, const std::string uuid = "");
    static int32_t DoRmUscEncryptDir(int32_t userid, const std::string uuid = "");
    static bool LookupUsdPolicy(CryptoPolicy &policy, int32_t userid);
    static bool LookupUscPolicy(CryptoPolicy &policy, int32_t userid);
    static std::string GenAuthToken(const std::string &token, const std::string &secret);
    static CryptoPolicy ssdPolicy_;
    static std::map<int, CryptoPolicy> mUsdPolicies_;
    static std::map<int, CryptoPolicy> mUscPolicies_;
};
} // namespace OHOS

#endif // STORAGE_SERVICE_SSCRYPT_FBE_CRYPTO_H

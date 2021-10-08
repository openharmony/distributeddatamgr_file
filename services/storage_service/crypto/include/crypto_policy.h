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
#ifndef STORAGE_SERVICE_SSCRYPTO_CRYPTO_POLICY_H
#define STORAGE_SERVICE_SSCRYPTO_CRYPTO_POLICY_H
#include <iostream>
#include <unistd.h>
#include <string>
#include <cctype>
#include <linux/fs.h>

#include "hks_api.h"
#include "hks_param.h"
#include "hks_type.h"
#include "nlohmann/json.hpp"

namespace OHOS {
class CryptoPolicy {
public:
    bool Init(const std::string &option);
    int32_t Get(const std::string &dir);
    int32_t Set(const std::string &dir);
    int32_t Load(const std::string &fname);
    int32_t Save(const std::string &fname);
    int32_t Delete(const std::string &fname);
    int32_t Copy(CryptoPolicy &policy);
    bool IsValid();
    fscrypt_policy cryptoPolicy_;
    bool isHwWrappedKey_;
    // Below value need to be clarify not use point, need use data array to store Key
    HksBlob keyAlias_;
    HksBlob encryptKey_;
    HksBlob rawKey_; // For secure reason, we will not keep raw key in the memory
    // rawKey_ will generate by Keyutils or Decrypted from encryptKey_
    // Will be cleared after InstallKey
    static std::string KeyblobToString(const HksBlob &key);
    static void StringToKeyblob(const std::string &ks, HksBlob &key);

private:
    int32_t LookupEncrytMode(const std::string &mode);
    void ToJson(nlohmann::json &jobj);
    void FromJson(const nlohmann::json &jobj);
};
} // namespace OHOS
#endif // STORAGE_SERVICE_SSCRYPTO_CRYPTO_POLICY_H
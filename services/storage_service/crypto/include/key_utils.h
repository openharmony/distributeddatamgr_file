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
#ifndef STORAGE_SERVICE_SSCRYPTO_KEY_UTILS_H
#define STORAGE_SERVICE_SSCRYPTO_KEY_UTILS_H

#include "crypto_path.h"
#include "crypto_policy.h"

namespace OHOS {
class KeyUtils {
public:
    static bool InitKeyring();
    static int32_t GenerateKey(HksBlob &rawkey);
    static int32_t RemoveKey(CryptoPolicy &policy);
    static int32_t InstallKey(CryptoPolicy &policy);
    static int32_t ReloadKey(CryptoPolicy &policy);
    static int32_t GenKeyAlias(HksBlob &keyalias, HksBlob &key);
    static int32_t
    EncryptKey(HksBlob &keyalias, HksBlob &raw, HksBlob &encrypt, const std::string &token = "");
    static int32_t
    DecryptKey(HksBlob &keyalias, HksBlob &encrypt, HksBlob &raw, const std::string &token = "");
    static int32_t InitKeyBlob(HksBlob &key, int32_t len);
    static void FreeKeyBlob(HksBlob &key);
    static void HashString(const std::string &prefix, const std::string &in, std::string &out);
    static void AppendEncryptParam(HksParamSet **param, const std::string &token);
    static void FreeParamSet(HksParamSet **param);
    static void NormalizeKey(HksBlob &key);

private:
    static int32_t SetKeyParamSet(HksParamSet **paramin);
    static int32_t SetEncryptParam(HksParamSet **param, bool isencrypt);
    static std::string GenKeyName(const std::string &keyname, const HksBlob &keyref);
    static void StringToKey(const std::string in, HksBlob &key);
    static int32_t ReadRandomKey(HksBlob &rawkey);
    static int32_t Base64Encode(const uint8_t *srcData,
                                const uint32_t srcDataSize,
                                uint8_t *outData,
                                const uint32_t outDataSize);
};
} // namespace OHOS
#endif // STORAGE_SERVICE_SSCRYPTO_KEY_UTILS_H
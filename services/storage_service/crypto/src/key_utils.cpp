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
#include "key_utils.h"

#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <openssl/sha.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <linux/fs.h>
#include <linux/keyctl.h>

#include "key_ctrl.h"
#include "securec.h"
#include "storage_hilog.h"
#include "storage_utils.h"
#include "hks_api.h"
#include "hks_param.h"
#define CalArraySize(arr) ((sizeof(arr)) / (sizeof((arr)[0])))

const int SSCRYPTO_AES_AAD_LEN = 16;
const int SSCRYPTO_MAX_KEY_SIZE = 64;
const int SSCRYPTO_KEY_DESCRIPTOR_SIZE = 8;
const int SSCRYPTO_AES_256_LEN = 256;
namespace OHOS {
const std::vector<std::string> SSCRYPTO_KEY_NAME_PREFIX = { "ext4:", "f2fs:", "fscrypt:" };
bool KeyUtils::InitKeyring()
{
    key_serial_t devKeyring = KeyCtrl::AddKey("keyring", "fscrypt", 0, 0, KEY_SPEC_SESSION_KEYRING);
    if (devKeyring == -1) {
        SSLOGFE("Error in create keyring");
        return false;
    }
    return true;
}

void KeyUtils::FreeKeyBlob(HksBlob &key)
{
    SSLOG_I("Entry FreeKeyBlob");
    if (key.size > 0) {
        if (key.data != nullptr) {
            memset(key.data, 0, key.size);
            free(key.data);
        }
        key.size = 0;
    } else {
        SSLOGFI("Keyblob is empty");
    }
}
void KeyUtils::StringToKey(const std::string in, HksBlob &key)
{
    SSLOG_I("Entry StringToKey");
    if (in.empty()) {
        SSLOG_E("Wrong string input!");
        return;
    }
    key.size = (in.size() > SSCRYPTO_MAX_KEY_SIZE) ? (SSCRYPTO_MAX_KEY_SIZE) : (in.size());
    key.data = nullptr;
    InitKeyBlob(key, key.size);
    memcpy(key.data, in.data(), key.size);
}
int32_t KeyUtils::GenerateKey(HksBlob &rawkey)
{
    SSLOG_I("Entry GenerateKey");
    rawkey.data = nullptr;
    InitKeyBlob(rawkey, SSCRYPTO_MAX_KEY_SIZE);

    if (HksGenerateRandom(nullptr, &rawkey) != 0) {
        std::string randKey;
        if (SsUtils::ReadRandom(rawkey.size, randKey) == 0) {
            memcpy(rawkey.data, randKey.data(), rawkey.size);
        } else {
            FreeKeyBlob(rawkey);
            SSLOGFE("Error in rawkey generator!");
            return -1;
        }
    }
    SSLOG_I("GenerateKey success");
    return 0;
}

int32_t KeyUtils::RemoveKey(CryptoPolicy &policy)
{
    key_serial_t krid = KeyCtrl::Search(KEY_SPEC_SESSION_KEYRING, "keyring", "fscrypt", 0);
    if (krid == -1) {
        SSLOG_E("Error searching session keyring for fscrypt-provisioning key for fscrypt");
        return -1;
    }
    for (auto prefix : SSCRYPTO_KEY_NAME_PREFIX) {
        std::string keyref = GenKeyName(prefix, policy.keyAlias_);
        key_serial_t ks = KeyCtrl::Search(krid, "logon", keyref.c_str(), 0);
        if (KeyCtrl::Unlink(ks, krid) != 0) {
            SSLOG_E("Failed to unlink key with serial %d ref %s", krid, keyref.c_str());
            return -1;
        }
        // Add below code for drop cache, make sure the keyring already clear the code
        sync();
        int fd = TEMP_FAILURE_RETRY(
            open("/proc/sys/vm/drop_caches", O_WRONLY | O_CREAT | O_NOFOLLOW | O_TRUNC | O_CLOEXEC, 0666));
        if (fd != -1) {
            write(fd, "2", 1);
            fsync(fd);
        }
        // Drop cache
    }
    // remove the policy related key
    FreeKeyBlob(policy.keyAlias_);

    FreeKeyBlob(policy.encryptKey_);
    return 0;
}
int32_t KeyUtils::InstallKey(CryptoPolicy &policy)
{
    SSLOG_I("Entry InstallKey");
    if (policy.rawKey_.size != SSCRYPTO_MAX_KEY_SIZE) {
        // invalid raw key
        SSLOG_E("error rawKey out of range");
        return -1;
    }
    fscrypt_key fskey;
    fskey.mode = FS_ENCRYPTION_MODE_AES_256_XTS;
    fskey.size = policy.rawKey_.size;
    memcpy(fskey.raw, policy.rawKey_.data, fskey.size);
    key_serial_t devKeyring = KeyCtrl::Search(KEY_SPEC_SESSION_KEYRING, "keyring", "fscrypt", 0);
    if (devKeyring == -1) {
        SSLOG_E("Error searching session keyring for fscrypt-provisioning key for fscrypt");
        return -1;
    }
    for (auto prefix : SSCRYPTO_KEY_NAME_PREFIX) {
        std::string keyref = GenKeyName(prefix, policy.keyAlias_);
        key_serial_t ks =
            KeyCtrl::AddKey("logon", keyref.c_str(), static_cast<void *>(&fskey), sizeof(fskey), devKeyring);
        if (ks == -1) {
            // Add key failed
            SSLOG_E("Failed to insert key into keyring %{public}d", devKeyring);
            return -1;
        }
        SSLOG_D("Added key %{public}d (%{public}s) to keyring %{public}d in process %{public}d", ks,
                keyref.c_str(), devKeyring, getpid());
    }
    // remove the raw key after installed
    FreeKeyBlob(policy.rawKey_);
    return 0;
}
// No need for legacy kernel
int32_t KeyUtils::ReloadKey(CryptoPolicy &policy)
{
    return 0;
}

int32_t KeyUtils::SetKeyParamSet(HksParamSet **paramin)
{
    SSLOG_I("Entry SetKeyParamSet");
    struct HksParam keyParam[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_GCM },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE }
    };
    int32_t ret = HksInitParamSet(paramin);
    if (ret != HKS_SUCCESS) {
        SSLOG_E("init param set failed, ret = %d", ret);
        return -1;
    }
    ret = HksAddParams(*paramin, keyParam, CalArraySize(&keyParam));
    if (ret != HKS_SUCCESS) {
        SSLOG_E("add param failed, ret = %d", ret);
        HksFreeParamSet(paramin);
        return -1;
    }

    ret = HksBuildParamSet(paramin);
    if (ret != HKS_SUCCESS) {
        SSLOG_E("build param set failed, ret = %d", ret);
        HksFreeParamSet(paramin);
        return -1;
    }
    return 0;
}

int32_t KeyUtils::SetEncryptParam(HksParamSet **param, bool isencrypt)
{
    SSLOG_I("Entry SetEncryptParam");
    struct HksParam EncryParam[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_GCM },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true }
    };
    int32_t ret = HksInitParamSet(param);
    if (ret != HKS_SUCCESS) {
        SSLOG_E("init ecrypt param set failed, ret = %d", ret);
        return -1;
    }
    ret = HksAddParams(*param, EncryParam, CalArraySize(&EncryParam));
    if (ret != HKS_SUCCESS) {
        SSLOG_E("add ecrypt / decrypt param failed, ret = %d", ret);
        HksFreeParamSet(param);
        return -1;
    }
    struct HksParam purposeParam = { 0 };
    purposeParam.tag = HKS_TAG_PURPOSE;
    if (isencrypt) {
        purposeParam.uint32Param = HKS_KEY_PURPOSE_ENCRYPT;
    } else {
        purposeParam.uint32Param = HKS_KEY_PURPOSE_DECRYPT;
    }
    ret = HksAddParams(*param, (const struct HksParam *)&purposeParam, 1);
    if (ret != 0) {
        SSLOG_E("add ecrypt / decrypt param failed\n");
        HksFreeParamSet(param);
        return -1;
    }
    return 0;
}
static const uint8_t g_base64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int32_t KeyUtils::Base64Encode(const uint8_t *srcData,
                               const uint32_t srcDataSize,
                               uint8_t *outData,
                               const uint32_t outDataSize)
{
    uint32_t j = 0;
    uint32_t i = 0;
    while (i < srcDataSize) {
        uint32_t a = (i < srcDataSize) ? (uint8_t)srcData[i] : 0;
        ++i;
        uint32_t b = (i < srcDataSize) ? (uint8_t)srcData[i] : 0;
        ++i;
        uint32_t c = (i < srcDataSize) ? (uint8_t)srcData[i] : 0;
        ++i;
        /* srcData each character takes up 8 bits. 1, 2 and 3 is offset */
        uint32_t byte = (a << (8 * 2)) + (b << (8 * 1)) + (c << (8 * 0));

        /* outData each character takes up 6 bits */
        outData[j++] = g_base64Table[(byte >> (6 * 3)) & 0b00111111]; /* 3 and 6 is offset */
        outData[j++] = g_base64Table[(byte >> (6 * 2)) & 0b00111111]; /* 2 and 6 is offset */
        outData[j++] = g_base64Table[(byte >> (6 * 1)) & 0b00111111]; /* 1 and 6 is offset */
        outData[j++] = g_base64Table[(byte >> (6 * 0)) & 0b00111111]; /* 0 and 6 is offset */
    }

    const int32_t padding = srcDataSize % 3; /* 3 in each group */
    if (padding == 0) {
        return 0;
    } else {
        outData[outDataSize - 1] = '='; /* 1: padding last character with '=' */
    }
    if (padding == 1) {
        outData[outDataSize - 2] = '='; /* 2: padding penultimate character with '=' */
    }
    return 0;
}
void KeyUtils::NormalizeKey(HksBlob &key)
{
    SSLOGFE("entry NormalizeKey");
    if (key.size <= 0) {
        return;
    }
    uint8_t *pDst = (uint8_t *)malloc(key.size);
    Base64Encode(key.data, key.size, pDst, key.size);
    memset(key.data, 0, key.size);
    memcpy(key.data, pDst, key.size);
}

int32_t KeyUtils::GenKeyAlias(HksBlob &keyalias, HksBlob &key)
{
    // Generate a Key Alias and AES key for RAW key encrypt,Also for the Key reference
    SSLOG_I("Entry GenKeyAlias");
    if (key.size <= 0) {
        // Cannot generate KeyAlias
        SSLOG_E("error Cannot generate KeyAlias");
        return -1;
    }
    SHA512_CTX c;

    SHA512_Init(&c);
    SHA512_Update(&c, key.data, key.size);
    unsigned char key_ref1[SHA512_DIGEST_LENGTH];
    SHA512_Final(key_ref1, &c);

    SHA512_Init(&c);
    SHA512_Update(&c, key_ref1, SHA512_DIGEST_LENGTH);
    unsigned char key_ref2[SHA512_DIGEST_LENGTH];
    SHA512_Final(key_ref2, &c);

    static_assert(SHA512_DIGEST_LENGTH >= SSCRYPTO_KEY_DESCRIPTOR_SIZE, "Hash too short for descriptor");
    keyalias.data = nullptr;
    InitKeyBlob(keyalias, SSCRYPTO_KEY_DESCRIPTOR_SIZE);
    memcpy(keyalias.data, key_ref2, SSCRYPTO_KEY_DESCRIPTOR_SIZE);
    NormalizeKey(keyalias);
    return 0;
}

void KeyUtils::FreeParamSet(HksParamSet **param)
{
    SSLOG_I("Entry FreeParamSet");
    if (param == nullptr) {
        return;
    }
    for (uint32_t i = 0; i < (*param)->paramsCnt; i++) {
        if (((*param)->params[i].tag & HKS_TAG_TYPE_MASK) == HKS_TAG_TYPE_BYTES) {
            if ((*param)->params[i].blob.data != nullptr) {
                free((*param)->params[i].blob.data);
                (*param)->params[i].blob.size = 0;
            }
        }
    }
    HksFreeParamSet(param);
}
void KeyUtils::AppendEncryptParam(HksParamSet **param, const std::string &token)
{
    std::string nonceToken;
    std::string aadToken;
    HksBlob nonce;
    HksBlob aad;
    HashString("NONCE hash gene by SHA512", token, nonceToken);
    HashString("AAD hash gene by SHA512", token, aadToken);
    nonceToken.resize(HKS_AE_NONCE_LEN);
    aadToken.resize(SSCRYPTO_AES_AAD_LEN);
    StringToKey(nonceToken, nonce);
    StringToKey(aadToken, aad);
    struct HksParam addParam[] = { { .tag = HKS_TAG_NONCE, .blob = { nonce.size, nonce.data } },
                                   { .tag = HKS_TAG_ASSOCIATED_DATA, .blob = { aad.size, aad.data } } };
    int ret = HksAddParams(*param, addParam, CalArraySize(&addParam));
    if (ret != 0) {
        SSLOG_E("add ecrypt / decrypt param failed\n");
        FreeParamSet(param);
    }
}
void KeyUtils::HashString(const std::string &prefix, const std::string &in, std::string &out)
{
    SSLOG_I("Entry HashString");
    std::string refstr = prefix;
    if (prefix.empty()) {
        refstr.append("dummystring Gen for SHA512");
    }
    SHA512_CTX c;

    SHA512_Init(&c);
    // Personalise the hashing by introducing a fixed prefix.
    // Hashing applications should use personalization except when there is a
    // specific reason not to; see section 4.11 of https://www.schneier.com/skein1.3.pdf
    refstr.resize(SHA512_CBLOCK);
    SHA512_Update(&c, refstr.data(), refstr.size());
    SHA512_Update(&c, in.data(), in.size());
    out.assign(SHA512_DIGEST_LENGTH, '\0');
    SHA512_Final(reinterpret_cast<uint8_t *>(&(out)[0]), &c);
}

int32_t KeyUtils::EncryptKey(HksBlob &keyalias, HksBlob &raw, HksBlob &encrypt, const std::string &token)
{
    // Use AES to encrypt raw key
    int ret = 0;
    if (keyalias.size <= 0 || raw.size <= 0) {
        // invalid input
        SSLOG_E("error invalid input");
        return -1;
    }
    // build parameter
    // Key generate parameter
    HksParamSet *paramSet = new HksParamSet;
    if (SetKeyParamSet(&paramSet) != 0) {
        SSLOG_E("Error in Generate Key for encryption!");
        return -1;
    }
    if (HksGenerateKey(&keyalias, paramSet, nullptr) != 0) {
        SSLOG_E("GenerateKey Error !");
        FreeParamSet(&paramSet);
        return -1;
    }
    FreeParamSet(&paramSet);
    // Encrypt parameter set
    if (SetEncryptParam(&paramSet, true) != 0) {
        SSLOG_E("Error in Generate ecryption parameter!");
        return -1;
    }
    AppendEncryptParam(&paramSet, token);
    encrypt.data = nullptr;
    InitKeyBlob(encrypt, SSCRYPTO_AES_256_LEN);
    // Need add Aad & Nonce paramter
    if (HksEncrypt(&keyalias, paramSet, &raw, &encrypt) != 0) {
        SSLOG_E("Error in Generate ecryption parameter!");

        FreeKeyBlob(encrypt);
        ret = -1;
    }
    FreeParamSet(&paramSet);
    return ret;
}
int32_t KeyUtils::DecryptKey(HksBlob &keyalias, HksBlob &encrypt, HksBlob &raw, const std::string &token)
{
    // Decrypt encrypt key to raw key
    // Use AES to encrypt raw key
    HksBlob tmpKey = {};
    int ret = 0;
    if (keyalias.size <= 0 || encrypt.size <= 0) {
        // invalid input
        SSLOG_E("error invalid input");
        return -1;
    }
    // build parameter
    HksParamSet *paramSet = nullptr;
    if (SetEncryptParam(&paramSet, false) != 0) {
        SSLOG_E("Error in Generate DecryptKey parameter!");
        return -1;
    }
    AppendEncryptParam(&paramSet, token);
    tmpKey.data = nullptr;
    InitKeyBlob(tmpKey, SSCRYPTO_AES_256_LEN);
    if (HksDecrypt(&keyalias, paramSet, &encrypt, &tmpKey) != 0) {
        SSLOG_E("Error in do decryption!");
        ret = -1;
    }
    raw.data = nullptr;
    InitKeyBlob(raw, SSCRYPTO_MAX_KEY_SIZE);
    for (int i = 0; i < SSCRYPTO_MAX_KEY_SIZE; i++) {
        raw.data[i] = tmpKey.data[i];
        SSLOGFE("%{public}d, ", raw.data[i]);
    }
    FreeParamSet(&paramSet);
    FreeKeyBlob(tmpKey);
    return ret;
}

std::string KeyUtils::GenKeyName(const std::string &keyname, const HksBlob &keyref)
{
    SSLOG_I("Entry GenKeyName");
    return keyname + CryptoPolicy::KeyblobToString(keyref);
}
int32_t KeyUtils::InitKeyBlob(HksBlob &key, int32_t len)
{
    SSLOG_I("Entry InitKeyBlob");
    if (len > 0) {
        key.size = len;
        if (key.data != nullptr) {
            memset(key.data, 0, key.size);
            return key.size;
        }
        key.data = nullptr;
        key.data = (uint8_t *)malloc(key.size);
        memset(key.data, 0, key.size);
    }
    return key.size;
}
} // namespace OHOS
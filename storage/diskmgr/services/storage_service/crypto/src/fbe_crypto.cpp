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
#include "fbe_crypto.h"

#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/fs.h>

#include "crypto_path.h"
#include "crypto_policy.h"
#include "key_utils.h"
#include "storage_hilog.h"
#include "ss_tab_mgr.h"
#include "utils_string.h"
using namespace OHOS::SsUtils;
namespace OHOS {
static constexpr int FLAG_SS_STORAGE_USD = 1 << 0;
static constexpr int FLAG_SS_STORAGE_USC = 1 << 1;
const std::string CRYPTO_SSD_POLICY_NAME = "/data/unencrypt/ssd.policy";
const std::string CRYPTO_USD_POLICY_DIR = "/data/system/key/usd/";
const std::string CRYPTO_USC_POLICY_DIR = "/data/system/key/usc/";
const std::string DECRYPT_USC_POLICY_DIR = "/data/system/usc/";

CryptoPolicy FbeCrypto::ssdPolicy_ = {};
std::map<int, CryptoPolicy> FbeCrypto::mUsdPolicies_ = {};
std::map<int, CryptoPolicy> FbeCrypto::mUscPolicies_ = {};

int32_t FbeCrypto::InstallUserKey(int32_t userid)
{
    SSLOG_E("entry InstallUserKey!\n");
    CryptoPolicy usdPolicy;
    CryptoPolicy uscPolicy;
    usdPolicy.Copy(ssdPolicy_);
    KeyUtils::GenerateKey(usdPolicy.rawKey_);
    KeyUtils::GenKeyAlias(usdPolicy.keyAlias_, usdPolicy.rawKey_);
    if (KeyUtils::EncryptKey(usdPolicy.keyAlias_, usdPolicy.rawKey_, usdPolicy.encryptKey_) != 0) {
        KeyUtils::FreeKeyBlob(usdPolicy.rawKey_);
        SSLOGFE("Key Encryption Error!");
        return CRYPTO_ERROR_ENCRYPT_KEY;
    }
    if (KeyUtils::InstallKey(usdPolicy) != 0) {
        KeyUtils::FreeKeyBlob(usdPolicy.rawKey_);
        SSLOGFE("Key install Error!");
        return CRYPTO_ERROR_INSTALL_KEY;
    }
    usdPolicy.Save(CRYPTO_USD_POLICY_DIR + StringPrintf("%d.policy", userid));
    mUsdPolicies_[userid] = usdPolicy;
    uscPolicy.Copy(ssdPolicy_);
    KeyUtils::GenerateKey(uscPolicy.rawKey_);
    KeyUtils::GenKeyAlias(uscPolicy.keyAlias_, uscPolicy.rawKey_);
    if (KeyUtils::EncryptKey(uscPolicy.keyAlias_, uscPolicy.rawKey_, uscPolicy.encryptKey_) != 0) {
        KeyUtils::FreeKeyBlob(uscPolicy.rawKey_);
        SSLOGFE("Key Encryption Error!");
        return CRYPTO_ERROR_ENCRYPT_KEY;
    }
    if (KeyUtils::InstallKey(uscPolicy) == 0) {
        mUscPolicies_[userid] = uscPolicy;
        uscPolicy.Save(CRYPTO_USC_POLICY_DIR + StringPrintf("%d.policy", userid));
        return CRYPTO_SUCCESS;
    } else {
        KeyUtils::FreeKeyBlob(uscPolicy.rawKey_);
        SSLOGFE("Key install Error!");
        return CRYPTO_ERROR_INSTALL_KEY;
    }
}
int32_t FbeCrypto::Enable()
{
    FsMountTab::FsMountEntry entry = {};
    SsTabMgr stm;
    if (!stm.GetEntry("/data", entry)) {
        SSLOGFE("Read Mount option failed for data");
        return CRYPTO_ERROR_PARAM;
    }
    if (!entry.IsEncryptable()) {
        SSLOGFE("Is not encrypt for data ");
        return CRYPTO_ERROR_PARAM;
    }

    /* If support crypto ,this function will be start in Init process */
    if (!KeyUtils::InitKeyring()) {
        SSLOGFE("Cannot create Keyring in startup!");
        return CRYPTO_ERROR_FAIL;
    }
    if (ssdPolicy_.Load(CRYPTO_SSD_POLICY_NAME) == 0) {
        if (KeyUtils::DecryptKey(ssdPolicy_.keyAlias_, ssdPolicy_.encryptKey_, ssdPolicy_.rawKey_) != 0) {
            SSLOGFE("Key decryption Error!");
            return CRYPTO_ERROR_DECRYPT_KEY;
        }
        if (KeyUtils::InstallKey(ssdPolicy_) != 0) {
            KeyUtils::FreeKeyBlob(ssdPolicy_.rawKey_);
            SSLOGFE("Key Install Error!");
            return CRYPTO_ERROR_INSTALL_KEY;
        }
        // Get saved ssd policy
    } else {
        // first time bootup, generate ssd policy, dummy code in here for initial
        if (!ssdPolicy_.Init(entry.encryptOptions)) {
            SSLOGFE("Init Policy encryptoption = %{pulbic}s error !", entry.encryptOptions.c_str());
            // Policy invalid
            return CRYPTO_ERROR_PARAM;
        }
        DoMkUnencryptDir();
        KeyUtils::GenerateKey(ssdPolicy_.rawKey_);
        KeyUtils::GenKeyAlias(ssdPolicy_.keyAlias_, ssdPolicy_.rawKey_);
        if (KeyUtils::EncryptKey(ssdPolicy_.keyAlias_, ssdPolicy_.rawKey_, ssdPolicy_.encryptKey_) != 0) {
            KeyUtils::FreeKeyBlob(ssdPolicy_.rawKey_);
            SSLOGFE("Key Encryption Error!");
            return CRYPTO_ERROR_ENCRYPT_KEY;
        }
        if (KeyUtils::InstallKey(ssdPolicy_) != 0) {
            KeyUtils::FreeKeyBlob(ssdPolicy_.rawKey_);
            SSLOGFE("Key Install Error!");
            return CRYPTO_ERROR_INSTALL_KEY;
        }
    }
    DoMkSsdEncryptDir();
    ssdPolicy_.Save(CRYPTO_SSD_POLICY_NAME);
    SSLOGFI("SSD policy saved to %{public}s", CRYPTO_SSD_POLICY_NAME.c_str());
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::Initialize()
{
    SSLOG_D("Initialize user0");
    CryptoPolicy usdPolicy;

    if (usdPolicy.Load(CRYPTO_USD_POLICY_DIR + "0.policy") == 0) {
        if (usdPolicy.IsValid()) {
            SSLOGFI("decode key and ready to install key");
            KeyUtils::DecryptKey(usdPolicy.keyAlias_, usdPolicy.encryptKey_, usdPolicy.rawKey_);
            // Check if need load all USD policy and Key, Now we have only load 1
            mUsdPolicies_[0] = usdPolicy;
            KeyUtils::InstallKey(usdPolicy);
        } else {
            SSLOGFE("policy is invalid, need to be re creart~!");
            InstallUserKey(0);
        }
    } else {
        SSLOGFI("Initialize user 0 policy ");
        // Generate USD policy and install key to kernel, save USD policy
        InstallUserKey(0);
    }
    UnLockKey(0, "", "");
    DoMkUsdEncryptDir(0);
    SSLOGFI("Create user device space ");
    // If need load all usd policy
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::CreateKey(int32_t userid)
{
    SSLOG_D("CreateKey for user %d", userid);
    if (mUscPolicies_.count(userid) != 0) {
        // Key already exist
        SSLOG_D("the key of user %d is already exist", userid);
        return 0;
    }
    InstallUserKey(userid);
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::DeleteKey(int32_t userid)
{
    SSLOG_D("DeleteKey for user %d", userid);
    int32_t ret = 0;
    CryptoPolicy uscPolicy;
    if (LookupUscPolicy(uscPolicy, userid)) {
        HksDeleteKey(&(uscPolicy.keyAlias_), nullptr);
        ret = KeyUtils::RemoveKey(uscPolicy);
        uscPolicy.Delete(CRYPTO_USC_POLICY_DIR + StringPrintf("%d.policy", userid));
    }
    mUscPolicies_.erase(userid);
    // Normally it need use secblkdiscard and rm -rf to clean the all things.
    // We ignore it in this version
    CryptoPolicy usdPolicy;
    if (LookupUsdPolicy(usdPolicy, userid)) {
        HksDeleteKey(&(usdPolicy.keyAlias_), nullptr);
        ret = KeyUtils::RemoveKey(usdPolicy);
        usdPolicy.Delete(CRYPTO_USD_POLICY_DIR + StringPrintf("%d.policy", userid));
    }
    mUsdPolicies_.erase(userid);
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::AddAuthKey(int32_t userid, const std::string &token, const std::string &secret)
{
    SSLOG_D("AddAuthKey");
    CryptoPolicy policy;
    if (policy.Load(CRYPTO_USC_POLICY_DIR + StringPrintf("%d.policy", userid)) == 0) {
        if (policy.IsValid()) {
            mUscPolicies_[userid] = policy;
        }
    }
    if (mUscPolicies_.count(userid) == 0) {
        // No userid policy's generate new policy and save it
        policy.Copy(ssdPolicy_);
        KeyUtils::GenerateKey(policy.rawKey_);
        KeyUtils::GenKeyAlias(policy.keyAlias_, policy.rawKey_);
        // If have Auth token, need regerate Key with AuthToken
        if (KeyUtils::EncryptKey(policy.keyAlias_, policy.rawKey_, policy.encryptKey_,
                                 GenAuthToken(token, secret)) != 0) {
            SSLOGFE("Add auth key encrypy key failed");
            return CRYPTO_ERROR_ENCRYPT_KEY;
        }
        KeyUtils::InstallKey(policy);
        mUscPolicies_[userid] = policy;
        policy.Save(CRYPTO_USC_POLICY_DIR + StringPrintf("%d.policy", userid));
    } else {
        CryptoPolicy newPolicy;
        LookupUscPolicy(newPolicy, userid);
        // If support AuthToken for decryption, we can add authtoke for decryption to check auth
        // Current remove the decryption process
        if (KeyUtils::DecryptKey(newPolicy.keyAlias_, newPolicy.encryptKey_, newPolicy.rawKey_) != 0) {
            // decrypt key error
            SSLOGFE("Decryptkey error with Auth key ");
            return CRYPTO_ERROR_DECRYPT_KEY;
        }
        KeyUtils::EncryptKey(newPolicy.keyAlias_, newPolicy.rawKey_, newPolicy.encryptKey_,
                             GenAuthToken(token, secret));

        mUscPolicies_[userid] = newPolicy;
        newPolicy.Save(CRYPTO_USC_POLICY_DIR + StringPrintf("%d.policy", userid));
    }
    SSLOGFI("Reupdate user policy");
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::DelAuthKey(int32_t userid, const std::string &token, const std::string &secret)
{
    SSLOG_D("DelAuthKey %d ", userid);
    if (mUscPolicies_.count(userid) == 0) {
        CryptoPolicy policy;
        // No userid policy's generate new policy and save it
        policy.Copy(ssdPolicy_);
        // Need use empty token as input to generate rawKey
        KeyUtils::GenerateKey(policy.rawKey_);
        KeyUtils::GenKeyAlias(policy.keyAlias_, policy.rawKey_);
        KeyUtils::InstallKey(policy);
        // If have Auth token, need regerate Key with AuthToken
        KeyUtils::EncryptKey(policy.keyAlias_, policy.rawKey_, policy.encryptKey_);
        mUscPolicies_[userid] = policy;
        policy.Save(CRYPTO_USC_POLICY_DIR + StringPrintf("%d.policy", userid));
    } else {
        CryptoPolicy newPolicy;
        LookupUscPolicy(newPolicy, userid);
        // If support AuthToken for decryption, we can add authtoke for decryption to check auth
        if (KeyUtils::DecryptKey(newPolicy.keyAlias_, newPolicy.encryptKey_, newPolicy.rawKey_,
                                 GenAuthToken(token, secret)) != 0) {
            // AuthToken error, So return error without
            return CRYPTO_ERROR_DECRYPT_KEY;
        }
        KeyUtils::EncryptKey(newPolicy.keyAlias_, newPolicy.rawKey_, newPolicy.encryptKey_);
        mUscPolicies_[userid] = newPolicy;
        newPolicy.Save(CRYPTO_USC_POLICY_DIR + StringPrintf("%d.policy", userid));
    }
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::UnLockKey(int32_t userid, const std::string &token, const std::string &secret)
{
    SSLOG_D("Unlock key %d ", userid);
    CryptoPolicy policy;
    if (mUscPolicies_.count(userid) != 0) {
        // Already have usc key and policy, no need to unlock
        return CRYPTO_SUCCESS;
    }
    // Ready to install key
    if (policy.Load(CRYPTO_USC_POLICY_DIR + StringPrintf("%d.policy", userid)) != 0) {
        // No correct policy to load
        return CRYPTO_ERROR_READ_KEY;
    }
    if (KeyUtils::DecryptKey(policy.keyAlias_, policy.encryptKey_, policy.rawKey_,
                             GenAuthToken(token, secret)) != 0) {
        // AuthToken error, So return error without
        return CRYPTO_ERROR_DECRYPT_KEY;
    }
    KeyUtils::InstallKey(policy);
    mUscPolicies_[userid] = policy;
    policy.Save(CRYPTO_USC_POLICY_DIR + StringPrintf("%d.policy", userid));
    policy.Set(DECRYPT_USC_POLICY_DIR + StringPrintf("%d", userid));
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::LockKey(int32_t userid)
{
    SSLOG_D("lock key %d ", userid);
    CryptoPolicy policy;
    if (mUscPolicies_.count(userid) == 0) {
        // not found this user policy ,default return true
        SSLOGFE("Not found current user id key ");
        return CRYPTO_SUCCESS;
    }
    LookupUscPolicy(policy, userid);
    KeyUtils::RemoveKey(policy);
    mUscPolicies_.erase(userid);
    SSLOGFI("Lockkey remove key success!");
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::InitUserSpace(const std::string &uuid, int32_t userid, int32_t flag)
{
    SSLOG_D("InitUserSpace");
    if (flag & FLAG_SS_STORAGE_USD) {
        DoMkUsdEncryptDir(userid, uuid);
        return CRYPTO_SUCCESS;
    }
    if (flag & FLAG_SS_STORAGE_USC) {
        DoMkUscEncryptDir(userid, uuid);
        return CRYPTO_SUCCESS;
    }
    SSLOGFE("Error input wrong flag %d", flag);
    // unknown flag input
    return CRYPTO_ERROR_FAIL;
}
int32_t FbeCrypto::RemoveUserSpace(const std::string &uuid, int32_t userid, int32_t flag)
{
    SSLOG_D("RemoveUserSpace");
    if (flag & FLAG_SS_STORAGE_USD) {
        DoRmUsdEncryptDir(userid, uuid);
        return CRYPTO_SUCCESS;
    }
    if (flag & FLAG_SS_STORAGE_USC) {
        DoRmUscEncryptDir(userid, uuid);
        return CRYPTO_SUCCESS;
    }
    // unknown flag input
    return CRYPTO_ERROR_FAIL;
}
// This function no need in current design
int32_t FbeCrypto::UpdateAuthKey(int32_t userid, const std::string &token, const std::string secret)
{
    return 0;
}

bool FbeCrypto::LookupUscPolicy(CryptoPolicy &policy, int32_t userid)
{
    SSLOGFI("LookupUscPolicy for user %{public}d ", userid);
    auto rp = mUscPolicies_.find(userid);
    if (rp == mUscPolicies_.end()) {
        SSLOGFE("LookupUscPolicy Not found for user %{public}d ", userid);
        return false;
    }

    policy.Copy(rp->second);

    return true;
}
bool FbeCrypto::LookupUsdPolicy(CryptoPolicy &policy, int32_t userid)
{
    SSLOGFI("LookupUsdPolicy for user %{public}d ", userid);
    auto rp = mUsdPolicies_.find(userid);
    if (rp == mUsdPolicies_.end()) {
        SSLOGFE("LookupUsdPolicy Not found for user %{public}d ", userid);
        return false;
    }
    policy.Copy(rp->second);

    return true;
}
int32_t FbeCrypto::DoMkDir(const std::vector<CryptoPath> &dirs)
{
    SSLOG_I("Entry DoMkDir");
    SSLOGFI("DoMkDir size is %{public}d ", dirs.size());
    for (auto dir : dirs) {
        // Create dir
        SSLOGFI("Make dir %{public}s", dir.path.c_str());
        if (dir.IsExist()) {
            SSLOGFI("Set dir policy %{public}s", dir.path.c_str());
            if (dir.isEncrypt) {
                ssdPolicy_.Set(dir.path);
            }
            continue;
        }
        SSLOGFI("Create new dir and set Policy %{public}s", dir.path.c_str());
        dir.Create();
        if (dir.isEncrypt) {
            ssdPolicy_.Set(dir.path);
        }
    }
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::DoMkUnencryptDir()
{
    SSLOG_D("Entry DoMkUnencryptDir");
    std::vector<CryptoPath> paths = {
        { "/data/lost+found", 1000, 1000, 0700, false }, { "/data/unencrypt", 1000, 1000, 0700, false },
        { "/data/system/usd", 1000, 1000, 0700, false }, { "/data/system/usc", 1000, 1000, 0700, false },
        { "/data/user", 1000, 1000, 0700, false },       { "/data/user/usd", 1000, 1000, 0700, false },
    };
    DoMkDir(paths);
    return 0;
}
int32_t FbeCrypto::DoMkSsdEncryptDir()
{
    SSLOG_D("Entry DoMkSsdEncryptDir");
    std::vector<CryptoPath> paths = {
        { "/data/system", 1000, 1000, 0700, true },
        { "/data/vendor", 0, 0, 0700, true },
        { "/data/app", 0, 0, 0700, true },
        { "/data/misc", 0, 0, 0700, true },
        { "/data/local", 0, 0, 0700, true },
        { "/data/system/key", 0, 0, 0700, true },
        { "/data/system/key/usd", 0, 0, 0700, true },
        { "/data/system/key/usc", 0, 0, 0700, true },
    };
    DoMkDir(paths);
    return 0;
}
int32_t FbeCrypto::DoMkUsdEncryptDir(int32_t userid, const std::string uuid)
{
    SSLOG_D("Make UsdEncryptDir");
    std::vector<struct CryptoPath> paths = {
        { "/data/system/usd/", 1000, 1000, 0770, false },
        { "/data/vendor/usd/", 1000, 1000, 0771, false },
        { "/data/misc/usd/", 1000, 1000, 01771, false },
        { "/data/user/usd/", 1000, 1000, 0771, false },
    };
    CryptoPolicy policy;
    bool find = LookupUsdPolicy(policy, userid);
    if (uuid.empty()) {
        if (find) {
            for (auto path : paths) {
                path.Create();
                path.path += StringPrintf("%d/", userid);
                path.Create();
                policy.Set(path.path);
            }
        }
    } else {
        if (find) {
            // Current is the mount path define
            CryptoPath path = { "/mnt/expand/user/usd/", 1000, 1000, 0771, false };
            path.path += StringPrintf("%s/%d/", uuid.c_str(), userid);
            path.Create();
            policy.Set(path.path);
            // Volume Key install
        }
    }
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::DoMkUscEncryptDir(int32_t userid, const std::string uuid)
{
    SSLOG_D("Make UscEncryptDir");
    std::vector<struct CryptoPath> paths = {
        { "/data/system/usc/", 0, 0, 0770, false },
        { "/data/vendor/usc/", 0, 0, 0771, false },
        { "/data/misc/usc/", 1000, 1000, 01771, false },
        { "/data/media/", 0, 0, 0771, false }
    };
    CryptoPolicy policy;
    bool find = LookupUscPolicy(policy, userid);
    if (uuid.empty()) {
        if (find) {
            for (auto path : paths) {
                path.Create();
                path.path += StringPrintf("%d/", userid);
                path.Create();
                policy.Set(path.path);
            }
        } else {
            // No user id related policy ,So don't create paths
            SSLOG_E("user %d no policy", userid);
            return -1;
        }
    } else {
        if (find) {
            CryptoPath path = { "/mnt/expand/data", 1000, 1000, 0771, false };
            path.path += StringPrintf("%s/%d/", uuid.c_str(), userid);
            path.Create();
            policy.Set(path.path);
            // Volume Key install
        }
    }
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::DoRmUsdEncryptDir(int32_t userid, const std::string uuid)
{
    SSLOG_D("delete UsdEncryptDir");
    std::vector<struct CryptoPath> paths = {
        { "/data/system/usd/", 1000, 1000, 0770, false },
        { "/data/vendor/usd/", 1000, 1000, 0771, false },
        { "/data/misc/usd/", 1000, 1000, 01771, false },
        { "/data/user/usd/", 1000, 1000, 0771, false },
    };
    if (uuid.empty()) {
        for (auto path : paths) {
            path.path += StringPrintf("%d/", userid);
            path.Delete();
        }
    } else {
        CryptoPath path = { "/mnt/expand/user/usd/", 1000, 1000, 0771, false };
        path.path += StringPrintf("%s/%d/", uuid.c_str(), userid);
        path.Delete();
    }
    return CRYPTO_SUCCESS;
}
int32_t FbeCrypto::DoRmUscEncryptDir(int32_t userid, const std::string uuid)
{
    SSLOG_D("delete UscEncryptDir");
    std::vector<struct CryptoPath> paths = {
        { "/data/system/usc/", 0, 0, 0770, false },
        { "/data/vendor/usc/", 0, 0, 0771, false },
        { "/data/misc/usc/", 1000, 1000, 01771, false },
        { "/data/media/", 0, 0, 0771, false }
    };
    if (uuid.empty()) {
        for (auto path : paths) {
            path.path += StringPrintf("%d/", userid);
            path.Delete();
        }
    } else {
        //
        CryptoPath path = { "/mnt/expand/data", 1000, 1000, 0771, false };
        path.path += StringPrintf("%s/%d/", uuid.c_str(), userid);
        path.Delete();
        // Volume Key install
    }
    return CRYPTO_SUCCESS;
}

std::string FbeCrypto::GenAuthToken(const std::string &token, const std::string &secret)
{
    // Generate AuthToken for encrypt and decrypt key
    std::string result = "";
    if (secret.empty()) {
        return result;
    }
    return secret;
}
} // namespace OHOS
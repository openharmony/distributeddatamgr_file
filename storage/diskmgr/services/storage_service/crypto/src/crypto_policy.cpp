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
#include "crypto_policy.h"

#include <cstring>
#include <fcntl.h>
#include <map>
#include <regex>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/fs.h>

#include "key_utils.h"
#include "storage_hilog.h"
#include "utils_string.h"
#include "utils_file.h"
using namespace OHOS::SsUtils;
namespace OHOS {
const int SSCRYPTO_DESCRIPTOR_SIZE = 8;
const int SSCRYPTO_POLICY_V1 = 0;
static const int NUM_TWO_ = 2;
static const int NUM_FOUR_ = 4;
constexpr int MAX_DIGIT_NUM = 9;
constexpr int DIFF_DIGIT_CHAR = 7;
int32_t CryptoPolicy::Get(const std::string &dir)
{
    int32_t ret = 0;
    // User ioctl get folder's policy
    int fd = open(dir.c_str(), O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
    if (fd < 0) {
        // Get Policy error with wrong directory
        SSLOG_E("Get Policy error with wrong directory");
        return fd;
    }
    ret = ioctl(fd, FS_IOC_GET_ENCRYPTION_POLICY, &cryptoPolicy_);
    close(fd);
    return ret;
}
/* EOPNOTSUPP: the kernel was not configured with encryption support for filesystems, 
 * or the filesystem superblock has not had encryption enabled on it. (For example, 
 * to use encryption on an ext4 filesystem, CONFIG_FS_ENCRYPTION or CONFIG_EXT4_ENCRYPTION
 * must be enabled in the kernel config, and the superblock must have had the “encrypt” 
 * feature flag enabled using tune2fs -O encrypt or mkfs.ext4 -O encrypt.)
 */
int32_t CryptoPolicy::Set(const std::string &dir)
{
    SSLOG_I("Entry Set");
    // use ioctl set current policy to the folder
    int ret = 0;
    if (!IsDirEmpty(dir)) {
        SSLOGFE("Dir is not empty!");
        return -1;
    }
    if (!IsValid()) {
        SSLOG_E("error policy is not valid");
        return -1;
    }
    memcpy(&cryptoPolicy_.master_key_descriptor[0], keyAlias_.data, SSCRYPTO_DESCRIPTOR_SIZE);
    int fd = open(dir.c_str(), O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
    if (fd == -1) {
        // Open dir error
        SSLOG_E("Open dir error");
        return -1;
    }
    ret = ioctl(fd, FS_IOC_SET_ENCRYPTION_POLICY, &cryptoPolicy_);
    if (ret != 0) {
        close(fd);
        if (errno == EEXIST) {
            // Policy already exist with different
            SSLOG_E("Policy already exist with different");
        }
        return -1;
    }
    close(fd);
    SSLOG_D("The directory has encrypted policy.\n");
    return 0;
}

int32_t CryptoPolicy::Load(const std::string &fname)
{
    SSLOG_I("Entry Load");
    // Load policy from the file
    std::string content;
    int flags = O_RDONLY | O_CLOEXEC;
    int fd = TEMP_FAILURE_RETRY(open(fname.c_str(), flags));
    if (fd == -1) {
        SSLOG_E("open file failed");
        return -1;
    }
    struct stat sb;
    if (fstat(fd, &sb) != -1 && sb.st_size > 0) {
        content.reserve(sb.st_size);
    }

    char buf[1024] __attribute__((__uninitialized__));
    ssize_t n;
    while ((n = TEMP_FAILURE_RETRY(read(fd, &buf[0], sizeof(buf)))) > 0) {
        content.append(buf, n);
    }
    close(fd);
    nlohmann::json jsonObj = nlohmann::json::parse(content);
    FromJson(jsonObj);
    if (content.size() > 0) {
        SSLOG_D("FromJson success");
        return 0;
    }
    SSLOG_E("FromJson failed");
    return -1;
}
int32_t CryptoPolicy::Delete(const std::string &fname)
{
    struct stat st;
    int result = lstat(fname.c_str(), &st);
    bool fileTypeRemovable = (result == 0 && (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)));
    if (fileTypeRemovable == 0) {
        SSLOG_E("is not a regular or symbol link file  %{public}d", fileTypeRemovable);
        return -1;
    }
    if (unlink(fname.c_str()) == -1) {
        SSLOG_E("Delete File a regular or symbol link file ");
        return -1;
    }
    return 0;
}

int32_t CryptoPolicy::Save(const std::string &fname)
{
    // save current policy to file
    SSLOG_I("Entry Save");
    ssize_t left = 0;
    int flags = O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC;
    int fd = TEMP_FAILURE_RETRY(open(fname.c_str(), flags, 0666));
    if (fd == -1) {
        SSLOG_E("open file failed");
        return false;
    }
    std::string content;
    nlohmann::json jsonObj;
    ToJson(jsonObj);
    // Ident set as 2
    content.append(jsonObj.dump(2));
    const char *p = content.data();
    left = content.size();
    while (left > 0) {
        ssize_t n = TEMP_FAILURE_RETRY(write(fd, p, left));
        if (n == -1) {
            close(fd);
            SSLOG_E("write data failed");
            return -1;
        }
        p += n;
        left -= n;
    }
    fsync(fd);
    close(fd);
    return 0;
}
int32_t CryptoPolicy::Copy(CryptoPolicy &policy)
{
    SSLOG_D("entry Copy\n");
    cryptoPolicy_.version = policy.cryptoPolicy_.version;
    cryptoPolicy_.contents_encryption_mode = policy.cryptoPolicy_.contents_encryption_mode;
    cryptoPolicy_.filenames_encryption_mode = policy.cryptoPolicy_.filenames_encryption_mode;
    cryptoPolicy_.flags = policy.cryptoPolicy_.flags;
    memcpy(reinterpret_cast<void *>(&cryptoPolicy_.master_key_descriptor[0]),
           reinterpret_cast<void *>(&policy.cryptoPolicy_.master_key_descriptor[0]),
           SSCRYPTO_DESCRIPTOR_SIZE);
    isHwWrappedKey_ = policy.isHwWrappedKey_;
    keyAlias_.data = nullptr;
    KeyUtils::InitKeyBlob(keyAlias_, policy.keyAlias_.size);
    memcpy(keyAlias_.data, policy.keyAlias_.data, keyAlias_.size);
    encryptKey_.data = nullptr;
    KeyUtils::InitKeyBlob(encryptKey_, policy.encryptKey_.size);
    memcpy(encryptKey_.data, policy.encryptKey_.data, encryptKey_.size); 
    // Copy from another policy
    SSLOG_D("Copy from another policy");
    return 0;
}
bool CryptoPolicy::IsValid()
{
    SSLOG_I("Entry IsValid");
    // check all the policy if is valid
    SSLOG_D("check all the policy if is valid");
    if (cryptoPolicy_.version != SSCRYPTO_POLICY_V1) {
        return false;
    }
    if (cryptoPolicy_.contents_encryption_mode == FS_ENCRYPTION_MODE_INVALID ||
        cryptoPolicy_.contents_encryption_mode > FS_ENCRYPTION_MODE_AES_256_CTS) {
        return false;
    }
    if (cryptoPolicy_.filenames_encryption_mode == FS_ENCRYPTION_MODE_INVALID ||
        cryptoPolicy_.filenames_encryption_mode > FS_ENCRYPTION_MODE_AES_256_CTS) {
        return false;
    }
    if (keyAlias_.size == 0 || keyAlias_.size != SSCRYPTO_DESCRIPTOR_SIZE) {
        // invalid key descriptor
        SSLOG_E("error invalid key descriptor");
        return false;
    }
    return true;
}
int32_t CryptoPolicy::LookupEncrytMode(const std::string &mode)
{
    SSLOG_I("Entry LookupEncrytMode");
    const std::map<std::string, int32_t> modeTable = {
        { "aes-256-xts", FS_ENCRYPTION_MODE_AES_256_XTS },
        { "software", FS_ENCRYPTION_MODE_AES_256_XTS },
        { "aes-256-cts", FS_ENCRYPTION_MODE_AES_256_CTS },
    };
    auto md = modeTable.find(mode);
    if (md == modeTable.end()) {
        SSLOG_E("error invalid mode");
        return FS_ENCRYPTION_MODE_INVALID;
    }
    return md->second;
}

bool CryptoPolicy::Init(const std::string &option)
{
    SSLOG_I("Entry Init");
    auto parts = Split(option, ":");
    // Encryption param max 3 parts
    if (parts.size() > 3) {
        // "Invalid encryption options
        SSLOG_E("error invalid encryption options");
        return false;
    }
    cryptoPolicy_.contents_encryption_mode = FS_ENCRYPTION_MODE_AES_256_XTS;
    if (parts.size() > 0 && !parts[0].empty()) {
        cryptoPolicy_.contents_encryption_mode = LookupEncrytMode(parts[0]);
    }
    cryptoPolicy_.filenames_encryption_mode = FS_ENCRYPTION_MODE_AES_256_CTS;
    if (parts.size() > 1 && !parts[1].empty()) {
        cryptoPolicy_.filenames_encryption_mode = LookupEncrytMode(parts[1]);
    }
    if (cryptoPolicy_.contents_encryption_mode == FS_ENCRYPTION_MODE_INVALID ||
        cryptoPolicy_.filenames_encryption_mode == FS_ENCRYPTION_MODE_INVALID) {
        // invalid policy
        SSLOG_E("error invalid policy");
        return false;
    }
    cryptoPolicy_.version = SSCRYPTO_POLICY_V1;
    cryptoPolicy_.flags = 0;
    isHwWrappedKey_ = false;
    if (parts.size() > NUM_TWO_ && !parts[NUM_TWO_].empty()) {
        auto flags = Split(parts[NUM_TWO_], "+");
        for (auto flag : flags) {
            if (flag == "v1") {
                cryptoPolicy_.version = SSCRYPTO_POLICY_V1;
            } else if (flag == "wrappedkey_v0") {
                isHwWrappedKey_ = true;
            } else {
                // unsupport parameter in this platform
                SSLOG_E("error unsupport parameter in this platform");
                return false;
            }
        }
    }
    if (cryptoPolicy_.version == SSCRYPTO_POLICY_V1 &&
        cryptoPolicy_.filenames_encryption_mode == FS_ENCRYPTION_MODE_AES_256_CTS) {
        cryptoPolicy_.flags |= FS_POLICY_FLAGS_PAD_4;
    } else {
        cryptoPolicy_.flags |= FS_POLICY_FLAGS_PAD_16;
    }
    return true;
}

std::string CryptoPolicy::KeyblobToString(const HksBlob &key)
{
    SSLOG_I("Entry KeyblobToString");
    std::string hex;
    const char *kLookup = "0123456789abcdef";
    for (size_t i = 0; i < key.size; i++) {
        hex.push_back(kLookup[(key.data[i] & 0xF0) >> NUM_FOUR_]);
        hex.push_back(kLookup[key.data[i] & 0x0F]);
    }
    return hex;
}
void CryptoPolicy::StringToKeyblob(const std::string &hex, HksBlob &key)
{
    SSLOG_I("Entry StringToKeyblob");
    if (hex.empty()) {
        return;
    }
    key.data = nullptr;
    KeyUtils::InitKeyBlob(key, hex.size() / NUM_TWO_);
    uint8_t cur = 0;
    int len = 0;
    for (size_t i = 0; i < hex.size(); i++) {
        int valHigh = toupper(hex[i]) - '0';
        int valLow = toupper(hex[++i]) - '0';
        if (valHigh > MAX_DIGIT_NUM) {
            valHigh -= DIFF_DIGIT_CHAR;
        }
        if (valLow > MAX_DIGIT_NUM) {
            valLow -= DIFF_DIGIT_CHAR;
        }
        cur = (valHigh << NUM_FOUR_) | valLow;
        key.data[len] = cur;
        len++;
        cur = 0;
    }
}
void CryptoPolicy::ToJson(nlohmann::json &jo)
{
    SSLOG_I("Entry ToJson");
    jo = {
        { "version", cryptoPolicy_.version },
        { "filename_mode", cryptoPolicy_.filenames_encryption_mode },
        { "content_mode", cryptoPolicy_.contents_encryption_mode },
        { "flags", cryptoPolicy_.flags },
        { "is_hw_wrap", isHwWrappedKey_ },
        { "key_alias", KeyblobToString(keyAlias_) },
        { "encrypt_key", KeyblobToString(encryptKey_) }
    };
}
void CryptoPolicy::FromJson(const nlohmann::json &jo)
{
    SSLOG_I("Entry FromJson");
    cryptoPolicy_.version = jo["version"];
    cryptoPolicy_.filenames_encryption_mode = jo["filename_mode"];
    cryptoPolicy_.contents_encryption_mode = jo["content_mode"];
    cryptoPolicy_.flags = jo["flags"];
    isHwWrappedKey_ = jo["is_hw_wrap"];
    std::string key = jo["key_alias"];
    StringToKeyblob(key, keyAlias_);
    key.clear();
    key = jo["encrypt_key"];
    StringToKeyblob(key, encryptKey_);
    if (keyAlias_.size == SSCRYPTO_DESCRIPTOR_SIZE) {
        memcpy(&cryptoPolicy_.master_key_descriptor[0], keyAlias_.data, keyAlias_.size);
    }
}
} // namespace OHOS

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

#ifndef FOUNDATION_FILEMANAGEMENT_FILEAPI_INTERFACES_INNERKITS_SECURITY_LABEL_H
#define FOUNDATION_FILEMANAGEMENT_FILEAPI_INTERFACES_INNERKITS_SECURITY_LABEL_H


#include <set>
#include <string>

#include <errno.h>
#include <sys/xattr.h>

namespace OHOS {
namespace DistributedFS {
namespace ModuleSecurityLabel {
constexpr char xattr_key[] = {"user.security"};
const std::string DEFAULT_DATA_LEVEL = "s3";
const std::set<std::string> DATA_LEVEL = {"s0", "s1", "s2", "s3", "s4"};
class SecurityLabel {
public:
    static bool SetSecurityLabel(std::string path, std::string dataLevel)
    {
        if (DATA_LEVEL.count(dataLevel) != 1) {
            return false;
        }
        if (setxattr(path.c_str(), xattr_key, dataLevel.c_str(), dataLevel.size(), 0) < 0) {
            return false;
        }
        return true;
    }

    static std::string GetSecurityLabel(std::string path)
    {
        auto xattr_value_size = getxattr(path.c_str(), xattr_key, NULL, 0);
        if (xattr_value_size == -1 || errno == ENOTSUP) {
            return "";
        }
        if (xattr_value_size <= 0) {
            return DEFAULT_DATA_LEVEL;
        }
        char *xattr_value = new char[xattr_value_size + 1];
        if (xattr_value == nullptr) {
            return "";
        } else {
            xattr_value_size = getxattr(path.c_str(), xattr_key, xattr_value, xattr_value_size);
            if (xattr_value_size == -1 || errno == ENOTSUP) {
                return "";
            }
            if (xattr_value_size <= 0) {
                return DEFAULT_DATA_LEVEL;
            }
            xattr_value[xattr_value_size] = 0;
            std::string result(xattr_value);
            delete[] xattr_value;
            return result;
        }
    }
};
} // namespace ModuleSecurityLabel
} // namespace FileIO
} // namespace OHOS
#endif
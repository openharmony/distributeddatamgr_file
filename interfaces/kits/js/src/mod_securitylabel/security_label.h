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

#include <cerrno>
#include <memory>
#include <sys/xattr.h>

namespace OHOS {
namespace DistributedFS {
namespace ModuleSecurityLabel {
const char XATTR_KEY[] = {"user.security"};
const std::string DEFAULT_DATA_LEVEL = "s3";
const std::set<std::string> DATA_LEVEL = {"s0", "s1", "s2", "s3", "s4"};
class SecurityLabel {
public:
    static bool SetSecurityLabel(const std::string &path, const std::string &dataLevel)
    {
        if (DATA_LEVEL.count(dataLevel) != 1) {
            return false;
        }
        if (setxattr(path.c_str(), XATTR_KEY, dataLevel.c_str(), dataLevel.size(), 0) < 0) {
            return false;
        }
        return true;
    }

    static std::string GetSecurityLabel(const std::string &path)
    {
        auto xattrValueSize = getxattr(path.c_str(), XATTR_KEY, NULL, 0);
        if (xattrValueSize == -1 || errno == ENOTSUP) {
            return "";
        }
        if (xattrValueSize <= 0) {
            return DEFAULT_DATA_LEVEL;
        }
        char *xattrValue = new char[xattrValueSize + 1];
        if (xattrValue == nullptr) {
            return "";
        }

        xattrValueSize = getxattr(path.c_str(), XATTR_KEY, xattrValue, xattrValueSize);
        if (xattrValueSize == -1 || errno == ENOTSUP) {
            delete[] xattrValue;
            return "";
        }
        if (xattrValueSize <= 0) {
            delete[] xattrValue;
            return DEFAULT_DATA_LEVEL;
        }
        xattrValue[xattrValueSize] = 0;
        std::string result(xattrValue);
        delete[] xattrValue;
        return result;
    }
};
} // namespace ModuleSecurityLabel
} // namespace FileIO
} // namespace OHOS
#endif
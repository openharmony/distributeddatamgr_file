/*
 * Copyright (c) 2021 Huawei Device Co., Ltd. 2021-2021.
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
#ifndef STORAGE_SERVICE_UTILS_STRINGS_H
#define STORAGE_SERVICE_UTILS_STRINGS_H

#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

#include <string_view>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <type_traits>
#include <limits>

namespace OHOS {
namespace SsUtils {
template <typename ContainerT, typename SeparatorT>
std::string Join(const ContainerT &things, SeparatorT separator)
{
    if (things.empty()) {
        return "";
    }
    std::ostringstream result;
    result << *things.begin();
    for (auto it = std::next(things.begin()); it != things.end(); ++it) {
        result << separator << *it;
    }
    return result.str();
}
bool FindValue(const std::string &raw, const std::string &key, std::string *value);
std::vector<std::string> Split(const std::string &s, const std::string &delimiters);
std::vector<std::string> DiskSplit(const std::string &s, const std::string &delimiters);
std::string StringPrintf(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
std::string Trim(const std::string &s);
int StrToHex(const std::string &str, std::string &hex);
bool StartsWith(std::string s, std::string prefix);
bool EqualsIgnoreCase(std::string lhs, std::string rhs);
int HexToStr(const std::string &hex, std::string &str);
int NormalizeHex(const std::string &in, std::string &out);
} // namespace SsUtils
} // namespace OHOS

#endif // STORAGE_SERVICE_UTILS_STRINGS_H

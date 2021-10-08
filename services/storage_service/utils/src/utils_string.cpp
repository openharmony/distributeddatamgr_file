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
#include "utils_string.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <ftw.h>
#include <libgen.h>
#include <memory>
#include <mutex>
#include <regex>
#include <string_view>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <numeric>
#include "storage_hilog.h"
namespace OHOS {
namespace SsUtils {
constexpr int MAX_DIGIT_NUM = 9;
constexpr int DIFF_DIGIT_CHAR = 7;
static int numberTwo = 2;
static int numberFour = 4;
int HexToStr(const std::string &hex, std::string &str)
{
    str.clear();
    char cur = 0;
    for (size_t i = 0; i < hex.size(); i++) {
        int valLow = toupper(hex[i]) - '0';
        int valHigh = toupper(hex[i++]) - '0';
        if (valHigh > MAX_DIGIT_NUM) {
            valHigh -= DIFF_DIGIT_CHAR;
        }
        if (valLow > MAX_DIGIT_NUM) {
            valLow -= DIFF_DIGIT_CHAR;
        }
        cur = (valHigh << numberFour) | valLow;
        str.push_back(cur);
        cur = 0;
    }
    return 0;
}

std::vector<std::string> Split(const std::string &in, const std::string &delimiters)
{
    std::regex reg{ delimiters };
    return std::vector<std::string>{ std::sregex_token_iterator(in.begin(), in.end(), reg, -1),
                                     std::sregex_token_iterator() };
}

std::vector<std::string> DiskSplit(const std::string &s, const std::string &delimiters)
{
    std::vector<std::string> result;
    size_t base = 0;
    size_t found;
    while (true) {
        found = s.find_first_of(delimiters, base);
        result.push_back(s.substr(base, found - base));
        if (found == s.npos) {
            break;
        }
        base = found + 1;
    }
    return result;
}

std::string Trim(const std::string &s)
{
    size_t len = s.size();
    if (len == 0) {
        return "";
    }
    size_t start = 0;
    size_t end = len - 1;

    while ((start < len) && (isspace(s[start]))) {
        start++;
    }

    while ((end >= start) && (isspace(s[end]))) {
        end--;
    }
    return (end < start ? "" : s.substr(start, end - start + 1));
}

std::string StringPrintf(const char *fmt, ...)
{
    std::string result;
    long buf_size = 256; // something reasonable to begin with
    va_list args;
    va_start(args, fmt);
    while (true) {
        char *buf = new char[buf_size];
        long ret_size = vsnprintf(buf, buf_size, fmt, args);
        if ((ret_size >= 0) && (ret_size < buf_size)) {
            buf[ret_size] = 0;
            result.append(buf, ret_size);
            delete[] buf;
            break;
        }
        if (ret_size < 0) {
            buf_size *= numberTwo;
        } else {
            buf_size = ret_size + 1;
        }
        delete[] buf;
    }
    va_end(args);
    return result;
}

bool StartsWith(std::string s, std::string prefix)
{
    return s.substr(0, prefix.size()) == prefix;
}

bool EqualsIgnoreCase(std::string lhs, std::string rhs)
{
    return lhs.size() == rhs.size() && strncasecmp(lhs.data(), rhs.data(), lhs.size()) == 0;
}

bool FindValue(const std::string &raw, const std::string &key, std::string *value)
{
    auto qual = key + "=\"";
    size_t start = 0;
    while (true) {
        start = raw.find(qual, start);
        if (start == std::string::npos) {
            return false;
        }
        if (start == 0 || raw[start - 1] == ' ') {
            break;
        }
        start += 1;
    }
    start += qual.length();
    auto end = raw.find("\"", start);
    if (end == std::string::npos) {
        return false;
    }
    *value = raw.substr(start, end - start);
    return true;
}

static const char *hexTable = "0123456789abcdef";

int StrToHex(const std::string &str, std::string &hex)
{
    hex.clear();
    size_t len = str.size();
    for (size_t i = 0; i < len; i++) {
        hex.push_back(hexTable[(str[i] & 0xF0) >> numberFour]);
        hex.push_back(hexTable[str[i] & 0x0F]);
    }
    return 0;
}

int NormalizeHex(const std::string &in, std::string &out)
{
    std::string tmp = {};
    if (HexToStr(in, tmp)) {
        return -1;
    }
    return StrToHex(tmp, out);
}
} // namespace SsUtils
} // namespace OHOS

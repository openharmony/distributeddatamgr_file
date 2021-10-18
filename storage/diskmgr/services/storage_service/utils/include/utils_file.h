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
#ifndef STORAGE_SERVICE_UTILS_FILE_HANDLE_H
#define STORAGE_SERVICE_UTILS_FILE_HANDLE_H
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <cstring>
#include <memory>
#include <vector>
#include <stddef.h>
#include <utility>
#include <algorithm>

#include "storage_utils.h"

namespace OHOS {
namespace SsUtils {
bool IsDirEmpty(const std::string &path);
int PrepareDir(const std::string &path, mode_t mode, uid_t uid, gid_t gid);

int64_t GetSysTime();

#define DISALLOW_COPY_AND_ASSIGN(TypeName)                                                                   \
    TypeName(const TypeName &) = delete;                                                                     \
    void operator=(const TypeName &) = delete

template <typename T, size_t N> constexpr size_t array_length(T (&)[N])
{
    return N;
}
#ifndef O_BINARY
#    define O_BINARY 0
#endif
bool WriteFile(const std::string &content, const std::string &path);
bool ReadFd(int fd, std::string *content);
bool ReadFile(const std::string &path, std::string *content);
bool WriteFd(const std::string &content, int fd);
bool Readlink(const std::string &path, std::string *result);
int Symlink(const std::string &target, const std::string &linkpath);
int Unlink(const std::string &linkpath);
bool CheckSymlink(const std::string &path, const std::string &prefix);
} // namespace SsUtils
} // namespace OHOS

#endif // STORAGE_SERVICE_UTILS_FILE_HANDLE_H

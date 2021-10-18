/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef STORAGEUTILS_H
#define STORAGEUTILS_H
#include "utils_file.h"

#include <iostream>
#include <map>
#include <string>
#include <chrono>
#include <linux/fiemap.h>
namespace OHOS {
namespace SsUtils {
int MountForBind(const std::string &source, const std::string &target);

int KillProcForPath(const std::string &path);
int KillProcWithFiles(const std::string &path, int signal);

int ForceUnMount(const std::string &path);
int ExecuteCmd(const char *execveHandlepath, const std::vector<std::string> &args);
int ExecuteCmd(const char *execveHandlepath,
               const std::vector<std::string> &args,
               std::vector<std::string> *output);
pid_t ExecuteCmdAync(const char *execvpHandlepath, const std::vector<std::string> &args);

int ReadDevInfo(const std::string &path, std::string *fsType, std::string *fsUuid, std::string *fsLabel);

std::string BuildKeyPath(const std::string &partGuid);

int ReadRandom(size_t bytes, std::string &out);
int ReadRandom(size_t bytes, char *buffer);
int GenerateRandomUuid(std::string &out);

int GetPid(const char *s);
bool IsFilesystemSupported(const std::string &fsType);
bool IsSdcardfsUsed();
uint64_t GetFreeSize(const std::string &path);
int64_t GetDirSize(const std::string &path);
} // namespace SsUtils
} // namespace OHOS.

#endif // STORAGEUTILS_H

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
#ifndef STORAGE_SERVICE_MGR_NTFS_H
#define STORAGE_SERVICE_MGR_NTFS_H

#include <string>

namespace OHOS {
namespace StorageService {
class NtfsMgr {
public:
    static bool IsSupported();

    static int Check(const std::string &source);
    static int Mount(const std::string &source, const std::string &target);
    static int Format(const std::string &source);

private:
    static const std::string kMkfsPath;
    static const std::string kFsckPath;
}; // namespace ntfs
} // namespace StorageService
} // namespace OHOS

#endif // STORAGE_SERVICE_MGR_NTFS_H

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
#include "crypto_path.h"

#include <cerrno>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/fs.h>

#include "storage_hilog.h"
#include "storage_utils.h"

namespace OHOS {
int32_t CryptoPath::Create()
{
    SSLOG_I("Entry Create");
    if (SsUtils::PrepareDir(path, mode, uid, gid) != 0) {
        SSLOG_E("failed to create path: %{public}s", path.c_str());
        return -1;
    }
    return 0;
}
int32_t CryptoPath::Delete()
{
    if (rmdir(path.c_str()) != 0 && errno != ENOENT) {
        SSLOG_E("failed to Delete path: %{public}s", path.c_str());
        return -1;
    }
    return 0;
}
bool CryptoPath::IsExist()
{
    SSLOG_I("Entry IsExist");
    if (access(path.c_str(), F_OK) != 0 || errno != ENOENT) {
        SSLOG_E("%{public}s is not exist", path.c_str());
        return false;
    }
    SSLOG_D("%{public}s is exist", path.c_str());
    return true;
}
} // namespace OHOS
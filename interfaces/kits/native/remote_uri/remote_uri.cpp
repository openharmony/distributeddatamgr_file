/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "remote_uri.h"
#include <string>
#include <fcntl.h>
#include <tuple>
#include <unistd.h>
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "hap_token_info.h"

namespace OHOS {
namespace DistributedFS {
namespace ModuleRemoteUri {
namespace {
    const std::string FRAGMENT_TAG = "#";
    const std::string FD_TAG = "=";
    const std::string REMOTE_URI_TAG = "fdFromBinder";
    const std::string SCHEME_TAG = ":";
    const std::string SCHEME = "datashare";
}

static std::string GetCallingPkgName()
{
    uint32_t pid = IPCSkeleton::GetCallingTokenID();
    Security::AccessToken::HapTokenInfo tokenInfo = Security::AccessToken::HapTokenInfo();
    Security::AccessToken::AccessTokenKit::GetHapTokenInfo(pid, tokenInfo);
    return tokenInfo.bundleName;
}

bool RemoteUri::IsRemoteUri(const std::string& path, int &fd, const int& flags)
{
    std::string::size_type posDatashare = path.find(SCHEME_TAG);
    std::string::size_type posFragment = path.find(FRAGMENT_TAG);
    std::string::size_type posFd = path.find(FD_TAG);
    if (posDatashare == std::string::npos || posFragment == std::string::npos ||
            posFd == std::string::npos) {
        return false;
    }
   
    std::string scheme = path.substr(0, posDatashare);
    if (scheme != SCHEME) {
        return false;
    }

    std::string fragment = path.substr(posFragment + 1, REMOTE_URI_TAG.size());
    if (fragment == REMOTE_URI_TAG) {
        std::string fdStr = path.substr(posFd + 1);
        fd = atoi(fdStr.c_str());
        if (fd < 0 || flags != O_RDONLY) {
            fd = -1;
        }
        return true;
    }
    return false;
}

int RemoteUri::ConvertUri(const int &fd, std::string &remoteUri)
{
    if (fd < 0) {
        return EINVAL;
    }
    std::string pkgName = GetCallingPkgName();
    remoteUri = SCHEME + ":///" + pkgName + "/" + FRAGMENT_TAG +
                            REMOTE_URI_TAG + FD_TAG + std::to_string(fd);
    return 0;
}

int RemoteUri::OpenRemoteUri(const std::string &remoteUri)
{
    int fd = -1;
    IsRemoteUri(remoteUri, fd);
    return fd;
}
} // namespace ModuleRemoteUri
} // namespace DistributedFS
} // namespace OHOS

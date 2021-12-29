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

#include "fd_guard.h"
#include <unistd.h>
namespace OHOS {
namespace DistributedFS {
FDGuard::FDGuard(int fd) : fd_(fd) {}

FDGuard::FDGuard(int fd, bool autoDestruct) : fd_(fd), autoDestruct_(autoDestruct) {}

FDGuard::FDGuard(const FDGuard& fdg) : fd_(fdg.fd_), autoDestruct_(fdg.autoDestruct_) {}

FDGuard& FDGuard::operator=(const FDGuard& fdg)
{
    if (this == &fdg) {
        return *this;
    }
    this->fd_ = fdg.fd_;
    this->autoDestruct_ = fdg.autoDestruct_;
    return *this;
}

FDGuard::~FDGuard()
{
    if (fd_ > 0 && autoDestruct_) {
        close(fd_);
    }
}

int FDGuard::GetFD() const
{
    return fd_;
}

void FDGuard::SetFD(int fd, bool autoDestruct)
{
    fd_ = fd;
    autoDestruct_ = autoDestruct;
}

void FDGuard::operator=(int fd)
{
    fd_ = fd;
}

void FDGuard::ClearFD()
{
    fd_ = -1;
}
} // namespace DistributedFS
} // namespace OHOS

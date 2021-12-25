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

#ifndef FD_GUARD_H
#define FD_GUARD_H

namespace OHOS {
namespace DistributedFS {
class FDGuard {
public:
    FDGuard() = default;
    FDGuard(int fd, bool autoDestruct);
    FDGuard(const FDGuard& fdg);
    FDGuard& operator=(const FDGuard& fdg);
    FDGuard(FDGuard& fdg);
    explicit FDGuard(int fd);
    ~FDGuard();

    int GetFD() const;
    void SetFD(int fd);
    void SetFD(int fd, bool autoDestruct);
    void ClearFD();
    void operator=(int fd);

private:
    int fd_ = -1;
    bool autoDestruct_ = true;
};
} // namespace DistributedFS
} // namespace OHOS
#endif
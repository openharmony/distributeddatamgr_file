/*
 *  Copyright (C) 2021 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "key_ctrl.h"

#include <cctype>
#include <cstdarg>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/fs.h>
#include <linux/keyctl.h>

namespace OHOS {
int32_t KeyCtrl::AddKey(const char *type,
                        const char *description,
                        const void *payload,
                        size_t payload_length,
                        int32_t ring_id)
{
    return syscall(__NR_add_key, type, description, payload, payload_length, ring_id);
}

int32_t KeyCtrl::GetKeyring(int32_t id, int create)
{
    return syscall(__NR_keyctl, KEYCTL_GET_KEYRING_ID, id, create);
}

long KeyCtrl::Revoke(int32_t id)
{
    return syscall(__NR_keyctl, KEYCTL_REVOKE, id);
}

long KeyCtrl::Search(int32_t ring_id,
                     const char *type,
                     const char *description,
                     int32_t dest_ring_id)
{
    return syscall(__NR_keyctl, KEYCTL_SEARCH, ring_id, type, description, dest_ring_id);
}

long KeyCtrl::SetPermission(int32_t id, int permissions)
{
    return syscall(__NR_keyctl, KEYCTL_SETPERM, id, permissions);
}

long KeyCtrl::Unlink(int32_t key, int32_t keyring)
{
    return syscall(__NR_keyctl, KEYCTL_UNLINK, key, keyring);
}

long KeyCtrl::RestrictKeyring(int32_t keyring, const char *type, const char *restriction)
{
    return syscall(__NR_keyctl, KEYCTL_RESTRICT_KEYRING, keyring, type, restriction);
}

long KeyCtrl::GetSecurity(int32_t id, char *buffer, size_t buflen)
{
    return syscall(__NR_keyctl, KEYCTL_GET_SECURITY, id, buffer, buflen);
}
} // namespace OHOS
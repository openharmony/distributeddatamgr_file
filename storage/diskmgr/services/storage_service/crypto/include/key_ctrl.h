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
#ifndef STORAGE_SERVICE_SSCRYPTO_KEYCTL_H_
#define STORAGE_SERVICE_SSCRYPTO_KEYCTL_H_
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <linux/fs.h>
#include <linux/keyctl.h>

namespace OHOS {
class KeyCtrl {
public:
    static int32_t AddKey(const char *type,
                               const char *description,
                               const void *payload,
                               size_t payload_length,
                               int32_t ring_id);

    static int32_t GetKeyring(int32_t id, int create);

    static long Revoke(int32_t id);

    static long
    Search(int32_t ring_id, const char *type, const char *description, int32_t dest_ring_id);

    static long SetPermission(int32_t id, int permissions);

    static long Unlink(int32_t key, int32_t keyring);

    static long RestrictKeyring(int32_t keyring, const char *type, const char *restriction);

    static long GetSecurity(int32_t key, char *buffer, size_t buflen);
};
} // namespace OHOS
#endif // STORAGE_SERVICE_SSCRYPTO_KEYCTL_H_
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

#include "document_n_exporter.h"

#include <cerrno>

#include "../common/uni_error.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace DistributedFS {
namespace ModuleDocument {
napi_value Choose(napi_env env, napi_callback_info info)
{
    UniError(EINVAL).ThrowErr(env, "error");
    return nullptr;
}

napi_value Show(napi_env env, napi_callback_info info)
{
    UniError(EINVAL).ThrowErr(env, "error");
    return nullptr;
}
} // namespace ModuleDocument
} // namespace DistributedFS
} // namespace OHOS
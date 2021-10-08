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
#include "module.h"

#include <memory>
#include <vector>

#include "../common/log.h"
#include "file_picker_exporter.h"

using namespace std;

namespace OHOS {
namespace DistributedFS {
namespace ModuleFPExpoter {
static napi_value Export(napi_env env, napi_value exports)
{
    std::vector<std::unique_ptr<OHOS::DistributedFS::NExporter>> products;
    products.emplace_back(make_unique<FilePickerExporter>(env, exports));

    for (auto &&product : products) {
        if (!product->Export()) {
            HILOGE("INNER BUG. Failed to export class %{public}s for module filepicker",
                   product->GetClassName().c_str());
            return nullptr;
        } else {
            HILOGE("Class %{public}s for module file has been exported", product->GetClassName().c_str());
        }
    }
    return exports;
}

NAPI_MODULE(filepicker, Export)
} // namespace ModuleFMSExpoter
} // namespace DistributedFS
} // namespace OHOS
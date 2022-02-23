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

#include "open_dir.h"
#include <memory>
#include <string>
#include <tuple>

#include "../../common/napi/n_async/n_async_work_callback.h"
#include "../../common/napi/n_async/n_async_work_promise.h"
#include "../../common/napi/n_class.h"
#include "../../common/napi/n_func_arg.h"
#include "../../common/napi/n_val.h"
#include "../../common/uni_error.h"
#include "../class_dir/dir_entity.h"
#include "../class_dir/dir_n_exporter.h"

namespace OHOS {
namespace DistributedFS {
namespace ModuleFileIO {
using namespace std;

napi_value OpenDir::Sync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    bool succ = false;
    unique_ptr<char[]> path;
    tie(succ, path, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid path");
        return nullptr;
    }

    std::unique_ptr<DIR, std::function<void(DIR *)>> dir = { opendir(path.get()), closedir };
    if (!dir) {
        UniError(errno).ThrowErr(env);
        return nullptr;
    }

    napi_value objDir = NClass::InstantiateClass(env, DirNExporter::className_, {});
    if (!objDir) {
        UniError(EINVAL).ThrowErr(env, "Cannot instantiate class DirSync");
        return nullptr;
    }

    auto dirEntity = NClass::GetEntityOf<DirEntity>(env, objDir);
    if (!dirEntity) {
        UniError(EINVAL).ThrowErr(env, "Cannot get the entity of objDir");
        return nullptr;
    }
    dirEntity->dir_.swap(dir);
    return objDir;
}

struct OpenDirArgs {
    NRef dirPtr_;
    DIR *dir = nullptr;
    explicit OpenDirArgs(NVal dirPtr) : dirPtr_(dirPtr) {}
    ~OpenDirArgs() = default;
};

napi_value OpenDir::Async(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    string path;
    unique_ptr<char[]> tmp;
    bool succ = false;
    tie(succ, tmp, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid path");
        return nullptr;
    }

    path = tmp.get();
    auto arg = make_shared<OpenDirArgs>(NVal(env, funcArg.GetThisVar()));
    auto cbExec = [arg, path](napi_env env) -> UniError {
        DIR *dir = nullptr;
        dir = opendir(path.c_str());
        if (dir == nullptr) {
            return UniError(errno);
        }
        arg->dir = dir;
        return UniError(ERRNO_NOERR);
    };
    auto cbCompl = [arg](napi_env env, UniError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        std::unique_ptr<DIR, std::function<void(DIR *)>> dir = { arg->dir, closedir };
        if (!dir) {
            return { env, UniError(errno).GetNapiErr(env) };
        }

        napi_value objDir = NClass::InstantiateClass(env, DirNExporter::className_, {});
        if (!objDir) {
            return { env, UniError(EINVAL).GetNapiErr(env) };
        }

        auto dirEntity = NClass::GetEntityOf<DirEntity>(env, objDir);
        if (!dirEntity) {
            return { env, UniError(EINVAL).GetNapiErr(env) };
        }
        dirEntity->dir_.swap(dir);
        return { env, objDir };
    };

    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        return NAsyncWorkPromise(env, thisVar).Schedule("fileIOOpenDir", cbExec, cbCompl).val_;
    } else {
        NVal cb(env, funcArg[NARG_POS::SECOND]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule("fileIOOpenDir", cbExec, cbCompl).val_;
    }
}
} // namespace ModuleFileIO
} // namespace DistributedFS
} // namespace OHOS
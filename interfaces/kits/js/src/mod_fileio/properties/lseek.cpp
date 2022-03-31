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
#include "lseek.h"
#include <cstring>
#include <tuple>
#include <unistd.h>

#include "../../common/napi/n_async/n_async_work_callback.h"
#include "../../common/napi/n_async/n_async_work_promise.h"
#include "../../common/napi/n_func_arg.h"
namespace OHOS {
namespace DistributedFS {
namespace ModuleFileIO {
using namespace std;

static tuple<bool, int, int, int> GetLseekArg(napi_env env, const NFuncArg &funcArg)
{
    bool succ = false;
    int fd;
    tie(succ, fd) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid fd");
        return { false, -1, -1, -1 };
    }

    int offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid offset");
        return { false, -1, -1, -1 };
    }

    int whence;
    tie(succ, whence) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid whence");
        return { false, -1, -1, -1 };
    }

    return { succ, fd, offset, whence };
}

napi_value Lseek::Sync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    bool succ = false;
    int fd;
    int offset;
    int whence;
    tie(succ, fd, offset, whence) = GetLseekArg(env, funcArg);
    if (!succ) {
        return nullptr;
    }

    int ret = lseek(fd, offset, whence);
    if (ret == -1) {
        UniError(errno).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateInt64(env, ret).val_;
}

napi_value Lseek::Async(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    size_t argc = funcArg.GetArgc();
    bool succ = false;
    int fd;
    int offset;
    int whence;
    tie(succ, fd, offset, whence) = GetLseekArg(env, funcArg);
    if (!succ) {
        return nullptr;
    }

    auto arg = make_shared<int32_t>();
    auto cbExec = [fd, offset, whence, arg](napi_env env) -> UniError {
        int ret = lseek(fd, offset, whence);
        *arg = ret;
        if (ret == -1) {
            return UniError(errno);
        } else {
            return UniError(ERRNO_NOERR);
        }
    };

    auto cbComplCallback = [arg](napi_env env, UniError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateInt64(env, *arg) };
    };

    string procedureName = "FileIOLseek";
    NVal thisVar(env, funcArg.GetThisVar());
    if (argc == NARG_CNT::THREE) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplCallback).val_;
    } else {
        NVal cb(env, funcArg[NARG_POS::FOURTH]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplCallback).val_;
    }
}
} // namespace ModuleFileIO
} // namespace DistributedFS
} // namespace OHOS
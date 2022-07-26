/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
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

#include "read_dir.h"
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

napi_value ReadDir::Sync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char[]> path;
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
    std::vector<std::string> dirFiles;
    struct dirent* entry = readdir(dir.get());
    while (entry) {
        if (strcmp(entry->d_name, "") != 0 && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            dirFiles.push_back(entry->d_name);
        }
        entry = readdir(dir.get());
    }
    return NVal::CreateArrayString(env, dirFiles).val_;
}

struct ReadDirArgs {
    std::vector<std::string> dirFiles;
    explicit ReadDirArgs()
    {
        dirFiles = std::vector<std::string>();
    }
    ~ReadDirArgs() = default;
};

napi_value ReadDir::Async(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    std::string path;
    std::unique_ptr<char[]> tmp;
    bool succ = false;
    tie(succ, tmp, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid path");
        return nullptr;
    }

    path = tmp.get();
    auto arg = make_shared<ReadDirArgs>();
    auto cbExec = [arg, path](napi_env env) -> UniError {
        DIR *dir = nullptr;
        dir = opendir(path.c_str());
        if (dir == nullptr) {
            return UniError(errno);
        }
        struct dirent* entry = readdir(dir);
        std::vector<std::string> dirnames;
        while (entry) {
            if (strcmp(entry->d_name, "") != 0 && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                dirnames.push_back(entry->d_name);
            }
            entry = readdir(dir);
        }
        arg->dirFiles = dirnames;
        return UniError(ERRNO_NOERR);
    };
    auto cbCompl = [arg](napi_env env, UniError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        } else {
            return NVal::CreateArrayString(env, arg->dirFiles);
        }
    };

    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        return NAsyncWorkPromise(env, thisVar).Schedule("fileIOReadDir", cbExec, cbCompl).val_;
    } else {
        NVal cb(env, funcArg[NARG_POS::SECOND]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule("fileIOReadDir", cbExec, cbCompl).val_;
    }
}
} // namespace ModuleFileIO
} // namespace DistributedFS
} // namespace OHOS
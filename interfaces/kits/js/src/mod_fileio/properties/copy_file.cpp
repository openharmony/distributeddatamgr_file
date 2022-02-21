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

#include "copy_file.h"
#include <cstring>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>

#include "../../common/file_helper/fd_guard.h"
#include "../../common/napi/n_async/n_async_work_callback.h"
#include "../../common/napi/n_async/n_async_work_promise.h"
#include "../../common/napi/n_func_arg.h"

namespace OHOS {
namespace DistributedFS {
namespace ModuleFileIO {
using namespace std;

static tuple<bool, int, bool> GetCopyFileModeAndProm(napi_env env, const NFuncArg &funcArg)
{
    bool succ = false;
    bool promise = false;
    bool hasMode = false;
    int mode = 0;
    if (funcArg.GetArgc() == static_cast<size_t>(NARG_CNT::THREE) &&
                             NVal(env, funcArg[static_cast<size_t>(NARG_POS::THIRD)]).TypeIs(napi_number)) {
        promise = true;
        hasMode = true;
    } else if (funcArg.GetArgc() == static_cast<size_t>(NARG_CNT::FOUR)) {
        hasMode = true;
    }
    if (hasMode) {
        tie(succ, mode) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::THIRD)]).ToInt32();
        if (!succ) {
            return { false, mode, promise };
        }
    }
    return { true, mode, promise };
}

struct FileInfo {
    bool isPath = false;
    unique_ptr<char[]> path;
    FDGuard fdg;
    FileInfo(const FileInfo &fileInfo) : isPath(fileInfo.isPath), fdg(fileInfo.fdg) {};
    FileInfo() = default;
    FileInfo &operator = (const FileInfo &fileInfo);
};

static FDGuard SetFDGuard(FileInfo fileInfo, const string &path, function<int(const char *)> executor)
{
    FDGuard fdg;
    if (fileInfo.isPath) {
        fdg.SetFD(executor(path.c_str()));
    } else {
        fdg.SetFD(fileInfo.fdg.GetFD(), false);
    }
    return fdg;
}

static UniError HandleCopyFile(FileInfo srcFileInfo, FileInfo destFileInfo,
                               const string &srcPath, const string &destPath)
{
    FDGuard sfd = SetFDGuard(srcFileInfo, srcPath, [](const char *buf) -> int { return open(buf, O_RDONLY, 0); });
    struct stat statbf;
    if (sfd.GetFD() == -1) {
        return UniError(errno);
    }
    if (fstat(sfd.GetFD(), &statbf) == -1) {
        return UniError(errno);
    }
    FDGuard ofd = SetFDGuard(destFileInfo, destPath,
        [statbf](const char *buf) -> int { return open(buf, O_WRONLY | O_CREAT, statbf.st_mode); });
    if (ofd.GetFD() == -1) {
        return UniError(errno);
    }
    if (sendfile(ofd.GetFD(), sfd.GetFD(), nullptr, statbf.st_size) == -1) {
        return UniError(errno);
    }
    if (ftruncate(ofd.GetFD(), statbf.st_size) == -1) {
        return UniError(errno);
    }
    return UniError(ERRNO_NOERR);
}

static bool GetCopyFileInfo(napi_env env, const NFuncArg &funcArg, FileInfo &srcFileInfo, FileInfo &destFileInfo)
{
    bool succ = false;
    if (NVal(env, funcArg[static_cast<size_t>(NARG_POS::FIRST)]).TypeIs(napi_string)) {
        tie(succ, srcFileInfo.path, ignore) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::FIRST)]).ToUTF8String();
        srcFileInfo.isPath = true;
    } else {
        srcFileInfo.fdg.SetFD(-1, false);
        tie(succ, srcFileInfo.fdg) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::FIRST)]).ToInt32();
    }
    if (!succ) {
        return succ;
    }
    if (NVal(env, funcArg[static_cast<size_t>(NARG_POS::SECOND)]).TypeIs(napi_string)) {
        tie(succ, destFileInfo.path, ignore) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::SECOND)]).ToUTF8String();
        destFileInfo.isPath = true;
    } else {
        destFileInfo.fdg.SetFD(-1, false);
        tie(succ, destFileInfo.fdg) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::SECOND)]).ToInt32();
    }
    return succ;
}

napi_value CopyFile::Sync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(static_cast<size_t>(NARG_CNT::TWO), static_cast<size_t>(NARG_CNT::THREE))) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    FileInfo srcFileInfo;
    FileInfo destFileInfo;
    succ = GetCopyFileInfo(env, funcArg, srcFileInfo, destFileInfo);
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid file path/fd");
        return nullptr;
    }
    int mode = 0;
    struct stat statbf;
    tie(succ, mode, ignore) = GetCopyFileModeAndProm(env, funcArg);
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid mode");
        return nullptr;
    }
    if (srcFileInfo.isPath) {
        srcFileInfo.fdg.SetFD(open(srcFileInfo.path.get(), O_RDONLY), true);
        if (srcFileInfo.fdg.GetFD() == -1) {
            UniError(errno).ThrowErr(env);
            return nullptr;
        }
    }
    int res = fstat(srcFileInfo.fdg.GetFD(), &statbf);
    if (res == -1) {
        UniError(errno).ThrowErr(env);
        return nullptr;
    }
    if (destFileInfo.isPath) {
        destFileInfo.fdg.SetFD(open(destFileInfo.path.get(), O_WRONLY | O_CREAT, statbf.st_mode), true);
        if (destFileInfo.fdg.GetFD() == -1) {
            UniError(errno).ThrowErr(env);
            return nullptr;
        }
    }
    int block = 4096;
    auto copyBuf = make_unique<char[]>(block);
    do {
        ssize_t readSize = read(srcFileInfo.fdg.GetFD(), copyBuf.get(), block);
        if (readSize == -1) {
            UniError(errno).ThrowErr(env);
            return nullptr;
        } else if (readSize == 0) {
            break;
        }
        ssize_t writeSize = write(destFileInfo.fdg.GetFD(), copyBuf.get(), readSize);
        if (writeSize != readSize) {
            UniError(errno).ThrowErr(env);
            return nullptr;
        }
        if (readSize != block) {
            break;
        }
    } while (true);
    return NVal::CreateUndefined(env).val_;
}

napi_value CopyFile::Async(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(static_cast<size_t>(NARG_CNT::TWO), static_cast<size_t>(NARG_CNT::FOUR))) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    int mode = 0;
    bool promise = false;
    tie(succ, mode, promise) = GetCopyFileModeAndProm(env, funcArg);
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid arg");
        return nullptr;
    }
    FileInfo srcFileInfo, destFileInfo;
    if (!GetCopyFileInfo(env, funcArg, srcFileInfo, destFileInfo)) {
        UniError(EINVAL).ThrowErr(env, "Invalid file path/fd");
        return nullptr;
    }
    string srcPath = (!srcFileInfo.isPath) ? "" : srcFileInfo.path.get();
    string destPath = (!destFileInfo.isPath) ? "" : destFileInfo.path.get();
    auto cbExec = [srcFileInfo, destFileInfo, srcPath, destPath, mode](napi_env env) -> UniError {
        return HandleCopyFile(srcFileInfo, destFileInfo, srcPath, destPath);
    };
    auto cbCompl = [](napi_env env, UniError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateUndefined(env) };
    };
    string procedureName = "FileIOCopyFile";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == static_cast<size_t>(NARG_CNT::TWO) || promise) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
    } else {
        NVal cb(env, funcArg[((funcArg.GetArgc() == static_cast<size_t>(NARG_CNT::THREE)) ? static_cast<size_t>(NARG_POS::THIRD) : static_cast<size_t>(NARG_POS::FOURTH))]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbCompl).val_;
    }
}
} // namespace ModuleFileIO
} // namespace DistributedFS
} // namespace OHOS

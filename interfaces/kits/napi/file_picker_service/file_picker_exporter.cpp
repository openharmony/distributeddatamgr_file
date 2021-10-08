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
#include "file_picker_exporter.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "../common/ability_helper.h"
#include "../common/common_func.h"
#include "../common/napi/n_class.h"
#include "../common/napi/n_func_arg.h"
#include "../common/napi/n_val.h"
#include "../common/uni_error.h"
#include "file_info.h"
#include "root_info.h"
#include "storage_ability.h"
#include "uri.h"

using Uri = OHOS::Uri;
using namespace std;
namespace OHOS {
namespace DistributedFS {
namespace ModuleFPExpoter {
enum COMMON_NUM {
    ZERO = 0,
    ONE = 1,
    TWO = 2,
    THREE = 3,
};

enum ERROR_CODE {
    SUCCESS_CODE = 200,
    OTHER_ARGUMENT_ERROR = 202,
    FILE_IO_ERROR = 300,
    FILE_PATH_ERROR = 301,
    URI_PARAMER_ERROR = 302,
};

void CallBackSuccess(napi_env env, napi_ref successFuncRef, int32_t count, napi_value obj)
{
    napi_value results = nullptr;
    napi_value successFunc = nullptr;
    napi_value global = nullptr;
    napi_get_global(env, &global);
    napi_get_reference_value(env, successFuncRef, &successFunc);
    if (successFunc == nullptr) {
        return;
    }
    napi_call_function(env, global, successFunc, count, &obj, &results);
}

void CallBackError(napi_env env, napi_ref failFuncRef, string errorProp, int errorCode)
{
    napi_value argvFail[2] = { 0 };
    napi_value results = nullptr;
    napi_value failFunc = nullptr;
    napi_value global = nullptr;
    napi_get_global(env, &global);
    argvFail[0] = NVal::CreateUTF8String(env, errorProp).val_;
    argvFail[1] = NVal::CreateInt32(env, errorCode).val_;
    napi_get_reference_value(env, failFuncRef, &failFunc);
    if (failFunc == nullptr) {
        return;
    }
    napi_call_function(env, global, failFunc, COMMON_NUM::TWO, argvFail, &results);
}

void CallComplete(napi_env env, napi_ref completeFuncRef)
{
    napi_value completeFunc = nullptr;
    napi_value results = nullptr;
    napi_value global = nullptr;
    napi_get_global(env, &global);
    napi_get_reference_value(env, completeFuncRef, &completeFunc);
    if (completeFunc == nullptr) {
        return;
    }
    napi_call_function(env, global, completeFunc, COMMON_NUM::ZERO, nullptr, &results);
}

void SaveFileExec(napi_env env, void *data)
{
    auto *asyncInfo = (SaveFileAsyncInfo *)data;
    unique_ptr<FileManager::StorageAbility> point;
    vector<FileManager::SaveFileInfo> result;
    Uri uriObj(asyncInfo->dstPath);
    asyncInfo->err = point->SaveFiles(uriObj, asyncInfo->srcPath, result);
    asyncInfo->saveFileList.assign(result.begin(), result.end());
}

void SaveFileComp(napi_env env, napi_status status, void *data)
{
    auto *asyncInfo = (SaveFileAsyncInfo *)data;
    if (asyncInfo->err == ERROR_CODE::SUCCESS_CODE) {
        napi_value saveListNapi;
        napi_create_array(env, &saveListNapi);
        int32_t i = 0;
        for (auto saveInfo : asyncInfo->saveFileList) {
            NVal objt = NVal::CreateObject(env);
            objt.AddProp("srcUri", NVal::CreateUTF8String(env, saveInfo.srcUri).val_);
            objt.AddProp("dstUri", NVal::CreateUTF8String(env, saveInfo.dstUri).val_);
            objt.AddProp("status", NVal::CreateBool(env, saveInfo.status).val_);

            napi_set_property(env, saveListNapi, NVal::CreateInt32(env, i).val_, objt.val_);
            i = i + 1;
        }
        NVal objn = NVal::CreateObject(env);
        objn.AddProp("saveFileList", saveListNapi);
        CallBackSuccess(env, asyncInfo->callback[COMMON_NUM::ZERO], COMMON_NUM::ONE, objn.val_);
    } else {
        CallBackError(env, asyncInfo->callback[COMMON_NUM::ONE], "SaveFile fail", asyncInfo->err);
    }
    CallComplete(env, asyncInfo->callback[COMMON_NUM::TWO]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::ZERO]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::ONE]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::TWO]);
    napi_delete_async_work(env, asyncInfo->asyncWork);
    delete asyncInfo;
}

void SearchFileExec(napi_env env, void *data)
{
    auto asyncInfo = (SearchFileAsyncInfo *)data;
    unique_ptr<FileManager::StorageAbility> point;
    vector<FileManager::FileInfo> result;
    Uri uriObj(asyncInfo->path);
    asyncInfo->err = point->SearchFiles(uriObj, asyncInfo->name, result);
    asyncInfo->fileInfoList.assign(result.begin(), result.end());
}

void SearchFileComp(napi_env env, napi_status status, void *data)
{
    auto asyncInfo = (SearchFileAsyncInfo *)data;
    if (asyncInfo->err == ERROR_CODE::SUCCESS_CODE) {
        napi_value fileListNapi;
        napi_create_array(env, &fileListNapi);
        int32_t i = 0;
        for (auto fileInfo : asyncInfo->fileInfoList) {
            NVal objt = NVal::CreateObject(env);
            objt.AddProp("uri", NVal::CreateUTF8String(env, fileInfo.fileUri).val_);
            objt.AddProp("name", NVal::CreateUTF8String(env, fileInfo.fileName).val_);

            objt.AddProp("lastModifiedTime", NVal::CreateInt64(env, fileInfo.lastUseTime).val_);
            objt.AddProp("length", NVal::CreateInt32(env, fileInfo.fileSize).val_);
            objt.AddProp("num", NVal::CreateInt32(env, fileInfo.dirNum).val_);
            string type = (fileInfo.typeDir == 1) ? "dir" : "file";
            objt.AddProp("type", NVal::CreateUTF8String(env, type).val_);
            objt.AddProp("suffix", NVal::CreateUTF8String(env, fileInfo.mimeType).val_);

            napi_set_property(env, fileListNapi, NVal::CreateInt32(env, i).val_, objt.val_);
            i = i + 1;
        }
        NVal objn = NVal::CreateObject(env);
        objn.AddProp("fileList", fileListNapi);
        CallBackSuccess(env, asyncInfo->callback[COMMON_NUM::ZERO], COMMON_NUM::ONE, objn.val_);
    } else {
        CallBackError(env, asyncInfo->callback[COMMON_NUM::ONE], "SearchFile fail", asyncInfo->err);
    }
    CallComplete(env, asyncInfo->callback[COMMON_NUM::TWO]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::ZERO]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::ONE]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::TWO]);
    napi_delete_async_work(env, asyncInfo->asyncWork);
    delete asyncInfo;
}

void ListExec(napi_env env, void *data)
{
    auto asyncInfo = (ListAsyncInfo *)data;
    unique_ptr<FileManager::StorageAbility> point;
    vector<FileManager::FileInfo> result;
    Uri uriObj(asyncInfo->path);
    asyncInfo->err = point->QueryFiles(uriObj, result);
    asyncInfo->fileInfoList.assign(result.begin(), result.end());
}

void ListComp(napi_env env, napi_status status, void *data)
{
    auto asyncInfo = (ListAsyncInfo *)data;
    if (asyncInfo->err == ERROR_CODE::SUCCESS_CODE) {
        napi_value fileListNapi;
        napi_create_array(env, &fileListNapi);
        int32_t i = 0;
        for (auto fileInfo : asyncInfo->fileInfoList) {
            NVal objt = NVal::CreateObject(env);
            objt.AddProp("uri", NVal::CreateUTF8String(env, fileInfo.fileUri).val_);
            objt.AddProp("name", NVal::CreateUTF8String(env, fileInfo.fileName).val_);

            objt.AddProp("lastModifiedTime", NVal::CreateInt64(env, fileInfo.lastUseTime).val_);
            objt.AddProp("length", NVal::CreateInt32(env, fileInfo.fileSize).val_);
            objt.AddProp("num", NVal::CreateInt32(env, fileInfo.dirNum).val_);
            string type = (fileInfo.typeDir == 1) ? "dir" : "file";
            objt.AddProp("type", NVal::CreateUTF8String(env, type).val_);
            objt.AddProp("suffix", NVal::CreateUTF8String(env, fileInfo.mimeType).val_);

            napi_set_property(env, fileListNapi, NVal::CreateInt32(env, i).val_, objt.val_);
            i = i + 1;
        }
        NVal objn = NVal::CreateObject(env);
        objn.AddProp("fileList", fileListNapi);
        CallBackSuccess(env, asyncInfo->callback[COMMON_NUM::ZERO], COMMON_NUM::ONE, objn.val_);
    } else {
        CallBackError(env, asyncInfo->callback[COMMON_NUM::ONE], "List fail", asyncInfo->err);
    }
    CallComplete(env, asyncInfo->callback[COMMON_NUM::TWO]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::ZERO]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::ONE]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::TWO]);
    napi_delete_async_work(env, asyncInfo->asyncWork);
    delete asyncInfo;
}

void GetDeviceInfoExec(napi_env env, void *data)
{
    auto asyncInfo = (GetDeviceAsyncInfo *)data;
    unique_ptr<FileManager::StorageAbility> point;
    vector<FileManager::RootInfo> result;
    asyncInfo->err = point->QueryDeviceInfo(result);
    asyncInfo->deviceInfoList.assign(result.begin(), result.end());
}

void GetDeviceInfoComp(napi_env env, napi_status status, void *data)
{
    auto asyncInfo = (GetDeviceAsyncInfo *)data;
    if (asyncInfo->err == ERROR_CODE::SUCCESS_CODE) {
        napi_value deviceListNapi;
        napi_create_array(env, &deviceListNapi);
        int32_t i = 0;
        for (auto deviceInfo : asyncInfo->deviceInfoList) {
            NVal objt = NVal::CreateObject(env);
            objt.AddProp("uri", NVal::CreateUTF8String(env, deviceInfo.path).val_);
            objt.AddProp("deviceID", NVal::CreateUTF8String(env, deviceInfo.deviceId).val_);

            objt.AddProp("mountID", NVal::CreateInt32(env, deviceInfo.mountFlags).val_);
            objt.AddProp("diskID", NVal::CreateUTF8String(env, deviceInfo.diskId).val_);

            napi_set_property(env, deviceListNapi, NVal::CreateInt32(env, i).val_, objt.val_);
            i = i + 1;
        }
        NVal objn = NVal::CreateObject(env);
        objn.AddProp("deviceList", deviceListNapi);
        CallBackSuccess(env, asyncInfo->callback[COMMON_NUM::ZERO], COMMON_NUM::ONE, objn.val_);
    } else {
        CallBackError(env, asyncInfo->callback[COMMON_NUM::ONE], "GetDeviceInfo fail", asyncInfo->err);
    }
    CallComplete(env, asyncInfo->callback[COMMON_NUM::TWO]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::ZERO]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::ONE]);
    napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::TWO]);
    napi_delete_async_work(env, asyncInfo->asyncWork);
    delete asyncInfo;
}

napi_value FilePickerExporter::Mkdir(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    auto *asyncInfo = new MkdirAsyncInfo;
    bool succ = false;
    tie(succ, asyncInfo->callback[COMMON_NUM::ZERO], asyncInfo->callback[COMMON_NUM::ONE],
        asyncInfo->callback[COMMON_NUM::TWO]) = CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> uri = nullptr;
    tie(succ, uri, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("uri").ToUTF8String();

    asyncInfo->path = (uri == nullptr) ? "" : uri.get();

    if (asyncInfo->path == "") {
        CallBackError(env, asyncInfo->callback[COMMON_NUM::ONE], "uri cannot be empty",
                      ERROR_CODE::URI_PARAMER_ERROR);
        delete asyncInfo;
        return nullptr;
    }

    napi_create_async_work(
        env, nullptr, NVal::CreateUTF8String(env, "Mkdir").val_,
        [](napi_env env, void *data) {
            auto *asyncInfo = (MkdirAsyncInfo *)data;
            unique_ptr<FileManager::StorageAbility> point;
            Uri uriObj(asyncInfo->path);
            asyncInfo->err = point->CreateDir(uriObj);
        },
        [](napi_env env, napi_status status, void *data) {
            auto *asyncInfo = (MkdirAsyncInfo *)data;
            if (asyncInfo->err == ERROR_CODE::SUCCESS_CODE) {
                CallBackSuccess(env, asyncInfo->callback[COMMON_NUM::ZERO], 0, nullptr);
            } else {
                CallBackError(env, asyncInfo->callback[COMMON_NUM::ONE], "Mkdir fail", asyncInfo->err);
            }
            CallComplete(env, asyncInfo->callback[COMMON_NUM::TWO]);
            napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::ZERO]);
            napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::ONE]);
            napi_delete_reference(env, asyncInfo->callback[COMMON_NUM::TWO]);
            napi_delete_async_work(env, asyncInfo->asyncWork);
            delete asyncInfo;
        },
        (void *)asyncInfo, &asyncInfo->asyncWork);
    napi_queue_async_work(env, asyncInfo->asyncWork);

    return NVal::CreateUndefined(env).val_;
}

napi_value FilePickerExporter::SaveFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    auto *asyncInfo = new SaveFileAsyncInfo;

    bool succ = false;
    tie(succ, asyncInfo->callback[COMMON_NUM::ZERO], asyncInfo->callback[COMMON_NUM::ONE],
        asyncInfo->callback[COMMON_NUM::TWO]) = CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> dstUri = nullptr;
    tie(succ, dstUri, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("dstUri").ToUTF8String();

    uint32_t srcLen = 0;
    vector<string> srcPath;
    NVal srcNapi = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("srcUri");
    napi_get_array_length(env, srcNapi.val_, &srcLen);
    for (uint32_t i = 0; i < srcLen; ++i) {
        napi_value srcElementNapi;
        unique_ptr<char[]> srcElement = nullptr;
        napi_get_element(env, srcNapi.val_, i, &srcElementNapi);
        tie(succ, srcElement, ignore) = NVal(env, srcElementNapi).ToUTF8String();
        string path = (srcElement == nullptr) ? "" : srcElement.get();
        srcPath.emplace_back(path);
    }

    asyncInfo->srcPath.assign(srcPath.begin(), srcPath.end());
    asyncInfo->dstPath = (dstUri == nullptr) ? "" : dstUri.get();
    asyncInfo->srcLen = srcLen;

    if (asyncInfo->dstPath == "" || asyncInfo->srcPath.empty()) {
        CallBackError(env, asyncInfo->callback[COMMON_NUM::ONE], "dstUri or srcUri cannot be empty",
                      ERROR_CODE::URI_PARAMER_ERROR);
        delete asyncInfo;
        return nullptr;
    }

    napi_create_async_work(env, nullptr, NVal::CreateUTF8String(env, "SaveFile").val_, SaveFileExec,
                           SaveFileComp, (void *)asyncInfo, &asyncInfo->asyncWork);
    napi_queue_async_work(env, asyncInfo->asyncWork);

    return NVal::CreateUndefined(env).val_;
}

napi_value FilePickerExporter::SearchFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    auto *asyncInfo = new SearchFileAsyncInfo;
    bool succ = false;
    tie(succ, asyncInfo->callback[COMMON_NUM::ZERO], asyncInfo->callback[COMMON_NUM::ONE],
        asyncInfo->callback[COMMON_NUM::TWO]) = CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> uri = nullptr;
    tie(succ, uri, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("uri").ToUTF8String();

    unique_ptr<char[]> name = nullptr;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("name").ToUTF8String();

    asyncInfo->path = (uri == nullptr) ? "" : uri.get();
    asyncInfo->name = (name == nullptr) ? "" : name.get();

    if (asyncInfo->path == "") {
        CallBackError(env, asyncInfo->callback[COMMON_NUM::ONE], "uri cannot be empty",
                      ERROR_CODE::URI_PARAMER_ERROR);
        delete asyncInfo;
        return nullptr;
    }

    napi_create_async_work(env, nullptr, NVal::CreateUTF8String(env, "SearchFile").val_, SearchFileExec,
                           SearchFileComp, (void *)asyncInfo, &asyncInfo->asyncWork);
    napi_queue_async_work(env, asyncInfo->asyncWork);

    return NVal::CreateUndefined(env).val_;
}

napi_value FilePickerExporter::List(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    auto *asyncInfo = new ListAsyncInfo;

    bool succ = false;
    tie(succ, asyncInfo->callback[COMMON_NUM::ZERO], asyncInfo->callback[COMMON_NUM::ONE],
        asyncInfo->callback[COMMON_NUM::TWO]) = CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> uri = nullptr;
    tie(succ, uri, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("uri").ToUTF8String();

    asyncInfo->path = (uri == nullptr) ? "" : uri.get();
    if (asyncInfo->path == "") {
        CallBackError(env, asyncInfo->callback[COMMON_NUM::ONE], "uri cannot be empty",
                      ERROR_CODE::URI_PARAMER_ERROR);
        delete asyncInfo;
        return nullptr;
    }

    napi_create_async_work(env, nullptr, NVal::CreateUTF8String(env, "List").val_, ListExec, ListComp,
                           (void *)asyncInfo, &asyncInfo->asyncWork);
    napi_queue_async_work(env, asyncInfo->asyncWork);

    return NVal::CreateUndefined(env).val_;
}

napi_value FilePickerExporter::GetDeviceInfo(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    auto *asyncInfo = new GetDeviceAsyncInfo;

    bool succ = false;
    tie(succ, asyncInfo->callback[COMMON_NUM::ZERO], asyncInfo->callback[COMMON_NUM::ONE],
        asyncInfo->callback[COMMON_NUM::TWO]) = CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    napi_create_async_work(env, nullptr, NVal::CreateUTF8String(env, "GetDeviceInfo").val_, GetDeviceInfoExec,
                           GetDeviceInfoComp, (void *)asyncInfo, &asyncInfo->asyncWork);
    napi_queue_async_work(env, asyncInfo->asyncWork);

    return NVal::CreateUndefined(env).val_;
}

bool FilePickerExporter::Export()
{
    return exports_.AddProp(
        { NVal::DeclareNapiFunction("mkdir", Mkdir), NVal::DeclareNapiFunction("saveFile", SaveFile),
          NVal::DeclareNapiFunction("searchFile", SearchFile), NVal::DeclareNapiFunction("list", List),
          NVal::DeclareNapiFunction("getDeviceInfo", GetDeviceInfo) });
}

string FilePickerExporter::GetClassName()
{
    return FilePickerExporter::className_;
}

FilePickerExporter::FilePickerExporter(napi_env env, napi_value exports)
    : NExporter(env, exports)
{}

FilePickerExporter::~FilePickerExporter() {}
} // namespace ModuleFPExpoter
} // namespace DistributedFS
} // namespace OHOS
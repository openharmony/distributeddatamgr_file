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

#include <tuple>

namespace OHOS {
namespace DistributedFS {
namespace ModuleDocument {
using namespace FileManagement::LibN;

namespace {
    const int64_t PARAM_ERROR_CODE = 401;
    const std::string PARAM_ERROR_MESSAGE = "BusinessError 401: Parameter error.";
    const int32_t SYSCAP_ERROR_CODE = 801;
}

struct AsyncCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callbackRef;
    napi_value resource;
    int status;
};

napi_value wrap_void_to_js(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static napi_value DoChoosePromise(AsyncCallbackInfo* asyncCallbackInfo)
{
    napi_deferred deferred;
    napi_value promise;
    NAPI_CALL(asyncCallbackInfo->env, napi_create_promise(asyncCallbackInfo->env, &deferred, &promise));
    asyncCallbackInfo->deferred = deferred;

    napi_create_async_work(
        asyncCallbackInfo->env,
        nullptr,
        asyncCallbackInfo->resource,
        [](napi_env env, void *data) {},
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfo* asyncCallbackInfo = (AsyncCallbackInfo*)data;

            napi_value result;
            napi_value error = nullptr;
            napi_create_object(env, &error);
            napi_value errCode = nullptr;
            napi_create_int32(env, SYSCAP_ERROR_CODE, &errCode);
            napi_set_named_property(env, error, "code", errCode);
            result = error;

            napi_reject_deferred(env, asyncCallbackInfo->deferred, result);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(asyncCallbackInfo->env, asyncCallbackInfo->asyncWork);
    return promise;
}

static napi_value DoChooseCallback(AsyncCallbackInfo* asyncCallbackInfo)
{
    napi_create_async_work(
        asyncCallbackInfo->env,
        nullptr,
        asyncCallbackInfo->resource,
        [](napi_env env, void* data) {},
        [](napi_env env, napi_status status, void* data) {
            if (data == nullptr) {
                return;
            }
            AsyncCallbackInfo* asyncCallbackInfo = (AsyncCallbackInfo*)data;

            napi_value callback = nullptr;
            napi_value undefined;
            napi_get_undefined(env, &undefined);

            napi_value result[2] = {0};

            // create error code
            napi_value error = nullptr;
            napi_create_object(env, &error);
            napi_value errCode = nullptr;
            napi_create_int32(env, SYSCAP_ERROR_CODE, &errCode);
            napi_set_named_property(env, error, "code", errCode);
            result[0] = error;
            napi_get_undefined(env, &result[1]);

            napi_get_reference_value(env, asyncCallbackInfo->callbackRef, &callback);
            napi_value callResult;
            napi_call_function(env, undefined, callback, NARG_CNT::TWO, result, &callResult);

            napi_delete_reference(env, asyncCallbackInfo->callbackRef);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void*)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    NAPI_CALL(asyncCallbackInfo->env, napi_queue_async_work(asyncCallbackInfo->env, asyncCallbackInfo->asyncWork));
    return wrap_void_to_js(asyncCallbackInfo->env);
}

napi_value Choose(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO, NARG_CNT::TWO)) {
        napi_throw_error(env, std::to_string(PARAM_ERROR_CODE).c_str(), PARAM_ERROR_MESSAGE.c_str());
        return nullptr;
    }
    bool isPromise = false;
    napi_value callback = nullptr;
    if (funcArg.GetArgc() == NARG_CNT::ZERO) {
        isPromise = true;
    } else if (funcArg.GetArgc() == NARG_CNT::ONE) {
        NVal cb(env, funcArg[NARG_POS::FIRST]);
        if (!cb.TypeIs(napi_function)) {
            isPromise = true;
        } else {
            callback = funcArg[NARG_POS::FIRST];
        }
    } else {
        NVal cb(env, funcArg[NARG_POS::SECOND]);
        if (!cb.TypeIs(napi_function)) {
            napi_throw_error(env, std::to_string(PARAM_ERROR_CODE).c_str(), PARAM_ERROR_MESSAGE.c_str());
            return nullptr;
        }
        callback = funcArg[NARG_POS::SECOND];
    }

    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "Choose", NAPI_AUTO_LENGTH, &resource));
    AsyncCallbackInfo* asyncCallbackInfo = new AsyncCallbackInfo {
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
        .callbackRef = nullptr,
        .resource = resource,
        .status = 0,
    };

    if (!isPromise) {
        napi_create_reference(env, callback, 1, &asyncCallbackInfo->callbackRef);
        return DoChooseCallback(asyncCallbackInfo);
    } else {
        return DoChoosePromise(asyncCallbackInfo);
    }
}

static napi_value DoShowPromise(AsyncCallbackInfo* asyncCallbackInfo)
{
    napi_deferred deferred;
    napi_value promise;
    NAPI_CALL(asyncCallbackInfo->env, napi_create_promise(asyncCallbackInfo->env, &deferred, &promise));
    asyncCallbackInfo->deferred = deferred;

    napi_create_async_work(
        asyncCallbackInfo->env,
        nullptr,
        asyncCallbackInfo->resource,
        [](napi_env env, void *data) {},
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfo* asyncCallbackInfo = (AsyncCallbackInfo*)data;

            napi_value result;
            napi_value error = nullptr;
            napi_create_object(env, &error);
            napi_value errCode = nullptr;
            napi_create_int32(env, SYSCAP_ERROR_CODE, &errCode);
            napi_set_named_property(env, error, "code", errCode);
            result = error;

            napi_reject_deferred(env, asyncCallbackInfo->deferred, result);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(asyncCallbackInfo->env, asyncCallbackInfo->asyncWork);
    return promise;
}

static napi_value DoShowCallback(AsyncCallbackInfo* asyncCallbackInfo)
{
    napi_create_async_work(
        asyncCallbackInfo->env,
        nullptr,
        asyncCallbackInfo->resource,
        [](napi_env env, void* data) {},
        [](napi_env env, napi_status status, void* data) {
            if (data == nullptr) {
                return;
            }
            AsyncCallbackInfo* asyncCallbackInfo = (AsyncCallbackInfo*)data;

            napi_value callback = nullptr;
            napi_value undefined;
            napi_get_undefined(env, &undefined);

            napi_value result[2] = {0};

            // create error code
            napi_value error = nullptr;
            napi_create_object(env, &error);
            napi_value errCode = nullptr;
            napi_create_int32(env, SYSCAP_ERROR_CODE, &errCode);
            napi_set_named_property(env, error, "code", errCode);
            result[0] = error;
            napi_get_undefined(env, &result[1]);

            napi_get_reference_value(env, asyncCallbackInfo->callbackRef, &callback);
            napi_value callResult;
            napi_call_function(env, undefined, callback, NARG_CNT::TWO, result, &callResult);

            napi_delete_reference(env, asyncCallbackInfo->callbackRef);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void*)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    NAPI_CALL(asyncCallbackInfo->env, napi_queue_async_work(asyncCallbackInfo->env, asyncCallbackInfo->asyncWork));
    return wrap_void_to_js(asyncCallbackInfo->env);
}

napi_value Show(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        napi_throw_error(env, std::to_string(PARAM_ERROR_CODE).c_str(), PARAM_ERROR_MESSAGE.c_str());
        return nullptr;
    }
    bool isPromise = false;
    napi_value callback = nullptr;
    if (funcArg.GetArgc() == NARG_CNT::TWO) {
        isPromise = true;
    } else {
        NVal cb(env, funcArg[NARG_POS::THIRD]);
        if (!cb.TypeIs(napi_function)) {
            napi_throw_error(env, std::to_string(PARAM_ERROR_CODE).c_str(), PARAM_ERROR_MESSAGE.c_str());
            return nullptr;
        }
        callback = funcArg[NARG_POS::THIRD];
    }

    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "Show", NAPI_AUTO_LENGTH, &resource));
    AsyncCallbackInfo* asyncCallbackInfo = new AsyncCallbackInfo {
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
        .callbackRef = nullptr,
        .resource = resource,
        .status = 0,
    };

    if (!isPromise) {
        napi_create_reference(env, callback, 1, &asyncCallbackInfo->callbackRef);
        return DoShowCallback(asyncCallbackInfo);
    } else {
        return DoShowPromise(asyncCallbackInfo);
    }
}
} // namespace ModuleDocument
} // namespace DistributedFS
} // namespace OHOS
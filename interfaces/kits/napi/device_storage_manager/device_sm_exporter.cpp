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

#include "device_sm_exporter.h"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <stack>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "../common/ability_helper.h"
#include "../common/common_func.h"
#include "../common/napi/n_class.h"
#include "../common/napi/n_func_arg.h"
#include "../common/napi/n_val.h"
#include "../common/uni_error.h"
#include "device_storage_manager.h"

using Uri = OHOS::Uri;
using namespace std;
namespace OHOS {
namespace DistributedFS {
namespace ModuleDSMExpoter {
enum COMMON_NUM {
    ZERO = 0,
    ONE = 1,
    TWO = 2,
    THREE = 3,
};
const int ERROR = 300;
const int NULL_ERROR = 202;

static void ForeachVomInfos(std::vector<std::shared_ptr<OHOS::DS::VolumeInfo>> &infos,
                            napi_env &env,
                            napi_value &getvolumenapi,
                            int32_t &userId)
{
    int32_t i = 0;
    
    for (auto vomInfo : infos) {
        NVal objv = NVal::CreateObject(env);
        objv.AddProp("mId", NVal::CreateUTF8String(env, vomInfo->GetId()).val_);
        objv.AddProp("mDiskId", NVal::CreateUTF8String(env, vomInfo->GetDiskId()).val_);
        objv.AddProp("mPartGuid", NVal::CreateUTF8String(env, vomInfo->GetPartGuid()).val_);
        objv.AddProp("mFsUuid", NVal::CreateUTF8String(env, vomInfo->GetFsUuid()).val_);
        objv.AddProp("mType", NVal::CreateInt64(env, vomInfo->GetType()).val_);
        objv.AddProp("mMountFlags", NVal::CreateInt64(env, vomInfo->GetMountFlags()).val_);
        objv.AddProp("mMountUserId", NVal::CreateInt64(env, vomInfo->GetMountUserId()).val_);
        objv.AddProp("mState", NVal::CreateInt64(env, vomInfo->GetState()).val_);
        objv.AddProp("mPath", NVal::CreateUTF8String(env, vomInfo->GetPath()).val_);
        objv.AddProp("mInternalPath", NVal::CreateUTF8String(env, vomInfo->GetInternalPath()).val_);
        objv.AddProp("mFsLabel", NVal::CreateUTF8String(env, vomInfo->GetFsLabel()).val_);
        objv.AddProp("IsEmulated", NVal::CreateBool(env, vomInfo->IsEmulated()).val_);
        objv.AddProp("IsPrimaryEmulatedForUser",
                     NVal::CreateBool(env, vomInfo->IsPrimaryEmulatedForUser(userId)).val_);
        objv.AddProp("IsRemovable", NVal::CreateBool(env, vomInfo->IsRemovable(userId)).val_);
        objv.AddProp("IsPrimary", NVal::CreateBool(env, vomInfo->IsPrimary()).val_);
        objv.AddProp("Description", NVal::CreateUTF8String(env, vomInfo->GetDescription()).val_);
        objv.AddProp("IsVisibleForUser", NVal::CreateBool(env, vomInfo->IsVisibleForUser(userId)).val_);

        napi_set_property(env, getvolumenapi, NVal::CreateInt32(env, i).val_, objv.val_);
        i++;
    }
}

static void ForeachDsmInfos(std::vector<std::shared_ptr<OHOS::DS::DiskInfo>> &infos,
                            napi_env &env,
                            napi_value &diaknapi)
{
    int32_t i = 0;
    
    for (auto dsmInfo : infos) {
        NVal objt = NVal::CreateObject(env);
        objt.AddProp("mId", NVal::CreateUTF8String(env, dsmInfo->GetId()).val_);
        objt.AddProp("mSysPath", NVal::CreateUTF8String(env, dsmInfo->GetSysPath()).val_);
        objt.AddProp("mSize", NVal::CreateInt64(env, dsmInfo->GetSize()).val_);
        objt.AddProp("mLabel", NVal::CreateUTF8String(env, dsmInfo->GetLabel()).val_);
        objt.AddProp("mFlags", NVal::CreateInt64(env, dsmInfo->GetFlags()).val_);
        objt.AddProp("IsUsb", NVal::CreateBool(env, dsmInfo->IsUsb()).val_);
        objt.AddProp("IsSd", NVal::CreateBool(env, dsmInfo->IsSd()).val_);
        objt.AddProp("IsAdoptable", NVal::CreateBool(env, dsmInfo->IsAdoptable()).val_);
        objt.AddProp("Description", NVal::CreateUTF8String(env, dsmInfo->GetDescription()).val_);

        napi_set_property(env, diaknapi, NVal::CreateInt32(env, i).val_, objt.val_);
        i++;
    }
}

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

std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
int32_t userId;
napi_value VolumesToNapi(napi_env env, std::vector<std::shared_ptr<DS::VolumeInfo>> infos)
{
    napi_value getvolumenapi;
    napi_create_array(env, &getvolumenapi);
    ForeachVomInfos(infos, env, getvolumenapi, userId);
    return getvolumenapi;
}

void PriVolToEmuVol(napi_env env, NVal a, std::shared_ptr<DS::VolumeInfo> &emuVol)
{
    bool succ = false;

    unique_ptr<char[]> id = nullptr;
    tie(succ, id, ignore) = a.GetProp("mId").ToUTF8String();
    string cId = (id == nullptr) ? "" : id.get();
    emuVol->SetId(cId);

    unique_ptr<char[]> diskId = nullptr;
    tie(succ, diskId, ignore) = a.GetProp("mDiskId").ToUTF8String();
    emuVol->SetDiskId((diskId == nullptr) ? "" : diskId.get());

    int64_t type = 0;
    tie(succ, type) = a.GetProp("mType").ToInt64();
    emuVol->SetType(type);

    unique_ptr<char[]> partGuid = nullptr;
    tie(succ, partGuid, ignore) = a.GetProp("mPartGuid").ToUTF8String();
    emuVol->SetPartGuid((partGuid == nullptr) ? "" : partGuid.get());

    int64_t mountFlags = 0;
    tie(succ, mountFlags) = a.GetProp("mMountFlags").ToInt64();
    emuVol->SetMountFlags(mountFlags);

    int64_t mountUserId = 0;
    tie(succ, mountUserId) = a.GetProp("mMountUserId").ToInt64();
    emuVol->SetMountUserId(mountUserId);

    unique_ptr<char[]> path = nullptr;
    tie(succ, path, ignore) = a.GetProp("mPath").ToUTF8String();
    emuVol->SetPath((path == nullptr) ? "" : path.get());

    unique_ptr<char[]> internalPath = nullptr;
    tie(succ, internalPath, ignore) = a.GetProp("mInternalPath").ToUTF8String();
    emuVol->SetInternalPath((internalPath == nullptr) ? "" : internalPath.get());

    unique_ptr<char[]> fsType = nullptr;
    tie(succ, fsType, ignore) = a.GetProp("mFsType").ToUTF8String();
    emuVol->SetFsUuid((fsType == nullptr) ? "" : fsType.get());

    unique_ptr<char[]> fsUuid = nullptr;
    tie(succ, fsUuid, ignore) = a.GetProp("mFsUuid").ToUTF8String();
    emuVol->SetFsUuid((fsUuid == nullptr) ? "" : fsUuid.get());

    unique_ptr<char[]> fsLabel = nullptr;
    tie(succ, fsLabel, ignore) = a.GetProp("mFsLabel").ToUTF8String();
    string tId = (fsLabel == nullptr) ? "" : fsLabel.get();
    emuVol->SetFsLabel(tId);

    int64_t state = 0;
    tie(succ, state) = a.GetProp("mState").ToInt64();
    emuVol->SetState(state);
}

void EmuVolToPriVol(napi_env env, NVal a, std::shared_ptr<DS::VolumeInfo> &priVol)
{
    bool succ = false;

    unique_ptr<char[]> id = nullptr;
    tie(succ, id, ignore) = a.GetProp("mId").ToUTF8String();
    string cId = (id == nullptr) ? "" : id.get();
    priVol->SetId(cId);

    unique_ptr<char[]> diskId = nullptr;
    tie(succ, diskId, ignore) = a.GetProp("mDiskId").ToUTF8String();
    priVol->SetDiskId((diskId == nullptr) ? "" : diskId.get());

    int64_t type = 0;
    tie(succ, type) = a.GetProp("mType").ToInt64();
    priVol->SetType(type);

    unique_ptr<char[]> partGuid = nullptr;
    tie(succ, partGuid, ignore) = a.GetProp("mPartGuid").ToUTF8String();
    priVol->SetPartGuid((partGuid == nullptr) ? "" : partGuid.get());

    int64_t mountFlags = 0;
    tie(succ, mountFlags) = a.GetProp("mMountFlags").ToInt64();
    priVol->SetMountFlags(mountFlags);

    int64_t mountUserId = 0;
    tie(succ, mountUserId) = a.GetProp("mMountUserId").ToInt64();
    priVol->SetMountUserId(mountUserId);

    unique_ptr<char[]> path = nullptr;
    tie(succ, path, ignore) = a.GetProp("mPath").ToUTF8String();
    priVol->SetPath((path == nullptr) ? "" : path.get());

    unique_ptr<char[]> internalPath = nullptr;
    tie(succ, internalPath, ignore) = a.GetProp("mInternalPath").ToUTF8String();
    priVol->SetInternalPath((internalPath == nullptr) ? "" : internalPath.get());

    unique_ptr<char[]> fsType = nullptr;
    tie(succ, fsType, ignore) = a.GetProp("mFsType").ToUTF8String();
    priVol->SetFsUuid((fsType == nullptr) ? "" : fsType.get());

    unique_ptr<char[]> fsUuid = nullptr;
    tie(succ, fsUuid, ignore) = a.GetProp("mFsUuid").ToUTF8String();
    priVol->SetFsUuid((fsUuid == nullptr) ? "" : fsUuid.get());

    unique_ptr<char[]> fsLabel = nullptr;
    tie(succ, fsLabel, ignore) = a.GetProp("mFsLabel").ToUTF8String();
    string tId = (fsLabel == nullptr) ? "" : fsLabel.get();
    priVol->SetFsLabel(tId);

    int64_t state = 0;
    tie(succ, state) = a.GetProp("mState").ToInt64();
    priVol->SetState(state);
}

void VolToDesCription(napi_env env, NVal a, std::shared_ptr<DS::VolumeInfo> &vol)
{
    bool succ = false;

    unique_ptr<char[]> id = nullptr;
    tie(succ, id, ignore) = a.GetProp("mId").ToUTF8String();
    string cId = (id == nullptr) ? "" : id.get();
    vol->SetId(cId);

    unique_ptr<char[]> diskId = nullptr;
    tie(succ, diskId, ignore) = a.GetProp("mDiskId").ToUTF8String();
    vol->SetDiskId((diskId == nullptr) ? "" : diskId.get());

    int64_t type = 0;
    tie(succ, type) = a.GetProp("mType").ToInt64();
    vol->SetType(type);

    unique_ptr<char[]> partGuid = nullptr;
    tie(succ, partGuid, ignore) = a.GetProp("mPartGuid").ToUTF8String();
    vol->SetPartGuid((partGuid == nullptr) ? "" : partGuid.get());

    int64_t mountFlags = 0;
    tie(succ, mountFlags) = a.GetProp("mMountFlags").ToInt64();
    vol->SetMountFlags(mountFlags);

    int64_t mountUserId = 0;
    tie(succ, mountUserId) = a.GetProp("mMountUserId").ToInt64();
    vol->SetMountUserId(mountUserId);

    unique_ptr<char[]> path = nullptr;
    tie(succ, path, ignore) = a.GetProp("mPath").ToUTF8String();
    vol->SetPath((path == nullptr) ? "" : path.get());

    unique_ptr<char[]> internalPath = nullptr;
    tie(succ, internalPath, ignore) = a.GetProp("mInternalPath").ToUTF8String();
    vol->SetInternalPath((internalPath == nullptr) ? "" : internalPath.get());

    unique_ptr<char[]> fsType = nullptr;
    tie(succ, fsType, ignore) = a.GetProp("mFsType").ToUTF8String();
    vol->SetFsUuid((fsType == nullptr) ? "" : fsType.get());

    unique_ptr<char[]> fsUuid = nullptr;
    tie(succ, fsUuid, ignore) = a.GetProp("mFsUuid").ToUTF8String();
    vol->SetFsUuid((fsUuid == nullptr) ? "" : fsUuid.get());

    unique_ptr<char[]> fsLabel = nullptr;
    tie(succ, fsLabel, ignore) = a.GetProp("mFsLabel").ToUTF8String();
    string tId = (fsLabel == nullptr) ? "" : fsLabel.get();
    vol->SetFsLabel(tId);

    int64_t state = 0;
    tie(succ, state) = a.GetProp("mState").ToInt64();
    vol->SetState(state);
}

napi_value DeviceSMExporter::Mount(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> deviceId = nullptr;
    tie(succ, deviceId, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("volId").ToUTF8String();

    std::string mvId = (deviceId == nullptr) ? "" : deviceId.get();
    if (mvId == "") {
        CallBackError(env, napiFailFun, "Mount incoming parameter is null", NULL_ERROR);
        CallComplete(env, napiCompFun);
        napi_delete_reference(env, napiSuccFun);
        napi_delete_reference(env, napiFailFun);
        napi_delete_reference(env, napiCompFun);
        return nullptr;
    }

    bool s = dsm->Mount(mvId);
    if (s) {
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ZERO, nullptr);
    } else {
        CallBackError(env, napiFailFun, "Mount return value error", ERROR);
    }
    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::UnMount(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> deviceId = nullptr;
    tie(succ, deviceId, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("volId").ToUTF8String();

    std::string mvId = (deviceId == nullptr) ? "" : deviceId.get();
    if (mvId == "") {
        CallBackError(env, napiFailFun, "UnMount  incoming parameter is null", NULL_ERROR);
        CallComplete(env, napiCompFun);
        napi_delete_reference(env, napiSuccFun);
        napi_delete_reference(env, napiFailFun);
        napi_delete_reference(env, napiCompFun);
        return nullptr;
    }

    bool s = dsm->UnMount(mvId);
    if (s) {
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ZERO, nullptr);
    } else {
        CallBackError(env, napiFailFun, "UnMount return value error", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::Format(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> deviceId = nullptr;
    tie(succ, deviceId, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("volId").ToUTF8String();

    std::string mvId = (deviceId == nullptr) ? "" : deviceId.get();
    if (mvId == "") {
        CallBackError(env, napiFailFun, "Format incoming parameter is null", NULL_ERROR);
        CallComplete(env, napiCompFun);
        napi_delete_reference(env, napiSuccFun);
        napi_delete_reference(env, napiFailFun);
        napi_delete_reference(env, napiCompFun);
        return nullptr;
    }

    bool s = dsm->Format(mvId);
    if (s) {
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ZERO, nullptr);
    } else {
        CallBackError(env, napiFailFun, "Format fail", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::IsEncrypted(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> devfilePath = nullptr;
    tie(succ, devfilePath, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("filePath").ToUTF8String();

    std::string mFilePath = (devfilePath == nullptr) ? "" : devfilePath.get();
    if (mFilePath == "") {
        CallBackError(env, napiFailFun, "IsEncrypted incoming parameter is null", NULL_ERROR);
        CallComplete(env, napiCompFun);
        napi_delete_reference(env, napiSuccFun);
        napi_delete_reference(env, napiFailFun);
        napi_delete_reference(env, napiCompFun);
        return nullptr;
    }

    bool s = dsm->IsEncrypted(mFilePath);
    if (s) {
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ZERO, nullptr);
    } else {
        CallBackError(env, napiFailFun, "Not A Encrypted Path", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}
napi_value DeviceSMExporter::PartitionPublic(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> deviceId = nullptr;
    tie(succ, deviceId, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("diskId").ToUTF8String();

    std::string dsId = (deviceId == nullptr) ? "" : deviceId.get();
    if (dsId == "") {
        CallBackError(env, napiFailFun, "PartitionPublic incoming parameter is null", NULL_ERROR);
        CallComplete(env, napiCompFun);
        napi_delete_reference(env, napiSuccFun);
        napi_delete_reference(env, napiFailFun);
        napi_delete_reference(env, napiCompFun);
        return nullptr;
    }

    bool s = dsm->PartitionPublic(dsId);
    if (s) {
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ZERO, nullptr);
    } else {
        CallBackError(env, napiFailFun, "PartitionPublic fail", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::PartitionPrivate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> deviceId = nullptr;
    tie(succ, deviceId, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("diskId").ToUTF8String();

    std::string dsId = (deviceId == nullptr) ? "" : deviceId.get();
    if (dsId == "") {
        CallBackError(env, napiFailFun, "PartitionPrivate incoming parameter is null", NULL_ERROR);
        CallComplete(env, napiCompFun);
        napi_delete_reference(env, napiSuccFun);
        napi_delete_reference(env, napiFailFun);
        napi_delete_reference(env, napiCompFun);
        return nullptr;
    }

    bool s = dsm->PartitionPrivate(dsId);
    if (s) {
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ZERO, nullptr);
    } else {
        CallBackError(env, napiFailFun, "PartitionPrivate fail", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::GetVolumes(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
    bool ret = dsm->GetVolumes(infos);
    if (ret) {
        napi_value getvolumenapi = VolumesToNapi(env, infos);

        NVal objc = NVal::CreateObject(env);
        objc.AddProp("volumeInfos", getvolumenapi);
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ONE, objc.val_);
    } else {
        CallBackError(env, napiFailFun, "getvolume not exist", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::GetDisks(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    std::vector<std::shared_ptr<DS::DiskInfo>> infos;
    bool ret = dsm->GetDisks(infos);
    if (ret) {
        napi_value diaknapi;
        napi_create_array(env, &diaknapi);
        ForeachDsmInfos(infos, env, diaknapi);
        NVal objn = NVal::CreateObject(env);
        objn.AddProp("diskInfos", diaknapi);
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ONE, objn.val_);
    } else {
        CallBackError(env, napiFailFun, "disk not exist", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::SetPrimaryStorageUuid(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> deviceId = nullptr;
    tie(succ, deviceId, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("volumeUuid").ToUTF8String();
    std::string vuId = (deviceId == nullptr) ? "" : deviceId.get();
    if (vuId == "") {
        CallBackError(env, napiFailFun, "SetPrimaryStorageUuid incoming parameter is null", NULL_ERROR);
        CallComplete(env, napiCompFun);
        napi_delete_reference(env, napiSuccFun);
        napi_delete_reference(env, napiFailFun);
        napi_delete_reference(env, napiCompFun);
        return nullptr;
    }

    bool s = dsm->SetPrimaryStorageUuid(vuId);
    if (s) {
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ZERO, nullptr);
    } else {
        CallBackError(env, napiFailFun, "SetPrimaryStorageUuid fail", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::FindVolumeById(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> deviceId = nullptr;
    tie(succ, deviceId, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("volId").ToUTF8String();

    std::string mvId = (deviceId == nullptr) ? "" : deviceId.get();
    if (mvId == "") {
        CallBackError(env, napiFailFun, "FindVolumeById incoming parameter is null", NULL_ERROR);
        CallComplete(env, napiCompFun);
        napi_delete_reference(env, napiSuccFun);
        napi_delete_reference(env, napiFailFun);
        napi_delete_reference(env, napiCompFun);
        return nullptr;
    }

    std::shared_ptr<DS::VolumeInfo> vol;
    bool ret = dsm->FindVolumeById(vol, mvId);
    if (ret) {
        napi_value getvolumenapi;
        napi_create_array(env, &getvolumenapi);
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        dsm->GetVolumes(infos);
        ForeachVomInfos(infos, env, getvolumenapi, userId);
        NVal objc = NVal::CreateObject(env);
        objc.AddProp("volumeInfos", getvolumenapi);
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ONE, objc.val_);
    } else {
        CallBackError(env, napiFailFun, "FindVolumeById not exist", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::FindVolumeByUuid(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> deviceId = nullptr;
    tie(succ, deviceId, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("volumeUuid").ToUTF8String();

    std::string mvmId = (deviceId == nullptr) ? "" : deviceId.get();
    if (mvmId == "") {
        CallBackError(env, napiFailFun, "FindVolumeByUuid incoming parameter is null", NULL_ERROR);
        CallComplete(env, napiCompFun);
        napi_delete_reference(env, napiSuccFun);
        napi_delete_reference(env, napiFailFun);
        napi_delete_reference(env, napiCompFun);
        return nullptr;
    }

    std::shared_ptr<DS::VolumeInfo> vol;
    if (dsm->FindVolumeByUuid(vol, mvmId)) {
        napi_value getvolumenapi;
        napi_create_array(env, &getvolumenapi);
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        dsm->GetVolumes(infos);
        ForeachVomInfos(infos, env, getvolumenapi, userId);
        NVal objc = NVal::CreateObject(env);
        objc.AddProp("volumeInfos", getvolumenapi);
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ONE, objc.val_);
    } else {
        CallBackError(env, napiFailFun, "FindVolumeByUuid not exist", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::FindDiskById(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    unique_ptr<char[]> deviceId = nullptr;
    tie(succ, deviceId, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("diskId").ToUTF8String();

    std::string dsId = (deviceId == nullptr) ? "" : deviceId.get();
    if (dsId == "") {
        CallBackError(env, napiFailFun, "FindDiskById incoming parameter is null", NULL_ERROR);
        CallComplete(env, napiCompFun);
        napi_delete_reference(env, napiSuccFun);
        napi_delete_reference(env, napiFailFun);
        napi_delete_reference(env, napiCompFun);
        return nullptr;
    }

    std::shared_ptr<DS::DiskInfo> disk;
    if (dsm->FindDiskById(disk, dsId)) {
        napi_value diaknapi;
        napi_create_array(env, &diaknapi);
        std::vector<std::shared_ptr<DS::DiskInfo>> infos;
        dsm->GetDisks(infos);
        ForeachDsmInfos(infos, env, diaknapi);
        NVal objn = NVal::CreateObject(env);
        objn.AddProp("diskInfos", diaknapi);
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ONE, objn.val_);
    } else {
        CallBackError(env, napiFailFun, "FindDiskById not exist", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::GetPrimaryStorageUuid(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    std::string primaryUuid;
    bool s = dsm->GetPrimaryStorageUuid(primaryUuid);
    if (s) {
        NVal obj = NVal::CreateObject(env);
        obj.AddProp("primaryUuid", NVal::CreateUTF8String(env, primaryUuid).val_);
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ONE, obj.val_);
    } else {
        CallBackError(env, napiFailFun, "GetPrimaryStorageUuid return value error", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::FindPrivateForEmulate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    std::shared_ptr<DS::VolumeInfo> emuVol = std::make_shared<DS::VolumeInfo>();

    NVal a = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("emuVol");
    if (a.TypeIs(napi_object)) {
    } else if (a.TypeIs(napi_undefined)) {
    } else if (a.TypeIs(napi_null)) {
    }

    PriVolToEmuVol(env, a, emuVol);
    std::shared_ptr<DS::VolumeInfo> priVol;
    bool ret = dsm->FindPrivateForEmulate(priVol, emuVol);
    if (ret) {
        napi_value getvolumenapi;
        napi_create_array(env, &getvolumenapi);
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        dsm->GetVolumes(infos);
        ForeachVomInfos(infos, env, getvolumenapi, userId);
        NVal objc = NVal::CreateObject(env);
        objc.AddProp("volumeInfos", getvolumenapi);
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ONE, objc.val_);
    } else {
        CallBackError(env, napiFailFun, "FindPrivateForEmulate not exist", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::FindEmulateForPrivate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    std::shared_ptr<DS::VolumeInfo> priVol = std::make_shared<DS::VolumeInfo>();

    NVal a = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("priVol");
    if (a.TypeIs(napi_object)) {
    } else if (a.TypeIs(napi_undefined)) {
    } else if (a.TypeIs(napi_null)) {
    }

    EmuVolToPriVol(env, a, priVol);
    std::shared_ptr<DS::VolumeInfo> emuVol;
    bool ret = dsm->FindEmulateForPrivate(emuVol, priVol);
    if (ret) {
        napi_value getvolumenapi;
        napi_create_array(env, &getvolumenapi);
        std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
        dsm->GetVolumes(infos);
        ForeachVomInfos(infos, env, getvolumenapi, userId);
        NVal objc = NVal::CreateObject(env);
        objc.AddProp("volumeInfos", getvolumenapi);
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ONE, objc.val_);
    } else {
        CallBackError(env, napiFailFun, "FindEmulateForPrivate not exist", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::GetBestVolumeDescription(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    std::shared_ptr<DS::VolumeInfo> vol = std::make_shared<DS::VolumeInfo>();

    NVal a = NVal(env, funcArg[NARG_POS::FIRST]).GetProp("vol");
    if (a.TypeIs(napi_object)) {
    } else if (a.TypeIs(napi_undefined)) {
    } else if (a.TypeIs(napi_null)) {
    }

    VolToDesCription(env, a, vol);

    std::string desCription;
    bool s = dsm->GetBestVolumeDescription(vol, desCription);
    if (s) {
        NVal obj = NVal::CreateObject(env);
        obj.AddProp("desCription", NVal::CreateUTF8String(env, vol->GetDescription()).val_);
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ONE, obj.val_);
    } else {
        CallBackError(env, napiFailFun, "GetBestVolumeDescription return value error", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

napi_value DeviceSMExporter::GetWritableVolumes(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    napi_ref napiSuccFun, napiCompFun, napiFailFun;
    tie(succ, napiSuccFun, napiFailFun, napiCompFun) =
        CommonFunc::GetCallbackHandles(env, funcArg[NARG_POS::FIRST]);

    std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
    bool ret = dsm->GetWritableVolumes(infos);
    if (ret) {
        napi_value getvolumenapi;
        napi_create_array(env, &getvolumenapi);
        ForeachVomInfos(infos, env, getvolumenapi, userId);
        NVal objc = NVal::CreateObject(env);
        objc.AddProp("volumeInfos", getvolumenapi);
        CallBackSuccess(env, napiSuccFun, COMMON_NUM::ONE, objc.val_);
    } else {
        CallBackError(env, napiFailFun, "GetWritableVolumes not exist", ERROR);
    }

    CallComplete(env, napiCompFun);
    napi_delete_reference(env, napiSuccFun);
    napi_delete_reference(env, napiFailFun);
    napi_delete_reference(env, napiCompFun);
    return NVal::CreateUndefined(env).val_;
}

bool DeviceSMExporter::Export()
{
    return exports_.AddProp({
        NVal::DeclareNapiFunction("mount", Mount),
        NVal::DeclareNapiFunction("unMount", UnMount),
        NVal::DeclareNapiFunction("format", Format),
        NVal::DeclareNapiFunction("partitionPublic", PartitionPublic),
        NVal::DeclareNapiFunction("partitionPrivate", PartitionPrivate),
        NVal::DeclareNapiFunction("getVolumes", GetVolumes),
        NVal::DeclareNapiFunction("getDisks", GetDisks),
        NVal::DeclareNapiFunction("setPrimaryStorageUuid", SetPrimaryStorageUuid),
        NVal::DeclareNapiFunction("findVolumeById", FindVolumeById),
        NVal::DeclareNapiFunction("findVolumeByUuid", FindVolumeByUuid),
        NVal::DeclareNapiFunction("findDiskById", FindDiskById),
        NVal::DeclareNapiFunction("getPrimaryStorageUuid", GetPrimaryStorageUuid),
        NVal::DeclareNapiFunction("findPrivateForEmulate", FindPrivateForEmulate),
        NVal::DeclareNapiFunction("findEmulateForPrivate", FindEmulateForPrivate),
        NVal::DeclareNapiFunction("getWritableVolumes", GetWritableVolumes),
        NVal::DeclareNapiFunction("getBestVolumeDescription", GetBestVolumeDescription),
        NVal::DeclareNapiFunction("isEncrypted", IsEncrypted),
    });
}

string DeviceSMExporter::GetClassName()
{
    return DeviceSMExporter::className_;
}

DeviceSMExporter::DeviceSMExporter(napi_env env, napi_value exports)
    : NExporter(env, exports)
{}

DeviceSMExporter::~DeviceSMExporter() {}
} // namespace ModuleDSMExpoter
} // namespace DistributedFS
} // namespace OHOS

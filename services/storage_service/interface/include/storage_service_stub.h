/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_SS_INCLUDE_SERVER_STORAGE_SERVICE_STUB_H
#define FRAMEWORKS_SS_INCLUDE_SERVER_STORAGE_SERVICE_STUB_H

#include "istorage_service.h"

namespace OHOS {
class StorageServiceStub : public IRemoteStub<IStorageService> {
public:
    virtual int32_t
    OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t HandleMount(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUnMount(MessageParcel &data, MessageParcel &reply);
    int32_t HandleFormat(MessageParcel &data, MessageParcel &reply);
    int32_t HandleReset(MessageParcel &reply);
    int32_t HandlePartition(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUserAdded(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUserRemoved(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUserStarted(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUserStoped(MessageParcel &data, MessageParcel &reply);
    int32_t HandleMoveStorage(MessageParcel &data, MessageParcel &reply);
    int32_t HandleBenchMark(MessageParcel &data, MessageParcel &reply);
    int32_t HandleRegisterStorageServiceCallback(MessageParcel &data, MessageParcel &reply);
    int32_t HandleCryptoEnable(MessageParcel &reply);
    int32_t HandleCryptoInitialize(MessageParcel &reply);
    int32_t HandleCryptoCreateKey(MessageParcel &data, MessageParcel &reply);
    int32_t HandleCryptoDeleteKey(MessageParcel &data, MessageParcel &reply);
    int32_t HandleCryptoAddAuthKey(MessageParcel &data, MessageParcel &reply);
    int32_t HandleCryptoDelAuthKey(MessageParcel &data, MessageParcel &reply);
    int32_t HandleCryptoUnlockKey(MessageParcel &data, MessageParcel &reply);
    int32_t HandleCryptoLockKey(MessageParcel &data, MessageParcel &reply);
    int32_t HandleCryptoInitUserSpace(MessageParcel &data, MessageParcel &reply);
    int32_t HandleCryptoRemoveUserSpace(MessageParcel &data, MessageParcel &reply);
    int32_t HandleCryptoUpdateAuthKey(MessageParcel &data, MessageParcel &reply);
    int32_t HandleIdleFsTrim(MessageParcel &reply);
    int32_t HandleRunIdleMaintain(MessageParcel &reply);
    int32_t HandleAbortIdleMaintain(MessageParcel &reply);
    int32_t HandlePersistFstab(MessageParcel &data, MessageParcel &reply);
    int32_t HandleIsEncrypted(MessageParcel &data, MessageParcel &reply);
};
} // namespace OHOS

#endif // FRAMEWORKS_SS_INCLUDE_SERVER_STORAGE_SERVICE_STUB_H

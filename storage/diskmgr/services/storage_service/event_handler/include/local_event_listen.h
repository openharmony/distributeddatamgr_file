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
#ifndef LOCAKEVENTLISTEN_H
#define LOCAKEVENTLISTEN_H

#include "local_envent_poll.h"

class LocalEventPacket;

class LocalEventListen : public LocalEventPoll {
    char mBuffer[64 * 1024] __attribute__((aligned(4)));

public:
    static const int LOCALEVENT_Format_ASCII = 0;
    static const int LOCALEVENT_Format_BINARY = 1;
    static const int LOCALEVENT_Format_BINARY_UNICAST = 2;

    LocalEventListen(int socket);
    virtual ~LocalEventListen() {}

protected:
    virtual bool OnDataAvailable(LocalEventSend *cli);
    void OnEvent(LocalEventPacket *evt);
};
#endif // LOCAKEVENTLISTEN_H

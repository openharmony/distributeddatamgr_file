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
#ifndef LOCALEVENTPACKET_H
#define LOCALEVENTPACKET_H

#include "local_event_listen.h"

#define NL_PARAMS_MAX 32
#define ND_OPT_RDNSS 25
#define ND_OPT_DNSSL 31

class LocalEventPacket {
public:
    enum class Action {
        LOCALEVENT_ACTION_UNKNOWN = 0,
        LOCALEVENT_ACTION_ADD = 1,
        LOCALEVENT_ACTION_REMOVE = 2,
        LOCALEVENT_ACTION_CHANGE = 3,
        LOCALEVENT_ACTION_LINKUP = 4,
        LOCALEVENT_ACTION_LINKDOWN = 5,
        LOCALEVENT_ACTION_ADDRESS_UPDATED = 6,
        LOCALEVENT_ACTION_ADDRESS_REMOVED = 7,
        LOCALEVENT_ACTION_RDNSS = 8,
        LOCALEVENT_ACTION_ROUTEUPDATED = 9,
        LOCALEVENT_ACTION_ROUTEREMOVED = 10,
    };

    struct in6_addr {
        uint8_t s6_addr[16];
    };

    struct nd_opt_rdnss {
        uint8_t nd_opt_rdnss_type;
        uint8_t nd_opt_rdnss_len;
        uint16_t nd_opt_rdnss_reserved;
        uint32_t nd_opt_rdnss_lifetime;
        struct in6_addr nd_opt_rdnss_addr[1];
    };

public:
    LocalEventPacket();
    virtual ~LocalEventPacket();

    bool Decode(char *buffer, int size);
    const char *FindParam(const char *paramName);

    const char *GetBlockEvent()
    {
        return eventPacketSubsystem;
    }
    Action getAction()
    {
        return eventPacketAction;
    }

protected:
    bool ParseAsciiLocalEventMessage(char *buffer, int size);

private:
    Action eventPacketAction;
    char *eventPacketSubsystem;
    char *eventPacketParams[NL_PARAMS_MAX];
};

#endif // LOCALEVENTPACKET_H
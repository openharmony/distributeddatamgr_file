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
#include "local_event_packet.h"

#define LOG_TAG "LocalEventPacket"

#include <arpa/inet.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

LocalEventPacket::LocalEventPacket()
{
    eventPacketAction = Action::LOCALEVENT_ACTION_UNKNOWN;
    memset(eventPacketParams, 0, sizeof(eventPacketParams));
    eventPacketSubsystem = nullptr;
}

LocalEventPacket::~LocalEventPacket()
{
    int i;

    if (eventPacketSubsystem) {
        free(eventPacketSubsystem);
    }

    for (i = 0; i < NL_PARAMS_MAX; i++) {
        if (!eventPacketParams[i]) {
            break;
        }
        free(eventPacketParams[i]);
    }
}

static const char *FindStr(const char *str, const char *end, const char *prefix, size_t prefixlen)
{
    if ((end - str) >= (ptrdiff_t)prefixlen && !memcmp(str, prefix, prefixlen)) {
        return str + prefixlen;
    } else {
        return nullptr;
    }
}

bool LocalEventPacket::ParseAsciiLocalEventMessage(char *buffer, int size)
{
    const char *s = buffer;
    const char *end;
    int param_idx = 0;

    if (size == 0) {
        return false;
    }

    buffer[size - 1] = '\0';
    end = s + size;

    while (s < end) {
        const char *a;
        if ((a = FindStr(s, end, "ACTION=", strlen("ACTION="))) != nullptr) {
            if (!strcmp(a, "add")) {
                eventPacketAction = Action::LOCALEVENT_ACTION_ADD;
            } else if (!strcmp(a, "remove")) {
                eventPacketAction = Action::LOCALEVENT_ACTION_REMOVE;
            } else if (!strcmp(a, "change")) {
                eventPacketAction = Action::LOCALEVENT_ACTION_CHANGE;
            }
        } else if ((a = FindStr(s, end, "SUBSYSTEM=", strlen("SUBSYSTEM="))) != nullptr) {
            eventPacketSubsystem = strdup(a);
        } else if (param_idx < NL_PARAMS_MAX) {
            eventPacketParams[param_idx++] = strdup(s);
        }
        s += strlen(s) + 1;
    }
    return true;
}

bool LocalEventPacket::Decode(char *buffer, int size)
{
    return ParseAsciiLocalEventMessage(buffer, size);
}

const char *LocalEventPacket::FindParam(const char *paramName)
{
    size_t len = strlen(paramName);
    for (int i = 0; i < NL_PARAMS_MAX && eventPacketParams[i] != nullptr; ++i) {
        const char *ptr = eventPacketParams[i] + len;
        if (!strncmp(eventPacketParams[i], paramName, len) && *ptr == '=') {
            return ++ptr;
        }
    }
    return nullptr;
}
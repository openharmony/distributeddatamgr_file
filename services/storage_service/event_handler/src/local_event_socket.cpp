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
#include "local_event_socket.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <linux/netlink.h>

#include "storage_hilog.h"

using namespace OHOS;

static const int  NUM_SIXTYFOUR_ = 64;
static const int  NUM_ONEZEROTWOFOUR_ = 1024;

LocalEventSocket *LocalEventSocket::eventSocketInstance = NULL;

LocalEventSocket::LocalEventSocket() {}

LocalEventSocket *LocalEventSocket::Instance()
{
    if (!eventSocketInstance) {
        eventSocketInstance = new LocalEventSocket();
    }
    return eventSocketInstance;
}

LocalEventSocket::~LocalEventSocket() {}

int LocalEventSocket::Start()
{
    struct sockaddr_nl nladdr;
    int sz = NUM_SIXTYFOUR_ * NUM_ONEZEROTWOFOUR_;
    int on = 1;

    bzero(&nladdr, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = getpid();
    nladdr.nl_groups = 0xffffffff;

    if ((eventSocketSock = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_KOBJECT_UEVENT)) < 0) {
        SSLOGFE("Unable to Create uevent socket");
        return -1;
    }

    if ((setsockopt(eventSocketSock, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz)) < 0) &&
        (setsockopt(eventSocketSock, SOL_SOCKET, SO_RCVBUF, &sz, sizeof(sz)) < 0)) {
        SSLOGFE("Unable to set uevent socket SO_RCVBUF/SO_RCVBUFFORCE option");
        goto out;
    }

    if (setsockopt(eventSocketSock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on)) < 0) {
        SSLOGFE("Unable to set uevent socket SO_PASSCRED option");
        goto out;
    }

    if (bind(eventSocketSock, (struct sockaddr *)&nladdr, sizeof(nladdr)) < 0) {
        SSLOGFE("Unable to bind uevent socket");
        goto out;
    }

    eventSocketHandler = new LocalEventHandleData(eventSocketSock);
    if (eventSocketHandler->Start()) {
        SSLOGFE("Unable to Start LocalEventHandleData");
        goto out;
    }

    return 0;

out:
    close(eventSocketSock);
    return -1;
}

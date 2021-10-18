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
#include "local_event_listen.h"

#define LOG_TAG "LocalEventListen"

#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/netlink.h>

#include "local_event_packet.h"
#include "storage_hilog.h"
#include "storage_manager.h"

using namespace OHOS;

LocalEventListen::LocalEventListen(int socket)
    : LocalEventPoll(socket)
{}

ssize_t UeventKernelRecv(int socket, void *buffer, size_t length)
{
    struct iovec iov = { buffer, length };
    struct sockaddr_nl addr;
    char control[CMSG_SPACE(sizeof(struct ucred))];

    struct msghdr hdr = {
        &addr, sizeof(addr), &iov, 1, control, sizeof(control), 0,
    };

    ssize_t n = TEMP_FAILURE_RETRY(recvmsg(socket, &hdr, 0));
    if (n <= 0) {
        return n;
    }

    if (addr.nl_groups == 0) {
        bzero(buffer, length);
        errno = EIO;
        return -1;
    }

    return n;
}

void LocalEventListen::OnEvent(LocalEventPacket *evt)
{
    StorageManager *sm = StorageManager::Instance();
    const char *blockEvent = evt->GetBlockEvent();

    if (!blockEvent) {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        return;
    }

    if (std::string(blockEvent) == "block") {
        SSLOG_I("dugl %{public}s %{public}s %{public}d", __FILE__, __func__, __LINE__);
        sm->KernelEventHandle(evt);
    }
}

bool LocalEventListen::OnDataAvailable(LocalEventSend *cli)
{
    int socket = cli->getSocket();
    ssize_t count;

    count = TEMP_FAILURE_RETRY(UeventKernelRecv(socket, mBuffer, sizeof(mBuffer)));
    if (count < 0) {
        return false;
    }

    LocalEventPacket *evt = new LocalEventPacket();
    if (evt->Decode(mBuffer, count)) {
        OnEvent(evt);
    }

    delete evt;
    return true;
}

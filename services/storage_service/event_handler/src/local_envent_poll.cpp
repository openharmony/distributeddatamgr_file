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
#include "local_envent_poll.h"

#define LOG_TAG "LocalEventPoll"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>

#include "local_event_send.h"
#include "storage_hilog.h"

using namespace OHOS;

static const int CTRL_PIPE_SHUT_DOWN = 0;
static const int NUM_ONE_ = 1;
static const int NUM_TWO_ = 2;

pthread_mutex_t eventPollClientsLock = PTHREAD_MUTEX_INITIALIZER;

LocalEventPoll::LocalEventPoll(int socketFd)
{
    Init(socketFd);
}

void LocalEventPoll::Init(int socketFd)
{
    eventPollSock = socketFd;
}

LocalEventPoll::~LocalEventPoll()
{
    if (eventPollCtrlPipe[0] != -1) {
        close(eventPollCtrlPipe[0]);
        close(eventPollCtrlPipe[1]);
    }
    for (auto pair : eventPollClients) {
        pair.second->DelRef();
    }
}

int LocalEventPoll::StartListener()
{
    if (eventPollSock == -1) {
        errno = EINVAL;
        return -1;
    }
    eventPollClients[eventPollSock] = new LocalEventSend(eventPollSock);

    if (pipe2(eventPollCtrlPipe, O_CLOEXEC)) {
        return -1;
    }

    if (pthread_create(&eventPollThread, nullptr, LocalEventPoll::ListenThread, this)) {
        return -1;
    }
    return 0;
}

void *LocalEventPoll::ListenThread(void *obj)
{
    LocalEventPoll *me = reinterpret_cast<LocalEventPoll *>(obj);
    me->RunListener();
    pthread_exit(nullptr);
    return nullptr;
}

void LocalEventPoll::EventProcess(std::vector<pollfd> &fds)
{
    std::vector<LocalEventSend *> pending;
    pthread_mutex_lock(&eventPollClientsLock);
    const int size = fds.size();
    for (int i = NUM_ONE_; i < size; ++i) {
        const struct pollfd &p = fds[i];
        if (p.revents & (POLLIN | POLLERR)) {
            auto it = eventPollClients.find(p.fd);
            if (it == eventPollClients.end()) {
                continue;
            }
            LocalEventSend *c = it->second;
            pending.push_back(c);
            c->AddRef();
        }
    }
    pthread_mutex_unlock(&eventPollClientsLock);

    for (LocalEventSend *c : pending) {
        OnDataAvailable(c);
        c->DelRef();
    }
}

void LocalEventPoll::RunListener()
{
    std::vector<pollfd> fds;

    while (true) {
        pthread_mutex_lock(&eventPollClientsLock);
        fds.reserve(NUM_TWO_ + eventPollClients.size());
        fds.push_back({ .fd = eventPollCtrlPipe[0], .events = POLLIN });

        for (auto pair : eventPollClients) {
            const int fd = pair.second->getSocket();
            fds.push_back({ .fd = fd, .events = POLLIN });
        }
        pthread_mutex_unlock(&eventPollClientsLock);

        if (TEMP_FAILURE_RETRY(poll(fds.data(), fds.size(), -1)) < 0) {
            sleep(1);
            continue;
        }

        if (fds[0].revents & (POLLIN | POLLERR)) {
            char c = CTRL_PIPE_SHUT_DOWN;
            TEMP_FAILURE_RETRY(read(eventPollCtrlPipe[0], &c, 1));
            if (c == CTRL_PIPE_SHUT_DOWN) {
                break;
            }
            continue;
        }
        EventProcess(fds);
    }
}
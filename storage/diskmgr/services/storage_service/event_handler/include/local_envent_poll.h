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
#ifndef LOCALEVENTPOLL_H
#define LOCALEVENTPOLL_H

#include "local_event_send.h"

#include <poll.h>
#include <pthread.h>
#include <unordered_map>
#include <vector>

class LocalEventPoll {
    int eventPollSock;
    std::unordered_map<int, LocalEventSend *> eventPollClients;
    pthread_mutex_t eventPollClientsLock;
    int eventPollCtrlPipe[2];
    pthread_t eventPollThread;

public:
    LocalEventPoll(int socketFd);

    virtual ~LocalEventPoll();
    int StartListener();

protected:
    virtual bool OnDataAvailable(LocalEventSend *c) = 0;

private:
    static void *ListenThread(void *obj);
    void RunListener();
    void Init(int socketFd);
    void EventProcess(std::vector<pollfd> &fds);
};
#endif // LOCALEVENTPOLL_H
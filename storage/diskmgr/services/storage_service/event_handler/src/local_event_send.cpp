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
#include "local_event_send.h"

#define LOG_TAG "LocalEventSend"

#include <alloca.h>
#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "storage_hilog.h"

using namespace OHOS;

pthread_mutex_t mRefNumMutex = PTHREAD_MUTEX_INITIALIZER;

LocalEventSend::LocalEventSend(int socket)
{
    Init(socket);
}

void LocalEventSend::Init(int socket)
{
    struct ucred creds;
    socklen_t szCreds = sizeof(creds);
    bzero(&creds, szCreds);

    mSocket = socket;
    if (getsockopt(socket, SOL_SOCKET, SO_PEERCRED, &creds, &szCreds) == 0) {
        mPid = creds.pid;
        mUid = creds.uid;
        mGid = creds.gid;
    }
}

LocalEventSend::~LocalEventSend() {}

void LocalEventSend::AddRef()
{
    pthread_mutex_lock(&mRefNumMutex);
    mRefNum++;
    pthread_mutex_unlock(&mRefNumMutex);
}

bool LocalEventSend::DelRef()
{
    bool deleteSelf = false;
    pthread_mutex_lock(&mRefNumMutex);
    mRefNum--;
    if (mRefNum == 0) {
        deleteSelf = true;
    }
    pthread_mutex_unlock(&mRefNumMutex);
    if (deleteSelf) {
        delete this;
    }
    return deleteSelf;
}
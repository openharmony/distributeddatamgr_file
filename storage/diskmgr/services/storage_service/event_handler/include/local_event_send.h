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
#ifndef LOCALEVENTSEND_H
#define LOCALEVENTSEND_H

#include <sys/types.h>
#include <pthread.h>
#include <sys/uio.h>

class LocalEventSend {
    pid_t mPid = -1;
    uid_t mUid = -1;
    gid_t mGid = -1;

    int mSocket;
    int mRefNum = 1;
    int mCmdNum = 0;

    bool mUseCmdNum;

public:
    LocalEventSend(int sock);
    virtual ~LocalEventSend();

    int getSocket() const
    {
        return mSocket;
    }
    pid_t getPid() const
    {
        return mPid;
    }
    uid_t getUid() const
    {
        return mUid;
    }
    gid_t getGid() const
    {
        return mGid;
    }
    void setCmdNum(int cmdNum)
    {
        cmdNum = 0;
        cmdNum++;
    }
    int getCmdNum()
    {
        return mCmdNum;
    }

    void AddRef();
    bool DelRef();
    static char *QuoteArg(const char *arg);

private:
    void Init(int socket);
};
#endif // LOCALEVENTSEND_H

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
#include "mount_info.h"

#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/wait.h>
#include <system_ability_definition.h>

#include "cJSON.h"
#include "fs_mount_tab.h"
#include "ss_tab_mgr.h"
#include "storage_hilog.h"

namespace OHOS {
static const int MAX_JOB_NAME_LEN = 64;
static const int MAX_CMD_NAME_LEN = 32;
static const int MAX_CMD_CONTENT_LEN = 256;
static const int MAX_CMD_CNT_IN_ONE_JOB = 200;

typedef struct {
    char name[MAX_CMD_NAME_LEN + 1];
    char cmdContent[MAX_CMD_CONTENT_LEN + 1];
} CmdLine;

typedef struct {
    char name[MAX_JOB_NAME_LEN + 1];
    int cmdLinesCnt;
    CmdLine* cmdLines;
} Job;

static const char *supportedComds[] = {
    "start ",      "mkdir ",  "chmod ",     "chown ", "mount ", "export ",   "loadcfg ",
    "insmod ",     "rm ",     "rmdir ",     "write ", "exec ",  "mknode ",   "makedev ",
    "symlink ",    "stop ",   "trigger ",   "reset ", "copy ",  "setparam ", "load_persist_params ",
    "load_param ", "reboot ", "setrlimit ",
};

static int gJobCnt = 0;
static Job *gJobs = nullptr;

static char *ReadFileToBuf(const char *configFile)
{
    SSLOG_I("ReadFileToBuf() start");
    char *buffer = nullptr;
    FILE *fd = nullptr;
    struct stat fileStat = { 0 };
    if (configFile == nullptr || *configFile == '\0') {
        return nullptr;
    }
    do {
        if (stat(configFile, &fileStat) != 0 || fileStat.st_size <= 0 ||
            fileStat.st_size > MAX_JSON_FILE_LEN) {
            SSLOG_I("Unexpected config file \" %{public}s \", check if it exist. if exist, check file size",
                    configFile);
            break;
        }
        fd = fopen(configFile, "r");
        if (fd == nullptr) {
            SSLOG_I("Open %{public}s failed. err = %{public}d", configFile, errno);
            break;
        }
        buffer = (char *)malloc(fileStat.st_size + 1);
        if (buffer == nullptr) {
            SSLOG_I("Failed to allocate memory for config file, err = %{public}d", errno);
            break;
        }
        if (fread(buffer, fileStat.st_size, 1, fd) != 1) {
            free(buffer);
            buffer = nullptr;
            break;
        }
        buffer[fileStat.st_size] = '\0';
    } while (0);
    if (fd != nullptr) {
        fclose(fd);
        fd = nullptr;
    }
    return buffer;
}

static int GetJobName(const cJSON *jobItem, Job *resJob)
{
    SSLOG_I("GetJobName() start");
    char *jobNameStr = cJSON_GetStringValue(cJSON_GetObjectItem(jobItem, "name"));
    if (jobNameStr == nullptr) {
        return 0;
    }
    if (memcpy(resJob->name, jobNameStr, strlen(jobNameStr)) == nullptr) {
        SSLOG_I("Get job name \"%{public}s\" from jobNameStr failed", jobNameStr);
    }
    resJob->name[strlen(jobNameStr)] = '\0';
    return 1;
}

static void ParseCmdLine(const char *cmdStr, CmdLine *resCmd, std::list<std::string> &mountList)
{
    SSLOG_I("ParseCmdLine() start");
    size_t cmdLineLen = 0;
    if (cmdStr == nullptr || resCmd == nullptr || (cmdLineLen = strlen(cmdStr)) == 0) {
        return;
    }
    size_t supportCmdCnt = sizeof(supportedComds) / sizeof(supportedComds[0]);
    int foundAndSucceed = 0;
    SSLOG_I("supportCmdCnt is %{public}d", supportCmdCnt);
    for (size_t i = 0; i < supportCmdCnt; ++i) {
        SSLOG_I("CmdLine--in for");
        size_t curCmdNameLen = strlen(supportedComds[i]);
        if (cmdLineLen > curCmdNameLen && cmdLineLen <= (curCmdNameLen + MAX_CMD_CONTENT_LEN) &&
            strncmp(supportedComds[i], cmdStr, curCmdNameLen) == 0) {
            if (memcpy(resCmd->name, cmdStr, curCmdNameLen) == nullptr) {
                SSLOG_I("resCmd name is null");
                break;
            }
            if (!(strcmp("mount ", resCmd->name))) {
                resCmd->name[curCmdNameLen] = '\0';
                SSLOG_I("mountTest--ParseCmdLine--find a mount cmd~!!");

                const char *cmdContent = cmdStr + curCmdNameLen;
                size_t cmdContentLen = cmdLineLen - curCmdNameLen;
                if (memcpy(resCmd->cmdContent, cmdContent, cmdContentLen) == nullptr) {
                    break;
                }
                mountList.push_back(resCmd->cmdContent);
                resCmd->cmdContent[cmdContentLen] = '\0';
            }
            foundAndSucceed = 1;
            break;
        }
    }
    if (!foundAndSucceed) {
        SSLOG_I("Cannot parse command: %{public}s", cmdStr);
        (void)memset(resCmd, 0, sizeof(*resCmd));
    }
}

static void ParseJob(const cJSON *jobItem, Job *resJob, std::list<std::string> &mountList)
{
    SSLOG_I("ParseJob() start");
    if (!GetJobName(jobItem, resJob)) {
        SSLOG_I("get JobName failed");
        (void)memset(resJob, 0, sizeof(*resJob));
        return;
    }
    cJSON *cmdsItem = cJSON_GetObjectItem(jobItem, CMDS_ARR_NAME_IN_JSON.c_str());
    if (!cJSON_IsArray(cmdsItem)) {
        SSLOG_I("job %{public}s is not an arrary", resJob->name);
        return;
    }
    int cmdLinesCnt = cJSON_GetArraySize(cmdsItem);
    if (cmdLinesCnt <= 0) {
        SSLOG_I("empty job \"%{public}s\"", resJob->name);
        return;
    }
    SSLOG_I("job = %{public}s, cmdLineCnt = %{public}d", resJob->name, cmdLinesCnt);
    if (cmdLinesCnt > MAX_CMD_CNT_IN_ONE_JOB) {
        SSLOG_I("-ParseAllJobs, too many cmds[cnt %{public}d] in one job, it should not exceed %{public}d.",
                cmdLinesCnt, MAX_CMD_CNT_IN_ONE_JOB);
        return;
    }
    resJob->cmdLines = (CmdLine *)malloc(cmdLinesCnt * sizeof(CmdLine));
    if (resJob->cmdLines == nullptr) {
        SSLOG_I("allocate memory for command line failed");
        return;
    }
    if (memset(resJob->cmdLines, 0, cmdLinesCnt * sizeof(CmdLine)) == nullptr) {
        free(resJob->cmdLines);
        resJob->cmdLines = nullptr;
        return;
    }
    resJob->cmdLinesCnt = cmdLinesCnt;
    for (int i = 0; i < cmdLinesCnt; ++i) {
        char *cmdLineStr = cJSON_GetStringValue(cJSON_GetArrayItem(cmdsItem, i));
        ParseCmdLine(cmdLineStr, &(resJob->cmdLines[i]), mountList);
    }
}

static void ParseMount(const cJSON *fileRoot, std::list<std::string> &mountList)
{
    SSLOG_I("ParseMount() start");
    if (fileRoot == nullptr) {
        SSLOG_I("ParseMount, input fileRoot is nullptr!");
        return;
    }
    cJSON *jobArr = cJSON_GetObjectItemCaseSensitive(fileRoot, JOBS_ARR_NAME_IN_JSON.c_str());
    if (!cJSON_IsArray(jobArr)) {
        SSLOG_I("ParseMount, job item is not array!");
        return;
    }
    int jobArrSize = cJSON_GetArraySize(jobArr);
    if (jobArrSize <= 0 || jobArrSize > MAX_JOBS_COUNT) {
        SSLOG_I(
            "ParseMount, jobs count %{public}d is invalid, should be positive and not exceeding %{public}d.",
            jobArrSize, MAX_JOBS_COUNT);
        return;
    }
    Job *retJobs = (Job *)realloc(gJobs, sizeof(Job) * (gJobCnt + jobArrSize));
    if (retJobs == nullptr) {
        SSLOG_I("ParseMount, malloc failed! job arrSize %{public}d.", jobArrSize);
        return;
    }
    Job *tmp = retJobs + gJobCnt;
    if (memset(tmp, 0, sizeof(Job) * jobArrSize) == nullptr) {
        SSLOG_I("ParseMount, memset failed.");
        free(retJobs);
        retJobs = nullptr;
        return;
    }
    for (int i = 0; i < jobArrSize; ++i) {
        cJSON *jobItem = cJSON_GetArrayItem(jobArr, i);
        ParseJob(jobItem, &(tmp[i]), mountList);
    }
    gJobs = retJobs;
    gJobCnt += jobArrSize;
}

void ParseInitCfg(const char *configFile, std::list<std::string> &mountList)
{
    SSLOG_I("ParseInitCfg() start");
    if (configFile == nullptr || *configFile == '\0') {
        SSLOG_I("Invalid config file");
        return;
    }
    char *fileBuf = ReadFileToBuf(configFile);
    if (fileBuf == nullptr) {
        SSLOG_I("Read %{public}s failed", configFile);
        return;
    }
    cJSON *fileRoot = cJSON_Parse(fileBuf);
    free(fileBuf);
    fileBuf = nullptr;
    if (fileRoot == nullptr) {
        SSLOG_I("InitReadCfg, parse failed! please check file %{public}s format.", configFile);
        return;
    }
    ParseMount(fileRoot, mountList);
    cJSON_Delete(fileRoot);
    return;
}
} // namespace OHOS

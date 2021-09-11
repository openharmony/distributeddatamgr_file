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
#include "../include/fms_utils.h"
#include <sys/stat.h>
#include <uri.h>
#include "../include/file_info.h"
#include "../include/log_util.h"
#include "parcel.h"

namespace OHOS {
namespace FileManager {
using namespace OHOS::AppExecFwk;
using namespace std;
using Uri = OHOS::Uri;
constexpr int DIR_FAULT_PERM = 0775;
constexpr int MIN_PATHVECTOR_LENGTH = 3;
constexpr int MULTIPLY_BY_THOUSAND = 1000;
constexpr int DIVIDE_BY_MILLION = 1000000;
unique_ptr<FmsUtils> FmsUtils::mInstance = nullptr;
FmsUtils *FmsUtils::Instance()
{
    if (mInstance.get() == nullptr) {
        mInstance.reset(new FmsUtils());
    }
    return mInstance.get();
}

FmsUtils::FmsUtils() {}
bool FmsUtils::IsPublicStorage(const Uri &uri) const
{
    std::vector<string> pathVector;
    const_cast<Uri &>(uri).GetPathSegments(pathVector);
    string mAbilityPath = pathVector[0];
    HILOGI("mAbilityPath: %{public}s", mAbilityPath.c_str());
    if (mAbilityPath.empty()) {
        return false;
    }
    if (!strcmp(mAbilityPath.c_str(), PUBLIC_STORAGE_ABILITY.c_str())) {
        return true;
    }
    return false;
}
bool FmsUtils::IsPrimaryUser(const Uri &uri) const
{
    std::vector<string> pathVector;
    const_cast<Uri &>(uri).GetPathSegments(pathVector);
    string mUserPath = pathVector[1];
    HILOGI("userPath: %{public}s", mUserPath.c_str());
    if (mUserPath.empty()) {
        return false;
    }
    if (!strcmp(mUserPath.c_str(), PRIMARY_USER.c_str())) {
        return true;
    }
    return false;
}

bool FmsUtils::IsCreateDir(const Uri &uri) const
{
    if (!strcmp(CREATE_DIR.c_str(), const_cast<Uri &>(uri).GetQuery().c_str())) {
        return true;
    }
    return false;
}

bool FmsUtils::IsSaveFiles(const Uri &uri) const
{
    if (!strcmp(SAVE_FILES.c_str(), const_cast<Uri &>(uri).GetQuery().c_str())) {
        return true;
    }
    return false;
}

bool FmsUtils::IsRootDirectory(const Uri &uri) const
{
    std::vector<string> pathVector;
    const_cast<Uri &>(uri).GetPathSegments(pathVector);
    int size = pathVector.size();
    if (size < MIN_PATHVECTOR_LENGTH) {
        return false;
    }
    string mRootPath = pathVector[COMMON_NUM::TWO];
    HILOGI(" mRootPath: %{public}s", mRootPath.c_str());
    if (mRootPath.empty()) {
        return false;
    }
    if (!strcmp(mRootPath.c_str(), MATCH_ROOTS.c_str())) {
        return true;
    }
    return false;
}
bool FmsUtils::IsChildDirectory(const Uri &uri) const
{
    std::vector<string> pathVector;
    const_cast<Uri &>(uri).GetPathSegments(pathVector);
    string mLastPath;
    if (pathVector.size() - 1 >= 0) {
        mLastPath = pathVector[pathVector.size() - 1];
    }
    string mRootPath = pathVector[COMMON_NUM::TWO];
    HILOGI("FmsUtils mLastPath : %{public}s", mLastPath.c_str());
    HILOGI("FmsUtils mRootPath: %{public}s", mRootPath.c_str());
    if (mRootPath.empty() || mLastPath.empty()) {
        return false;
    }
    if ((!strcmp(mRootPath.c_str(), MATCH_FILE.c_str())) &&
        (!strcmp(mLastPath.c_str(), MATCH_FILE_CHILDREN.c_str()))) {
        return true;
    }
    return false;
}
string FmsUtils::GetCurrentPath(const Uri &uri) const
{
    std::vector<string> pathVector;
    std::string mPath = "";
    const_cast<Uri &>(uri).GetPathSegments(pathVector);
    for (unsigned int count = 3; count < pathVector.size() - 1; count++) {
        printf("pathVector------>%s\n", pathVector[count].c_str());
    }
    if (!IsPublicStorage(uri) && IsPrimaryUser(uri)) {
        mPath = "/data/accounts/account_0/appdata/ohos.acts.test/";
        if (IsChildDirectory(uri)) {
            for (unsigned int count = 3; count < pathVector.size() - 1; count++) {
                mPath.append(pathVector[count]);
                mPath.append("/");
            }
        } else if (IsRootDirectory(uri)) {
            for (unsigned int count = 3; count < pathVector.size(); count++) {
                mPath.append(pathVector[count]);
                mPath.append("/");
            }
        }
        mPath = "";
    } else if (!IsPrimaryUser(uri)) {
        mPath = "/dev/block/";
        mPath.append(GetCurrentUser(uri));
        mPath.append("/");
        if (IsChildDirectory(uri)) {
            for (unsigned int count = 3; count < pathVector.size() - 1; count++) {
                mPath.append(pathVector[count]);
                mPath.append("/");
            }
        } else if (IsRootDirectory(uri)) {
            for (unsigned int count = 3; count < pathVector.size(); count++) {
                mPath.append(pathVector[count]);
                mPath.append("/");
            }
        }
        mPath = "";
    }
    return mPath;
}
string FmsUtils::GetCurrentUser(const Uri &uri) const
{
    std::vector<string> pathVector;
    const_cast<Uri &>(uri).GetPathSegments(pathVector);
    string mUserPath;
    printf("FmsUtils pathVector.size(), %d \n", pathVector.size());
    if (pathVector.size() > 1) {
        mUserPath = pathVector[1];
    }
    HILOGI("userPath: %{public}s", mUserPath.c_str());

    return mUserPath;
}
std::shared_ptr<ResultSet> FmsUtils::VectorToResultset1(const std::vector<std::string> &columns) const
{
    printf("==================resultset vector<FileInfo> start ======================\n");
    int listSize = columns.size();
    string column = "";
    for (int i = 0; i < listSize; i++) {
        column = columns.at(i);
        printf("columns[%d]--->%s", i, column.c_str());
    }
    printf("==================resultset vector<FileInfo> end ========================\n");
    Parcel parcel(nullptr);
    bool result = false;

    int size = columns.size();
    printf("VectorToResultset, size=%d\n", size);
    if (size <= 0) {
        return nullptr;
    }
    result = parcel.WriteStringVector(columns);
    if (result) {
        ResultSet *value = ResultSet::Unmarshalling(parcel);
        std::shared_ptr<ResultSet> ptr(value);
        return ptr;
    } else {
        return nullptr;
    }
}

std::shared_ptr<ResultSet> FmsUtils::Int32ToResultset(int32_t parm) const
{
    printf("==================resultset int32_t start ======================\n");
    printf("param -->%d\n", parm);
    printf("==================resultset int32_t  end  ======================\n");
    Parcel parcel(nullptr);
    bool result = false;
    result = parcel.WriteInt32(parm);
    if (result) {
        ResultSet *value = ResultSet::Unmarshalling(parcel);
        std::shared_ptr<ResultSet> ptr(value);
        return ptr;
    } else {
        return nullptr;
    }
}

std::shared_ptr<ResultSet> FmsUtils::VectorToResultset(const std::vector<FileInfo> &columns) const
{
    printf("==================resultset vector<FileInfo> start ======================\n");
    int listSize = columns.size();
    FileInfo fileInfo;
    for (int i = 0; i < listSize; i++) {
        fileInfo = columns.at(i);
        printf("---------------%d--------------------\n", i);
        printf("fi--fileSize-->%d\n", fileInfo.fileSize);
        printf("fi--lastUseTime-->%lld\n", fileInfo.lastUseTime);
        printf("fi--fileUri-->%s\n", fileInfo.fileUri.c_str());
        printf("fi--typeDir-->%d\n", fileInfo.typeDir);
        printf("fi--fileName-->%s\n", fileInfo.fileName.c_str());
        printf("fi--dirNum-->%d\n", fileInfo.dirNum);
        printf("fi--mimeType-->%s\n", fileInfo.mimeType.c_str());
    }
    printf("==================resultset vector<FileInfo> end ========================\n");
    Parcel parcel(nullptr);
    bool result = false;

    int size = columns.size();
    printf("VectorToResultset, size=%d\n", size);
    // Parcel未提供该方法，需要寻找替代方案
    if (result) {
        ResultSet *value = ResultSet::Unmarshalling(parcel);
        std::shared_ptr<ResultSet> ptr(value);
        return ptr;
    } else {
        return nullptr;
    }
}
int32_t FmsUtils::Mkdirs(string path) const
{
    if (opendir(path.c_str()) != nullptr) {
        printf(" Mkdirs opendir success, return -1 \n");
        return -1;
    }

    if (strstr(path.c_str(), ":") != nullptr || strstr(path.c_str(), "*") != nullptr ||
        strstr(path.c_str(), "?") != nullptr || strstr(path.c_str(), "<") != nullptr ||
        strstr(path.c_str(), ">") != nullptr || strstr(path.c_str(), "|") != nullptr) {
        printf("Contain illegal characters, return -1 \n");
        return -1;
    }

    for (size_t i = 1; i < path.length(); ++i) {
        if (path[i] == '/') {
            path[i] = '\0';
            if (access(path.c_str(), 0) != 0) {
                int t = mkdir(path.c_str(), DIR_FAULT_PERM);
                if (t == -1) {
                    return -1;
                }
            }
            path[i] = '/';
        }
    }
    if (path.length() > 0 && access(path.c_str(), 0) != 0) {
        int t = mkdir(path.c_str(), DIR_FAULT_PERM);
        if (t == -1) {
            return -1;
        }
    }
    return 0;
}

int FmsUtils::GetCurrentDirFileInfoList(std::string path, std::vector<FileInfo> &fileList) const
{
    printf(" GetFileInfoList\n");
    DIR *pDir = nullptr;
    struct dirent *ptr;
    FileInfo fileInfo;
    string fullPath;
    if (!(pDir = opendir(path.c_str()))) {
        printf(" GetFileInfoList opendir fail return\n");
        return STATUS_NUM::FAIL;
    }
    while ((ptr = readdir(pDir)) != nullptr) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
            fullPath = path + ptr->d_name;
            if (ptr->d_type == DT_DIR) {
                fileInfo = GetFileInfo(fullPath, ptr->d_name, true);
            } else {
                fileInfo = GetFileInfo(fullPath, ptr->d_name, false);
            }
            fileList.push_back(fileInfo);
        }
    }
    closedir(pDir);
    return STATUS_NUM::OK;
}

size_t FmsUtils::GetFileSize(const std::string &fileName) const
{
    FILE *fp = fopen(fileName.c_str(), "r");
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fclose(fp);
    return size;
}
int FmsUtils::GetDirNum(string path) const
{
    int size = 0;
    DIR *pDir = nullptr;
    struct dirent *ptr;
    if ((pDir = opendir(path.c_str())) == nullptr) {
        printf("can't open dir.\n");
        return -1;
    }
    while ((ptr = readdir(pDir)) != nullptr) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        size = size + 1;
    }
    return size;
}

FileInfo FmsUtils::GetFileInfo(string sourcePath, string sourceName, bool isDir) const
{
    FileInfo fi;
    struct stat tmp;
    int r = stat(sourcePath.c_str(), &tmp);
    if (r == 0) {
        if (isDir) {
            fi.typeDir = 1;
            fi.dirNum = GetDirNum(sourcePath);
            fi.mimeType = "";
            fi.fileSize = 0;
        } else {
            fi.typeDir = 0;
            fi.dirNum = 0;
            fi.mimeType = GetFileType(sourcePath);
            fi.fileSize = tmp.st_size;
        }
        fi.lastUseTime = tmp.st_mtime * MULTIPLY_BY_THOUSAND +
                         static_cast<int64_t>((tmp.st_mtim).tv_nsec / DIVIDE_BY_MILLION);
        fi.fileUri = sourcePath;
        fi.fileName = sourceName;
    }
    return fi;
}

void FmsUtils::GetSearchFileInfoList(string dirPath,
                                     string fileName,
                                     std::vector<FileInfo> &fileInfoList) const
{
    DIR *pDir = nullptr;
    struct dirent *ptr;
    string fullPath;
    FileInfo fileInfo;
    string mCurrentFileName;
    if ((pDir = opendir(dirPath.c_str())) == nullptr) {
        printf("can't open dir dirPath = %s \n", dirPath.c_str());
    }
    while ((ptr = readdir(pDir)) != nullptr) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        mCurrentFileName = ptr->d_name;
        if (ptr->d_type & DT_DIR) {
            fullPath = dirPath + ptr->d_name + "/";
            if (mCurrentFileName.find(fileName.c_str(), 0) != string::npos) {
                fileInfo = GetFileInfo(fullPath, ptr->d_name, true);
                fileInfoList.push_back(fileInfo);
            }
            GetSearchFileInfoList(fullPath, fileName, fileInfoList);
        } else {
            if (mCurrentFileName.find(fileName.c_str(), 0) != string::npos) {
                fullPath = dirPath + ptr->d_name;
                fileInfo = GetFileInfo(fullPath, ptr->d_name, false);
                fileInfoList.push_back(fileInfo);
            }
        }
    }
    closedir(pDir);
}

std::string FmsUtils::GetFileType(const std::string &fileName) const
{
    string suffixStr = fileName.substr(fileName.find_last_of('.') + 1);
    return suffixStr;
}
} // namespace FileManager
} // namespace OHOS

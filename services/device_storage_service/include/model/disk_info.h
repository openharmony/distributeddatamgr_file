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
#ifndef STORAGE_DISKINFO_H
#define STORAGE_DISKINFO_H
#include <string>
#include "parcel.h"

namespace OHOS {
namespace DS {
class DiskInfo : public Parcelable {
public:
    DiskInfo(const std::string &id, int32_t flags);
    DiskInfo();
    virtual ~DiskInfo();

    enum Flags {
        kAdoptable = 1 << 0,
        kDefaultPrimary = 1 << 1,
        kSd = 1 << 2,
        kUsb = 1 << 3,
    };

    const std::string &GetId() const
    {
        return mId;
    }
    const std::string &GetSysPath() const
    {
        return mSysPath;
    }
    uint64_t GetSize() const
    {
        return mSize;
    }
    const std::string &GetLabel() const
    {
        return mLabel;
    }
    int32_t GetFlags() const
    {
        return mFlags;
    }
    void SetSize(uint64_t size);
    void SetSysPath(const std::string &sysPath);
    void SetLabel(const std::string &label);
    bool IsUsb();
    bool IsSd();
    bool IsAdoptable();
    const std::string &GetDescription();

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static DiskInfo *Unmarshalling(Parcel &parcel);

private:
    std::string mId;
    std::string mSysPath;
    uint64_t mSize;
    std::string mLabel;
    int32_t mFlags;
};
} // namespace DS
} // namespace OHOS

#endif // STORAGE_DISKINFO_H

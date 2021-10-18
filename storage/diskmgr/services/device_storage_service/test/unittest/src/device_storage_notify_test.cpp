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
#include "device_storage_notify_test.h"

#include <common_event_data.h>
#include <common_event_manager.h>
#include <common_event_publish_info.h>
#include <common_event_subscriber.h>
#include <common_event_support.h>
#include <datetime_ex.h>
#include <gtest/gtest.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <string_ex.h>

#include "device_storage_service.h"
#include "device_storage_manager.h"
using namespace testing::ext;
using namespace OHOS;
using namespace std;
using namespace OHOS::AAFwk;
using namespace OHOS::EventFwk;

using NeedWaitFunc = std::function<bool()>;
using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace {
constexpr int MAX_RETRY_TIME = 2;
constexpr int WAIT_EVENT_TIME_S = 1;
constexpr int RETRY_WAIT_TIME_US = 50000;
constexpr int SLEEP_WAIT_TIME_S = 3;
constexpr int STATE_MOUNTED = 2;
constexpr int STATE_UNMOUNTED = 0;
constexpr int VOLUME_TYPE = 0;

class TestCommonEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
public:
    std::mutex onReceivedLock_;
    bool received_ = false;
    TimePoint receivedTime_;
    std::string action_;
    shared_ptr<OHOS::AppExecFwk::EventHandler> receiveHandler_ = nullptr;
    bool mounted = false;

    explicit TestCommonEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &subscribeInfo)
        : OHOS::EventFwk::CommonEventSubscriber(subscribeInfo)
    {}

    TestCommonEventSubscriber() {}

    ~TestCommonEventSubscriber() override {}
    void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data) override
    {
        GTEST_LOG_(INFO) << "DeviceStorageNotifyTest:: OnReceive!!";
        receiveHandler_ = OHOS::AppExecFwk::EventHandler::Current();
        receivedTime_ = Clock::now();
        received_ = true;
        OHOS::AAFwk::Want want = data.GetWant();
        action_ = data.GetWant().GetAction();
        if (action_ == OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_DISK_MOUNTED) {
            GTEST_LOG_(INFO) << "DeviceStorageNotifyTest:: OnReceive,Volume is Mounted";
            mounted = true;
        }
        if (action_ == OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_DISK_UNMOUNTED) {
            GTEST_LOG_(INFO) << "DeviceStorageNotifyTest:: OnReceive,Volume is UnMounted";
            mounted = false;
        }
    }
};

shared_ptr<TestCommonEventSubscriber> RegisterEvent()
{
    GTEST_LOG_(INFO) << "DeviceStorageNotifyTest:: Regist Subscriber Start!!";
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_DISK_MOUNTED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_DISK_UNMOUNTED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    int tryTimes = 0;
    std::shared_ptr<TestCommonEventSubscriber> subscriber =
        std::make_shared<TestCommonEventSubscriber>(subscriberInfo);
    while (tryTimes < MAX_RETRY_TIME) {
        const auto result = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber);
        if (ERR_OK == result) {
            break;
        } else {
            GTEST_LOG_(INFO) << "DeviceStorageNotifyTest:: Fail to register Subscriber, Sleep 50ms and try "
                                "again!!!";
            usleep(RETRY_WAIT_TIME_US);
            subscriber = std::make_shared<TestCommonEventSubscriber>(subscriberInfo);
        }
        tryTimes++;
    }
    if (MAX_RETRY_TIME == tryTimes) {
        GTEST_LOG_(INFO) << "DeviceStorageNotifyTest:: Fail to register Subscriber!!!";
        return nullptr;
    }
    GTEST_LOG_(INFO) << "DeviceStorageNotifyTest:: register Subscriber Success!!";
    return subscriber;
}
} // namespace

void DeviceStorageNotifyTest::SetUpTestCase(void) {}

void DeviceStorageNotifyTest::TearDownTestCase(void) {}

void DeviceStorageNotifyTest::SetUp(void) {}

void DeviceStorageNotifyTest::TearDown(void) {}

/**
 * @tc.name: DeviceStorageNotifyTest001
 * @tc.desc: test device storage for broadcast volumestate
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStorageNotifyTest, DeviceStorageNotifyTest001, TestSize.Level0)
{
    int waitForStatusOk = 15;
    sleep(waitForStatusOk);
    GTEST_LOG_(INFO) << "DeviceStorageNotifyTest001lli.wang: Test device storage for broadcast volumestate "
                        "start.";
    std::shared_ptr<DeviceStorageManager> dsm = DelayedSingleton<DeviceStorageManager>::GetInstance();
    GTEST_LOG_(INFO) << "DeviceStorageNotifyTest001: Wakeup Device before test.";
    std::vector<std::shared_ptr<DS::VolumeInfo>> infos;
    bool ret = dsm->GetVolumes(infos);
    if (ret) {
        GTEST_LOG_(INFO) << "size is" << infos.size();
        for (auto &iter : infos) {
            GTEST_LOG_(INFO) << "DeviceStorageNotifyTest001" << iter->GetState() << iter->GetType();
            if (iter->GetType() == VOLUME_TYPE && iter->GetState() == STATE_MOUNTED) {
                GTEST_LOG_(INFO) << "DeviceStorageNotifyTest001:Finded the volume!";
                dsm->UnMount(iter->GetId());
                sleep(SLEEP_WAIT_TIME_S); // wait for 3 second
                EXPECT_EQ(iter->GetState(), STATE_UNMOUNTED) << "DeviceStorageNotifyTest001: unmounted Fail";
                GTEST_LOG_(INFO) << "DeviceStorageNotifyTest001: Volume is Unmounted, Begin to Mount!";
                shared_ptr<TestCommonEventSubscriber> subscriber = RegisterEvent();
                EXPECT_FALSE(subscriber == nullptr);
                dsm->Mount(iter->GetId());
                sleep(WAIT_EVENT_TIME_S);
                auto err = CommonEventManager::UnSubscribeCommonEvent(subscriber);
                EXPECT_EQ(ERR_OK, err);
                GTEST_LOG_(INFO) << "DeviceStorageNotifyTest001: Test volumestate broadcast end.";
            }
        }
    } else {
        GTEST_LOG_(INFO) << "volumeInfos_ data is empty";
        return;
    }
}

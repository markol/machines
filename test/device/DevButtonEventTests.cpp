#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "device/butevent.hpp"
#include "butevent.cpp"

using ::testing::AtLeast;
using ::testing::Return;

class MockDevTime
{
public:
    MOCK_METHOD(double, time, (), (const));
};

class ButtonEvent : public DevButtonEventT<MockDevTime>
{
public:
    ButtonEvent(    ScanCode code,
                    DevButtonEventT::Action   action,
                    bool prev, bool shift, bool ctrl, bool alt,
                    double time,
                    int x, int y,
                    ushort repeat,
                    char print)
        : DevButtonEventT<MockDevTime>(code,action,prev,shift,ctrl,alt,time,x,y,repeat,print)
    {}

    void setMock(MockDevTime* mock)
    {
        this->timeDependency_.set(*mock);
    }
};

///////////////////////////////////////////////////////////////////////////////

TEST(DevButtonEventTests, GetAgeOfButtonEvent)
{
    MockDevTime mockTimeSingleton;

    EXPECT_CALL(mockTimeSingleton, time())
            .Times(AtLeast(1))
            .WillOnce(Return(10001.0));

    auto buttonEvent =
            ButtonEvent{ ButtonEvent::ScanCode::HOME_PAD, ButtonEvent::PRESS, false, false, false, false, 10000.0, 20, 20, 1, 'H' };

    buttonEvent.setMock(&mockTimeSingleton);

    ASSERT_EQ(1.0, buttonEvent.age());
}

TEST(DevButtonEventTests, GetPrintableCharOfButtonEvent_WhenInvalid)
{
    auto buttonEvent =
            ButtonEvent{ ButtonEvent::ScanCode::HOME_PAD, ButtonEvent::PRESS, false, false, false, false, 10000.0, 20, 20, 1, '\xF4' };

    // In Machines and PlanetEd, the PRE() would flip out. Not here :)
    ASSERT_EQ('\xF4', buttonEvent.printableChar());
    ASSERT_FALSE(buttonEvent.isPrintable());
}

TEST(DevButtonEventTests, GetActionOfButtonEvent)
{
    auto eventPress =
            ButtonEvent{ ButtonEvent::ScanCode::MIDDLE_MOUSE, ButtonEvent::PRESS, false, false, false, false, 10000.0, 20, 20, 1, 'M' };
    auto eventRelease =
            ButtonEvent{ ButtonEvent::ScanCode::MIDDLE_MOUSE, ButtonEvent::RELEASE, false, false, false, false, 10000.0, 20, 20, 1, 'M' };
    auto eventScrollUp =
            ButtonEvent{ ButtonEvent::ScanCode::MIDDLE_MOUSE, ButtonEvent::SCROLL_UP, false, false, false, false, 10000.0, 20, 20, 1, 'M' };
    auto eventScrollDown =
            ButtonEvent{ ButtonEvent::ScanCode::MIDDLE_MOUSE, ButtonEvent::SCROLL_DOWN, false, false, false, false, 10000.0, 20, 20, 1, 'M' };

    ASSERT_EQ(ButtonEvent::PRESS, eventPress.action());
    ASSERT_EQ(ButtonEvent::RELEASE, eventRelease.action());
    ASSERT_EQ(ButtonEvent::SCROLL_UP, eventScrollUp.action());
    ASSERT_EQ(ButtonEvent::SCROLL_DOWN, eventScrollDown.action());
}

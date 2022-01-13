#include <utility>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "device/mouse.hpp"
#include "butevent.cpp"
#include "sdlmouse.cpp"

using ::testing::TypedEq;
using ::testing::Return;

class MockDevTime
{
public:
    MOCK_METHOD(double, time, (), (const));
};
//Needed for the devbuttonevent .-_-;
ostream& operator<<(ostream& o, const DevButtonEventT<MockDevTime>&)
{
    return o;
}

class MockDevEventQueue
{
public:
    MOCK_METHOD(void, queueEvent, (const DevButtonEvent& event), ());
    MOCK_METHOD(void, queueEvent, (const DevButtonEventT<MockDevTime>& event), ());
};

class MockRecRecorder
{
public:
    MOCK_METHOD(RecRecorder::State, state, (), (const));
};

class MockRecRecorderPrivate
{
public:
    MOCK_METHOD(DevMousePosition, playbackMousePosition, (), (const));
    MOCK_METHOD(bool, playbackLeftButton, (), (const));
    MOCK_METHOD(bool, playbackRightButton, (), (const));
    MOCK_METHOD(void, recordMousePosition, (const DevMousePosition& position), ());
    MOCK_METHOD(void, recordLeftButton, (bool state), ());
    MOCK_METHOD(void, recordRightButton, (bool state), ());
};

class MockSdlDelegate : public SdlDelegate
{
public:
    MOCK_METHOD(int, showCursor, (const int toggle), (override));
    using IntPair = std::pair<int,int>;
    MOCK_METHOD(IntPair, getCursorPosition, (), (override));
    MOCK_METHOD(void, moveCursorToPosition, (SDL_Window* window, const int x, const int y), (override));
};
MockSdlDelegate mockSDL;

class Mouse : public DevMouseT<MockRecRecorder, MockRecRecorderPrivate, MockDevTime, MockDevEventQueue>
{
public:
    Mouse()
        : DevMouseT(&mockSDL)
    {}

    void setMocks(MockRecRecorder* rec, MockRecRecorderPrivate* recPriv, MockDevEventQueue* eq)
    {
        this->recorderDependency_.set(*rec);
        this->recorderPrivDependency_.set(*recPriv);
        this->eventQueueDependency_.set(*eq);
    }

    using DevMouseT<MockRecRecorder, MockRecRecorderPrivate, MockDevTime, MockDevEventQueue>::wm_button;
};

///////////////////////////////////////////////////////////////////////////////

TEST(DevMouseTests, WMbutton_DispatchesClick)
{
    MockRecRecorder recorder;
    MockRecRecorderPrivate privRecorder;
    MockDevEventQueue eventQueue;

    // Essentially AfxSdlApp::dispatchMouseEvent()
    const auto code = DevKey::LEFT_MOUSE;
    const auto act  = DevButtonEventT<MockDevTime>::PRESS;
    const bool previous = 0;
    const bool shift = false;
    const bool ctrl = false;
    const bool alt = false;
    const double time = 1000.0;
    const int x = 100;
    const int y = 100;
    const size_t repeats = 1;
    const DevButtonEventT<MockDevTime> ev(code, act, previous, shift, ctrl, alt, time, x, y, repeats);

    EXPECT_CALL(eventQueue, queueEvent(TypedEq<decltype(ev)&>(ev)))
            .Times(1);

    EXPECT_CALL(recorder, state())
            .WillRepeatedly(Return(RecRecorder::INACTIVE));

    Mouse mouse;
    mouse.setMocks(&recorder, &privRecorder, &eventQueue);

    // DISPATCH THE MOUSE EVENT
    mouse.wm_button(ev);

    const auto& position = mouse.position();
    ASSERT_TRUE(mouse.leftButton());
    ASSERT_EQ(x, position.first);
    ASSERT_EQ(y, position.second);
}

TEST(DevMouseTests, WMbutton_DispatchesScrollUp)
{
    MockRecRecorder recorder;
    MockRecRecorderPrivate privRecorder;
    MockDevEventQueue eventQueue;

    // Essentially AfxSdlApp::dispatchMouseScrollEvent()
    const auto code = DevKey::MIDDLE_MOUSE;
    const auto act  = DevButtonEventT<MockDevTime>::SCROLL_UP;
    const bool previous = 0;
    const bool shift = false;
    const bool ctrl = false;
    const bool alt = false;
    const double time = 1000.0;
    const int x = 100;
    const int y = 100;
    const size_t repeats = 1;
    const DevButtonEventT<MockDevTime> ev(code, act, previous, shift, ctrl, alt, time, x, y, repeats);

    EXPECT_CALL(eventQueue, queueEvent(TypedEq<decltype(ev)&>(ev)))
            .Times(1);

    EXPECT_CALL(recorder, state())
            .WillRepeatedly(Return(RecRecorder::INACTIVE));

    Mouse mouse;
    mouse.setMocks(&recorder, &privRecorder, &eventQueue);

    // DISPATCH THE MOUSE EVENT
    mouse.wm_button(ev);

    const auto& position = mouse.position();
    ASSERT_FALSE(mouse.leftButton());
    ASSERT_FALSE(mouse.rightButton());
    ASSERT_EQ(x, position.first);
    ASSERT_EQ(y, position.second);
    ASSERT_TRUE(mouse.wheelScrollUp());
    // Unless mouse.wm_button(ev) is called again with another scroll, this shall be false.
    ASSERT_FALSE(mouse.wheelScrollUp());
}

TEST(DevMouseTests, WMbutton_DispatchesScrollDown)
{
    MockRecRecorder recorder;
    MockRecRecorderPrivate privRecorder;
    MockDevEventQueue eventQueue;

    // Essentially AfxSdlApp::dispatchMouseScrollEvent()
    const auto code = DevKey::MIDDLE_MOUSE;
    const auto act  = DevButtonEventT<MockDevTime>::SCROLL_DOWN;
    const bool previous = 0;
    const bool shift = false;
    const bool ctrl = false;
    const bool alt = false;
    const double time = 1000.0;
    const int x = 100;
    const int y = 100;
    const size_t repeats = 1;
    const DevButtonEventT<MockDevTime> ev(code, act, previous, shift, ctrl, alt, time, x, y, repeats);

    EXPECT_CALL(eventQueue, queueEvent(TypedEq<decltype(ev)&>(ev)))
            .Times(1);

    EXPECT_CALL(recorder, state())
            .WillRepeatedly(Return(RecRecorder::INACTIVE));

    Mouse mouse;
    mouse.setMocks(&recorder, &privRecorder, &eventQueue);

    // DISPATCH THE MOUSE EVENT
    mouse.wm_button(ev);

    const auto& position = mouse.position();
    ASSERT_FALSE(mouse.leftButton());
    ASSERT_FALSE(mouse.rightButton());
    ASSERT_EQ(x, position.first);
    ASSERT_EQ(y, position.second);
    ASSERT_TRUE(mouse.wheelScrollDown());
    // Unless mouse.wm_button(ev) is called again with another scroll, this shall be false.
    ASSERT_FALSE(mouse.wheelScrollDown());
}

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "device/eventq.hpp"
#include "butevent.cpp"
#include "eventq.cpp"

using ::testing::Return;

class MockDevTime
{
public:
    MOCK_METHOD(double, time, (), (const));
};

class MockRecRecorder
{
public:
    MOCK_METHOD(RecRecorder::State, state, (), (const));
};

class MockRecRecorderPrivate
{
public:
    MOCK_METHOD(DevButtonEvent, playbackButtonEvent, (), (const));
    MOCK_METHOD(size_t, playbackEventQueueLength, (), (const));
    MOCK_METHOD(void, recordButtonEvent, (const DevButtonEvent& event), ());
    MOCK_METHOD(void, recordEventQueueLength, (size_t length), ());
};

class EventQueue : public DevEventQueueT<MockRecRecorder, MockRecRecorderPrivate, MockDevTime>
{
public:
    EventQueue() {}
    virtual ~EventQueue() {}

    void setMocks(MockRecRecorder* rec, MockRecRecorderPrivate* recPriv)
    {
        this->recorderDependency_.set(*rec);
        this->recorderPrivDependency_.set(*recPriv);
    }

    using DevEventQueueT<MockRecRecorder, MockRecRecorderPrivate, MockDevTime>::getReleaseFilterFor;
    using DevEventQueueT<MockRecRecorder, MockRecRecorderPrivate, MockDevTime>::getPressFilterFor;
    using DevEventQueueT<MockRecRecorder, MockRecRecorderPrivate, MockDevTime>::queueEvent;
};

class EventQueue_RealTime : public DevEventQueueT<MockRecRecorder, MockRecRecorderPrivate, DevTime>
{
public:
    EventQueue_RealTime() {}
    virtual ~EventQueue_RealTime() {}

    void setMocks(MockRecRecorder* rec, MockRecRecorderPrivate* recPriv)
    {
        this->recorderDependency_.set(*rec);
        this->recorderPrivDependency_.set(*recPriv);
    }

    using DevEventQueueT<MockRecRecorder, MockRecRecorderPrivate, DevTime>::getReleaseFilterFor;
    using DevEventQueueT<MockRecRecorder, MockRecRecorderPrivate, DevTime>::getPressFilterFor;
    using DevEventQueueT<MockRecRecorder, MockRecRecorderPrivate, DevTime>::getScrollUpFilter;
    using DevEventQueueT<MockRecRecorder, MockRecRecorderPrivate, DevTime>::getScrollDownFilter;
    using DevEventQueueT<MockRecRecorder, MockRecRecorderPrivate, DevTime>::queueEvent;
};

///////////////////////////////////////////////////////////////////////////////

TEST(DevEventQueueTests, LengthIsEmpty_WhenQueueEmpty)
{
    MockRecRecorder recorder;
    MockRecRecorderPrivate privRecorder;

    EXPECT_CALL(recorder, state())
            .WillRepeatedly(Return(RecRecorder::INACTIVE));

    EventQueue eventQueue;
    eventQueue.setMocks(&recorder, &privRecorder);

    ASSERT_EQ(true, eventQueue.isEmpty());
    ASSERT_EQ(0, eventQueue.length());
}

TEST(DevEventQueueTests, QueueEvents_ScanCode)
{
    EventQueue eventQueue;

    eventQueue.queueEvents(DevKey::HOME_PAD);
    ASSERT_EQ(true, eventQueue.getPressFilterFor(DevKey::HOME_PAD));
    ASSERT_EQ(true, eventQueue.getReleaseFilterFor(DevKey::HOME_PAD));
}

TEST(DevEventQueueTests, DontQueueEvents_ScanCode)
{
    EventQueue eventQueue;

    eventQueue.dontQueueEvents(DevKey::HOME_PAD);
    ASSERT_EQ(false, eventQueue.getPressFilterFor(DevKey::HOME_PAD));
    ASSERT_EQ(false, eventQueue.getReleaseFilterFor(DevKey::HOME_PAD));
}

TEST(DevEventQueueTests, QueueEvents_ScanCodeAndAction)
{
    EventQueue eventQueue;
    auto swapAction = [](EventQueue::Action action) -> EventQueue::Action {
        return (action == EventQueue::Action::PRESS) ? EventQueue::Action::RELEASE : EventQueue::Action::PRESS;
    };


    auto action = EventQueue::Action::PRESS;
    bool wantPress = true;
    bool wantRelease = false;
    // ESCAPE = 1
    for (int c = DevKey::ESCAPE; c < DevKey::MAX_CODE; ++c)
    {
        eventQueue.queueEvents(static_cast<EventQueue::ScanCode>(c), action);

        ASSERT_EQ(wantPress, eventQueue.getPressFilterFor(static_cast<EventQueue::ScanCode>(c)));
        ASSERT_EQ(wantRelease, eventQueue.getReleaseFilterFor(static_cast<EventQueue::ScanCode>(c)));

        action = swapAction(action);
        wantPress = !wantPress;
        wantRelease = !wantRelease;
    }
}

TEST(DevEventQueueTests, DontQueueEvents_ScanCodeAndAction)
{
    EventQueue eventQueue;
    auto swapAction = [](EventQueue::Action action) -> EventQueue::Action {
        return (action == EventQueue::Action::PRESS) ? EventQueue::Action::RELEASE : EventQueue::Action::PRESS;
    };


    auto action = EventQueue::Action::PRESS;
    // ESCAPE = 1
    for (int c = DevKey::ESCAPE; c < DevKey::MAX_CODE; ++c)
    {
        eventQueue.dontQueueEvents(static_cast<EventQueue::ScanCode>(c), action);

        ASSERT_EQ(false, eventQueue.getPressFilterFor(static_cast<EventQueue::ScanCode>(c)));
        ASSERT_EQ(false, eventQueue.getReleaseFilterFor(static_cast<EventQueue::ScanCode>(c)));

        action = swapAction(action);
    }
}

TEST(DevEventQueueTests, QueueNewEventAndRetrieveIt_SingleEvent)
{
    MockRecRecorder recorder;
    MockRecRecorderPrivate privRecorder;

    EXPECT_CALL(recorder, state())
            .WillRepeatedly(Return(RecRecorder::INACTIVE));

    EventQueue_RealTime eventQueue;
    eventQueue.setMocks(&recorder, &privRecorder);

    // You can always count on your HomePad ;p
    auto homePadEvent =
            EventQueue_RealTime::DevButtonEventType{ DevKey::HOME_PAD, EventQueue_RealTime::Action::PRESS, false, false, false, false, 1.0, 0, 0, 1, 'H' };

    // Tell the DEQ to filter this key
    eventQueue.queueEvents(DevKey::HOME_PAD);
    ASSERT_EQ(true, eventQueue.getPressFilterFor(DevKey::HOME_PAD));

    // filterEvent() is covered by this call
    eventQueue.queueEvent(homePadEvent);
    auto retrievedEvent = eventQueue.oldestEvent();
    ASSERT_EQ('H', retrievedEvent.getChar());
    // The enqueued event shall always have a repeat count >= 1
    ASSERT_GE(homePadEvent.repeatCount(), retrievedEvent.repeatCount());
}

TEST(DevEventQueueTests, QueueNewEventAndRetrieveIt_RepeatEvents)
{
    MockRecRecorder recorder;
    MockRecRecorderPrivate privRecorder;

    EXPECT_CALL(recorder, state())
            .WillRepeatedly(Return(RecRecorder::INACTIVE));

    EventQueue_RealTime eventQueue;
    eventQueue.setMocks(&recorder, &privRecorder);

    // You can always count on your HomePad ;p
    auto homePadEvent =
            EventQueue_RealTime::DevButtonEventType{ DevKey::HOME_PAD, EventQueue_RealTime::Action::PRESS, false, false, false, false, 1.0, 0, 0, 1, 'H' };
    auto homePadEvent2 =
            homePadEvent;
    auto homePadEvent3 =
            homePadEvent;

    // Tell the DEQ to filter this key
    eventQueue.queueEvents(DevKey::HOME_PAD);
    ASSERT_EQ(true, eventQueue.getPressFilterFor(DevKey::HOME_PAD));

    // Repeated mashings!
    eventQueue.queueEvent(homePadEvent);
    eventQueue.queueEvent(homePadEvent2);
    eventQueue.queueEvent(homePadEvent3);
    // The events will be compressed
    ASSERT_EQ(1, eventQueue.length());

    auto retrievedEvent = eventQueue.oldestEvent();
    ASSERT_EQ('H', retrievedEvent.getChar());
    // The result event will have repeat count of 1
    ASSERT_GE(1, retrievedEvent.repeatCount());
}

TEST(DevEventQueueTests, QueueNewEventAndRetrieveIt_MouseScrollEvent)
{
    MockRecRecorder recorder;
    MockRecRecorderPrivate privRecorder;

    EXPECT_CALL(recorder, state())
            .WillRepeatedly(Return(RecRecorder::INACTIVE));

    EventQueue_RealTime eventQueue;
    eventQueue.setMocks(&recorder, &privRecorder);

    auto scrollUpEvent =
            EventQueue_RealTime::DevButtonEventType{ DevKey::MIDDLE_MOUSE, EventQueue_RealTime::Action::SCROLL_UP, false, false, false, false, 1.0, 0, 0, 1, '\xBE' };
    auto scrollDownEvent =
            EventQueue_RealTime::DevButtonEventType{ DevKey::MIDDLE_MOUSE, EventQueue_RealTime::Action::SCROLL_DOWN, false, false, false, false, 1.0, 0, 0, 1, '\xBF' };

    eventQueue.queueEvents(DevKey::MIDDLE_MOUSE, EventQueue_RealTime::Action::SCROLL_UP);
    ASSERT_EQ(true, eventQueue.getScrollUpFilter());
    eventQueue.queueEvents(DevKey::MIDDLE_MOUSE, EventQueue_RealTime::Action::SCROLL_DOWN);
    ASSERT_EQ(true, eventQueue.getScrollDownFilter());

    eventQueue.queueEvent(scrollUpEvent);
    eventQueue.queueEvent(scrollDownEvent);
    ASSERT_EQ(2, eventQueue.length());

    auto upEvent = eventQueue.oldestEvent();
    auto downEvent = eventQueue.oldestEvent();
    ASSERT_EQ('\xBE', upEvent.getChar());
    ASSERT_EQ('\xBF', downEvent.getChar());

    eventQueue.dontQueueEvents(DevKey::MIDDLE_MOUSE, EventQueue_RealTime::Action::SCROLL_UP);
    ASSERT_EQ(false, eventQueue.getScrollUpFilter());
    eventQueue.dontQueueEvents(DevKey::MIDDLE_MOUSE, EventQueue_RealTime::Action::SCROLL_DOWN);
    ASSERT_EQ(false, eventQueue.getScrollDownFilter());
}

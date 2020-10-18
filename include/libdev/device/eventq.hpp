/*
 * E V E N T Q . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _DEVICE_EVENTQ_HPP
#define _DEVICE_EVENTQ_HPP

#include "base/base.hpp"
#include "device/butevent.hpp"

//class ostream;
template <class T> class ctl_list;

// Stores a FIFO of input device events (currently just button events).
class DevEventQueue
{
public:
	typedef DevButtonEvent::ScanCode	ScanCode;
	typedef DevButtonEvent::Action		Action;

    //  Singleton class
    static DevEventQueue& instance();
    ~DevEventQueue();

	// Removes the oldest event from the queue.  If an event has a repeat
	// counts greater than one, this fn. will return multiple copies each of
	// which has its count set to one, hence the post-condition.
	// PRE(!isEmpty());
	// POST(result.repeatCount() == 1);
	DevButtonEvent oldestEvent();

	// For each scan-code you must request that events be queued; otherwise they
	// won't be added to the queue.  You can request both press and release
	// events, using the first form; or you can request just one or the other.
	// By default no events are stored in the queue.
	// PRE(code < DevKey::MAX_CODE);  applies to all ScanCode args
	void queueEvents(ScanCode);
	void dontQueueEvents(ScanCode);
	void queueEvents(ScanCode, Action);
	void dontQueueEvents(ScanCode, Action);

	// Throw away certain events.  Four flavours: absolutely everything; all
	// events relating to a given button; any event older than a given time;
	// and events relating to a given button, older than the given time.
	void discardAll();
	void discard(ScanCode);
	void discardOlderThan(double time);
	void discardOlderThan(double time, ScanCode);

	size_t length() const;
	bool   isEmpty() const;		// length() == 0

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const DevEventQueue& t );

private:
	// Only these classes can add events to the back of the queue.
	friend class DevWin95Keyboard;
	friend class DevSdlKeyboard;
	friend class DevMouse;

	// PRE(event.scanCode() < DevKey::MAX_CODE);
	void queueEvent(const DevButtonEvent&);

	// Internal convenience methods.
    DevEventQueue();
	bool filterEvent(const DevButtonEvent&) const;

	ctl_list<DevButtonEvent>*	list_;

	// These tables determine which events are queued.
	uchar	releaseFilter_[DevKey::MAX_CODE];
	uchar	  pressFilter_[DevKey::MAX_CODE];

    // Operation deliberately revoked
    DevEventQueue( const DevEventQueue& );
    DevEventQueue& operator =( const DevEventQueue& );
    bool operator ==( const DevEventQueue& );
};

#endif

/* End EVENTQ.HPP ***************************************************/

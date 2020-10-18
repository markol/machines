/*
 * E V E N T Q . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "device/eventq.hpp"
#include "ctl/list.hpp"

#include "recorder/recorder.hpp"
#include "recorder/private/recpriv.hpp"

// static
DevEventQueue& DevEventQueue::instance()
{
    static DevEventQueue instance_;
    return instance_;
}

DevEventQueue::DevEventQueue():
	list_(_NEW(ctl_list<DevButtonEvent>))
{
	ASSERT(list_, "Failed to allocate device event queue.");

	// By default we don't queue anything until asked.  Reset the filters.
	for (int i=0; i!=DevKey::MAX_CODE; ++i)
		pressFilter_[i] = releaseFilter_[i] = false;

    TEST_INVARIANT;
}

DevEventQueue::~DevEventQueue()
{
    TEST_INVARIANT;
	_DELETE(list_);
}

DevButtonEvent DevEventQueue::oldestEvent()
{
    DevButtonEvent result;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackButtonEvent();
    else
    {
    	DevButtonEvent& front = list_->front();

    	if (front.repeatCount() == 1)
    	{
    		result = front;
    		list_->pop_front();
    	}
    	else
    	{
    		result = front.decompressRepeats();
    	}


        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordButtonEvent( result );
    }


	POST(result.repeatCount() == 1);
	POST(result.scanCode() < DevKey::MAX_CODE);
	return result;
}

void DevEventQueue::queueEvent(const DevButtonEvent& event)
{
	PRE(event.scanCode() < DevKey::MAX_CODE);

	if (filterEvent(event))
	{
		// If there is an event at the back of the queue which matches the
		// current one, they are compressed and we don't add a new event.

        //  list_->size() is used rather than length() because this function
        //  is called from the Windows event handler. This stops the recording
        //  working properly. Bob

		if (list_->size() > 0 && list_->back().compressRepeats(event))
			return;
		else
			list_->push_back(event);
	}
}

bool DevEventQueue::filterEvent(const DevButtonEvent& event) const
{
	PRE(event.scanCode() < DevKey::MAX_CODE);

	switch (event.action())
	{
		case DevButtonEvent::RELEASE:	return releaseFilter_[event.scanCode()];
		case DevButtonEvent::PRESS:		return   pressFilter_[event.scanCode()];
		default:						ASSERT_BAD_CASE;
	}

	return false;
}

void DevEventQueue::queueEvents(ScanCode code)
{
	PRE(code < DevKey::MAX_CODE);
	releaseFilter_[code] = pressFilter_[code] = true;
}

void DevEventQueue::dontQueueEvents(ScanCode code)
{
	PRE(code < DevKey::MAX_CODE);
	releaseFilter_[code] = pressFilter_[code] = false;
}

void DevEventQueue::queueEvents(ScanCode code, Action action)
{
	PRE(code < DevKey::MAX_CODE);

	switch (action)
	{
		case DevButtonEvent::RELEASE:	releaseFilter_[code] = true; break;
		case DevButtonEvent::PRESS:		  pressFilter_[code] = true; break;
		default:						ASSERT_BAD_CASE;
	}
}

void DevEventQueue::dontQueueEvents(ScanCode code, Action action)
{
	PRE(code < DevKey::MAX_CODE);

	switch (action)
	{
		case DevButtonEvent::RELEASE:	releaseFilter_[code] = false; break;
		case DevButtonEvent::PRESS:		  pressFilter_[code] = false; break;
		default:						ASSERT_BAD_CASE;
	}
}

size_t DevEventQueue::length() const
{
    size_t result;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackEventQueueLength();
    else
    {
  		result = list_->size();

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordEventQueueLength( result );
    }

    return result;
}

bool DevEventQueue::isEmpty() const
{
	return length() == 0;
}

void DevEventQueue::CLASS_INVARIANT
{
	INVARIANT(list_);

	// This will fail for default objects!
	ctl_list<DevButtonEvent>::const_iterator it = list_->begin();
	while (it != list_->end())
	{
		const DevButtonEvent& ev = *it;
		INVARIANT(ev.scanCode() < DevKey::MAX_CODE);
		++it;
	}
}

ostream& operator <<( ostream& o, const DevEventQueue& t )
{
	o << "Event queue:" << std::endl;

	ctl_list<DevButtonEvent>::const_iterator it = t.list_->begin();
	while (it != t.list_->end())
	{
		o << "\t" << *it << std::endl;
		++it;
	}

	o << std::endl;

	o << "Filter tables:	(press)		(release)\n";

	for (int i=0; i!=DevKey::MAX_CODE; ++i)
		o << i << "\t\t\t\t   " << (int) t.pressFilter_[i] << "\t\t   " << (int) t.releaseFilter_[i] << "\n";

	o << std::endl;

	return o;
}

/* End EVENTQ.CPP ***************************************************/

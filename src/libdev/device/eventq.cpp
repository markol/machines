/*
 * E V E N T Q . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "device/eventq.hpp"

// static
template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>& DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::instance()
{
    static DevEventQueue instance_;
    return instance_;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::DevEventQueueT()
    : list_(new ctl_list<typename DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::DevButtonEventType>)
{
	ASSERT(list_, "Failed to allocate device event queue.");

    // By default we don't queue anything until asked.  Reset the filters.
    for (int i=0; i!=DevKey::MAX_CODE; ++i)
    {
        pressFilter_[i] = releaseFilter_[i] = false;
    }

    // The middle mouse wheel is the only button that would receive these.
    // Explicity ask for these using queueEvents w/ middle mouse wheel scan code.
    scrollUpFilter_   = false;
    scrollDownFilter_ = false;

    TEST_INVARIANT;
}

// virtual
template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::~DevEventQueueT()
{
    TEST_INVARIANT;
    delete list_;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
typename DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::DevButtonEventType DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::oldestEvent()
{
    using ButtonEvent = typename DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::DevButtonEventType;
    ButtonEvent result;

    if( recorderDependency_.get().state() == RecRecorder::PLAYING )
    {
        result = recorderPrivDependency_.get().playbackButtonEvent();
    }
    else
    {
        ButtonEvent& front = list_->front();

    	if (front.repeatCount() == 1)
    	{
    		result = front;
    		list_->pop_front();
    	}
    	else
    	{
    		result = front.decompressRepeats();
    	}


        if( recorderDependency_.get().state() == RecRecorder::RECORDING )
        {
            recorderPrivDependency_.get().recordButtonEvent( result );
        }
    }


	POST(result.repeatCount() == 1);
	POST(result.scanCode() < DevKey::MAX_CODE);
	return result;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
void DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::queueEvent(const typename DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::DevButtonEventType& event)
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
        {
			return;
        }
		else
        {
			list_->push_back(event);
        }
	}
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
bool DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::filterEvent(const typename DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::DevButtonEventType& event) const
{
    using ButtonEvent = typename DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::DevButtonEventType;
	PRE(event.scanCode() < DevKey::MAX_CODE);

	switch (event.action())
	{
        case ButtonEvent::RELEASE:
            return releaseFilter_[event.scanCode()];
        case ButtonEvent::PRESS:
            return   pressFilter_[event.scanCode()];
        case ButtonEvent::SCROLL_UP:
            return (event.scanCode() == DevKey::MIDDLE_MOUSE and scrollUpFilter_);
        case ButtonEvent::SCROLL_DOWN:
            return (event.scanCode() == DevKey::MIDDLE_MOUSE and scrollDownFilter_);
        default:
            ASSERT_BAD_CASE;
        break;
	}

	return false;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
void DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::queueEvents(ScanCode code)
{
	PRE(code < DevKey::MAX_CODE);
	releaseFilter_[code] = pressFilter_[code] = true;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
void DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::dontQueueEvents(ScanCode code)
{
	PRE(code < DevKey::MAX_CODE);
	releaseFilter_[code] = pressFilter_[code] = false;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
void DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::queueEvents(ScanCode code, Action action)
{
    using ButtonEvent = typename DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::DevButtonEventType;
	PRE(code < DevKey::MAX_CODE);

	switch (action)
	{
        case ButtonEvent::RELEASE:
            releaseFilter_[code] = true;
        break;
        case ButtonEvent::PRESS:
            pressFilter_[code] = true;
        break;
        case ButtonEvent::SCROLL_UP:
            // only set for middle mouse
            scrollUpFilter_ = code == DevKey::MIDDLE_MOUSE;
        break;
        case ButtonEvent::SCROLL_DOWN:
            // only set for middle mouse
            scrollDownFilter_ = code == DevKey::MIDDLE_MOUSE;
        break;
        default:
            ASSERT_BAD_CASE;
        break;
	}
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
void DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::dontQueueEvents(ScanCode code, Action action)
{
    using ButtonEvent = typename DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::DevButtonEventType;
	PRE(code < DevKey::MAX_CODE);

	switch (action)
	{
        case ButtonEvent::RELEASE:
            releaseFilter_[code] = false;
        break;
        case ButtonEvent::PRESS:
            pressFilter_[code] = false;
        break;
        case ButtonEvent::SCROLL_UP:
            // only set for middle mouse
            scrollUpFilter_ = (code == DevKey::MIDDLE_MOUSE) ? false : scrollUpFilter_;
        break;
        case ButtonEvent::SCROLL_DOWN:
            // only set for middle mouse
            scrollDownFilter_ = (code == DevKey::MIDDLE_MOUSE) ? false : scrollDownFilter_;
        break;
        default:
            ASSERT_BAD_CASE;
        break;
	}
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
size_t DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::length() const
{
    size_t result;

    if( recorderDependency_.get().state() == RecRecorder::PLAYING )
    {
        result = recorderPrivDependency_.get().playbackEventQueueLength();
    }
    else
    {
  		result = list_->size();

        if( recorderDependency_.get().state() == RecRecorder::RECORDING )
        {
            recorderPrivDependency_.get().recordEventQueueLength( result );
        }
    }

    return result;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
bool DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::isEmpty() const
{
	return length() == 0;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep>
void DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::CLASS_INVARIANT
{
	INVARIANT(list_);

    // Ayy Lmao
    using ButtonEvent = typename DevEventQueueT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep>::DevButtonEventType;
    using ButtonEvent_Iterator = typename ctl_list<ButtonEvent>::const_iterator;

	// This will fail for default objects!
    ButtonEvent_Iterator it = list_->begin();
	while (it != list_->end())
	{
        const ButtonEvent& ev = *it;
		INVARIANT(ev.scanCode() < DevKey::MAX_CODE);
		++it;
	}
}


//Instantiate the template identified by DevEventQueue alias
template class DevEventQueueT<RecRecorder,RecRecorderPrivate,DevTime>;
/* End EVENTQ.CPP ***************************************************/

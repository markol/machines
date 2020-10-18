/*
 * E V D I A R Y . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"
//#include "stdlib/algorith.hpp"
#include <algorithm>
#include "sim/evdiary.hpp"

#ifndef _INLINE
    #include "sim/evdiary.ipp"
#endif

/////////////////////////////////////////////////////////////////////////////////////////

SimEventDiary::SimEventDiary( void )
: eventsSorted_( true )
{
    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

SimEventDiary::~SimEventDiary()
{
    TEST_INVARIANT;

    //Extract every event from the list, and disassociate it
    while (not events_.empty())
    {
        //Remove last element from queue
        SimDiscreteEventPtr event = events_.back();
        events_.pop_back();

        //Disassociate the event from the diary
        event->diary( NULL );
    }
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimEventDiary::add (const SimDiscreteEventPtr& event)
{
    //Add the event to the back of the queue. This makes a copy of the ref-ptr
    events_.push_back( event );

    //Store the diary pointer in the event
    event->diary( this );

    //The diary queue is no longer sorted
    eventsSorted_ = false;

    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimEventDiary::remove (const SimDiscreteEventPtr& event)
{
    //The event must be in this diary
    PRE( event->diary() == this );

    //Iterate along the queue until we find a match
    const SimDiscreteEvent* pEvent = &(*event);

    SimEventQueue::iterator i;
    for (i = events_.begin(); i != events_.end(); ++i)
    {
        const SimDiscreteEvent& iEvent = *(*i);
        if (pEvent == &iEvent)
        {
            //Disassociate the event
            (*i)->diary( NULL );

            //Remove from the queue
            events_.erase( i );

            //No need to continue looking
            break;
        }
    }

    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

SimDiscreteEventPtr SimEventDiary::takeNextEvent()
{
    //Check the diary isn't empty
    PRE( not events_.empty() );

    //Ensure the order is correct
    validateOrder();

    //Remove an event from front of queue
    SimDiscreteEventPtr eventPtr = events_.front();
    events_.pop_front();

    //Disassociate from the diary
    eventPtr->diary( NULL );

    return eventPtr;
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimEventDiary::validateOrder() const
{
    //No action needed if events are already in order
    if (not eventsSorted_)
    {

        //We need a non-const pointer to this item
        SimEventDiary* pNCT = (SimEventDiary*)this;

        //Sort them into order
        std::sort( pNCT->events_.begin(), pNCT->events_.end(), EventOrder() );

        //Update flag
        eventsSorted_ = true;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimEventDiary::CLASS_INVARIANT
{
	INVARIANT( this );
}
/////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const SimEventDiary& t )
{

    o << "SimEventDiary " << (void*)&t << " start" << std::endl;
    o << "SimEventDiary " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const SimEventDiary& diary )
{
//	PER_WRITE_RAW_OBJECT( ostr, diary.eventsSorted_ );
	ostr << diary.events_;
}

void perRead( PerIstream& istr, SimEventDiary& diary )
{
//	bool temp;
//	PER_READ_RAW_OBJECT( istr, temp );
//	diary.eventsSorted_ = temp;
	istr >> diary.events_;
}


/////////////////////////////////////////////////////////////////////////////////////////

/* End EVDIARY.CPP **************************************************/

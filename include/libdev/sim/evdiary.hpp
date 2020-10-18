/*
 * E V D I A R Y . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    SimEventDiary

    Maintain a collection of pending SimDiscreteEvents, sorted conveniently to
    facilitate access in time order.
*/

#ifndef _EVDIARY_HPP
#define _EVDIARY_HPP

//#include <deque.hpp>
#include <functional>
#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/deque.hpp"
#include "sim/disevent.hpp"

//concrete, orthodox canonical (revoked)
class SimEventDiary
{
public:
    //Constructor initialises with empty event list
    SimEventDiary( void );

    //Disassociates all members of the event list from the diary
    ~SimEventDiary( void );

    // Adds event to the diary
    void add (const SimDiscreteEventPtr& event);

    // Removes event from the diary
    void remove (const SimDiscreteEventPtr& event);

    // Returns the earliest scheduled time of all events in the diary
    PhysAbsoluteTime nextEventTime() const;
    //PRE( not isEmpty() )

    //true iff the diary contains no events
    bool isEmpty() const;

    // Removes the next event from the diary, returning a reference to it
    SimDiscreteEventPtr takeNextEvent();

    //  Clear all events out of the diary without executing them
    void clear( void );
    // POST( isEmpty() );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SimEventDiary& t );

//	PER_MEMBER_PERSISTANT_DEFAULT( SimEventDiary );
	PER_MEMBER_PERSISTENT_DEFAULT( SimEventDiary );
	PER_FRIEND_READ_WRITE( SimEventDiary );

private:
    friend class SimDiscreteEvent;

    // Operation deliberately revoked
    SimEventDiary( const SimEventDiary& );

    // Operation deliberately revoked
    SimEventDiary& operator =( const SimEventDiary& );

    // Operation deliberately revoked
    bool operator ==( const SimEventDiary& );

    //Ensure the diary events are currently sorted into order
    void validateOrder() const;

    //The time of a stored event has changed.
    void eventTimeChanged();

    //Defines a comparator function for the SimDiscreteEventPtrs in the deque
    class EventOrder
    : public std::binary_function< const SimDiscreteEventPtr&, const SimDiscreteEventPtr&, bool >
    {
    public:
	    bool operator ()( const SimDiscreteEventPtr&, const SimDiscreteEventPtr& ) const;
    };

    //data members
    typedef ctl_deque<SimDiscreteEventPtr> SimEventQueue;
//    typedef deque< CtlCountedPtr<SimDiscreteEvent> > SimEventQueue;
//    typedef deque< SimDiscreteEvent* > SimEventQueue;
    SimEventQueue events_; //Sorted into ascending time order if
                                            //eventsSorted is true
    mutable bool eventsSorted_ :1; //true if the events_ vector is sorted
};

PER_DECLARE_PERSISTENT( SimEventDiary );

#ifdef _INLINE
    #include "sim/evdiary.ipp"
#endif


#endif

/* End EVDIARY.HPP **************************************************/

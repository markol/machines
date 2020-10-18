/*
 * D I S E V E N T . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    SimDiscreteEvent

    Represents an event which is timed to occur at a specific time in the simulation.
    MUST be constructed on the heap. Use ref-counting class SimDiscreteEventPtr to store
    references to the event. It will be deleted when the last Ptr referencing it is destroyed.

    Responsibilities:
    Execute the event in the virtual execute() method.
*/

#ifndef _DISEVENT_HPP
#define _DISEVENT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "ctl/countptr.hpp"

//Forward declarations
class SimEventDiary;

//Abstract, orthodox canonical (revoked)
class SimDiscreteEvent
{
public:
    //Constructor specifies time at which the event oocurs
    SimDiscreteEvent( const PhysAbsoluteTime& time );

    //Destructor
    virtual ~SimDiscreteEvent( void );
    //PRE( not in a diary )

    /* Executes the action defined by the event. This action must compute very quickly,
       and should therefore not perform any expensive computations. It should not perform
       AI etc, but may take action necessary to induce AI to be performed in a subsequent
       SimActor::update() call, for example. */
    virtual void execute() = 0;

    //The scheduled time for the event
    const PhysAbsoluteTime& time() const;

    /* Set the event time. If the time is changed, and the event is in a diary, the diary
       is notified of the change in time, so that the ordering can be adjusted. */
    void time( const PhysAbsoluteTime& time );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SimDiscreteEvent& t );

    virtual void doOutputOperator( ostream& ) const;

	PER_MEMBER_PERSISTENT_ABSTRACT( SimDiscreteEvent );
	PER_FRIEND_READ_WRITE( SimDiscreteEvent );

private:
    friend class SimEventDiary;
    // Operation deliberately revoked
    SimDiscreteEvent( const SimDiscreteEvent& );

    // Operation deliberately revoked
    SimDiscreteEvent& operator =( const SimDiscreteEvent& );

    // Operation deliberately revoked
    bool operator ==( const SimDiscreteEvent& );

    //Set/get the SimEventDiary of which the event is a member
    void diary( SimEventDiary* pDiary );
    SimEventDiary* diary( void ) const;

    //Data members
    PhysAbsoluteTime time_; //Time at which the event occurs
    SimEventDiary* pDiary_; //Any diary in which currently entered
};

PER_DECLARE_PERSISTENT( SimDiscreteEvent );

//Implements a ref-counted pointer to an event
typedef CtlCountedPtr<SimDiscreteEvent> SimDiscreteEventPtr;

#ifdef _INLINE
    #include "sim/disevent.ipp"
#endif


#endif

/* End DISEVENT.HPP *************************************************/

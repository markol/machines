/*
 * A C T O R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    SimActor

    Abstract class representing an object which performs activities in the simulation.

    Responsibilities:
    Register itself with owning SimProcess on construction.
    De-register on destruction.

    Sub-class Responsibilities:
    Override the update() method to implement the activity.
    Override the renDomain() method.
*/

#ifndef _ACTOR_HPP
#define _ACTOR_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "world4d/subject.hpp"
#include "world4d/observer.hpp"
#include "sim/sim.hpp"


//Forward declarations
class RenDomain;

class SimActor : public W4dSubject, public W4dObserver
{
public:
    // Constructor registers with pProcess.
    // The physical rep for the actor is pPhysObject.
    SimActor (SimProcess* pProcess, W4dEntity* pPhysObject);
    //PRE( pProcess != NULL);

    enum Update { UPDATE_EVERY_CYCLE };

    // Constructor registers with pProcess.
    // The physical rep for the actor is pPhysObject.
    // This actor will be updated every frame
    SimActor (SimProcess* pProcess, W4dEntity* pPhysObject, Update);

    //The owning SimProcess
    const SimProcess& process( void ) const;

    //Set the priority. When CPU time is in short supply, an actor with
    //priority 10 will get approx 10 times the CPU time as an actor with priority 1 owned
    //by the same process.
    //This balancing is achieved by the SimManager.
    void priority( const SimPriority& );
    const SimPriority& priority() const;

    //Sub-class must override to update the current state of the actor.
    //This method is called regularly by the SimManager, depending on priorities.
    //clearanceFromDisplayedVolume represents a minimum distance from any camera's
    //view frustrum. If not displayed, the override may model behaviour with less accuracy.
    //Return the time interval to next desired update call. So if actor knows that no
    //work is required for the next 5 seconds, return 5. Alternatively, return 0 if, say
    //in the middle of AI computation and want to be called again as soon as possible.
    //This function must not use more than maxCPUTime milliseconds in this call.
    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearanceFromDisplayedVolume ) = 0;

    //Must be overriden to return the RenDomain in which the actor currently resides
//    virtual const RenDomain& renDomain( void ) const = 0;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SimActor& t );

    // Get the isDead flag
    bool isDead( void ) const;

	//Actors may assign themselves to different processes
	//in public interface as persistence mechanism may have to do this as well.
	void assignToDifferentProcess( SimProcess* );

    PER_MEMBER_PERSISTENT_ABSTRACT( SimActor );
    PER_FRIEND_READ_WRITE( SimActor );

protected:

    // Set/get the time at which the actor has requested it should be next updated
    void nextUpdateTime( const PhysAbsoluteTime& );
    const PhysAbsoluteTime& nextUpdateTime( void ) const;

    //Default implementation does nothing
	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

    virtual void domainDeleted( W4dDomain* pDomain );

    // Set the isDead flag
    void isDead( bool die );

    //Destructor unregisters with owning SimProcess.
    virtual ~SimActor( void );

private:
    friend class SimManager;
    friend class SimProcess;
    friend class SimProjectile;

    // Operation deliberately revoked
    SimActor( const SimActor& );

    // Operation deliberately revoked
    SimActor& operator =( const SimActor& );

    // Operation deliberately revoked
    bool operator ==( const SimActor& );

    //Prevent derived classes from deleting actors
   // static void operator delete( void* p )
    //    { ::delete( p ); };

    bool    updateEveryCycle() const;

    //Sets the isDead flag without adding to the dead actor list.
    //Used during destruction of a SimProcess.
    void markAsDead();

    //Data members
    SimProcess* pProcess_; // The owning process
    SimPriority priority_; //Actor's priority within its process
    PhysAbsoluteTime nextUpdateTime_; // Time at which next update was requested
    bool isDead_; //true when flagged as dead
    bool updateEveryCycle_;
};

PER_DECLARE_PERSISTENT( SimActor );

#ifdef _INLINE
    #include "sim/actor.ipp"
#endif


#endif

/* End ACTOR.HPP ****************************************************/

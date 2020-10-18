/*
 * P R O C E S S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    SimProcess

    Abstract class representing a single process which is active in the simulation.
    For example, a HumanPlayer class, and a ComputerPlayer class may be derived.

    Responsibilities:
    Maintain a collection of SimActors.

    Responsibilities of sub-classes:
    Model any activities not represented by its SimActors.
    Set the priority of each of its SimActors.

    Not responsibilities:
    Calling the update() method of its SimActors (SimManager does this).
*/

#ifndef _PROCESS_HPP
#define _PROCESS_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "sim/sim.hpp"
//#include "sim/actor.hpp"

class SimActor;
class SimProcessImpl;

class SimProcess
{
public:
	enum ToBeUpdated { MANAGER_UPDATE, MANAGER_NOT_UPDATE };
    // Constructor registers this Simprocess with the SimManager
    SimProcess( void );

    // Destructor unregisters with the SimManager, and deletes all its actors
    virtual ~SimProcess( void );

    //Set the priority. When CPU time is in short supply, a process (including its actors) with
    //priority 10 will get approx 10 times the CPU time as a process with priority 1.
    //This balancing is achieved by the SimManager.
    void priority( const SimPriority& );
    const SimPriority& priority() const;

    //Override to perform any non-actor updating. Do NOT call the SimActor::update() methods -
    //this is handled by the SimManager.
    //Processing time must be limited to maxCPUTime.
    //Default implementation does nothing.
    virtual void update( const PhysRelativeTime& maxCPUTime );

    //Exports collection of SimActors
    const SimActors& actors() const;

	ToBeUpdated toBeUpdated() const;
	void toBeUpdated( ToBeUpdated );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SimProcess& t );
    friend ostream& operator <<( ostream& o, const SimProcess::ToBeUpdated& t );

protected:
    // Register pActor to be owned by this process
    void add( SimActor* pActor );

private:
    friend class SimManager;
    friend class SimActor;

    // Operation deliberately revoked
    SimProcess( const SimProcess& );

    // Operation deliberately revoked
    SimProcess& operator =( const SimProcess& );

    // Operation deliberately revoked
    bool operator ==( const SimProcess& );

    // Unregister pActor from this process
    void remove( SimActor* pActor );

    // Update as many actors as possible in the time allowed.
    void updateActors( const PhysRelativeTime& maxCPUTime );

    PhysRelativeTime timeUsedForLastUpdate() const;

    //Data members
	SimProcessImpl* pImpl_;
};

#endif

/* End PROCESS.HPP **************************************************/

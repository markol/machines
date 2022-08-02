/*
 * M A N A G E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    SimManager

    Responsibilities:
    Overall control of the simulation process.
    Maintains a linear relationship between real time and simulation time.
    Can be suspended, for example while option menu in use.
    Maintains a collection of SimProcesses and RenCameras.
    Allocates CPU time for each SimProcess in specified ratios of the time
    available to all SimProcesses.
    Maintains as high a render frame rate as possible, whilst balancing the
    needs of simulation activities.
    Maintains an efficient diary of scheduled discrete events.

*/

#ifndef _SIMMANAGER_HPP
#define _SIMMANAGER_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"

#include "stdlib/string.hpp"

class DevTimer;
class SimActor;
class SimConditionsManager;
class SimAction;
class SimManagerImpl;
class SimProcess;

class SimDiscreteEvent;
template < class X > class CtlCountedPtr;
typedef CtlCountedPtr<SimDiscreteEvent> SimDiscreteEventPtr;

class SimManager
{
public:
    //  Singleton class
    static SimManager& instance();
    ~SimManager();

    // Current simulation time
    const PhysAbsoluteTime& currentTime() const;

	// Set time zero to be now.  Should be called when all other
	// initialisation has been done and the game is about to start.
	void resetTime();

	//sometime in multiplayer the times can go out of sync so we give the client
	//code the ability to reset it every so often.
	void setSimulationTime( const PhysAbsoluteTime& );

    // Perform a cycle of updating the simulation and refreshing the display
    void cycle();

    // Suspend simulation activities, simulation time stops advancing
    void suspend();

    // Resumes simulation activities and restarts time advancing
    void resume();

	bool isSuspended();

    // Adds eventPtr to the diary
    void add (SimDiscreteEventPtr eventPtr);

    // Removes the discrete event referenced by eventPtr from the diary
    void remove (SimDiscreteEventPtr eventPtr);

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SimManager& t );

	void deleteAllProcesses();

    //  Clear all events out of the diary without executing them
    void clearDiary();

	void addAction( SimAction* );
	void enableAction( const string& );

	SimConditionsManager& conditionsManager() const;

private:
    friend class SimProcess; // To give access to the add/remove( SimProcess* ) methods.
    friend class SimActor;

    // Operation deliberately revoked
    SimManager( const SimManager& );

    // Operation deliberately revoked
    SimManager& operator =( const SimManager& );

    // Operation deliberately revoked
    bool operator ==( const SimManager& );

    // We only want a single instance
    SimManager();

    // Update the stored current time
    void updateCurrentTime();

    // Execute all the discrete events due by the current time
    void executeDiscreteEvents();

    // Add pProcess to collection
    void add( SimProcess* pProcess );

    // Remove pProcess from collection
    void remove( SimProcess* pProcess );

    //  Add an actor that will be updated every cycle without fail
    void addUpdateEveryCycle( SimActor* pActor );

    //  Remove an actor that was be updated every cycle without fail
    void removeUpdateEveryCycle( SimActor* pActor );

    //  Update all of the processes we know about
    void updateProcesses( PhysRelativeTime maxCPUTime );

    //  Pass to sort to get processes into order of increasing time taken.
    static bool sortProcessByTime(
      const SimProcess* pLeftProcess,
      const SimProcess* pRightProcess );

	//if a sim process changes its priority then we need to change the ratios that we work with.
	void recomputePriorities();

    //Add pActor to the list of actors to be deleted
    void addDeadActor( SimActor* pActor );

    //Delete all the dead actors
    void deleteDeadActors();

	SimManagerImpl* pImpl_;
};


#endif

/* End MANAGER.HPP **************************************************/

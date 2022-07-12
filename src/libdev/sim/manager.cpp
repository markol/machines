/*
 * M A N A G E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"

#include "world4d/manager.hpp"
#include "world4d/subject.hpp"

#include "phys/phys.hpp"

#include "sim/actor.hpp"
#include "sim/manager.hpp"
#include "sim/process.hpp"
#include "sim/simstats.hpp"
#include "sim/conditim.hpp"

#include "sim/internal/manageri.hpp"

#include "ctl/algorith.hpp"

#include "network/netnet.hpp"

/////////////////////////////////////////////////////////////////////////////////////////

// static
SimManager& SimManager::instance()
{
    static SimManager instance_;
    return instance_;
}
////////////////////////////////////////////////////////////////////////////////////////

SimManager::SimManager()
{
	pImpl_ = _NEW(SimManagerImpl());
    CB_DEPIMPL(PhysAbsoluteTime, currentTime_);
    CB_DEPIMPL(PhysAbsoluteTime, devStartTime_);
	CB_DEPIMPL(SimConditionsManager*, pConditionsManager_);
    CB_DEPIMPL(SimManagerImpl::SimProcesses, processes_);

	pConditionsManager_ = _NEW( SimConditionsManager() );

    // Initialise the current time. The device library clock may not start at zero
    devStartTime_ = 0;
    updateCurrentTime();
    devStartTime_ = currentTime_;
    currentTime_ = 0;

    processes_.reserve( 16 );

    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

SimManager::~SimManager()
{
    CB_DEPIMPL(DevTimer, timer_);
	CB_DEPIMPL(bool,processesAllDeleted_);
	CB_DEPIMPL(SimConditionsManager*, pConditionsManager_);

    TEST_INVARIANT;
	ASSERT( processesAllDeleted_, "SimManager::DTOR is being called but processes still there!" );

    _DELETE( &timer_ );

	_DELETE( pConditionsManager_ );

	_DELETE(pImpl_);
}

void SimManager::deleteAllProcesses()
{
    CB_DEPIMPL(SimManagerImpl::SimProcesses, processes_);
	CB_DEPIMPL(bool,processesAllDeleted_);
	CB_DEPIMPL(SimConditionsManager*, pConditionsManager_);
    CB_DEPIMPL(SimActors, deadActors_);

    //Clear out the dead actor list
    deadActors_.erase( deadActors_.begin(), deadActors_.end() );

    //Remove all the processes
    while( processes_.size() > 0 )
	{
		//process calls back into manager to get removed from list of processes
        _DELETE( *processes_.begin() );
	}

	processesAllDeleted_ = true;
	pConditionsManager_->unloadGame();

}
/////////////////////////////////////////////////////////////////////////////////////////

void SimManager::cycle()
{
    CB_DEPIMPL(bool, suspended_);
    CB_DEPIMPL(PhysAbsoluteTime, currentTime_);
    CB_DEPIMPL(PhysAbsoluteTime, lastRenderStartTime_);
    CB_DEPIMPL(SimActors, updateEveryCycleActors_);
    CB_DEPIMPL(SimManagerImpl::SimProcesses, processes_);
	CB_DEPIMPL(SimConditionsManager*, pConditionsManager_);

    //get the RenLib manager
    W4dManager& renManager = W4dManager::instance();

    //Don't do any simulation if the process is suspended
    if( not suspended_ )
    {
        //Hardcode the minimum computation time at 10ms, and the target time between
        //render passes at 30ms
        const PhysRelativeTime minComputationTime = SimStats::instance().minComputationTime();
        const PhysRelativeTime targetRenderInterval = SimStats::instance().targetRenderInterval();

        const PhysAbsoluteTime startTime = Phys::time();

        //Work out the time up to which we can do updating of processes.
        //Try to keep to the frame rate schedule, but ensure we get our minimum
        //computation time.
        updateCurrentTime();
        renManager.time( currentTime_ );
        PhysAbsoluteTime targetRenderTime = lastRenderStartTime_ + targetRenderInterval;
        PhysAbsoluteTime nextRenderTime = currentTime_ + minComputationTime;
        if( targetRenderTime > nextRenderTime ) nextRenderTime = targetRenderTime;

        //Update the per-frame actors
        for( size_t index = 0; index < updateEveryCycleActors_.size(); )
        {
            SimActor* pActor = updateEveryCycleActors_[ index ];

            //Garbage collect dead actors

            if( not pActor->isDead() )
            {
                pActor->update( nextRenderTime - currentTime_, 0 );
                ++index;
            }
        }

        //Ensure all dead actors are deleted
        deleteDeadActors();

        // Execute outstanding discrete events
        executeDiscreteEvents();

        const PhysRelativeTime timeSoFar = Phys::time() - startTime;

        updateProcesses( minComputationTime - timeSoFar );

		pConditionsManager_->update();
//        updateCurrentTime();
//        renManager.time( currentTime_ );
    }

    //Perform a render pass
    lastRenderStartTime_ = currentTime_;
    //renManager.render();
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimManager::suspend()
{
    CB_DEPIMPL(bool, suspended_);
    CB_DEPIMPL(DevTimer, timer_);
    //Stop the clock and update the current time
    timer_.pause();
    updateCurrentTime();

    // Set flag
    suspended_ = true;

    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimManager::resume()
{
    CB_DEPIMPL(bool, suspended_);
    CB_DEPIMPL(DevTimer, timer_);
    //Restart the clock and update the current time
    timer_.resume();
    updateCurrentTime();

    // Reset flag
    suspended_ = false;

    TEST_INVARIANT;
}

bool SimManager::isSuspended()
{
    CB_DEPIMPL(bool, suspended_);
	return suspended_;
}

/////////////////////////////////////////////////////////////////////////////////////////

void SimManager::updateCurrentTime()
{
    CB_DEPIMPL(PhysAbsoluteTime, currentTime_);
    CB_DEPIMPL(PhysAbsoluteTime, devStartTime_);
    CB_DEPIMPL(DevTimer, timer_);

    currentTime_ = _STATIC_CAST( PhysAbsoluteTime, timer_.time() ) - devStartTime_;

    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimManager::executeDiscreteEvents()
{
    CB_DEPIMPL(PhysAbsoluteTime, currentTime_);
    CB_DEPIMPL(SimEventDiary, diary_);

    // Find all the events due by now
    while( not diary_.isEmpty()  and  diary_.nextEventTime() <= currentTime_ )
    {
        //Take the event from the diary and execute it
        SimDiscreteEventPtr eventPtr = diary_.takeNextEvent();
        eventPtr->execute();
    }

    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimManager::add( SimProcess* pProcess )
{
    CB_DEPIMPL(SimManagerImpl::SimProcesses, processes_);
	CB_DEPIMPL(bool,processesAllDeleted_);

    PRE( pProcess != NULL );

    // Add to the end of the list
    processes_.push_back( pProcess );
	recomputePriorities();
    processesAllDeleted_ = false;

    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimManager::remove( SimProcess* pProcess )
{
    CB_DEPIMPL(SimManagerImpl::SimProcesses, processes_);
    CB_DEPIMPL(size_t, nextUpdateProcess_);

    PRE( pProcess != NULL );

    // Find its position in the list
    PRE_DATA( bool foundIt = false; )
    size_t index = 0;
    for( SimManagerImpl::SimProcesses::iterator i = processes_.begin(); i != processes_.end() ; ++i )
    {
        if( *i == pProcess )
        {
            // Remove the entry from the collection
            processes_.erase( i );
            POST_DATA( foundIt = true; )
            break;
        }

        ++index;
    }

    // Check we found it
    POST( foundIt );

    //Adjust the index of the next process to update if required
    if( nextUpdateProcess_ > index ) --nextUpdateProcess_;
    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////

void SimManager::addUpdateEveryCycle( SimActor* pActor )
{
    CB_DEPIMPL(SimActors, updateEveryCycleActors_);

    updateEveryCycleActors_.push_back( pActor );
}

/////////////////////////////////////////////////////////////////////////////////////////

void SimManager::removeUpdateEveryCycle( SimActor* pActor )
{
    CB_DEPIMPL(SimActors, updateEveryCycleActors_);

    ASSERT( updateEveryCycleActors_.size() != 0, "" );

    SimActors::iterator i = updateEveryCycleActors_.begin();

    while( pActor != *i )
    {
        ++i;
        ASSERT( i != updateEveryCycleActors_.end(), "Actor not present in SimManager update every cycle list" );
    }

    updateEveryCycleActors_.erase( i );
}

/////////////////////////////////////////////////////////////////////////////////////////

void SimManager::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
/////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const SimManager& t )
{

    o << "SimManager " << (void*)&t << " start" << std::endl;
	o << " Processes " << t.pImpl_->processes_.size() << std::endl;
	if( t.pImpl_->processes_.size() > 0 )
		for( SimManagerImpl::SimProcesses::const_iterator i = t.pImpl_->processes_.begin(); i!= t.pImpl_->processes_.end(); ++i )
			o << *i << std::endl;
    o << "SimManager " << (void*)&t << " end" << std::endl;

    return o;
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimManager::resetTime()
{
    CB_DEPIMPL(PhysAbsoluteTime, currentTime_);
    CB_DEPIMPL(PhysAbsoluteTime, devStartTime_);
    CB_DEPIMPL(DevTimer, timer_);

    devStartTime_ = timer_.time();
	currentTime_ = 0;
}

void SimManager::setSimulationTime( const PhysAbsoluteTime& newTime )
{
    CB_DEPIMPL(PhysAbsoluteTime, currentTime_);
    CB_DEPIMPL(PhysAbsoluteTime, devStartTime_);
    CB_DEPIMPL(DevTimer, timer_);

	currentTime_ = newTime;
    devStartTime_ = _STATIC_CAST( PhysAbsoluteTime, timer_.time() ) - currentTime_;

    W4dManager::instance().time( currentTime_ );
}

void SimManager::recomputePriorities()
{
    CB_DEPIMPL(SimManagerImpl::SimProcesses, processes_);
	CB_DEPIMPL(SimPriority, totalAmountOfPriority_);

	totalAmountOfPriority_ = 0;
	for( SimManagerImpl::SimProcesses::const_iterator i = processes_.begin(); i != processes_.end(); ++i )
	{
		totalAmountOfPriority_ += (*i)->priority();
	}
}

void SimManager::addAction( SimAction* pAction )
{
	CB_DEPIMPL(SimConditionsManager*, pConditionsManager_);

	pConditionsManager_->addAction( pAction );
}

void SimManager::enableAction( const string& keyName )
{
	CB_DEPIMPL(SimConditionsManager*, pConditionsManager_);

	pConditionsManager_->enableAction( keyName );
}


SimConditionsManager& SimManager::conditionsManager() const
{
	CB_DEPIMPL(SimConditionsManager*, pConditionsManager_);

	return *pConditionsManager_;
}

const PhysAbsoluteTime& SimManager::currentTime() const
{
    CB_DEPIMPL(PhysAbsoluteTime, currentTime_);

	return currentTime_;
}

void SimManager::add (SimDiscreteEventPtr eventPtr)
{
    CB_DEPIMPL(SimEventDiary, diary_);

	diary_.add( eventPtr );
}

void SimManager::remove (SimDiscreteEventPtr eventPtr)
{
    CB_DEPIMPL(SimEventDiary, diary_);

	diary_.remove( eventPtr );
}

void SimManager::clearDiary()
{
    CB_DEPIMPL(SimEventDiary, diary_);

    diary_.clear();
}

void SimManager::addDeadActor( SimActor* pActor )
{
    PRE( pActor );

    CB_DEPIMPL(SimActors, deadActors_);
    deadActors_.push_back( pActor );
}

void SimManager::deleteDeadActors()
{
    CB_DEPIMPL(SimActors, deadActors_);

    while( not deadActors_.empty() )
    {
        //Get the next dead actor to delete
        SimActor* pActor = deadActors_.back();
        deadActors_.pop_back();
        ASSERT( pActor->isDead(), "Undead actor in dead list" );

        //Notify all observers
        pActor->notifyObservers( W4dSubject::DELETED );

        //delete it
        _DELETE( pActor );
    }
}

void SimManager::updateProcesses( PhysRelativeTime maxCPUTime )
{
    CB_DEPIMPL( SimManagerImpl::SimProcesses, processes_ );
	CB_DEPIMPL( SimPriority, totalAmountOfPriority_ );

    const PhysAbsoluteTime startTime = Phys::time();

    sort( processes_.begin(), processes_.end(), sortProcessByTime );

    MATHEX_SCALAR priorityLeft = totalAmountOfPriority_;

    for( SimManagerImpl::SimProcesses::iterator i = processes_.begin(); i != processes_.end(); ++i )
    {
        const PhysRelativeTime timeLeft = maxCPUTime - ( Phys::time() - startTime );

        const MATHEX_SCALAR scalarPriority = (*i)->priority();

        const PhysRelativeTime biasMaxCPU = timeLeft * scalarPriority / priorityLeft;

        const PhysRelativeTime timeToUse = std::max( biasMaxCPU, SimStats::instance().minProcessUpdateTime() );

		if( not NetNetwork::instance().imStuffed() )
		    (*i)->updateActors( timeToUse );

        //Ensure all dead actors are deleted
        deleteDeadActors();

        priorityLeft -= (*i)->priority();
    }

    TEST_INVARIANT;


}

// static
bool SimManager::sortProcessByTime( const SimProcess* pLeftProcess, const SimProcess* pRightProcess )
{
    return pLeftProcess->timeUsedForLastUpdate() < pRightProcess->timeUsedForLastUpdate();
}

/////////////////////////////////////////////////////////////////////////////////////////
/* End MANAGER.CPP **************************************************/

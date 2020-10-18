/*
 * P R O C E S S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "sim/process.hpp"

#include "ctl/vector.hpp"
#include "sim/manager.hpp"
#include "sim/sim.hpp"
#include "sim/actor.hpp"

class SimProcessImpl
{
public:
	SimProcessImpl();
	~SimProcessImpl();

    SimActors actors_; // Collection of actors
    SimActors updateEveryCycleActors_; // Collection of actors that must be updated every cycle (this is done by the sim manager)
    SimPriority priority_; // The process priority
    size_t nextUpdateActor_; // Index into actors_ of next actor to update
	SimActor*		pSavedActor_;
	int				incrementActorCount_;
	bool			incrementActor_;
	SimProcess::ToBeUpdated		toBeUpdated_;

    //  By storing the time used by update we can sort the processes into
    //  a more efficient calling order for the next frame.
    PhysRelativeTime    timeUsed_;
};

SimProcessImpl::SimProcessImpl()
:   priority_( 1 ),
    nextUpdateActor_( 0 ),
	incrementActor_( true ),
	incrementActorCount_( 0 ),
	toBeUpdated_( SimProcess::MANAGER_UPDATE ),
    timeUsed_( 0 )
{
}

SimProcessImpl::~SimProcessImpl()
{
}
/////////////////////////////////////////////////////////////////////////////////////////

SimProcess::SimProcess()
{
	pImpl_ = _NEW(SimProcessImpl());

    CB_DEPIMPL(SimActors, actors_);
    CB_DEPIMPL(SimActors, updateEveryCycleActors_);
    CB_DEPIMPL(SimPriority, priority_);
    CB_DEPIMPL(size_t, nextUpdateActor_);
	CB_DEPIMPL(SimActor*, pSavedActor_);
	CB_DEPIMPL(int, incrementActorCount_);
	CB_DEPIMPL(bool, incrementActor_);
	CB_DEPIMPL(ToBeUpdated, toBeUpdated_);

    // Register with the SimManager
    SimManager::instance().add( this );

    //Set the size for the vector
    actors_.reserve( 20 );
    updateEveryCycleActors_.reserve( 20 );
    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

SimProcess::~SimProcess()
{
    CB_DEPIMPL(SimActors, actors_);
    CB_DEPIMPL(SimActors, updateEveryCycleActors_);

    TEST_INVARIANT;

    // Delete all the SimActors
    while( actors_.size() > 0 )
    {
        // Get the first member of the collection
        // Destroy the actor - this will remove it from the collection
        SimActor* pActor = actors_.back();
        pActor->markAsDead();
        pActor->notifyObservers( W4dSubject::DELETED );
        _DELETE( pActor );
    }

    while( updateEveryCycleActors_.size() > 0 )
    {
        // Get the first member of the collection
        // Destroy the actor - this will remove it from the collection
        SimActor* pActor = updateEveryCycleActors_.back();
        pActor->markAsDead();
        pActor->notifyObservers( W4dSubject::DELETED );
        _DELETE( pActor );
    }
    // Unregister with SimManager
    SimManager::instance().remove( this );

	_DELETE(pImpl_);
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimProcess::priority( const SimPriority& priority )
{
    CB_DEPIMPL(SimPriority, priority_);

    priority_ = priority;
	SimManager::instance().recomputePriorities();
    TEST_INVARIANT;
}

/////////////////////////////////////////////////////////////////////////////////////////

void SimProcess::update( const PhysRelativeTime& )
{
    //Default implementation does nothing
}

/////////////////////////////////////////////////////////////////////////////////////////

void SimProcess::remove( SimActor* pActor )
{
    CB_DEPIMPL(SimActors, updateEveryCycleActors_);
    CB_DEPIMPL(size_t, nextUpdateActor_);
    CB_DEPIMPL(SimActors, actors_);
    CB_DEPIMPL(SimActor*, pSavedActor_);
    CB_DEPIMPL(bool, incrementActor_);

    PRE( pActor != NULL );

    //Find the actor's position in the collection
    PRE_DATA( bool foundIt = false; )

	bool isEveryCycleActor = pActor->updateEveryCycle();
    SimActors&  actors = isEveryCycleActor ? updateEveryCycleActors_ : actors_;

    if( isEveryCycleActor )
    {
        SimManager::instance().removeUpdateEveryCycle( pActor );
    }

    size_t index = 0;
    for( SimActors::iterator i = actors.begin(); i != actors.end(); ++i )
    {
        //See if we have a hit
        if( *i == pActor )
        {
            if( not isEveryCycleActor )
            {
                //Tidy up saved actor
                if( pActor == pSavedActor_ )
                {
                    pSavedActor_ = NULL;
                    incrementActor_ = true;
                }

                //Ensure we don't skip the update on any unfortunate actor just after this one
                if( index < nextUpdateActor_ )
                    --nextUpdateActor_;
            }

            //Remove this entry
            actors.erase( i );
            POST_DATA( foundIt = true; )
            break;
        }

        ++index;
    }

    //Check it was in the list
    POST( foundIt );

    //Adjust the index number of the next actor to update if necessary
    if( not isEveryCycleActor and nextUpdateActor_ > index )
    	--nextUpdateActor_;

    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimProcess::updateActors( const PhysRelativeTime& maxCPUTime )
{
    CB_DEPIMPL(size_t, nextUpdateActor_);
	CB_DEPIMPL(SimActor*, pSavedActor_);
	CB_DEPIMPL(int, incrementActorCount_);
	CB_DEPIMPL(bool, incrementActor_);
    CB_DEPIMPL(SimActors, actors_);
	CB_DEPIMPL(ToBeUpdated, toBeUpdated_);
	CB_DEPIMPL(PhysRelativeTime, timeUsed_);

    const PhysAbsoluteTime entryTime = Phys::time();

    //Get rid of any dead actors
    SimManager::instance().deleteDeadActors();

    //Find a SimActor in the collection from the next update index to the end which
    //is ready to be updated.
    const PhysAbsoluteTime currentTime = SimManager::instance().currentTime();
    bool doneAnUpdate = false;

    size_t maxActors = actors_.size();

    bool finished = false;

    while( ( not finished ) and ( actors_.size() != 0 ) )
    {
        nextUpdateActor_ = nextUpdateActor_ % actors_.size();

        SimActor* pActor;

		if( incrementActor_ )
		{
	        pActor = actors_[ nextUpdateActor_++ ];
			incrementActorCount_ = 0;
		}
		else
		{
			pActor = pSavedActor_;
			++incrementActorCount_;
            doneAnUpdate = true;
		}

        if( pActor->nextUpdateTime() <= currentTime )
        {
            //Update this actor
            PhysRelativeTime offsetToNextUpdate = 0.25;
			if( toBeUpdated_ == MANAGER_UPDATE )
	            offsetToNextUpdate = pActor->update( maxCPUTime, 0 );
			bool isDead = pActor->isDead();
			if( offsetToNextUpdate == 0 and not isDead )
			{
				pSavedActor_ = pActor;
				incrementActor_ = false;
				++incrementActorCount_;
				if( incrementActorCount_ > 3 )
				{
					incrementActor_ = true;
					incrementActorCount_ = 0;
				}
			}
			else
			{
				incrementActor_ = true;
				pSavedActor_ = NULL;
			}
            doneAnUpdate = true;

            //Set its next update time
            if( not isDead )
                pActor->nextUpdateTime( currentTime + offsetToNextUpdate );

            //Delete this or any other now dead actors
            SimManager::instance().deleteDeadActors();
        }

        //  Finish if we've run out of time, or if we've processed each actor once
        if( Phys::time() - entryTime > maxCPUTime )
            finished = true;

        if( --maxActors == 0 )
            finished = true;
    }

    //If didn't find an actor to update, update the process
    if( not doneAnUpdate )
    {
        const PhysRelativeTime timeUsedSoFar = Phys::time() - entryTime;

        //Update this process
        update( maxCPUTime - timeUsedSoFar );

        //Next time update first actor
        nextUpdateActor_ = 0;
    }

    timeUsed_ = Phys::time() - entryTime;

    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimProcess::CLASS_INVARIANT
{
	INVARIANT( this );
}

SimProcess::ToBeUpdated SimProcess::toBeUpdated() const
{
	CB_DEPIMPL(ToBeUpdated, toBeUpdated_);

	return toBeUpdated_;
}

void SimProcess::toBeUpdated( SimProcess::ToBeUpdated newToBeUpdated )
{
	CB_DEPIMPL(ToBeUpdated, toBeUpdated_);

	toBeUpdated_ = newToBeUpdated;
}

const SimPriority& SimProcess::priority() const
{
    CB_DEPIMPL(SimPriority, priority_);

	return priority_;
}

const SimActors& SimProcess::actors() const
{
    CB_DEPIMPL(SimActors, actors_);

	return actors_;
}

void SimProcess::add( SimActor* pActor )
{
    CB_DEPIMPL(SimActors, actors_);
    CB_DEPIMPL(SimActors, updateEveryCycleActors_);

	PRE( pActor != NULL );

    if( pActor->updateEveryCycle() )
    {
	    updateEveryCycleActors_.push_back( pActor );
        SimManager::instance().addUpdateEveryCycle( pActor );
    }
    else
    {
    	//Add to end of collection
	    actors_.push_back( pActor );
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const SimProcess& t )
{

    o << "SimProcess " << (void*)&t << " start" << std::endl;
	o << " ToBeUpdated " << t.pImpl_->toBeUpdated_ << std::endl;
	o << " Actors " << t.pImpl_->actors_.size() << std::endl;
	if( t.pImpl_->actors_.size() > 0 )
		for( SimActors::const_iterator i = t.pImpl_->actors_.begin(); i!= t.pImpl_->actors_.end(); ++i )
			o << *(*i) << std::endl;
    o << "SimProcess " << (void*)&t << " end" << std::endl;

    return o;
}

ostream& operator <<( ostream& o, const SimProcess::ToBeUpdated& t )
{
	switch( t )
	{
		case SimProcess::MANAGER_UPDATE:
			o << "MANAGER_UPDATE" << std::endl;
			break;
		case SimProcess::MANAGER_NOT_UPDATE:
			o << "MANAGER_NOT_UPDATE" << std::endl;
			break;
		default:
			o << "Unknown in ToBeUpdated type " << (int)t << std::endl;
	}
	return o;
}

PhysRelativeTime SimProcess::timeUsedForLastUpdate() const
{
    return pImpl_->timeUsed_;
}

/////////////////////////////////////////////////////////////////////////////////////////
/* End PROCESS.CPP **************************************************/

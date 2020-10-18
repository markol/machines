/*
 * A C T O R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"
#include "sim/actor.hpp"
#include "sim/process.hpp"
#include "sim/manager.hpp"

#ifndef _INLINE
    #include "sim/actor.ipp"
#endif

/////////////////////////////////////////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT_ABSTRACT( SimActor )

SimActor::SimActor( SimProcess* pProcess, W4dEntity* pPhysObject )
:   W4dSubject( pPhysObject ),
    pProcess_( pProcess ),
    priority_( 1 ),
    nextUpdateTime_( 0 ),
    isDead_( false ),
    updateEveryCycle_( false )
{
    PRE( pProcess != NULL );

    // Register with the process
    pProcess->add( this );

    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

SimActor::SimActor( SimProcess* pProcess, W4dEntity* pPhysObject, Update )
:   W4dSubject( pPhysObject ),
    pProcess_( pProcess ),
    priority_( 1 ),
    nextUpdateTime_( 0 ),
    isDead_( false ),
    updateEveryCycle_( true )
{
    PRE( pProcess != NULL );

    // Register with the process
    pProcess->add( this );

    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

SimActor::~SimActor()
{
    TEST_INVARIANT;
    //Unregister with the owning process
	HAL_STREAM("SimActor::DTOR calling remove\n" );
    if( pProcess_ )
    	pProcess_->remove( this );
	HAL_STREAM(" done" );
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimActor::priority( const SimPriority& priority )
{
    TEST_INVARIANT;

    priority_ = priority;
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimActor::CLASS_INVARIANT
{
	INVARIANT( this );
}
/////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const SimActor& t )
{

    o << "SimActor " << (void*)&t << " start" << std::endl;
    o << "SimActor " << (void*)&t << " end" << std::endl;

    return o;
}
/////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool SimActor::beNotified( W4dSubject* , W4dSubject::NotificationEvent , int )
{
    return false;
}

//virtual
void SimActor::domainDeleted( W4dDomain* )
{
}

void SimActor::assignToDifferentProcess( SimProcess* pNewProcess )
{
	HAL_STREAM("Assign to different process\n" );
	//pProcess MAY be NULL at this point as persistence mechanism may call into this
	//to get the actors registered again
	if( pProcess_ )
	    pProcess_->remove( this );
	pProcess_ = pNewProcess;
	pProcess_->add( this );
}

void perWrite( PerOstream& ostr, const SimActor& actor )
{
	const W4dSubject& base1 = actor;
	const W4dObserver& base2 = actor;

	ostr << base1;
	ostr << base2;

    ostr << actor.priority_;
    ostr << actor.nextUpdateTime_;
	ostr << actor.isDead_;
    ostr << actor.updateEveryCycle_;
}

void perRead( PerIstream& istr, SimActor& actor )
{
	W4dSubject& base1 = actor;
	W4dObserver& base2 = actor;

	istr >> base1;
	istr >> base2;

    istr >> actor.priority_;
    istr >> actor.nextUpdateTime_;
	istr >> actor.isDead_;
    istr >> actor.updateEveryCycle_;
	actor.pProcess_ = NULL;
}

SimActor::SimActor( PerConstructor con )
:	W4dSubject( con )
{
}

void SimActor::isDead( bool die )
{
    //The manager will delete this actor very soon
    if( die )
    {
        if( not isDead_ )
            SimManager::instance().addDeadActor( this );
    }
    else
    {
        ASSERT( not isDead_, "Attempt to resurrect dead actor" );
    }

	isDead_ = die;
}

void SimActor::markAsDead()
{
	isDead_ = true;
}

/* End ACTOR.CPP ****************************************************/

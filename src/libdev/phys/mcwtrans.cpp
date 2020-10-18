/*
 * M C W T R A N S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/mcwtrans.hpp"
#include "device/keytrans.hpp"
#include "mathex/vec2.hpp"
#include "mathex/point3d.hpp"
#include "mathex/eulerang.hpp"
#include "profiler/stktrace.hpp"

PhysMotionControlWithTrans::PhysMotionControlWithTrans
(
	PhysMotionControlled* pMotionControlled,
	const MexVec2& forwards
):
	PhysMotionControl( pMotionControlled ),
	pKeyTranslator_( NULL ),
	pMotionConstraint_( _NEW( PhysUnconstrainedMotion() ) ),
	motion_(forwards)
{
	PRE( pMotionControlled );

    commandList_.reserve( NUM_COMMANDS );

	for ( size_t i = 0; i < NUM_COMMANDS; ++i )
	{
		commandList_.push_back( DevKeyToCommandTranslator::Command() );
	}

    TEST_INVARIANT;
}

PhysMotionControlWithTrans::PhysMotionControlWithTrans
(
	PhysMotionControlled* pMotionControlled,
	PhysMotionConstraint* pMotionConstraint,
	const MexVec2& forwards
):
	PhysMotionControl( pMotionControlled ),
	pKeyTranslator_( NULL ),
	pMotionConstraint_( pMotionConstraint ),
	motion_(forwards)
{
	PRE( pMotionControlled );
	PRE( pMotionConstraint );

    commandList_.reserve( NUM_COMMANDS );

	for ( size_t i = 0; i < NUM_COMMANDS; ++i )
	{
		commandList_.push_back( DevKeyToCommandTranslator::Command() );
	}

    TEST_INVARIANT;
}

PhysMotionControlWithTrans::~PhysMotionControlWithTrans()
{
    TEST_INVARIANT;

	_DELETE( pMotionConstraint_ );
	_DELETE( pKeyTranslator_ );
}

void PhysMotionControlWithTrans::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// virtual
void PhysMotionControlWithTrans::disableInput()
{
	PhysMotionControl::disableInput();

	pKeyTranslator_->resetCommands( &commandList_, true );
}

void PhysMotionControlWithTrans::resetCommands()
{
	pKeyTranslator_->resetCommands( &commandList_ );
}

void PhysMotionControlWithTrans::initEventQueue()
{
	if ( pKeyTranslator_ )
		pKeyTranslator_->initEventQueue();
}


void PhysMotionControlWithTrans::forceCommandOn( Command CommandId )
{
	PRE( CommandId < commandList_.size() );

	commandList_[CommandId].forceOn();
}


// virtual
bool PhysMotionControlWithTrans::doProcessButtonEvent( const DevButtonEvent& buttonEvent )
{
	bool processed = false;

	if ( pKeyTranslator_ )
	{
		processed = pKeyTranslator_->translate( buttonEvent, &commandList_ );
	}

	return processed;
}

void PhysMotionControlWithTrans::setKeyTranslator( DevKeyToCommandTranslator* pTranslator )
{
	PRE( pTranslator );

	_DELETE( pKeyTranslator_ );
	pKeyTranslator_ = pTranslator;
}

void PhysMotionControlWithTrans::imposeConstraint( PhysMotionConstraint* pMotionConstraint )
{
	PRE( pMotionConstraint );

	_DELETE( pMotionConstraint_ );

	pMotionConstraint_ = pMotionConstraint;
}

// virtual
void PhysMotionControlWithTrans::snapTo( const MexPoint3d& location )
{
	PRE( pMotionConstraint_ );

	MexPoint3d newLocation( location );

	if ( pMotionConstraint_->snapTo( &newLocation ) )
	{
		// Snap to new location but retain old orientation
		MexTransform3d oldTrans = pMotionControlled_->globalTransform();
		MexEulerAngles oldAngles;
		oldTrans.rotation( &oldAngles );
		pMotionControlled_->globalTransform( MexTransform3d( oldAngles, newLocation ) );
	}

    //Stop all motion
    motion_.climb( 0.0 );
    motion_.speed( 0.0 );
	motion_.drift( 0.0 );
	motion_.heading( 0.0 );
	motion_.pitch( 0.0 );
	motion_.roll( 0.0 );

	// Begin timing the next frame.
	frameTimer_.time(0);
}

// virtual
void PhysMotionControlWithTrans::snapTo( const MexTransform3d& trans )
{
	PRE( pMotionConstraint_ );

	MexTransform3d newTrans( trans );

	if ( pMotionConstraint_->snapTo( &newTrans ) )
	{
		pMotionControlled_->globalTransform( newTrans );
	}

    //Stop all motion
    motion_.climb( 0.0 );
    motion_.speed( 0.0 );
	motion_.drift( 0.0 );
	motion_.heading( 0.0 );
	motion_.pitch( 0.0 );
	motion_.roll( 0.0 );

	// Begin timing the next frame.
	frameTimer_.time(0);
}

ostream& operator <<( ostream& o, const PhysMotionControlWithTrans& t )
{

    o << "PhysMotionControlWithTrans " << (void*)&t << " start" << std::endl;
    o << "PhysMotionControlWithTrans " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MCWTRANS.CPP *************************************************/

/*
 * S P I D E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"

#include "ctl/countptr.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "phys/timeangl.hpp"
#include "phys/linetrav.hpp"
#include "phys/rampacce.hpp"

#include "world4d/composit.hpp"
#include "world4d/compplan.hpp"
#include "sim/manager.hpp"

#include "machphys/spider.hpp"
#include "machphys/internal/spideri.hpp"
#include "machphys/machine.hpp"
#include "machphys/mcmovinf.hpp"

#include "mathex/utility.hpp"

//////////////////////////////////////////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT( MachPhysSpiderLegs );

MachPhysSpiderLegs::MachPhysSpiderLegs( MachPhysMachine* pMachine,
                                        const W4dCompositePlanPtr& restingPlanPtr,
                                        const W4dCompositePlanPtr& walkingPlanPtr,
                                        const W4dCompositePlanPtr& turningLeftPlanPtr,
                                        const W4dCompositePlanPtr& turningRightPlanPtr,
                                        const W4dCompositePlanPtr& startWalkingPlanPtr,
                                        const W4dCompositePlanPtr& stopWalkingPlanPtr)
: MachPhysLocomotionMethod( pImpl_ = _NEW( MachPhysSpiderLegsImpl( pMachine, this,
        restingPlanPtr,
        walkingPlanPtr,
        turningLeftPlanPtr,
        turningRightPlanPtr,
        startWalkingPlanPtr,
    	stopWalkingPlanPtr ) )	)
{

    TEST_INVARIANT;
}

MachPhysSpiderLegs::MachPhysSpiderLegs( PerConstructor con )
: MachPhysLocomotionMethod( con ),
  pImpl_( NULL )
{
}

//////////////////////////////////////////////////////////////////////////////////////////

MachPhysSpiderLegs::~MachPhysSpiderLegs()
{
    TEST_INVARIANT;

    // pImpl_ will be removed by MachPhysLocomotionMethod destructor
    //_DELETE( pImpl_ );
}

// virtual
MachPhysLocomotionMethod* MachPhysSpiderLegs::clone( MachPhysMachine* pMachine,
  const W4dLinks& )
{
	CB_DEPIMPL( W4dCompositePlanPtr, restingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, walkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningLeftPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningRightPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, startWalkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, stopWalkingPlanPtr_);

    return _NEW( MachPhysSpiderLegs(
      pMachine, restingPlanPtr_, walkingPlanPtr_, turningLeftPlanPtr_, turningRightPlanPtr_,
    								startWalkingPlanPtr_, stopWalkingPlanPtr_) );
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysSpiderLegs::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

// virtual
void MachPhysSpiderLegs::moveAnimations( const PhysLinearTravelPlan& linearTravelPlan,
    							 const TurnAngles& turnAngles, const MachPhysMachineMoveInfo& info )

{
    PHYS_MOTION_STREAM( "Enter MachPhysSpiderLegs::moveAnimations " << (void*)this << std::endl );
    PHYS_MOTION_INDENT( 2 );

	CB_DEPIMPL( W4dCompositePlanPtr, restingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, walkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningLeftPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningRightPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, startWalkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, stopWalkingPlanPtr_);

    PRE( linearTravelPlan.nSegments() == turnAngles.size() );


    const PhysAbsoluteTime moveStartTime = info.startTime();

    //Deal with each segment of the travel plan
//    const PhysMotionPlan::Times& segmentTimes = *linearTravelPlan.segmentTimes();
    const PhysMotionPlan::RampAccelerations& rampAccelerations = *linearTravelPlan.rampAccelerations();

    ASSERT_INFO( rampAccelerations.back().endSpeed() );

    ASSERT( fabs( rampAccelerations.back().endSpeed() ) < MexEpsilon::instance(), "" );

//    PhysMotionPlan::Times::const_iterator iTimes = segmentTimes.begin();
    TurnAngles::const_iterator iAngles = turnAngles.begin();
    PhysMotionPlan::RampAccelerations::const_iterator iAcceleration = rampAccelerations.begin();

    PhysRelativeTime lastSegmentEndTime = 0;

    PhysRelativeTime translationStartTime = 0;

    for( size_t i = 0; i < linearTravelPlan.nSegments(); ++i, ++iAngles, ++iAcceleration )
//    for( ; iAngles != turnAngles.end(); ++iTimes, ++iAngles, ++iAcceleration )
    {
        const MexRadians angle = *iAngles;
        const PhysRampAcceleration& acceleration = *iAcceleration;

        const PhysRelativeTime  segmentStartTime = lastSegmentEndTime;
        const PhysRelativeTime  segmentEndTime = segmentStartTime + acceleration.totalTime();
        const PhysRelativeTime  segmentDuration = acceleration.totalTime();

        if( angle == MexRadians( 0.0 ) )
        {
            //  Collect all consecutive translation segments into one for the
            //  purposes of the animation.

			if( fabs( acceleration.startSpeed() ) < MexEpsilon::instance() )
            {
                translationStartTime = segmentStartTime;
            }

			if( fabs( acceleration.endSpeed() ) < MexEpsilon::instance() )
            {
                PhysRelativeTime duration = segmentEndTime - translationStartTime;

                translationAnimations( moveStartTime + translationStartTime, duration );
            }
        }
        else
        {
            rotationAnimations( angle, moveStartTime + segmentStartTime, segmentDuration );
        }

        lastSegmentEndTime = segmentEndTime;
    }

    PHYS_MOTION_INSPECT( moveStartTime );
    PHYS_MOTION_INSPECT( lastSegmentEndTime );

    //Register a resting plan at the end time, so we don't carry on waggling on the spot
    if( restingPlanPtr_.isDefined() )
        pMachine()->plan( *restingPlanPtr_, moveStartTime + lastSegmentEndTime, 0, MachPhys::MACHINE_LOCOMOTION );

    PHYS_MOTION_INDENT( -2 );
    PHYS_MOTION_STREAM( "Exit MachPhysSpiderLegs::moveAnimations " << (void*)this << std::endl );
}

void MachPhysSpiderLegs::translationAnimations( PhysAbsoluteTime startTime, PhysRelativeTime duration )
{
    PHYS_MOTION_STREAM( "Enter MachPhysSpiderLegs::translationAnimations " << (void*)this << std::endl );
    PHYS_MOTION_INDENT( 2 );

	CB_DEPIMPL( W4dCompositePlanPtr, restingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, walkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningLeftPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningRightPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, startWalkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, stopWalkingPlanPtr_);

    PHYS_MOTION_INSPECT( startTime );
    PHYS_MOTION_INSPECT( duration );

//    PhysRelativeTime walkingStartTime = startTime + lastTime;
    PhysRelativeTime startWalkingTime;
    PhysRelativeTime stopWalkingTime;

    if( startWalkingPlanPtr_.isDefined() )
    	startWalkingTime = startWalkingPlanPtr_->finishTime();
    else
    	startWalkingTime = 0;

    if( stopWalkingPlanPtr_.isDefined() )
    	stopWalkingTime = stopWalkingPlanPtr_->finishTime();
    else
    	stopWalkingTime = 0;

    PHYS_MOTION_INSPECT( startWalkingTime );
    PHYS_MOTION_INSPECT( stopWalkingTime );

    PhysRelativeTime    timeOffset = 0.0;

    if( startWalkingPlanPtr_.isDefined() )
    {
    	pMachine()->plan( *startWalkingPlanPtr_, startTime, 0, MachPhys::MACHINE_LOCOMOTION );
    	timeOffset += startWalkingTime;
    }

    PHYS_MOTION_INSPECT( timeOffset );

    if( walkingPlanPtr_.isDefined() )
    {
    	PhysRelativeTime planDuration = walkingPlanPtr_->finishTime();

    	uint nTimes = (uint)( ( duration - startWalkingTime - stopWalkingTime + 0.5) / planDuration );

        PHYS_MOTION_INSPECT( nTimes );
        PHYS_MOTION_INSPECT( planDuration );
        PHYS_MOTION_INSPECT( nTimes * planDuration );

    	if( nTimes > 0 ) //if( nTimes == 0 ) nTimes = 1;
    	{
    		pMachine()->plan( *walkingPlanPtr_, startTime + timeOffset, nTimes - 1, MachPhys::MACHINE_LOCOMOTION );
    		timeOffset += planDuration * nTimes;
    	}
    }

    PHYS_MOTION_INSPECT( timeOffset );

    if( stopWalkingPlanPtr_.isDefined() )
    {
    	if( duration > timeOffset )
    	{
    		pMachine()->plan( *stopWalkingPlanPtr_, startTime + timeOffset, 0, MachPhys::MACHINE_LOCOMOTION );
    	}
    }

    PHYS_MOTION_INSPECT( timeOffset + stopWalkingTime );
    PHYS_MOTION_INSPECT( startTime + timeOffset + stopWalkingTime );

    PHYS_MOTION_INDENT( -2 );
    PHYS_MOTION_STREAM( "Exit MachPhysSpiderLegs::translationAnimations " << (void*)this << std::endl );
}

void MachPhysSpiderLegs::rotationAnimations( MexRadians angle, PhysAbsoluteTime startTime, PhysRelativeTime duration )
{
    PHYS_MOTION_STREAM( "Enter MachPhysSpiderLegs::rotationAnimations " << (void*)this << std::endl );
    PHYS_MOTION_INDENT( 2 );

    PHYS_MOTION_INSPECT( angle );
    PHYS_MOTION_INSPECT( startTime );
    PHYS_MOTION_INSPECT( duration );

	CB_DEPIMPL( W4dCompositePlanPtr, turningLeftPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningRightPlanPtr_);

    W4dCompositePlanPtr turningPlanPtr;

    if( angle > MexRadians( 0.0 ) )
        turningPlanPtr = turningRightPlanPtr_;
    else
        turningPlanPtr = turningLeftPlanPtr_;

    if( turningPlanPtr.isDefined() )
	{
		PhysRelativeTime planDuration = turningPlanPtr->finishTime();

		uint nTimes = (uint)( ( duration + 0.5) / planDuration );
		if( nTimes == 0 )
            nTimes = 1;

		pMachine()->plan( *turningPlanPtr, startTime, nTimes - 1, MachPhys::MACHINE_LOCOMOTION );
	}

    PHYS_MOTION_INDENT( -2 );
    PHYS_MOTION_STREAM( "Exit MachPhysSpiderLegs::rotationAnimations " << (void*)this << std::endl );
}

//////////////////////////////////////////////////////////////////////////////////////////

// virtual
void MachPhysSpiderLegs::calculateAccelerations(
    const TransformsPtr& transformsPtr,
    MATHEX_SCALAR startSpeed,
    MATHEX_SCALAR capTranslationSpeed,
    MachPhys::FinalState finalState,
    RampAccelerationsPtr rampAccelerationsPtr ) const
{
    PHYS_MOTION_STREAM( "Enter MachPhysSpiderLegs::calculateAccelerations " << (void*)this << std::endl );
    PHYS_MOTION_INDENT( 2 );

    PRE( transformsPtr.isDefined() );
    PRE( rampAccelerationsPtr.isDefined() );
    PRE( transformsPtr->size() >= 2 );
    PRE( rampAccelerationsPtr->size() == 0 );
    PRE( MexUtility::inStraightLineXY( *transformsPtr ) );

    PHYS_MOTION_STREAM( "Transforms:\n" << *transformsPtr );

    //  Get the ramp accelerations we would use if we weren't doing anything special with them.

    doCalculateAccelerations( transformsPtr, startSpeed, capTranslationSpeed, finalState, rampAccelerationsPtr );

    PHYS_MOTION_STREAM( "RampAccelerations:\n" << *rampAccelerationsPtr );

    RampAccelerations::iterator iAcc = rampAccelerationsPtr->begin();
    RampAccelerations::iterator iStartTranslation = iAcc;

    for( size_t i = 0; i < transformsPtr->size() - 1; ++i, ++iAcc )
    {
        const MexTransform3d& tx1 = (*transformsPtr)[ i ];
        const MexTransform3d& tx2 = (*transformsPtr)[ i + 1 ];

        const MATHEX_SCALAR distance = tx1.position().euclidianDistance( tx2.position() );

        PHYS_MOTION_INSPECT( distance );
        PHYS_MOTION_STREAM( *iAcc );

        if( distance < MexEpsilon::instance() )
        {
            //  This movement is pure rotation

            reviseRotationProfile( iAcc );
        }
        else
        {
            //  This movement is a translation - keep track of consective
            //  translations and revise them all at the same time.

            if( fabs( (*iAcc).startSpeed() ) < MexEpsilon::instance() )
            {
                PHYS_MOTION_STREAM( "Found start of a translation" << std::endl );

                iStartTranslation = iAcc;
            }

            if( fabs( (*iAcc).endSpeed() ) < MexEpsilon::instance() or
              i == transformsPtr->size() - 2 )
            {
                PHYS_MOTION_STREAM( "Found end of a translation" << std::endl );

                reviseTranslationProfile( iStartTranslation, iAcc + 1 );
            }
        }
    }

    POST( rampAccelerationsPtr->size() == transformsPtr->size() - 1 );

    PHYS_MOTION_INDENT( -2 );
    PHYS_MOTION_STREAM( "Exit MachPhysSpiderLegs::calculateAccelerations " << (void*)this << std::endl );
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysSpiderLegs::reviseTranslationProfile
(
    RampAccelerations::iterator begin,
    RampAccelerations::iterator end
) const
{
    PHYS_MOTION_STREAM( "Enter MachPhysSpiderLegs::reviseTranslationProfile " << (void*)this << std::endl );
    PHYS_MOTION_INDENT( 2 );

	CB_DEPIMPL( W4dCompositePlanPtr, restingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, walkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningLeftPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningRightPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, startWalkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, stopWalkingPlanPtr_);

    //  Calculate how long the move is taking at the moment

    MATHEX_SCALAR   originalTranslationTime = 0.0;
    for( RampAccelerations::iterator i = begin; i != end; ++i )
        originalTranslationTime += (*i).totalTime();

    PHYS_MOTION_INSPECT( originalTranslationTime );

	PhysRelativeTime startTime = 0;
	PhysRelativeTime stopTime = 0;

    const bool startAtRest =  fabs( (*begin).startSpeed() ) < MexEpsilon::instance();
    const bool finishAtRest = fabs( (*(end-1)).endSpeed() ) < MexEpsilon::instance();

    PHYS_MOTION_INSPECT( startAtRest );
    PHYS_MOTION_INSPECT( finishAtRest );

	if( startWalkingPlanPtr_.isDefined() and startAtRest )
        startTime = startWalkingPlanPtr_->finishTime();

    if( stopWalkingPlanPtr_.isDefined() and finishAtRest )
		stopTime = stopWalkingPlanPtr_->finishTime();

    PHYS_MOTION_INSPECT( startTime );
    PHYS_MOTION_INSPECT( stopTime );

	const PhysRelativeTime startStopTime = startTime + stopTime;

	const PhysRelativeTime walkingStepDuration = walkingPlanPtr_->finishTime();

	PhysRelativeTime walkingTime = originalTranslationTime - startStopTime;

    PHYS_MOTION_INSPECT( walkingTime );

    MATHEX_SCALAR translationTime;

	if(	walkingTime > 0)
	{
		uint wholeCycles = ( (uint) ( walkingTime / walkingStepDuration ) ) + 1;

        PHYS_MOTION_INSPECT( wholeCycles );

		walkingTime = wholeCycles * walkingStepDuration;
		translationTime = walkingTime + startStopTime;
	}
	else
	{
		if(	originalTranslationTime > startTime)
			translationTime = startStopTime;
		else
			translationTime = startTime;
	}

    PHYS_MOTION_INSPECT( translationTime );

    ASSERT( translationTime != 0.0, "" );

    MATHEX_SCALAR   scaleFactor = translationTime / originalTranslationTime;

    PHYS_MOTION_INSPECT( scaleFactor );

    ASSERT_INFO( translationTime );
    ASSERT_INFO( originalTranslationTime );
    ASSERT_INFO( scaleFactor );

    ASSERT( scaleFactor >= 1.0, "" );

    const MATHEX_SCALAR maxAcceleration = pMachine()->maxTranslationAcceleration();
    const MATHEX_SCALAR maxDeceleration = maxAcceleration;

    //  Adjust each individual ramp acceleration to run over a different time
    for( RampAccelerations::iterator i = begin; i != end; ++i )
    {
        PHYS_MOTION_STREAM( "Old ramp acceleration:" << std::endl );
        PHYS_MOTION_STREAM( *i );
        PHYS_MOTION_STREAM( std::endl );

        MATHEX_SCALAR newTime = (*i).totalTime() * scaleFactor;

        (*i) = PhysRampAcceleration(
          (*i).totalDistance(), (*i).startSpeed(), (*i).constantSpeed(),
          (*i).endSpeed(), newTime, maxAcceleration, maxDeceleration,
          PhysRampAcceleration::BY_DISTANCE_TIME );

        PHYS_MOTION_STREAM( "New ramp acceleration:" << std::endl );
        PHYS_MOTION_STREAM( *i );
        PHYS_MOTION_STREAM( std::endl );
    }

    PHYS_MOTION_INDENT( -2 );
    PHYS_MOTION_STREAM( "Exit MachPhysSpiderLegs::reviseTranslationProfile " << (void*)this << std::endl );
}

void MachPhysSpiderLegs::reviseRotationProfile(
    RampAccelerations::iterator iAcc
) const
{
	CB_DEPIMPL( W4dCompositePlanPtr, restingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, walkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningLeftPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningRightPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, startWalkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, stopWalkingPlanPtr_);

    PRE( turningLeftPlanPtr_.isDefined() );
    PRE( turningRightPlanPtr_.isDefined() );

	PhysRelativeTime turnTime = (*iAcc).totalTime();
    MATHEX_SCALAR   turnAngle = (*iAcc).totalDistance();

	PhysRelativeTime cycle;

	if ( turnAngle < 0 )
		cycle = turningLeftPlanPtr_->finishTime();
	else
		cycle = turningRightPlanPtr_->finishTime();

	uint wholeCycles = (uint) turnTime / cycle;

	if(	wholeCycles == 0 )
		wholeCycles += 1;

	//revise the turnTime
	turnTime = wholeCycles * cycle;

    MATHEX_SCALAR desiredSpeed = (*iAcc).constantSpeed();

    MATHEX_SCALAR maxAcceleration = pMachine()->maxRotationAcceleration().asScalar();
    MATHEX_SCALAR maxDeceleration = maxAcceleration;

    //  Set up a new acceleration which runs for the appropriate time.
    (*iAcc) = PhysRampAcceleration(
      turnAngle, (*iAcc).startSpeed(), desiredSpeed, (*iAcc).endSpeed(),
      turnTime, maxAcceleration, maxDeceleration,
      PhysRampAcceleration::BY_DISTANCE_TIME );
}

// virtual
void MachPhysSpiderLegs::stopDead()
{
	CB_DEPIMPL( W4dCompositePlanPtr, stopWalkingPlanPtr_);

    //Play the stop walking animation
  	pMachine()->plan( *stopWalkingPlanPtr_, SimManager::instance().currentTime(), 0, MachPhys::MACHINE_LOCOMOTION );
}

ostream& operator <<( ostream& o, const MachPhysSpiderLegs& t )
{

    o << "MachPhysSpiderLegs " << (void*)&t << " start" << std::endl;
    o << "MachPhysSpiderLegs " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysSpiderLegs& legs )
{
    const MachPhysLocomotionMethod& base = legs;

    ostr << base;

    ostr << legs.pImpl_;
}

void perRead( PerIstream& istr, MachPhysSpiderLegs& legs )
{
    MachPhysLocomotionMethod& base = legs;

    istr >> base;

    istr >> legs.pImpl_;
}

void MachPhysSpiderLegs::doFirstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state )
{
	CB_DEPIMPL( W4dCompositePlanPtr, restingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, walkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningLeftPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, turningRightPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, startWalkingPlanPtr_);
	CB_DEPIMPL( W4dCompositePlanPtr, stopWalkingPlanPtr_);

    //We don't want to play turning animatins, because they make first person too difficult.
    //So locally map turning states into stopped state.
    //But we do want to do them if remotely controlled
    MachPhysLocomotionMethod::FirstPersonMotionState oldState = pImpl_->lastState();

    if( not isRemotelyControlledFirstPerson() )
    {
        if( oldState == MachPhysLocomotionMethod::TURNING_LEFT or
            oldState == MachPhysLocomotionMethod::TURNING_RIGHT or
            oldState == MachPhysLocomotionMethod::ENTERING)
            oldState = MachPhysLocomotionMethod::STOPPED;

        if( state == MachPhysLocomotionMethod::TURNING_LEFT or
            state == MachPhysLocomotionMethod::TURNING_RIGHT or
            state == MachPhysLocomotionMethod::ENTERING )
            state = MachPhysLocomotionMethod::STOPPED;
    }

    //If we haven't changed state do nothing
    if( state != oldState )
    {
        switch( state )
        {
            case MachPhysLocomotionMethod::STOPPED:
            {
                //Play the stop walking animation once
              	pMachine()->plan( *restingPlanPtr_, SimManager::instance().currentTime(),
              	                  0, MachPhys::MACHINE_LOCOMOTION );
                break;
            }

            case MachPhysLocomotionMethod::MOVING_FORWARDS:
            case MachPhysLocomotionMethod::MOVING_BACKWARDS:
            {
                //Play the walking animation forever
              	pMachine()->plan( *walkingPlanPtr_, SimManager::instance().currentTime(),
              	                  1000000, MachPhys::MACHINE_LOCOMOTION );
                break;
            }

            case MachPhysLocomotionMethod::TURNING_LEFT:
            {
                //Play the turning animation forever
              	pMachine()->plan( *turningLeftPlanPtr_, SimManager::instance().currentTime(),
              	                  1000000, MachPhys::MACHINE_LOCOMOTION );
                break;
            }

            case MachPhysLocomotionMethod::TURNING_RIGHT:
            {
                //Play the turning animation forever
              	pMachine()->plan( *turningRightPlanPtr_, SimManager::instance().currentTime(),
              	                  1000000, MachPhys::MACHINE_LOCOMOTION );
                break;
            }

            case MachPhysLocomotionMethod::ENTERING:
            case MachPhysLocomotionMethod::LEAVING:
                break;
        }
    }
}

/* End SPIDER.CPP ***************************************************/

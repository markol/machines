/*
 * H O V E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"

#include "machphys/hover.hpp"
#include "machphys/internal/hoveri.hpp"
#include "machphys/machine.hpp"
#include "machphys/mcmovinf.hpp"

#include "world4d/link.hpp"
#include "world4d/composit.hpp"
#include "world4d/entyplan.hpp"

#include "phys/timeangl.hpp"
#include "phys/linetrav.hpp"
#include "mathex/point3d.hpp"

#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysHoverBoots );

MachPhysHoverBoots::MachPhysHoverBoots(
    MachPhysMachine* pMachine,
    W4dLink* pLHoverBoot,
    W4dLink* pRHoverBoot,
    W4dLink* pBobbingLink,
    MATHEX_SCALAR bobHeight )

: MachPhysLocomotionMethod( pImpl_ = _NEW( MachPhysHoverBootsImpl( pMachine, this, pLHoverBoot, pRHoverBoot, pBobbingLink, bobHeight ) ) )
{
    startBobbing();

    TEST_INVARIANT;
}

MachPhysHoverBoots::MachPhysHoverBoots( PerConstructor con )
: MachPhysLocomotionMethod( con ),
  pImpl_( NULL )
{
}

MachPhysHoverBoots::~MachPhysHoverBoots()
{
    TEST_INVARIANT;

    // pImpl_ will be removed by MachPhysLocomotionMethod destructor
    //_DELETE( pImpl_ );
}

// virtual
MachPhysLocomotionMethod* MachPhysHoverBoots::clone( MachPhysMachine* pNewMachine, const W4dLinks& links )
{
    CB_DEPIMPL(W4dLink*, pLHoverBoot_);
    CB_DEPIMPL(W4dLink*, pRHoverBoot_);
	CB_DEPIMPL(W4dLink*, pBobbingLink_);
	CB_DEPIMPL(MATHEX_SCALAR, bobHeight_);

    W4dLink* pLHoverBoot = NULL;
    W4dLink* pRHoverBoot = NULL;
	W4dLink* pBobbingLink = NULL;


	if( pLHoverBoot_!= NULL )
    	pLHoverBoot = links[ pLHoverBoot_->id() ];

	if( pRHoverBoot_!= NULL )
    	pRHoverBoot = links[ pRHoverBoot_->id() ];

	if( pBobbingLink_!= NULL )
		pBobbingLink = links[ pBobbingLink_->id() ];

    return _NEW( MachPhysHoverBoots( pNewMachine, pLHoverBoot, pRHoverBoot, pBobbingLink, bobHeight_ ) );
}

// virtual
void MachPhysHoverBoots::moveAnimations
(
    const PhysLinearTravelPlan& linearTravelPlan, const TurnAngles& turnAngles,
    const MachPhysMachineMoveInfo& info
)
{
    CB_DEPIMPL(W4dLink*, pLHoverBoot_);
    CB_DEPIMPL(W4dLink*, pRHoverBoot_);
	CB_DEPIMPL(W4dLink*, pBobbingLink_);
	CB_DEPIMPL(MATHEX_SCALAR, bobHeight_);
	CB_DEPIMPL(MexRadians, lastLAngle_);
	CB_DEPIMPL(MexRadians, lastRAngle_);

    PRE( linearTravelPlan.nSegments() == turnAngles.size() );

	PhysAbsoluteTime startTime = info.startTime();

    const PhysMotionPlan::RampAccelerations& rampAccelerations = (*linearTravelPlan.rampAccelerations());

    //Set up collections for the angles and times
    PhysMotionPlan::Angles*  pLAngles = _NEW( PhysMotionPlan::Angles );
    PhysMotionPlan::Angles*  pRAngles = _NEW( PhysMotionPlan::Angles );
    PhysMotionPlan::Times*   pTimes = _NEW( PhysMotionPlan::Times );

    size_t  nSegments = linearTravelPlan.nSegments();
    pLAngles->reserve( 1 + nSegments * 5 );
    pRAngles->reserve( 1 + nSegments * 5 );
    pTimes->reserve( nSegments * 5 );

    //  The first angle is the last one queued
    PhysRelativeTime segmentStartTime = 0;
    pLAngles->push_back( lastLAngle_ );
    pRAngles->push_back( lastRAngle_ );

    //Add angles for each segment
    MexRadians maxSwingAngle = MexDegrees( 30.0 );
    MexRadians maxTurnAngle = MexDegrees( 5.0 );

    for( size_t i = 0; i < nSegments; ++i )
    {
        //Get the times spent at constant velocity, etc
        PhysRelativeTime segmentEndTime = segmentStartTime + rampAccelerations[ i ].totalTime();
        PhysRelativeTime accelerationTime = rampAccelerations[ i ].firstAccelerationTime();
        PhysRelativeTime decelerationTime = rampAccelerations[ i ].secondAccelerationTime();
        PhysRelativeTime constantVelocityTime = (segmentEndTime - segmentStartTime) -
                                        (accelerationTime + decelerationTime);

        ASSERT( constantVelocityTime >= -MexEpsilon::instance(), "" );

        if( constantVelocityTime < 0 ) constantVelocityTime = 0;

        //Decide whether turning or translating
        if( fabs( turnAngles[ i ].asScalar() ) < 0.01 )
        {
            //Straight line motion. Swing angle is determined by the acceleration
            //period, if any. Otherwise use the maximum angle
            MexRadians swingAngle;
            if( accelerationTime != 0.0 )
            {
                swingAngle = MexDegrees( accelerationTime * 20.0 );
                if( swingAngle > maxSwingAngle ) swingAngle = maxSwingAngle;
            }
            else swingAngle = maxSwingAngle;

            //Need a transition time during the constant velocity period when we switch
            //between acceleration/deceleration postures and constant velocity posture.
            PhysRelativeTime transitionTime = constantVelocityTime * 0.333;
            if( transitionTime > 1.0 ) transitionTime = 1.0;
            else if( transitionTime < 0.25 ) transitionTime = 0.0;

            //Deal with any acceleration period
            PhysRelativeTime eventTime = segmentStartTime;
            if( accelerationTime != 0.0 )
            {
                //Move angle throughout acceleration
                pLAngles->push_back( -swingAngle );
                pRAngles->push_back( -swingAngle );
                eventTime += accelerationTime;
                pTimes->push_back( eventTime );

                //Swing back to constant velocity, if doing it
                if( transitionTime )
                {
                    pLAngles->push_back( -swingAngle * 0.33 );
                    pRAngles->push_back( -swingAngle * 0.33 );
                    pTimes->push_back( eventTime + transitionTime );
                }
            }

            //Deal with any deceleration period
            PhysRelativeTime previousTime = eventTime;
            if( decelerationTime != 0.0 )
            {
                //get time of start of deceleration, and ensure not earlier than
                //end of acceleration.
                eventTime = segmentEndTime - decelerationTime;
                if( eventTime < previousTime ) eventTime = previousTime;

                //Swing forwards to deceleration posture if doing a transition
                if( transitionTime )
                {
                    pLAngles->push_back( -swingAngle * 0.33 );
                    pRAngles->push_back( -swingAngle * 0.33 );
                    pTimes->push_back( eventTime - transitionTime );

                    pLAngles->push_back( swingAngle );
                    pRAngles->push_back( swingAngle );
                    pTimes->push_back( eventTime );
                }

                //Move angle throughout deceleration
                lastLAngle_ = lastRAngle_ = 0.0;

            }
            else
            {
                //Ensure maintain constant velocity posture to end
                lastLAngle_ = lastRAngle_ = -swingAngle * 0.33;
            }
        }
        else
        {
            //  Turning on the spot, one hoverboot goes forwards, the other
            //  backwards. The amount by which they swing forwards or back
            //  depends on the angle through which we are turning - the bigger
            //  the angle, the bigger the swing.

            MexRadians swingAngle = turnAngles[ i ] / 3.0;
            if( swingAngle > maxTurnAngle ) swingAngle = maxTurnAngle;
            else if( swingAngle < -maxTurnAngle ) swingAngle = -maxTurnAngle;

            pLAngles->push_back( -swingAngle );
            pRAngles->push_back( swingAngle );
            pTimes->push_back( (segmentStartTime + segmentEndTime) * 0.5 );

            lastLAngle_ = lastRAngle_ = 0.0;
        }

        //Push back the final angle
        pLAngles->push_back( lastLAngle_ );
        pRAngles->push_back( lastRAngle_ );
        pTimes->push_back( segmentEndTime );

        segmentStartTime = segmentEndTime;
    }

    PhysMotionPlan::AnglesPtr   lAnglesPtr( pLAngles );
    PhysMotionPlan::AnglesPtr   rAnglesPtr( pRAngles );
    PhysMotionPlan::TimesPtr    timesPtr( pTimes );

    MexVec3 axis( 0.0, -1.0, 0.0 );

if(pLHoverBoot_ != NULL and pRHoverBoot_!= NULL)
{
    PhysTimedAnglePlan* pLPlan = _NEW( PhysTimedAnglePlan( lAnglesPtr, timesPtr, axis, pLHoverBoot_->localTransform().position() ) );
    PhysTimedAnglePlan* pRPlan = _NEW( PhysTimedAnglePlan( rAnglesPtr, timesPtr, axis, pRHoverBoot_->localTransform().position() ) );

    PhysMotionPlanPtr lPlanPtr( pLPlan );
    PhysMotionPlanPtr rPlanPtr( pRPlan );

    pLHoverBoot_->entityPlanForEdit().absoluteMotion( lPlanPtr, startTime, 0, MachPhys::MACHINE_LOCOMOTION );
    pRHoverBoot_->entityPlanForEdit().absoluteMotion( rPlanPtr, startTime, 0, MachPhys::MACHINE_LOCOMOTION );
}
}

void MachPhysHoverBoots::startBobbing( void )
{
	CB_DEPIMPL(W4dLink*, pBobbingLink_);
	CB_DEPIMPL(MATHEX_SCALAR, bobHeight_);

    PRE( pBobbingLink_ != NULL );

    if( bobHeight_ > 0.0 )
    {
        //  Set up a general "bobbing up and down" motion for the hovering machine

        //Get the initial transform
        MexTransform3d downTransform( pBobbingLink_->localTransform() );

        MexTransform3d upTransform( downTransform );
        MexPoint3d upPosition = upTransform.position();
        upPosition.z( upPosition.z() + bobHeight_ );
        upTransform.position( upPosition );

        uint    repeatForever = 1000000;

        PhysMotionPlan::Transforms* pTransforms = _NEW( PhysMotionPlan::Transforms );
        pTransforms->reserve( 3 );

        pTransforms->push_back( downTransform );
        pTransforms->push_back( upTransform );
        pTransforms->push_back( downTransform );

        MATHEX_SCALAR startTranslationSpeed = 0.0;
        MATHEX_SCALAR translationSpeed = 3.0;
        MATHEX_SCALAR translationAcceleration = 0.2;
        MATHEX_SCALAR translationDeceleration = 0.2;
        MATHEX_SCALAR startRotationSpeed = 0.0;
        MATHEX_SCALAR rotationSpeed = 0.1;
        MATHEX_SCALAR rotationAcceleration = 0.1;
        MATHEX_SCALAR rotationDeceleration = 0.1;
        bool comeToRest = true;

        PhysLinearTravelPlan* pPlan = _NEW( PhysLinearTravelPlan(
          PhysMotionPlan::TransformsPtr( pTransforms ),
          startTranslationSpeed,
          translationSpeed,
          translationAcceleration,
          translationDeceleration,
          startRotationSpeed,
          rotationSpeed,
          rotationAcceleration,
          rotationDeceleration,
          comeToRest ) );

        PhysAbsoluteTime now = SimManager::instance().currentTime();

        pBobbingLink_->entityPlanForEdit().absoluteMotion(
          PhysMotionPlanPtr( pPlan ),
          now,
          repeatForever, BOBBING );
    }
}

void MachPhysHoverBoots::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// virtual
void MachPhysHoverBoots::stopDead()
{
	CB_DEPIMPL(W4dLink*, pLHoverBoot_);
	CB_DEPIMPL(W4dLink*, pRHoverBoot_);

    if( pLHoverBoot_ )
    	pitchOrRollAnimation(pLHoverBoot_, MexDegrees( 30 ), MexVec3(0, 1, 0), 1.0 );

	if( pRHoverBoot_ )
    	pitchOrRollAnimation(pRHoverBoot_, MexDegrees( 30 ), MexVec3(0, 1, 0), 1.0 );
}

// virtual
void MachPhysHoverBoots::reviseProfile( TransformsPtr transformsPtr )
{
    forceUpright( transformsPtr );
}

ostream& operator <<( ostream& o, const MachPhysHoverBoots& t )
{

    o << "MachPhysHoverBoots " << (void*)&t << " start" << std::endl;
    o << "MachPhysHoverBoots " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysHoverBoots& boots )
{
    const MachPhysLocomotionMethod& base = boots;

    ostr << base;

    ostr << boots.pImpl_;
}

void perRead( PerIstream& istr, MachPhysHoverBoots& boots )
{
    MachPhysLocomotionMethod& base = boots;

    istr >> base;

    istr >> boots.pImpl_;
}

void MachPhysHoverBoots::doFirstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state )
{
	CB_DEPIMPL(W4dLink*, pLHoverBoot_);
	CB_DEPIMPL(W4dLink*, pRHoverBoot_);

    //If we haven't changed state do nothing
    if( state != pImpl_->lastState() )
    {
        switch( state )
        {
            case MachPhysLocomotionMethod::STOPPED:
            {
				//hover rocks forward, backward, then come to rest
				pitchOrRollAnimation(pLHoverBoot_, MexDegrees( 25 ), MexVec3(0, 1, 0), 1.0 );
				pitchOrRollAnimation(pRHoverBoot_, MexDegrees( 25 ), MexVec3(0, 1, 0), 1.0 );
                break;
            }

            case MachPhysLocomotionMethod::MOVING_FORWARDS:
			{
				//hover pitches down
				MexPoint3d position = pLHoverBoot_->localTransform().position();
				pLHoverBoot_->localTransform( transform(MexDegrees( -25 ), MexVec3( 0, 1, 0), position ) );

				position = pRHoverBoot_->localTransform().position();
				pRHoverBoot_->localTransform( transform(MexDegrees( -25 ), MexVec3( 0, 1, 0), position ) );
                break;
			}
            case MachPhysLocomotionMethod::MOVING_BACKWARDS:
            {
				//hover pitches up
				MexPoint3d position = pLHoverBoot_->localTransform().position();
				pLHoverBoot_->localTransform( transform(MexDegrees( 25 ), MexVec3( 0, 1, 0), position ) );

				position = pRHoverBoot_->localTransform().position();
				pRHoverBoot_->localTransform( transform(MexDegrees( 25 ), MexVec3( 0, 1, 0), position ) );
                break;
            }

            case MachPhysLocomotionMethod::TURNING_LEFT:
            case MachPhysLocomotionMethod::TURNING_RIGHT:
            case MachPhysLocomotionMethod::ENTERING:
            case MachPhysLocomotionMethod::LEAVING:
                break;
        }
    }
}

/* End HOVER.CPP ****************************************************/

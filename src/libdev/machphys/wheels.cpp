/*
 * W H E E L S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/radians.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/eulerang.hpp"
#include "phys/rampacce.hpp"

#include "machphys/wheels.hpp"
#include "machphys/internal/wheelsi.hpp"
#include "machphys/mcmovinf.hpp"

#include "world4d/link.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/manager.hpp"
#include "phys/linetrav.hpp"
#include "phys/linemoti.hpp"
#include "phys/timespin.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysWheels );

MachPhysWheels::MachPhysWheels(
  MachPhysMachine* pMachine,
  MATHEX_SCALAR wheelRadius, const W4dLinks& leftWheels, const W4dLinks& rightWheels )
: MachPhysLocomotionMethod( pImpl_ = _NEW( MachPhysWheelsImpl( pMachine, this,
                                              wheelRadius, leftWheels, rightWheels ) ) )
{

    TEST_INVARIANT;
}

MachPhysWheels::MachPhysWheels( PerConstructor con )
: MachPhysLocomotionMethod( con ),
  pImpl_( NULL )
{
}

MachPhysWheels::~MachPhysWheels()
{
    TEST_INVARIANT;

    // pImpl_ will be removed by MachPhysLocomotionMethod destructor
    //_DELETE( pImpl_ );
}

// virtual
MachPhysLocomotionMethod* MachPhysWheels::clone( MachPhysMachine* pNewMachine, const W4dLinks& links )
{
    CB_DEPIMPL(W4dLinks, rightWheels_);
    CB_DEPIMPL(W4dLinks, leftWheels_);
    CB_DEPIMPL(MATHEX_SCALAR, wheelRadius_);

    W4dLinks    leftWheels;
    leftWheels.reserve( leftWheels_.size() );

    W4dLinks    rightWheels;
    rightWheels.reserve( rightWheels_.size() );

    for( W4dLinks::const_iterator i = leftWheels_.begin(); i != leftWheels_.end(); ++i )
    {
        W4dLink*    pLink = links[ (*i)->id() ];
        leftWheels.push_back( pLink );
    }

    for( W4dLinks::const_iterator i = rightWheels_.begin(); i != rightWheels_.end(); ++i )
    {
        W4dLink*    pLink = links[ (*i)->id() ];
        rightWheels.push_back( pLink );
    }

    return _NEW( MachPhysWheels( pNewMachine, wheelRadius_, leftWheels, rightWheels ) );
}

void MachPhysWheels::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// virtual
void MachPhysWheels::moveAnimations
(
    const PhysLinearTravelPlan& linearTravelPlan, const TurnAngles& turnAngles,
    const MachPhysMachineMoveInfo&  info
)
{
    CB_DEPIMPL(MATHEX_SCALAR, wheelRadius_);
    CB_DEPIMPL(MexRadians, leftLastAngle_);
    CB_DEPIMPL(MexRadians, rightLastAngle_);
    CB_DEPIMPL(MexRadians, leftLastAngleSpeed_);
    CB_DEPIMPL(MexRadians, rightLastAngleSpeed_);
    CB_DEPIMPL(W4dLinks, leftWheels_);
    CB_DEPIMPL(W4dLinks, rightWheels_);

    MISC_STREAM("MachPhysWheels::moveAnimations entry " << (void*)this << std::endl);
    MISC_STREAM("  linearTravelPlan.nSegments() " << linearTravelPlan.nSegments() << std::endl);
    MISC_STREAM("  turnAngles.size() " << turnAngles.size() << std::endl);
    MISC_INSPECT( leftLastAngle_ );
    MISC_INSPECT( leftLastAngleSpeed_ );
    MISC_INSPECT( rightLastAngle_ );
    MISC_INSPECT( rightLastAngleSpeed_ );

    PRE( linearTravelPlan.nSegments() == turnAngles.size() );
    PRE( leftWheels_.size() == rightWheels_.size() );

	PhysAbsoluteTime startTime = info.startTime();

    //Get the time and acceleration data collections
    const PhysMotionPlan::Times& segmentTimes = *(linearTravelPlan.segmentTimes());
    const PhysMotionPlan::RampAccelerations& rampAccelerations = *(linearTravelPlan.rampAccelerations());

    //Get number of segments. Each segment may have an acceleration period, a rest
    //period and a deceleration period. Hence total possible number of segments in our
    //spin plan is 3 * number of segments in the travel plan
    size_t nSegments = segmentTimes.size();
    size_t nSpinSegments = nSegments * 3;

    //Set up various data
    MexVec3 axis( 0.0, 1.0, 0.0 );
    MATHEX_SCALAR eps = MexEpsilon::instance();
    uint nWheelPairs = leftWheels_.size();

    //Iterate through each pair of wheels
    W4dLinks::iterator il = leftWheels_.begin();
    W4dLinks::iterator ir = rightWheels_.begin();

    for( ; il != leftWheels_.end(); ++il, ++ir )
    {
        //Get the left and right wheel pointers
        W4dLink* pLeftWheel = *il;
        W4dLink* pRightWheel = *ir;

        //Get the basic wheel orientation and location
        MexPoint3d leftOrigin = pLeftWheel->localTransform().position();
        MexPoint3d rightOrigin = pRightWheel->localTransform().position();

        PhysTimedSpinPlan* pLPlan =
            _NEW( PhysTimedSpinPlan( axis, leftOrigin,
                                     leftLastAngle_, leftLastAngleSpeed_, nSpinSegments ) );

        PhysTimedSpinPlan* pRPlan =
            _NEW( PhysTimedSpinPlan( axis, rightOrigin,
                                     rightLastAngle_, rightLastAngleSpeed_, nSpinSegments ) );

        //Extend the plans for each linear plan segment
        PhysRelativeTime lastTime = 0;
        for( size_t i = 0; i < nSegments; ++i )
        {
            //Decide on direction of motion for wheels on each side
            bool leftForwards, rightForwards;
            if( fabs( turnAngles[ i ].asScalar() ) < eps )
            {
                //  Straight line motion
                leftForwards = rightForwards = true;
            }
            else
            {
                rightForwards = turnAngles[ i ] < MexRadians( 0.0 );
                leftForwards = not rightForwards;
            }

            //Compute the times for each part of the segment.
            PhysRelativeTime accelerationTime = rampAccelerations[i].firstAccelerationTime();
            PhysRelativeTime decelerationTime = rampAccelerations[i].secondAccelerationTime();
            PhysRelativeTime segmentEndTime = segmentTimes[i];
            PhysRelativeTime constantVelocityTime =
                segmentEndTime - lastTime - accelerationTime - decelerationTime;

            MISC_INSPECT( segmentEndTime );
            MISC_INSPECT( constantVelocityTime );
            MISC_INSPECT( segmentEndTime );
            MISC_INSPECT( lastTime );
            MISC_INSPECT( accelerationTime );
            MISC_INSPECT( decelerationTime );

            lastTime = segmentEndTime;

            MexRadians rotationalAcceleration( rampAccelerations[ i ].firstAcceleration() / wheelRadius_ );
            MISC_INSPECT( rampAccelerations[ i ].firstAcceleration() );
            MISC_INSPECT( accelerationTime );

            //Extend the spin plans for acceleration in this segment
            if( accelerationTime != 0.0 )
            {
                pLPlan->addSegment( accelerationTime, leftForwards ? rotationalAcceleration :
                                                                    -rotationalAcceleration );
                pRPlan->addSegment( accelerationTime, rightForwards ? rotationalAcceleration :
                                                                     -rotationalAcceleration );
            }

            //Extend for constant velocity period
            if( constantVelocityTime > 0.0 )
            {
                pLPlan->addSegment( constantVelocityTime, 0.0 );
                pRPlan->addSegment( constantVelocityTime, 0.0 );
            }

            //Extend the spin plans for deceleration in this segment
            if( decelerationTime != 0.0 )
            {
                pLPlan->addSegment( decelerationTime, leftForwards ? -rotationalAcceleration :
                                                                      rotationalAcceleration );
                pRPlan->addSegment( decelerationTime, rightForwards ? -rotationalAcceleration :
                                                                       rotationalAcceleration );
            }
        }

        //Register the plans for left and right wheels
        PhysMotionPlanPtr lPlanPtr( pLPlan );
        pLeftWheel->entityPlanForEdit().absoluteMotion( lPlanPtr, startTime, 0, MachPhys::MACHINE_LOCOMOTION );

        PhysMotionPlanPtr rPlanPtr( pRPlan );
        pRightWheel->entityPlanForEdit().absoluteMotion( rPlanPtr, startTime, 0, MachPhys::MACHINE_LOCOMOTION );

        //Update the cached wheel angles and speeds
        if( --nWheelPairs == 0 )
        {
            leftLastAngle_ = pLPlan->angle( lastTime );
            leftLastAngleSpeed_ = pLPlan->speed( lastTime );
            rightLastAngle_ = pRPlan->angle( lastTime );
            rightLastAngleSpeed_ = pRPlan->speed( lastTime );
        }
    }
    MISC_INSPECT( leftLastAngle_ );
    MISC_INSPECT( leftLastAngleSpeed_ );
    MISC_INSPECT( rightLastAngle_ );
    MISC_INSPECT( rightLastAngleSpeed_ );
    MISC_STREAM( "MachPhysWheels::moveAnimations exit " << std::endl);
}

// virtual
void MachPhysWheels::stopDead()
{
    CB_DEPIMPL(MATHEX_SCALAR, wheelRadius_);
    CB_DEPIMPL(MexRadians, leftLastAngle_);
    CB_DEPIMPL(MexRadians, rightLastAngle_);
    CB_DEPIMPL(MexRadians, leftLastAngleSpeed_);
    CB_DEPIMPL(MexRadians, rightLastAngleSpeed_);
    CB_DEPIMPL(W4dLinks, leftWheels_);
    CB_DEPIMPL(W4dLinks, rightWheels_);

    MISC_STREAM("MachPhysWheels::stopDead() entry " << (void*)this << std::endl);
    //Iterate through each pair of wheels
    W4dLinks::iterator il = leftWheels_.begin();
    W4dLinks::iterator ir = rightWheels_.begin();

    MexVec3 axis( 0.0, 1.0, 0.0 );
    uint nWheelPairs = leftWheels_.size();

	PhysAbsoluteTime startTime = W4dManager::instance().time();

    for( ; il != leftWheels_.end(); ++il, ++ir )
    {
        //Get the left and right wheel pointers
        W4dLink* pLeftWheel = *il;
        W4dLink* pRightWheel = *ir;

        //Get the basic wheel orientation and location
        MexPoint3d leftOrigin = pLeftWheel->localTransform().position();
        MexPoint3d rightOrigin = pRightWheel->localTransform().position();

        const size_t nSpinSegments = 1;

        PhysTimedSpinPlan* pLPlan =
            _NEW( PhysTimedSpinPlan( axis, leftOrigin,
                                     leftLastAngle_, leftLastAngleSpeed_, nSpinSegments ) );

        PhysTimedSpinPlan* pRPlan =
            _NEW( PhysTimedSpinPlan( axis, rightOrigin,
                                     rightLastAngle_, rightLastAngleSpeed_, nSpinSegments ) );

        //  Spin the wheels backwards very quickly
        const MATHEX_SCALAR acceleration = 1000;
        const PhysRelativeTime accelerationTime = 0.5;

        //Extend the spin plans for acceleration in this segment
        if( accelerationTime != 0.0 )
        {
            pLPlan->addSegment( accelerationTime, acceleration );
            pRPlan->addSegment( accelerationTime, -acceleration );
        }

        //Register the plans for left and right wheels
        PhysMotionPlanPtr lPlanPtr( pLPlan );
        pLeftWheel->entityPlanForEdit().absoluteMotion( lPlanPtr, startTime, 0 );

        PhysMotionPlanPtr rPlanPtr( pRPlan );
        pRightWheel->entityPlanForEdit().absoluteMotion( rPlanPtr, startTime, 0 );

        //Update the cached wheel angles and speeds
        if( --nWheelPairs == 0 )
        {
            leftLastAngle_ = pLPlan->angle( accelerationTime );
            leftLastAngleSpeed_ = 0.0;
            rightLastAngle_ = pRPlan->angle( accelerationTime );
            rightLastAngleSpeed_ = 0.0;
        }
    }
    MISC_INSPECT( leftLastAngle_ );
    MISC_INSPECT( leftLastAngleSpeed_ );
    MISC_INSPECT( rightLastAngle_ );
    MISC_INSPECT( rightLastAngleSpeed_ );
    MISC_STREAM("MachPhysWheels::stopDead() exit " << (void*)this << std::endl);
}

void MachPhysWheels::firstPersonMotionAnimations( bool leftForwards, bool rightForwards )
{
    CB_DEPIMPL(MATHEX_SCALAR, wheelRadius_);
    CB_DEPIMPL(MexRadians, leftLastAngle_);
    CB_DEPIMPL(MexRadians, rightLastAngle_);
    CB_DEPIMPL(MexRadians, leftLastAngleSpeed_);
    CB_DEPIMPL(MexRadians, rightLastAngleSpeed_);
    CB_DEPIMPL(W4dLinks, leftWheels_);
    CB_DEPIMPL(W4dLinks, rightWheels_);

    //Iterate through each pair of wheels
    W4dLinks::iterator il = leftWheels_.begin();
    W4dLinks::iterator ir = rightWheels_.begin();

    uint nWheelPairs = leftWheels_.size();

	PhysAbsoluteTime startTime = W4dManager::instance().time();

    for( ; il != leftWheels_.end(); ++il, ++ir )
    {
        //Get the left and right wheel pointers
        W4dLink* pLeftWheel = *il;
        W4dLink* pRightWheel = *ir;

        //Get the basic wheel orientation and location
        MexPoint3d leftOrigin = pLeftWheel->localTransform().position();
        MexPoint3d rightOrigin = pRightWheel->localTransform().position();
		MATHEX_SCALAR scalarAngle = std::max(leftLastAngleSpeed_.asScalar(), rightLastAngleSpeed_.asScalar() );
		PhysRelativeTime sectionTime = 120.0 / (scalarAngle>0? scalarAngle:360.0);

        MexTransform3d leftSpinTransform( MexEulerAngles( 0.0, MexDegrees(leftForwards? 120:-120 ), 0.0 ) );
        MexTransform3d rightSpinTransform( MexEulerAngles( 0.0, MexDegrees(rightForwards? 120:-120), 0.0 ) );

        //Set start and first position transforms
        const MexTransform3d& leftStartPosition = pLeftWheel->localTransform();
        const MexTransform3d& rightStartPosition = pRightWheel->localTransform();

        MexTransform3d leftEndPosition( leftStartPosition );
        			   leftEndPosition.transform( leftSpinTransform );

        MexTransform3d rightEndPosition( rightStartPosition );
        			   rightEndPosition.transform( rightSpinTransform );

        PhysLinearMotionPlan* pLeftPlan =
            _NEW( PhysLinearMotionPlan( leftStartPosition, leftEndPosition, sectionTime ) );

        PhysLinearMotionPlan* pRightPlan =
            _NEW( PhysLinearMotionPlan( rightStartPosition, rightEndPosition, sectionTime ) );

        //Add a further rotation of 120 degrees
        leftEndPosition.transform( leftSpinTransform );
        pLeftPlan->add( leftEndPosition, 2.0*sectionTime );

        rightEndPosition.transform( rightSpinTransform );
        pRightPlan->add( rightEndPosition, 2.0*sectionTime );

        //Finally move back to start position
        pLeftPlan->add( leftStartPosition, 3.0*sectionTime );
        pRightPlan->add( rightStartPosition, 3.0*sectionTime );

        //Register the plans for left and right wheels
        PhysMotionPlanPtr lPlanPtr( pLeftPlan );
        pLeftWheel->entityPlanForEdit().absoluteMotion( lPlanPtr, startTime, 10000000 );

        PhysMotionPlanPtr rPlanPtr( pRightPlan );
        pRightWheel->entityPlanForEdit().absoluteMotion( rPlanPtr, startTime, 10000000 );
    }
}

ostream& operator <<( ostream& o, const MachPhysWheels& t )
{

    o << "MachPhysWheels " << (void*)&t << " start" << std::endl;
    o << "MachPhysWheels " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysWheels& wheels )
{
    const MachPhysLocomotionMethod& base = wheels;

    ostr << base;
    ostr << wheels.pImpl_;

}

void perRead( PerIstream& istr, MachPhysWheels& wheels )
{
    MachPhysLocomotionMethod& base = wheels;

    istr >> base;
    istr >> wheels.pImpl_;
}

void MachPhysWheels::doFirstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state )
{
    //If we haven't changed state do nothing
    if( state != pImpl_->lastState() )
    {
		bool leftForwards, rightForwards;
        switch( state )
        {
            case MachPhysLocomotionMethod::STOPPED:
            {
                //Play the stop walking animation once
              	stopDead();
                break;
            }

            case MachPhysLocomotionMethod::MOVING_FORWARDS:
			{
				leftForwards = true;
				rightForwards = true;
				firstPersonMotionAnimations( leftForwards, rightForwards );
				break;
			}
            case MachPhysLocomotionMethod::MOVING_BACKWARDS:
            {
				leftForwards = false;
				rightForwards = false;
				firstPersonMotionAnimations( leftForwards, rightForwards );
                break;
            }

            case MachPhysLocomotionMethod::TURNING_LEFT:
            {
				leftForwards = false;
				rightForwards = true;
				firstPersonMotionAnimations( leftForwards, rightForwards );
                break;
            }

            case MachPhysLocomotionMethod::TURNING_RIGHT:
            {
				leftForwards = true;
				rightForwards = false;
				firstPersonMotionAnimations( leftForwards, rightForwards );
                break;
            }

            case MachPhysLocomotionMethod::ENTERING:
            case MachPhysLocomotionMethod::LEAVING:
                break;
        }
    }
}

/* End WHEELS.CPP ***************************************************/

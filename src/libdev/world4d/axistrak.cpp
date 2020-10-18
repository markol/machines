/*
 * A X I S T R A K . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <math.h>

#include "mathex/vec3.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/radians.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/abox3d.hpp"

#include "world4d/axistrak.hpp"
#include "world4d/manager.hpp"
#include "world4d/entity.hpp"
#include "world4d/composit.hpp"

#ifndef _INLINE
    #include "world4d/axistrak.ipp"
#endif

W4dAxisTrackerPlan::W4dAxisTrackerPlan
(
    const W4dEntity& turningObject, W4dEntity& targetObject, const MexTransform3d& baseTransform,
    W4d::Axis turningAxis, W4d::Axis pointingAxis, MexRadians turnRate, const PhysRelativeTime& duration
)
:   PhysMotionPlan( duration ),
    pTurningObject_( &turningObject ),
    pTargetObject_( &targetObject ),
    targetOffset_( W4dAxisTrackerPlan::targetOffset( targetObject ) ),
    baseTransform_( baseTransform ),
    turningAxis_( turningAxis ),
    pointingAxis_( pointingAxis ),
    turnRate_( turnRate ),
    limitAngles_( false ),
    nFramesPerTargetOffsetUpdate_( 0 ),
    lastUpdateTime_( W4dManager::instance().time() ),
    lastAngle_( currentAngle() )
{
    PRE( turnRate.asScalar() > 0.0 );
    PRE( turningAxis != pointingAxis );

    TEST_INVARIANT;
}

W4dAxisTrackerPlan::~W4dAxisTrackerPlan()
{
    TEST_INVARIANT;
}

void W4dAxisTrackerPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dAxisTrackerPlan& t )
{

    o << "W4dAxisTrackerPlan " << (void*)&t << " start" << std::endl;
    o << "W4dAxisTrackerPlan " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void W4dAxisTrackerPlan::transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult ) const
{
    //Needed for mutable data members
    W4dAxisTrackerPlan* nonConstThis = _CONST_CAST( W4dAxisTrackerPlan*, this );

    //First call indicated by target offset frame update interval set to 0.
    //In this case initialise to default value of 50 frames.
    bool firstTime = nFramesPerTargetOffsetUpdate_ == 0;
    if( firstTime)
        nonConstThis->nFramesPerTargetOffsetUpdate_ = 50;

    //Update the target offset if the required number of frames have elapsed, or first time
    W4dManager& w4dManager = W4dManager::instance();
    if( firstTime or (w4dManager.frameNumber() % nFramesPerTargetOffsetUpdate_) == 0 )
        nonConstThis->targetOffset_ = targetOffset( *pTargetObject_ );

    //Compute the global position of the target point
    MexPoint3d targetPoint( targetOffset_ );
    pTargetObject_->globalTransform().transform( &targetPoint );

    //Hence compute the target point relative to the turning object's base transform
    MexTransform3d turnerTransform;
    pTurningObject_->pParent()->globalTransform().transform( baseTransform_, &turnerTransform );
    turnerTransform.transformInverse( &targetPoint );

    //Project the target point into the plane through the origin which is normal to the turn axis,
    //making a unit vector unless the point lies on the turn axis.
    MATHEX_SCALAR newAngle;
    MexVec3 v( targetPoint );
    v[ turningAxis_ ] = 0.0;
    MATHEX_SCALAR size = v.modulus();
    bool pointOnAxis =  size < MexEpsilon::instance();

    if( pointOnAxis )
    {
        newAngle = lastAngle_.asScalar();
    }
    else
    {
        //Make the vector a unit vector
        v /= size;

        //Get the sine and cosine of the desired turn angle, depending on point and aim axes
        MATHEX_SCALAR cosine = v[ pointingAxis_ ];
        MATHEX_SCALAR sine = v[ 3 - pointingAxis_ - turningAxis_ ];
        if( ((turningAxis_ + 1) % 3) != pointingAxis_ )
            sine = -sine;

        //hence get the actual angle
        newAngle = atan2( sine, cosine );
    }

    //If we have a limit, apply it
    if( limitAngles_ )
    {
        if( newAngle > maxAngle_.asScalar() )
            newAngle = maxAngle_.asScalar();
        else if( newAngle < minAngle_.asScalar() )
            newAngle = minAngle_.asScalar();
    }

    //Compute the change in angle. Ensure we use the shortest route (<= pi).
    MATHEX_SCALAR deltaAngle = newAngle - lastAngle_.asScalar();
    const MATHEX_SCALAR pi = Mathex::PI;
    const MATHEX_SCALAR twoPi = pi * 2.0;
    if( deltaAngle > pi )
        deltaAngle -= twoPi;
    else if( deltaAngle < -pi )
        deltaAngle += twoPi;

    bool negative = deltaAngle < 0.0;
    if( negative )
        deltaAngle = -deltaAngle;

    //Now constrain the new angle based on the turn rate
    PhysAbsoluteTime now = W4dManager::instance().time();
    MATHEX_SCALAR maxDeltaAngle = turnRate_.asScalar() * (now - lastUpdateTime_);
    if( maxDeltaAngle < 0.0 )
        maxDeltaAngle = 0.0;

    if( deltaAngle > maxDeltaAngle )
        deltaAngle = maxDeltaAngle;

    if( negative )
        deltaAngle = -deltaAngle;

    newAngle = lastAngle_.asScalar() + deltaAngle;

    //Ensure the new angle lies in the range [-pi,pi]
    if( newAngle > pi )
        newAngle -= twoPi;
    else if( newAngle <= -pi )
        newAngle += twoPi;

    MexRadians theAngle( newAngle );

    //Construct a transform for the rotation based on the angle
    MexEulerAngles angles;
    switch( turningAxis_ )
    {
        case W4d::X_AXIS: angles.roll( theAngle ); break;
        case W4d::Y_AXIS: angles.elevation( theAngle ); break;
        case W4d::Z_AXIS: angles.azimuth( theAngle ); break;
    }

    MexTransform3d turnTransform( angles );
    baseTransform_.transform( turnTransform, pResult );

    //Store the current time and angle, unless doing a look-ahead calculation
    if( not w4dManager.hasArtificialTime() )
    {
        nonConstThis->lastUpdateTime_ = now;
        nonConstThis->lastAngle_ = theAngle;
    }
}

void W4dAxisTrackerPlan::targetObject( const W4dEntity& targetObject )
{
    pTargetObject_ = &targetObject;
}

//virtual
MexPoint3d W4dAxisTrackerPlan::targetOffset( const W4dEntity& targetObject ) const
{
    //Get the bounding volume, using composite inclusive one if appropriate.
    const MexAlignedBox3d& boundingVolume =
        (targetObject.isComposite() ? targetObject.asComposite().compositeBoundingVolume()
                                    : targetObject.boundingVolume() );

    // hence compute and return the centroid
	// NOTE - multiple exit points used here for efficiency - uses RCO.
	if( boundingVolume.minCorner().z() >= 0.0 )
        return boundingVolume.centroid();
	else
	{
		// whoah! This fella is actually (partially) below the ground! We must ensure that
		// we only get the centroid of the portion that is above the ground.
		MexAlignedBox3d aboveGroundBox( boundingVolume );
		MexPoint3d newMinCorner( aboveGroundBox.minCorner().x(), aboveGroundBox.minCorner().y(), 0.0 );
		aboveGroundBox.minCorner( newMinCorner );
		return aboveGroundBox.centroid();
	}
}

void W4dAxisTrackerPlan::forceCurrentAngle()
{
    //Store the current angle and time
    lastAngle_ = currentAngle();
    lastUpdateTime_ = W4dManager::instance().time();
}

MexRadians W4dAxisTrackerPlan::currentAngle()
{
    //Compute the difference transform between current transform and the base transform
    MexTransform3d differenceTransform;
    baseTransform_.transformInverse( pTurningObject_->localTransform(), &differenceTransform );

    //Convert this to euler angles. Hence get the rotation angle. Note that in general the
    //transform may not be a simple rotation about the turning axis. But that's the client's
    //problem.
    MexEulerAngles angles;
    differenceTransform.rotation( &angles );

    MexRadians angle;
    switch( turningAxis_ )
    {
    case W4d::X_AXIS: angle = angles.roll(); break;
    case W4d::Y_AXIS: angle = angles.elevation(); break;
    case W4d::Z_AXIS: angle = angles.azimuth(); break;
    DEFAULT_ASSERT_BAD_CASE( turningAxis_ )
    }

    return angle;
}

void W4dAxisTrackerPlan::setLimits( MexRadians minAngle, MexRadians maxAngle )
{
    PRE( minAngle.asScalar() <= maxAngle.asScalar() );

    minAngle_ = minAngle;
    maxAngle_ = maxAngle;
    limitAngles_ = true;
}

void W4dAxisTrackerPlan::nFramesPerTargetOffsetUpdate( uint nFrames )
{
    PRE( nFrames != 0 );

    //Store the value
    nFramesPerTargetOffsetUpdate_ = nFrames;

    //Get an update now
    targetOffset_ = targetOffset( *pTargetObject_ );
}

bool W4dAxisTrackerPlan::nearLimits( MexRadians tolerance ) const
{
    //Can't be near limits unless we have some
    bool result = limitAngles_;

    if( result )
    {
        //We do, so check within tolerance of max or min
        MATHEX_SCALAR angle = lastAngle_.asScalar();
        MATHEX_SCALAR maxDiff = tolerance.asScalar();
        result = (Mathex::abs( angle - minAngle_.asScalar() ) < maxDiff)  or
                 (Mathex::abs( angle - maxAngle_.asScalar() ) < maxDiff);
    }

    return result;
}

void W4dAxisTrackerPlan::baseTransform( const MexTransform3d& transform )
{
    baseTransform_ = transform;
}
/* End AXISTRAK.CPP *************************************************/

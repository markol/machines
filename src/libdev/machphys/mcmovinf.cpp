/*
 * M C M O V I N F . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <numeric>

#include "machphys/mcmovinf.hpp"
#include "mathex/line2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/utility.hpp"
#include "mathex/quatern.hpp"
#include "utility/indent.hpp"

#ifndef _INLINE
    #include "machphys/mcmovinf.ipp"
#endif

//  This function is only here to keep ctl_vector happy
MachPhysMachineMoveInfo::MachPhysMachineMoveInfo()
{
}

MachPhysMachineMoveInfo::MachPhysMachineMoveInfo(
    TransformsPtr transformsPtr,
    RampAccelerationsPtr rampAccelerationsPtr,
    const PhysAbsoluteTime& startTime )
: transformsPtr_( transformsPtr ),
  rampAccelerationsPtr_( rampAccelerationsPtr ),
  startTime_( startTime ),
  turnAngle_( 0.0 )
{
    PRE( transformsPtr->size() > 1 );
    PRE( transformsPtr->size() == rampAccelerationsPtr->size() + 1 );
//    PRE( MexUtility::inStraightLineXY( *transformsPtr ) );

    totalTime_ = 0.0;

    for( size_t i = 0; i < rampAccelerationsPtr->size(); ++i )
        totalTime_ += (*rampAccelerationsPtr)[ i ].totalTime();

    needsToTurn_ = (*transformsPtr)[ 0 ].position() == (*transformsPtr)[ 1 ].position();

    if( needsToTurn_ )
        turnAngle_ = (*transformsPtr)[ 0 ].xBasis().signedAngleBetween( (*transformsPtr)[ 1 ].xBasis() );

    TEST_INVARIANT;
}

MachPhysMachineMoveInfo::~MachPhysMachineMoveInfo()
{
    TEST_INVARIANT;

}

PhysRelativeTime    MachPhysMachineMoveInfo::totalTime() const
{
    return totalTime_;
}

void MachPhysMachineMoveInfo::CLASS_INVARIANT
{
    INVARIANT( this != NULL );

    //  Check that we aren't trying to go up any vertical walls - look for adjacent
    //  positions with the same x, y coordinates but different z coordinates

    MexPoint3d  lastPoint = transformsPtr_->front().position();

    for( Transforms::const_iterator i = transformsPtr_->begin(); i != transformsPtr_->end(); ++i )
    {
        MexPoint3d  point = (*i).position();

        MATHEX_SCALAR sqrXyDistance = MexPoint2d( lastPoint ).sqrEuclidianDistance( point );

        INVARIANT_INFO( lastPoint );
        INVARIANT_INFO( point );

        INVARIANT( implies(
          sqrXyDistance < MexEpsilon::instance() * MexEpsilon::instance(),
          fabs( lastPoint.z() - point.z() ) < (MATHEX_SCALAR)MexEpsilon::instance() ) );

        lastPoint = point;
    }
}

//  Returns an acceleration profile which averages out correctly
//  for the translation part of the motion.
PhysRampAcceleration    MachPhysMachineMoveInfo::averageProfile() const
{
    size_t  translationStartIndex = 0;

    if( needsToTurn() )
        translationStartIndex = 1;

    PhysRelativeTime    translationTime = totalTime() - turnTime();

    MATHEX_SCALAR   distance = 0.0;

    MATHEX_SCALAR accelerationRate = (*rampAccelerationsPtr_)[ translationStartIndex ].firstAcceleration();
    MATHEX_SCALAR decelerationRate = (*rampAccelerationsPtr_)[ translationStartIndex ].firstAcceleration();
    MATHEX_SCALAR maxSpeed = (*rampAccelerationsPtr_)[ translationStartIndex ].startSpeed();

    for( size_t i = translationStartIndex; i < rampAccelerationsPtr_->size(); ++i )
    {
        const PhysRampAcceleration& acceleration = (*rampAccelerationsPtr_)[ i ];

        accelerationRate = std::max( accelerationRate, acceleration.firstAcceleration() );
        accelerationRate = std::max( accelerationRate, acceleration.secondAcceleration() );

        decelerationRate = std::min( decelerationRate, acceleration.firstAcceleration() );
        decelerationRate = std::min( decelerationRate, acceleration.secondAcceleration() );

        maxSpeed = std::max( maxSpeed, (*rampAccelerationsPtr_)[ translationStartIndex ].startSpeed() );
        maxSpeed = std::max( maxSpeed, (*rampAccelerationsPtr_)[ translationStartIndex ].endSpeed() );
        maxSpeed = std::max( maxSpeed, (*rampAccelerationsPtr_)[ translationStartIndex ].constantSpeed() );

        distance += acceleration.totalDistance();
    }

    MATHEX_SCALAR startSpeed = (*rampAccelerationsPtr_)[ translationStartIndex ].startSpeed();
    MATHEX_SCALAR finishSpeed = rampAccelerationsPtr_->back().endSpeed();

    //  Take small rounding errors into account
    if( finishSpeed < 0.0 and finishSpeed > -MexEpsilon::instance() )
        finishSpeed = 0.0;

    PhysRampAcceleration    acceleration(
      distance,
      startSpeed,
      maxSpeed,
      finishSpeed,
      translationTime,
      fabs( accelerationRate ),
      fabs( decelerationRate ),
      PhysRampAcceleration::BY_DISTANCE_TIME );

    POST( fabs( acceleration.totalDistance() - distance ) < MexEpsilon::instance() );

    return acceleration;
}

ostream& operator <<( ostream& o, const MachPhysMachineMoveInfo& t )
{
    o << "MachPhysMachineMoveInfo " << (void*)&t << " start" << std::endl;

    UtlIndentOstream ostr2( o, "  " );
    UtlIndentOstream ostr4( o, "    " );

    ostr2 << "transforms:\n";
    const MachPhysMachineMoveInfo::Transforms& transforms = *(t.transformsPtr_);
    for( size_t i = 0; i < transforms.size(); ++i )
        ostr4 << transforms[ i ] << std::endl;

    ostr2 << "ramp accelerations:\n";
    const MachPhysMachineMoveInfo::RampAccelerations& rampAccelerations = *(t.rampAccelerationsPtr_);
    for( size_t i = 0; i < rampAccelerations.size(); ++i )
        ostr4 << rampAccelerations[ i ] << std::endl;

    ostr2 << "start time: " << t.startTime_ << std::endl;
    ostr2 << "turn angle: " << t.turnAngle_ << std::endl;
    ostr2 << ( t.needsToTurn_ ? "needs to turn" : "doesn't need to turn" ) << std::endl;
    ostr2 << "total time: " << t.totalTime_ << std::endl;
    ostr2 << "finish time: " << t.startTime_  + t.totalTime_ << std::endl;

    o << "MachPhysMachineMoveInfo " << (void*)&t << " end" << std::endl;

    return o;
}

//  Note that the time is an _absolute_ time, i.e. it is
// _not_ relative to the beginning of this motion.
MexTransform3d MachPhysMachineMoveInfo::transform( PhysAbsoluteTime time ) const
{
    PhysRelativeTime relativeTime = time - startTime();

    MexTransform3d result;

    if( relativeTime < 0 )
    {
        result = startTransform();
    }
    else if( relativeTime >= totalTime() )
    {
        result = endTransform();
    }
    else
    {
        bool    foundSegment = false;
        size_t  segmentIndex = 0;
        MATHEX_SCALAR proportionOfDistance = 0;

        for( size_t i = 0; i != (*rampAccelerationsPtr_).size() and not foundSegment; ++i )
        {
            const PhysRampAcceleration& acceleration = (*rampAccelerationsPtr_)[ i ];

            if( relativeTime < acceleration.totalTime() )
            {
                foundSegment = true;
                segmentIndex = i;
                proportionOfDistance = acceleration.proportionOfDistance( relativeTime );
            }

            relativeTime -= acceleration.totalTime();
        }

        ASSERT( foundSegment, "" );

        const PhysRampAcceleration& acceleration = (*rampAccelerationsPtr_)[ segmentIndex ];
        const MexTransform3d& tx1 = (*transformsPtr_)[ segmentIndex ];
        const MexTransform3d& tx2 = (*transformsPtr_)[ segmentIndex + 1 ];

        if( needsToTurn() and segmentIndex == 0 )
        {
            //  We are on the rotation part of the movement. Interpolate the orientation.
            MexTransform3d differenceTransform;
            tx1.invert( &differenceTransform );
            differenceTransform.transform( tx2 );

            MexQuaternion params;
            differenceTransform.rotation( &params );

            MexVec3 rotationAxis;
            MATHEX_SCALAR angle = params.rotation( &rotationAxis );

            angle *= proportionOfDistance;
            params.set( rotationAxis, angle );

            result = MexTransform3d( params, tx1.position() );
        }
        else
        {
            //  We are on a translation. Interpolate the position.

            result = tx1;
            MexPoint3d newPosition = tx1.position();
            newPosition.interpolate( tx2.position(), proportionOfDistance );
            result.position( newPosition );
        }
    }

    return result;
}

/* End MCMOVINF.CPP *************************************************/

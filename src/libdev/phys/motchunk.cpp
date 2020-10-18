/*
 * M O T C H U N K . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

//#include "stdlib/algorith.hpp"

//#include "mathex/epsilon.hpp"
#include "mathex/sausag2d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/circle2d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/quadrati.hpp"

#include "phys/motchunk.hpp"
#include "phys/cmotchun.hpp"

#ifndef _INLINE
    #include "phys/motchunk.ipp"
#endif

PhysMotionChunk::PhysMotionChunk()
{
    ASSERT_FAIL( "This should never be called" );
}

PhysMotionChunk::PhysMotionChunk
(
    const MexPoint2d& startPoint,
    const MexPoint2d& endPoint,
    MATHEX_SCALAR clearance,
    const PhysRampAcceleration& motionProfile,
    const PhysAbsoluteTime& createTime,
    const PhysRelativeTime& motionTimeOffset,
    const MexDouble& minHeight,
    const MexDouble& maxHeight
)
:   startPoint_( startPoint ),
    endPoint_( endPoint ),
    motionProfile_( motionProfile ),
    createTime_( createTime ),
    motionTimeOffset_( motionTimeOffset ),
    clearance_( clearance ),
    hasExpiryTime_( false ),
    minHeight_( minHeight ),
    maxHeight_( maxHeight )
{
    PRE( clearance >= 0.0 );
    PRE( motionTimeOffset >= 0.0 );
    PRE_INFO( motionProfile.distance( motionProfile.totalTime() ) );
    PRE_INFO( startPoint.euclidianDistance( endPoint ) );
    PRE_INFO( minHeight );
    PRE_INFO( maxHeight );
    PRE( minHeight <= maxHeight );

    //  Commented out because this is not applicable to 3D moves. We will need a
    //  more sophisticated precondition for 3D.

//     PRE( fabs( motionProfile.distance( motionProfile.totalTime() ) -
//                                        startPoint.euclidianDistance( endPoint ) )
//          < MexEpsilon::instance() );

    TEST_INVARIANT;
}

PhysMotionChunk::~PhysMotionChunk()
{
    TEST_INVARIANT;

}

void PhysMotionChunk::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysMotionChunk& t )
{

    o << "PhysMotionChunk " << (void*)&t << " start" << std::endl;
    o << "  from " << t.startPoint_ << " to " << t.endPoint_ << " at " << t.createTime_ << std::endl;
    o << "  motion time offset " << t.motionTimeOffset_ << "  clearance " << t.clearance_ << std::endl;
    if( t.hasExpiryTime_ )
        o << "  expires at " << t.expiryTime_ << std::endl;
    o << "  start speed " << t.motionProfile_.startSpeed() << std::endl;
    o << "  accelerations " << t.motionProfile_.firstAcceleration() << "  "
      << t.motionProfile_.secondAcceleration();
    o << "  times " << t.motionProfile_.totalTime() << " "
                    << t.motionProfile_.firstAccelerationTime() << " "
                    << t.motionProfile_.secondAccelerationTime() << std::endl;
    o << t.motionProfile_;
    o << "  clearance " << t.clearance_ << std::endl;
    o << "PhysMotionChunk " << (void*)&t << " end" << std::endl;

    return o;
}

void PhysMotionChunk::expiryTime( const PhysAbsoluteTime& expiryTime )
{
    PRE( expiryTime >= createTime() );

    hasExpiryTime_ = true;
    expiryTime_ = expiryTime;

	POST( hasExpiryTime() );
}

void PhysMotionChunk::boundary( MexAlignedBox2d* pBox ) const
{
    MATHEX_SCALAR x1 = startPoint_.x();
    MATHEX_SCALAR y1 = startPoint_.y();
    MATHEX_SCALAR x2 = endPoint_.x();
    MATHEX_SCALAR y2 = endPoint_.y();

    MATHEX_SCALAR xMin, yMin, xMax, yMax;
    if( x1 <= x2 )
    {
        xMin = x1;
        xMax = x2;
    }
    else
    {
        xMin = x2;
        xMax = x1;
    }

    if( y1 <= y2 )
    {
        yMin = y1;
        yMax = y2;
    }
    else
    {
        yMin = y2;
        yMax = y1;
    }

    pBox->corners( MexPoint2d( xMin - clearance_, yMin - clearance_ ),
                   MexPoint2d( xMax + clearance_, yMax + clearance_ ) );
}

bool PhysMotionChunk::intersects
(
    const PhysMotionChunk& rhs, PhysAbsoluteTime* pCollideTime
) const
{
    PRE( pCollideTime != NULL );

    MATHEX_SCALAR epsilon = MexEpsilon::instance();

    //Check there is a time overlap
    bool result = (not hasExpiryTime_  or  expiryTime_ > rhs.createTime_) and
                  (not rhs.hasExpiryTime_ or rhs.expiryTime_ > createTime_);

    //  Check that there is a height overlap
    if( result )
    {
        if( minHeight_ > rhs.maxHeight_ or
          rhs.minHeight_ > maxHeight_ )
        {
            result = false;
        }
    }

    //If we have a potential collision perform more detailed checks.
    //This loop is only executed once.
    while( result )
    {
        //Construct sausages for the full spatial extent of each chunk, and check
        //for any overlap
        MexSausage2d lhsFullSausage( startPoint_, endPoint_, clearance_ );
        MexSausage2d rhsFullSausage( rhs.startPoint_, rhs.endPoint_, rhs.clearance_ );
        result = lhsFullSausage.intersects( rhsFullSausage );
        if( not result )
            break;

        //Extract motion profile times for each chunk
        PhysRelativeTime lhsAccn1Duration = motionProfile_.firstAccelerationTime();
        PhysRelativeTime lhsAccn2Duration = motionProfile_.secondAccelerationTime();
        PhysRelativeTime lhsMotionDuration = motionProfile_.totalTime();

        PhysRelativeTime rhsAccn1Duration = rhs.motionProfile_.firstAccelerationTime();
        PhysRelativeTime rhsAccn2Duration = rhs.motionProfile_.secondAccelerationTime();
        PhysRelativeTime rhsMotionDuration = rhs.motionProfile_.totalTime();

        //Each chunk has 5 sections:
        //  Initial rest phase at start point
        //  First acceleration
        //  Constant speed section
        //  Second acceleration( usually deceleration)
        //  Final rest phase at end point
        //Compute start and end times for each section
        PhysAbsoluteTime lhsInitialStartTime = createTime_;
        PhysAbsoluteTime lhsAccn1StartTime = createTime_ + motionTimeOffset_;
        PhysAbsoluteTime lhsConstantSpeedStartTime = lhsAccn1StartTime + lhsAccn1Duration;
        PhysAbsoluteTime lhsFinalStartTime = lhsAccn1StartTime + lhsMotionDuration;
        PhysAbsoluteTime lhsAccn2StartTime = lhsFinalStartTime - lhsAccn2Duration;
        PhysAbsoluteTime lhsEndTime;
        if( hasExpiryTime_ )
            lhsEndTime = expiryTime_;

        PhysAbsoluteTime rhsInitialStartTime = rhs.createTime_;
        PhysAbsoluteTime rhsAccn1StartTime = rhs.createTime_ + rhs.motionTimeOffset_;
        PhysAbsoluteTime rhsConstantSpeedStartTime = rhsAccn1StartTime + rhsAccn1Duration;
        PhysAbsoluteTime rhsFinalStartTime = rhsAccn1StartTime + rhsMotionDuration;
        PhysAbsoluteTime rhsAccn2StartTime =rhsFinalStartTime - rhsAccn2Duration;
        PhysAbsoluteTime rhsEndTime;
        if( rhs.hasExpiryTime_ )
            rhsEndTime = rhs.expiryTime_;

        //Set flags indicating whether the duration of each section is not negligible
        bool lhsHasInitial = motionTimeOffset_ > epsilon;
        bool lhsHasAccn1 = lhsAccn1Duration > epsilon;
        bool lhsHasConstantSpeed
            = (lhsMotionDuration - lhsAccn1Duration - lhsAccn2Duration) > epsilon;
        bool lhsHasAccn2 = lhsAccn2Duration > epsilon;
        bool lhsHasFinal = not hasExpiryTime_  or  (lhsEndTime - lhsFinalStartTime) > epsilon;

        bool rhsHasInitial = rhs.motionTimeOffset_ > epsilon;
        bool rhsHasAccn1 = rhsAccn1Duration > epsilon;
        bool rhsHasConstantSpeed
            = (rhsMotionDuration - rhsAccn1Duration - rhsAccn2Duration) > epsilon;
        bool rhsHasAccn2 = rhsAccn2Duration > epsilon;
        bool rhsHasFinal = not rhs.hasExpiryTime_  or  (rhsEndTime - rhsFinalStartTime) > epsilon;

        //Construct points at the start and end of the constant velocity section for
        //each chunk. Note the use of defaults in case negligble acceleration.
        MexPoint2d lhsConstantSpeedStartPoint( startPoint_ );
        if( lhsHasAccn1 )
        {
            MATHEX_SCALAR f = motionProfile_.proportionOfDistance( lhsAccn1Duration );
            lhsConstantSpeedStartPoint = MexPoint2d( startPoint_, endPoint_, f );
        }

        MexPoint2d lhsConstantSpeedEndPoint( endPoint_ );
        if( lhsHasAccn2 )
        {
            MATHEX_SCALAR f =
                motionProfile_.proportionOfDistance( lhsMotionDuration - lhsAccn2Duration );
            lhsConstantSpeedEndPoint = MexPoint2d( startPoint_, endPoint_, f );
        }

        MexPoint2d rhsConstantSpeedStartPoint( rhs.startPoint_ );
        if( rhsHasAccn1 )
        {
            MATHEX_SCALAR f = rhs.motionProfile_.proportionOfDistance( rhsAccn1Duration );
            rhsConstantSpeedStartPoint = MexPoint2d( rhs.startPoint_, rhs.endPoint_, f );
        }

        MexPoint2d rhsConstantSpeedEndPoint( rhs.endPoint_ );
        if( rhsHasAccn2 )
        {
            MATHEX_SCALAR f =
                rhs.motionProfile_.proportionOfDistance( rhsMotionDuration - rhsAccn2Duration );
            rhsConstantSpeedEndPoint = MexPoint2d( rhs.startPoint_, rhs.endPoint_, f );
        }

        //Construct circles representing initial and final sections for each chunk
        MexCircle2d lhsStartCircle( startPoint_, clearance_ );
        MexCircle2d rhsStartCircle( rhs.startPoint_, rhs.clearance_ );
        MexCircle2d lhsEndCircle( endPoint_, clearance_ );
        MexCircle2d rhsEndCircle( rhs.endPoint_, rhs.clearance_ );

        //Construct first acceleration sausages. Note use of default in case negligble.
        MexSausage2d lhsAccn1Sausage( lhsFullSausage );
        if( lhsHasAccn1)
            lhsAccn1Sausage = MexSausage2d( startPoint_, lhsConstantSpeedStartPoint, clearance_ );

        MexSausage2d rhsAccn1Sausage( rhsFullSausage );
        if( rhsHasAccn1)
            rhsAccn1Sausage =
                MexSausage2d( rhs.startPoint_, rhsConstantSpeedStartPoint, rhs.clearance_ );

        //Construct constant speed sausages.
        MexSausage2d lhsConstantSpeedSausage( lhsFullSausage );
        if( lhsHasConstantSpeed )
            lhsConstantSpeedSausage =
                MexSausage2d( lhsConstantSpeedStartPoint, lhsConstantSpeedEndPoint, clearance_ );

        MexSausage2d rhsConstantSpeedSausage( rhsFullSausage );
        if( rhsHasConstantSpeed )
            rhsConstantSpeedSausage =
                MexSausage2d( rhsConstantSpeedStartPoint, rhsConstantSpeedEndPoint, rhs.clearance_ );

        //Construct second acceleration sausages. Note use of default in case negligble.
        MexSausage2d lhsAccn2Sausage( lhsFullSausage );
        if( lhsHasAccn2)
            lhsAccn2Sausage = MexSausage2d( lhsConstantSpeedEndPoint, endPoint_, clearance_ );

        MexSausage2d rhsAccn2Sausage( rhsFullSausage );
        if( rhsHasAccn2)
            rhsAccn2Sausage =
                MexSausage2d( rhsConstantSpeedEndPoint, rhs.endPoint_, rhs.clearance_ );

        //Now perform checks between each section
        if( lhsHasInitial  and  lhsAccn1StartTime > rhsInitialStartTime  and
            (not rhs.hasExpiryTime_  or  rhsEndTime > lhsInitialStartTime ) )
        {
            if( rhsHasInitial and
                circleIntersectsCircle( lhsStartCircle, lhsInitialStartTime,
                                        true, lhsAccn1StartTime,
                                        rhsStartCircle, rhsInitialStartTime,
                                        true, rhsAccn1StartTime,
                                        pCollideTime ) )
                break;

            if( rhsHasAccn1 and
                circleIntersectsAccelerationSausage( lhsStartCircle, lhsInitialStartTime,
                                                     true, lhsAccn1StartTime,
                                                     rhsAccn1Sausage, rhsAccn1StartTime,
                                                     rhsConstantSpeedStartTime,
                                                     pCollideTime ) )
                break;

            if( rhsHasConstantSpeed and
                circleIntersectsConstantSpeedSausage( lhsStartCircle, lhsInitialStartTime,
                                                      true, lhsAccn1StartTime,
                                                      rhsConstantSpeedSausage,
                                                      rhsConstantSpeedStartTime,
                                                      rhsAccn2StartTime,
                                                      pCollideTime ) )
                break;

            if( rhsHasAccn2 and
                circleIntersectsAccelerationSausage( lhsStartCircle, lhsInitialStartTime,
                                                     true, lhsAccn1StartTime,
                                                     rhsAccn2Sausage, rhsAccn2StartTime,
                                                     rhsFinalStartTime,
                                                     pCollideTime ) )
                break;

            if( rhsHasFinal and
                circleIntersectsCircle( lhsStartCircle, lhsInitialStartTime,
                                        true, lhsAccn1StartTime,
                                        rhsEndCircle, rhsFinalStartTime,
                                        rhs.hasExpiryTime_, rhsEndTime,
                                        pCollideTime ) )
                break;
        }

        if( lhsHasAccn1  and  lhsConstantSpeedStartTime > rhsInitialStartTime  and
            (not rhs.hasExpiryTime_  or  rhsEndTime > lhsAccn1StartTime ) )
        {
            if( rhsHasInitial and
                circleIntersectsAccelerationSausage( rhsStartCircle, rhsInitialStartTime,
                                                     true, rhsAccn1StartTime,
                                                     lhsAccn1Sausage, lhsAccn1StartTime,
                                                     lhsConstantSpeedStartTime,
                                                     pCollideTime ) )
                break;

            if( rhsHasAccn1 and
                accelerationSausageIntersectsAccelerationSausage ( lhsAccn1Sausage,
                                                                   lhsAccn1StartTime,
                                                                   lhsConstantSpeedStartTime,
                                                                   rhsAccn1Sausage,
                                                                   rhsAccn1StartTime,
                                                                   rhsConstantSpeedStartTime,
                                                                   pCollideTime ) )
                break;

            if( rhsHasConstantSpeed and
                accelerationSausageIntersectsConstantSpeedSausage (lhsAccn1Sausage,
                                                                   lhsAccn1StartTime,
                                                                   lhsConstantSpeedStartTime,
                                                                   rhsConstantSpeedSausage,
                                                                   rhsConstantSpeedStartTime,
                                                                   rhsAccn2StartTime,
                                                                   pCollideTime ) )
                break;

            if( rhsHasAccn2 and
                accelerationSausageIntersectsAccelerationSausage ( lhsAccn1Sausage,
                                                                   lhsAccn1StartTime,
                                                                   lhsConstantSpeedStartTime,
                                                                   rhsAccn2Sausage,
                                                                   rhsAccn2StartTime,
                                                                   rhsFinalStartTime,
                                                                   pCollideTime ) )
                break;

            if( rhsHasFinal and
                circleIntersectsAccelerationSausage( rhsEndCircle, rhsFinalStartTime,
                                                     rhs.hasExpiryTime_, rhsEndTime,
                                                     lhsAccn1Sausage, lhsAccn1StartTime,
                                                     lhsConstantSpeedStartTime,
                                                     pCollideTime ) )
                break;
        }

        if( lhsHasConstantSpeed  and  lhsAccn2StartTime > rhsInitialStartTime  and
            (not rhs.hasExpiryTime_  or  rhsEndTime > lhsConstantSpeedStartTime ) )
        {
            if( rhsHasInitial and
                circleIntersectsConstantSpeedSausage( rhsStartCircle, rhsInitialStartTime,
                                                     true, rhsAccn1StartTime,
                                                     lhsConstantSpeedSausage,
                                                     lhsConstantSpeedStartTime,
                                                     lhsAccn2StartTime,
                                                     pCollideTime ) )
                break;

            if( rhsHasAccn1 and
                accelerationSausageIntersectsConstantSpeedSausage ( rhsAccn1Sausage,
                                                                    rhsAccn1StartTime,
                                                                    rhsConstantSpeedStartTime,
                                                                    lhsConstantSpeedSausage,
                                                                    lhsConstantSpeedStartTime,
                                                                    lhsAccn2StartTime,
                                                                    pCollideTime ) )
                break;

            if( rhsHasConstantSpeed and
                constantSpeedSausageIntersectsConstantSpeedSausage (lhsConstantSpeedSausage,
                                                                    lhsConstantSpeedStartTime,
                                                                    lhsAccn2StartTime,
                                                                    rhsConstantSpeedSausage,
                                                                    rhsConstantSpeedStartTime,
                                                                    rhsAccn2StartTime,
                                                                    pCollideTime ) )
                break;

            if( rhsHasAccn2 and
                accelerationSausageIntersectsConstantSpeedSausage ( rhsAccn2Sausage,
                                                                    rhsAccn2StartTime,
                                                                    rhsFinalStartTime,
                                                                    lhsConstantSpeedSausage,
                                                                    lhsConstantSpeedStartTime,
                                                                    lhsAccn2StartTime,
                                                                    pCollideTime ) )
                break;

            if( rhsHasFinal and
                circleIntersectsConstantSpeedSausage( rhsEndCircle, rhsFinalStartTime,
                                                      rhs.hasExpiryTime_, rhsEndTime,
                                                      lhsConstantSpeedSausage,
                                                      lhsConstantSpeedStartTime,
                                                      lhsAccn2StartTime,
                                                      pCollideTime ) )
                break;
        }

        if( lhsHasAccn2  and  lhsFinalStartTime > rhsInitialStartTime  and
            (not rhs.hasExpiryTime_  or  rhsEndTime > lhsAccn2StartTime ) )
        {
            if( rhsHasInitial and
                circleIntersectsAccelerationSausage( rhsStartCircle, rhsInitialStartTime,
                                                     true, rhsAccn1StartTime,
                                                     lhsAccn2Sausage, lhsAccn2StartTime,
                                                     lhsFinalStartTime,
                                                     pCollideTime ) )
                break;

            if( rhsHasAccn1 and
                accelerationSausageIntersectsAccelerationSausage ( lhsAccn2Sausage,
                                                                   lhsAccn2StartTime,
                                                                   lhsFinalStartTime,
                                                                   rhsAccn1Sausage,
                                                                   rhsAccn1StartTime,
                                                                   rhsConstantSpeedStartTime,
                                                                   pCollideTime ) )
                break;

            if( rhsHasConstantSpeed and
                accelerationSausageIntersectsConstantSpeedSausage (lhsAccn2Sausage,
                                                                   lhsAccn2StartTime,
                                                                   lhsFinalStartTime,
                                                                   rhsConstantSpeedSausage,
                                                                   rhsConstantSpeedStartTime,
                                                                   rhsAccn2StartTime,
                                                                   pCollideTime ) )
                break;

            if( rhsHasAccn2 and
                accelerationSausageIntersectsAccelerationSausage ( lhsAccn2Sausage,
                                                                   lhsAccn2StartTime,
                                                                   lhsFinalStartTime,
                                                                   rhsAccn2Sausage,
                                                                   rhsAccn2StartTime,
                                                                   rhsFinalStartTime,
                                                                   pCollideTime ) )
                break;

            if( rhsHasFinal and
                circleIntersectsAccelerationSausage( rhsEndCircle, rhsFinalStartTime,
                                                     rhs.hasExpiryTime_, rhsEndTime,
                                                     lhsAccn2Sausage, lhsAccn2StartTime,
                                                     lhsFinalStartTime,
                                                     pCollideTime ) )
                break;
        }

        if( lhsHasFinal  and
            (not hasExpiryTime_ or lhsEndTime > rhsInitialStartTime)  and
            (not rhs.hasExpiryTime_  or  rhsEndTime > lhsFinalStartTime ) )
        {
            if( rhsHasInitial and
                circleIntersectsCircle( lhsEndCircle, lhsFinalStartTime,
                                        hasExpiryTime_, lhsEndTime,
                                        rhsStartCircle, rhsInitialStartTime,
                                        true, rhsAccn1StartTime,
                                        pCollideTime ) )
                break;

            if( rhsHasAccn1 and
                circleIntersectsAccelerationSausage( lhsEndCircle, lhsFinalStartTime,
                                                     hasExpiryTime_, lhsEndTime,
                                                     rhsAccn1Sausage, rhsAccn1StartTime,
                                                     rhsConstantSpeedStartTime,
                                                     pCollideTime ) )
                break;

            if( rhsHasConstantSpeed and
                circleIntersectsConstantSpeedSausage( lhsEndCircle, lhsFinalStartTime,
                                                      hasExpiryTime_, lhsEndTime,
                                                      rhsConstantSpeedSausage,
                                                      rhsConstantSpeedStartTime,
                                                      rhsAccn2StartTime,
                                                      pCollideTime ) )
                break;

            if( rhsHasAccn2 and
                circleIntersectsAccelerationSausage( lhsEndCircle, lhsFinalStartTime,
                                                     hasExpiryTime_, lhsEndTime,
                                                     rhsAccn2Sausage, rhsAccn2StartTime,
                                                     rhsFinalStartTime,
                                                     pCollideTime ) )
                break;

            if( rhsHasFinal and
                circleIntersectsCircle( lhsEndCircle, lhsFinalStartTime,
                                        hasExpiryTime_, lhsEndTime,
                                        rhsEndCircle, rhsFinalStartTime,
                                        rhs.hasExpiryTime_, rhsEndTime,
                                        pCollideTime ) )
                break;
        }

        //No intersection
        result = false;
    }

    return result;
}


bool PhysMotionChunk::circleIntersectsCircle
(
    const MexCircle2d& lhsCircle,
    const PhysAbsoluteTime& lhsStartTime,
    bool lhsHasEndTime,
    const PhysAbsoluteTime& lhsEndTime,
    const MexCircle2d& rhsCircle,
    const PhysAbsoluteTime& rhsStartTime,
    bool rhsHasEndTime,
    const PhysAbsoluteTime& rhsEndTime,
    PhysAbsoluteTime* pCollideTime
)
{
    //Check the times overlap
    bool result = (not lhsHasEndTime  or  lhsEndTime > rhsStartTime) and
                  (not rhsHasEndTime  or  rhsEndTime > lhsStartTime);

    //If so, check for spatial overlap
    if( result )
    {
        result = lhsCircle.intersects( rhsCircle );
        if( result )
            *pCollideTime = std::max( lhsStartTime, rhsStartTime );
    }

    return result;
}

bool PhysMotionChunk::circleIntersectsAccelerationSausage
(
    const MexCircle2d& lhsCircle,
    const PhysAbsoluteTime& lhsStartTime,
    bool lhsHasEndTime,
    const PhysAbsoluteTime& lhsEndTime,
    const MexSausage2d& rhsSausage,
    const PhysAbsoluteTime& rhsStartTime,
    const PhysAbsoluteTime& rhsEndTime,
    PhysAbsoluteTime* pCollideTime
)
{
    //Check the times overlap
    bool result = (not lhsHasEndTime  or  lhsEndTime > rhsStartTime) and
                  (rhsEndTime > lhsStartTime);

    //If so, check for spatial overlap
    if( result )
    {
        result = rhsSausage.intersects( lhsCircle );
        if( result )
            *pCollideTime = std::max( lhsStartTime, rhsStartTime );
    }

    return result;
}

bool PhysMotionChunk::circleIntersectsConstantSpeedSausage
(
    const MexCircle2d& lhsCircle,
    const PhysAbsoluteTime& lhsStartTime,
    bool lhsHasEndTime,
    const PhysAbsoluteTime& lhsEndTime,
    const MexSausage2d& rhsSausage,
    const PhysAbsoluteTime& rhsStartTime,
    const PhysAbsoluteTime& rhsEndTime,
    PhysAbsoluteTime* pCollideTime
)
{
    //Check the times overlap
    PhysAbsoluteTime overlapStartTime = std::max( lhsStartTime, rhsStartTime );
    PhysAbsoluteTime overlapEndTime =
        ( lhsHasEndTime ? std::min( lhsEndTime, rhsEndTime) : rhsEndTime );

    bool result = overlapStartTime < overlapEndTime;

    //If so, check for any spatial overlap
    if( result )
        result = rhsSausage.intersects( lhsCircle ) ;

    //Do more checks.
    if( result )
    {
        result = false;

        //Get the square of the collision distance
        MATHEX_SCALAR sqrLimit = lhsCircle.radius() + rhsSausage.radius();
        sqrLimit *= sqrLimit;

        //Now do the more detailed time based check.
        //Construct coefficients for the quadratic in time which we need to solve.
        //Note that we temporarily treat rhsStartTime as zero to reduce the magnitude
        //of time quantities.
        MexVec2 a( lhsCircle.centre(), rhsSausage.startPoint() );
        MATHEX_SCALAR aSqr = a.squareModulus();
        MexVec2 b( lhsCircle.centre(), rhsSausage.endPoint() );
        MATHEX_SCALAR bSqr = b.squareModulus();
        MATHEX_SCALAR ab = a.dotProduct( b );
        MATHEX_SCALAR tor = rhsEndTime - rhsStartTime;

        MATHEX_SCALAR aCoeff = aSqr + bSqr - 2.0 * ab;
        MATHEX_SCALAR bCoeff = 2.0 * tor * (ab - aSqr);
        MATHEX_SCALAR cCoeff = (aSqr - sqrLimit) * tor * tor;

        //Get the solutions (guaranteed in ascending order)
        MATHEX_SCALAR roots[2];
        int nRoots = MexQuadratic::solve( aCoeff, bCoeff, cCoeff, roots );
        for( size_t i = 0; i!= nRoots; ++i )
            roots[i] += rhsStartTime;

        //Action depends on number of roots
        if( nRoots == 2 )
        {
            //Intersection occurs from roots[0] to roots[1]
            //Check for any time overlap.
            if( roots[1] >= overlapStartTime  and  overlapEndTime >= roots[0] )
            {
                result = true;
                *pCollideTime = std::max( roots[0], overlapStartTime );
            }
        }
        else if( nRoots ==1 )
        {
            //Intersection occurs at roots[0].
            if( roots[0] >= overlapStartTime  and  overlapEndTime >= roots[0] )
            {
                result = true;
                *pCollideTime = roots[0];
            }
        }
    }

    return result;
}

bool PhysMotionChunk::accelerationSausageIntersectsAccelerationSausage
(
    const MexSausage2d& lhsSausage,
    const PhysAbsoluteTime& lhsStartTime,
    const PhysAbsoluteTime& lhsEndTime,
    const MexSausage2d& rhsSausage,
    const PhysAbsoluteTime& rhsStartTime,
    const PhysAbsoluteTime& rhsEndTime,
    PhysAbsoluteTime* pCollideTime
)
{
    //Check the times overlap
    bool result = lhsEndTime > rhsStartTime  and  rhsEndTime > lhsStartTime;

    //If so, check for spatial overlap
    if( result )
    {
        result = rhsSausage.intersects( lhsSausage );
        if( result )
            *pCollideTime = std::max( lhsStartTime, rhsStartTime );
    }

    return result;
}

bool PhysMotionChunk::accelerationSausageIntersectsConstantSpeedSausage
(
    const MexSausage2d& lhsAccnSausage,
    const PhysAbsoluteTime& lhsStartTime,
    const PhysAbsoluteTime& lhsEndTime,
    const MexSausage2d& rhsCSSausage,
    const PhysAbsoluteTime& rhsStartTime,
    const PhysAbsoluteTime& rhsEndTime,
    PhysAbsoluteTime* pCollideTime
)
{
    //Check the times overlap, and sausages overlap
    MATHEX_SCALAR epsilon = MexEpsilon::instance();
    PhysAbsoluteTime overlapStartTime = std::max( lhsStartTime, rhsStartTime );
    PhysAbsoluteTime overlapEndTime = std::min( lhsEndTime, rhsEndTime);

    bool result = (overlapEndTime - overlapStartTime) > epsilon  and
                  rhsCSSausage.intersects( lhsAccnSausage ) ;

    //If so, do more detailed check
    if( result )
    {
        //Now construct a sausage consisting of the section of the constant speed
        //sausage during the overlap time.
        MATHEX_SCALAR rhsDuration = rhsEndTime - rhsStartTime;
        MATHEX_SCALAR f = (overlapStartTime - rhsStartTime) / rhsDuration;
        MATHEX_SCALAR g = (overlapEndTime - rhsStartTime) / rhsDuration;

        const MexPoint2d& rhsStartPoint = rhsCSSausage.startPoint();
        const MexPoint2d& rhsEndPoint = rhsCSSausage.endPoint();
        MexPoint2d overlapStartPoint( rhsStartPoint, rhsEndPoint, f );
        MexPoint2d overlapEndPoint( rhsStartPoint, rhsEndPoint, g );

        //Check for overlap
        MexSausage2d overlapCSSausage( overlapStartPoint, overlapEndPoint, rhsCSSausage.radius() );
        result = overlapCSSausage.intersects( lhsAccnSausage );
        if( result )
            *pCollideTime = overlapStartTime;
    }

    return result;
}

bool PhysMotionChunk::constantSpeedSausageIntersectsConstantSpeedSausage
(
    const MexSausage2d& lhsSausage,
    const PhysAbsoluteTime& lhsStartTime,
    const PhysAbsoluteTime& lhsEndTime,
    const MexSausage2d& rhsSausage,
    const PhysAbsoluteTime& rhsStartTime,
    const PhysAbsoluteTime& rhsEndTime,
    PhysAbsoluteTime* pCollideTime
)
{
    MATHEX_SCALAR epsilon = MexEpsilon::instance();
    PRE( (lhsEndTime - lhsStartTime) > epsilon );
    PRE( (rhsEndTime - rhsStartTime) > epsilon );

    //Check the times overlap
    PhysAbsoluteTime overlapStartTime = std::max( lhsStartTime, rhsStartTime );
    PhysAbsoluteTime overlapEndTime = std::min( lhsEndTime, rhsEndTime);

    bool result = (overlapEndTime - overlapStartTime) > epsilon;

    //If so, check for any spatial overlap
    if( result )
        result = rhsSausage.intersects( lhsSausage ) ;

    //Do more checks.
    //This loop only executed once
    while( result )
    {
        //Adjust the times so that they are relative to start of overlap time.
        //This reduces potential rounding errors as times become large
        MATHEX_SCALAR s1 = lhsStartTime - overlapStartTime;
        MATHEX_SCALAR e1 = lhsEndTime - overlapStartTime;
        MATHEX_SCALAR s2 = rhsStartTime - overlapStartTime;
        MATHEX_SCALAR e2 = rhsEndTime - overlapStartTime;

        //Compute scalars used to construct equation vectors
        MATHEX_SCALAR f1 = 1.0 / (e1 - s1);
        MATHEX_SCALAR f2 = 1.0 / (e2 - s2);
        const MexPoint2d& p1 = lhsSausage.startPoint();
        const MexPoint2d& q1 = lhsSausage.endPoint();
        const MexPoint2d& p2 = rhsSausage.startPoint();
        const MexPoint2d& q2 = rhsSausage.endPoint();
        MATHEX_SCALAR p1xf1 = p1.x() * f1;
        MATHEX_SCALAR p1yf1 = p1.y() * f1;
        MATHEX_SCALAR q1xf1 = q1.x() * f1;
        MATHEX_SCALAR q1yf1 = q1.y() * f1;
        MATHEX_SCALAR p2xf2 = p2.x() * f2;
        MATHEX_SCALAR p2yf2 = p2.y() * f2;
        MATHEX_SCALAR q2xf2 = q2.x() * f2;
        MATHEX_SCALAR q2yf2 = q2.y() * f2;

        //Construct equation vectors
        MexVec2 a( e1 * p1xf1 - s1 * q1xf1 - e2 * p2xf2 + s2 * q2xf2,
                   e1 * p1yf1 - s1 * q1yf1 - e2 * p2yf2 + s2 * q2yf2 );
        MexVec2 b( q1xf1 - p1xf1 - q2xf2 + p2xf2,
                   q1yf1 - p1yf1 - q2yf2 + p2yf2 );


        //Construct coefficients for the quadratic in time which we need to solve.
        MATHEX_SCALAR sqrLimit = lhsSausage.radius() + rhsSausage.radius();
        sqrLimit *= sqrLimit;

        MATHEX_SCALAR aCoeff = b.squareModulus();
        MATHEX_SCALAR bCoeff = 2.0 * a.dotProduct( b );
        MATHEX_SCALAR cCoeff = a.squareModulus() - sqrLimit;

        //Get the solutions (guaranteed in ascending order)
        MATHEX_SCALAR roots[2];
        int nRoots = MexQuadratic::solve( aCoeff, bCoeff, cCoeff, roots );

        //Check for one inside the overlapping time frame
        result = false;
        for( int i = 0; i != nRoots; ++i )
        {
            MATHEX_SCALAR t = roots[i] + overlapStartTime;
            result = t >= overlapStartTime and t <= overlapEndTime;
            if( result )
            {
                *pCollideTime = t;
                break;
            }
        }

        if( result )
            break;
    }

    return result;
}

PhysMotionChunk::PhysMotionChunk( const PhysCompressedMotionChunk& chunk )
:   startPoint_( chunk.startPoint_ ),
    endPoint_( chunk.endPoint_ ),
    motionProfile_( chunk.motionProfile_ ),
    createTime_( chunk.createTime_ ),
    motionTimeOffset_( chunk.motionTimeOffset_ ),
    clearance_( chunk.clearance_ ),
    hasExpiryTime_( chunk.hasExpiryTime_ ),
    minHeight_( chunk.minHeight_ ),
    maxHeight_( chunk.maxHeight_ )
{
}

bool PhysMotionChunk::timeIntersects( PhysAbsoluteTime time ) const
{
	bool result = true;

	if( time < createTime_ )
		result = false;

	if( hasExpiryTime() )
	{
		if( time > expiryTime() )
			result = false;
	}

	return result;
}

bool PhysMotionChunk::timeIntersects( PhysAbsoluteTime startTime, PhysAbsoluteTime endTime ) const
{
	bool result = true;

	if( endTime < createTime_ )
		result = false;
	else
	{
		if( hasExpiryTime() )
		{
			if( startTime > expiryTime() )
				result = false;
		}
	}

	return result;
}

/* End MOTCHUNK.CPP *************************************************/

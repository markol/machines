/*
 * M O T C H U N K . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysMotionChunk

    Represents a chunk of straightline motion for a moving object.
    HAS-A start point and end point
    HAS-A PhysRampAcceleration defining the motion profile between the points
    HAS-A creationTime for the time at which the object is at the start point
    HAS-A time offset from creationTime to the start of the motion
    HAS-A optional expiryTime after which the object vanishes. If none defined,
          considered to remain at the end point.
    HAS-A Clearance defining distance around the object for collision avoidance.

	If you change the data members in this class it is very important to remember to change
	the compressed version as well.

*/

#ifndef _PHYS_MOTCHUNK_HPP
#define _PHYS_MOTCHUNK_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"
#include "mathex/double.hpp"

#include "phys/phys.hpp"
#include "phys/rampacce.hpp"

//Forward declarations
class MexCircle2d;
class MexSausage2d;
class MexAlignedBox2d;
class PhysCompressedMotionChunk;

//Bitwise canonical
class PhysMotionChunk
{
public:
    //  This is suppied purely to keep ctl_vector happy - do not call it.
    PhysMotionChunk();
   
    //ctor. Motion is from startPoint to endPoint with radius of clearance required
    //for collision avoidance.
    //motionProfile defines the speed profile, which start at motionTimeOffset from
    //createTime. Between these 2 times the object is considered to remain at startPoint.
    //After the motion is complete, the object is considered to remain at endPoint.
    PhysMotionChunk
    (
        const MexPoint2d& startPoint,
        const MexPoint2d& endPoint,
        MATHEX_SCALAR clearance,
        const PhysRampAcceleration& motionProfile,
        const PhysAbsoluteTime& createTime,
        const PhysRelativeTime& motionTimeOffset,
        const MexDouble& minHeight,
        const MexDouble& maxHeight
    );
    //PRE( clearance >= 0.0 )
    //PRE( motionTimeOffset >= 0.0 );
    //PRE( motionProfile defines motion covering same distance as that from startPoint to endPoint)

	PhysMotionChunk( const PhysCompressedMotionChunk& );

    //dtor
    ~PhysMotionChunk( void );

    //Set expiry time. After this time the object is considered not to be at any
    //location defined by the motion chunk.
    void expiryTime( const PhysAbsoluteTime& expiryTime );
	//  POST( hasExpiryTime() );

    //Accessors
    const MexPoint2d& startPoint() const;
    const MexPoint2d& endPoint() const;
    MATHEX_SCALAR clearance() const;
    const PhysRampAcceleration& motionProfile() const;
    const PhysAbsoluteTime& createTime() const;
    const PhysRelativeTime& motionTimeOffset() const;
	bool hasExpiryTime() const;
    const PhysAbsoluteTime& expiryTime() const;
	//	PRE( hasExpiryTime() );

    //returns bounding box for the motion chunk in pBox
    void boundary( MexAlignedBox2d* pBox ) const;

    //True iff intersects with rhs, taking time and space into account.
    //If so returns the time at which collision occurs in pCollideTime.
    //Note that no bounding box check is performed - the client is advised to do this first.
    bool intersects( const PhysMotionChunk& rhs, PhysAbsoluteTime* pCollideTime ) const;

	bool timeIntersects( PhysAbsoluteTime ) const;
	bool timeIntersects( PhysAbsoluteTime startTime, PhysAbsoluteTime endTime ) const;

    const MexDouble& minHeight() const;
    const MexDouble& maxHeight() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysMotionChunk& t );

private:
	friend class PhysCompressedMotionChunk;
    // Operation deliberately revoked

    //True iff lhsCircle intersects rhsCircle in both time and space.
    //If so returns earliest collision time in pCollideTime.
    //lhsCircle exists from lhsStartTime, until lhsEndTime (if lhsHasEndTime, or
    //indefinitely otherwise).
    //Similar for rhs.
    static
    bool circleIntersectsCircle
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
    );

    //True iff lhsCircle intersects rhsSausage in both time and space.
    //If so returns earliest collision time in pCollideTime.
    //lhsCircle exists from lhsStartTime, until lhsEndTime (if lhsHasEndTime, or
    //indefinitely otherwise).
    //rhsSausage represents an object accelerating form its start to end point,
    //starting at rhsStartTime, and arriving at rhsEndTime.
    static
    bool circleIntersectsAccelerationSausage
    (
        const MexCircle2d& lhsCircle,
        const PhysAbsoluteTime& lhsStartTime,
        bool lhsHasEndTime,
        const PhysAbsoluteTime& lhsEndTime,
        const MexSausage2d& rhsSausage,
        const PhysAbsoluteTime& rhsStartTime,
        const PhysAbsoluteTime& rhsEndTime,
        PhysAbsoluteTime* pCollideTime
    );

    //True iff lhsCircle intersects rhsSausage in both time and space.
    //If so returns earliest collision time in pCollideTime.
    //lhsCircle exists from lhsStartTime, until lhsEndTime (if lhsHasEndTime, or
    //indefinitely otherwise).
    //rhsSausage represents an object moving at constant speed from its start to end point,
    //starting at rhsStartTime, and arriving at rhsEndTime.
    static
    bool circleIntersectsConstantSpeedSausage
    (
        const MexCircle2d& lhsCircle,
        const PhysAbsoluteTime& lhsStartTime,
        bool lhsHasEndTime,
        const PhysAbsoluteTime& lhsEndTime,
        const MexSausage2d& rhsSausage,
        const PhysAbsoluteTime& rhsStartTime,
        const PhysAbsoluteTime& rhsEndTime,
        PhysAbsoluteTime* pCollideTime
    );

    //True iff lhsSausage intersects rhsSausage in both time and space.
    //If so returns earliest collision time in pCollideTime.
    //lhsSausage represents an object accelerating form its start to end point,
    //starting at lhsStartTime, and arriving at lhsEndTime.
    //Similar for rhsSausage.
    static
    bool accelerationSausageIntersectsAccelerationSausage
    (
        const MexSausage2d& lhsSausage,
        const PhysAbsoluteTime& lhsStartTime,
        const PhysAbsoluteTime& lhsEndTime,
        const MexSausage2d& rhsSausage,
        const PhysAbsoluteTime& rhsStartTime,
        const PhysAbsoluteTime& rhsEndTime,
        PhysAbsoluteTime* pCollideTime
    );

    //True iff lhsAccnSausage intersects rhsCSSausage in both time and space.
    //If so returns earliest collision time in pCollideTime.
    //lhsSausage represents an object accelerating form its start to end point,
    //starting at lhsStartTime, and arriving at lhsEndTime.
    //rhsSausage represents an object moving at constant speed from its start to end point,
    //starting at rhsStartTime, and arriving at rhsEndTime.
    static
    bool accelerationSausageIntersectsConstantSpeedSausage
    (
        const MexSausage2d& lhsAccnSausage,
        const PhysAbsoluteTime& lhsStartTime,
        const PhysAbsoluteTime& lhsEndTime,
        const MexSausage2d& rhsCSSausage,
        const PhysAbsoluteTime& rhsStartTime,
        const PhysAbsoluteTime& rhsEndTime,
        PhysAbsoluteTime* pCollideTime
    );

    //True iff lhsSausage intersects rhsSausage in both time and space.
    //If so returns earliest collision time in pCollideTime.
    //lhsSausage represents an object moving at constant speed from its start to end point,
    //starting at lhsStartTime, and arriving at lhsEndTime.
    //Similar for rhsSausage.
    static
    bool constantSpeedSausageIntersectsConstantSpeedSausage
    (
        const MexSausage2d& lhsSausage,
        const PhysAbsoluteTime& lhsStartTime,
        const PhysAbsoluteTime& lhsEndTime,
        const MexSausage2d& rhsSausage,
        const PhysAbsoluteTime& rhsStartTime,
        const PhysAbsoluteTime& rhsEndTime,
        PhysAbsoluteTime* pCollideTime
    );

    //Data members
    MexPoint2d startPoint_; //start location
    MexPoint2d endPoint_; //end location
    PhysRampAcceleration motionProfile_; //Velocity profile for motion between points
    PhysAbsoluteTime createTime_; //Time at which object appears at start point
    PhysRelativeTime motionTimeOffset_; //Offset to start of motion
    PhysAbsoluteTime expiryTime_; //Offset from createTime to time at which object
                                        //disappears from end point
    MATHEX_SCALAR clearance_; // Radius required around object centre for collision avoidance
    bool hasExpiryTime_ :1;//True if expiry time defined

    MexDouble   minHeight_;
    MexDouble   maxHeight_;
};

#ifdef _INLINE
    #include "phys/motchunk.ipp"
#endif


#endif

/* End MOTCHUNK.HPP *************************************************/

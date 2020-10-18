/*
 * R A M P A C C E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysRampAcceleration

    Encapsulates the maths of motion of a value from a start speed to end speed using
    an acceleration and deceleration rate and a terminal velocity. The motion has a
    period of acceleration, followed by a period of constant velocity, followed by
    a further period of acceleration. Either acceleration period might actually be
    a deceleration. The duration of any period might be zero.

    Static helper methods are provided to perform computations when construction of an
    object instance is inappropriate.

	If you change the data members in this class it is very important to remember to change
	the compressed version as well.
*/

#ifndef _RAMPACCE_HPP
#define _RAMPACCE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"

class PhysCompressedRampAcceleration;

//Bitwise canonical
class PhysRampAcceleration
{
public:
    //ctor.
	PhysRampAcceleration( void );

    PhysRampAcceleration
    (
        MATHEX_SCALAR startSpeed,  //Initial speed
        MATHEX_SCALAR firstAcceleration, //Signed: -ve indicates deceleration
        MATHEX_SCALAR secondAcceleration, //Signed: -ve indicates deceleration
        const PhysRelativeTime& totalTime, //Cumulative time of all 3 sections
        const PhysRelativeTime& firstAccelerationTime, //Time of 1st acceleration period
        const PhysRelativeTime& secondAccelerationTime //Time of 2nd acceleration period
    );
    //PRE( firstAcceleration != 0.0 )
    //PRE( secondAcceleration != 0.0 )
    //PRE( firstAccelerationTime >= 0.0 )
    //PRE( secondAccelerationTime >= 0.0 )
    //PRE( totalTime >= firstAccelerationTime + secondAccelerationTime )

	enum DistanceVersion { BY_DISTANCE };

    PhysRampAcceleration
    (
        MATHEX_SCALAR startSpeed,  //Initial speed
        const PhysRelativeTime& firstAccelerationTime, //Signed: -ve indicates deceleration
        const PhysRelativeTime& secondAccelerationTime, //Signed: -ve indicates deceleration
        MATHEX_SCALAR distance1,  // distance covered in the first acceleration period
        MATHEX_SCALAR distance2, //distance covered in the constant velocity period
        MATHEX_SCALAR distance3,
        DistanceVersion version //distance covered in the second acceleration period
    );

    //  Note that this constructor works on an "as good as possible" method. It
    //  is possible to specify the input values such that it is impossible to
    //  reach the desiredSpeed or the endSpeed. If this happens the constructor
    //  will attempt to get as close as possible. The aims in their priority order
    //  are as follows:

    //      1. Cover the given distance (it will always achieve this)
    //      2. Finish at the given end speed
    //      3. Reach the desired speed at some point in the motion

    //  Use the access methods to discover what the end speed actually is

	enum SpeedVersion { BY_SPEED };

    PhysRampAcceleration
    (
      MATHEX_SCALAR distance,
      MATHEX_SCALAR startSpeed,
      MATHEX_SCALAR desiredSpeed,
      MATHEX_SCALAR endSpeed,
      MATHEX_SCALAR accelerationRate,
      MATHEX_SCALAR decelerationRate,
      SpeedVersion
    );
    //  PRE( startSpeed >= 0.0 );
    //  PRE( endSpeed >= 0.0 );
    //  PRE( accelerationRate >= 0.0 );
    //  PRE( decelerationRate >= 0.0 );
    //  POST( totalDistance() == distance );

    enum DistanceTimeVersion { BY_DISTANCE_TIME };

    //  The resulting ramp acceleration will cover the specified distance
    //  in the specified time without exceeding the acceleration and
    //  deceleration rates given. It will make a vague effort to get to
    //  the desired speed but might fail to do this. Desired speed is
    //  basically a hint.
    PhysRampAcceleration
    (
      MATHEX_SCALAR distance,
      MATHEX_SCALAR startSpeed,
      MATHEX_SCALAR desiredSpeed,
      MATHEX_SCALAR endSpeed,
      MATHEX_SCALAR totalTime,
      MATHEX_SCALAR maxAcceleration,
      MATHEX_SCALAR maxDeceleration,
      DistanceTimeVersion
    );
    // PRE( startSpeed >= 0.0 );
    // PRE( endSpeed >= 0.0 );
    // PRE( time >= 0.0 );
    // PRE( maxAcceleration >= 0.0 );
    // PRE( maxDeceleration >= 0.0 );

	// POST( firstAccelerationTime() >= 0 );
	// POST( secondAccelerationTime() >= 0 );
    // POST( firstAccelerationTime() + secondAccelerationTime() <= totalTime() );
    // POST( fabs( totalDistance() - distance ) < MexEpsilon::instance() );

    PhysRampAcceleration( const PhysCompressedRampAcceleration& );

    ~PhysRampAcceleration();

    //reset all the values
    void set
    (
        MATHEX_SCALAR startSpeed,  //Initial speed
        MATHEX_SCALAR firstAcceleration, //Signed: -ve indicates deceleration
        MATHEX_SCALAR secondAcceleration, //Signed: -ve indicates deceleration
        const PhysRelativeTime& totalTime, //Cumulative time of all 3 sections
        const PhysRelativeTime& firstAccelerationTime, //Time of 1st acceleration period
        const PhysRelativeTime& secondAccelerationTime //Time of 2nd acceleration period
    );
    //PRE( firstAcceleration != 0.0 )
    //PRE( secondAcceleration != 0.0 )
    //PRE( firstAccelerationTime >= 0.0 )
    //PRE( secondAccelerationTime >= 0.0 )
    //PRE( totalTime >= firstAccelerationTime + secondAccelerationTime )

    //Accessors
    MATHEX_SCALAR startSpeed() const;
    MATHEX_SCALAR firstAcceleration() const;
    MATHEX_SCALAR secondAcceleration() const;
    const PhysRelativeTime& totalTime() const;
    const PhysRelativeTime& firstAccelerationTime() const;
    const PhysRelativeTime& secondAccelerationTime() const;

    MATHEX_SCALAR distance1() const;
    MATHEX_SCALAR distance2() const;
    MATHEX_SCALAR distance3() const;
	MATHEX_SCALAR totalDistance() const;

    //The speed at the end of the motion
    MATHEX_SCALAR endSpeed() const;

    //  The speed during the constant velocity section. Note
    //  that this section might have a duration of zero.
    MATHEX_SCALAR constantSpeed() const;

    //  The duration of the constant velocity section of the ramp
    MATHEX_SCALAR constantVelocityTime() const;

    //Speed at timeOffset from start of motion
    MATHEX_SCALAR speed( const PhysRelativeTime& timeOffset ) const;

    //Distance covered at timeOffset from start of motion
    MATHEX_SCALAR distance( const PhysRelativeTime& timeOffset ) const;

    //Proportion of the total distance covered at timeOffset from start of motion
    MATHEX_SCALAR proportionOfDistance( const PhysRelativeTime& timeOffset ) const;

    ///////////////////////////////////////////////////////////
    // Static helper methods
    ///////////////////////////////////////////////////////////
    //Encapsulate basic equations of motion
    //v = u + a*t
    static MATHEX_SCALAR endSpeed( MATHEX_SCALAR startSpeed, MATHEX_SCALAR acceleration,
                                   const PhysRelativeTime& time );

    //s = d + ut + 0.5*a*t*t
    static MATHEX_SCALAR distance( MATHEX_SCALAR startValue, MATHEX_SCALAR startSpeed,
                                   MATHEX_SCALAR acceleration, const PhysRelativeTime& time );


    //Returns distance covered given start speed, signed acceleration, signed top speed, and duration.
    //Also returns teh final speed in pEndSpeed.
    static MATHEX_SCALAR distanceAtTime( MATHEX_SCALAR startSpeed, MATHEX_SCALAR acceleration,
                                         MATHEX_SCALAR topSpeed, const PhysRelativeTime& duration,
                                         MATHEX_SCALAR* pEndSpeed);

    ///////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////
    //Rest-to-rest with same acceleration/deceleration value

    //Returns the time required to move from startValue to endValue, using terminal
    //speed speed and acceleration/deceleration rate acceleration.
    //Returns the time spent accelerating in accelerationTime. The time spent in
    //deceleration is the same.
    //NB speed/acceleration negated if necessary - pass in positive values.
    static PhysRelativeTime totalTime( MATHEX_SCALAR startValue, MATHEX_SCALAR endValue,
                               MATHEX_SCALAR speed, MATHEX_SCALAR acceleration,
                               PhysRelativeTime* pAccelerationTime );
    //PRE( acceleration > 0.0 );
    //PRE( speed > 0.0 );

    //Returns the proportion of the total distance covered at time timeOffset,
    //given that the total time for the profile is totalTime, and the time
    //spent in acceleration/deceleration is accelerationTime.
    static MATHEX_SCALAR proportionOfDistance( const PhysRelativeTime& totalTime,
                                               const PhysRelativeTime& accelerationTime,
                                               const PhysRelativeTime& timeOffset );
    //PRE( totalTime >= 0 )
    //PRE( accelerationTime <= 0.5*totalTime )

    //Distance covered when overall time and acceleration/deceleration time
    //are supplied, and the acceleration value.
    static MATHEX_SCALAR distance( const PhysRelativeTime& totalTime,
                                   const PhysRelativeTime& accelerationTime,
                                   MATHEX_SCALAR acceleration );
    //PRE( totalTime >= 0 )
    //PRE( accelerationTime <= 0.5*totalTime )
    ///////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////
    //Full profile: ie not necessarily rest-to-rest; differing acceleration/deceleration

    //Return time required to move through distance with initial, final and intermediate
    //terminal speeds given, using supplied ac/decelerations.
    //The period of ac/deceleration return in final 2 arguments.
    static PhysRelativeTime totalTime
    (
        MATHEX_SCALAR distance,
        MATHEX_SCALAR startSpeed, MATHEX_SCALAR terminalSpeed, MATHEX_SCALAR endSpeed,
        MATHEX_SCALAR acceleration, MATHEX_SCALAR deceleration,
        PhysRelativeTime* pAccelerationTime, PhysRelativeTime* pDecelerationTime
    );
    //PRE( startSpeed >= 0.0 );
    //PRE( terminalSpeed >= startSpeed );
    //PRE( acceleration >= 0.0 );
    //PRE( deceleration >= 0.0 );
    //PRE( implies( terminalSpeed > startSpeed, acceleration > 0.0 ) );
    //PRE( implies( terminalSpeed > endSpeed, deceleration > 0.0 ) );
    //PRE( pAccelerationTime != NULL );
    //PRE( pDecelerationTime != NULL );


    //Returns the proportion of the total distance covered at time timeOffset,
    static MATHEX_SCALAR proportionOfDistance
    (
        MATHEX_SCALAR startSpeed,
        MATHEX_SCALAR firstAcceleration, MATHEX_SCALAR secondAcceleration,
        const PhysRelativeTime& totalTime,
        const PhysRelativeTime& firstAccelerationTime,
        const PhysRelativeTime& secondAccelerationTime,
        const PhysRelativeTime& timeOffset
    );
    //PRE( firstAccelerationTime >= 0.0 );
    //PRE( secondAccelerationTime >= 0.0 );
    //PRE( totalTime >= (accelerationTime + decelerationTime) );


    static MATHEX_SCALAR distance
    (
        MATHEX_SCALAR startSpeed,
        MATHEX_SCALAR firstAcceleration, MATHEX_SCALAR secondAcceleration,
        const PhysRelativeTime& totalTime,
        const PhysRelativeTime& firstAccelerationTime,
        const PhysRelativeTime& secondAccelerationTime,
        const PhysRelativeTime& timeOffset
    );
    //PRE( firstAccelerationTime >= 0.0 );
    //PRE( secondAccelerationTime >= 0.0 );
    //PRE( totalTime >= (accelerationTime + decelerationTime) );

	friend ostream& operator<<(ostream& o, const PhysRampAcceleration&  t);
	PhysRampAcceleration& operator = ( const PhysRampAcceleration& copyMe);
    ///////////////////////////////////////////////////////////

    PER_MEMBER_PERSISTENT_DEFAULT( PhysRampAcceleration );
    PER_FRIEND_READ_WRITE( PhysRampAcceleration );

private:
    // Operations deliberately revoked
	friend class PhysCompressedRampAcceleration;
    //PhysRampAcceleration( void );

    bool solveForTime(
        MATHEX_SCALAR a1, MATHEX_SCALAR a3, MATHEX_SCALAR t,
        MATHEX_SCALAR u1, MATHEX_SCALAR v3, MATHEX_SCALAR s,
        MATHEX_SCALAR* pT1, MATHEX_SCALAR* pT3 ) const;

    //  Return true iff it was possible to meet all of the constraints specified
    bool doSpeedConstructor(
      MATHEX_SCALAR desiredDistance,
      MATHEX_SCALAR startSpeed,
      MATHEX_SCALAR desiredSpeed,
      MATHEX_SCALAR endSpeed,
      MATHEX_SCALAR accelerationRate,
      MATHEX_SCALAR decelerationRate );

    //  Return true iff it was possible to meet all of the constraints specified
    bool doDistanceTimeConstructor(
      MATHEX_SCALAR distance,
      MATHEX_SCALAR startSpeed,
      MATHEX_SCALAR desiredSpeed,
      MATHEX_SCALAR finalSpeed,
      MATHEX_SCALAR time,
      MATHEX_SCALAR maxAcceleration,
      MATHEX_SCALAR maxDeceleration );

    //data members
    MATHEX_SCALAR startSpeed_; //Initial speed
    MATHEX_SCALAR firstAcceleration_; //Signed
    MATHEX_SCALAR secondAcceleration_; //Signed
    PhysRelativeTime totalTime_; //Cumulative time of all 3 sections
    PhysRelativeTime firstAccelerationTime_; //Time of 1st acceleration period
    PhysRelativeTime secondAccelerationTime_; //Time of 2nd acceleration period
};

PER_DECLARE_PERSISTENT( PhysRampAcceleration );

#ifdef _INLINE
    #include "phys/rampacce.ipp"
#endif


#endif

/* End RAMPACCE.HPP *************************************************/

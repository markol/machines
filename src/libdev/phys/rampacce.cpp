/*
 * R A M P A C C E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"
#include <cmath>
#include <cstdlib>
//#include "mathex/epsilon.hpp"
#include "mathex/utility.hpp"

#include "phys/rampacce.hpp"
#include "phys/crampacc.hpp"

#ifndef _INLINE
    #include "phys/rampacce.ipp"
#endif

PER_DEFINE_PERSISTENT( PhysRampAcceleration );

//////////////////////////////////////////////////////////////////////////////////////////

PhysRampAcceleration::PhysRampAcceleration()
{
}

//////////////////////////////////////////////////////////////////////////////////////////

PhysRampAcceleration::~PhysRampAcceleration()
{
}
//////////////////////////////////////////////////////////////////////////////////////////

//static
PhysRelativeTime PhysRampAcceleration::totalTime(
    MATHEX_SCALAR startValue, MATHEX_SCALAR endValue,
    MATHEX_SCALAR speed, MATHEX_SCALAR acceleration,
    PhysRelativeTime* pAccelerationTime )
{
    PRE_INFO( startValue );
    PRE_INFO( endValue );
    PRE_INFO( speed );
    PRE_INFO( acceleration );

    //Check for nonsense values
    PRE( acceleration > 0.0 );
    PRE( speed > 0.0 );
    PRE( pAccelerationTime != NULL );

    //Check if we more than cover the required distance while accelerating to
    //terminal speed.
    MATHEX_SCALAR distance = endValue - startValue;
    bool reachTerminalSpeed = speed * speed / acceleration <= fabs( distance );

    //Convert the speed and acceleration to correct sign
    if( distance < 0.0 )
    {
        speed = -speed;
        acceleration = -acceleration;
    }

    //Compute the time and acceleration time
    PhysRelativeTime duration;
    if( reachTerminalSpeed )
    {
        *pAccelerationTime = speed / acceleration;
        duration = (distance / speed) + (*pAccelerationTime);
    }
    else
    {
        *pAccelerationTime = sqrt( distance / acceleration );
        duration = *pAccelerationTime * 2.0;
    }

    POST_INFO( *pAccelerationTime );
    POST_INFO( duration );

    return duration;
}
//////////////////////////////////////////////////////////////////////////////////////////
//static
MATHEX_SCALAR PhysRampAcceleration::proportionOfDistance(
    const PhysRelativeTime& totalTime,
    const PhysRelativeTime& accelerationTime,
    const PhysRelativeTime& timeOffset )
{
    PRE_INFO( totalTime );
    PRE_INFO( accelerationTime );
    PRE_INFO( timeOffset );

    //Check valid data
    PRE( totalTime >= 0 )
    PRE( accelerationTime * 0.99999 <= 0.5 * totalTime )

    //Check for range
    MATHEX_SCALAR diff = totalTime - accelerationTime;
    MATHEX_SCALAR proportion;

    if( timeOffset >= totalTime )
        proportion = 1.0;
    else if( timeOffset <= 0.0 )
        proportion = 0.0;
    else if( timeOffset <= accelerationTime)
        proportion = timeOffset * timeOffset / (2.0 * accelerationTime * diff );
    else if( timeOffset <= diff )
        proportion = (timeOffset - 0.5 * accelerationTime) / diff;
    else
    {
        MATHEX_SCALAR x = totalTime - timeOffset;
        proportion = 1.0 - x * x / (2.0 * accelerationTime * diff );
    }

    if( proportion > 1.0 )
        proportion = 1.0;

    POST_INFO( proportion );
    POST( proportion >= 0.0 );

    return proportion;
}
//////////////////////////////////////////////////////////////////////////////////////////
//static
MATHEX_SCALAR PhysRampAcceleration::distance(
    const PhysRelativeTime& totalTime,
    const PhysRelativeTime& accelerationTime,
    MATHEX_SCALAR acceleration )
{
    PRE_INFO( totalTime );
    PRE_INFO( accelerationTime );
    PRE_INFO( acceleration );

    PRE( totalTime >= 0 )
    PRE( accelerationTime * 0.9999 <= 0.5*totalTime )

    return acceleration * accelerationTime * (totalTime - accelerationTime);
}
//////////////////////////////////////////////////////////////////////////////////////////

//static
PhysRelativeTime PhysRampAcceleration::totalTime
(
    MATHEX_SCALAR distance,
    MATHEX_SCALAR startVelocity, MATHEX_SCALAR terminalSpeed, MATHEX_SCALAR endVelocity,
    MATHEX_SCALAR acceleration, MATHEX_SCALAR deceleration,
    PhysRelativeTime* pAccelerationTime, PhysRelativeTime* pDecelerationTime
)
{
    PRE_INFO( distance );
    PRE_INFO( startVelocity );
    PRE_INFO( terminalSpeed );
    PRE_INFO( endVelocity );
    PRE_INFO( acceleration );
    PRE_INFO( deceleration );

    //Check for nonsense values

    PRE( acceleration >= 0.0 );
    PRE( deceleration >= 0.0 );
    PRE( pAccelerationTime != NULL );
    PRE( pDecelerationTime != NULL );

    //  Since the only thing this function returns is time we can
    //  do the calculation for negative distances by negating the distance and velocities.
    if( distance < 0 )
    {
        distance =-distance;
        startVelocity = -startVelocity;
        endVelocity = -endVelocity;
        terminalSpeed = -terminalSpeed;
    }

    //Compute the times and distance for the 2 acceleration periods in the
    //normal scenario
    PhysRelativeTime accelerationTime = (acceleration == 0.0 ? 0.0 :
                                (terminalSpeed - startVelocity) / acceleration);
    PhysRelativeTime decelerationTime = (deceleration == 0.0 ? 0.0 :
                                 (terminalSpeed - endVelocity) / deceleration);

    MATHEX_SCALAR accelerationDistance = 0.5 * accelerationTime * (startVelocity + terminalSpeed);
    MATHEX_SCALAR decelerationDistance = 0.5 * decelerationTime * (endVelocity + terminalSpeed);

    //Check for having time do this before covering the required distance
    MATHEX_SCALAR terminalDistance = distance - accelerationDistance - decelerationDistance;

    PhysRelativeTime totalTime;
    if( terminalDistance >= 0.0 )
    {
        totalTime = accelerationTime + decelerationTime + (terminalDistance / terminalSpeed);
    }
    else
    {
        //Check for special cases in which we can't reach the required end speed
        MATHEX_SCALAR minDistance;
        if( startVelocity > endVelocity )
            minDistance = (startVelocity * startVelocity - endVelocity * endVelocity) / (2 * deceleration);
        else if( startVelocity < endVelocity )
            minDistance = (endVelocity * endVelocity - startVelocity * startVelocity) / (2 * acceleration);
        else
            minDistance = 0;

        MATHEX_SCALAR root;
        if( minDistance <= distance )
        {
            //We can accelerate to an intermediate speed
//             root = (2 * acceleration * deceleration * distance +
//                     acceleration * startVelocity * startVelocity +
//                     deceleration * endVelocity * endVelocity) /
//                    (acceleration + deceleration);

            root = (2 * acceleration * (-deceleration) * distance +
                    (-deceleration) * startVelocity * startVelocity -
                    acceleration * endVelocity * endVelocity) /
                   ((-deceleration) - acceleration);

            if( root < 0 ) root = 0.0;
            else root = sqrt( root );


            accelerationTime = (acceleration == 0.0 ? 0.0 :
                                (root - startVelocity) / acceleration);

            decelerationTime = (deceleration == 0.0 ? 0.0 :
                                (root - endVelocity) / deceleration);
            totalTime = accelerationTime + decelerationTime;
        }
        else if( startVelocity > endVelocity )
        {
            //Just decelerate towards end speed until distance covered
            root = startVelocity * startVelocity - 2.0 * deceleration * distance;
            if( root < 0 ) root = 0.0;
            else root = sqrt( root );

            accelerationTime = 0.0;
            totalTime = decelerationTime = (startVelocity - root) / deceleration;
        }
        else
        {
            //Just accelerate towards end speed until distance covered
            root = startVelocity * startVelocity + 2.0 * acceleration * distance;

            if( root < 0 ) root = 0.0;
            else root = sqrt( root );


            totalTime = accelerationTime = (root - startVelocity) / acceleration;
            decelerationTime = 0.0;
        }
    }

    //return computed values
    *pAccelerationTime = accelerationTime;
    *pDecelerationTime = decelerationTime;

    POST_INFO( *pAccelerationTime );
    POST_INFO( *pDecelerationTime );
    POST_INFO( totalTime );

    POST_DATA( MATHEX_SCALAR distance1 = startVelocity * *pAccelerationTime + acceleration * *pAccelerationTime * *pAccelerationTime / 2.0 );
    POST_DATA( MATHEX_SCALAR constantVelocity = startVelocity + acceleration * *pAccelerationTime );
    POST_DATA( MATHEX_SCALAR distance2 = constantVelocity * ( totalTime - *pAccelerationTime - *pDecelerationTime ) );
    POST_DATA( MATHEX_SCALAR distance3 = constantVelocity * *pDecelerationTime - deceleration * *pDecelerationTime * *pDecelerationTime / 2.0 );

    POST_INFO( distance1 );
    POST_INFO( distance2 );
    POST_INFO( distance3 );
    POST_INFO( distance1 + distance2 + distance3 );
    POST_INFO( constantVelocity );

    POST( MexEpsilon::isWithinEpsilonOf( distance1 + distance2 + distance3, distance ) );

    return totalTime;
}
//////////////////////////////////////////////////////////////////////////////////////////
//static
MATHEX_SCALAR PhysRampAcceleration::proportionOfDistance
(
    MATHEX_SCALAR startSpeed,
    MATHEX_SCALAR firstAcceleration, MATHEX_SCALAR secondAcceleration,
    const PhysRelativeTime& totalTime,
    const PhysRelativeTime& firstAccelerationTime,
    const PhysRelativeTime& secondAccelerationTime,
    const PhysRelativeTime& timeOffset
)
{
    PRE_INFO( startSpeed );
    PRE_INFO( firstAcceleration );
    PRE_INFO( secondAcceleration );
    PRE_INFO( totalTime );
    PRE_INFO( firstAccelerationTime );
    PRE_INFO( secondAccelerationTime );
    PRE_INFO( timeOffset );

    PRE( firstAccelerationTime >= 0.0 );
    PRE( secondAccelerationTime >= 0.0 );
    PRE( totalTime >= 0.99999 * (firstAccelerationTime + secondAccelerationTime) );

    MATHEX_SCALAR proportion;

    //Check for range
    if( timeOffset >= totalTime )
        proportion = 1.0;
    else if( timeOffset <= 0.0 )
        proportion = 0.0;
    else
    {
        //Compute the total distance, etc
        MATHEX_SCALAR terminalSpeed = startSpeed + firstAcceleration * firstAccelerationTime;
        MATHEX_SCALAR endSpeed = terminalSpeed + secondAcceleration * secondAccelerationTime;
        MATHEX_SCALAR distance =
            0.5 * ((startSpeed - terminalSpeed) * firstAccelerationTime +
                   (endSpeed - terminalSpeed) * secondAccelerationTime) +
            totalTime * terminalSpeed;

        //Compute the distance
        MATHEX_SCALAR timeDistance;
        if( timeOffset <= firstAccelerationTime )
        {
            timeDistance = (startSpeed + 0.5 * firstAcceleration * timeOffset) * timeOffset;
        }
        else if( timeOffset <= totalTime - secondAccelerationTime )
        {
            timeDistance = 0.5 * (startSpeed - terminalSpeed) * firstAccelerationTime +
                           terminalSpeed * timeOffset;
        }
        else
        {
            MATHEX_SCALAR timeToGo = totalTime - timeOffset;
            timeDistance = distance -
                           (endSpeed - 0.5 * secondAcceleration * timeToGo) * timeToGo;
        }

        proportion = (distance == 0.0 ? 1.0 : timeDistance / distance);
    }

    if( proportion > 1.0 )
        proportion = 1.0;

    POST_INFO( proportion );
    POST( proportion >= 0.0 );

    return proportion;
}
//////////////////////////////////////////////////////////////////////////////////////////
//static
MATHEX_SCALAR PhysRampAcceleration::distance
(
    MATHEX_SCALAR startSpeed,
    MATHEX_SCALAR firstAcceleration, MATHEX_SCALAR secondAcceleration,
    const PhysRelativeTime& totalTime,
    const PhysRelativeTime& firstAccelerationTime,
    const PhysRelativeTime& secondAccelerationTime,
    const PhysRelativeTime& timeOffset
)
{
    PRE_INFO( startSpeed );
    PRE_INFO( firstAcceleration );
    PRE_INFO( secondAcceleration );
    PRE_INFO( totalTime );
    PRE_INFO( firstAccelerationTime );
    PRE_INFO( secondAccelerationTime );
    PRE_INFO( timeOffset );

    PRE( firstAccelerationTime >= 0.0 );
    PRE( secondAccelerationTime >= 0.0 );
    PRE( totalTime >= 0.99999 * (firstAccelerationTime + secondAccelerationTime) );

    //Check for range
    MATHEX_SCALAR distance;
    if( timeOffset <= 0.0 )
        distance = 0.0;
    else
    {
        //Compute the intermediate and final speed
        MATHEX_SCALAR terminalSpeed = startSpeed + firstAcceleration * firstAccelerationTime;
        MATHEX_SCALAR endSpeed = terminalSpeed + secondAcceleration * secondAccelerationTime;

        //Compute the distance
        if( timeOffset >= totalTime )
        {
            distance = 0.5 * ((startSpeed - terminalSpeed) * firstAccelerationTime +
                              (endSpeed - terminalSpeed) * secondAccelerationTime) +
                       totalTime * terminalSpeed;
        }
        else if( timeOffset <= firstAccelerationTime )
        {
            distance = (startSpeed + 0.5 * firstAcceleration * timeOffset) * timeOffset;
        }
        else if( timeOffset <= totalTime - secondAccelerationTime )
        {
            distance = 0.5 * (startSpeed - terminalSpeed) * firstAccelerationTime +
                       terminalSpeed * timeOffset;
        }
        else
        {
            PhysRelativeTime a2Time = secondAccelerationTime - (totalTime - timeOffset);
            distance = 0.5 * (startSpeed - terminalSpeed) * firstAccelerationTime +
                       terminalSpeed * timeOffset +
                       0.5 * secondAcceleration * a2Time * a2Time;
        }
    }

    POST_INFO( distance );

    return distance;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysRampAcceleration::PhysRampAcceleration
(
    MATHEX_SCALAR startSpeed,  //Initial speed
    MATHEX_SCALAR firstAcceleration, //Signed: -ve indicates deceleration
    MATHEX_SCALAR secondAcceleration, //Signed: -ve indicates deceleration
    const PhysRelativeTime& totalTime, //Cumulative time of all 3 sections
    const PhysRelativeTime& firstAccelerationTime, //Time of 1st acceleration period
    const PhysRelativeTime& secondAccelerationTime //Time of 2nd acceleration period
)
:   startSpeed_( startSpeed ),
    firstAcceleration_( firstAcceleration ),
    secondAcceleration_( secondAcceleration ),
    totalTime_( totalTime ),
    firstAccelerationTime_( firstAccelerationTime ),
    secondAccelerationTime_( secondAccelerationTime )
{
    PRE_INFO( startSpeed );
    PRE_INFO( firstAcceleration );
    PRE_INFO( secondAcceleration );
    PRE_INFO( totalTime );
    PRE_INFO( firstAccelerationTime );
    PRE_INFO( secondAccelerationTime  );

    PRE( firstAcceleration != 0.0 );
    PRE( secondAcceleration != 0.0 );
    PRE( firstAccelerationTime >= 0.0 );
    PRE( secondAccelerationTime >= 0.0 );
    PRE( totalTime >= firstAccelerationTime + secondAccelerationTime - MexEpsilon::instance() );
}


PhysRampAcceleration::PhysRampAcceleration
(
    MATHEX_SCALAR startSpeed,  //Initial speed
    const PhysRelativeTime& firstAccelerationTime, //Time of 1st acceleration period
    const PhysRelativeTime& secondAccelerationTime, //Time of 2nd acceleration period
    MATHEX_SCALAR distance1,  // distance covered in the first acceleration period
    MATHEX_SCALAR distance2, //distance covered in the constant velocity period
    MATHEX_SCALAR distance3, //distance covered in the second acceleration period
    DistanceVersion
)
:   startSpeed_( startSpeed ),
    firstAccelerationTime_( firstAccelerationTime ),
    secondAccelerationTime_( secondAccelerationTime )
{
    PRE_INFO( startSpeed );
    PRE_INFO( firstAccelerationTime );
    PRE_INFO( secondAccelerationTime );
    PRE_INFO( distance1 );
    PRE_INFO( distance2 );
    PRE_INFO( distance3 );

	PRE( firstAccelerationTime  >= 0.0 );
	PRE( secondAccelerationTime >= 0.0 );

	firstAcceleration_ = 2.0 * (distance1 - startSpeed*firstAccelerationTime)/(firstAccelerationTime*firstAccelerationTime);

	MATHEX_SCALAR constVelocity = startSpeed + firstAcceleration_*firstAccelerationTime_;
	MATHEX_SCALAR constVelocityTime;

	if( constVelocity != 0)

	   constVelocityTime = distance2/constVelocity;

	else
	{
		ASSERT( distance2 == 0, logic_error("Cannot travel this far with no speed and no acceleration!") );
		constVelocityTime = 0;
	}

	secondAcceleration_	= 2.0 * (distance3 -constVelocity*secondAccelerationTime)/(secondAccelerationTime*secondAccelerationTime);

	totalTime_ = firstAccelerationTime_ + constVelocityTime + secondAccelerationTime_;
}

PhysRampAcceleration::PhysRampAcceleration
(
  MATHEX_SCALAR desiredDistance,
  MATHEX_SCALAR startSpeed,
  MATHEX_SCALAR desiredSpeed,
  MATHEX_SCALAR endSpeed,
  MATHEX_SCALAR accelerationRate,
  MATHEX_SCALAR decelerationRate,
  SpeedVersion
)
: startSpeed_( startSpeed )
{
    PRE_INFO( desiredDistance );
    PRE_INFO( startSpeed );
    PRE_INFO( desiredSpeed );
    PRE_INFO( endSpeed );
    PRE_INFO( accelerationRate );
    PRE_INFO( decelerationRate );

    PRE( startSpeed >= 0.0 );
    PRE( endSpeed >= 0.0 );
    PRE( accelerationRate >= 0.0 );
    PRE( decelerationRate >= 0.0 );

    PRE( implies( desiredDistance == 0.0, ( startSpeed == 0.0 and endSpeed == 0.0 ) ) );

    PRE( implies( desiredDistance > 0.0, desiredSpeed >= 0.0 ) );
    PRE( implies( desiredDistance < 0.0, desiredSpeed <= 0.0 ) );

    bool    finished = false;

    ASSERT_DATA( int counter = 0 );

    while( not finished )
    {
        if( doSpeedConstructor( desiredDistance, startSpeed, desiredSpeed, endSpeed, accelerationRate, decelerationRate ) )
        {
            finished = true;
        }
        else
        {
            //  TBD: Actually calculate the necessary accelerations, don't just iterate
            accelerationRate *= 2.0;
            decelerationRate *= 2.0;
        }

        ASSERT( ++counter < 100, "" );
    }

    POST_INFO( totalDistance() );
    POST_INFO( desiredDistance );
    POST( fabs( fabs( totalDistance() ) - fabs( desiredDistance ) ) < MexEpsilon::instance() );

    POST_INFO( totalTime_ );
    POST_INFO( firstAccelerationTime_ );
    POST_INFO( secondAccelerationTime_ );

    POST( totalTime_ >= firstAccelerationTime_ + secondAccelerationTime_ - MexEpsilon::instance() );

    POST_INFO( totalTime_ );
    POST( totalTime_ >= 0.0 );
}

bool PhysRampAcceleration::doSpeedConstructor
(
  MATHEX_SCALAR desiredDistance,
  MATHEX_SCALAR startSpeed,
  MATHEX_SCALAR desiredSpeed,
  MATHEX_SCALAR endSpeed,
  MATHEX_SCALAR accelerationRate,
  MATHEX_SCALAR decelerationRate
)
{
    PRE_INFO( desiredDistance );
    PRE_INFO( startSpeed );
    PRE_INFO( desiredSpeed );
    PRE_INFO( endSpeed );
    PRE_INFO( accelerationRate );
    PRE_INFO( decelerationRate );

    PRE( startSpeed >= 0.0 );
    PRE( endSpeed >= 0.0 );
    PRE( accelerationRate >= 0.0 );
    PRE( decelerationRate >= 0.0 );

    PRE( implies( desiredDistance > 0.0, desiredSpeed >= 0.0 ) );
    PRE( implies( desiredDistance < 0.0, desiredSpeed <= 0.0 ) );

    bool    reachedEndSpeed = true;

    startSpeed_ = startSpeed;

    if( startSpeed <= desiredSpeed )
        firstAcceleration_ = accelerationRate;
    else
        firstAcceleration_ = -decelerationRate;
    ASSERT( firstAcceleration_ != 0.0, "" );
    firstAccelerationTime_ = ( desiredSpeed - startSpeed ) / firstAcceleration_;


    MATHEX_SCALAR distance1 = distance( 0.0, startSpeed,
      firstAcceleration_, firstAccelerationTime_ );

    if( desiredSpeed <= endSpeed )
        secondAcceleration_ = accelerationRate;
    else
        secondAcceleration_ = -decelerationRate;

    ASSERT( secondAcceleration_ != 0.0, "" );
    secondAccelerationTime_ = ( endSpeed - desiredSpeed ) / secondAcceleration_;

    MATHEX_SCALAR distance3 = distance( 0.0, desiredSpeed,
      secondAcceleration_, secondAccelerationTime_ );

    if( fabs( distance1 ) > fabs( desiredDistance ) or
      ( fabs( distance1 ) + fabs( distance3 ) ) > fabs( desiredDistance ) or
      desiredSpeed < MexEpsilon::instance() )
    {
        //  Cannot reach the desired speed and then the end speed. Head straight for
        //  the end speed.

        if( startSpeed < endSpeed )
            firstAcceleration_ = accelerationRate;
        else
            firstAcceleration_ = -decelerationRate;

        //  Calculate how long it would take to reach the end speed
        firstAccelerationTime_ = ( endSpeed - startSpeed ) / firstAcceleration_;

        //  Calculate the distance travelled by the time we reach the end speed
        distance1 = distance( 0.0, startSpeed,
            firstAcceleration_, firstAccelerationTime_ );

        if( fabs( distance1 ) <= fabs( desiredDistance ) )
        {
            //  We can make it to the desired end speed

            if( endSpeed == 0 )
            {
                totalTime_ = totalTime( desiredDistance, startSpeed,
                  desiredSpeed, endSpeed, accelerationRate, decelerationRate,
                  &firstAccelerationTime_, &secondAccelerationTime_ );

//                if( desiredSpeed > startSpeed )
                {
                    firstAcceleration_ = accelerationRate;
                    secondAcceleration_ = -decelerationRate;
                }
//                 else
//                 {
//                     firstAcceleration_ = -accelerationRate;
//                     secondAcceleration_ = decelerationRate;
//                 }
            }
            else
            {
                MATHEX_SCALAR remainingDistance = desiredDistance - distance1;
                ASSERT( endSpeed != 0.0, "" );
                PhysRelativeTime remainingTime = remainingDistance / endSpeed;
                totalTime_ = firstAccelerationTime_ + remainingTime;
                secondAccelerationTime_ = 0.0;
                secondAcceleration_ = firstAcceleration_;
            }
        }
        else
        {
            //  We can't make it to the desired end speed.

            reachedEndSpeed = false;
        }
    }
    else
    {
        //  Can reach the desired speed and the end speed. Just need to fill
        //  in the total time spent on the movement.

        MATHEX_SCALAR distance2 = desiredDistance - distance1 - distance3;

        PhysRelativeTime constantSpeedTime = distance2 / desiredSpeed;

        totalTime_ = firstAccelerationTime_ + constantSpeedTime + secondAccelerationTime_;
    }

    return reachedEndSpeed;
}

PhysRampAcceleration::PhysRampAcceleration
(
  MATHEX_SCALAR distance,
  MATHEX_SCALAR startSpeed,
  MATHEX_SCALAR desiredSpeed,
  MATHEX_SCALAR finalSpeed,
  MATHEX_SCALAR time,
  MATHEX_SCALAR maxAcceleration,
  MATHEX_SCALAR maxDeceleration,
  DistanceTimeVersion
)
: startSpeed_( startSpeed ),
  totalTime_( time )
{
    PRE_INFO( distance );
    PRE_INFO( startSpeed );
    PRE_INFO( desiredSpeed );
    PRE_INFO( finalSpeed );
    PRE_INFO( time );
    PRE_INFO( maxAcceleration );
    PRE_INFO( maxDeceleration );

    PRE( startSpeed >= 0.0 );
    PRE( finalSpeed > -MexEpsilon::instance() );
    PRE( time > 0.0 );
    PRE( maxAcceleration >= 0.0 );
    PRE( maxDeceleration >= 0.0 );

    //  Hack to allow for slightly negative end speeds.
    //  TBD: Investigate the whole reason these occur at all and
    //  correct the problem at source.

    if( finalSpeed < 0 and finalSpeed > -MexEpsilon::instance() )
        finalSpeed = 0.0;

    bool    finished = false;

    int counter = 0;

    while( not finished  and  counter++ != 20 )
    {
        ASSERT_INFO( counter );

        if( doDistanceTimeConstructor( distance, startSpeed, desiredSpeed,
          finalSpeed, time, maxAcceleration, maxDeceleration ) )
        {
            finished = true;
        }
        else
        {
            //  TBD: Actually calculate the necessary accelerations, don't just iterate
            maxAcceleration *= 4.0;
            maxDeceleration *= 4.0;
        }
    }

    if( not finished )
    {
        ASSERT_FAIL( "PhysRampAcceleration time/distance ctor failed" );

        //So we don't hang in release, do a simple acceleration from some value to the desired speed,
        //covering the required distance in the time
        firstAccelerationTime_ = 0.0;
        secondAccelerationTime_ = time;
        firstAcceleration_ = maxAcceleration; //dummy - not used
        secondAcceleration_ = 2.0 * (finalSpeed * time  - distance) / (time * time);
        startSpeed_ = finalSpeed - secondAcceleration_ * time;
    }

	POST( firstAccelerationTime() >= 0 );
	POST( secondAccelerationTime() >= 0 );
    POST( firstAccelerationTime() + secondAccelerationTime() <= totalTime() );

    POST_INFO( totalDistance() );
    POST_INFO( distance );

    POST_INFO( finalSpeed );
    POST_INFO( endSpeed() );

    POST( MexEpsilon::isWithinEpsilonOf( finalSpeed, endSpeed() ) );

    POST( fabs( totalDistance() - distance ) < MexEpsilon::instance() );
}

bool PhysRampAcceleration::doDistanceTimeConstructor
(
  MATHEX_SCALAR distance,
  MATHEX_SCALAR startSpeed,
  MATHEX_SCALAR desiredSpeed,
  MATHEX_SCALAR finalSpeed,
  MATHEX_SCALAR time,
  MATHEX_SCALAR maxAcceleration,
  MATHEX_SCALAR maxDeceleration
)
{
    PRE_INFO( distance );
    PRE_INFO( startSpeed );
    PRE_INFO( desiredSpeed );
    PRE_INFO( finalSpeed );
    PRE_INFO( time );
    PRE_INFO( maxAcceleration );
    PRE_INFO( maxDeceleration );

    PRE( startSpeed >= 0.0 );
    PRE( finalSpeed >= 0.0 );
    PRE( time > 0.0 );
    PRE( maxAcceleration >= 0.0 );
    PRE( maxDeceleration >= 0.0 );
    PRE( distance >= 0.0 or (startSpeed == 0.0  and  finalSpeed == 0.0) );

    startSpeed_ = startSpeed;
    totalTime_ = time;

    bool finished = false;
    bool sufficientTimeToChangeSpeed;

    //We can determine the direction of acceleration/deceleration based on the distance
    //to be covered and relative magnitudes of the start/end speeds
    if( distance < 0.0 )
    {
        //moving negative. With current preconditions must be moving from/to zero speed
        firstAcceleration_ = -maxAcceleration;
        secondAcceleration_ = maxDeceleration;
        sufficientTimeToChangeSpeed = true;
    }
    else
    {
        MATHEX_SCALAR deltaSpeed = finalSpeed - startSpeed;
        if( deltaSpeed >= 0.0 )
        {
            //Must end up faster than we started
            MATHEX_SCALAR firstAccelerationTime = deltaSpeed / maxAcceleration;
            MATHEX_SCALAR upperLimitDistance = finalSpeed * time - 0.5 * firstAccelerationTime * deltaSpeed;

            MATHEX_SCALAR secondAccelerationTime = deltaSpeed / maxDeceleration;
            MATHEX_SCALAR lowerLimitDistance = startSpeed * time + 0.5 * secondAccelerationTime * deltaSpeed;

            sufficientTimeToChangeSpeed = firstAccelerationTime <= time  and  secondAccelerationTime <= time;
            if( sufficientTimeToChangeSpeed )
            {
                if( distance > upperLimitDistance )
                {
                    firstAcceleration_ = maxAcceleration;
                    secondAcceleration_ = -maxDeceleration;
                }
                else if( distance < lowerLimitDistance )
                {
                    firstAcceleration_ = -maxAcceleration;
                    secondAcceleration_ = maxDeceleration;
                }
                else
                {
                    firstAcceleration_ = maxAcceleration;
                    secondAcceleration_ = maxDeceleration;
                }
            }
        }
        else
        {
            //Must end up slower than we started
            deltaSpeed = -deltaSpeed;
            MATHEX_SCALAR firstAccelerationTime = deltaSpeed / maxAcceleration;
            MATHEX_SCALAR lowerLimitDistance = finalSpeed * time + 0.5 * firstAccelerationTime * deltaSpeed;

            MATHEX_SCALAR secondAccelerationTime = deltaSpeed / maxDeceleration;
            MATHEX_SCALAR upperLimitDistance = startSpeed * time - 0.5 * secondAccelerationTime * deltaSpeed;

            sufficientTimeToChangeSpeed = firstAccelerationTime <= time  and  secondAccelerationTime <= time;
            if( sufficientTimeToChangeSpeed )
            {
                if( distance > upperLimitDistance )
                {
                    firstAcceleration_ = maxAcceleration;
                    secondAcceleration_ = -maxDeceleration;
                }
                else if( distance < lowerLimitDistance )
                {
                    firstAcceleration_ = -maxAcceleration;
                    secondAcceleration_ = maxDeceleration;
                }
                else
                {
                    firstAcceleration_ = -maxAcceleration;
                    secondAcceleration_ = -maxDeceleration;
                }
            }
        }
    }

    if( sufficientTimeToChangeSpeed  and
        solveForTime( firstAcceleration_, secondAcceleration_, time,
                      startSpeed, finalSpeed, distance, &firstAccelerationTime_, &secondAccelerationTime_ ) )
    {
        //Compute the intermediate speed that will be attained
        const MATHEX_SCALAR middleSpeed = startSpeed + firstAccelerationTime_ * firstAcceleration_;

        //This speed must have the same sign as the distance we want to move
        const bool correctSign = (distance >= 0.0 and middleSpeed >= 0.0)  or
                           (distance < 0.0 and middleSpeed < 0.0);

        if( correctSign  and
            firstAccelerationTime_ >= 0.0 and secondAccelerationTime_ >= 0.0 and
          ( firstAccelerationTime_ + secondAccelerationTime_ ) <= time + MexEpsilon::instance() )
        {
            finished = true;
        }
    }

//    ASSERT( finished, "Could not find a solution" );

    if( finished )
    {
        if( firstAccelerationTime_ + secondAccelerationTime_ > time )
        {
            //  Correct for rounding errors by rescaling the acceleration times

            const MATHEX_SCALAR proportion = firstAccelerationTime_ / ( firstAccelerationTime_ + secondAccelerationTime_ );

            firstAccelerationTime_ = time * proportion;
            secondAccelerationTime_ = time - firstAccelerationTime_;
        }
    }

    return finished;
}

MATHEX_SCALAR PhysRampAcceleration::distance1() const
{
	return ( startSpeed_ + 0.5*firstAcceleration_*firstAccelerationTime_ )*firstAccelerationTime_;
}

MATHEX_SCALAR PhysRampAcceleration::distance2() const
{
	MATHEX_SCALAR constVelocity = startSpeed_ + firstAcceleration_*firstAccelerationTime_;
	MATHEX_SCALAR constVelocityTime = totalTime_ -  firstAccelerationTime_ - secondAccelerationTime_;
	return  constVelocity * constVelocityTime;
}


MATHEX_SCALAR PhysRampAcceleration::distance3() const
{
	MATHEX_SCALAR constVelocity = startSpeed_ + firstAcceleration_*firstAccelerationTime_;
	return 	( constVelocity + 0.5*secondAcceleration_*secondAccelerationTime_ ) * secondAccelerationTime_;
}

MATHEX_SCALAR PhysRampAcceleration::totalDistance() const
{
	return distance1() + distance2() + distance3();
}

void PhysRampAcceleration::set
(
    MATHEX_SCALAR startSpeed,
    MATHEX_SCALAR firstAcceleration,
    MATHEX_SCALAR secondAcceleration,
    const PhysRelativeTime& totalTime,
    const PhysRelativeTime& firstAccelerationTime,
    const PhysRelativeTime& secondAccelerationTime
)
{
    PRE_INFO( startSpeed );
    PRE_INFO( firstAcceleration );
    PRE_INFO( secondAcceleration );
    PRE_INFO( totalTime );
    PRE_INFO( firstAccelerationTime );
    PRE_INFO( secondAccelerationTime );

    PRE( firstAcceleration != 0.0 );
    PRE( secondAcceleration != 0.0 );
    PRE( firstAccelerationTime >= 0.0 );
    PRE( secondAccelerationTime >= 0.0 );
    PRE( totalTime >= firstAccelerationTime + secondAccelerationTime - MexEpsilon::instance() );

    startSpeed_ = startSpeed;
    firstAcceleration_ = firstAcceleration;
    secondAcceleration_ = secondAcceleration;
    totalTime_ = totalTime;
    firstAccelerationTime_ = firstAccelerationTime;
    secondAccelerationTime_ = secondAccelerationTime;
}

MATHEX_SCALAR PhysRampAcceleration::constantSpeed() const
{
    return startSpeed_ + ( firstAcceleration_ * firstAccelerationTime_ );
}

MATHEX_SCALAR PhysRampAcceleration::endSpeed() const
{
    return startSpeed_ + (firstAcceleration_ * firstAccelerationTime_) +
                         (secondAcceleration_ * secondAccelerationTime_);
}

MATHEX_SCALAR PhysRampAcceleration::speed( const PhysRelativeTime& timeOffset ) const
{
    MATHEX_SCALAR result;

    result = startSpeed_;

    if( timeOffset <= firstAccelerationTime_ )
        result += timeOffset * firstAcceleration_;
    else
    {
        result += firstAccelerationTime_ * firstAcceleration_;

        PhysRelativeTime a2Time = timeOffset -
            (totalTime_ - firstAccelerationTime_ - secondAccelerationTime_);

        if( a2Time > 0.0 )
        {
            result += secondAcceleration_ *
                      (a2Time >= secondAccelerationTime_ ? secondAccelerationTime_ : a2Time );
        }
    }

    return result;
}


MATHEX_SCALAR PhysRampAcceleration::distance( const PhysRelativeTime& timeOffset ) const
{
    return distance( startSpeed_, firstAcceleration_, secondAcceleration_,
                     totalTime_, firstAccelerationTime_, secondAccelerationTime_,
                     timeOffset );
}

MATHEX_SCALAR PhysRampAcceleration::proportionOfDistance( const PhysRelativeTime& timeOffset ) const
{
    return proportionOfDistance( startSpeed_, firstAcceleration_, secondAcceleration_,
                                 totalTime_, firstAccelerationTime_, secondAccelerationTime_,
                                 timeOffset );
}

MATHEX_SCALAR PhysRampAcceleration::constantVelocityTime() const
{
    return totalTime_ - firstAccelerationTime_ - secondAccelerationTime_;
}

ostream& operator<<(ostream& stream, const PhysRampAcceleration&  t)
{
    stream << "start speed              " << t.startSpeed_ << std::endl;
    stream << "constant speed           " << t.constantSpeed() << std::endl;
    stream << "end speed                " << t.endSpeed() << std::endl;
    stream << "first acceleration       " << t.firstAcceleration_ << std::endl;
    stream << "second acceleration      " << t.secondAcceleration_ << std::endl;
    stream << "first acceleration time  " << t.firstAccelerationTime_ << std::endl;
    stream << "constant velocity time   " << t.constantVelocityTime() << std::endl;
    stream << "second acceleration time " << t.secondAccelerationTime_ << std::endl;
    stream << "total time               " << t.totalTime_ << std::endl;
    stream << "distance 1               " << t.distance1() << std::endl;
    stream << "distance 2               " << t.distance2() << std::endl;
    stream << "distance 3               " << t.distance3() << std::endl;

	return stream;
}

PhysRampAcceleration& PhysRampAcceleration::operator = ( const PhysRampAcceleration& copyMe)
{
    startSpeed_ = copyMe.startSpeed_;
    firstAcceleration_ = copyMe.firstAcceleration_;
    secondAcceleration_ = copyMe.secondAcceleration_;
    totalTime_ = copyMe.totalTime_;
    firstAccelerationTime_ = copyMe.firstAccelerationTime_;
    secondAccelerationTime_ = copyMe.secondAccelerationTime_;

    return *this;
}

//  The maths here is based on the standard notation for the equations of motion:
//  a - acceleration
//  u - initial velocity
//  v - final velocity
//  t - time
//  s - distance
//  The suffixes indicate which portion of the motion
//  ( first, second or third ) the value refers to.

bool PhysRampAcceleration::solveForTime(
    MATHEX_SCALAR a1, MATHEX_SCALAR a3, MATHEX_SCALAR t,
    MATHEX_SCALAR u1, MATHEX_SCALAR v3, MATHEX_SCALAR s,
    MATHEX_SCALAR* pT1, MATHEX_SCALAR* pT3 ) const
{
    PRE_INFO( a1 );
    PRE_INFO( a3 );
    PRE_INFO( t );
    PRE_INFO( u1 );
    PRE_INFO( v3 );
    PRE_INFO( s );

    MATHEX_SCALAR a = -a1 / 2.0 + ( a1 * a1 ) / ( 2.0 * a3 );
    MATHEX_SCALAR b = a1 * t + ( ( u1 * a1 ) / a3 ) - ( ( v3 * a1 ) / a3 );
    MATHEX_SCALAR c = u1 * t + ( v3 * v3 ) / ( 2.0 * a3 )  + ( u1 * u1 ) / ( 2.0 * a3 ) - ( u1 * v3 ) / a3 - s;

    MATHEX_SCALAR root1 = 0.0;
    MATHEX_SCALAR root2 = 0.0;

    bool canSolve;

    if( MexEpsilon::isWithinEpsilonOf( a, 0.0 ) and MexEpsilon::isWithinEpsilonOf( b, 0.0 ) )
    {
        canSolve = false;
    }
    else
    {
        uint nRoots = MexUtility::quadraticRealRoots( a, b, c, &root1, &root2 );

        if( nRoots == 0 )
        {
            canSolve = false;
        }
        else
        {
            canSolve = true;

            if( nRoots == 1 )
                *pT1 = root1;
            else
            {
                if( root1 < 0.0 )
                    *pT1 = root2;
                else if( root2 < 0.0 )
                    *pT1 = root1;
                else
                    *pT1 = std::min( root1, root2 );
            }

            *pT3 = ( v3 - u1 - a1 * *pT1 ) / a3;
        }
    }

    //  Check for time values that are slightly negative and see if by
    //  making them zero we can still get close enough
    if( canSolve and ( *pT1 < 0.0 or *pT3 < 0.0 ) )
    {
        if( *pT1 < 0.0 )
            *pT1 = 0.0;

        if( *pT3 < 0.0 )
            *pT3 = 0.0;

        const MATHEX_SCALAR u2 = u1 + a1 * *pT1;
        const MATHEX_SCALAR u3 = u2;
        const MATHEX_SCALAR t2 = t - *pT1 - *pT3;

        const MATHEX_SCALAR distance = u1 * *pT1 + a1 * *pT1 * *pT1 / 2.0 +
          u2 * t2 +
          u3 * *pT3 + a3 * *pT3 * *pT3 / 2.0;

        const MATHEX_SCALAR finalSpeed = u3 + a3 * *pT3;

        if( MexEpsilon::isWithinEpsilonOf( distance, s ) and
          MexEpsilon::isWithinEpsilonOf( finalSpeed, v3 ) )
        {
            canSolve = true;
        }
        else
        {
            canSolve = false;
        }
    }


    return canSolve;
}

void perWrite( PerOstream& ostr, const PhysRampAcceleration& ramp )
{
    ostr << ramp.startSpeed_;
    ostr << ramp.firstAcceleration_;
    ostr << ramp.secondAcceleration_;
    ostr << ramp.totalTime_;
    ostr << ramp.firstAccelerationTime_;
    ostr << ramp.secondAccelerationTime_;
}

void perRead( PerIstream& istr, PhysRampAcceleration& ramp )
{
    istr >> ramp.startSpeed_;
    istr >> ramp.firstAcceleration_;
    istr >> ramp.secondAcceleration_;
    istr >> ramp.totalTime_;
    istr >> ramp.firstAccelerationTime_;
    istr >> ramp.secondAccelerationTime_;
}

//static
MATHEX_SCALAR PhysRampAcceleration::distanceAtTime
(
    MATHEX_SCALAR startSpeed, MATHEX_SCALAR acceleration,
    MATHEX_SCALAR topSpeed, const PhysRelativeTime& duration,
    MATHEX_SCALAR* pEndSpeed
)
{
    PRE_INFO( startSpeed );
    PRE_INFO( acceleration );
    PRE_INFO( topSpeed );
    PRE_INFO( duration );

    PRE( acceleration != 0.0 );

    //Compute the time it will take to reach the top speed
    PhysRelativeTime accnTime = (topSpeed - startSpeed) / acceleration;

    //Hence compute
    MATHEX_SCALAR distance;
    if( accnTime > duration )
    {
        //Doesn't reach top speed
        distance = (startSpeed + 0.5 * acceleration * duration) * duration;
        *pEndSpeed = startSpeed + acceleration * duration;
    }
    else
    {
        //Does reach top speed
        *pEndSpeed = topSpeed;
        distance = duration * topSpeed - 0.5 * accnTime * (topSpeed - startSpeed);
    }

    POST_INFO( distance );

    return distance;
}

PhysRampAcceleration::PhysRampAcceleration( const PhysCompressedRampAcceleration& ramp )
:   startSpeed_( ramp.startSpeed_ ),
    firstAcceleration_( ramp.firstAcceleration_ ),
    secondAcceleration_( ramp.secondAcceleration_ ),
    totalTime_( ramp.totalTime_ ),
    firstAccelerationTime_( ramp.firstAccelerationTime_ ),
    secondAccelerationTime_( ramp.secondAccelerationTime_ )
{
}


/* End RAMPACCE.CPP *************************************************/

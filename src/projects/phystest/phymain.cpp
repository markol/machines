/*
 * P H Y M A I N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "phystest/phytest.hpp"

#include "phys/rampacce.hpp"
#include "phys/timespin.hpp"
#include "phys/motchunk.hpp"

#include "mathex/utility.hpp"
#include "mathex/random.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/eulerang.hpp"

//void    bobTest();

static void rampAccelerationTest();
static void rampAccelerationTest2();
static void testByDistanceTime();
static void testBySpeed();
static void timedSpinTest();
static void motionChunkTest();
static PhysMotionChunk chunk1();
static PhysMotionChunk chunk2();

static void timedSpinPlanTest( MexBasicRandom* pR, PhysTimedSpinPlan* pPlan );
static MexVec3 randomNonZeroVec3( MexBasicRandom* pR );
static MexVec3 randomVec3( MexBasicRandom* pR );
static MexPoint3d randomPoint3d( MexBasicRandom* pR );
static MexTransform3d randomTransform3d( MexBasicRandom* pR );

int main()

{
	PhysTest	tester;

	//tester.testAlignedBSPTree2d(std::cout);

	//motionChunkTest(); //OK

    //rampAccelerationTest();

    //timedSpinTest(); // OK? memleak/dev timer?

    //bobTest();

	return 0;
}


void timedSpinTest()
{
    const size_t N_TESTS = 100000;

    MexBasicRandom  r1;

    r1.seed( 20 );

    for( size_t i = 0; i < N_TESTS; ++i )
    {
        std::cout << "Test " << i << "\r";

        MexVec3 axis( randomNonZeroVec3( &r1 ) );
        MexRadians startAngle = mexRandomScalar( &r1, -100.0, 100.0 );
        MexRadians startSpeed = mexRandomScalar( &r1, -100.0, 100.0 );
        size_t likelySegmentCount = mexRandomInt( &r1, 0, 100 );

        if( mexRandomInt( &r1, 2 ) == 0 )
        {
            MexPoint3d position( randomPoint3d( &r1 ) );

            PhysTimedSpinPlan plan( axis, position, startAngle, startSpeed, likelySegmentCount );

            timedSpinPlanTest( &r1, &plan );
        }
        else
        {
            MexTransform3d baseTransform( randomTransform3d( &r1 ) );

            PhysTimedSpinPlan plan( axis, baseTransform, startAngle, startSpeed, likelySegmentCount );

            timedSpinPlanTest( &r1, &plan );
        }
    }
}

void timedSpinPlanTest( MexBasicRandom* pR, PhysTimedSpinPlan* pPlan )
{
    int nSegments;

    if( mexRandomInt( pR, 2 ) )
        nSegments = mexRandomInt( pR, 0, 5 );
    else
        nSegments = mexRandomInt( pR, 0, 500 );

    for( size_t i = 0; i < nSegments; ++i )
    {
        const PhysRelativeTime segmentDuration = mexRandomScalar( pR, 0.0, 100.0 );
        const MexRadians acceleration = mexRandomScalar( pR, 0.0, 100.0 );

        pPlan->addSegment( segmentDuration, acceleration );
    }

    const size_t N_TESTS = 100;

    for( size_t i = 0; i < N_TESTS; ++i )
    {
        MexTransform3d  tx;

        PhysRelativeTime offset = mexRandomScalar( pR, 0.0, 10000.0 );

        pPlan->transform( offset, &tx );
        pPlan->angle( offset );
        pPlan->speed( offset );
    }
}

MexVec3 randomNonZeroVec3( MexBasicRandom* pR )
{
    MexVec3 vec;

    do
    {
        vec = randomVec3( pR );
    } while( vec.modulus() == 0.0 );

    return vec;
}

MexVec3 randomVec3( MexBasicRandom* pR )
{
    MexVec3 vec(
      mexRandomScalar( pR, -100.0, 100.0 ),
      mexRandomScalar( pR, -100.0, 100.0 ),
      mexRandomScalar( pR, -100.0, 100.0 ) );

    return vec;
}

MexPoint3d randomPoint3d( MexBasicRandom* pR )
{
    MexPoint3d point(
      mexRandomScalar( pR, -100.0, 100.0 ),
      mexRandomScalar( pR, -100.0, 100.0 ),
      mexRandomScalar( pR, -100.0, 100.0 ) );

    return point;
}

MexTransform3d randomTransform3d( MexBasicRandom* pR )
{
    const MexEulerAngles  angles(
      mexRandomScalar( pR, -100.0, 100.0 ),
      mexRandomScalar( pR, -100.0, 100.0 ),
      mexRandomScalar( pR, -100.0, 100.0 ) );

    MexTransform3d transform( angles, randomPoint3d( pR ) );

    return transform;
}

void motionChunkTest()
{
	const PhysMotionChunk ch1 = chunk1();
	const PhysMotionChunk ch2 = chunk2();

	PhysAbsoluteTime collideTime;

    if( ch1.intersects( ch2, &collideTime ) )
    {
		std::cout << "Chunks intersect at time " << collideTime << std::endl;
    }
    else
    {
		std::cout << "Chunks do not intersect" << std::endl;
    }

	const PhysRampAcceleration& ra2 = ch2.motionProfile();

	const uint N_STEPS = 100;
	const PhysRelativeTime timeIncrement = ra2.totalTime() / N_STEPS;

	const MexPoint2d& startPoint1 = ch1.startPoint();
	const MexPoint2d& startPoint2 = ch2.startPoint();
	const MexPoint2d& endPoint2 = ch2.endPoint();

	for( PhysRelativeTime t = 0.0; t < ra2.totalTime(); t += timeIncrement )
	{
	    const MATHEX_SCALAR distance = ra2.distance( t );
		const MATHEX_SCALAR f = distance / ra2.totalDistance();
	    const MexPoint2d point( startPoint2, endPoint2, f );

		std::cout << t + ch2.createTime() + ch2.motionTimeOffset();
		std::cout << "  " << point;
		std::cout << "  " << point.euclidianDistance( startPoint1 ) << std::endl;
	}

}

PhysMotionChunk chunk1()
{
	const MATHEX_SCALAR startSpeed = 0;
	const MATHEX_SCALAR firstAcceleration = 400;
	const MATHEX_SCALAR secondAcceleration = -400;
  	const PhysRelativeTime totalTime = 0.01 + 1.0;
	const PhysRelativeTime firstAccelerationTime = 0.005;
	const PhysRelativeTime secondAccelerationTime = 0.005;

    PhysRampAcceleration ramp(
        startSpeed,
        firstAcceleration,
        secondAcceleration,
        totalTime,
        firstAccelerationTime,
        secondAccelerationTime );

	const MexPoint2d startPoint(241.036,391.177);
	const MexPoint2d endPoint(241.036,391.177);
	const MATHEX_SCALAR clearance = 1.2;
    const PhysAbsoluteTime createTime = 2350.41 - 0.5;
    const PhysRelativeTime& motionTimeOffset = 0;
    const MexDouble minHeight = 0;
    const MexDouble maxHeight = 10.0;

    const PhysMotionChunk chunk(
		startPoint,
		endPoint,
		clearance,
		ramp,
		createTime,
		motionTimeOffset,
		minHeight,
		maxHeight );

	return chunk;
}

PhysMotionChunk chunk2()
{
	const MATHEX_SCALAR startSpeed = 0;
	const MATHEX_SCALAR firstAcceleration = 76;
	const MATHEX_SCALAR secondAcceleration = -76;
  	const PhysRelativeTime totalTime = 3.47951;
	const PhysRelativeTime firstAccelerationTime = 0.178541;
	const PhysRelativeTime secondAccelerationTime = 0.178541;

    PhysRampAcceleration ramp(
        startSpeed,
        firstAcceleration,
        secondAcceleration,
        totalTime,
        firstAccelerationTime,
        secondAccelerationTime );

	const MexPoint2d startPoint(240.03,382.756);
	const MexPoint2d endPoint(245.346,427.23);
	const MATHEX_SCALAR clearance = 1.2;
    const PhysAbsoluteTime createTime = 2349.29;
    const PhysRelativeTime& motionTimeOffset = 0.177004;
    const MexDouble minHeight = 0;
    const MexDouble maxHeight = 10.0;

    const PhysMotionChunk chunk(
		startPoint,
		endPoint,
		clearance,
		ramp,
		createTime,
		motionTimeOffset,
		minHeight,
		maxHeight );

	return chunk;
}


/*
 * P H Y S T E S T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//#include <conio.h>
#include <fstream>
#include <ctime>
#include "phys/rampacce.hpp"
#include "phys/eulertx.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"
#include "base/base.hpp"
//#include "stdlib/algorith.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/quatern.hpp"
#include "mathex/point3d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/random.hpp"
#include "phys/linemoti.hpp"
#include "phys/linetrav.hpp"
#include "phys/timespin.hpp"
#include "phys/accetumb.hpp"
#include "phys/absp2d.hpp"
#include "phys/motchunk.hpp"
#include "phystest/phytest.hpp"
#include "phystest/cs2dtest.hpp"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////////////

PhysTest::PhysTest()
{
	std::ofstream outputStream( "phystest.log" );
	time_t timeOfDay = time( NULL );

	outputStream << "Phys Test Log File: " << ctime( &timeOfDay );

	//Run the tests
    std::cout << "PhysTimedSpinPlan test "
         << (testPhysTimedSpinPlan( outputStream ) ? "passed" : "failed")
         << std::endl;

    std::cout << "PhysAlignedBSPTree2d test "
         << (testAlignedBSPTree2d( outputStream ) ? "passed" : "failed")
         << std::endl;

    std::cout << "PhysLinearTravelPlan test "
         << (testPhysLinearTravelPlan( outputStream ) ? "passed" : "failed")
         << std::endl;

    std::cout << "PhysAccelerationTumblePlan test "
         << (testPhysAccelerationTumblePlan( outputStream ) ? "passed" : "failed")
         << std::endl;

    {
        PhysTestConfigSpace2d spaceTest( outputStream ); //fails
    }

    std::cout << "Linear motion interpolation test "
         << (testPhysLinearMotionPlan( outputStream ) ? "passed" : "failed")
         << std::endl;

    // fails
    std::cout << "PhysMotionChunk test "
         << (testPhysMotionChunk( outputStream ) ? "passed" : "failed")
         << std::endl;

    std::cout << "PhysRampAcceleration distance / time test "
         << (testPhysRampAccelerationByDistanceTime( outputStream ) ? "passed" : "failed")
         << std::endl;

    std::cout << "PhysRampAcceleration speed test "
         << (testPhysRampAccelerationBySpeed( outputStream ) ? "passed" : "failed")
         << std::endl;


    std::cout  << "Press key to terminate" << std::endl;
    getchar();
    //while (!kbhit());
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysTest::~PhysTest()
{
	//Intentionally empty
}

//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTest::testPhysLinearMotionPlan( ostream& out) const
{
	bool okay = true;
    MATHEX_SCALAR piOver180 = 0.01745329252;

    //Construct 4 transforms for a path
    MexPoint3d pos1( 0, 0, 0 );
    MexPoint3d pos2( 400, 0, 200 );
    MexPoint3d pos3( 400, 300, 100 );
    MexPoint3d pos4( 0, 300, 0 );

//    MexQuaternion rot1( 0, 0, 0 );
//    MexQuaternion rot2( 0, 0, 90 * piOver180 );
//    MexQuaternion rot3( 0, 0, 180 * piOver180 );
//    MexQuaternion rot4( 0, 0, 180 * piOver180 );

    MexEulerAngles rot1( 0, 0, 0 );
    MexEulerAngles rot2( 0, 0, 90 * piOver180 );
    MexEulerAngles rot3( 0, 0, 180 * piOver180 );
    MexEulerAngles rot4( 0, 0, 180 * piOver180 );

    MexTransform3d txf1( rot1, pos1 );
    MexTransform3d txf2( rot2, pos2 );
    MexTransform3d txf3( rot3, pos3 );
    MexTransform3d txf4( rot4, pos4 );

    //Construct a PhysLinearMotionPlan from the 4 transforms
    PhysLinearMotionPlan plan( txf1, txf2, 4.0 );
    plan.add( txf3, 7.0 );
    plan.add( txf4, 9.0 );

    //Check the returned transform at the location times
    MexTransform3d tempTxf;
    plan.transform( 0.0, &tempTxf );
    if( not (tempTxf == txf1) )
    {
		out << "Linear Motion junction transform incorrect" << std::endl;
		okay = false;
    }

    plan.transform( 4.0, &tempTxf );
    if( not (tempTxf == txf2) )
    {
		out << "Linear Motion junction transform incorrect" << std::endl;
		okay = false;
    }

    plan.transform( 7.0, &tempTxf );
    if( not (tempTxf == txf3) )
    {
		out << "Linear Motion junction transform incorrect" << std::endl;
		okay = false;
    }

    plan.transform( 9.0, &tempTxf );
    if( not (tempTxf == txf4) )
    {
		out << "Linear Motion junction transform incorrect" << std::endl;
		okay = false;
    }

    plan.transform( 12.0, &tempTxf );
    if( not (tempTxf == txf4) )
    {
		out << "Linear Motion junction transform incorrect" << std::endl;
		okay = false;
    }

    //Now construct and test some intermediate positions
    MexPoint3d pos;
    MexEulerAngles rot;

    pos.x( 100 ); pos.y( 0 ); pos.z( 50 );
    rot.roll( 22.5 * piOver180 );
    MexTransform3d txfa( rot, pos );
    plan.transform( 1.0, &tempTxf );
    if( not (tempTxf == txfa) )
    {
		out << "Linear Motion interpolated transform incorrect" << std::endl;
		okay = false;
    }

    pos.x( 200 ); pos.y( 300 ); pos.z( 50 );
    rot.roll( 180.0 * piOver180 );
    MexTransform3d txfb( rot, pos );
    plan.transform( 8.0, &tempTxf );
    if( not (tempTxf == txfb) )
    {
		out << "Linear Motion interpolated transform incorrect" << std::endl;
		okay = false;
    }

    pos.x( 100 ); pos.y( 300 ); pos.z( 25 );
    rot.roll( 180.0 * piOver180 );
    MexTransform3d txfc( rot, pos );
    plan.transform( 8.5, &tempTxf );
    if( not (tempTxf == txfc) )
    {
		out << "Linear Motion interpolated transform incorrect" << std::endl;
		okay = false;
    }


	out << "PhysLinearMotionPlan test " << (okay ? "passed" : "failed!!!") << std::endl;

	return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTest::testPhysLinearTravelPlan( ostream& out) const
{
	bool okay = true;
    MATHEX_SCALAR piOver180 = 0.01745329252;

    //Construct 3 transforms for a path. First is rotation only, second translation only
    MexPoint3d pos1(10, 20, 30);
    MexPoint3d pos2(10, 20, 30);
    MexPoint3d pos3(40, 60, 30);
    MexEulerAngles rot1( 45*piOver180, 0, 0 );
    MexEulerAngles rot2( -45*piOver180, 0, 0 );
    MexEulerAngles rot3( -45*piOver180, 0, 0 );
    MexTransform3d txf1( rot1, pos1 );
    MexTransform3d txf2( rot2, pos2 );
    MexTransform3d txf3( rot3, pos3 );

    //Construct a vector of transforms with a syscounted pointer to it
    PhysMotionPlan::Transforms* pTransforms = _NEW( PhysMotionPlan::Transforms );
    PhysMotionPlan::TransformsPtr transformsPtr( pTransforms );
    pTransforms->push_back( txf1 );
    pTransforms->push_back( txf2 );
    pTransforms->push_back( txf3 );

    //Get the travel plan
    MATHEX_SCALAR translationSpeed = 50;
    MATHEX_SCALAR translationAcceleration = 100;
    MATHEX_SCALAR rotationSpeed = 200*piOver180;
    MATHEX_SCALAR rotationAcceleration = 360*piOver180;
    PhysLinearTravelPlan plan( transformsPtr, 0, translationSpeed, translationAcceleration,
                               translationAcceleration,
                               0, rotationSpeed, rotationAcceleration,
                               rotationAcceleration, true );

    //Check the duration
    okay = abs( plan.duration() - 2.5) < 0.00001;

    //Check the segment times
    if( okay )
    {
        PhysMotionPlan::Times& segmentTimes = *plan.segmentTimes();
        okay = abs( segmentTimes[0] - 1.0 ) < 0.00001   and
               abs( segmentTimes[1] - 2.5 ) < 0.00001;
    }

    //Construct the correct transforms at 0.25, 1.75
    MexPoint3d pos4(10, 20, 30);
    MexPoint3d pos5(25, 40, 30);
    MexEulerAngles rot4( 33.75*piOver180, 0, 0 );
    MexEulerAngles rot5( -45*piOver180, 0, 0 );
    MexTransform3d txf4( rot4, pos4 );
    MexTransform3d txf5( rot5, pos5 );

    //Check each one
    MexTransform3d result;

    plan.transform( 0.0, &result);
    if( okay ) okay = result == txf1;

    plan.transform( 1.0, &result);
    if( okay ) okay = result == txf2;

    plan.transform( 1.75, &result);
    if( okay ) okay = result == txf5;

    plan.transform( 2.5, &result);
    if( okay ) okay = result == txf3;

    plan.transform( 0.25, &result);
    if( okay ) okay = result == txf4;

    plan.transform( 4.0, &result);
    if( okay ) okay = result == txf3;

    if( not okay )
		out << "PhysLinearTravelPlan incorrect results" << std::endl;

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTest::testPhysTimedSpinPlan( ostream& out) const
{
	bool okay = true;

    //Construct plan
    MexVec3 pos(1,2,3);
    PhysTimedSpinPlan plan( MexVec3( 0, 0, 1), pos,MexRadians(50),MexRadians(0), 6);

    MexRadians acceleration( 100.0 );
    MexRadians deceleration( -100.0 );
    plan.addSegment( 2, acceleration );
    plan.addSegment( 2, deceleration );
    plan.addSegment( 1, MexRadians( 0 ) );
    plan.addSegment( 1, deceleration );
    plan.addSegment( 3, MexRadians( 0 ) );
    plan.addSegment( 1, acceleration );

    //Check angles at times
    if( okay )
        okay = fabs( plan.angle( 0 ).asScalar() - 50 ) < 0.00001;

    if( okay )
        okay = fabs( plan.angle( 1 ).asScalar() - 100 ) < 0.00001;

    if( okay )
        okay = fabs( plan.angle( 2 ).asScalar() - 250 ) < 0.00001;

    if( okay )
        okay = fabs( plan.angle( 3 ).asScalar() - 400 ) < 0.00001;

    if( okay )
        okay = fabs( plan.angle( 4.5 ).asScalar() - 450 ) < 0.00001;

    if( okay )
        okay = fabs( plan.angle( 7.5 ).asScalar() - 250 ) < 0.00001;

    if( okay )
        okay = fabs( plan.angle( 11 ).asScalar() - 50 ) < 0.00001;

    //Check some transforms
    MexTransform3d result;
    MexEulerAngles rot1(450, 0, 0);
    MexTransform3d txf1( rot1, pos );

    if( okay )
    {
        plan.transform( 4.5, &result );
        okay = result == txf1;
    }

    if( not okay )
		out << "PhysTimedSpinPlan incorrect results" << std::endl;

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTest::testPhysAccelerationTumblePlan( ostream& out ) const
{
     MexTransform3d  tx1( MexPoint3d( 10.4, 9.6, 0.0 ) );
     MexTransform3d  tx2( MexPoint3d( 11.4, 0.0, 0.0 ) );
     MexTransform3d  tx3( MexPoint3d( 12.4, 19.6, 0.0 ) );

     PhysAccelerateTumblePlan::EulerTransformsPtr transformsPtr(
       _NEW( PhysAccelerateTumblePlan::EulerTransforms ) );

     transformsPtr->push_back( tx1 );
     transformsPtr->push_back( tx2 );
     transformsPtr->push_back( tx3 );

     PhysMotionPlan::Times*  pTimes = _NEW( PhysMotionPlan::Times );
     pTimes->push_back( 5.0 );
     pTimes->push_back( 7.0 );

     PhysMotionPlan::TimesPtr        timesPtr( pTimes );

     MexVec3 acceleration( 0.0, 0.0, -9.81 );
     MATHEX_SCALAR tumbleRateRadiansPerSecond = 1.0;

     PhysAccelerateTumblePlan plan(
         transformsPtr,
         timesPtr,
         acceleration );

     for( size_t i = 0; i < 110; ++i )
     {
         PhysRelativeTime time = (PhysRelativeTime)i * plan.duration() / 100.0;

         MexTransform3d  result;

         plan.transform( time, &result );
         out << time << std::endl;
         out << result << std::endl;
     }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTest::testAlignedBSPTree2d( ostream& out ) const
{
	bool okay = true;

    //Construct a BSP tree for storing some lines
    MexPoint2d p1( -600, 400 );
    MexPoint2d p2( 1000, 1200 );
    PhysAlignedBSPTree2d< MexLine2d* > tree( p1, p2, 100.0, 3, 0 );

    //Define some lines
    MexLine2d l1( MexPoint2d( -500, 500), MexPoint2d( -400, 500 ) );
    MexLine2d l2( MexPoint2d( -400, 600), MexPoint2d( -500, 600 ) );
    MexLine2d l3( MexPoint2d( -300, 700), MexPoint2d( -300, 900 ) );
    MexLine2d l4( MexPoint2d( -400, 1100), MexPoint2d( -400, 900 ) );
    MexLine2d l5( MexPoint2d( -150, 450), MexPoint2d( -100, 450 ) );
    MexLine2d l6( MexPoint2d( -150, 500), MexPoint2d( -150, 700 ) );
    MexLine2d l7( MexPoint2d( -100, 500), MexPoint2d( -100, 700 ) );
    MexLine2d l8( MexPoint2d( -50, 500), MexPoint2d( -50, 700 ) );
    MexLine2d l9( MexPoint2d( -100, 750), MexPoint2d( -50, 750 ) );
    MexLine2d l10( MexPoint2d( 300, 500), MexPoint2d( 900, 500 ) );
    MexLine2d l11( MexPoint2d( 450, 850), MexPoint2d( 550, 850 ) );
    MexLine2d l12( MexPoint2d( 450, 900), MexPoint2d( 550, 900 ) );
    MexLine2d l13( MexPoint2d( 450, 950), MexPoint2d( 550, 950 ) );
    MexLine2d l14( MexPoint2d( 700, 900), MexPoint2d( 300, 1100 ) );
    MexLine2d l15( MexPoint2d( 350, 900), MexPoint2d( 250, 900 ) );

    //Add the lines
    tree.add( &l1 );
    tree.add( &l2 );
    tree.add( &l3);
    tree.add( &l4 );
    tree.add( &l5 );
    tree.add( &l6 );
    tree.add( &l7 );
    tree.add( &l8 );
    tree.add( &l9 );
    tree.add( &l10 );
    tree.add( &l11 );
    tree.add( &l12 );
    tree.add( &l13 );
    tree.add( &l14 );
    tree.add( &l15 );

    if( tree.nItems() != 15 ) okay = false;

    //Trace it
    out << tree;

    //Check some overlaps
    MexAlignedBox2d b1( MexPoint2d( -350, 650 ), MexPoint2d( 0, 750 ) );
    MexAlignedBox2d b2( MexPoint2d( 100, 550 ), MexPoint2d( 250, 850 ) );
    MexAlignedBox2d b3( MexPoint2d( 300, 750 ), MexPoint2d( 650, 1150 ) );

    ctl_vector< MexLine2d* > ps;
    if( okay and tree.overlapping( b1, &ps ) != 5 ) okay = false;
    if( okay and std::find( ps.begin(), ps.end(), &l3 ) == ps.end() ) okay = false;
    if( okay and std::find( ps.begin(), ps.end(), &l6 ) == ps.end() ) okay = false;
    if( okay and std::find( ps.begin(), ps.end(), &l7 ) == ps.end() ) okay = false;
    if( okay and std::find( ps.begin(), ps.end(), &l8 ) == ps.end() ) okay = false;
    if( okay and std::find( ps.begin(), ps.end(), &l9 ) == ps.end() ) okay = false;
    ps.erase( ps.begin(), ps.end() );

    if( okay and tree.overlapping( b2, &ps ) != 0 ) okay = false;

    tree.remove( &l12 );
    tree.remove( &l10 );
    if( tree.nItems() != 13 ) okay = false;

    if( okay and tree.overlapping( b3, &ps ) != 4 ) okay = false;
    if( okay and std::find( ps.begin(), ps.end(), &l11 ) == ps.end() ) okay = false;
    if( okay and std::find( ps.begin(), ps.end(), &l13 ) == ps.end() ) okay = false;
    if( okay and std::find( ps.begin(), ps.end(), &l14 ) == ps.end() ) okay = false;
    if( okay and std::find( ps.begin(), ps.end(), &l15 ) == ps.end() ) okay = false;

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////


void    bobTest( void )
{
    MATHEX_SCALAR   height = 10.0;

    PhysMotionPlan::Transforms* pTransforms = _NEW( PhysMotionPlan::Transforms );

    pTransforms->push_back( MexPoint3d( 0.0, 0.0, 0.0 ) );
    pTransforms->push_back( MexPoint3d( 0.0, 0.0, height ) );
    pTransforms->push_back( MexPoint3d( 0.0, 0.0, 0.0 ) );

    MATHEX_SCALAR startTranslationSpeed = 0.0;
    MATHEX_SCALAR translationSpeed = 3.0;
    MATHEX_SCALAR translationAcceleration = 0.1;
    MATHEX_SCALAR translationDeceleration = 0.5;
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

    PhysRelativeTime    duration = 20.0;

    for( size_t i = 0; i < 100; ++i )
    {
        PhysRelativeTime time = duration * i / 100.0;
        MexTransform3d  tx;

        pPlan->transform( time, &tx );

        std::cout << time << std::endl;
        std::cout << tx.position() << std::endl;
    }
}




bool PhysTest::testPhysMotionChunk( ostream& ) const
{
	bool okay = true;
    PhysAbsoluteTime collideTime;

    if( okay )
    {
        PhysRampAcceleration aProfile( 0.0, 25.0, -25.0, 7.577773343, 1.2, 1.2 );
        PhysRampAcceleration bProfile( 0.0, 25.0, -25.0, 0.979769469, 0.489884734, 0.489884734 );
        PhysMotionChunk aChunk( MexPoint2d( 714.719,849.588 ), MexPoint2d( 904.082,822.201 ), 3.0,
                                aProfile, 168.397, 3.83267, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        PhysMotionChunk bChunk( MexPoint2d(904.969,829.639), MexPoint2d(905.179,823.643), 3.0,
                                bProfile, 173.517, 2.04342, MexDouble::minusInfinity(), MexDouble::plusInfinity() );

        okay = aChunk.intersects( bChunk, &collideTime ) and
               bChunk.intersects( aChunk, &collideTime );
    }

    //Construct a pair of motion profiles to use
    PhysRampAcceleration aProfile( 0.0, 2.5, -10.0, 5.5, 2.0, 0.5 );
    PhysRampAcceleration bProfile( 0.0, 20.0, -5.0, 6.0, 0.5, 2.0 );

    //Construct a profile moving in x
    PhysMotionChunk aChunk( MexPoint2d( 300, 200 ), MexPoint2d( 321.25, 200 ), 2.0,
                            aProfile, 50.0, 2.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );

    //Now test against chunks in y

    if( okay )
    {
        //Intersects in both initial phases
        PhysMotionChunk bChunk( MexPoint2d( 300, 202.5 ), MexPoint2d( 300, 250 ), 0.5,
                                bProfile, 49.0, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               collideTime == 50.0 and
               bChunk.intersects( aChunk, &collideTime ) and
               collideTime == 50.0;
    }

    if( okay )
    {
        //Intersects in (a) initial phase (b) accn1 phase
        PhysMotionChunk bChunk( MexPoint2d( 300, 195 ), MexPoint2d( 300, 242.5 ), 0.5,
                                bProfile, 49.0, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               collideTime == 50.0 and
               bChunk.intersects( aChunk, &collideTime ) and
               collideTime == 50.0;
    }

    if( okay )
    {
        //Intersects in (a) initial phase (b) constant speed phase
        PhysMotionChunk bChunk( MexPoint2d( 300, 180 ), MexPoint2d( 300, 227.5 ), 0.5,
                                bProfile, 48.0, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 51.0 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 51.0 ) < 0.0001;
    }

    if( okay )
    {
        //Intersects in (a) initial phase (b) accn2 phase
        PhysMotionChunk bChunk( MexPoint2d( 300, 150 ), MexPoint2d( 300, 197.5 ), 0.5,
                                bProfile, 47.0, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 52.0 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 52.0 ) < 0.0001;
    }

    if( okay )
    {
        //Intersects in (a) initial phase (b) final phase
        PhysMotionChunk bChunk( MexPoint2d( 300, 150 ), MexPoint2d( 300, 197.5 ), 0.5,
                                bProfile, 42.0, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 50.0 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 50.0 ) < 0.0001;
    }

    if( okay )
    {
        //Intersects in (a) acc1 phase (b) acc1 phase
        PhysMotionChunk bChunk( MexPoint2d( 305, 195 ), MexPoint2d( 305, 242.5 ), 0.5,
                                bProfile, 52.5, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 53.5 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 53.5 ) < 0.0001;
    }

    if( okay )
    {
        //Intersects in (a) acc1 phase (b) cs phase
        PhysMotionChunk bChunk( MexPoint2d( 305, 160 ), MexPoint2d( 305, 207.5 ), 0.5,
                                bProfile, 49, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 52.0 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 52.0 ) < 0.0001;
    }

    if( okay )
    {
        //Intersects in (a) acc1 phase (b) acc2 phase
        PhysMotionChunk bChunk( MexPoint2d( 305, 150 ), MexPoint2d( 305, 197.5 ), 0.5,
                                bProfile, 47, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 52.0 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 52.0 ) < 0.0001;
    }

    if( okay )
    {
        //Intersects in (a) acc1 phase (b) final phase
        PhysMotionChunk bChunk( MexPoint2d( 305, 150 ), MexPoint2d( 305, 197.5 ), 0.5,
                                bProfile, 40, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 52.0 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 52.0 ) < 0.0001;
    }

    if( okay )
    {
        //Intersects in (a) cs phase (b) cs phase
        PhysMotionChunk bChunk( MexPoint2d( 312.5, 180 ), MexPoint2d( 312.5, 227.5 ), 0.5,
                                bProfile, 52.5, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 55.5 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 55.5 ) < 0.0001;
    }

    if( okay )
    {
        //Intersects in (a) cs phase (b) acc2 phase
        PhysMotionChunk bChunk( MexPoint2d( 312.5, 150 ), MexPoint2d( 312.5, 197.5 ), 0.5,
                                bProfile, 48.5, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 54.0 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 54.0 ) < 0.0001;
    }

    if( okay )
    {
        //Intersects in (a) cs phase (b) final phase
        PhysMotionChunk bChunk( MexPoint2d( 312.5, 150 ), MexPoint2d( 312.5, 197.5 ), 0.5,
                                bProfile, 40.0, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 55.5 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 55.5 ) < 0.0001;
    }

    if( okay )
    {
        //Intersects in (a) acc2 phase (b) acc2 phase
        PhysMotionChunk bChunk( MexPoint2d( 321, 150 ), MexPoint2d( 321, 197.5 ), 0.5,
                                bProfile, 50.25, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 57.0 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 57.0 ) < 0.0001;
    }

    if( okay )
    {
        //Intersects in (a) acc2 phase (b) final phase
        PhysMotionChunk bChunk( MexPoint2d( 321, 150 ), MexPoint2d( 321, 197.5 ), 0.5,
                                bProfile, 40.0, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = aChunk.intersects( bChunk, &collideTime ) and
               fabs( collideTime - 57.0 ) < 0.0001 and
               bChunk.intersects( aChunk, &collideTime ) and
               fabs( collideTime - 57.0 ) < 0.0001;
    }

    if( okay )
    {
        //(a) follows (b) 5.5 metres behind, same profile
        PhysMotionChunk bChunk( MexPoint2d( 308, 200 ), MexPoint2d( 329.25, 200 ), 0.5,
                                aProfile, 50.0, 2.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = not aChunk.intersects( bChunk, &collideTime ) and
               not bChunk.intersects( aChunk, &collideTime );
    }

    if( okay )
    {
        //(a) follows (b) 0.5 metres behind, different profile
        PhysMotionChunk bChunk( MexPoint2d( 303, 200 ), MexPoint2d( 350.5, 200 ), 0.5,
                                bProfile, 50.0, 1.0, MexDouble::minusInfinity(), MexDouble::plusInfinity() );
        okay = not aChunk.intersects( bChunk, &collideTime ) and
               not bChunk.intersects( aChunk, &collideTime );
    }

    return okay;
}


bool PhysTest::testPhysRampAccelerationByDistanceTime( ostream& ) const
{

    MATHEX_SCALAR testValues[][ 7 ] =
    {
    // distance startSpeed desiredSpeed endSpeed time       acc     decc
        0.02765,    0,         14.85,    14.85, 0.003028,   100,    100,
        19.44,   	0,         14.85,    14.85,  1.384,     100,    100,
        128,        0,         14.85,    14.85,  8.698,     100,    100,
        80.051,   	8,       	8,  	  8, 	10.0105,	6144,	6144,
        36.93,      5,	        5,        5,	7.385,  	3,      3,
        935.05859375, 16.34521484375 ,	36.859130859375,	53.8055419921875, 0.1595211791992188, 36074.515625, 36074.515625,
    };

    for( size_t i = 0; i < elementsof( testValues ); ++i )
    {
        const MATHEX_SCALAR desiredDistance = testValues[ i ][ 0 ];
        const MATHEX_SCALAR startSpeed = testValues[ i ][ 1 ];
        const MATHEX_SCALAR desiredSpeed = testValues[ i ][ 2 ];
        const MATHEX_SCALAR endSpeed = testValues[ i ][ 3 ];
        const MATHEX_SCALAR time = testValues[ i ][ 4 ];
        const MATHEX_SCALAR accelerationRate = testValues[ i ][ 5 ];
        const MATHEX_SCALAR decelerationRate = testValues[ i ][ 6 ];

        PhysRampAcceleration    ramp(
          desiredDistance,
          startSpeed,
          desiredSpeed,
          endSpeed,
          time,
          accelerationRate,
          decelerationRate,
          PhysRampAcceleration::BY_DISTANCE_TIME );
    }

    const uint N_RANDOM_TESTS = 10;

    MexBasicRandom  r1;

    r1.seed( 20 );

    for( size_t i = 0; i < N_RANDOM_TESTS; ++i )
    {
        MATHEX_SCALAR   desiredDistance, startSpeed, desiredSpeed, endSpeed;
        MATHEX_SCALAR   time, accelerationRate, decelerationRate ;

        do
        {
            desiredDistance = mexRandomScalar( &r1, -1000.0, 1000.0 );
        } while( desiredDistance == 0.0 );

        if( mexRandomInt( &r1, 2 ) == 0 or desiredDistance < 0.0 )
            startSpeed = 0.0;
        else
            startSpeed = mexRandomScalar( &r1, 0.0, 100.0 );

        desiredSpeed = mexRandomScalar( &r1, -100.0, 100.0 );

        if( mexRandomInt( &r1, 2 ) == 0 or desiredDistance < 0.0  )
            endSpeed = 0.0;
        else
            endSpeed = mexRandomScalar( &r1, 0.0, 100.0 );

        time = mexRandomScalar( &r1, 0.01, 100.0 );
        accelerationRate = mexRandomScalar( &r1, 0.1, 100.0 );

        if( mexRandomInt( &r1, 2 ) == 0 )
            decelerationRate = accelerationRate;
        else
            decelerationRate = mexRandomScalar( &r1, 0.1, 100.0 );

        PhysRampAcceleration    ramp(
          desiredDistance,
          startSpeed,
          desiredSpeed,
          endSpeed,
          time,
          accelerationRate,
          decelerationRate,
          PhysRampAcceleration::BY_DISTANCE_TIME );
    }

    const bool okay = true;

    return okay;
}

bool PhysTest::testPhysRampAccelerationBySpeed( ostream& ) const
{
    MATHEX_SCALAR testValues[][ 6 ] =
    {
    // distance startSpeed desiredSpeed endSpeed acc     decc
        1.565,      0,          8,        8,      10,     10,
        0,          0,         1.309,     0,      10,     10,
      180.542,		20.0836,	99.1364,  0,	10.4717,  15.8405,
      309.448,	    42.4347,	0,		50.4486,  98,	98,
    };

    for( size_t i = 0; i < elementsof( testValues ); ++i )
    {
        const MATHEX_SCALAR desiredDistance = testValues[ i ][ 0 ];
        const MATHEX_SCALAR startSpeed = testValues[ i ][ 1 ];
        const MATHEX_SCALAR desiredSpeed = testValues[ i ][ 2 ];
        const MATHEX_SCALAR endSpeed = testValues[ i ][ 3 ];
        const MATHEX_SCALAR accelerationRate = testValues[ i ][ 4 ];
        const MATHEX_SCALAR decelerationRate = testValues[ i ][ 5 ];

        PhysRampAcceleration    ramp(
          desiredDistance,
          startSpeed,
          desiredSpeed,
          endSpeed,
          accelerationRate,
          decelerationRate,
          PhysRampAcceleration::BY_SPEED );
    }

    const uint N_RANDOM_TESTS = 10;

    MexBasicRandom  r1;

    r1.seed( 20 );

    for( size_t i = 0; i < N_RANDOM_TESTS; ++i )
    {
        MATHEX_SCALAR   desiredDistance, startSpeed, desiredSpeed, endSpeed;
        MATHEX_SCALAR   time, accelerationRate, decelerationRate ;

        desiredDistance = mexRandomScalar( &r1, -1000.0, 1000.0 );

        if( mexRandomInt( &r1, 2 ) == 0 or desiredDistance <= 0.0 )
            startSpeed = 0.0;
        else
            startSpeed = mexRandomScalar( &r1, 0.0, 100.0 );

        if( desiredDistance >= 0.0 )
            desiredSpeed = mexRandomScalar( &r1, 0.0, 100.0 );
        else
            desiredSpeed = mexRandomScalar( &r1, -100.0, 0.0 );

        if( mexRandomInt( &r1, 2 ) == 0 or desiredDistance <= 0.0  )
            endSpeed = 0.0;
        else
            endSpeed = mexRandomScalar( &r1, 0.0, 100.0 );

        accelerationRate = mexRandomScalar( &r1, 0.1, 100.0 );

        if( mexRandomInt( &r1, 2 ) == 0 )
            decelerationRate = accelerationRate;
        else
            decelerationRate = mexRandomScalar( &r1, 0.1, 100.0 );

        PhysRampAcceleration    ramp(
          desiredDistance,
          startSpeed,
          desiredSpeed,
          endSpeed,
          accelerationRate,
          decelerationRate,
          PhysRampAcceleration::BY_SPEED );
    }

    const bool okay = true;

    return okay;
}

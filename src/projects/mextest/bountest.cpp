
/*
 * B O U N T E S T . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#include "mextest/bountest.hpp"
// #include "system/sysrate.hpp"
#include <conio.h>
#include <algorithm>

#include "mathex/quatern.hpp"
#include "mathex/radians.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/circle2d.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/cvexpgon.hpp"
#include "mathex/sausag2d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/vec3.hpp"
#include "mathex/line2d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/transf3d.hpp"

#include "mathex/hlfpln2d.hpp"
//#include "mathex/hlfspc3d.hpp"
#include "mathex/plane3d.hpp"
#include "mathex/quad3d.hpp"
#include "mathex/mathex.hpp"
#include "mathex/angle.hpp"
//#include "mathex/ubox3d.hpp"
#include "mathex/tri2d.hpp"
#include "mathex/degrees.hpp"

///////////////////////////////////

using namespace std;

BoundaryTest::BoundaryTest()
// creates the log file and tests all of the methods continuously until the
// user presses a key
:	numberOfTests_( 0 ),
	numberOfFailures_( 0 )
{
	ofstream outputStream( "bountest.log" );
	time_t timeOfDay = time( NULL );
//    SysRate testRate;

	outputStream << "Intersection Test Log File: " << ctime( &timeOfDay );

	timeSeed_ = 857751682UL;
//	timeSeed_ = time( &timeOfDay );

	//Inverse transform test
	if (testInverseTransform( outputStream ))
		cout << "Inverse transform test passed ok." << endl;
	else
		cout << "Inverse transform test failed!!!" << endl;

    //Do one-time persistence test
    if (testPersistence( outputStream ))
        cout << "Persistence test passed ok" << endl;
    else
        cout << "Persistence test failed!!!" << endl;

    //Do one-time test
    if (testLine3dIntersectsTriangle( outputStream ))
        cout << "testLine3dIntersectsTriangle test passed ok" << endl;
    else
        cout << "testLine3dIntersectsTriangle test failed!!!" << endl;

    //Do one-time test
    if (testAlignedBoxIntersectsLine( outputStream ))
        cout << "AlignedBoxIntersectsLine test passed ok" << endl;
    else
        cout << "AlignedBoxIntersectsLine test failed!!!" << endl;

    //Do one-time sausage test
    if (testSausageIntersect( outputStream ))
        cout << "Sausage test passed ok" << endl;
    else
        cout << "Sausage test failed!!!" << endl;

    //Do one-time ref-counted pointer test for MexBoundary
    if (testMexBoundary3dPtr( outputStream ))
        cout << "MexBoundary ref-count test passed ok" << endl;
    else
        cout << "MexBoundary ref-count test failed!!!" << endl;

    //Do one-time EulerParameter test
    if (testEulerParameters( outputStream ))
        cout << "Euler parameter test passed ok" << endl;
    else
        cout << "Euler parameter test failed!!!" << endl;

    //Do one-time Polygon intersect test
    if (testPolygonIntersect( outputStream ))
        cout << "Polygon Intersect test passed ok" << endl;
    else
        cout << "Polygon Intersect test failed!!!" << endl;


	for( ; !kbhit(); )
	{
		srand( timeSeed_ );    // initialise the psuedo-random number generator
//        testRate.update( numberOfTests_ );

		intersectAllOk( outputStream );
//		intersectUnalignedBox3dHalfSpace3dOk( outputStream );
//		intersectUnalignedBox3dUnalignedBox3dOk( outputStream );
//        intersectUnalignedBox3dLine3dOk( outputStream );

	  	timeSeed_++;

        cout << numberOfTests_ << " tests    ";
        cout << numberOfFailures_ << " failures  ";
        cout << "\r";

//        cout << testRate.rate() << " tests / second    \r";

//        break;
		//getch();
	}

	outputStream << "Number of tests: " << numberOfTests_ << endl;
	outputStream << "Number of failures: " << numberOfFailures_ << endl;
}

BoundaryTest::~BoundaryTest()
{
	/* intentionally empty */
}

bool BoundaryTest::intersectAllOk( ostream& out )
// makes one call to each of the intersect test methods
{
	bool    ok;

	ok = intersectPoint3dPoint3dOk( out );
	ok = ok and intersectPoint3dAlignedBox3dOk( out );
	ok = ok and intersectPoint3dUnalignedBox3dOk( out );
  	ok = ok and intersectPoint3dLine3dOk( out );
	ok = ok and intersectPoint3dPlane3dOk( out );
//	ok = ok and intersectPoint3dHalfSpace3dOk( out );

	ok = ok and intersectAlignedBox3dAlignedBox3dOk( out );
	ok = ok and intersectAlignedBox3dUnalignedBox3dOk( out );
	ok = ok and intersectAlignedBox3dLine3dOk( out );
	ok = ok and intersectAlignedBox3dPlane3dOk( out );
//	ok = ok and intersectAlignedBox3dHalfSpace3dOk( out );

	ok = ok and intersectUnalignedBox3dUnalignedBox3dOk( out );
	ok = ok and intersectUnalignedBox3dLine3dOk( out );
	ok = ok and intersectUnalignedBox3dPlane3dOk( out );
//	ok = ok and intersectUnalignedBox3dHalfSpace3dOk( out );

	ok = ok and intersectLine3dLine3dOk( out );
	ok = ok and intersectLine3dPlane3dOk( out );
//	ok = ok and intersectLine3dHalfSpace3dOk( out );

	ok = ok and intersectPlane3dPlane3dOk( out );
//	ok = ok and intersectPlane3dHalfSpace3dOk( out );

//	ok = ok and intersectHalfSpace3dHalfSpace3dOk( out );

    return ok;
}

bool BoundaryTest::intersectPoint3dPoint3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;

	//out << "intersecting Point3d and Point3d" << endl;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

//    IntersectionData    i;

	Point3 object1 = createRandomPoint3();
	Point3 object2;

	if( testIntersectingObjects )
	{
		// use the same object as we know these intersect
		object2 = object1;

		intersectionResult = object1.intersects( object2 );
	}
	else
	{
		TestType scalar = randomInt();
		if( scalar == 0 )
			++scalar;
		object2 = object1;

		object2 += scalar;		// cannot use *= due to zeroVector condition

		intersectionResult = object1.intersects( object2 );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;
		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexVec3 intersecting with MexVec3" << endl;
		out << "Object1: " << object1 << "   Object2: " << object2 << endl;


		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}

bool BoundaryTest::intersectPoint3dAlignedBox3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;
	Point3 object2;
    TestTransform3d identityTransform;

	//out << "intersecting Vec3 and AlignedBox" << endl;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

	TestAlignedBox3d object1 = createRandomAlignedBox3d();

	if( testIntersectingObjects )
	{
		object2 = createPointInsideAlignedBox3d( object1 );

		intersectionResult = object1.intersects( object2, identityTransform );
	}
	else
	{
		object2 = createPointOutsideAlignedBox3d( object1 );

		intersectionResult = object1.intersects( object2, identityTransform );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexVec3 intersecting with MexAlignedBox3d" << endl;
		out.precision( 12 );
		out << "Object1: " << object1 << endl;
		out << "Object2: " << object2 << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}

bool BoundaryTest::intersectPoint3dUnalignedBox3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;
    TestTransform3d identityTransform;

	Point3 object2;

	//out << "intersecting Vec3 and UnalignedBox" << endl;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

	TestUnalignedBox3d object1 = createRandomUnalignedBox3d();
//	cout << "objecy1 = " << object1 << endl;

	if( testIntersectingObjects )
	{
		// get a point which definitely intersects with the new box
		object2 = createPointInsideUnalignedBox3d( object1 );

		intersectionResult = object1.intersects( object2, identityTransform );
	}
	else
	{
		object2 = createPointOutsideUnalignedBox3d( object1 );

		intersectionResult = object1.intersects( object2, identityTransform );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexVec3 intersecting with MexUnalignedBox3d" << endl;
		out << "Object1: " << object1 << endl;
		out << "Object2: " << object2 << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}

bool BoundaryTest::intersectPoint3dLine3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;
	Point3 result;

	//out << "intersecting Vec3 and Line3d" << endl;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

	TestLine3d object1 = createRandomLine3d();

	if( testIntersectingObjects )
	{
		Point3 object2 = createPointInsideLine3d( object1 );

		intersectionResult = object1.intersects( object2 );

		result = object2;
	}
	else
	{
		Point3 object2 = createPointOutsideLine3d( object1 );

		intersectionResult = object1.intersects( object2 );

		result = object2;
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexVec3 intersecting with MexLine3d" << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}

bool BoundaryTest::intersectPoint3dPlane3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;

	//out << "intersecting Vec3 and Plane3d" << endl;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

	TestPlane3d object1 = createRandomPlane3d();
	Point3 object2;

	if( testIntersectingObjects )
	{
		object2 = createPointInsidePlane3d( object1 );

		intersectionResult = object1.intersects( object2 );
	}
	else
	{
		object2 = createPointOutsidePlane3d( object1 );

		intersectionResult = object1.intersects( object2 );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexVec3 intersecting with MexPlane3d" << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		out << "Plane (a,b,c,d): " << object1.a() << ", " << object1.b()
			<< ", " << object1.c() << ", " << object1.d() << endl;
		out << "Point: " << object2 << endl;
	}

	++numberOfTests_;

	return intersectionResult == testIntersectingObjects;
}
/*
bool BoundaryTest::intersectPoint3dHalfSpace3dOk( ostream& out )
{
	// out << "Vec3 intersecting with HalfSpace3d" << endl;
	++numberOfTests_;

	bool testIntersectingObjects = true;

    TestType x = randomFloat();
    TestType y = randomFloat();
    TestType z = randomFloat();

    Point3 p1(x,y,z);

    x = randomFloat();
    y = randomFloat();
    z = randomFloat();

    Point3 p2(x,y,z);

    x = randomFloat();
    y = randomFloat();
    z = randomFloat();

    Point3 p3(x,y,z);

	bool testResult=true;

    TestHalfSpace3d object1( p1, p2, p3 );

    if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;
		testIntersectingObjects = true;

	if( testIntersectingObjects )
	{
		Point3 object2( createPointInsideHalfSpace3d( object1 ) );
		//cout << "got here past create Random HalfSpacePoint" << endl;
		testResult = object1.intersects( object2 );
	}
	else
	{
	    Point3 object2( createPointOutsideHalfSpace3d( object1 ) );
		testResult = object1.intersects( object2 );
	}

	if( testResult != testIntersectingObjects )
	{
		++ numberOfFailures_;
		out << "ERROR******* HalfSpace3d intersecting with Vec3" << endl;
		synsug(testIntersectingObjects,out);
				// output info about objects here
	}

	return (testIntersectingObjects == testResult);

}
*/

bool BoundaryTest::intersectAlignedBox3dAlignedBox3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;
    TestTransform3d worldTransform;

	//out << "Intersecting AlignedBox3d and AlignedBox3d" << endl;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

	TestAlignedBox3d object1 = createRandomAlignedBox3d();

	if( testIntersectingObjects )
	{
		TestAlignedBox3d object2 = createAlignedBoxIntersectsAlignedBox( object1 );

        //Sometimes make the 2 boxes spin-safe or realignable, and use a non-identity transform
    	if( randomInt() & 1 )
        {
            randomInt();
            if( randomInt() & 1 )
            {
                object1.makeSpinSafe();
                object2.makeSpinSafe();
            }
            else
            {
                object1.realign( true );
                object2.realign( true );
            }
            worldTransform = createRandomTransform3d();
        }

		intersectionResult = object1.intersects( object2, worldTransform, worldTransform );
	}
	else
	{
		TestAlignedBox3d object2 = createAlignedBoxNotIntersectsAlignedBox( object1 );

		intersectionResult = object1.intersects( object2, worldTransform, worldTransform );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexAlignedBox3d intersecting with MexAlignedBox3d" << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}

bool BoundaryTest::intersectAlignedBox3dUnalignedBox3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;
    TestTransform3d worldTransform;

/*	Point3 up1( 20, 20, 20 );
	Point3 up2( 30, 30, 30 );
	TestAlignedBox3d temp( up1, up2 );
	TestUnalignedBox3d object2( temp );
*/
	TestUnalignedBox3d object2;

	//out << "Intersecting AlignedBox3d and AlignedBox3d" << endl;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

/*	Point3 p1( 0, 0, 0 );
	Point3 p2( 10, 10, 10 );
	TestAlignedBox3d object1( p1, p2 );// = createRandomAlignedBox3d();
*/
	TestAlignedBox3d object1 = createRandomAlignedBox3d();

	if( testIntersectingObjects )
	{
		object2 = createUnalignedBoxIntersectsAlignedBox( object1 );

        //Sometimes make the aligned box spin safe and use a proper random world transform
        if( randomInt() & 1 )
        {
            object1.makeSpinSafe();
            worldTransform = createRandomTransform3d();
        }
		intersectionResult = object1.intersects( object2, worldTransform, worldTransform );
	}
	else
	{
		object2 = createUnalignedBoxNotIntersectsAlignedBox( object1 );

		intersectionResult = object1.intersects( object2, worldTransform, worldTransform );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexAlignedBox3d intersecting with MexUnalignedBox3d" << endl;
		out << "Object1: " << object1 << endl;
		out << "Object2: " << object2 << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}

bool BoundaryTest::intersectAlignedBox3dLine3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;
    TestTransform3d identityTransform;

	//out << "Intersecting AlignedBox3d and AlignedBox3d" << endl;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

	TestAlignedBox3d object1 = createRandomAlignedBox3d();
	TestLine3d object2;

	if( testIntersectingObjects )
	{
		TestLine3d object2 = createLine3dIntersectsAlignedBox( object1 );

		intersectionResult = object1.intersects( object2, identityTransform );
	}
	else
	{
		TestLine3d object2 = createLine3dNotIntersectsAlignedBox( object1 );

		intersectionResult = object1.intersects( object2, identityTransform );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexAlignedBox3d intersecting with MexLine3d" << endl;
		out << "Object1 (ABOX): " << object1 << endl;
		out << "Object2 (LINE): " << object2.end1() << "   "
			<< object2.end2() << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}


bool BoundaryTest::intersectAlignedBox3dPlane3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;
    TestTransform3d identityTransform;

	//out << "Intersecting AlignedBox3d and AlignedBox3d" << endl;

	TestAlignedBox3d object2;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

	TestPlane3d object1 = createRandomPlane3d();

	if( testIntersectingObjects )
	{
		object2 = createAlignedBoxIntersectsPlane3d( object1 );

		intersectionResult = object2.intersects( object1, identityTransform );
	}
	else
	{
		object2 = createAlignedBoxNotIntersectsPlane3d( object1 );

		intersectionResult = object2.intersects( object1, identityTransform );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexAlignedBox3d intersecting with MexPlane3d" << endl;
		//out << "object1: " << object1 << endl;
		out << "object2: " << object2 << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}
/*
bool BoundaryTest::intersectAlignedBox3dHalfSpace3dOk( ostream& out )
{
    ++ numberOfTests_;
	bool testResult=true;
	TestHalfSpace3d object1(createRandomHalfSpace3d());

	bool testIntersectingObjects=true;

	if( ( rand() & 1 ) == 0 )
		testIntersectingObjects = false;


	if( testIntersectingObjects )
	{
		Point3 point1(createPointInsideHalfSpace3d(object1));
		Point3 point2(createRandomPoint3());

		TestAlignedBox3d object2(point1,point2);
		testResult = object1.intersects(object2);
	}
	else
	{
	    Point3 point1(createHalfSpacePoint(object1,-(randomFloat())));
		Point3 point2(createHalfSpacePoint(object1,-(randomFloat())));

		TestPlane3d plane(object1.boundary());
		TestVec3 offset(plane.normal());

		TestVec3 p1Vec(point1);
		TestVec3 p2Vec(point2);

		TestType distance=p1Vec.EuclidianDistance( &p2Vec);
	    //cout << distance << endl;

	    offset *= distance;

		p1Vec -= offset;
		p2Vec -= offset;

		point1 = p1Vec;
		point2 = p2Vec;

	    TestAlignedBox3d object2(point1,point2);
		testResult = object1.intersects(object2);
	}

	if( testResult != testIntersectingObjects )
	{
		++ numberOfFailures_;
		out << "ERROR******* HalfSpace3d intersecting with AlignedBox3d" << endl;
		synsug(testIntersectingObjects,out);
		// output info about objects here
	}

	return testResult;
}
*/
bool BoundaryTest::intersectUnalignedBox3dUnalignedBox3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;
    TestTransform3d worldTransform = createRandomTransform3d();

	TestUnalignedBox3d object2;

	if( ( randomInt() & 1 ) == 0 )
	  	testIntersectingObjects = false;

	TestUnalignedBox3d object1 = createRandomUnalignedBox3d();

	if( testIntersectingObjects )
	{
		//cout << "calling createUnalignedBoxIntersectsUnalignedBox " << endl;
		object2 = createUnalignedBoxIntersectsUnalignedBox( object1 );
	}
	else
	{
		object2 = createUnalignedBoxNotIntersectsUnalignedBox( object1 );
	}

    intersectionResult = object1.intersects( object2, worldTransform, worldTransform );

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexUnalignedBox3d intersecting with MexUnalignedBox3d" << endl;
		out << "Object1: " << object1 << endl;
		out << "Object2: " << object2 << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}

bool BoundaryTest::intersectUnalignedBox3dLine3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;
    TestTransform3d identityTransform;

	//out << "Intersecting AlignedBox3d and AlignedBox3d" << endl;

	TestLine3d object2;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

	TestUnalignedBox3d object1 = createRandomAlignedBox3d();

	if( testIntersectingObjects )
	{
		object2 = createLine3dIntersectsUnalignedBox( object1 );

		intersectionResult = object1.intersects( object2, identityTransform );
	}
	else
	{
		object2 = createLine3dNotIntersectsUnalignedBox( object1 );

		intersectionResult = object1.intersects( object2, identityTransform );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexUnalignedBox3d intersecting with MexLine3d" << endl;
		out << "Object1: " << object1 << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}

bool BoundaryTest::intersectUnalignedBox3dPlane3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;
    TestTransform3d identityTransform;

	//out << "Intersecting AlignedBox3d and AlignedBox3d" << endl;

	TestUnalignedBox3d object2;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

	TestPlane3d object1 = createRandomPlane3d();

	if( testIntersectingObjects )
	{
		object2 = createUnalignedBoxIntersectsPlane3d( object1 );

		intersectionResult = object2.intersects( object1, identityTransform );
	}
	else
	{
		object2 = createUnalignedBoxNotIntersectsPlane3d( object1 );

		intersectionResult = object2.intersects( object1, identityTransform );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexUnalignedBox3d intersecting with MexPlane3d" << endl;
		out << "Object2: " << object2 << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}

/*
bool BoundaryTest::intersectUnalignedBox3dHalfSpace3dOk( ostream& out )
{
	++ numberOfTests_;
	bool testResult=true;
	TestUnalignedBox3d object1;
	TestHalfSpace3d object2(createRandomHalfSpace3d());

	bool testIntersectingObjects=true;

	if( ( rand() & 1 ) == 0 )
		testIntersectingObjects = false;

	if( testIntersectingObjects )
	{

		Point3 point1( createPointInsideHalfSpace3d( object2 ) );
		Point3 point2( createPointOutsideHalfSpace3d( object2 ) );

		TestAlignedBox3d uBox(point1,point2);
		TestUnalignedBox3d box(uBox);
        object1 = box;
	}
	else
	{
		Point3 point1( createHalfSpacePoint( object2, -( randomFloat() ) ) );
		Point3 point2( createHalfSpacePoint( object2, -( randomFloat() ) ) );

		TestPlane3d plane(object2.boundary());
		Point3 offset(plane.normal());

		TestVec3 off1(offset);
		TestVec3 p1(point1);
		TestVec3 p2(point2);
        TestVec3* vPtr = &p2;


		TestType distance=p1.EuclidianDistance(vPtr);
	    //cout << distance << endl;

	    off1 *= distance;

		point1 -= off1;
		point2 -= off1;

	    TestAlignedBox3d uBox(point1,point2);
		TestUnalignedBox3d box(uBox);
        object1 = box;
	}

    TestTransform3d tx = createRandomTransform3d();

    object1.transform( tx );
    object2.transform( tx );

	testResult = object2.intersects(object1);

	if( testResult != testIntersectingObjects )
	{
		++ numberOfFailures_;
		out << "ERROR******* HalfSpace3d intersecting with UnalignedBox3d" << endl;
		synsug(testIntersectingObjects,out);

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here

        out << object1 << endl;
        out << object2 << endl;
	}

	return testIntersectingObjects == testResult;
}
*/
bool BoundaryTest::intersectLine3dLine3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;

	TestLine3d object2;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

	TestLine3d object1 = createRandomLine3d();

	if( testIntersectingObjects )
	{
		object2 = createLine3dIntersectsLine3d( object1 );

		intersectionResult = object1.intersects( object2 );
	}
	else
	{
		object2 = createLine3dNotIntersectsLine3d( object1 );

		intersectionResult = object1.intersects( object2 );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexLine3d intersecting with MexLine3d" << endl;
//		out << "Object1: " << object1 << endl;
//		out << "Object2: " << object2 << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}

bool BoundaryTest::intersectLine3dPlane3dOk( ostream& out )
{
	bool testIntersectingObjects = true;
	bool intersectionResult;

	TestLine3d object2;

	if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;

	TestPlane3d object1 = createRandomPlane3d();

	if( testIntersectingObjects )
	{
		object2 = createLine3dIntersectsPlane3d( object1 );

		intersectionResult = object1.intersects( object2 );
	}
	else
	{
		object2 = createLine3dNotIntersectsPlane3d( object1 );

		intersectionResult = object1.intersects( object2 );
	}

	if( intersectionResult != testIntersectingObjects )
	{
		++numberOfFailures_;

		out << "Seed: " << timeSeed_ << endl;
		out << "ERROR: MexLine3d intersecting with MexPlane3d" << endl;
		//out << "Object1: " << object1 << endl;
		//out << "Object2: " << object2 << endl;

		if( testIntersectingObjects )
			out << "Objects should have intersected but did not" << endl;
		else
			out << "Objects should not have intersected but did" << endl;

		// output info about objects here
	}

	++numberOfTests_;
	return intersectionResult == testIntersectingObjects;
}
/*
bool BoundaryTest::intersectLine3dHalfSpace3dOk( ostream& out )
{
	 //	out << "Line intersecting HalfSpace" << endl;
    ++numberOfTests_;

    bool testIntersectingObjects = true;


    TestType x = randomFloat();
    TestType y = randomFloat();
    TestType z = randomFloat();

    Point3 p1(x,y,z);

    x = randomFloat();
    y = randomFloat();
    z = randomFloat();

    Point3 p2(x,y,z);

    x = randomFloat();
    y = randomFloat();
    z = randomFloat();

    Point3 p3(x,y,z);

	bool testResult=true;

    TestHalfSpace3d object1(p1, p2, p3 );

	if( ( rand() & 1 ) == 0 )
		testIntersectingObjects = false;


	if( testIntersectingObjects )
	{
	    Point3 x(createPointInsideHalfSpace3d(object1));
		Point3 y(createRandomPoint3());
		TestLine3d object2(x,y);
		testResult = object1.intersects( object2 );
	}

	else
	{
	    Point3 x(createPointOutsideHalfSpace3d(object1));
		Point3 y(createPointOutsideHalfSpace3d(object1));
		TestLine3d object2(x,y);
		testResult = object1.intersects( object2 );

	}

	if( testResult != testIntersectingObjects )
	{
		++ numberOfFailures_;
		out << "ERROR******* HalfSpace3d intersecting with Line3d" << endl;
		synsug(testIntersectingObjects,out);
		// output info about objects here
	}

	return testIntersectingObjects == testResult;
}
*/
bool BoundaryTest::intersectPlane3dPlane3dOk( ostream& )
{
	return false;
}
/*
bool BoundaryTest::intersectPlane3dHalfSpace3dOk(ostream& out )
{
 //out << "Plane intersecting HalfSpace" << endl;
  	++numberOfTests_;

	 bool testIntersectingObjects = true;

	Point3 p1(randomFloat(),randomFloat(),randomFloat());
	Point3 p2(randomFloat(),randomFloat(),randomFloat());
	Point3 p3(randomFloat(),randomFloat(),randomFloat());

	//Point3 p1(10,0,5);
	//Point3 p2(0,10,5);
	//Point3 p3(10,10,5);


	bool testResult=true;

	//cout << "prior to halfSpace creation";
    //cout << p1 << endl << p2 << endl << p3;
    TestHalfSpace3d object1( p1, p2, p3 );
	//cout << endl << " first entrance into intersect func " << p1 << " " << p2 << " " << p3 << endl << endl;
	//cout << object1.boundary() << " prior to plane creation endl " << endl << endl;
	TestPlane3d halfSpaceBoundary(object1.boundary());
    //cout << endl << "from intersection of plane and halfspace test" << endl;
	//cout << halfSpaceBoundary << endl;

    if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = true;

	if( testIntersectingObjects )
	{

		//cout << "Testing intersecting cases" << endl;

		Point3 t(createPointInsideHalfSpace3d(object1));
		// << t << " << vector in half space " << endl;
		Point3 u(createRandomPoint3());
		Point3 z(createRandomPoint3());

		TestPlane3d object2( t, u, z );
	    testResult = object1.intersects( object2 );
	}
	else
	{
	    //Point3 t1(createPointInsidePlane3d(halfSpaceBoundary));
		//Point3 t2(createPointInsidePlane3d(halfSpaceBoundary));
		//Point3 t3(createPointInsidePlane3d(halfSpaceBoundary));

	  	//cout << " non Intersecting case " << endl;

		TestType scalar = randomFloat();
		   if (scalar==0)
		      scalar+=2;

		Point3 offset = halfSpaceBoundary.normal();

        //cout << "prior to scalar multiplication " << endl;
		TestVec3 off1(offset);
		off1 *= scalar;

		//cout << "prior to adding to t1" << t1 << endl << t2 << endl << t3 << endl;


		//cout << " offset (halfspace normal) " << offset << endl;
		p1 -= off1;
		p2 -= off1;
		p3 -= off1;
// 		assert(object1.pointRelativeToHalfSpace(p1)==false);
// 		assert(object1.pointRelativeToHalfSpace(p2)==false);
// 		assert(object1.pointRelativeToHalfSpace(p3)==false);

	    TestPlane3d object2(p1,p2,p3);


		//cout << " post creation of object2 " << endl;
		//cout << "object1 normal " << halfSpaceBoundary.normal() << endl;
		//cout << "object2 normal " << object2.normal() << endl;

		testResult = object1.intersects( object2 );

	    //cout << "end of test" << endl;
	}

	if( testResult != testIntersectingObjects )
	{
		++ numberOfFailures_;
		out << "ERROR******* HalfSpace3d intersecting with Plane3d" << endl;
		synsug(testIntersectingObjects,out);
				// output info about objects here
	}

	return testIntersectingObjects==testResult;

}

bool BoundaryTest::intersectHalfSpace3dHalfSpace3dOk( ostream& out )
{
 //out << "HalfSpace intersecting HalfSpace" << endl;
  ++numberOfTests_;

 bool testIntersectingObjects = true;


    TestType x = randomFloat();
    TestType y = randomFloat();
    TestType z = randomFloat();

    Point3 p1(x,y,z);

    x = randomFloat();
    y = randomFloat();
    z = randomFloat();

    Point3 p2(x,y,z);

    x = randomFloat();
    y = randomFloat();
    z = randomFloat();

    Point3 p3(x,y,z);

	bool testResult=true;

    TestHalfSpace3d object1( p1, p2, p3 );

	TestType scalar = randomFloat();
    if( ( randomInt() & 1 ) == 0 )
		testIntersectingObjects = false;


	if( testIntersectingObjects )
	{
		Point3 t(createPointInsideHalfSpace3d(object1));
		// << t << " << vector in half space " << endl;
		Point3 u(createRandomPoint3());
		Point3 z(createRandomPoint3());


		TestHalfSpace3d object2( t,u,z );
		testResult = object1.intersects(object2);
	}
	else
	{

	    TestPlane3d halfSpaceBoundary(object1.boundary());
	    TestType scalar= randomFloat() * (-1);
		if (scalar == 0)
		  	scalar += 2;

        p1 = createHalfSpacePoint(object1,scalar);
		p2 = createHalfSpacePoint(object1,scalar);
		p3 = createHalfSpacePoint(object1,scalar);

	    TestHalfSpace3d object2( p1, p2, p3);
		testResult = object1.intersects(object2);
	}

	if( testResult != testIntersectingObjects )
	{
		++ numberOfFailures_;
		out << "ERROR******* HalfSpace3d intersecting with HalfSpace3d" << endl;
		synsug(testIntersectingObjects,out);
				// output info about objects here
	}

	return testIntersectingObjects==testResult;

}
*/
///////////////////////////////////////////////////////////////////


Point3 BoundaryTest::createRandomPoint3() const
{
	TestType p1 = randomFloat( MexEpsilon::instance(), 20000.0 );
	TestType p2 = randomFloat( MexEpsilon::instance(), 20000.0 );
	TestType p3 = randomFloat( MexEpsilon::instance(), 20000.0 );

	Point3 vector( p1, p2, p3 );

	return vector;
}

TestVec3 BoundaryTest::createRandomVec3() const
{
	TestType p1 = randomFloat( MexEpsilon::instance(), 20000.0 );
	TestType p2 = randomFloat( MexEpsilon::instance(), 20000.0 );
	TestType p3 = randomFloat( MexEpsilon::instance(), 20000.0 );

	TestVec3 vector( p1, p2, p3 );

	return vector;
}

Point3 BoundaryTest::createRandomUnitVector() const
{
	TestVec3 v = createRandomVec3();

	v.makeUnitVector();

	return v;
}

TestAlignedBox3d BoundaryTest::createRandomAlignedBox3d() const
{
	Point3 p1 = createRandomPoint3();
	Point3 p2;

//MexEpsilon::instance()
    do
    {
        p2 = createRandomPoint3();
    } while (p1.x() == p2.x() or p1.y() == p2.y() or p1.z() == p2.z());

	Point3 minVec( min( p1.x(), p2.x() ),
					 min( p1.y(), p2.y() ),
					 min( p1.z(), p2.z() ) );

	Point3 maxVec( max( p1.x(), p2.x() ),
					 max( p1.y(), p2.y() ),
					 max( p1.z(), p2.z() ) );
 	TestAlignedBox3d aBox( minVec, maxVec );

	return aBox;
}

TestUnalignedBox3d BoundaryTest::createRandomUnalignedBox3d() const
{
	TestTransform3d t = createRandomTransform3d();
	Point3 v = createRandomPoint3();

	TestUnalignedBox3d uBox( v, t );

	return uBox;
}

TestLine3d BoundaryTest::createRandomLine3d() const
{
	Point3 p1 = createRandomPoint3();
	Point3 p2 = createRandomPoint3();

	Point3 minVec( min( p1.x(), p2.x() ),
					 min( p1.y(), p2.y() ),
					 min( p1.z(), p2.z() ) );

	Point3 maxVec( max( p1.x(), p2.x() ),
					 max( p1.y(), p2.y() ),
					 max( p1.z(), p2.z() ) );

	TestLine3d line( minVec, maxVec );

	return line;
}

TestPlane3d BoundaryTest::createRandomPlane3d() const
{
	Point3 p1 = createRandomPoint3();
	Point3 p2 = createRandomPoint3();
	Point3 p3 = createRandomPoint3();

	TestPlane3d result( p1, p2, p3 );

	return result;
}
/*
TestHalfSpace3d BoundaryTest::createRandomHalfSpace3d() const
{
	Point3 x(createRandomPoint3());
	Point3 y(createRandomPoint3());
	Point3 z(createRandomPoint3());
	TestHalfSpace3d result(x,y,z);

	return result;
}
*/
TestTransform3d BoundaryTest::createRandomTransform3d() const
{
	// create random azimuth, elevation and roll and a random vector
	TestType a = randomFloat( MexEpsilon::instance(), 20000.0 );
	TestType e = randomFloat( MexEpsilon::instance(), 20000.0 );
	TestType r = randomFloat( MexEpsilon::instance(), 20000.0 );

	TestEulerAngles eulerAngles( a, e, r );

//	MexEulerAngles< TestType > eulerAngles;
	Point3 v = createRandomPoint3();

	TestTransform3d result( eulerAngles, v );

	return result;
}

TestTransform3d BoundaryTest::createRandomTransform3d( const Point3& rhs ) const
{
	// create random azimuth, elevation and roll and a random vector
	TestType a = randomFloat( MexEpsilon::instance(), 20000.0 );
	TestType e = randomFloat( MexEpsilon::instance(), 20000.0 );
	TestType r = randomFloat( MexEpsilon::instance(), 20000.0 );

	TestEulerAngles eulerAngles( a, e, r );

//	MexEulerAngles< TestType > eulerAngles;

	Point3 v = rhs;

	TestTransform3d result( eulerAngles, v );

	return result;
}

Point3 BoundaryTest::createPointInsideAlignedBox3d( const TestAlignedBox3d& rhs ) const
{
    TestTransform3d identityTransform;
    const MexAlignedBox3d::Data& world = rhs.world( identityTransform );
	Point3 dimensions = world.maxCorner_;

	dimensions -= world.minCorner_;

	TestType x = randomFloat( MexEpsilon::instance(), dimensions.x() - MexEpsilon::instance() );
	TestType y = randomFloat( MexEpsilon::instance(), dimensions.y() - MexEpsilon::instance() );
	TestType z = randomFloat( MexEpsilon::instance(), dimensions.z() - MexEpsilon::instance() );

	// limits holds the dimensions of the aligned box.  now generate
	// a random point
	Point3 result( x, y, z );

	// now move the point to inside the box
	result += world.minCorner_;

	// POST(rhs.contains( result ) );

	return result;
}

Point3 BoundaryTest::createPointInsideUnalignedBox3d( const TestUnalignedBox3d& rhs ) const
{
//	cout << "in function createPointInsideUnalignedBox3d " << endl;
//	cout << "rhs: "<< rhs << endl;

/*	Point3 zero;
	TestAlignedBox3d aBox( zero, rhs.dimensions() );
	Point3 result = createPointInsideAlignedBox3d( aBox );
*/
	Point3 dimensions = rhs.dimensions();

//	cout << "dimensions: " << dimensions << endl;

	// random points within the range of the aligned representation
	// this code seems to be okay
	TestType x = randomFloat( MexEpsilon::instance(), dimensions.x() - MexEpsilon::instance() );
	TestType y = randomFloat( MexEpsilon::instance(), dimensions.y() - MexEpsilon::instance() );
	TestType z = randomFloat( MexEpsilon::instance(), dimensions.z() - MexEpsilon::instance() );

//	cout << "size of rhs: " << dimensions << endl;

	Point3 result( x, y, z );

//	cout << "result = " << result << endl;

 	// now tranform the point using the unaligned box's transform
/*	cout << "box.position: " << rhs.dimensions() << endl;
	cout << "box.transformposition: " << rhs.transformPosition() << endl;
	cout << "result before: " << result << endl;
*/
	// this could be the dodgy code
    TestTransform3d identityTransform;
    TestTransform3d t = rhs.overallTransform( identityTransform );

	t.transform( &result );

//	cout << "point inside = " << result << endl;

	POST( rhs.contains( result, identityTransform ) );

  //	cout << "result after: " << result << endl;

	return result;
}

Point3 BoundaryTest::createPointOutsideAlignedBox3d( const TestAlignedBox3d& rhs ) const
{
    TestTransform3d identityTransform;
    const MexAlignedBox3d::Data& world = rhs.world( identityTransform );
	Point3 limits = world.maxCorner_;
	Point3 result = createPointInsideAlignedBox3d( rhs );

	limits -= world.minCorner_;

	result += limits;

	TestType scalar = randomInt();
	if( scalar < MexEpsilon::instance() )
		scalar += 1;

	TestVec3 r1(result);

	r1 *= scalar;		// place it somewhere random in space

	// POST(rhs.contains( result ) == false );
	result = r1;

	return result;
}

Point3 BoundaryTest::createPointOutsideUnalignedBox3d( const TestUnalignedBox3d& rhs ) const
{
	Point3 zero;		// vector( 0, 0, 0 )
	Point3 dimensions = rhs.dimensions();

	// create an aligned box from the unaligned
	TestAlignedBox3d aBox( zero, dimensions );

	// find a point outside of it
	Point3 result = createPointOutsideAlignedBox3d( aBox );

	// the transform the vector
    TestTransform3d identityTransform;
	TestTransform3d t( rhs.overallTransform( identityTransform ) );

	t.transform( &result );

//	POST( rhs.contains( result ) == false );

	return result;
}

Point3 BoundaryTest::createPointInsideLine3d( const TestLine3d& rhs ) const
{
	Point3 d = rhs.end2();

	// calculate direction vector for line
	d -= rhs.end1();

	// need a random number between 0 and 1
	TestType i = ( randomFloat( MexEpsilon::instance(), RAND_MAX ) / (TestType)RAND_MAX );
	TestType x = d.x() * i;
	TestType y = d.y() * i;
	TestType z = d.z() * i;

	Point3 result( x, y, z );

	result += rhs.end1();
	d = result;

	POST( rhs.contains( d ) );

	return d;
}

Point3 BoundaryTest::createPointOutsideLine3d( const TestLine3d& rhs ) const
{
	TestLine3d line( createLine3dNotIntersectsLine3d( rhs ) );
	Point3 result = createPointInsideLine3d( line );

	POST( rhs.contains( result ) == false );

	return result;
}

Point3 BoundaryTest::createPointInsidePlane3d( const TestPlane3d& rhs ) const
{
	PRE( rhs.c() != 0.0 );

	TestType x = randomFloat();
	TestType y = randomFloat();
	TestType z = ( 0.0 - rhs.a() * x - rhs.b() * y - rhs.d() ) / rhs.c();

	Point3 v( x, y, z );

//	POST( rhs.contains( v ) );

	return v;
}

Point3 BoundaryTest::createPointOnPositiveSidePlane3d( const TestPlane3d& rhs ) const
{
	TestType x = randomFloat();
	TestType y = randomFloat();
	TestType positiveScalar = randomFloat( 1.0, 20000.0 );// don't want zero

	TestType z = ( positiveScalar - x * rhs.a() - y * rhs.b() - rhs.d() ) / rhs.c();

//	cout << "pos sc = " << positiveScalar << endl;

	Point3 v( x, y, z );

//	cout << "v = "<< v << endl;

//	POST( rhs.contains( v ) == false );

	return v;
}

Point3 BoundaryTest::createPointOnNegativeSidePlane3d( const TestPlane3d& rhs ) const
{
	TestType x = randomFloat();
	TestType y = randomFloat();
	TestType negativeScalar = ( -1.0 ) * randomFloat( 1.0, 20000.0 );

//	cout << "neg sc = " << negativeScalar << endl;

	TestType z = ( negativeScalar - x * rhs.a() - y * rhs.b() - rhs.d() ) / rhs.c();

	Point3 v( x, y, z );

//	cout << "v= " << v;

//	POST( rhs.contains( v ) == false );

	return v;
}

Point3 BoundaryTest::createPointOutsidePlane3d( const TestPlane3d& rhs ) const
{
	PRE( rhs.c() != 0.0 );

	if( ( randomInt() & 1 ) == 0 )
	{
//		cout << "creating point on negative side" << endl;
		return createPointOnNegativeSidePlane3d( rhs );
	}

//	cout << "creating point on positive side " << endl;
	return createPointOnPositiveSidePlane3d( rhs );
}
/*
Point3 BoundaryTest::createHalfSpacePoint(const TestHalfSpace3d &h,const TestType &d) const
{
	TestPlane3d bound(h.boundary());

//	assert(bound.c() != 0);

	Point3 offset = bound.normal();

    Point3 point;
	point = createPointInsidePlane3d( bound );

	TestVec3 o1(offset);

	o1 *= d;

	offset = o1;

	point += offset;


//    cout << d << " distance from HalfSpace ";

// 	if (d < 0)
// 		assert (h.pointRelativeToHalfSpace(point)==false);
// 	else
// 	    assert (h.pointRelativeToHalfSpace(point)==true);

	return (point);
}

Point3 BoundaryTest::createPointInsideHalfSpace3d(const TestHalfSpace3d &h) const
{
	TestPlane3d bound(h.boundary());

//	assert(bound.c() != 0);

	TestType negScalar = randomFloat();
	Point3 offset = bound.normal();

    Point3 point;
	point = createPointInsidePlane3d(bound);

	TestVec3 o1(offset);

	o1 *= negScalar;
	offset = o1;

	point += offset;

//	assert (h.pointRelativeToHalfSpace(point)==true);

	return (point);
}


Point3 BoundaryTest::createPointOutsideHalfSpace3d( const TestHalfSpace3d &h) const
{
	TestPlane3d bound(h.boundary());

//	assert(bound.c() != 0);

	TestType negScalar = randomFloat() * (-1);
	if (negScalar == 0)
	   negScalar --;

	Point3 offset = bound.normal();

    Point3 point;
	point = createPointInsidePlane3d(bound);

	TestVec3 o1(offset);

	o1 *= negScalar;

	offset = o1;

  	point  += offset;

	//if (h.pointRelativeToHalfSpace(point)==false)
	//    cout << point;

//	assert (h.pointRelativeToHalfSpace(point)==false);

	return point;
}
*/
TestAlignedBox3d BoundaryTest::createAlignedBoxIntersectsAlignedBox( const TestAlignedBox3d& rhs ) const
{
	// create a point inside the box
	TestLine3d l = createLine3dIntersectsAlignedBox( rhs );

	Point3 point1 = l.end1();
	Point3 point2 = l.end2();

	Point3 minCorner( min( point1.x(), point2.x() ),
		  	  		    min( point1.y(), point2.y() ),
				        min( point1.z(), point2.z() ) );

	Point3 maxCorner( max( point1.x(), point2.x() ),
				        max( point1.y(), point2.y() ),
				        max( point1.z(), point2.z() ) );

	TestAlignedBox3d aBox( minCorner, maxCorner );

/*	cout << "rhs.minCorner = " << rhs.minCorner() << endl;
	cout << "rhs.maxCorner = " << rhs.maxCorner() << endl;
	cout << "minCorner = " << minCorner << endl;
	cout << "maxCorner = " << maxCorner << endl;
*/
//	POST( rhs.intersects( aBox ) );

	return aBox;
}

TestAlignedBox3d BoundaryTest::createAlignedBoxNotIntersectsAlignedBox( const TestAlignedBox3d& rhs ) const
{
	TestAlignedBox3d aBox = createAlignedBoxIntersectsAlignedBox( rhs );

    TestTransform3d identityTransform;
	PRE( rhs.intersects( aBox, identityTransform, identityTransform ) );

	// calculate the diagnol sizes of the two boxes
    const MexAlignedBox3d::Data rhsWorld = rhs.world( identityTransform );
	Point3 dimensions1 = rhsWorld.maxCorner_;

	dimensions1 -= rhsWorld.minCorner_;
	TestVec3 d1(dimensions1);
	TestType size1 = d1.modulus();

    const MexAlignedBox3d::Data& aBoxWorld = aBox.world( identityTransform );
	Point3 dimensions2 = aBoxWorld.maxCorner_;
	TestVec3 d2(dimensions2);
	TestType size2 = d2.modulus();

	// calculate the total size add on a random scalar ( plus a bit extra for safe measures )
	TestType size = size1 + size2 + randomFloat( 20000.0 ) + ( 10 * MexEpsilon::instance() );

	TestVec3 v = createRandomUnitVector();
	v *= size;

	// now move the unaligned by this amount
	dimensions1 += v;
	dimensions2 += v;

	TestAlignedBox3d result( dimensions1, dimensions2 );

//	POST( result.intersects( rhs ) == false );

	return result;
}

TestUnalignedBox3d BoundaryTest::createUnalignedBoxIntersectsAlignedBox( const TestAlignedBox3d& rhs ) const
{
	Point3 v = createPointInsideAlignedBox3d( rhs );

/*	cout << "in function createUnalignedBoxIntersectsAligned" << endl;
	cout << "rhs: " << rhs << endl;
	cout << "point inside rhs = " << v << endl;
*/
	TestUnalignedBox3d uBox = createUnalignedBoxIncludingPoint( v );

//	POST( rhs.contains( v ) );
//	POST( uBox.contains( v ) );

//	POST( rhs.intersects( uBox ) );

	return uBox;
}

TestUnalignedBox3d BoundaryTest::createUnalignedBoxNotIntersectsAlignedBox( const TestAlignedBox3d& rhs ) const
{
	// create an unaligned box which intersects with the aligned
	TestUnalignedBox3d uBox = createUnalignedBoxIntersectsAlignedBox( rhs );
    TestTransform3d identityTransform;

	PRE( rhs.intersects( uBox, identityTransform, identityTransform ) );

	// calculate the diagonal sizes of the two boxes
    const MexAlignedBox3d::Data& rhsWorld = rhs.world( identityTransform );
	Point3 dimensionsAligned = rhsWorld.maxCorner_;
	TestVec3 da(dimensionsAligned);
	da -= rhsWorld.minCorner_;
	TestType sizeAligned = da.modulus();

	Point3 dimensionsUnaligned = uBox.dimensions();
	TestVec3 du(dimensionsUnaligned);
	TestType sizeUnaligned = du.modulus();

	// calculate the total size add on a random scalar ( plus a bit extra for safe measures )
	TestType size = sizeUnaligned + sizeAligned + randomFloat( 20000.0 ) + ( 10 * MexEpsilon::instance() );

	TestVec3 v = createRandomUnitVector();
	v *= size;

	Point3 v1 = v;
	// now move the unaligned by this amount
	TestTransform3d t = uBox.localTransform();
	t.translate( v1 );
	uBox.localTransform( t );

/*	cout << "dimensionsAligned 		= " << dimensionsAligned << endl;
	cout << "sizeAligned 			= " << sizeAligned << endl;
	cout << "dimensionsUnaligned 	= " << dimensionsUnaligned << endl;
	cout << "sizeUnaligned 			= " << sizeUnaligned << endl;
	cout << "scalar size value 		= " << size << endl;
	cout << "vector size value 		= " << v << endl;
	cout << "final unalignedBox 	= " << uBox << endl;
*/
	return uBox;
}

TestLine3d BoundaryTest::createLine3dIntersectsAlignedBox( const TestAlignedBox3d& rhs ) const
{
	TestVec3 v = createPointInsideAlignedBox3d( rhs );

	TestType scalar = randomFloat( MexEpsilon::instance(), 100 );	 // don't want number too large
	TestVec3 p1 = createRandomPoint3();
	TestVec3 p2( -p1.x(), -p1.y(), -p1.z() );

	p1 *= scalar;

	scalar = randomFloat( MexEpsilon::instance(), 100 );

	p2 *= scalar;

	p1 += v;
	p2 += v;

	Point3 point1(p1);
	Point3 point2(p2);
	TestLine3d result( p2, p1 );

	// POST(result.intersects( rhs ) );

	return result;
}

TestLine3d BoundaryTest::createLine3dNotIntersectsAlignedBox( const TestAlignedBox3d& rhs ) const
{
	// cunningly create an aligned box which doesn't intersect with rhs
	// and make a line from it's two corners
	TestAlignedBox3d aBox = createAlignedBoxNotIntersectsAlignedBox( rhs );

    TestTransform3d identityTransform;
    const MexAlignedBox3d::Data& aBoxWorld = aBox.world( identityTransform );
	TestLine3d result( aBoxWorld.minCorner_, aBoxWorld.maxCorner_ );

	// POST(result.intersects( rhs ) == false );

	return result;
}
/*
TestPlane3d BoundaryTest::createPlane3dIntersectsAlignedBox( const TestAlignedBox3d& rhs ) const
{
}

TestPlane3d BoundaryTest::createPlane3dNotIntersectsAlignedBox( const TestAlignedBox3d& rhs ) const
{
}

*/

TestUnalignedBox3d BoundaryTest::createUnalignedBoxIntersectsUnalignedBox( const TestUnalignedBox3d& rhs ) const
{
//	cout << "rhs: " << rhs << endl;

//	cout << "in function createUnalignedBoxIntersectsUnalignedBox" << endl;

	Point3 v = createPointInsideUnalignedBox3d( rhs );

//	cout << "point in unaligned: " << v << endl;
    TestTransform3d identityTransform;
	POST(rhs.contains( v, identityTransform ) );

//	cout << "calling createUnalignedBoxIncludingPoint" << endl;
	TestUnalignedBox3d uBox = createUnalignedBoxIncludingPoint( v );
/*
	TestTransform3d t = createRandomTransform3d( v );
	Point3 v2 = createRandomPoint3();

	TestUnalignedBox3d uBox( v2, t );

	cout << "rhs: " << rhs << endl;
	cout << "ubox:" << uBox << endl;
*/

	POST( uBox.intersects( v, identityTransform ) );

	return uBox;
}

TestUnalignedBox3d BoundaryTest::createUnalignedBoxNotIntersectsUnalignedBox( const TestUnalignedBox3d& rhs ) const
{
	TestUnalignedBox3d uBox = createUnalignedBoxIntersectsUnalignedBox( rhs );

	Point3 dimensions1 = rhs.dimensions();
	TestVec3 d1(dimensions1);
	TestType size1 = d1.modulus();

	Point3 dimensions2 = uBox.dimensions();
	TestVec3 d2(dimensions2);
	TestType size2 = d2.modulus();

	// calculate the total size add on a random scalar ( plus a bit extra for safe measures )
	TestType size = size1 + size2 + randomFloat( 20000.0 ) + ( 10 * MexEpsilon::instance() );

	TestVec3 v1 = createRandomUnitVector();
	v1 *= size;

	Point3 v(v1);

	// now move the unaligned by this amount
	TestTransform3d t = uBox.localTransform();
	t.translate( v );
	uBox.localTransform( t );

	return uBox;
}

TestLine3d BoundaryTest::createLine3dIntersectsUnalignedBox( const TestUnalignedBox3d& rhs ) const
{
	// make axis aligned representation of the box
	Point3 zero;	// zero vector
	TestAlignedBox3d aBox( zero, rhs.dimensions() );
    TestTransform3d identityTransform;
	TestTransform3d t( rhs.overallTransform( identityTransform ) );

	TestLine3d line = createLine3dIntersectsAlignedBox( aBox );
	Point3 minCorner = line.end1();
	Point3 maxCorner = line.end2();

	t.transform( &minCorner );
	t.transform( &maxCorner );

	TestLine3d result( minCorner, maxCorner );

	// POST(result.intersects( rhs ) );

	return result;
}

TestLine3d BoundaryTest::createLine3dNotIntersectsUnalignedBox( const TestUnalignedBox3d& rhs ) const
{
	Point3 v;	// a zero vector
	TestAlignedBox3d aBox( v, rhs.dimensions() );
	TestTransform3d identityTransform;
	TestTransform3d t( rhs.overallTransform( identityTransform ) );		// store the transform of rhs

	TestAlignedBox3d aBox2 = createAlignedBoxNotIntersectsAlignedBox( aBox );
    const MexAlignedBox3d::Data& aBox2World = aBox2.world( identityTransform );
	TestLine3d line( aBox2World.minCorner_, aBox2World.maxCorner_ );

	// we now need to transform the line
	Point3 end1 = line.end1();
	Point3 end2 = line.end2();

	t.transform( &end1 );
	t.transform( &end2 );

	TestLine3d result( end1, end2 );

	POST(rhs.intersects( result, identityTransform ) == false );

	return result;
}

TestLine3d BoundaryTest::createLine3dIntersectsLine3d( const TestLine3d& rhs ) const
{
	Point3 v = createPointInsideLine3d( rhs );

	PRE( rhs.contains( v ) );

	TestType scalar = randomFloat( MexEpsilon::instance(), 2000.0 );	 // don't want number too large
	Point3 p1 = createRandomPoint3();
	Point3 p2( -p1.x(), -p1.y(), -p1.z() );

//	cout << "p1 = " << p1 << endl << "p2 = " << p2 << endl << "v = " << v << endl;
//	cout << "scalar1 = " << scalar << endl;

	//p1 *= scalar;

	scalar = randomFloat( MexEpsilon::instance(), 20000.0 );

//	cout << "scalar2 = " << scalar << endl;

	//p2 *= scalar;

	p1 += v;
	p2 += v;

	TestLine3d result( p2, p1 );

//	cout << "result = " << result << endl;
	//POST(result.intersects( rhs ) );

	return result;
}

TestLine3d BoundaryTest::createLine3dNotIntersectsLine3d( const TestLine3d& rhs ) const
{
	// create an aligned box with the line
	TestAlignedBox3d aBox( rhs.end1(), rhs.end2() );

	// create a box which doesn't intersect
	TestAlignedBox3d aBox2 = createAlignedBoxNotIntersectsAlignedBox( aBox );
    TestTransform3d identityTransform;
    const MexAlignedBox3d::Data& aBox2World = aBox2.world( identityTransform );

	Point3 point1 = aBox2World.minCorner_;
	Point3 point2 = aBox2World.maxCorner_;

	// obtain a line from the two opposite vertices
	Point3 minCorner( min( point1.x(), point2.x() ),
		  	  		    min( point1.y(), point2.y() ),
				        min( point1.z(), point2.z() ) );

	Point3 maxCorner( max( point1.x(), point2.x() ),
				        max( point1.y(), point2.y() ),
				        max( point1.z(), point2.z() ) );

	TestLine3d result( minCorner, maxCorner );

	return result;
}

TestLine3d BoundaryTest::createLine3dIntersectsPlane3d( const TestPlane3d& rhs ) const
{
	Point3 p1( createPointOnPositiveSidePlane3d( rhs ) );
	Point3 p2( createPointOnNegativeSidePlane3d( rhs ) );

	TestLine3d result( p2, p1 );

	return result;
}

TestLine3d BoundaryTest::createLine3dNotIntersectsPlane3d( const TestPlane3d& rhs ) const
{
	Point3 p1, p2;

	if( ( randomInt() & 1 ) == 0 )
	{
		p1 = createPointOnPositiveSidePlane3d( rhs );
		p2 = createPointOnPositiveSidePlane3d( rhs );
	}
	else
	{
		p1 = createPointOnNegativeSidePlane3d( rhs );
		p2 = createPointOnNegativeSidePlane3d( rhs );
	}

	TestLine3d result( p1, p2 );

	return result;
}

TestAlignedBox3d BoundaryTest::createAlignedBoxIntersectsPlane3d( const TestPlane3d& rhs ) const
{
/*	TestType x = randomFloat();
	TestType y = randomFloat();
	TestType positiveScalar = randomFloat();
	TestType negativeScalar = ( -1.0 ) * randomFloat();
	TestType z = ( positiveScalar - x * rhs.a() - y * rhs.b() - rhs.d() ) / rhs.c();

	Point3 point1( x, y, z );

	x = randomFloat();
	y = randomFloat();
	z = ( negativeScalar - x * rhs.a() - y * rhs.b() - rhs.d() ) / rhs.c();

	Point3 point2( x, y, z );
*/
	Point3 point1 = createPointOnNegativeSidePlane3d( rhs );
	Point3 point2 = createPointOnPositiveSidePlane3d( rhs );

	Point3 minCorner( min( point1.x(), point2.x() ),
		  	  		    min( point1.y(), point2.y() ),
				        min( point1.z(), point2.z() ) );

	Point3 maxCorner( max( point1.x(), point2.x() ),
				        max( point1.y(), point2.y() ),
				        max( point1.z(), point2.z() ) );

	TestAlignedBox3d aBox( minCorner, maxCorner );

	return aBox;
}


TestAlignedBox3d BoundaryTest::createAlignedBoxNotIntersectsPlane3d( const TestPlane3d& rhs ) const
{
/*	Point3 point1 = createPointOnNegativeSidePlane3d( rhs );
	Point3 point2 = createPointOnNegativeSidePlane3d( rhs );
	Point3 minCorner( min( point1.x(), point2.x() ),
				  		min( point1.y(), point2.y() ),
				  		min( point1.z(), point2.z() ) );

	Point3 maxCorner( max( point1.x(), point2.x() ),
				  		max( point1.y(), point2.y() ),
				  		max( point1.z(), point2.z() ) );

	Point3 dimensions = maxCorner;
	dimensions -= minCorner;

	Point3 safePoint = rhs.normal();
	TestType size = dimensions.modulus() + randomFloat( 20000.0 ) + 10 * MexEpsilon::instance();
	safePoint *= size;

	minCorner += safePoint;
	maxCorner += safePoint;

	TestAlignedBox3d result( minCorner, maxCorner );

	cout << "dimensions = " << dimensions << endl;
	cout << "size = " << size << endl;
	cout << "safe point = " << safePoint << endl;
	cout << "result = " << result << endl;
*/
	Point3 point1 = createPointOnPositiveSidePlane3d( rhs );
    Point3 point2 = createPointOnPositiveSidePlane3d( rhs );
	Point3 offset = rhs.normal();

 //	Point3 *vPtr = &point2;
	TestVec3 u(point1);
	TestVec3 v(point2);
	TestType distance = u.euclidianDistance( v );
	point2 = v;
	point1 = u;
    TestType scalar = randomFloat( 20000.0 );

	TestVec3 v2(offset);
	v2 *= ( distance + scalar );
	offset = v2;

	point1 += offset;
	point2 += offset;

	TestAlignedBox3d result( point1, point2 );

	return result;
}

TestUnalignedBox3d BoundaryTest::createUnalignedBoxIntersectsPlane3d( const TestPlane3d& rhs ) const
{
	Point3 point = createPointInsidePlane3d( rhs );
	TestUnalignedBox3d result = createUnalignedBoxIncludingPoint( point );

	return result;
}

TestUnalignedBox3d BoundaryTest::createUnalignedBoxNotIntersectsPlane3d( const TestPlane3d& rhs ) const
{
	TestUnalignedBox3d uBox = createUnalignedBoxIntersectsPlane3d( rhs );
	Point3 v( uBox.dimensions() );
	Point3 offset( rhs.normal() );

	TestVec3 vP(v);
	TestType size = vP.modulus() + ( 10 * MexEpsilon::instance() ) + randomFloat( MexEpsilon::instance(), 2000 );
	v = vP;

//	cout << "transformposition = " << uBox.transformPosition() << endl;
	TestVec3 off(offset);
	off *= size;
	offset = off;

	Point3 p( uBox.localTransform().position() );
	p += offset;

	TestTransform3d t( uBox.localTransform() );
	t.position( p );
	uBox.localTransform( t );

/*	cout << "v = " << v << endl;
	cout << "distance = " << distance << endl;
	cout << "offset = " << offset << endl;
	cout << "size = " << size << endl;
	cout << "uBox = " << uBox << endl;
*/
	return uBox;
}

TestUnalignedBox3d BoundaryTest::createUnalignedBoxIncludingPoint( const Point3& rhs ) const
{
//	cout << "in function createUnalignedBoxIncludingPoint"  << endl;
//	cout << "point to be used = " << rhs << endl;

	// treat the point as the world origin and create an aligned box around
	// this origin
//	cout << "creating unaligned box around point: " << rhs << endl;

//

//	cout << "in function createUnalignedBoxIncludingpoint" << endl;

	Point3 temp = createRandomPoint3();
	Point3 maxCorner = createRandomPoint3();
	Point3 minCorner( -temp.x(), -temp.y(), -temp.z() );
//	minCorner += rhs;
//	maxCorner += rhs;

//	cout << "alignedbox = " << minCorner << "   " << maxCorner << endl;

	TestAlignedBox3d aBox( minCorner, maxCorner );
	TestUnalignedBox3d uBox( aBox ); 	// create unaligned box from aligned

	// random Euler angles
	TestType a = randomFloat( MexEpsilon::instance(), 20000.0 );
	TestType e = randomFloat( MexEpsilon::instance(), 20000.0 );
	TestType r = randomFloat( MexEpsilon::instance(), 20000.0 );
	TestEulerAngles eulerAngles( a, e, r );

	// create a random rotation
	TestTransform3d rotation( eulerAngles );
	TestTransform3d t = uBox.localTransform(); // store the unaligned transformation

//	cout << "uBox dimensions = " << uBox.dimensions() << endl;
//	cout << "uBox position = " << uBox.transformPosition() << endl;

	// apply rotation to the unaligned box's transform and add on the new
	// point
	rotation.transform( t );
	Point3 pos = rotation.position();
	pos += rhs;
	rotation.position( pos );     // reset the position of the rotation transform
	uBox.localTransform( rotation );   // transform the box

//	cout << "unaligned box = " << uBox << endl;

//  	POST( uBox.intersects( rhs ) );

	return uBox;
}

void BoundaryTest::synsug(const bool& c, ostream& out) const
{
    if (c==true)
        out << "Objects should have intersected but did not" << endl;
    else
		out << "Objects should not have intersected but did" << endl;

    out << "Seed = " << timeSeed_ << endl;
}

bool BoundaryTest::testMexBoundary3dPtr( ostream& out )
{
    bool okay = true;

    //Construct two intersecting aligned box boundaries on the heap
    MexAlignedBox3d *pBox1 = _NEW( MexAlignedBox3d( 100, 100, 100, 200, 200, 200 ) );
    MexAlignedBox3d *pBox2 = _NEW( MexAlignedBox3d( 150, 150, 150, 300, 300, 300 ) );

    //Create reference counted pointers to them.
    //The boxes will be deleted when these pointers go out of scope
    MexBoundary3dPtr aBoxPtr1( pBox1 );
    MexBoundary3dPtr aBoxPtr2( pBox2 );

    //Check we can dereference ok
    MexTransform3d identityTransform;
    if (not aBoxPtr1->intersects( *aBoxPtr2, identityTransform, identityTransform ))
    {
        out << "Aligned box intersection via ref-counted pointer failed" << endl;
        okay = false;
    }

    //Now increase the reference count for the first by 2, and check flagged
    //as shared
    if (okay)
    {
        MexBoundary3dPtr aBoxCopy1Ptr( aBoxPtr1 );
        MexBoundary3dPtr aBoxCopy2Ptr( aBoxPtr2 );
        aBoxCopy2Ptr = aBoxCopy1Ptr;

        if (not aBoxPtr1.isShared())
        {
            out << "Aligned box ref-count incorrect" << endl;
            okay = false;
        }

        if (aBoxPtr2.isShared())
        {
            out << "Aligned box ref-count incorrect" << endl;
            okay = false;
        }
    }

    //Now the ref-count should be back down to 1
    if (okay and aBoxPtr1.isShared())
    {
        out << "Aligned box ref-count incorrect" << endl;
        okay = false;
    }

    out << "MexBoundary3d ref-counted pointer test "
        << (okay ? "passed" : "failed!!!") << endl;

    return okay;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool BoundaryTest::testEulerParameters( ostream& out )
{
    bool okay = true;
    MATHEX_SCALAR piOver180 = 0.01745329252;

    MATHEX_SCALAR testValues[5][4] =
    {
        1.0, 0.0, 0.0, 0.0,
        1.0, 0.0, 0.0, 30.0,
        0.0, 1.0, 0.0, 99.99,
        0.0, 0.0, 1.0, 0.001,
        0.577350269, 0.577350269, 0.577350269, 90.0
    };

    //Do test for each set of values
    for( size_t i = 0; i < 5; ++i )
    {
        //Construct euler parameter set using the values
        MexQuaternion params;
        MexVec3 directionVector( testValues[i][0], testValues[i][1], testValues[i][2] );
        MATHEX_SCALAR angle = testValues[i][3] * piOver180;

        params.set( directionVector, angle );

        //Use the parameters to set the rotation part of a transform
        MexTransform3d transform;
        transform.rotation( params );

        //Get the transform rotation back in as euler parameters
        MexQuaternion backParams;
        transform.rotation( &backParams );

        //Convert back to direction vector and angle
        MATHEX_SCALAR backAngle;
        MexVec3 backDirectionVector;
        backAngle = backParams.rotation( &backDirectionVector );

        //Check they match
        if( not (backDirectionVector == directionVector) or
            abs( backAngle - angle ) > 0.000001 )
        {
            out << "Euler parameter values incorrect" << endl;
            okay = false;
        }
    }

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool BoundaryTest::testPolygonIntersect( ostream& out )
{
    bool okay = true;

    //Create a 2d polygon
    MexConvexPolygon2d::Points* pPolygonPoints = _NEW( MexConvexPolygon2d::Points );
    pPolygonPoints->reserve( 5 );
    pPolygonPoints->push_back( MexPoint2d( 0, 1700 ) );
    pPolygonPoints->push_back( MexPoint2d( -300, 800 ) );
    pPolygonPoints->push_back( MexPoint2d( 0, 500 ) );
    pPolygonPoints->push_back( MexPoint2d( 200, 800 ) );
    pPolygonPoints->push_back( MexPoint2d( 200, 1200 ) );
    MexConvexPolygon2d pgon( pPolygonPoints );

    //Construct the test points
    MexPoint2d p1a( -300, 600 );
    MexPoint2d p1b( -200, 700 );
    MexPoint2d p2a( -200, 600 );
    MexPoint2d p2b( 0, 600 );
    MexPoint2d p3a( 100, 600 );
    MexPoint2d p3b( 300, 600 );
    MexPoint2d p4a( 100, 800 );
    MexPoint2d p4b( 0, 900 );
    MexPoint2d p5a( 400, 700 );
    MexPoint2d p5b( 200, 800 );
    MexPoint2d p6a( 200, 1000 );
    MexPoint2d p6b( 200, 900 );
    MexPoint2d p7a( 300, 1100 );
    MexPoint2d p7b( -400, 1100 );
    MexPoint2d p8a( 200, 1500 );
    MexPoint2d p8b( 200, 1700 );

    //Check the intersections
    if( okay  and  not pgon.intersects( p1a, p1b)) okay = false;
    if( okay  and  not pgon.intersects( p2a, p2b)) okay = false;
    if( okay  and      pgon.intersects( p3a, p3b)) okay = false;
    if( okay  and  not pgon.intersects( p4a, p4b)) okay = false;
    if( okay  and  not pgon.intersects( p5a, p5b)) okay = false;
    if( okay  and  not pgon.intersects( p6a, p6b)) okay = false;
    if( okay  and  not pgon.intersects( p7a, p7b)) okay = false;
    if( okay  and      pgon.intersects( p8a, p8b)) okay = false;

    //Construct an expanded polygon
    MexConvexPolygon2d biggerPolygon( pgon, 100 );
    if( biggerPolygon.nVertices() != 7 ) okay = false;

    //Check the intersections
    if( okay  and  not biggerPolygon.intersects( p1a, p1b)) okay = false;
    if( okay  and  not biggerPolygon.intersects( p2a, p2b)) okay = false;
    if( okay  and  not biggerPolygon.intersects( p3a, p3b)) okay = false;
    if( okay  and  not biggerPolygon.intersects( p4a, p4b)) okay = false;
    if( okay  and  not biggerPolygon.intersects( p5a, p5b)) okay = false;
    if( okay  and  not biggerPolygon.intersects( p6a, p6b)) okay = false;
    if( okay  and  not biggerPolygon.intersects( p7a, p7b)) okay = false;
    if( okay  and      biggerPolygon.intersects( p8a, p8b)) okay = false;

    //Report
    if( not okay ) out << "Polygon intersect test failed" << endl;

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool BoundaryTest::testSausageIntersect( ostream& )
{
    bool okay = true;

    //Create a 2d triangle polygon
    MexConvexPolygon2d::Points* pPolygonPoints = _NEW( MexConvexPolygon2d::Points );
    pPolygonPoints->reserve( 3 );
    pPolygonPoints->push_back( MexPoint2d( 100,100 ) );
    pPolygonPoints->push_back( MexPoint2d( 200,100 ) );
    pPolygonPoints->push_back( MexPoint2d( 150,200 ) );
    MexConvexPolygon2d pgon( pPolygonPoints );

    //Create some sausages
    MexSausage2d aSausage( MexPoint2d( 150,120 ), MexPoint2d( 160, 120 ), 5 );
    MexSausage2d bSausage( MexPoint2d( 210,100 ), MexPoint2d( 230, 100 ), 10 );
    MexSausage2d cSausage( MexPoint2d( 220,120 ), MexPoint2d( 150, 150 ), 5 );
    MexSausage2d dSausage( MexPoint2d( 190,150 ), MexPoint2d( 210, 150 ), 15 );
    MexSausage2d eSausage( MexPoint2d( 190,150 ), MexPoint2d( 210, 150 ), 5 );

    //Check the intersections
    if( okay and not aSausage.intersects( pgon ) )
        okay = false;

    if( okay and not bSausage.intersects( pgon, Mathex::TOUCH_IS_INTERSECT ) )
        okay = false;

    if( okay and bSausage.intersects( pgon, Mathex::TOUCH_ISNT_INTERSECT ) )
        okay = false;

    if( okay and not cSausage.intersects( pgon ) )
        okay = false;

    if( okay and not dSausage.intersects( pgon ) )
        okay = false;

    if( okay and eSausage.intersects( pgon ) )
        okay = false;

    if( okay and aSausage.intersects( bSausage ) )
        okay = false;

    if( okay and aSausage.intersects( cSausage ) )
        okay = false;

    if( okay and aSausage.intersects( dSausage ) )
        okay = false;

    if( okay and bSausage.intersects( cSausage ) )
        okay = false;

    if( okay and bSausage.intersects( dSausage ) )
        okay = false;

    if( okay and not cSausage.intersects( dSausage ) )
        okay = false;

    if( okay and cSausage.intersects( eSausage ) )
        okay = false;

    if( okay and not dSausage.intersects( eSausage ) )
        okay = false;

    return okay;
}


bool BoundaryTest::testAlignedBoxIntersectsLine( ostream& )
{
    bool okay = true;

    MexAlignedBox3d box( MexPoint3d( 100, 200, 300 ), MexPoint3d( 400, 500, 600 ) );
    MATHEX_SCALAR entryDistance = -1.0, exitDistance = -1.0;

    MexLine3d lineA( MexPoint3d( 150, 250, 10 ), MexPoint3d( 150, 250, 620 ) );
    okay = okay and box.intersects( lineA, &entryDistance, &exitDistance, Mathex::TOUCH_ISNT_INTERSECT )
                and entryDistance == 290  and  exitDistance == 590;

    MexLine3d lineB( MexPoint3d( 10, 200, 420 ), MexPoint3d( 410, 200, 420 ) );
    okay = okay and not box.intersects( lineB, &entryDistance, &exitDistance, Mathex::TOUCH_ISNT_INTERSECT )
                and box.intersects( lineB, &entryDistance, &exitDistance, Mathex::TOUCH_IS_INTERSECT )
                and entryDistance == 90  and  exitDistance == 390;

    MexLine3d lineC( MexPoint3d( -50, 450, 520 ), MexPoint3d( 700, 200, 520 ) );
    okay = okay and box.intersects( lineC, &entryDistance, &exitDistance, Mathex::TOUCH_ISNT_INTERSECT )
                and fabs( entryDistance - 158.113883 ) < 0.00001
                and fabs( exitDistance - 3*158.113883 ) < 0.0001;

    return okay;
}

bool BoundaryTest::testLine3dIntersectsTriangle( ostream& )
{
    bool okay = true;

    //Set up the triangle points
    MexPoint3d p1( 200, 100, 50 );
    MexPoint3d p2( 300, 100, 75 );
    MexPoint3d p3( 500, 100, 50 );

    //And test against some lines
    MATHEX_SCALAR distance;

    MexLine3d l1( MexPoint3d( 305, 0, 70 ), MexPoint3d( 305, 150, 70 ) );
    okay = okay and l1.intersects( p1, p2, p3, &distance )
                and (distance == 100.0);
    okay = okay and l1.intersects( p3, p2, p1, &distance )
                and (distance == 100.0);

    MexLine3d l2( MexPoint3d( 301, 0, 75 ), MexPoint3d( 301, 150, 75 ) );
    okay = okay and not l2.intersects( p1, p2, p3, &distance );

    MexLine3d l3( MexPoint3d( 0, 0, 50 ), MexPoint3d( 400, 200, 50 ) );
    okay = okay and l3.intersects( p1, p2, p3, &distance )
                and (fabs( distance - 223.6067978) < 0.00001);

    MexLine3d l4( MexPoint3d( 200, 50, 75 ), MexPoint3d( 500, 50, 60 ) );
    okay = okay and not l4.intersects( p1, p2, p3, &distance );

    MexLine3d l5( MexPoint3d( -6.95585, 87.5, 1.36626 ), MexPoint3d( -35.6522, -87.5, 0.398602 ) );
    okay = okay and l5.intersects( MexPoint3d( -4e-06, 6e-06, 2.5 ),
                                   MexPoint3d( -29.1667, 8e-06, -2e-06 ),
                                   MexPoint3d( -5e-06, -29.1667, 7.49999 ), &distance );
    return okay;
}


bool BoundaryTest::testPersistence( ostream& )
{
    PER_REGISTER_DERIVED_CLASS( MexDegrees );
    PER_REGISTER_DERIVED_CLASS( MexRadians );
    PER_REGISTER_DERIVED_CLASS( MexTriangle2d );
    PER_REGISTER_DERIVED_CLASS( MexConvexPolygon2d );

    bool okay = true;

    //Construct the originals
    MexAlignedBox2d aAlignedBox2d( MexPoint2d( 110,210 ), MexPoint2d( 310,410 ) );
    MexAlignedBox3d aAlignedBox3d( MexPoint3d( 120,220,320 ), MexPoint3d( 420,520,620 ) );
    				aAlignedBox3d.realign( true );

	MexDegrees degree(89.9);
	MexAngle* paAngle = &degree;   //~Angle  is protected!   tested through degrees and radians

	//MexBoundary aBoundary;

    MexCircle2d aCircle2d( MexPoint2d( 100, 200 ), 300 );
	//MexConvexPolygon2d aConvexPolygon2d( aAlignedBox2d );
	//MexPolygon2d* paPolygon2d = &aConvexPolygon2d;

	MexDegrees aDegree(2.3456);
	//MexDouble aDouble;

	//MexEpsilon aEpsilon;
    MexEulerAngles aEulerAngles( 2.345, 3.456, 4.567 );

	MexHalfPlane2d aHalfPlane2d( MexVec2(0, 1), MexVec2( 5.678, 6.789 ));
	//MexHalfSpace3d aHalfSpace3d( MexPoint3d(0, 1, 5), MexPoint3d(5, 9, 10), aPoint3d);   %Abstract class?

    MexLine2d aLine2d( MexPoint2d( 150, 250 ), MexPoint2d( 350, 450 ) );
    MexLine3d aLine3d( MexPoint3d( 160, 260, 360 ), MexPoint3d( 460, 560, 660 ) );

	MexPlane3d aPlane3d( MexPoint3d(0, 0, 0), MexPoint3d(4, 5, 0), MexPoint3d(10, 20, 0) );
    MexPoint2d aPoint2d( 130,230 );
    MexPoint3d aPoint3d( 140,240,340 );
	//MexPolygon2d aPolygon2d;

	MexQuad3d aQuad3d( MexPoint3d(1, 1, 0), MexPoint3d(4, 5, 0), MexPoint3d(10, 20, 0), MexPoint3d(3, 10, 0));
	//MexQuadratic aQuadratic;
    MexQuaternion aQuaternion( MexVec3( 0.1, 0.2, 0.3 ), MexRadians( 33.3 ) );

    MexRadians aRadians( 1.234 );
	MATHEX_SCALAR ANG =aRadians.asScalar();
	MexAngle* paRadians = &aRadians;

    MexSausage2d aSausage2d( MexPoint2d( 170, 270 ), MexPoint2d( 370,470 ), 570 );

    MexTransform3d aTransform3d( MexEulerAngles( 1.2, 2.3, 3.4 ), MexPoint3d( 180, 280, 380 ) );
	MexTriangle2d aTriangle2d(MexPoint2d(1, 1), MexPoint2d(4, 5), MexPoint2d(10, 20) );
	MexPolygon2d* paMexPolygon2d = &aTriangle2d;

	MexUnalignedBox3d aUnalignedBox3d( MexPoint3d( 120,220,320 ), aTransform3d );

    MexVec2 aVec2( 5.678, 6.789 );
    MexVec3 aVec3( 7.890, 8.901, 9.012 );

	MexXCoord<double> aXCoord(1.5);
	MexYCoord<double> aYCoord(3.5);

	MATHEX_SCALAR x = aXCoord.xCoord();
	MATHEX_SCALAR y = aYCoord.yCoord();

	//Open a file, stream them out and close it
    {
        ofstream str( "streamtst.tmp", ios::binary );
        PerOstream testFile( str );

        testFile << aAlignedBox2d;
        testFile << aAlignedBox3d;
		testFile << *paAngle;

		//testFile << aBoundary;

        testFile << aCircle2d;
		//testFile << aConvexPolygon2d;

		testFile << aDegree;
		//testFile << aDouble;

		//testFile << aEpsilon;
        testFile << aEulerAngles;

		testFile << aHalfPlane2d;
		//testFile << aHalfSpace3d;

        testFile << aLine2d;
        testFile << aLine3d;

		testFile << aPlane3d;
        testFile << aPoint2d;
        testFile << aPoint3d;
		//testFile << aPolygon2d;

		testFile << aQuad3d;
        //testFile << aQuadratic;
        testFile << aQuaternion;

        testFile << aRadians;
        testFile << *paRadians;

        testFile << aSausage2d;

        testFile << aTransform3d;
		testFile << aTriangle2d;
		testFile << *paMexPolygon2d;

		testFile << aUnalignedBox3d;

        testFile << aVec2;
        testFile << aVec3;

		testFile << aXCoord;
		testFile << aYCoord;
	}

    //Restore the originals
    ifstream str( "streamtst.tmp", ios::binary );
    PerIstream testFile( str );

    MexAlignedBox2d bAlignedBox2d( testFile );
    MexAlignedBox3d bAlignedBox3d( testFile );

	MexAngle* pbAngle;
	 		  testFile >> pbAngle;

	//MexBoundary bBoundary;

    MexCircle2d bCircle2d( testFile );
	//MexConvexPolygon2d bConvexPolygon2d( testFile );

	MexDegrees bDegree( testFile );
	//MexDouble bDouble;

	//MexEpsilon bEpsilon;
    MexEulerAngles bEulerAngles( testFile );

	MexHalfPlane2d bHalfPlane2d (testFile );
	//MexHalfSpace3d bHalfSpace3d (testFile );

    MexLine2d bLine2d( testFile );
    MexLine3d bLine3d( testFile );

	MexPlane3d bPlane3d (testFile );
    MexPoint2d bPoint2d( testFile );
    MexPoint3d bPoint3d( testFile );
	//MexPolygon2d bPolygon2d( testFile );

	MexQuad3d bQuad3d (testFile );
	//MexQuadratic bQuadratic(testFile );
    MexQuaternion bQuaternion( testFile );

    MexRadians bRadians( testFile );
	MexAngle* pbRadians;
	 		  testFile >> pbRadians;

	//MATHEX_SCALAR radians = pbRadians->inRadians();
	//cout << "radians out : " << radians << endl;

    MexSausage2d bSausage2d( testFile );

    MexTransform3d bTransform3d( testFile );
	MexTriangle2d bTriangle2d(testFile );
	MexPolygon2d* pbMexPolygon2d;

	testFile >> pbMexPolygon2d;

	MexUnalignedBox3d bUnalignedBox3d(testFile );

    MexVec2 bVec2( testFile );
    MexVec3 bVec3( testFile );

	MexXCoord< double > bXCoord (testFile );
	MexYCoord< double > bYCoord (testFile );

	MATHEX_SCALAR bx = bXCoord.xCoord();
	MATHEX_SCALAR by = bYCoord.yCoord();

    //Compare values

    okay = okay and
           aAlignedBox2d.minCorner().x() == bAlignedBox2d.minCorner().x()  and
           aAlignedBox2d.minCorner().y() == bAlignedBox2d.minCorner().y()  and
           aAlignedBox2d.maxCorner().x() == bAlignedBox2d.maxCorner().x()  and
           aAlignedBox2d.maxCorner().y() == bAlignedBox2d.maxCorner().y();

    okay = okay and
           aAlignedBox3d.local().minCorner_.x() == bAlignedBox3d.local().minCorner_.x()  and
           aAlignedBox3d.local().minCorner_.y() == bAlignedBox3d.local().minCorner_.y()  and
           aAlignedBox3d.local().minCorner_.z() == bAlignedBox3d.local().minCorner_.z()  and
           aAlignedBox3d.local().maxCorner_.x() == bAlignedBox3d.local().maxCorner_.x()  and
           aAlignedBox3d.local().maxCorner_.y() == bAlignedBox3d.local().maxCorner_.y()  and
           aAlignedBox3d.local().maxCorner_.z() == bAlignedBox3d.local().maxCorner_.z()  and
           aAlignedBox3d.realign() == bAlignedBox3d.realign();


	okay = okay and
		   *paAngle == *pbAngle;

    okay = aCircle2d.radius() == bCircle2d.radius() and
           aCircle2d.centre().x() == bCircle2d.centre().x() and
           aCircle2d.centre().y() == bCircle2d.centre().y();

   //	okay = okay and
    	   //aConvexPolygon2d == bConvexPolygon2d;

	okay = okay and
		   aDegree.asScalar() == bDegree.asScalar();

    okay = okay and
           aEulerAngles.azimuth() == bEulerAngles.azimuth()  and
           aEulerAngles.elevation() == bEulerAngles.elevation()  and
           aEulerAngles.roll() == bEulerAngles.roll();

	okay = okay and
		   aHalfPlane2d == bHalfPlane2d;

	//okay = okay and
		   //aHalfSpace3d == bHalfSpace3d;

    okay = okay and
           aLine2d.end1().x() == bLine2d.end1().x()  and
           aLine2d.end1().y() == bLine2d.end1().y()  and
           aLine2d.end2().x() == bLine2d.end2().x()  and
           aLine2d.end2().y() == bLine2d.end2().y();

    okay = okay and
           aLine3d.end1().x() == bLine3d.end1().x()  and
           aLine3d.end1().y() == bLine3d.end1().y()  and
           aLine3d.end1().z() == bLine3d.end1().z()  and
           aLine3d.end2().x() == bLine3d.end2().x()  and
           aLine3d.end2().y() == bLine3d.end2().y()  and
           aLine3d.end2().z() == bLine3d.end2().z();

	okay = okay and
		   aPlane3d == bPlane3d;

    okay = okay and
           aPoint2d.x() == bPoint2d.x()  and
           aPoint2d.y() == bPoint2d.y();

    okay = okay and
           aPoint3d.x() == bPoint3d.x()  and
           aPoint3d.y() == bPoint3d.y()  and
           aPoint3d.z() == bPoint3d.z();

	//okay = okay and
		   //aPolygon2d == bPolygon2d;

	MATHEX_SCALAR aAng = aRadians.asScalar();
	MATHEX_SCALAR bAng = bRadians.asScalar();

    okay = okay and
           aRadians.asScalar() == bRadians.asScalar();

	okay = okay and
           *paRadians == *pbRadians;

	okay = okay and
 		   aQuad3d == bQuad3d;

    okay = okay and
           aQuaternion.vector().x() == bQuaternion.vector().x()  and
           aQuaternion.vector().y() == bQuaternion.vector().y()  and
           aQuaternion.vector().z() == bQuaternion.vector().z()  and
           aQuaternion.scalar() == bQuaternion.scalar();

    okay = okay and
           aSausage2d.startPoint().x() == bSausage2d.startPoint().x()  and
           aSausage2d.startPoint().y() == bSausage2d.startPoint().y()  and
           aSausage2d.endPoint().x() == bSausage2d.endPoint().x()  and
           aSausage2d.endPoint().y() == bSausage2d.endPoint().y()  and
           aSausage2d.radius() == bSausage2d.radius()  and
           aSausage2d.length() == bSausage2d.length();

    okay = okay and
           aTransform3d.xBasis().x() == bTransform3d.xBasis().x()  and
           aTransform3d.xBasis().y() == bTransform3d.xBasis().y()  and
           aTransform3d.xBasis().z() == bTransform3d.xBasis().z()  and
           aTransform3d.yBasis().x() == bTransform3d.yBasis().x()  and
           aTransform3d.yBasis().y() == bTransform3d.yBasis().y()  and
           aTransform3d.yBasis().z() == bTransform3d.yBasis().z()  and
           aTransform3d.zBasis().x() == bTransform3d.zBasis().x()  and
           aTransform3d.zBasis().y() == bTransform3d.zBasis().y()  and
           aTransform3d.zBasis().z() == bTransform3d.zBasis().z()  and
           aTransform3d.position().x() == bTransform3d.position().x()  and
           aTransform3d.position().y() == bTransform3d.position().y()  and
           aTransform3d.position().z() == bTransform3d.position().z();

	okay = okay and
		   aTriangle2d == bTriangle2d;

	okay = okay and
		   paMexPolygon2d->nVertices() == pbMexPolygon2d->nVertices();

 	okay = okay and
		   aUnalignedBox3d == bUnalignedBox3d;

    okay = okay and
           aVec2.x() == bVec2.x()  and
           aVec2.y() == bVec2.y();

    okay = okay and
           aVec3.x() == bVec3.x()  and
           aVec3.y() == bVec3.y()  and
           aVec3.z() == bVec3.z();

	okay = okay and
		   aXCoord == bXCoord;


   	okay = okay and
		   aYCoord == bYCoord;


    return okay;
}

bool BoundaryTest::testInverseTransform( ostream& )
{

	MexFloatMatrix3d m;
	int i,j;
	for(i =0; i<4; ++i)
		for(j=0; j<4; ++j)
			m[i][j] = 0;

	m[0][0] = 1;
	m[1][2] = -1;
	m[2][1] = 1;
	m[3][3] = 1;

	m[3][0] = 1;
	m[3][1] = 5;
	m[3][2] = 9;

	MexTransform3d rootXform;
   	rootXform.setTransform(m);

	MexTransform3d aXform = rootXform;
	MexTransform3d inVaXform;

	aXform.invert( &inVaXform );


	for(i =0; i<4; ++i)
		for(j=0; j<4; ++j)
			m[i][j] = 0;

	m[0][1] = -1;
	m[1][0] = 1;
	m[2][2] = 1;
	m[3][3] = 1;

	m[3][0] = 6;
	m[3][1] = 34;
	m[3][2] = 65;

	MexTransform3d t;
	t.setTransform(m);

	bool okay = true;

//this = inV(this) * t
	      inVaXform.transform(t);
	      aXform.transformInverse(t);

	okay = okay and
	      inVaXform == aXform;

//this = t * inV(this)

	aXform = rootXform;
	aXform.invert( &inVaXform );

	inVaXform.preTransform(t);
	aXform.preTransformInverse(t);

	okay = okay and
	      inVaXform == aXform;

//	result = inV(this) * t

	aXform = rootXform;
	aXform.invert( &inVaXform );

	MexTransform3d aResult, bResult;

    inVaXform.transform( t, &aResult );
    aXform.transformInverse( t, &bResult );

	okay = okay and
    	   aResult  == bResult;

//  vector = Inv(this) * vector

	MexVec3 aVector(1, 3, 5);
	MexVec3 bVector = aVector;
    aXform.transformInverse( &aVector );
	inVaXform.transform( &bVector );

	okay = okay and
    	   aVector  == bVector;

//p = this * p

	MexPoint3d aPoint3d(1, 3, 5);
	MexPoint3d bPoint3d = aPoint3d;
    aXform.transformInverse( &aPoint3d );
	inVaXform.transform( &bPoint3d );

	okay = okay and
    	   aPoint3d  == bPoint3d;

//pPoint = this * MexPoint3d( pPoint->x(), pPoint->y(), 0.0 );
	MexPoint2d aPoint2d(1, 3);
	MexPoint2d bPoint2d = aPoint2d;
    aXform.transformInverse( &aPoint2d );
	inVaXform.transform( &bPoint2d );

	okay = okay and
    	   aPoint2d  == bPoint2d;


//pLine = MexLine2d( this * pLine->end1(), this * pLine->end2() )
	MexLine2d aLine2d(MexPoint2d(1, 1), MexPoint2d(1, 3));
	MexLine2d bLine2d = aLine2d;
    aXform.transformInverse( &aLine2d );
	inVaXform.transform( &bLine2d );

	okay = okay and
    	   aLine2d.end1()  == bLine2d.end1() and
    	   aLine2d.end2()  == bLine2d.end2();

	return okay;
}

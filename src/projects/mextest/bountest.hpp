/*
 * B O U N T E S T . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#include <conio.h>
#include <fstream>
#include <ctime>

#include "mathex/transf3d.hpp"
// #include "mathex/transrep.hpp"

#include "mathex/eulerang.hpp"
#include "mathex/point3d.hpp"
// #include "mathex/mathex.hpp"
//#include "mathex/boundary.hpp"

//#include "mathex/hlfspc3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/abox3d.hpp"
//#include "mathex/ubox3d.hpp"
#include "mathex/plane3d.hpp"
#include "mextest/random.hpp"
//#include "mathex/intdata.hpp"

////////////////////////////////

#define ERROR_TEST cout << __LINE__ << endl << __FILE__ << "EPS: " << EPS << endl;

typedef double TestType;
typedef MexVec3 TestVec3;
typedef MexTransform3d	TestTransform3d;
typedef MexPoint3d Point3;
typedef MexAlignedBox3d TestAlignedBox3d;
typedef MexUnalignedBox3d TestUnalignedBox3d;
typedef MexLine3d TestLine3d;
typedef MexPlane3d TestPlane3d;
typedef MexEulerAngles TestEulerAngles;
//typedef MexHalfSpace3d TestHalfSpace3d;
// typedef IntersectionData IntersectTest;
////////////////////////////////

class BoundaryTest
{
public:

	BoundaryTest();
	~BoundaryTest();

	bool intersectAllOk( ostream& );

	// all the possible intersection combinations follow
	bool intersectPoint3dPoint3dOk( ostream& );
	bool intersectPoint3dAlignedBox3dOk( ostream& );
	bool intersectPoint3dUnalignedBox3dOk( ostream& );
	bool intersectPoint3dLine3dOk( ostream& );
	bool intersectPoint3dPlane3dOk( ostream& );
//	bool intersectPoint3dHalfSpace3dOk( ostream&  );

	bool intersectAlignedBox3dAlignedBox3dOk( ostream&  );
	bool intersectAlignedBox3dUnalignedBox3dOk( ostream&  );
	bool intersectAlignedBox3dLine3dOk( ostream&  );
	bool intersectAlignedBox3dPlane3dOk( ostream&  );
//	bool intersectAlignedBox3dHalfSpace3dOk( ostream&  );

	bool intersectUnalignedBox3dUnalignedBox3dOk( ostream&  );
	bool intersectUnalignedBox3dLine3dOk( ostream&  );
	bool intersectUnalignedBox3dPlane3dOk( ostream&  );
//	bool intersectUnalignedBox3dHalfSpace3dOk( ostream&  );

	bool intersectLine3dLine3dOk( ostream&  );
	bool intersectLine3dPlane3dOk( ostream&  );
//	bool intersectLine3dHalfSpace3dOk( ostream&  );

	bool intersectPlane3dPlane3dOk( ostream&  );
//	bool intersectPlane3dHalfSpace3dOk( ostream&  );

//	bool intersectHalfSpace3dHalfSpace3dOk( ostream&  );
    bool testMexBoundary3dPtr( ostream& );

    bool testEulerParameters( ostream& );
    bool testPolygonIntersect( ostream& out );
    bool testSausageIntersect( ostream& out );
    bool testAlignedBoxIntersectsLine( ostream& );
    bool testLine3dIntersectsTriangle( ostream& );
    bool testPersistence( ostream& );
	bool BoundaryTest::testInverseTransform( ostream& );

private:

	TestVec3 createRandomVec3() const;
	Point3 createRandomPoint3() const;
	Point3 createRandomUnitVector() const;
	TestAlignedBox3d createRandomAlignedBox3d() const;
	TestUnalignedBox3d createRandomUnalignedBox3d() const;
	TestLine3d createRandomLine3d() const;
	TestPlane3d createRandomPlane3d() const;
//	TestHalfSpace3d createRandomHalfSpace3d() const;
	TestTransform3d createRandomTransform3d() const;
	TestTransform3d createRandomTransform3d( const Point3& ) const;

	Point3 createPointInsideUnalignedBox3d( const TestUnalignedBox3d& ) const;
	Point3 createPointInsideAlignedBox3d( const TestAlignedBox3d& ) const;
	Point3 createPointOutsideAlignedBox3d( const TestAlignedBox3d& ) const;
	Point3 createPointOutsideUnalignedBox3d( const TestUnalignedBox3d& ) const;
	Point3 createPointInsideLine3d( const TestLine3d& ) const;
	Point3 createPointOutsideLine3d( const TestLine3d& ) const;
	Point3 createPointOnNegativeSidePlane3d( const TestPlane3d& ) const;
	Point3 createPointOnPositiveSidePlane3d( const TestPlane3d& ) const;
	Point3 createPointInsidePlane3d( const TestPlane3d& ) const;
	Point3 createPointOutsidePlane3d( const TestPlane3d& ) const;
//	Point3 createHalfSpacePoint( const TestHalfSpace3d &, const TestType & ) const;
//	Point3 createPointInsideHalfSpace3d( const TestHalfSpace3d & ) const;
//	Point3 createPointOutsideHalfSpace3d( const TestHalfSpace3d & ) const;

	TestAlignedBox3d createAlignedBoxIntersectsAlignedBox( const TestAlignedBox3d& ) const;
	TestAlignedBox3d createAlignedBoxNotIntersectsAlignedBox( const TestAlignedBox3d& ) const;
	TestUnalignedBox3d createUnalignedBoxIntersectsAlignedBox( const TestAlignedBox3d& ) const;
	TestUnalignedBox3d createUnalignedBoxNotIntersectsAlignedBox( const TestAlignedBox3d& ) const;
	TestLine3d createLine3dIntersectsAlignedBox( const TestAlignedBox3d& ) const;
	TestLine3d createLine3dNotIntersectsAlignedBox( const TestAlignedBox3d& ) const;
//	TestPlane3d createPlane3dIntersectsAlignedBox( const TestAlignedBox3d& ) const;
//	TestPlane3d createPlane3dNotIntersectsAlignedBox( const TestAlignedBox3d& ) const;

	TestUnalignedBox3d createUnalignedBoxIntersectsUnalignedBox( const TestUnalignedBox3d& ) const;
	TestUnalignedBox3d createUnalignedBoxNotIntersectsUnalignedBox( const TestUnalignedBox3d& ) const;
	TestLine3d createLine3dIntersectsUnalignedBox( const TestUnalignedBox3d& ) const;
	TestLine3d createLine3dNotIntersectsUnalignedBox( const TestUnalignedBox3d& ) const;
	TestLine3d createLine3dIntersectsLine3d( const TestLine3d& ) const;
	TestLine3d createLine3dNotIntersectsLine3d( const TestLine3d& ) const;
	TestLine3d createLine3dIntersectsPlane3d( const TestPlane3d& ) const;
	TestLine3d createLine3dNotIntersectsPlane3d( const TestPlane3d& ) const;
	TestAlignedBox3d createAlignedBoxIntersectsPlane3d( const TestPlane3d& ) const;
	TestAlignedBox3d createAlignedBoxNotIntersectsPlane3d( const TestPlane3d& ) const;
	TestUnalignedBox3d createUnalignedBoxIntersectsPlane3d( const TestPlane3d& ) const;
	TestUnalignedBox3d createUnalignedBoxNotIntersectsPlane3d( const TestPlane3d& ) const;

	TestUnalignedBox3d createUnalignedBoxIncludingPoint( const Point3& ) const;

	void synsug(const bool&, ostream& out) const;

	// copy-ctor, assignment and equivalence operators intentionally revoked
	BoundaryTest( const BoundaryTest& );
	const BoundaryTest& operator =( const BoundaryTest& );
	bool operator ==( const BoundaryTest& ) const;


	unsigned long numberOfTests_;
	unsigned long numberOfFailures_;
	time_t 		  timeSeed_;
};

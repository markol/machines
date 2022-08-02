/*
 * C S 2 D T E S T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysTestConfigSpace2d

    Implements test for the PhysConfigSpace2d class.
*/

#ifndef _PHYS_CS2DTEST_HPP
#define _PHYS_CS2DTEST_HPP

#include "base/base.hpp"

#include "phys/cspace2.hpp"

//Forward refs
//class ostream;
class PhysConfigSpace2d;

class PhysTestConfigSpace2d
{
public:
    PhysTestConfigSpace2d( ostream& out );
    ~PhysTestConfigSpace2d();

    //Add the test domains, checking results
    bool addDomains( PhysConfigSpace2d* pSpace );

    //Add the test portals, checking results
    bool addPortals( PhysConfigSpace2d* pSpace );

    //Add the permanent/temporary obstacles
    bool addPermanentPolygons( PhysConfigSpace2d* pSpace );
    bool addTemporaryPolygons( PhysConfigSpace2d* pSpace );

    //Check containment methods
    bool testContainsPoint( PhysConfigSpace2d* pSpace );
    bool testContainsLine( PhysConfigSpace2d* pSpace );
    bool testContainsPolygon( PhysConfigSpace2d* pSpace );

    //Check path finding
    bool testFindPaths( PhysConfigSpace2d* pSpace, ostream& out );
    bool testFindPath
    (
        ostream& out,
        PhysConfigSpace2d* pSpace,
        const MexPoint2d& startPoint,
        const MexPoint2d& endPoint,
        MATHEX_SCALAR clearance,
        size_t nAnswerPoints,
        MATHEX_SCALAR xRight[],
        MATHEX_SCALAR yRight[],
        MATHEX_SCALAR maxError[]
    );

    bool testDomainFindPaths( PhysConfigSpace2d* pSpace, ostream& out );
    bool oneTest( ostream& out );
    bool testDomainFindPath
    (
        ostream& out,
        PhysConfigSpace2d* pSpace,
        const MexPoint2d& startPoint,
        const MexPoint2d& endPoint,
        size_t nAnswerPoints,
        PhysConfigSpace2d::PortalId portalIdRight[],
        MATHEX_SCALAR distanceRight[]
    );

    bool testIntersects( ostream& out );

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    PhysTestConfigSpace2d( const PhysTestConfigSpace2d& );
    PhysTestConfigSpace2d& operator =( const PhysTestConfigSpace2d& );
    bool operator ==( const PhysTestConfigSpace2d& );

    //data Members
    PhysConfigSpace2d::DomainId aDomainIds[12];
    PhysConfigSpace2d::PortalId aPortalIds[12];
    PhysConfigSpace2d::PolygonId aPolygonIds[32];
    size_t nPolygons_;
};


#endif

/* End CS2DTEST.HPP *************************************************/

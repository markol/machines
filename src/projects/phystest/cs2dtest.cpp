/*
 * C S 2 D T E S T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

//#include <conio.h>
#include <fstream>

#include "ctl/nbvector.hpp"

#include "device/time.hpp"
#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/double.hpp"
#include "mathex/cvexpgon.hpp"

#include "phys/cspace2.hpp"
#include "phys/rampacce.hpp"
#include "phys/motchunk.hpp"
#include "phystest/cs2dtest.hpp"

//////////////////////////////////////////////////////////////////////////////////////////

PhysTestConfigSpace2d::PhysTestConfigSpace2d( ostream& out )
:   nPolygons_( 0 )
{
    const MATHEX_SCALAR minPortalClearance = 1.0;
    const MATHEX_SCALAR maxPortalClearance = 8.0;

    const PhysConfigSpace2d::ObstacleFlags allFlags = 3;

    //Construct a config space
    PhysConfigSpace2d space( MexPoint2d( 0, 0 ), MexPoint2d( 2800, 2800 ),
                             PhysConfigSpace2d::SUBTRACTIVE, 100,
                             minPortalClearance,
                             maxPortalClearance, allFlags );

    //Add the domains, checking works ok
    bool okay = true;
    okay = addDomains( &space );

    //Add the permanent obstacles, checking works ok
    if( okay ) okay = addPermanentPolygons( &space );

    //Add the temporary obstacles, checking works ok
    if( okay ) okay = addTemporaryPolygons( &space );

    //Add the portals, checking works ok
    if( okay ) okay = addPortals( &space );

    out << space;

    //Check containment methods
    if( okay ) okay = testContainsPoint( &space );
    if( okay ) okay = testContainsLine( &space );
    if( okay ) okay = testContainsPolygon( &space );

    //Check path finding
    if( okay ) okay = testFindPaths( &space, out );
    if( okay ) okay = testDomainFindPaths( &space, out );

    if( okay ) okay = testIntersects( out );

    space.traceObstacles( Diag::instance().bobStream() );

    std::cout << "ConfigSpace2d test " << (okay ? "passed" : "failed !!!") << std::endl;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysTestConfigSpace2d::~PhysTestConfigSpace2d()
{
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysTestConfigSpace2d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::addDomains( PhysConfigSpace2d* pSpace )
{
    bool okay = true;
    typedef MexConvexPolygon2d::Points Points;

    //Add each domain
    MexAlignedBox2d box0( MexPoint2d(333, 333), MexPoint2d(444, 444) );
    aDomainIds[0] = pSpace->addDomain( box0 );

    MexAlignedBox2d box1( MexPoint2d(2100, 0), MexPoint2d(2800, 800) );
    aDomainIds[1] = pSpace->addDomain( box1 );

    MexAlignedBox2d box2( MexPoint2d(1300, 800), MexPoint2d(2000, 1600) );
    Points* pPoints2 = _NEW( ctl_vector< MexPoint2d > );
    pPoints2->reserve( 6 );
    pPoints2->push_back( MexPoint2d(1400,800) );
    pPoints2->push_back( MexPoint2d(2000,800) );
    pPoints2->push_back( MexPoint2d(1500,1600) );
    pPoints2->push_back( MexPoint2d(1300,1600) );
    pPoints2->push_back( MexPoint2d(1300,900) );
    std::auto_ptr< Points > points2APtr( pPoints2 );
    MexConvexPolygon2d* pPolygon2 = _NEW( MexConvexPolygon2d( points2APtr ) );
    std::auto_ptr< MexPolygon2d > polygon2APtr( pPolygon2 );
    aDomainIds[2] = pSpace->addDomain( box2, polygon2APtr );

    MexAlignedBox2d box3( MexPoint2d(700, 1700), MexPoint2d(1300, 2800) );
    aDomainIds[3] = pSpace->addDomain( box3 );

    MexAlignedBox2d box4( MexPoint2d(0, 800), MexPoint2d(700, 1700) );
    aDomainIds[4] = pSpace->addDomain( box4 );

    MexAlignedBox2d box5( MexPoint2d(0, 1700), MexPoint2d(700, 2800) );
    aDomainIds[5] = pSpace->addDomain( box5 );

    MexAlignedBox2d box6( MexPoint2d(555, 555), MexPoint2d(666, 666) );
    aDomainIds[6] = pSpace->addDomain( box6 );

    MexAlignedBox2d box7( MexPoint2d(1500, 800), MexPoint2d(2800, 2800) );
    Points* pPoints7 = _NEW( ctl_vector< MexPoint2d > );
    pPoints7->reserve( 5 );
    pPoints7->push_back( MexPoint2d(2000,800) );
    pPoints7->push_back( MexPoint2d(2800,800) );
    pPoints7->push_back( MexPoint2d(2800,2800) );
    pPoints7->push_back( MexPoint2d(1500,2800) );
    pPoints7->push_back( MexPoint2d(1500,1600) );
    std::auto_ptr< Points > points7APtr( pPoints7 );
    MexConvexPolygon2d* pPolygon7 = _NEW( MexConvexPolygon2d( points7APtr ) );
    std::auto_ptr< MexPolygon2d > polygon7APtr( pPolygon7 );
    aDomainIds[7] = pSpace->addDomain( box7, polygon7APtr );

    MexAlignedBox2d box8( MexPoint2d(1400, 0), MexPoint2d(2100, 800) );
    aDomainIds[8] = pSpace->addDomain( box8 );

    MexAlignedBox2d box9( MexPoint2d(0, 0), MexPoint2d(700, 800) );
    aDomainIds[9] = pSpace->addDomain( box9 );

    MexAlignedBox2d box10( MexPoint2d(700, 0), MexPoint2d(1400, 900) );
    Points* pPoints10 = _NEW( ctl_vector< MexPoint2d > );
    pPoints10->reserve( 6 );
    pPoints10->push_back( MexPoint2d(700,0) );
    pPoints10->push_back( MexPoint2d(1400,0) );
    pPoints10->push_back( MexPoint2d(1400,800) );
    pPoints10->push_back( MexPoint2d(1300,900) );
    pPoints10->push_back( MexPoint2d(800,800) );
    pPoints10->push_back( MexPoint2d(700,700) );
    std::auto_ptr< Points > points10APtr( pPoints10 );
    MexConvexPolygon2d* pPolygon10 = _NEW( MexConvexPolygon2d( points10APtr ) );
    std::auto_ptr< MexPolygon2d > polygon10APtr( pPolygon10 );
    aDomainIds[10] = pSpace->addDomain( box10, polygon10APtr );

    MexAlignedBox2d box11( MexPoint2d(700, 800), MexPoint2d(1300, 1700) );
    Points* pPoints11 = _NEW( ctl_vector< MexPoint2d > );
    pPoints11->reserve( 5 );
    pPoints11->push_back( MexPoint2d(700,800) );
    pPoints11->push_back( MexPoint2d(800,800) );
    pPoints11->push_back( MexPoint2d(1300,900) );
    pPoints11->push_back( MexPoint2d(1300,1700) );
    pPoints11->push_back( MexPoint2d(700,1700) );
    std::auto_ptr< Points > points11APtr( pPoints11 );
    MexConvexPolygon2d* pPolygon11 = _NEW( MexConvexPolygon2d( points11APtr ) );
    std::auto_ptr< MexPolygon2d > polygon11APtr( pPolygon11 );
    aDomainIds[11] = pSpace->addDomain( box11, polygon11APtr );


    //Add a spurious portal between the spurious domains
    aPortalIds[0] = pSpace->addPortal( aDomainIds[0], aDomainIds[6],
                                      MexPoint2d(444,444), MexPoint2d(555,555) );

    //Delete the spurious ones
    pSpace->removeDomain( aDomainIds[0] );
    pSpace->removeDomain( aDomainIds[6] );

    //Check exist
    if( okay ) okay = not pSpace->domainExists( aDomainIds[0] );
    if( okay ) okay = pSpace->domainExists( aDomainIds[1] );
    if( okay ) okay = pSpace->domainExists( aDomainIds[2] );
    if( okay ) okay = pSpace->domainExists( aDomainIds[3] );
    if( okay ) okay = pSpace->domainExists( aDomainIds[4] );
    if( okay ) okay = pSpace->domainExists( aDomainIds[5] );
    if( okay ) okay = not pSpace->domainExists( aDomainIds[6] );
    if( okay ) okay = pSpace->domainExists( aDomainIds[7] );
    if( okay ) okay = pSpace->domainExists( aDomainIds[8] );
    if( okay ) okay = pSpace->domainExists( aDomainIds[9] );
    if( okay ) okay = pSpace->domainExists( aDomainIds[10] );
    if( okay ) okay = pSpace->domainExists( aDomainIds[11] );
    if( okay ) okay = not pSpace->portalExists( aPortalIds[0] );

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::addPortals( PhysConfigSpace2d* pSpace )
{
    bool okay = true;

    //Add each portal
    aPortalIds[1] = pSpace->addPortal( aDomainIds[9], aDomainIds[10],
                                      MexPoint2d(700,100), MexPoint2d(700,700) );
    aPortalIds[2] = pSpace->addPortal( aDomainIds[10], aDomainIds[8],
                                      MexPoint2d(1400,700), MexPoint2d(1400,100) );
    aPortalIds[3] = pSpace->addPortal( aDomainIds[10], aDomainIds[11],
                                      MexPoint2d(800,800), MexPoint2d(1300,900) );
    aPortalIds[4] = pSpace->addPortal( aDomainIds[8], aDomainIds[2],
                                      MexPoint2d(1500,800), MexPoint2d(2000,800) );
    aPortalIds[5] = pSpace->addPortal( aDomainIds[1], aDomainIds[7],
                                      MexPoint2d(2200,800), MexPoint2d(2700,800) );
    aPortalIds[6] = pSpace->addPortal( aDomainIds[2], aDomainIds[11],
                                      MexPoint2d(1300,900), MexPoint2d(1300,1600) );
    aPortalIds[7] = pSpace->addPortal( aDomainIds[4], aDomainIds[11],
                                      MexPoint2d(700,900), MexPoint2d(700,1600) );
    aPortalIds[8] = pSpace->addPortal( aDomainIds[2], aDomainIds[7],
                                      MexPoint2d(2000,800), MexPoint2d(1500,1600) );
    aPortalIds[9] = pSpace->addPortal( aDomainIds[4], aDomainIds[5],
                                      MexPoint2d(200,1700), MexPoint2d(600,1700) );
    aPortalIds[10] = pSpace->addPortal( aDomainIds[5], aDomainIds[3],
                                      MexPoint2d(700,1800), MexPoint2d(700,2700) );

    //Check we have some
    if( okay ) okay = pSpace->portalExists( aPortalIds[1] );
    if( okay ) okay = pSpace->portalExists( aPortalIds[8] );

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::addPermanentPolygons( PhysConfigSpace2d* pSpace )
{
    bool okay = true;

    //Define all the points
    MexPoint2d aPoints[] =
    {
        MexPoint2d(0,0),//0
        MexPoint2d(200,0),
        MexPoint2d(300,0),
        MexPoint2d(1900,0),
        MexPoint2d(2800,0),
        MexPoint2d(300,100),//5
        MexPoint2d(1900,100),
        MexPoint2d(200,200),//7
        MexPoint2d(2000,200),
        MexPoint2d(2200,200),
        MexPoint2d(2700,200),
        MexPoint2d(2800,200),
        MexPoint2d(0,600),//12
        MexPoint2d(200,600),
        MexPoint2d(300,700),//14
        MexPoint2d(700,700),
        MexPoint2d(1400,700),
        MexPoint2d(800,800),//17
        MexPoint2d(1500,800),
        MexPoint2d(2000,800),
        MexPoint2d(2200,800),
        MexPoint2d(300,900),//21
        MexPoint2d(700,900),
        MexPoint2d(1300,900),
        MexPoint2d(0,1000),//24
        MexPoint2d(200,1000),
        MexPoint2d(700,1600),//26
        MexPoint2d(1100,1600),
        MexPoint2d(1300,1600),
        MexPoint2d(1500,1600),
        MexPoint2d(600,1700),//30
        MexPoint2d(700,1800),//31
        MexPoint2d(1100,1800),
        MexPoint2d(1300,2000),//33
        MexPoint2d(0,2500),//34
        MexPoint2d(200,2500),
        MexPoint2d(400,2700),//36
        MexPoint2d(1300,2700),
        MexPoint2d(1500,2700),
        MexPoint2d(2700,2700),
        MexPoint2d(2800,2700),
        MexPoint2d(0,2800),//41
        MexPoint2d(400,2800),
        MexPoint2d(2800,2800),
        MexPoint2d(800,800),
        MexPoint2d(1000,800), //45
        MexPoint2d(1000,900),
        MexPoint2d(800,900),
    };

    size_t aPolygonVertices[] =
    {
        0,4,5,999,//To be deleted
        0,1,13,12,999,//H1
        1,2,5,7,999,//H2
        2,3,6,5,999,//H3
        3,4,11,8,6,999,//H4
        8,9,20,19,999,//H5
        30,26,27,32,31,999,//H6
        27,28,33,32,999,//H7
        28,29,38,37,999,//H8
        16,18,23,999,//H9
        14,15,17,22,21,999,//H10
        12,13,14,21,25,24,999,//H11
        24,25,35,34,999,//H12
        34,35,36,42,41,999,//H13
        36,40,43,42,999,//H14
        10,11,40,39,999,//H15
        44,45,46,47,999,
        9999
    };

    PhysConfigSpace2d::ObstacleFlags   flags = 0;

    //Define the polygons
    ctl_vector< MexPoint2d* > points;
    size_t i = 0;
    while( okay and aPolygonVertices[i] != 9999 )
    {
        while( aPolygonVertices[i] != 999 )
        {
            if( aPolygonVertices[i] == 44 )
                flags = 1;

            points.push_back( aPoints+(aPolygonVertices[i++]) );
        }
        ++i;

        MexPolygon2d* pPolygon = _NEW( MexConvexPolygon2d( points ) );
        points.erase( points.begin(), points.end() );

        std::auto_ptr< MexPolygon2d > polygonAPtr( pPolygon );
        aPolygonIds[ nPolygons_ ] = pSpace->add( polygonAPtr, 444, flags, PhysConfigSpace2d::PERMANENT );
        okay = pSpace->exists( aPolygonIds[ nPolygons_++ ] );
    }

    //try deleting the first one
    if( okay )
    {
        pSpace->remove( aPolygonIds[0] );
        okay = not pSpace->exists( aPolygonIds[0] );
    }

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::addTemporaryPolygons( PhysConfigSpace2d* pSpace )
{
    bool okay = true;

    //Define all the points
    MexPoint2d aPoints[] =
    {
        MexPoint2d(900,1100),//T1
        MexPoint2d(1100,1100),
        MexPoint2d(1100,1300),
        MexPoint2d(900,1300),
        MexPoint2d(2000,1200),//T2
        MexPoint2d(2400,1200),
        MexPoint2d(2400,1400),
        MexPoint2d(2000,1400),
        MexPoint2d(1600,1600),//T3
        MexPoint2d(1800,1600),
        MexPoint2d(1800,1700),
        MexPoint2d(1600,1700),
        MexPoint2d(1900,2000),//T4
        MexPoint2d(2300,1700),
        MexPoint2d(2000,2200),
        MexPoint2d(2400,2400),//T5
        MexPoint2d(2200,2400),
        MexPoint2d(2200,2100),
        MexPoint2d(2400,2100)
    };

    size_t aPolygonVertices[] =
    {
        0,1,2,3,999,//T16
        4,5,6,7,999,//T17
        8,9,10,11,999,//T18
        12,13,14,999,//T19
        15,16,17,18,999,//T20
        9999
    };

    //Define the polygons
    ctl_vector< MexPoint2d* > points;
    size_t i = 0;
    while( okay and aPolygonVertices[i] != 9999 )
    {
        while( aPolygonVertices[i] != 999 )
        {
            points.push_back( aPoints+(aPolygonVertices[i++]) );
        }
        ++i;

        MexPolygon2d* pPolygon = _NEW( MexConvexPolygon2d( points ) );
        points.erase( points.begin(), points.end() );

        std::auto_ptr< MexPolygon2d > polygonAPtr( pPolygon );
        const PhysConfigSpace2d::ObstacleFlags   flags = 0;
        aPolygonIds[ nPolygons_ ] = pSpace->add( polygonAPtr, 111, flags, PhysConfigSpace2d::TEMPORARY );
        okay = pSpace->exists( aPolygonIds[ nPolygons_++ ] );
    }

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::testContainsPoint( PhysConfigSpace2d* pSpace )
{
    bool okay = true;
    PhysConfigSpace2d::PolygonId polygonId;

    const PhysConfigSpace2d::ObstacleFlags   flags = 0;

    if( okay )
        okay = not pSpace->contains( MexPoint2d( 250, 125 ), flags, &polygonId,
                                     PhysConfigSpace2d::USE_ALL )
               and polygonId == aPolygonIds[2];

    if( okay )
        okay = pSpace->contains( MexPoint2d( 250, 175 ), flags, &polygonId,
                                     PhysConfigSpace2d::USE_ALL );

    if( okay )
        okay = not pSpace->contains( MexPoint2d( 2200, 1300 ), flags, &polygonId,
                                     PhysConfigSpace2d::USE_TEMPORARY_ONLY )
               // this is questionable, point is in polygon created as 18th, smbdy forgot about zero index?
               //and polygonId == aPolygonIds[17];
               and polygonId == aPolygonIds[18];

    if( okay )
        okay = pSpace->contains( MexPoint2d( 2200, 1300 ), flags, &polygonId,
                                     PhysConfigSpace2d::USE_PERMANENT_ONLY );

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::testContainsLine( PhysConfigSpace2d* pSpace )
{
    bool okay = true;
    PhysConfigSpace2d::PolygonId polygonId;

    const PhysConfigSpace2d::ObstacleFlags   flags = 0;

    if( okay )
        okay = pSpace->contains( MexPoint3d( 400, 2600, 10 ), MexPoint3d( 1100, 1900, 10 ),
                                     flags, &polygonId, PhysConfigSpace2d::USE_ALL );

    if( okay )
        okay = not pSpace->contains( MexPoint3d( 1200, 800, 10 ), MexPoint3d( 1700, 700, 10 ),
                                     flags, &polygonId, PhysConfigSpace2d::USE_PERMANENT_ONLY )
               and polygonId == aPolygonIds[9];

    if( okay )
        okay = pSpace->contains( MexPoint3d( 1200, 800, 1000 ), MexPoint3d( 1700, 700, 1000 ),
                                     flags, &polygonId, PhysConfigSpace2d::USE_PERMANENT_ONLY );

    if( okay )
        okay = pSpace->contains( MexPoint3d( 1200, 800, 10 ), MexPoint3d( 1700, 700, 10 ),
                                     flags, &polygonId, PhysConfigSpace2d::USE_TEMPORARY_ONLY );

    if( okay )
        okay = not pSpace->contains( MexPoint3d( 1300, 700, 10 ), MexPoint3d( 1700, 1650, 10 ),
                                     flags, &polygonId, PhysConfigSpace2d::USE_TEMPORARY_ONLY )
               //and polygonId == aPolygonIds[18];
               and polygonId == aPolygonIds[19];

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::testContainsPolygon( PhysConfigSpace2d* pSpace )
{
    bool okay = true;
    PhysConfigSpace2d::PolygonId polygonId;

    //Define all the points
    MexPoint2d aPoints[] =
    {
        MexPoint2d(300,1900),//1
        MexPoint2d(1100,1900),
        MexPoint2d(1200,2000),
        MexPoint2d(1200,2600),
        MexPoint2d(400,2600),
        MexPoint2d(300,2500),
        MexPoint2d(1800,2000),//2
        MexPoint2d(1800,1900),
        MexPoint2d(2000,1900),
        MexPoint2d(2000,2000),
        MexPoint2d(1399,799),//3
        MexPoint2d(1401,799),
        MexPoint2d(1401,801),
        MexPoint2d(1399,801)
    };

    size_t aPolygonVertices[] =
    {
        0,1,2,3,4,5,999,//1
        6,7,8,9,999,//2
        10,11,12,13,999,//3
        9999
    };

    //Define the polygons
    ctl_vector< MexPoint2d* > points;
    ctl_vector< MexPolygon2d* > polygons;
    size_t i = 0;
    while( okay and aPolygonVertices[i] != 9999 )
    {
        while( aPolygonVertices[i] != 999 )
        {
            points.push_back( aPoints+(aPolygonVertices[i++]) );
        }
        ++i;

        polygons.push_back( _NEW( MexConvexPolygon2d( points ) ) );
        points.erase( points.begin(), points.end() );
    }

    const PhysConfigSpace2d::ObstacleFlags   flags = 0;

    //Perform the tests
    if( okay ) okay = pSpace->contains( *(polygons[0]), flags, &polygonId,
                                        PhysConfigSpace2d::USE_ALL );

    if( okay ) okay = pSpace->contains( *(polygons[1]), flags, &polygonId,
                                        PhysConfigSpace2d::USE_PERMANENT_ONLY );

    if( okay ) okay = not pSpace->contains( *(polygons[1]), flags, &polygonId,
                                            PhysConfigSpace2d::USE_TEMPORARY_ONLY )
                      //and polygonId == aPolygonIds[19];
                      and polygonId == aPolygonIds[20];

    if( okay ) okay = pSpace->contains( *(polygons[2]), flags, &polygonId,
                                        PhysConfigSpace2d::USE_TEMPORARY_ONLY );

    if( okay ) okay = not pSpace->contains( *(polygons[2]), flags, &polygonId,
                                            PhysConfigSpace2d::USE_PERMANENT_ONLY )
                      and polygonId == aPolygonIds[9];

    //Delete the polygons
    size_t nPolygons = polygons.size();
    for( size_t j = 0; j != nPolygons; ++j )
        _DELETE( polygons[j] );

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::testFindPaths( PhysConfigSpace2d* pSpace, ostream& out )
{
    bool okay = true;

    if( okay )
        okay = oneTest( out );

    if( okay )
    {
        MATHEX_SCALAR xRight[] = {400, 1100};
        MATHEX_SCALAR yRight[] = {2500,2000};
        MATHEX_SCALAR maxError [] = {0, 0};

        okay = testFindPath( out, pSpace, MexPoint2d(400,2500), MexPoint2d(1100,2000), 50.0,
                             2, xRight, yRight, maxError );
    }

    //Make the temporary polygons permanent
    pSpace->longevity( aPolygonIds[16], PhysConfigSpace2d::PERMANENT );
    pSpace->longevity( aPolygonIds[17], PhysConfigSpace2d::PERMANENT );
    pSpace->longevity( aPolygonIds[18], PhysConfigSpace2d::PERMANENT );
    pSpace->longevity( aPolygonIds[19], PhysConfigSpace2d::PERMANENT );
    pSpace->longevity( aPolygonIds[20], PhysConfigSpace2d::PERMANENT );

    if( okay )
    {
        MATHEX_SCALAR xRight[] = {2000,2130,2470,2470,2400};
        MATHEX_SCALAR yRight[] = {2400,2470,2470,2030,1900};
        MATHEX_SCALAR maxError [] = {0, 10, 10, 10, 0};
        okay = testFindPath( out, pSpace, MexPoint2d(2000,2400), MexPoint2d(2400,1900), 50.0,
                             5, xRight, yRight, maxError );
    }

    if( okay )
    {
        MATHEX_SCALAR xRight[] = {700,830,1300};
        MATHEX_SCALAR yRight[] = {1000,1370,1500};
        MATHEX_SCALAR maxError [] = {0, 10, 0};
        okay = testFindPath( out, pSpace, MexPoint2d(700,1000), MexPoint2d(1300,1500), 50.0,
                             3, xRight, yRight, maxError );
    }

    if( okay )
    {
        MATHEX_SCALAR xRight[] = {1550,1870,2300,2390,2300};
        MATHEX_SCALAR yRight[] = {1515,1530,1610,1680,2000};
        MATHEX_SCALAR maxError [] = {10, 10, 10, 10, 0};
        okay = testFindPath( out, pSpace, MexPoint2d(1550,1515), MexPoint2d(2300,2000), 50.0,
                             5, xRight, yRight, maxError );
    }

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::testFindPath
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
)
{
    bool okay = true;

    //Time it
    double startTime = DevTime::instance().time();

    const PhysConfigSpace2d::ObstacleFlags   flags = 0;

    //Set up the findPath
    // TODO: check added last arg
    PhysConfigSpace2d::FindPathId findPathId =
        pSpace->addFindPath( startPoint, endPoint, clearance, flags,
                            Phys::defaultPathFindingPriority() );

    okay = pSpace->findPathExists( findPathId );

    //Loop updating while we get the answer
    while( okay and not pSpace->isFindPathDone( findPathId ) )
        pSpace->updateFindPath( findPathId, PhysRelativeTime( 10 ) );

    //Get the answer
    PhysConfigSpace2d::Path path;
    okay = pSpace->findPath( findPathId, &path ) and
        path.size() == nAnswerPoints;

    //Report time taken
    double timeTaken = DevTime::instance().time() - startTime;
    std::cout << "FindPath search took " << timeTaken << " seconds" << std::endl;

    if( okay )
    {
        //Check the coordinates
        for( size_t i = 0; okay and i != nAnswerPoints; ++i )
        {
            okay = fabs( path[i].x() - xRight[i] ) <= maxError[i] and
                   fabs( path[i].y() - yRight[i] ) <= maxError[i];
        }
    }

    //Report the path found if we have one, but not the one expected
    if( not okay and path.size() != 0 )
    {
        out << "Different path found" << std::endl;
        out << "Start found path" << std::endl;
        for( size_t i = 0; i != path.size(); ++i )
        {
            out << "  " << path[i] << std::endl;
        }
        out << "End found path" << std::endl;

        out << "Start expected path" << std::endl;
        for( size_t i = 0; i != nAnswerPoints; ++i )
        {
            out << "  " << xRight[i] << "  " << yRight[ i ] << std::endl;
        }
        out << "End expected path" << std::endl;
    }

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::testDomainFindPaths( PhysConfigSpace2d* pSpace, ostream& out )
{
    bool okay = true;

    if( okay )
    {
        PhysConfigSpace2d::PortalId portalIdRight[] = {aPortalIds[9], aPortalIds[7],
                                         aPortalIds[3], aPortalIds[1]};
//        MATHEX_SCALAR distanceRight[] = {300,600,100,500};
        MATHEX_SCALAR distanceRight[] = {392,692,8,592};

        okay = testDomainFindPath( out, pSpace, MexPoint2d(600,2000), MexPoint2d(400,600),
                             4, portalIdRight, distanceRight );
    }

    if( okay )
    {
        PhysConfigSpace2d::PortalId portalIdRight[] = {aPortalIds[4], aPortalIds[8],
                                         aPortalIds[5]};
//        MATHEX_SCALAR distanceRight[] = {400,100,100};
        MATHEX_SCALAR distanceRight[] = {492,8,8};
        okay = testDomainFindPath( out, pSpace, MexPoint2d(1900,600), MexPoint2d(2300,600),
                             3, portalIdRight, distanceRight );
    }

    if( okay )
    {
        PhysConfigSpace2d::PortalId portalIdRight[] = {aPortalIds[7], aPortalIds[6],
                                         aPortalIds[8]};
//        MATHEX_SCALAR distanceRight[] = {100,600,843};
        MATHEX_SCALAR distanceRight[] = {8,692,935.398};

        okay = testDomainFindPath( out, pSpace, MexPoint2d(600,1000), MexPoint2d(2300,2000),
                             3, portalIdRight, distanceRight );
    }

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::testDomainFindPath
(
    ostream& out,
    PhysConfigSpace2d* pSpace,
    const MexPoint2d& startPoint,
    const MexPoint2d& endPoint,
    size_t nAnswerPoints,
    PhysConfigSpace2d::PortalId portalIdRight[],
    MATHEX_SCALAR distanceRight[]
)
{
    bool okay = true;

    //Time it
    double startTime = DevTime::instance().time();

    const MATHEX_SCALAR clearance = 2.0;
    const PhysConfigSpace2d::ObstacleFlags   flags = 0;

    //Set up the findPath

    PhysConfigSpace2d::DomainFindPathId domainFindPathId =
        pSpace->addDomainFindPath( startPoint, endPoint, clearance, flags,
                                  Phys::defaultPathFindingPriority() );

    okay = pSpace->domainFindPathExists( domainFindPathId );

    //Loop updating while we get the answer
    while( okay and not pSpace->isDomainFindPathDone( domainFindPathId ) )
        pSpace->updateDomainFindPath( domainFindPathId, PhysRelativeTime( 0.005 ) );

    //Get the answer
    PhysConfigSpace2d::PortalPoints portalPoints;
    portalPoints.reserve( nAnswerPoints );
    okay = pSpace->domainFindPath( domainFindPathId, &portalPoints ) and
        portalPoints.size() == nAnswerPoints;

    //Report time taken
    double timeTaken = DevTime::instance().time() - startTime;
    std::cout << "DomainFindPath search took " << timeTaken << " seconds" << std::endl;

    if( okay )
    {
        //Check the coordinates
        for( size_t i = 0; okay and i != nAnswerPoints; ++i )
        {
            okay = portalPoints[i].first == portalIdRight[i]    and
                   fabs( portalPoints[i].second - distanceRight[i] ) < 1.0;
        }
    }

    //Report the path found if we have one, but not the one expected
    if( not okay and portalPoints.size() != 0 )
    {
        out << "Different path found" << std::endl;
        out << "Start found path" << std::endl;
        for( size_t i = 0; i != portalPoints.size(); ++i )
        {
            out << "  " << portalPoints[i].first.asScalar()
                << "  " << portalPoints[i].second << std::endl;
        }
        out << "End found path" << std::endl;
        out << "Start expected path" << std::endl;
        for( size_t i = 0; i != portalPoints.size(); ++i )
        {
            out << "  " << portalIdRight[i].asScalar()
                << "  " << distanceRight[i] << std::endl;
        }
        out << "End expected path" << std::endl;
    }

    return okay;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysTestConfigSpace2d::oneTest( ostream& out )
{
    const MATHEX_SCALAR minPortalClearance = 1.0;
    const MATHEX_SCALAR maxPortalClearance = 9.0;

    const PhysConfigSpace2d::ObstacleFlags allFlags = 3;

    //Construct a config space
    PhysConfigSpace2d space( MexPoint2d( 0, 0 ), MexPoint2d( 2800, 2800 ),
                             PhysConfigSpace2d::SUBTRACTIVE, 100,
                             minPortalClearance,
                             maxPortalClearance, allFlags );

    //Add a single polygon
    ctl_vector< MexPoint2d* > points;
    MexPoint2d p1(583, 233);
    MexPoint2d p2(817, 233);
    MexPoint2d p3(817, 379);
    MexPoint2d p4(583, 379);
    points.push_back( &p1 );
    points.push_back( &p2 );
    points.push_back( &p3 );
    points.push_back( &p4 );

    MexPolygon2d* pPolygon = _NEW( MexConvexPolygon2d( points ) );
    std::auto_ptr< MexPolygon2d > polygonAPtr( pPolygon );
    const PhysConfigSpace2d::ObstacleFlags   flags = 0;
    space.add( polygonAPtr, 444, flags, PhysConfigSpace2d::PERMANENT );

    //Test the findpath
    MATHEX_SCALAR xRight[]= {643.007, 821.285, 829};
    MATHEX_SCALAR yRight[]= {227, 228.715, 233};
    MATHEX_SCALAR maxError[]= {1,1,1};
    bool okay = testFindPath( out, &space, MexPoint2d(643.007,227),
                              MexPoint2d(829,233), 3.0, 3, xRight, yRight, maxError);

    return okay;
}

bool PhysTestConfigSpace2d::testIntersects( ostream& )
{
    bool    okay = true;

    const MATHEX_SCALAR minPortalClearance = 1.0;
    const MATHEX_SCALAR maxPortalClearance = 8.0;

    const PhysConfigSpace2d::ObstacleFlags allFlags = 3;

    //Construct a config space
    PhysConfigSpace2d space( MexPoint2d( 0, 0 ), MexPoint2d( 2800, 2800 ),
                             PhysConfigSpace2d::SUBTRACTIVE, 100,
                             minPortalClearance,
                             maxPortalClearance, allFlags );

    MATHEX_SCALAR           clearance = 1.0;

    MATHEX_SCALAR       startSpeed( 0.0 );
    MATHEX_SCALAR       firstAcceleration( 1.0 );
    MATHEX_SCALAR       secondAcceleration( -1.0 );
    PhysRelativeTime    totalTime( 20.0 );
    PhysRelativeTime    firstAccelerationTime( 4.0 );
    PhysRelativeTime    secondAccelerationTime( 4.0 );

    PhysRampAcceleration    motionProfile(
        startSpeed,
        firstAcceleration,
        secondAcceleration,
        totalTime,
        firstAccelerationTime,
        secondAccelerationTime );

    MexPoint2d  startPoint( 1000.0, 100.0 );
    MexPoint2d  endPoint(
      startPoint.x(),
      startPoint.y() + motionProfile.distance( motionProfile.totalTime() ) );

    PhysAbsoluteTime        createTime( 0.0 );
    PhysRelativeTime        motionTimeOffset( 0.0 );

    PhysMotionChunk     motionChunk(
        startPoint,
        endPoint,
        clearance,
        motionProfile,
        createTime,
        motionTimeOffset,
        MexDouble::minusInfinity(),
        MexDouble::plusInfinity() );

    PhysConfigSpace2d::MotionChunkId       motionChunkId;
    PhysConfigSpace2d::ObjectId            objectId = 1;
    PhysConfigSpace2d::ObjectId            collisionObjectId;
    PhysAbsoluteTime    time;

    bool added = space.add( motionChunk, objectId, &motionChunkId, &collisionObjectId, &time );

    if( not added )
        okay = false;

    if( okay )
    {
        //  Test a motion path that doesn't intersect
        MexPoint2d      startPoint2( startPoint.x() + 50.0, startPoint.y() );
        MexPoint2d      endPoint2( endPoint.x() + 50.0, endPoint.y() );

        PhysConfigSpace2d::ObjectId        objectId2( 2 );

        PhysMotionChunk     motionChunk2(
            startPoint2,
            endPoint2,
            clearance,
            motionProfile,
            createTime,
            motionTimeOffset,
            MexDouble::minusInfinity(),
            MexDouble::plusInfinity() );

        PhysConfigSpace2d::Intersections    intersections;

        space.findIntersections( motionChunk2, objectId2, &intersections );

        if( intersections.size() != 0 )
            okay = false;
    }

    if( okay )
    {
        //  Test a motion path that intersects once
        MexPoint2d      startPoint2(
          startPoint.x() - ( endPoint.y() - startPoint.y() ) / 2,
          ( endPoint.y() + startPoint.y() ) / 2 );

        MexPoint2d      endPoint2(
          startPoint.x() + ( endPoint.y() - startPoint.y() ) / 2,
          ( endPoint.y() + startPoint.y() ) / 2 );

        PhysConfigSpace2d::ObjectId        objectId2( 2 );

        PhysMotionChunk     motionChunk2(
            startPoint2,
            endPoint2,
            clearance,
            motionProfile,
            createTime,
            motionTimeOffset,
            MexDouble::minusInfinity(),
            MexDouble::plusInfinity() );

        PhysConfigSpace2d::Intersections    intersections;

        space.findIntersections( motionChunk2, objectId2, &intersections );

        if( intersections.size() == 1 )
        {
            if( intersections[ 0 ].collisionObjectId_ != 1 )
                okay = false;
        }
        else
            okay = false;
    }

    if( okay )
    {
        //  Test a motion path that intersects twice

        MexPoint2d      startPoint2(
          startPoint.x() - ( endPoint.y() - startPoint.y() ) / 2,
          ( endPoint.y() + startPoint.y() ) / 2 );

        MexPoint2d      endPoint2(
          startPoint2.x(),
          startPoint2.y() + endPoint.y() - startPoint.y() );

        PhysMotionChunk     motionChunk(
            startPoint2,
            endPoint2,
            clearance,
            motionProfile,
            createTime,
            motionTimeOffset,
            MexDouble::minusInfinity(),
            MexDouble::plusInfinity() );

        PhysConfigSpace2d::ObjectId            objectId = 2;

        bool added = space.add( motionChunk, objectId, &motionChunkId, &collisionObjectId, &time );

        MexPoint2d      startPoint3(
          startPoint.x() - ( endPoint.y() - startPoint.y() ) / 2,
          ( endPoint.y() + startPoint.y() ) / 2 );

        MexPoint2d      endPoint3(
          startPoint.x() + ( endPoint.y() - startPoint.y() ) / 2,
          ( endPoint.y() + startPoint.y() ) / 2 );

        PhysConfigSpace2d::ObjectId        objectId2( 1000 );

        PhysMotionChunk     motionChunk2(
            startPoint3,
            endPoint3,
            clearance,
            motionProfile,
            createTime,
            motionTimeOffset,
            MexDouble::minusInfinity(),
            MexDouble::plusInfinity() );

        PhysConfigSpace2d::Intersections    intersections;

        space.findIntersections( motionChunk2, objectId2, &intersections );

        if( intersections.size() == 2 )
        {
            if( intersections[ 0 ].collisionObjectId_ == 1 )
            {
                if( intersections[ 1 ].collisionObjectId_ != 2 )
                    okay = false;
            }
            else if( intersections[ 0 ].collisionObjectId_ == 1 )
            {
                if( intersections[ 1 ].collisionObjectId_ != 1 )
                    okay = false;
            }
            else
                okay = false;
        }
        else
            okay = false;
    }

    return okay;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2DTEST.CPP *************************************************/

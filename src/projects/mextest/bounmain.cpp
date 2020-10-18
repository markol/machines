/*
 * B O U N M A I N . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

//#include "mextest/bountest.hpp"
#include "mathex/random.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/utility.hpp"
#include "mathex/vec2.hpp"
#include "mathex/point2d.hpp"
#include "mathex/grid2d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/cvexpgon.hpp"
#include "mathex/degrees.hpp"
#include "mathex/mathex.hpp"

#include "device/timer.hpp"

static  void    randomTest();
static  void    quadraticTest();
static  void angleTest();
static  void angleTest2();
static  void gridTest();
static  void gridTest1();

void testPolygonIntersect();
MexPoint2d randomPoint2d( MexBasicRandom* pRandom );
MexConvexPolygon2d randomConvexPolygon2d( MexBasicRandom* pRandom );

using namespace std;

int main( void )
{
    INSPECT_ON( cout, MexEpsilon::instance() );
    INSPECT_ON( cout, MexEpsilon::sqrInstance() );

//	BoundaryTest	bt1;

//    randomTest();

    //quadraticTest();  // OK??


    //angleTest(); //OK?
    //angleTest2(); //OK

    //gridTest1(); //OK?
    //gridTest(); // OK?

    for( size_t i = 0; i < 100; ++i )
        testPolygonIntersect(); //OK

	return 0;
}

void quadraticTest()
{
    MATHEX_SCALAR   a, b, c;
    MATHEX_SCALAR   root1, root2;
    uint    nRoots;

    MexBasicRandom   r6;

    for( size_t i = 0; i < 100000; ++i )
    {
        a = mexRandomScalar( &r6, 0.0, 1.0 );
        b = mexRandomScalar( &r6, 0.0, 1.0 );
        c = mexRandomScalar( &r6, 0.0, 1.0 );

        nRoots = MexUtility::quadraticRealRoots(
          a, b, c, &root1, &root2 );
    }
}

// void    randomTest()
// {
//     MexBasicRandom   r1;
//     MexBasicRandom   r2;
//     MexBasicRandom   r3( r2 );
//     MexBasicRandom   r4;
//
//     r4.seedFromTime();
//
//     MexBasicRandom   r5( r4 );
//     MexBasicRandom   r6;
//
//     for( size_t i = 0; i < 2000000; ++i )
//     {
//         cout << r1.next() << " ";
//         cout << r2.next() << " ";
//         cout << r3.next() << " ";
//         cout << r4.next() << " ";
//         cout << r5.next() << " ";
//         cout << r6.next() << " ";
//         cout << mexRandomScalar( r6, 0.0, 1.0 ) << " ";
//
//         cout << mexRandomInt( r6, -200, 100 ) << " ";
//         cout << mexRandomInt( r6, 15000000 ) << " ";
//
//         cout << endl;
//     }
// }

void angleTest()
{
    MexBasicRandom   r1;
    r1.seedFromTime();

    for( size_t i = 0; i < 1000000; ++i )
    {
        MexVec3 v1;

        do{
            v1 = MexVec3(
              mexRandomScalar( &r1, -1.0, 1.0 ),
              mexRandomScalar( &r1, -1.0, 1.0 ),
              mexRandomScalar( &r1, -1.0, 1.0 ) );
        } while( v1.modulus() == 0.0 );

        MexVec3 v2;

        do{
            v2 = MexVec3(
              mexRandomScalar( &r1, -1.0, 1.0 ),
              mexRandomScalar( &r1, -1.0, 1.0 ),
              mexRandomScalar( &r1, -1.0, 1.0 ) );
        } while( v2.modulus() == 0.0 );

        MATHEX_SCALAR a1 = v1.angleBetween( v2 ).asScalar();
        MATHEX_SCALAR a2 = v2.angleBetween( v1 ).asScalar();
        MATHEX_SCALAR a3 = v1.signedAngleBetween( v2 ).asScalar();
        MATHEX_SCALAR a4 = v2.signedAngleBetween( v1 ).asScalar();

        ASSERT( fabs( a1 - a2 ) < MexEpsilon::instance(), "" );
        ASSERT( fabs( a3 + a4 ) < MexEpsilon::instance(), "" );

//        cout << a1 << " " << a2 << " " << a3 << " " << a4 << endl;
   }
}

void angleTest2()
{
    MexBasicRandom   r1;
//    r1.seedFromTime();

    for( size_t i = 0; i < 10000; ++i )
    {
        MexVec2 vec1;

        do{
            vec1 = MexVec2(
              mexRandomScalar( &r1, -1.0, 1.0 ),
              mexRandomScalar( &r1, -1.0, 1.0 ) );
        } while( vec1.modulus() == 0.0 );

//        vec1 = MexVec2(
//          mexRandomScalar( &r1, 0.0, 1.0 ) > 0.5 ? 1 : -1,
//          0.0 );


        MexVec2 vec2;

        do{
            vec2 = MexVec2(
              mexRandomScalar( &r1, -1.0, 1.0 ),
              mexRandomScalar( &r1, -1.0, 1.0 ) );
        } while( vec2.modulus() == 0.0 );

//        vec2 = MexVec2(
//          0.0,
//          mexRandomScalar( &r1, 0.0, 1.0 ) > 0.5 ? 1 : -1 );

        MexDegrees angle = vec1.angleBetween( vec2 );

        INSPECT_ON( cout, vec1 );
        INSPECT_ON( cout, vec2 );
        INSPECT_ON( cout, angle );

        vec1.rotate( angle );

        vec1.makeUnitVector();
        vec2.makeUnitVector();

        INSPECT_ON( cout, vec1 );
        INSPECT_ON( cout, vec2 );

        MexPoint2d p1 = vec1;
        MexPoint2d p2 = vec2;

        ASSERT( p1.euclidianDistance( p2 ) < 0.001, "" );

        cout << endl;
    }
}

void gridTest1()
{
    const size_t nXCells = 20;
    const size_t nYCells = 20;
    const MexPoint2d origin( 0, 0 );
    const MATHEX_SCALAR xSize = 120.0;
    const MATHEX_SCALAR ySize = 120.0;
WHERE;

    MexGrid2d   grid(
        nXCells,
        nYCells,
        xSize,
        ySize,
        origin );

WHERE;
    MexLine2d line(
        MexPoint2d( 731.2508,599.9916 ),
        MexPoint2d( 737.8994,600.0027 ) );

WHERE;
    MexGrid2d::Cells   cells;
    MexGrid2d::Points  points;

WHERE;
    grid.intersect( line, &cells, &points );

WHERE;

    for( size_t i = 0; i < cells.size(); ++i )
        cout << cells[ i ].xIndex() << "  " << cells[ i ].yIndex() << endl;

    for( size_t i = 0; i < points.size(); ++i )
        cout << points[ i ] << endl;
WHERE;
}

void gridTest()
{
    MexBasicRandom   r6;

    const size_t nTests = 1000000;

    for( size_t i = 0; i < nTests; ++i )
    {
        if( i % 100 == 0 )
            cout << "Test " << i << " of " << nTests << "\r";

        const size_t nXCells = mexRandomInt( &r6, 1, 1000 );
        const size_t nYCells = mexRandomInt( &r6, 1, 1000 );
        const MexPoint2d origin( mexRandomScalar( &r6, -1000.0, 1000.0 ), mexRandomScalar( &r6, -1000.0, 1000.0 ) );
        const MATHEX_SCALAR xSize = mexRandomScalar( &r6, 0.1, 1000.0 );
        const MATHEX_SCALAR ySize = mexRandomScalar( &r6, 0.1, 1000.0 );

        MexGrid2d   grid(
            nXCells,
            nYCells,
            xSize,
            ySize,
            origin );

        MexPoint2d end1;
        MexPoint2d end2;

        if( mexRandomInt( &r6, 0, 3 ) == 0 )
            end1.x( origin.x() );
        else if( mexRandomInt( &r6, 0, 2 ) == 0 )
            end1.x( origin.x() + nXCells * xSize );
        else if( mexRandomInt( &r6, 0, 2 ) == 0 )
            end1.x( origin.x() + mexRandomInt( &r6, -10, 1000 ) * xSize );
        else
            end1.x( mexRandomScalar( &r6, -1000.0, 1000.0 ) );

        if( mexRandomInt( &r6, 0, 3 ) == 0 )
            end1.y( origin.y() );
        else if( mexRandomInt( &r6, 0, 2 ) == 0 )
            end1.y( origin.y() + nYCells * ySize );
        else if( mexRandomInt( &r6, 0, 2 ) == 0 )
            end1.y( origin.y() + mexRandomInt( &r6, -10, 1000 ) * ySize );
        else
            end1.y( mexRandomScalar( &r6, -1000.0, 1000.0 ) );

        if( mexRandomInt( &r6, 0, 3 ) == 0 )
            end2.x( origin.x() );
        else if( mexRandomInt( &r6, 0, 2 ) == 0 )
            end2.x( origin.x() + nXCells * xSize );
        else if( mexRandomInt( &r6, 0, 2 ) == 0 )
            end2.x( origin.x() + mexRandomInt( &r6, -10, 1000 ) * xSize );
        else
            end2.x( mexRandomScalar( &r6, -1000.0, 1000.0 ) );

        if( mexRandomInt( &r6, 0, 3 ) == 0 )
            end2.y( origin.y() );
        else if( mexRandomInt( &r6, 0, 2 ) == 0 )
            end2.y( origin.y() + nYCells * ySize );
        else if( mexRandomInt( &r6, 0, 2 ) == 0 )
            end2.y( origin.y() + mexRandomInt( &r6, -10, 1000 ) * ySize );
        else
            end2.y( mexRandomScalar( &r6, -1000.0, 1000.0 ) );

        const MexLine2d line( end1, end2 );

        MexGrid2d::Cells   cells;
        MexGrid2d::Points  points;

        grid.intersect( line, &cells, &points );
    }
}

void testPolygonIntersect()
{
    MexBasicRandom r1;

    r1.seedFromTime();

    for( size_t i = 0; i < 1000; ++i )
    {
        MexConvexPolygon2d polygon = randomConvexPolygon2d( &r1 );

        for( size_t j = 0; j < 1000; ++j )
        {
            MexPoint2d point = randomPoint2d( &r1 );

            const size_t vertexIndex = mexRandomInt( &r1, (int)polygon.nVertices() );

            INSPECT_ON( cout, polygon );
            INSPECT_ON( cout, point );
            INSPECT_ON( cout, vertexIndex );

            polygon.intersects( vertexIndex, point );
        }
    }
}

MexPoint2d randomPoint2d( MexBasicRandom* pRandom )
{
    MexPoint2d point(
      mexRandomScalar( pRandom, -1000.0, 1000.0 ),
      mexRandomScalar( pRandom, -1000.0, 1000.0 ) );

    return point;
}

MexConvexPolygon2d randomConvexPolygon2d( MexBasicRandom* pRandom )
{
    MexConvexPolygon2d::Points  points;

    const size_t nVertices = mexRandomInt( pRandom, 3, 20 );
    const MATHEX_SCALAR radius = mexRandomScalar( pRandom, 10.0, 100.0 );

    MexDegrees angle = mexRandomScalar( pRandom, 0.0, 360.0 );

    MATHEX_SCALAR remainingAngle = 360.0;

    for( size_t i = 0; i < nVertices; ++i )
    {
        points.push_back( MexPoint2d( radius * cos( angle ), radius * sin( angle ) ) );

        const MATHEX_SCALAR angleIncrement = mexRandomScalar( pRandom, remainingAngle * 0.1, remainingAngle * 0.5 );

        remainingAngle -= angleIncrement;

        angle = angle.asScalar() + angleIncrement;
    }

    MexConvexPolygon2d result( points );

    return result;
}

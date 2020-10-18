/*
 * T E S T T I B O . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"

#include "mathex/point3d.hpp"
#include "machphys/tileboun.hpp"

#include "planeted/testtibo.hpp"

//////////////////////////////////////////////////////////////////////////////////////////

MachPhysTestTileBoundary::MachPhysTestTileBoundary()
{

    //Construct a tile boundary
    MachPhysTileBoundary tileBoundary( 4, 3, 6, -5.0, 10.0, 10.0, 30.0 );
    MexPoint3d p0( -5, 10, 200 );
    MexPoint3d p1( 5, 10, 0 );
    MexPoint3d p2( 10, 10, 100 );
    MexPoint3d p3( -5, 20, 0 );
    MexPoint3d p4( 5, 20, 0 );
    MexPoint3d p5( 10, 20, 0 );
    MexPoint3d p6( -5, 30, 0 );
    MexPoint3d p7( 10, 30, 300 );

    tileBoundary.add( p0, p1, p4 );
    tileBoundary.add( p2, p1, p4 );
    tileBoundary.add( p0, p3, p4 );
    tileBoundary.add( p5, p3, p6 );
    tileBoundary.add( p2, p5, p4 );
    tileBoundary.add( p5, p6, p7 );

    tileBoundary.doneBuilding();

    //Check some test coordinates
    MATHEX_SCALAR x[19] = {-5,0,5,10,-5,0,5,10,-5,0,5,10,7.5,7.5,7.5,2.5,0,3,7.5};
    MATHEX_SCALAR y[19] = {10,10,10,10,20,20,20,20,30,30,30,30,10,11,17.5,17,17.5,21,29};
    MATHEX_SCALAR z[19] = {200,100,0,100,0,0,0,0,0,100,200,300,50,50,25,50,50,0,220};

    for( size_t i = 0; i < 19; ++i )
    {
        MATHEX_SCALAR height = tileBoundary.height( x[i], y[i] );
        POST( abs( height - z[i] ) < 0.001 );
    }

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhysTestTileBoundary::~MachPhysTestTileBoundary()
{
    TEST_INVARIANT;

}
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysTestTileBoundary::CLASS_INVARIANT
{
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End TESTTIBO.CPP *************************************************/

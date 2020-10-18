/*
 * I N T E R S E C . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/intersec.hpp"
#include "mathex/point2d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/epsilon.hpp"

//static
uint MexIntersections::lineVsTriangle2d
(
    const MexPoint2d& p, const MexVec2& d,
    const MexPoint2d& t0, const MexPoint2d& t1, const MexPoint2d& t2,
    MATHEX_SCALAR* pL1, MATHEX_SCALAR* pL2
)
{
    PRE( Mathex::abs( d.modulus() - 1.0 ) < MexEpsilon::instance() );
    PRE( pL1 != NULL );
    PRE( pL2 != NULL );

    //Construct a normal vector to the line
    MexVec2 n = d.normal();

    //Compute the side of the line each vertex is on, and its distance along the line
    MATHEX_SCALAR s[3];
    MATHEX_SCALAR h[3];
    Mathex::Side side[3];

    MexVec2 e0(p, t0);
    h[0] = n.dotProduct( e0 );
    s[0] = d.dotProduct( e0 );
    side[0] = Mathex::side( h[0] );

    MexVec2 e1(p, t1);
    h[1] = n.dotProduct( e1 );
    s[1] = d.dotProduct( e1 );
    side[1] = Mathex::side( h[1] );

    MexVec2 e2(p, t2);
    h[2] = n.dotProduct( e2 );
    s[2] = d.dotProduct( e2 );
    side[2] = Mathex::side( h[2] );

    //Now consider each edge and list the intersection distances
    MATHEX_SCALAR l[3];
    uint nIntersections = 0;

    for( size_t i = 0; i != 3; ++i )
    {
        //If this point is on the line, add as an intersection point
        Mathex::Side iSide = side[i];
        ASSERT( iSide != Mathex::STRADDLE, "" );

        if( iSide == Mathex::ONEDGE )
        {
            l[nIntersections++] = s[i];
        }
        else
        {
            //Check for next vertex being on opposite side of line
            size_t j = (i + 1) % 3;
            Mathex::Side jSide = side[j];
            if( jSide != iSide  and  jSide != Mathex::ONEDGE )
            {
                //There is an intersection
                l[nIntersections++] = (s[j]*h[i] - s[i]*h[j]) / (h[i] - h[j]);
            }
        }
    }

    //Return the values in ascending order
    if( nIntersections > 1 )
    {
        if( l[0] > l[1] )
        {
            *pL1 = l[1];
            *pL2 = l[0];
        }
        else
        {
            *pL1 = l[0];
            *pL2 = l[1];
        }
    }
    else if( nIntersections == 1 )
        *pL1 = l[0];

    return nIntersections;
}

/* End INTERSEC.CPP *************************************************/

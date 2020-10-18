/*
 * I N T E R S E C . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexIntersections

    A stateless class that performs a variety of fast intersection tests
    using static methods
*/

#ifndef _MATHEX_INTERSEC_HPP
#define _MATHEX_INTERSEC_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"

//forward refs
class MexPoint2d;
class MexVec2;

class MexIntersections
// Canonical form revoked
{
public:
    //The number of intersections between the infinite line through p with unit
    //direction vector d, and the triangle with points t1, t2, t3.
    //Result is 0, 1 or 2. The signed distances along the line to any intersection
    //points are returned in pL1 and pL2 if found.
    static uint lineVsTriangle2d( const MexPoint2d& p, const MexVec2& d,
                                  const MexPoint2d& t1, const MexPoint2d& t2, const MexPoint2d& t3,
                                  MATHEX_SCALAR* pL1, MATHEX_SCALAR* pL2 );
    //PRE( Mathex::abs( d.modulus() - 1.0 ) < MexEpsilon::instance() );
    //PRE( pL1 != NULL );
    //PRE( pL2 != NULL );

private:
    //stateless
    MexIntersections( void );
    ~MexIntersections( void );
    MexIntersections( const MexIntersections& );
    MexIntersections& operator =( const MexIntersections& );

};


#endif

/* End INTERSEC.HPP *************************************************/

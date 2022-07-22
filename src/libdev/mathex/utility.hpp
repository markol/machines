/*
 * U T I L I T Y . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifndef _MEX_UTILITY_HPP
#define _MEX_UTILITY_HPP

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "mathex/transf3d.hpp"
#include "ctl/vector.hpp"

class MexVec2;
class MexPolygon2d;

//////////////////////////////////////////////////////////////////////

//  This is a class without state and exists purely to namespace the
//  various utility functions
class MexUtility
{
public:

    typedef ctl_vector< MexPoint2d > Point2ds;

    //  Returns true iff the transforms passed in are in a straight line
    //  in the XY plane
    static bool inStraightLineXY( const ctl_vector< MexTransform3d >& transforms );

    //  Returns true iff the three points are in a straight line
    static bool inStraightLine( const MexPoint2d& p1, const MexPoint2d& p2, const MexPoint2d& p3 );
    
    //  Calculate the solution(s) to the quadratic ax^2 + bx + c = 0
    //  Returns the number of solutions - 0, 1 or 2
    //  If there is only one solution it is returned in *pRoot1
    static uint quadraticRealRoots( MATHEX_SCALAR a, MATHEX_SCALAR b, MATHEX_SCALAR c,
      MATHEX_SCALAR* pRoot1, MATHEX_SCALAR* pRoot2 );
    //  POST( implies( result >= 1, a * *pRoot1 * *pRoot1 + b * *pRoot1 + c == 0 ) );
    //  POST( implies( result == 2, a * *pRoot2 * *pRoot2 + b * *pRoot2 + c == 0 ) );

    //You are trying to move from startPoint along motionDirection through distance,
    //but polygon is in the way. Return a new point which is slid through distance along the
    //nearest edge of polygon.
    static void slideAlongPolygon
    (
        const MexPolygon2d& polygon, const MexPoint2d& startPoint, const MexVec2& motionDirection,
        MATHEX_SCALAR distance, Point2ds* pSlidePoints
    );

private:
    MexUtility();
};


#endif  /*  #ifndef _MEX_MATHEX_HPP    */

/* End MATHEX.HPP *****************************************************/

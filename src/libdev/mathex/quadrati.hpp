/*
 * Q U A D R A T I . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MexQuadratic

    Encapsulates solution of the standard quadratic equation
    Ax**2 + Bx + C = 0
*/

#ifndef _MATHEX_QUADRATI_HPP
#define _MATHEX_QUADRATI_HPP

#include "base/base.hpp"

#include "mathex/mathex.hpp"

//No state.
class MexQuadratic
{
public:
    typedef MATHEX_SCALAR Roots[2];

    //The number of solutions to the equation. The solutions are returned in aRoots,
    //in ascending order.
    static int solve( MATHEX_SCALAR a, MATHEX_SCALAR b, MATHEX_SCALAR c, Roots aRoots );


private:
    // Operations deliberately revoked
    MexQuadratic( const MexQuadratic& );
    MexQuadratic& operator =( const MexQuadratic& );
    bool operator ==( const MexQuadratic& );
    MexQuadratic();
    ~MexQuadratic();

};

#endif

/* End QUADRATI.HPP *************************************************/

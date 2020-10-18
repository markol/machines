/*
 * Q U A D R A T I . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <math.h>

#include "mathex/quadrati.hpp"

//static
int MexQuadratic::solve( MATHEX_SCALAR a, MATHEX_SCALAR b, MATHEX_SCALAR c, Roots aRoots )
{
    MATHEX_SCALAR epsilon = MexEpsilon::instance();
    int nRoots;

    //Check for linear
    if( fabs( a ) < epsilon )
    {
        //Check for bad
        if( fabs( b ) < epsilon )
            nRoots = 0;
        else
        {
            nRoots = 1;
            aRoots[0] = -c / b;
        }
    }
    else
    {
        //Valid equation. Compute the square bit.
        MATHEX_SCALAR square = b * b - 4.0 * a * c;
        if( square > epsilon )
        {
            //Normal situation
            MATHEX_SCALAR root = sqrt( square );
            MATHEX_SCALAR root1 =  (root - b) / (2.0 * a);
            MATHEX_SCALAR root2 = -(root + b) / (2.0 * a);

            nRoots = 2;
            if( root1 < root2 )
            {
                aRoots[0] = root1;
                aRoots[1] = root2;
            }
            else
            {
                aRoots[0] = root2;
                aRoots[1] = root1;
            }
        }
        else if( square > -epsilon )
        {
            nRoots = 1;
            aRoots[0] = -b / (2.0 * a);
        }
        else
            nRoots = 0;
    }

    return nRoots;
}

/* End QUADRATI.CPP *************************************************/

/*
 * M A T H E X . C P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved.
 */

//////////////////////////////////////////////////////////////////////

#include <limits.h>

#include "mathex/mathex.hpp"

#ifndef _INLINE
    #include "mathex/mathex.ipp"
#endif

const double Mathex::PI         = 3.14159265358979323846;
const double Mathex::PI_DIV_2   = PI / 2.0;
const double Mathex::SQRT_2     = sqrt( 2.0 );
const double Mathex::LOG10_2    = log10( 2.0 );

//////////////////////////////////////////////////////////////////////

bool isintegerpowerof2( size_t n )
{
	unsigned ones = 0;
	for( size_t i=1; i; i <<= 1 )
		if( n & i and ( ++ones > 1 ) )
			return  false;

	return ones == 1;
}

//////////////////////////////////////////////////////////////////////

bool isinteger( double x )
{
	double abs_x = abs( x );
	return  abs_x - floor( abs_x ) < MexEpsilon::instance();
}

//////////////////////////////////////////////////////////////////////

size_t rmo( size_t n )
{
    size_t i=1, j=0;
    for( ; i; i <<= 1, --j )
        if( n & i )
            return  j;

    return  NPOS;
}

///////////////////////////////////

size_t lmo( size_t n )
{
    size_t j = bitsizeof( n ) - 1;
    size_t msbit = 1 << j;
    for( size_t i=msbit; i; i >>= 1, --j )
        if( n & i )
            return  j;

    return  NPOS;
}

size_t log2( size_t n )
{
    return  rmo( n );
}

//////////////////////////////////////////////////////////////////////
//static
Mathex::Side Mathex::side( MATHEX_SCALAR h )
{
	MATHEX_SCALAR epsilon = MexEpsilon::instance();
	Side result;

	if( h > epsilon ) result = VANILLA;
	else if( h < -epsilon ) result = CHOCOLATE;
	else result = ONEDGE;

	return result;
}
//////////////////////////////////////////////////////////////////////
/* End MATHEX.CPP *****************************************************/

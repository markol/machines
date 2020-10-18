/*
 * M A T H E X . I P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

#include <stdlib.h>

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

//////////////////////////////////////////////////////////////////////////

_CODE_INLINE
double round( double x )
{
	return x > 0.0 ? floor( x + 0.5 ) : ceil( x - 0.5 );
}

//////////////////////////////////////////////////////////////////////////

 _CODE_INLINE
int Mathex::abs( int x )
{
	return  x < 0 ? -x : x;
}

_CODE_INLINE
long Mathex::abs( long x )
{
	return labs( x );
}

_CODE_INLINE
float Mathex::abs( float x )
{
	return fabs( x );
}

_CODE_INLINE
double Mathex::abs( double x )
{
	return fabs( x );
}

_CODE_INLINE
long double
Mathex::abs( long double x )
{
	return  fabs( (double)x );
}

// According to the Watcom manual, a Nan is defined as, "When the exponent
// field is all 1 bits and the significand field is non-zero then the
// quantity is a special value called a NAN (Not-A-Number)."  This is the
// definition for the IEEE format which should be used widely.  Of course
// this will fail beautifully if the floating point format is different.
// Watcom defines this macro when the "fpi" floating-point option is enabled.
// There are other Watcom options which don't use IEEE floating-point.  When
// porting to another compiler, this condition will probably have to be modified.
//#ifndef __FPI__
//#error The Watcom IEEE floating point option is not in use.
//#else
_CODE_INLINE
bool Mathex::isNan(float floaty)
{
	ASSERT_COMPILE_TIME(sizeof(float) == sizeof(uint));

	const uint exponentMask    = 0x7f800000;		// bits 30-23
	const uint significandMask = 0x007fffff;		// bits 22-0
	const uint& cruftyCast     = *((const uint*) &floaty);

	return (cruftyCast & exponentMask) == exponentMask && (cruftyCast & significandMask) != 0;
}

_CODE_INLINE
bool Mathex::isNan(double dubby)
{
	ASSERT_COMPILE_TIME(sizeof(double) == 2*sizeof(uint));

	const uint exponentMaskHi    = 0x7ff00000;		// bits 62-52
	const uint significandMaskHi = 0x000fffff;		// bits 51-32
	const uint significandMaskLo = 0xffffffff;		// bits 31-0

	const char* ptr      = (char*) &dubby;
	const uint& cruftyLo = *((const uint*) ptr);
	const uint& cruftyHi = *((const uint*) (ptr+4));

	const bool nonZeroSig = (cruftyHi & significandMaskHi) != 0 ||
							(cruftyLo & significandMaskLo) != 0;

	return (cruftyHi & exponentMaskHi) == exponentMaskHi && nonZeroSig;
}
//#endif

//////////////////////////////////////////////////////////////////////

_CODE_INLINE float mod( float a, float b ) { return fmod( a, b ); }
_CODE_INLINE double mod( double a, double b ) { return fmod( a, b ); }
_CODE_INLINE long double mod( long double a, long double b ) { return fmod( (double)a, (double)b ); }

//////////////////////////////////////////////////////////////////////
_CODE_INLINE
//static
Mathex::Side Mathex::side( Side a, Side b )
{
	return ( a == b ? a : STRADDLE );
}
//////////////////////////////////////////////////////////////////////
/* End MATHEX.IPP *****************************************************/

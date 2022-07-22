/*
 * M A T H E X . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifndef _MEX_MATHEX_HPP
#define _MEX_MATHEX_HPP

#include "base/base.hpp"


#ifndef _MATHEX_CLIENT_DEFINED_SCALAR
    #ifndef _MATHEX_USE_DOUBLE
        #ifndef _MATHEX_USE_FLOAT
            #ifndef _MATHEX_USE_FIXED
                //  Default to double
                #define _MATHEX_USE_DOUBLE
            #endif
        #endif
    #endif

    #ifdef _MATHEX_USE_DOUBLE
        typedef double MATHEX_SCALAR;
    #endif

    #ifdef _MATHEX_USE_FLOAT
        typedef float MATHEX_SCALAR;
    #endif

    #ifdef _MATHEX_USE_FIXED
        typedef MexFix16Pt16 MATHEX_SCALAR;
    #endif
#endif

#include "mathex/epsilon.hpp"

//////////////////////////////////////////////////////////////////////

extern "C"
{

#include <math.h>
#include <stdlib.h>

}

//////////////////////////////////////////////////////////////////////

class Mathex
{
    #ifdef PI
        #undef PI
    #endif

public:

	static int			abs( int );
	static long			abs( long );
	static float		abs( float );
	static double		abs( double );
	static long double	abs( long double );

	static bool isNan(double);
	static bool isNan(float);

    static const double PI;
    static const double PI_DIV_2;
    static const double SQRT_2;
    static const double LOG10_2;

	// Used by MexHalfPlane2d for determining which side of
	// a line points, polys etc. lie on.
	// What names do you give to the two sides of a line,
	// maybe left/right or plus/minus would be better?
	enum Side { VANILLA, CHOCOLATE, ONEDGE, STRADDLE };

    //Determines Side for a point a distance h from a line:
    // h > MexEpsilon ==> VANILLA
    // h < -MexEpsilon ==> CHOCOLATE
    // -MexEpsilon <= h <= MexEpsilon ==> ONEDGE
    static Side side( MATHEX_SCALAR h );

    //Determines Side for line joining 2 points with Side values a and b
    static Side side( Side a, Side b );

    //Used to distinguish touching as is/is not an intersection in various
    //methods
    enum TouchingRule { TOUCH_ISNT_INTERSECT, TOUCH_IS_INTERSECT };
};

//////////////////////////////////////////////////////////////////////

double round( double );
bool isinteger( double );

//////////////////////////////////////////////////////////////////////

//template < class T > T sqr( T );
//template < class T > T cube( T );
template < class T > T round( T );

template < class T > bool isodd( T );
template < class T > bool iseven( T );

// float mod( float a, float b );
// double mod( double a, double b );
// long double mod( long double a, long double b );
template < class T > T mod( T a, T b );

// template < class T > bool divides( T divisor, T dividend );

template < class T > bool isprime( T );
template < class T > T prime_ceil( T );

template < class T > bool isintegerpowerof2( T );
bool isintegerpowerof2( size_t );

template < class T > T integerpowerof2ceil( T );

//////////////////////////////////////////////////////////////////////

// returns the index ( 0..bitsizeof( size_t ) - 1 )
// of leftmost set bit, or NPOS iff n is 0.
size_t lmo( size_t n );

// returns the index ( 0..bitsizeof( size_t ) - 1 )
// of rightmost set bit, or NPOS iff n is 0.
size_t rmo( size_t n );

//////////////////////////////////////////////////////////////////////

size_t log2( size_t n );

//////////////////////////////////////////////////////////////////////

// functions to check if a value lies within an interval
// the 'O' qualifier refers to an open interval ( <=, >= )
// the 'C' qualifier refers to a closed interval ( <, > )

template < class T >
bool inIntervalOO ( const T& t , const T& lBound , const T& uBound );

template < class T >
bool inIntervalCC( const T& t , const T& lBound , const T& uBound );

template < class T >
bool inIntervalOC( const T& t , const T& lBound , const T& uBound );

template < class T >
bool inIntervalCO( const T& t , const T& lBound , const T& uBound );

//////////////////////////////////////////////////////////////////////

//  As inputValue varies between inputLimit1 and inputLimit2 the
//  result varies linearly between outputLimit1 and outputLimit2
template< class T1, class T2 >
T1 mexInterpolate( const T1& inputValue,
  const T1& inputLimit1, const T1& inputLimit2,
  const T2& outputLimit1, const T2& outputLimit2 );
// PRE( inputValue in range inputLimit1 -> inputLimit2 );
// PRE( implies( inputLimit1 == inputLimit2, outputLimit1 == outputLimit2 and inputValue == inputLimit1 ) );
// POST( result in range outputLimit1 -> outputLimit2 );

//  Clamp a value to lie between two limits
template< class T1 >
T1 mexClamp( const T1& inputValue,
  const T1& lowerLimit, const T1& upperLimit );
// PRE( lowerLimit <= upperLimit );
// POST( lowerLimit <= result and result <= upperLimit );

#include "base/persist.hpp"

template< class SCALAR >
class MexXCoord
{
public:
    MexXCoord( SCALAR s ) : data_( s ) {}

    operator SCALAR& () { return data_; }
	const SCALAR& xCoord() const { return data_; }
	bool operator == (const MexXCoord< SCALAR >& x) const {return data_ == x.data_;}

    //Persistence
    PER_MEMBER_PERSISTENT( MexXCoord );
    template<class T>
    friend ostream &operator<<( ostream & ostr, const MexXCoord<T>& t );
    template<class T>
    friend void perWrite( PerOstream& stream, const MexXCoord<T>& t );
    template<class T>
    friend void perRead( PerIstream& stream, MexXCoord<T>& t );

private:
//	MexXCoord( PerConstructor );
    SCALAR data_;
};

PER_DECLARE_PERSISTENT_T1( MexXCoord );

template< class SCALAR >
class MexYCoord
{
public:
    MexYCoord( SCALAR s ) : data_( s ) {}
    operator SCALAR& () { return data_; }
	const SCALAR& yCoord() const { return data_; }
	bool operator == (const MexYCoord< SCALAR >& y) const {return data_ == y.data_;}

    //Persistence
    PER_MEMBER_PERSISTENT( MexYCoord );
    template<class T>
    friend ostream &operator<<( ostream & ostr, const MexYCoord<T>& t );
    template<class T>
    friend void perWrite( PerOstream& stream, const MexYCoord<T>& t );
    template<class T>
    friend void perRead( PerIstream& stream, MexYCoord<T>& t );
private:
//    MexYCoord( PerConstructor );

    SCALAR data_;
};

PER_DECLARE_PERSISTENT_T1( MexYCoord );
//////////////////////////////////////////////////////////////////////

#ifdef _INLINE
    #include "mathex/mathex.ipp"
    #include "mathex/mathex.itf"
#endif

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "mathex/mathex.ctf"
//#endif

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
	#include "mathex/mathex.ctp"
//#endif

//////////////////////////////////////////////////////////////////////

#endif  /*  #ifndef _MEX_MATHEX_HPP    */

/* End MATHEX.HPP *****************************************************/

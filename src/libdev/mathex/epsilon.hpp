/*
 * E P S I L O N . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved.
 */

#ifndef _MEX_EPSILON_HPP
#define _MEX_EPSILON_HPP


#include "mathex/mathex.hpp"

class MexEpsilon
// singleton
{
public:

	static MATHEX_SCALAR instance();
	static MATHEX_SCALAR sqrInstance();

	MATHEX_SCALAR set( MATHEX_SCALAR newValue );

    //  If the value to be clamped is within epsilon of the value 
    //  to clamp to, return the value to clamp to, otherwise return
    //  the original value.
    //  E.g. MexEpsilon::clampTo( value, 0.0 ) will return zero if
    //  value is within epsilon of zero and value otherwise.
    static MATHEX_SCALAR clampTo(
      MATHEX_SCALAR valueToBeClamped,
      MATHEX_SCALAR valueToClampTo );

    //  Returns true iff the two values are within epsilon of each other.
    static bool isWithinEpsilonOf(
      MATHEX_SCALAR value1,
      MATHEX_SCALAR value2 );

private:
    
    MexEpsilon( MATHEX_SCALAR defaultValue = 0.0001 );

	// revoked...
    MexEpsilon( const MexEpsilon& );
    const MexEpsilon& operator =( const MexEpsilon& );
    bool operator ==( const MexEpsilon& ) const;

    static MATHEX_SCALAR& epsilon_();
};


#ifdef _INLINE
    #include "mathex/epsilon.ipp"
#endif

#endif

/* End EPSILON.HPP *****************************************************/

/*
 * E P S I L O N . I P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved.
 */

//////////////////////////////////////////////////////////////////////

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include <math.h>

_CODE_INLINE

MATHEX_SCALAR
MexEpsilon::instance()
{
	static MexEpsilon instance_;
	return instance_.epsilon_();
}


_CODE_INLINE

MATHEX_SCALAR
MexEpsilon::sqrInstance()
{
	return MexEpsilon::instance() * MexEpsilon::instance();
}


_CODE_INLINE

MATHEX_SCALAR&
MexEpsilon::epsilon_()
{
	static MATHEX_SCALAR e_;
	return e_;
}


//////////////////////////////////////////////////////////////////////

_CODE_INLINE
MexEpsilon::MexEpsilon( MATHEX_SCALAR defaultValue )
{
	epsilon_() = defaultValue;
}

//////////////////////////////////////////////////////////////////////////

MATHEX_SCALAR
_CODE_INLINE
MexEpsilon::set( MATHEX_SCALAR newValue )
{
	MATHEX_SCALAR result = epsilon_();
	epsilon_() = newValue;
	return result;
}

//////////////////////////////////////////////////////////////////////////

// static
_CODE_INLINE
MATHEX_SCALAR MexEpsilon::clampTo(
    MATHEX_SCALAR valueToBeClamped,
    MATHEX_SCALAR valueToClampTo )
{
    MATHEX_SCALAR result = valueToBeClamped;

    if( isWithinEpsilonOf( valueToBeClamped, valueToClampTo ) )
        result = valueToClampTo;

    return result;
}

//////////////////////////////////////////////////////////////////////////

// static
_CODE_INLINE
bool MexEpsilon::isWithinEpsilonOf(
    MATHEX_SCALAR value1,
    MATHEX_SCALAR value2 )
{
    PRE_INFO( value1 );
    PRE_INFO( value2 );
    PRE_INFO( value1 - value2 );

    return fabs( value1 - value2 ) < instance();
}

/* End EPSILON.IPP *******************************************************/

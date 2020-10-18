/*
 * V E C 3 . I T P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#include "mathex/vec2.hpp"

/////////////////////////////////////////////////////////

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif


_CODE_INLINE
MexVec3::~MexVec3()
{
    /* intentionally Empty */
}

//////////////////////////////////////////////////////////////////////


_CODE_INLINE
const MexVec3& MexVec3::zeroVector()
{
    static MexVec3 zero_ = MexVec3( 0, 0, 0 );
    return zero_;        
}

//////////////////////////////////////////////////////////////////////


_CODE_INLINE
MATHEX_SCALAR MexVec3::x() const
{
    return vec_[ X ];
}

//////////////////////////////////////////////////////////////////////


_CODE_INLINE
MATHEX_SCALAR MexVec3::y() const
{
    return  vec_[ Y ];
}

//////////////////////////////////////////////////////////////////////


_CODE_INLINE
MATHEX_SCALAR MexVec3::z() const
{
    return  vec_[ Z ];
}


_CODE_INLINE
void MexVec3::x( MATHEX_SCALAR newX )
{
	vec_[ X ] = newX;
}


_CODE_INLINE
void MexVec3::y( MATHEX_SCALAR newY )
{
	vec_[ Y ] = newY;
}


_CODE_INLINE
void MexVec3::z( MATHEX_SCALAR newZ )
{
	vec_[ Z ] = newZ;
}

/////////////////////////////////////////////////////////////


_CODE_INLINE
MATHEX_SCALAR MexVec3::operator []( size_t i ) const
{
    PRE( i < 3 );
	return ( i == 0 ) ? vec_[ X ] : ( i == 1 ) ? vec_[ Y ] : vec_[ Z ];
}

/////////////////////////////////////////////////////////////


_CODE_INLINE
MATHEX_SCALAR& MexVec3::operator []( size_t i )
{
    PRE( i < 3 );
	return ( i == 0 ) ? vec_[ X ] : ( i == 1 ) ? vec_[ Y ] : vec_[ Z ];
}

/* //////////////////////////////////////////////////////////////// */

_CODE_INLINE
MexVec3::MexVec3( const MexVec2& v2 )
{
    vec_[ X ] = v2.x();
    vec_[ Y ] = v2.y();
    vec_[ Z ] = 0;

	POST( *this == MexVec3( v2.x(), v2.y(), 0 ) );
}

/* //////////////////////////////////////////////////////////////// */

/* End VEC3.IPP *****************************************************/
/*
 * V E C 2 . I P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#include "mathex/vec3.hpp"

/////////////////////////////////////////////////////////

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include "mathex/point2d.hpp"

_CODE_INLINE
MexVec2::MexVec2( MATHEX_SCALAR x, MATHEX_SCALAR y ):
	x_(x),
	y_(y)
{
}

_CODE_INLINE
MexVec2::MexVec2( const MexVec2& v):
	x_(v.x_),
	y_(v.y_)

{
}

_CODE_INLINE
MexVec2::MexVec2( const MexPoint2d& rhs )
:	x_(rhs.x()),
	y_(rhs.y())
{
}


_CODE_INLINE
MexVec2::~MexVec2()
{
    /* intentionally left blank */
}

_CODE_INLINE
MATHEX_SCALAR MexVec2::x( void ) const
{
    return x_;
}

_CODE_INLINE
MATHEX_SCALAR MexVec2::y( void ) const
{
    return y_;
}

_CODE_INLINE
void MexVec2::setVector( MATHEX_SCALAR x, MATHEX_SCALAR y )
{
    x_ = x;
    y_ = y;
}

//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR MexVec2::dotProduct( const MexVec2& rhs ) const
{
	return x_ * rhs.x_  +  y_ * rhs.y_;
}
//////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
MexVec2 MexVec2::normal( void ) const
{
	return MexVec2( y_, -x_ );
}

/* //////////////////////////////////////////////////////////////// */

_CODE_INLINE
MexVec2::MexVec2( const MexVec3& rhs )
: x_( rhs.x() ),
  y_( rhs.y() )
{
	// Intentionally Empty
}

/* //////////////////////////////////////////////////////////////// */

_CODE_INLINE
MATHEX_SCALAR MexVec2::modulus() const
{
    return sqrt( x_ * x_ + y_ * y_ );
}

_CODE_INLINE
MATHEX_SCALAR MexVec2::squareModulus() const
{
    return x_ * x_ + y_ * y_;
}

/* End VEC2.IPP *****************************************************/
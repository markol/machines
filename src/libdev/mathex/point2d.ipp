/*
 * P O I N T 2 D . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "mathex/point3d.hpp"

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include "mathex/vec2.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MexPoint2d::MexPoint2d( MATHEX_SCALAR x, MATHEX_SCALAR y )
{
	x_ = x;
	y_ = y;
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MexPoint2d::MexPoint2d( const MexPoint2d& rhs)
{
	x_ = rhs.x_;
	y_ = rhs.y_;
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MexPoint2d::MexPoint2d( const MexVec2& rhs)
{
	x_ = rhs.x();
	y_ = rhs.y();
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
MexPoint2d::~MexPoint2d()
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MexPoint2d& MexPoint2d::operator =( const MexPoint2d& rhs )
{
	x_ = rhs.x_;
	y_ = rhs.y_;
    TEST_INVARIANT;

	return *this;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
bool MexPoint2d::operator ==( const MexPoint2d& rhs ) const
{
    TEST_INVARIANT;

    return Mathex::abs( x() - rhs.x() ) < MexEpsilon::instance()
    	   and Mathex::abs( y() - rhs.y() ) < MexEpsilon::instance();
}

_CODE_INLINE
bool MexPoint2d::operator !=( const MexPoint2d& rhs ) const
{
    return !(*this == rhs);
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR MexPoint2d::x( void ) const
{
    TEST_INVARIANT;
	return x_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR MexPoint2d::y( void ) const
{
    TEST_INVARIANT;
	return y_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void MexPoint2d::x( MATHEX_SCALAR xNew )
{
	x_ = xNew;
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void MexPoint2d::y( MATHEX_SCALAR yNew )
{
	y_ = yNew;
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void MexPoint2d::operator +=( const MexVec2& rhs )
{
	x_ += rhs.x();
	y_ += rhs.y();
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void MexPoint2d::operator -=( const MexVec2& rhs )
{
	x_ -= rhs.x();
	y_ -= rhs.y();
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR MexPoint2d::operator []( size_t i ) const
{
	PRE( i == 0  or  i == 1 );
    TEST_INVARIANT;

	return ( i == 0 ? x_ : y_ );
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR MexPoint2d::euclidianDistance( const MexPoint2d& rhs ) const
{
	return sqrt( sqrEuclidianDistance( rhs ) );
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR MexPoint2d::sqrEuclidianDistance( const MexPoint2d& rhs ) const
{
	MATHEX_SCALAR x = rhs.x_ - x_;
	MATHEX_SCALAR y = rhs.y_ - y_;
	return x*x + y*y;
}

/* //////////////////////////////////////////////////////////////// */

_CODE_INLINE
MexPoint2d::MexPoint2d( const MexPoint3d& rhs )
: x_( rhs.x() ),
  y_( rhs.y() )
{
	// Intentionally Empty
}

/* //////////////////////////////////////////////////////////////// */

/* End POINT2D.IPP **************************************************/

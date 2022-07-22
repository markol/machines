/*
 * P O I N T 3 D . I P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

#include "mathex/point2d.hpp"

///////////////////////////////////

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include "base/base.hpp"

///////////////////////////////////

_CODE_INLINE
MATHEX_SCALAR MexPoint3d::x( void ) const
{
    return  point_[ X ];
}

//////////////////////////////////////////////////////////////////////

_CODE_INLINE
MATHEX_SCALAR MexPoint3d::y( void ) const
{
    return  point_[ Y ];
}

//////////////////////////////////////////////////////////////////////


_CODE_INLINE
MATHEX_SCALAR MexPoint3d::z( void ) const
{
    return  point_[ Z ];
}

/////////////////////////////////////////////////////////////


_CODE_INLINE
void MexPoint3d::x( MATHEX_SCALAR newX )
{
    point_[ X ] = newX;
}

//////////////////////////////////////////////////////////////////////


_CODE_INLINE
void MexPoint3d::y( MATHEX_SCALAR newY )
{
    point_[ Y ] = newY;
}

//////////////////////////////////////////////////////////////////////


_CODE_INLINE
void MexPoint3d::z( MATHEX_SCALAR newZ )
{
    point_[ Z ] = newZ;
}

//////////////////////////////////////////////////////////////////////


_CODE_INLINE
MATHEX_SCALAR MexPoint3d::operator []( size_t i ) const
{
    PRE( i < 3 );
	return ( i == 0 ) ? point_[ X ] : ( i == 1 ) ? point_[ Y ] : point_[ Z ];
}

/////////////////////////////////////////////////////////////

_CODE_INLINE
MATHEX_SCALAR& MexPoint3d::operator []( size_t i )
{
    PRE( i < 3 );
	return ( i == 0 ) ? point_[ X ] : ( i == 1 ) ? point_[ Y ] : point_[ Z ];
}

///////////////////////////////////

_CODE_INLINE
bool MexPoint3d::contains( const MexPlane3d& ) const
{
    // A point can never contain a plane
   
    return false;
}

/* //////////////////////////////////////////////////////////////// */

_CODE_INLINE
MexPoint3d::MexPoint3d( const MexPoint2d& v2 )
{
    point_[ X ] = v2.x();
    point_[ Y ] = v2.y();
    point_[ Z ] = 0;
    
	POST( *this == MexPoint3d( v2.x(), v2.y(), 0 ) );
}

/* //////////////////////////////////////////////////////////////// */

/* End POINT3D.IPP *****************************************************/

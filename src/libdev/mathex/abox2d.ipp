/* 
 * B O X 2 . I P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifdef _INLINE
    #define	_CODE_INLINE	inline
#else
    #define	_CODE_INLINE
#endif

_CODE_INLINE
void MexAlignedBox2d::minCorner( const MexPoint2d& newVal )
{
    vmin_ = newVal;

    POST( vmin_ == newVal );
}

_CODE_INLINE
void MexAlignedBox2d::maxCorner( const MexPoint2d& newVal )
{
    vmax_ = newVal;

    POST( vmax_ == newVal );
}

_CODE_INLINE
MATHEX_SCALAR MexAlignedBox2d::area( ) const
{
    return (vmax_.x() - vmin_.x()) * (vmax_.y() - vmin_.y());
}

_CODE_INLINE
void MexAlignedBox2d::xMin( MATHEX_SCALAR xNew )
{
    PRE( xNew <= vmax_.x() )
	vmin_.x( xNew );
}

_CODE_INLINE
void MexAlignedBox2d::xMax( MATHEX_SCALAR xNew )
{
    PRE( xNew >= vmin_.x() )
	vmax_.x( xNew );
}

_CODE_INLINE
void MexAlignedBox2d::yMin( MATHEX_SCALAR yNew )
{
    PRE( yNew <= vmax_.y() )
	vmin_.y( yNew );
}

_CODE_INLINE
void MexAlignedBox2d::yMax( MATHEX_SCALAR yNew )
{
    PRE( yNew >= vmin_.y() )
	vmax_.y( yNew );
}

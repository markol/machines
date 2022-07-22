/*
 * A B O X 3 D . I P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#include "mathex/point3d.hpp"
#include "mathex/t3dkey.hpp"

//////////////////////////////////////////////////////////////////////

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

//////////////////////////////////////////////////////////////////////


_CODE_INLINE
MexAlignedBox3d::MexAlignedBox3d()
:   isEmpty_( true ),
    spinSafe_( false )
{
    TEST_INVARIANT;
}

//////////////////////////////////////////////////////////////////////



_CODE_INLINE
bool MexAlignedBox3d::operator ==( const MexAlignedBox3d& b ) const
{
    return (minCorner_ == b.minCorner_) and
           (maxCorner_ == b.maxCorner_);
}

_CODE_INLINE
bool MexAlignedBox3d::operator !=( const MexAlignedBox3d& b ) const
{
    return !(*this == b);
}

_CODE_INLINE
const MexPoint3d& MexAlignedBox3d::minCorner() const
{
    return minCorner_;
}

_CODE_INLINE
const MexPoint3d& MexAlignedBox3d::maxCorner() const
{
    return maxCorner_;
}

_CODE_INLINE
void MexAlignedBox3d::minCorner( const MexPoint3d& p )
{
    PRE( p.x() <= maxCorner_.x() );
    PRE( p.y() <= maxCorner_.y() );
    PRE( p.z() <= maxCorner_.z() );

    minCorner_ = p;
}

_CODE_INLINE
void MexAlignedBox3d::maxCorner( const MexPoint3d& q )
{
    PRE( q.x() >= minCorner_.x() );
    PRE( q.y() >= minCorner_.y() );
    PRE( q.z() >= minCorner_.z() );

    maxCorner_ = q;
}

_CODE_INLINE
void MexAlignedBox3d::corners( const MexPoint3d& p, const MexPoint3d& q )
{
    PRE( p.x() <= q.x() );
    PRE( p.y() <= q.y() );
    PRE( p.z() <= q.z() );

    minCorner_ = p;
    maxCorner_ = q;
}

_CODE_INLINE
MATHEX_SCALAR MexAlignedBox3d::xLength() const
{
    return maxCorner_.x() - minCorner_.x();
}

_CODE_INLINE
MATHEX_SCALAR MexAlignedBox3d::yLength() const
{
    return maxCorner_.y() - minCorner_.y();
}

_CODE_INLINE
MATHEX_SCALAR MexAlignedBox3d::zLength() const
{
    return maxCorner_.z() - minCorner_.z();
}

_CODE_INLINE
MATHEX_SCALAR MexAlignedBox3d::volume() const
{
	return xLength() * yLength() * zLength();
}
/* End ABOX3D.IPP ***************************************************/

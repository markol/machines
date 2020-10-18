/*
 * P O I N T 2 D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <iostream>
#include <cmath>

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "mathex/cpoint2d.hpp"

#ifndef _INLINE
    #include "mathex/point2d.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

void MexPoint2d::CLASS_INVARIANT
{
	INVARIANT(this != NULL);
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MexPoint2d& t )
{

    o << "  (" << t.x_ << "," << t.y_ << ")";
    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

MexPoint2d::MexPoint2d( const MexPoint2d& p, const MexPoint2d& q, MATHEX_SCALAR f )
{
    MATHEX_SCALAR g = 1.0 - f;
    x_ = g * p.x_ + f * q.x_;
    y_ = g * p.y_ + f * q.y_;
}

//////////////////////////////////////////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT( MexPoint2d );

void perWrite( PerOstream& str, const MexPoint2d& t )
{
    str << t.x_;
    str << t.y_;
}

void perRead( PerIstream& str, MexPoint2d& t )
{
    str >> t.x_;
    str >> t.y_;
}

bool operator <( const MexPoint2d& a, const MexPoint2d& b )
{
    if( a.x() < b.x() )
        return true;
    else if( a.x() > b.x() )
        return false;
    else if( a.y() < b.y() )
        return true;
    else if( a.y() > b.y() )
        return false;

    return false;
}

bool operator >( const MexPoint2d& a, const MexPoint2d& b )
{
    return (b < a);
}

MexPoint2d::MexPoint2d( const MexCompressedPoint2d& p )
{
    x_ = p.point_[0];
    y_ = p.point_[1];
}
/* End POINT2D.CPP **************************************************/

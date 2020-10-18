/*
 * C P O I N T 3 D . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/cpoint2d.hpp"
#include "mathex/point2d.hpp"

MexCompressedPoint2d::MexCompressedPoint2d()
{
	point_[0] = 0;
	point_[1] = 0;
    TEST_INVARIANT;
}

MexCompressedPoint2d::MexCompressedPoint2d( const MexPoint2d& point )
{
	point_[0] = point.x();
	point_[1] = point.y();
}

MexCompressedPoint2d::~MexCompressedPoint2d()
{
    TEST_INVARIANT;

}

void MexCompressedPoint2d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MexCompressedPoint2d& t )
{

	o << " MexCPoint2d: ( " << t.point_[0] << ", " << t.point_[1] << " )\n";

    return o;
}

/* End CPOINT2d.CPP *************************************************/

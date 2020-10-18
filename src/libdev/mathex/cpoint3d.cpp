/*
 * C P O I N T 3 D . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/cpoint3d.hpp"
#include "mathex/point3d.hpp"

MexCompressedPoint3d::MexCompressedPoint3d()
{
	point_[0] = 0;
	point_[1] = 0;
	point_[2] = 0;
    TEST_INVARIANT;
}

MexCompressedPoint3d::MexCompressedPoint3d( const MexPoint3d& point )
{
	point_[0] = point.x();
	point_[1] = point.y();
	point_[2] = point.z();
}

MexCompressedPoint3d::~MexCompressedPoint3d()
{
    TEST_INVARIANT;

}

void MexCompressedPoint3d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MexCompressedPoint3d& t )
{

	o << " MexCPoint3d: ( " << t.point_[0] << ", " << t.point_[1] << ", " << t.point_[2] << " )\n";

    return o;
}

/* End CPOINT3D.CPP *************************************************/

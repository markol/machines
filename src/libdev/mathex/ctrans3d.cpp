/*
 * C T R A N S 3 D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/ctrans3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"

MexCompressedTransform3d::MexCompressedTransform3d()
{
    TEST_INVARIANT;
}

MexCompressedTransform3d::MexCompressedTransform3d( const MexTransform3d& trans )
{
	position_ = trans.position();
	xBasis_ = trans.xBasis();
	yBasis_ = trans.yBasis();

    TEST_INVARIANT;
}

MexCompressedTransform3d::~MexCompressedTransform3d()
{
    TEST_INVARIANT;

}

void MexCompressedTransform3d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MexCompressedTransform3d& t )
{

    o << "MexCompressedTransform3d " << (void*)&t << " start" << std::endl;
	o << " position " << t.position_ << std::endl;
	o << " xBasis " << t.xBasis_ << std::endl;
	o << " yBasis " << t.yBasis_ << std::endl;
    o << "MexCompressedTransform3d " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CTRANS3D.CPP *************************************************/

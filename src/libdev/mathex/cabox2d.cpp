/*
 * C P O I N T 3 D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/cabox2d.hpp"
#include "mathex/abox2d.hpp"

MexCompressedAlignedBox2d::MexCompressedAlignedBox2d()
{
    TEST_INVARIANT;
}

MexCompressedAlignedBox2d::MexCompressedAlignedBox2d( const MexAlignedBox2d& abox ):
    vmin_(abox.vmin_),
    vmax_(abox.vmax_),
    isEmpty_(abox.isEmpty_)
{
}

MexCompressedAlignedBox2d::~MexCompressedAlignedBox2d()
{
    TEST_INVARIANT;

}

void MexCompressedAlignedBox2d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MexCompressedAlignedBox2d& t )
{

	o << " MexCAlignedBox2d: ( " << t.vmin_ << ", " << t.vmax_ << ": " << t.isEmpty_ <<" )\n";

    return o;
}

/* End CPOINT2d.CPP *************************************************/

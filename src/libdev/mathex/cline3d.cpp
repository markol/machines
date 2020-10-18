/*
 * C L I N E 3 D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/cline3d.hpp"
#include "mathex/line3d.hpp"

MexCompressedLine3d::MexCompressedLine3d()
{

    TEST_INVARIANT;
}

MexCompressedLine3d::MexCompressedLine3d(const MexLine3d& line )
: end1_( line.end1() ),
  end2_( line.end2() )
{
}

MexCompressedLine3d::~MexCompressedLine3d()
{
    TEST_INVARIANT;

}

void MexCompressedLine3d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MexCompressedLine3d& t )
{

    o << "MexCompressedLine3d " << t.end1_ << " - " << t.end2_ << std::endl;

    return o;
}

/* End CLINE3D.CPP **************************************************/

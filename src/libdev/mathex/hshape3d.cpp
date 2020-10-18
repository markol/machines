/*
 * H S H A P E 3 D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/hshape3d.hpp"

MexHierShape3d::MexHierShape3d()
{

    TEST_INVARIANT;
}

MexHierShape3d::~MexHierShape3d()
{
    TEST_INVARIANT;

}

void MexHierShape3d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MexHierShape3d& t )
{
   t.streamout(o);

    return o;
}

void MexHierShape3d::streamout(ostream &o) const
{
    o << "MexHierShape3d " << (void*) this << " start" << std::endl;
    o << "MexHierShape3d " << (void*) this << " end" << std::endl;
}

/* End SHAPE3D.CPP ******************************************************/

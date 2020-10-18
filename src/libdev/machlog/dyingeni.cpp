/*
 * D Y I N G E N I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/dyingeni.hpp"

MachLogDyingEntityEventImpl::MachLogDyingEntityEventImpl()
{

    TEST_INVARIANT;
}

MachLogDyingEntityEventImpl::~MachLogDyingEntityEventImpl()
{
    TEST_INVARIANT;

}

void MachLogDyingEntityEventImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogDyingEntityEventImpl& t )
{

    o << "MachLogDyingEntityEventImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogDyingEntityEventImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End DYINGENI.CPP *************************************************/

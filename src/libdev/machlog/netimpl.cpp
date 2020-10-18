/*
 * N E T I M P L . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "machlog/internal/netimpl.hpp"

MachLogNetworkDataImpl::MachLogNetworkDataImpl()
{

    TEST_INVARIANT;
}

MachLogNetworkDataImpl::~MachLogNetworkDataImpl()
{
    TEST_INVARIANT;

}

void MachLogNetworkDataImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogNetworkDataImpl& t )
{

    o << "MachLogNetworkDataImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogNetworkDataImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End NETIMPL.CPP **************************************************/

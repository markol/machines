/*
 * N O D E I . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "network/internal/nodei.hpp"

NetNodeImpl::NetNodeImpl()
:	pingString_("Ping")
{

    TEST_INVARIANT;
}

NetNodeImpl::~NetNodeImpl()
{
    TEST_INVARIANT;

}

void NetNodeImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const NetNodeImpl& t )
{

    o << "NetNodeImpl " << (void*)&t << " start" << std::endl;
    o << "NetNodeImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End NODEI.CPP ****************************************************/

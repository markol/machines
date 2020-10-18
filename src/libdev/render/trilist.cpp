/*
 * T R I L I S T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "render/trilist.hpp"

RenTriangleList::RenTriangleList()
{

    TEST_INVARIANT;
}

RenTriangleList::~RenTriangleList()
{
    TEST_INVARIANT;

}

void RenTriangleList::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const RenTriangleList& t )
{

    o << "RenTriangleList " << (void*)&t << " start" << std::endl;
    o << "RenTriangleList " << (void*)&t << " end" << std::endl;

    return o;
}

/* End TRILIST.CPP **************************************************/

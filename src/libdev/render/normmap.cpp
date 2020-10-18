/*
 * N O R M M A P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "render/normmap.hpp"

RenNormalMap::RenNormalMap()
{

    TEST_INVARIANT;
}

RenNormalMap::~RenNormalMap()
{
    TEST_INVARIANT;

}

void RenNormalMap::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RenNormalMap& t )
{

    o << "RenNormalMap " << (void*)&t << " start" << std::endl;
    o << "RenNormalMap " << (void*)&t << " end" << std::endl;

    return o;
}

/* End NORMMAP.CPP **************************************************/

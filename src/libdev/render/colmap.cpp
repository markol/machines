/*
 * C O L M A P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "render/colmap.hpp"

RenColourMap::RenColourMap()
{

    TEST_INVARIANT;
}

RenColourMap::~RenColourMap()
{
    TEST_INVARIANT;

}

void RenColourMap::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const RenColourMap& t )
{

    o << "RenColourMap " << (void*)&t << " start" << std::endl;
    o << "RenColourMap " << (void*)&t << " end" << std::endl;

    return o;
}

/* End COLMAP.CPP ***************************************************/

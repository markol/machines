/*
 * C L A S S 1 1 . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "pertest/class11.hpp"

#ifndef _INLINE
    #include "pertest/class11.ipp"
#endif

Class11::Class11()
{

    TEST_INVARIANT;
}

Class11::~Class11()
{
    TEST_INVARIANT;

}

void Class11::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const Class11& t )
{

    o << "Class11 " << (void*)&t << " start" << endl;
    o << "Class11 " << (void*)&t << " end" << endl;

    return o;
}

/* End CLASS11.CPP **************************************************/

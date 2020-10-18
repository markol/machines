/*
 * T E S T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "internal/test.hpp"

Test::Test()
{

    TEST_INVARIANT;
}

Test::~Test()
{
    TEST_INVARIANT;

}

void Test::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const Test& t )
{

    o << "Test " << (void*)&t << " start" << endl;
    o << "Test " << (void*)&t << " end" << endl;

    return o;
}

/* End TEST.CPP *****************************************************/

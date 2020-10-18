/*
 * C L A S S 8 . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "pertest/class8.hpp"

Class8::Class8()
{

    TEST_INVARIANT;
}

Class8::Class8( const Class8& )
{

    TEST_INVARIANT;
}

Class8::~Class8()
{
    TEST_INVARIANT;

}

Class8& Class8::operator =( const Class8& )
{
    return *this;
}

void Class8::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const Class8& t )
{

    o << "Class8 " << (void*)&t << " start" << endl;
    o << "Class8 " << (void*)&t << " end" << endl;

    return o;
}

/* End CLASS8.CPP ***************************************************/

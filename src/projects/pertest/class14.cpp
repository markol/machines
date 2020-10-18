/*
 * C L A S S 1 4 . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "pertest/class14.hpp"
#include "pertest/persist.hpp"

PER_DEFINE_PERSISTENT( Class14 );

Class14::Class14()
{
    size_t size = randomInt( 50 + 20 );
    reserve( size );
    for( size_t i = 0; i < size; ++i )
        push_back( randomInt() );

    TEST_INVARIANT;
}

Class14::~Class14()
{
    TEST_INVARIANT;

}

void Class14::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void perWrite( PerOstream& ostr, const Class14& c14 )
{
    const Class14::Base& base = c14;
    
    ostr << base;
}

void perRead( PerIstream& istr, Class14& c14 )
{
    Class14::Base& base = c14;
    
    istr >> base;
}

bool operator ==( const Class14& a, const Class14& b )
{
    const Class14::Base& baseA = a;
    const Class14::Base& baseB = b;
    
    return baseA == baseB;
}

ostream& operator <<( ostream& o, const Class14& t )
{

    o << "Class14 " << (void*)&t << " start" << endl;
    o << "Class14 " << (void*)&t << " end" << endl;

    return o;
}

/* End CLASS14.CPP **************************************************/

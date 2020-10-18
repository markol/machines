/*
 * C L A S S 1 0 . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "pertest/class10.hpp"

PER_DEFINE_PERSISTENT( Class10 );

Class10::Class10()
: Class9( 10.0 )
{

    TEST_INVARIANT;
}

Class10::~Class10()
{
    TEST_INVARIANT;

}

void    perWrite( PerOstream& ostr, const Class10& c10 )
{
    const Class9< int >&    c9 = c10;
    ostr << c9;
}

void    perRead( PerIstream& istr, Class10& c10 )
{
    Class9< int >&    c9 = c10;
    istr >> c9;
}

bool operator ==( const Class10& a, const Class10& b )
{
    const Class9< int >&    a9 = a;
    const Class9< int >&    b9 = b;
    
    bool    result = true;
    
    COMPARE_OBJECTS( a9, b9 );
    
    return result;
}

// virtual
void Class10::overrideMe() const
{
}

void Class10::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const Class10& t )
{
    const Class9< int >& base = t;
    o << base;

    return o;
}

/* End CLASS10.CPP **************************************************/

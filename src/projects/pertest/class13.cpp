/*
 * C L A S S 1 3 . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "pertest/class13.hpp"
#include "pertest/persist.hpp"

PER_DEFINE_PERSISTENT( Class13 );

Class13::Class13()
: i1_( randomInt() ),
  ptr_( NULL )
{

    TEST_INVARIANT;
}

Class13::~Class13()
{
    TEST_INVARIANT;

}

void Class13::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const Class13& t )
{

    o << "Class13 " << (void*)&t << " start" << endl;
    o << "Class13 " << (void*)&t << " end" << endl;

    return o;
}

bool operator ==( const Class13& a, const Class13& b )
{
    bool    result = true;
    
    COMPARE_OBJECTS( a.i1_, b.i1_ );
    
    return result;
}

void perWrite( PerOstream& ostr, const Class13& c13 )
{
    ostr << c13.i1_;
    ostr << c13.ptr_;
}

void perRead( PerIstream& istr, Class13& c13 )
{
    istr >> c13.i1_;
    istr >> c13.ptr_;
}

/* End CLASS13.CPP **************************************************/

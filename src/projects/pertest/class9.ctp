/*
 * C L A S S 9 . C T P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline template methods

#include "pertest/persist.hpp"

template< class X >
Class9< X >::Class9()
{
    ASSERT_FAIL( "This should never be called" );
    TEST_INVARIANT;
}

template< class X >
Class9< X >::Class9( double d )
: d1_( d ),
  x1_( randomInt() )
{
    TEST_INVARIANT;
}

template< class X >
Class9< X >::~Class9()
{
    TEST_INVARIANT;

}

template< class X >
bool operator ==( const Class9< X >& a, const Class9< X >& b )
{
    bool    result = true;
    
    COMPARE_OBJECTS( a.x1_, b.x1_ );
    COMPARE_OBJECTS( a.d1_, b.d1_ );

    return result;
}

template< class X >
void perWrite( PerOstream& ostr, const Class9< X >& c9 )
{
    ostr << c9.x1_;
    ostr << c9.d1_;
}

template< class X >
void perRead( PerIstream& istr, Class9< X >& c9 )
{
    istr >> c9.x1_;
    istr >> c9.d1_;
}

template< class X >
void Class9< X >::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

template< class X >
ostream& operator <<( ostream& o, const Class9< X >& t )
{
    o << t.x1_ << " ";
    o << t.d1_ << endl;

    return o;
}

/* End CLASS9.CTP ***************************************************/

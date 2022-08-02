/*
 * T C L A S S 1 . C T P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline template methods

#include "pertest/persist.hpp"
#include "pertest/per.hpp"

template< class T1, class T2 >
TClass1< T1, T2 >::TClass1()
: d1_( randomInt() ),
  d2_( randomInt() )
{

    TEST_INVARIANT;
}

template< class T1, class T2 >
TClass1< T1, T2 >::~TClass1()
{
    TEST_INVARIANT;

}

template< class T1, class T2 >
void TClass1< T1, T2 >::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

template< class T1, class T2 >
ostream& operator <<( ostream& o, const TClass1< T1, T2 >& t )
{

    o << "TClass1< T1, T2 > " << (void*)&t << " start" << std::endl;
    o << "TClass1< T1, T2 > " << (void*)&t << " end" << std::endl;

    return o;
}

/* End TCLASS1.CTP **************************************************/

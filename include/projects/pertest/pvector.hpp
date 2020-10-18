/*
 * P V E C T O R . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

/*
    ctl_pvector
    
    This is a version of the STL vector class that only contains
    pointers to objects.
*/

#ifndef _CTL_PVECTOR_HPP
#define _CTL_PVECTOR_HPP


#include "base/persist.hpp"

//////////////////////////////////////////////////////////////////////

template < class T >
class A
{
};

template< class T1 >
PerOstream& operator <<( PerOstream& ostr, const A< T1 >* pOb );



template < class T >
class B : public A< int >
{
};

template< class T1 >
PerOstream& operator <<( PerOstream& ostr, const B< T1 >* pOb );


fn( PerOstream& ostr )
{
    B<int>* pB;

    ostr << pB;
}



#endif
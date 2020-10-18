/*
 * U T I L I T Y . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

#ifndef _STD_UTILITY
#define _STD_UTILITY

#include "base/persist.hpp"
//#include "stdlib/private/ospace.hpp"
//#include "ospace/utility.h"

PER_DEFINE_NON_INTRUSIVE_PERSISTENT_T2( std::pair );

template< class T1, class T2 >
void perWrite( PerOstream& ostr, const std::pair< T1, T2 >& p )
{
    ostr << p.first;
    ostr << p.second;
}

template< class T1, class T2 >
void perRead( PerIstream& istr, std::pair< T1, T2 >& p )
{
    istr >> p.first;
    istr >> p.second;
}


#endif	/*	#ifndef _STD_UTILITY	*/

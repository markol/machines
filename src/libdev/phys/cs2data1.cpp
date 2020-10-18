/*
 * C S 2 D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/internal/cs2impl.hpp"

//////////////////////////////////////////////////////////////////////////////////////////

void myFn()
{
    //TEST_INVARIANT;

    int i, j;
    for( i = 0; i < 100; ++i )
        j += i;

    //Delete all the domains
    //PhysConfigSpace2d::DomainId id = domainIdGenerator_.begin();
    //for( ; id != domainIdGenerator_.end(); ++id )
    {
      //  if( domains_.contains( id ) )
      //      _DELETE( domains_[ id ] );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2DATA.CPP **************************************************/

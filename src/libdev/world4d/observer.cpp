/*
 * O B S E R V E R . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/observer.hpp"

#ifndef _INLINE
    #include "world4d/observer.ipp"
#endif

PER_DEFINE_PERSISTENT_ABSTRACT( W4dObserver );

void perWrite( PerOstream&, const W4dObserver& )
{
}

void perRead( PerIstream&, W4dObserver& )
{
}

/* End OBSERVER.CPP *************************************************/

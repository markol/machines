/*
 * O F A C T O R Y . C T P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline template methods

#include "machphys/levels.hpp"

template< class SUB_TYPE, class PART >
MachPhysSubTypedObjectFactory< SUB_TYPE, PART >::MachPhysSubTypedObjectFactory( size_t nEntries )
: root_( 10000 ),
  entries_( nEntries, NULL )
{
    TEST_INVARIANT;
}

template< class SUB_TYPE, class PART >
MachPhysSubTypedObjectFactory< SUB_TYPE, PART >::~MachPhysSubTypedObjectFactory()
{
    TEST_INVARIANT;
    
    //  Since the factory doesn't own the parts we don't have
    //  to delete them here. The parts will be deleted when
    //  the root is deleted.
}

template< class SUB_TYPE, class PART >
PART& MachPhysSubTypedObjectFactory< SUB_TYPE, PART >::part( SUB_TYPE subType, size_t hardwareLevel )
{
	size_t index = MachPhysLevels::instance().uniqueHardwareIndex( subType, hardwareIndex );

    if( entries_[ index ] == NULL )
        entries_[ index ] = _NEW( PART( &root_, subType, hardwareLevel ) );
    
    return *entries_[ index ];
}


template< class SUB_TYPE, class PART >
void MachPhysSubTypedObjectFactory< SUB_TYPE, PART >::CLASS_INVARIANT
{
}


/* End OFACTORY.CTP **************************************************/

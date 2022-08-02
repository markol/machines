/*
 * O F A C T O R Y . C T P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline template methods

#include "machphys/levels.hpp"

template< class ID, class PART >
MachPhysObjectFactory< ID, PART >::MachPhysObjectFactory( size_t nEntries )
: root_( 10000 ),
  entries_( nEntries, NULL )
{
    TEST_INVARIANT;
}

template< class ID, class PART >
MachPhysObjectFactory< ID, PART >::MachPhysObjectFactory( PerConstructor )
: root_( 10000 )
{
}

template< class ID, class PART >
MachPhysObjectFactory< ID, PART >::~MachPhysObjectFactory()
{
    TEST_INVARIANT;

    //  Since the factory doesn't own the parts we don't have
    //  to delete them here. The parts will be deleted when
    //  the root is deleted.
}

template< class ID, class PART >
PART& MachPhysObjectFactory< ID, PART >::part( const ID& id, size_t index )
{
    if( entries_[ index ] == NULL )
        entries_[ index ] = _NEW( PART( &root_, id ) );

    return *entries_[ index ];
}


template< class ID, class PART >
void MachPhysObjectFactory< ID, PART >::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//#include "ctl/fixedvec.ctp"

/* End OFACTORY.CTP **************************************************/

/*
 * B I D V O I D M . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "utility/private/bidvoidm.hpp"

#ifndef _INLINE
    #include "utility/private/bidvoidm.ipp"
#endif

UtlBoundedIdVoidPMap::UtlBoundedIdVoidPMap( UtlId upperBound )
:   upperBound_( upperBound ),
    aPointers_( NULL )
{
    PRE( upperBound > 1 );

    //Allocate the array to use for the void*
    aPointers_ = _NEW_ARRAY( void*, upperBound );

    //Initialise them
    for( UtlId i = upperBound; i--; )
        aPointers_[i] = NULL;

    TEST_INVARIANT;
}

UtlBoundedIdVoidPMap::~UtlBoundedIdVoidPMap()
{
    TEST_INVARIANT;

    //Delete the allocated memory
    _DELETE_ARRAY( aPointers_ );
}

void UtlBoundedIdVoidPMap::add( UtlId id, void* p )
{
    PRE( id < upperBound() );
    PRE( not contains( id ) );

    aPointers_[id] = p;
}

void UtlBoundedIdVoidPMap::remove( UtlId id )
{
    PRE( id < upperBound() );
    PRE( contains( id ) );

    aPointers_[id] = NULL;
}

void UtlBoundedIdVoidPMap::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const UtlBoundedIdVoidPMap& t )
{

    o << "UtlBoundedIdVoidPMap " << (void*)&t << " start" << std::endl;
    o << "UtlBoundedIdVoidPMap " << (void*)&t << " end" << std::endl;

    return o;
}
/* End BIDVOIDM.CPP *************************************************/

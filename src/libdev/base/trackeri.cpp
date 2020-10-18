/*
 * T R A C K E R I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/internal/trackeri.hpp"

DiagPointerTrackerImpl::DiagPointerTrackerImpl()
{
    TEST_INVARIANT;
}

DiagPointerTrackerImpl::~DiagPointerTrackerImpl()
{
    TEST_INVARIANT;

}

void    DiagPointerTrackerImpl::addPointer( void* ptr )
{
    pointers_.insert( ptr );
}

bool    DiagPointerTrackerImpl::pointerPresent( void* ptr ) const
{
    return pointers_.find( ptr ) != pointers_.end();
}

void    DiagPointerTrackerImpl::removePointer( void* ptr )
{
    PRE( pointerPresent( ptr ) );

    pointers_.erase( ptr );
}

void DiagPointerTrackerImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const DiagPointerTrackerImpl& t )
{
    for( ctl_set< void*, less_ptr< void > >::const_iterator i = t.pointers_.begin(); i != t.pointers_.end(); ++i )
    {
        o << *i << " ";
    }

    return o;
}

/* End TRACKERI.CPP *************************************************/

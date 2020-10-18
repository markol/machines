/*
 * T R A C K E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/tracker.hpp"
#include "base/internal/trackeri.hpp"

DiagPointerTracker::DiagPointerTracker()
: pImplementation_( _NEW( DiagPointerTrackerImpl ) )
{
    TEST_INVARIANT;
}

DiagPointerTracker::~DiagPointerTracker()
{
    TEST_INVARIANT;

    _DELETE( pImplementation_ );
}

void    DiagPointerTracker::addPointer( void* ptr )
{
    pImplementation_->addPointer( ptr );
}

bool    DiagPointerTracker::pointerPresent( void* ptr ) const
{
    return pImplementation_->pointerPresent( ptr );
}

void    DiagPointerTracker::removePointer( void* ptr )
{
    PRE( pointerPresent( ptr ) );

    pImplementation_->removePointer( ptr );
}

void DiagPointerTracker::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const DiagPointerTracker& t )
{
    o << *t.pImplementation_;

    return o;
}

/* End TRACKER.CPP **************************************************/

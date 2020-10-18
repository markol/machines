/*
 * G A R B C O L L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/garbcoll.hpp"

#include "ctl/algorith.hpp"
#include "world4d/manager.hpp"
#include "world4d/entity.hpp"

// static
W4dGarbageCollector& W4dGarbageCollector::instance()
{
    static W4dGarbageCollector instance_;
    return instance_;
}

W4dGarbageCollector::W4dGarbageCollector()
:   nextDeleteTime_( 0.0 )
{
    //Set reasonable vector sizes
    times_.reserve( 256 );
    entities_.reserve( 256 );

    TEST_INVARIANT;
}

W4dGarbageCollector::~W4dGarbageCollector()
{
    TEST_INVARIANT;

    //Reset the flags for all the entities, so they will get destructed in the normal way
    uint n = entities_.size();
    while( n-- )
    {
        W4dEntity* pEntity = entities_[n];
        if( pEntity != NULL )
            pEntity->isGarbage( false );
    }
}

void W4dGarbageCollector::add( W4dEntity* pEntity, const PhysAbsoluteTime& time )
{
    TEST_INVARIANT;

    //Store the time and entity
    times_.push_back( time );
    entities_.push_back( pEntity );

    //Update earliest time if applicable
    if( time < nextDeleteTime_ )
        nextDeleteTime_ = time;

    //Mark the entity as garbage
    pEntity->isGarbage( true );
}

void W4dGarbageCollector::collect()
{
    TEST_INVARIANT;

    //Check it's time
    PhysAbsoluteTime now = W4dManager::instance().time();
    if( now > nextDeleteTime_ )
    {
        //We'll update the delete time as we go
        nextDeleteTime_ = now + 10000.0;
        for( size_t i = times_.size(); i-- != 0; )
        {
            W4dEntity* pEntity = entities_[i];
            PhysAbsoluteTime iTime = times_[i];
            if( pEntity == NULL or iTime <= now )
            {
                //Delete the entity
                if( pEntity != NULL )
                {
                    ASSERT( pEntity->isGarbage(), "" );
                    pEntity->isGarbage( false );
                    _DELETE( pEntity );
                }

                //Remove these 2 entries
                times_.erase( times_.begin() + i );
                entities_.erase( entities_.begin() + i );
            }
            else if( iTime < nextDeleteTime_ )
            {
                nextDeleteTime_ = iTime;
            }
        }
    }

    TEST_INVARIANT;
}

void W4dGarbageCollector::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
    INVARIANT( times_.size() == entities_.size() );
}

ostream& operator <<( ostream& o, const W4dGarbageCollector& t )
{

    o << "W4dGarbageCollector " << (void*)&t << " start" << std::endl;
    o << "W4dGarbageCollector " << (void*)&t << " end" << std::endl;

    return o;
}

void W4dGarbageCollector::remove( W4dEntity* pEntity)
{
    TEST_INVARIANT;

    //Find the entry, and set it to null rather than removing it. Otherwise we might be editing
    //the vector while iterating through it in collect().
    ctl_pvector< W4dEntity >::iterator itEnd = entities_.end();
    ctl_pvector< W4dEntity >::iterator it = find( entities_.begin(), itEnd, pEntity );
    if( it != itEnd )
        (*it) = NULL;
}
/* End GARBCOLL.CPP *************************************************/

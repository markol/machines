/*
 * C R A S H I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/internal/crashi.hpp"

// static
BaseCrashInternal& BaseCrashInternal::instance()
{
    static BaseCrashInternal instance_;
    return instance_;
}

BaseCrashInternal::BaseCrashInternal()
{
    functions_.reserve( 4 );
    TEST_INVARIANT;
}

BaseCrashInternal::~BaseCrashInternal()
{
    TEST_INVARIANT;

}

void BaseCrashInternal::addCrashFunction( PFn pFn )
{
    functions_.push_back( pFn );
}

void BaseCrashInternal::handleCrash() const
{
    for( size_t i = 0; i < functions_.size(); ++i )
    {
        (*functions_[ i ])();
    }

    for( Streams::const_iterator i = streams_.begin(); i != streams_.end(); ++i )
    {
        std::ofstream& ostr = *(*i);

        ostr << " Stream closed under control ";
        ostr.close();
    }
}

void BaseCrashInternal::addCrashStream( std::ofstream& str )
{
    streams_.insert( &str );
}

void BaseCrashInternal::removeCrashStream( std::ofstream& str )
{
    streams_.erase( &str );
}

void BaseCrashInternal::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const BaseCrashInternal& t )
{

    o << "BaseCrashInternal " << (void*)&t << " start" << std::endl;
    o << "BaseCrashInternal " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CRASHI.CPP ***************************************************/

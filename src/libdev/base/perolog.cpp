/*
 * P E R O L O G . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/internal/perolog.hpp"

PersistenceObjectLog::PersistenceObjectLog()
: ptr_( NULL )
{

    TEST_INVARIANT;
}

PersistenceObjectLog::PersistenceObjectLog( const char* classname, const void* ptr )
: classname_( classname ),
  ptr_( ptr )
{
    ASSERT_INFO( "A" );
}

PersistenceObjectLog::~PersistenceObjectLog()
{
    TEST_INVARIANT;

}

void PersistenceObjectLog::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PersistenceObjectLog& t )
{

    o << "PersistenceObjectLog " << (void*)&t << " " << t.classname_ << " " << (void*)t.ptr_;

    return o;
}

bool operator ==( const PersistenceObjectLog& a, const PersistenceObjectLog& b )
{
    ASSERT_INFO( "A" );

    const bool result = ( a.classname_ == b.classname_ ) and ( a.ptr_ == b.ptr_ );

    ASSERT_INFO( "A" );

    return result;
}

bool operator <( const PersistenceObjectLog& a, const PersistenceObjectLog& b )
{
    bool result;

    if( a.ptr_ < b.ptr_ )
        result =  true;
    else if( a.ptr_ > b.ptr_ )
        result = false;
    else
        result = a.classname_ < b.classname_;

    return result;
}


/* End PEROLOG.CPP **************************************************/

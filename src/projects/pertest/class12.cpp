/*
 * C L A S S 1 2 . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "pertest/class12.hpp"
#include "pertest/persist.hpp"

PER_DEFINE_PERSISTENT( Class12 );

Class12::Class12()
{
    nBytes_ = 50 + randomInt( 200 );
    
    ptr_ = _NEW_ARRAY( char, nBytes_ );
    
    for( size_t i = 0; i < nBytes_; ++i )
    {
        ptr_[ i ] = _STATIC_CAST( char, randomInt() );
    }
    
    TEST_INVARIANT;
}

Class12::~Class12()
{
    TEST_INVARIANT;

    _DELETE_ARRAY( ptr_ );
}

bool operator ==( const Class12& a, const Class12& b )
{
    bool    result = true;

    if( a.nBytes_ != b.nBytes_ )
    {
        result = false;
    }
    else
    {
        if( memcmp( a.ptr_, b.ptr_, a.nBytes_ ) != 0 )
            result = false;
    }
    
    return result;
}

void perWrite( PerOstream& ostr, const Class12& c12 )
{
    ostr << c12.nBytes_;
    PER_WRITE_RAW_DATA( ostr, c12.ptr_, c12.nBytes_ );
}

void perRead( PerIstream& istr, Class12& c12 )
{
    istr >> c12.nBytes_;

    _DELETE_ARRAY( c12.ptr_ );
    c12.ptr_ = _NEW_ARRAY( char, c12.nBytes_ );

    PER_READ_RAW_DATA( istr, c12.ptr_, c12.nBytes_ );
}

void Class12::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const Class12& t )
{

    o << "Class12 " << (void*)&t << " start" << endl;
    o << "Class12 " << (void*)&t << " end" << endl;

    return o;
}

/* End CLASS12.CPP **************************************************/

/*
 * F I L T B U F F . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <algorithm>

#include "base/private/filter.hpp"

#include "base/internal/filtbuff.hpp"

BaseFilterBuffer::BaseFilterBuffer( BaseFilterOstream* pFilter )
: pFilter_( pFilter )
{
    TEST_INVARIANT;
}

BaseFilterBuffer::~BaseFilterBuffer()
{
    TEST_INVARIANT;
}

// virtual
int BaseFilterBuffer::overflow( int c )
{
//    cout << "overflow " << (char)c << endl;

    char    c1 = (char)c;

    do_sputn( &c1, 1 );

    return !EOF;
}

// virtual
int BaseFilterBuffer::underflow( void )
{
//    cout << "underflow" << endl;
    return EOF;
}

// virtual
int        BaseFilterBuffer::do_sgetn( char *, int len )
{
//    cout << "do_sgetn" << endl;

    return len;
}

// virtual
int        BaseFilterBuffer::do_sputn( char const *buf, int len )
{
//      cout << "do_sputn " << len << endl;
//
//      for( size_t i = 0; i < len; ++i )
//          cout << buf[ i ];

    pFilter_->filterCharacters( buf, len );

    return len;
}

// virtual
int        BaseFilterBuffer::sync()
{
    //  Dummy sync - always succeed

    return !EOF;
}

/* End FILTBUFF.CPP *************************************************/

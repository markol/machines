/*
 * F I L T B U F F . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <algorithm>

#include "base/internal/prepend.hpp"

#include "base/internal/prepbuff.hpp"

DiagPrependBuffer::DiagPrependBuffer( DiagOstreamPrepend* pFilter )
: pFilter_( pFilter )
{
}

DiagPrependBuffer::~DiagPrependBuffer()
{
}

// virtual
int DiagPrependBuffer::overflow( int c )
{
//    cout << "overflow " << (char)c << endl;

    char    c1 = (char)c;

    do_sputn( &c1, 1 );

    return !EOF;
}

// virtual
int DiagPrependBuffer::underflow()
{
//    cout << "underflow" << endl;
    return EOF;
}

// virtual
int        DiagPrependBuffer::do_sgetn( char *, int len )
{
//    cout << "do_sgetn" << endl;

    return len;
}

// virtual
int        DiagPrependBuffer::do_sputn( char const *buf, int len )
{
    pFilter_->streamData( buf, len );

    return len;
}

// virtual
int        DiagPrependBuffer::sync()
{
    //  Dummy sync - always succeed

    return !EOF;
}

/* End FILTBUFF.CPP *************************************************/

/*
 * I S T R B U F F . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <algorithm>
#include <iostream>

#include "base/istr.hpp"
#include "base/persist.hpp"
#include "base/internal/istrbuff.hpp"
#include "base/internal/istrimpl.hpp"

PerIstreamBuffer::PerIstreamBuffer( PerIstream* pFilter )
: pFilter_( pFilter )
{
}

PerIstreamBuffer::~PerIstreamBuffer()
{
}

// virtual
int PerIstreamBuffer::overflow( int )
{
//    cout << "overflow " << (char)c << endl;
//
//     char    c1 = (char)c;
//
//     do_sputn( &c1, 1 );
//
    return !EOF;
}
//
// virtual
int PerIstreamBuffer::underflow()
{
//    cout << "underflow" << endl;

    int c = pFilter_->istr_.get();

//    cout << "Character = " << c << endl;

    return c;
}

// virtual
int        PerIstreamBuffer::do_sgetn( char* buf, int len )
{
    pFilter_->pImpl_->logDataRead( len );

    pFilter_->istr_.read( buf, len );

    if( pFilter_->logRead() )
    {
        PER_READ_INDENT_STREAM( "$" );
        for( size_t i = 0; i < len; ++i )
        {
            PER_READ_INDENT_STREAM( std::hex << (int)buf[ i ] << " " << std::dec );
        }
        PER_READ_INDENT_STREAM( "$" << std::endl );
    }

    return len;
}

// virtual
int        PerIstreamBuffer::do_sputn( char const *, int len )
{
//    cout << "do_sputn " << len << endl;
//
//     for( size_t i = 0; i < len; ++i )
//         cout << buf[ i ];

//    pFilter_->filterCharacters( buf, len );

    return len;
}
//
// // virtual
// int        PerIstreamBuffer::sync()
// {
//     //  Dummy sync - always succeed
//
//     return __NOT_EOF;
// }

/* End ISTRBUFF.CPP *************************************************/

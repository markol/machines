/*
 * A P P B U F F . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <algorithm>
#include <cstring>

#include "base/internal/appbuff.hpp"

BaseAppendBuffer::BaseAppendBuffer( const char* fileName )
{
    PRE( fileName != NULL );

    pFileName_ = (char*)malloc( strlen( fileName + 1 ) );
    strcpy( pFileName_, fileName );
}

BaseAppendBuffer::BaseAppendBuffer()
: pFileName_( NULL )
{
}

BaseAppendBuffer::~BaseAppendBuffer()
{
    if( pFileName_ )
        free( pFileName_ );
}

void BaseAppendBuffer::name( const char* fileName )
{
    if( pFileName_ )
        free( pFileName_ );

    pFileName_ = (char*)malloc( strlen( fileName + 1 ) );
    strcpy( pFileName_, fileName );
}

// virtual
int BaseAppendBuffer::overflow( int c )
{
//    cout << "overflow " << (char)c << endl;

    char    c1 = (char)c;

    do_sputn( &c1, 1 );

    return !EOF;
}

// virtual
int BaseAppendBuffer::underflow()
{
//    cout << "underflow" << endl;
    return EOF;
}

// virtual
int        BaseAppendBuffer::do_sgetn( char *, int len )
{
//    cout << "do_sgetn" << endl;

    return len;
}

// virtual
int        BaseAppendBuffer::do_sputn( char const *buf, int len )
{
//     cout << "do_sputn " << len << endl;
//
//     for( size_t i = 0; i < len; ++i )
//         cout << buf[ i ];

    std::ofstream    ostr( pFileName_, std::ios::app );

    ostr.write( buf, len );

    return len;
}

// virtual
int        BaseAppendBuffer::sync()
{
    //  Dummy sync - always succeed

    return !EOF;
}

void BaseAppendBuffer::clear()
{
    std::ofstream    ostr( pFileName_ );
}

/* End APPBUFF.CPP *************************************************/

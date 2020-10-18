/*
 * F I L T E R . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <sstream>
#include <ctime>
#include <cstdio>
#include <cstring>

#include "base/internal/prepend.hpp"
#include "base/internal/prepbuff.hpp"
#include "base/internal/memchk.hpp"

//  This class uses malloc, realloc and free to avoid calling the
//  memory management system (which would happen if it used new or delete).
//  This avoids problems with startup ordering.

DiagOstreamPrepend::DiagOstreamPrepend( ostream* pOstr )
: ostream( &dummyStreamBuf( this ) ),
  pOstr_( pOstr ),
  prependNow_( true ),
  date_( false ),
  clock_( false ),
  buffer_( this ),
  prependString_( NULL ),
  nCharsAllocated_( 0 )
{
    //ostream& thisAsOStream = *this;
    // this again
    //thisAsOStream = &buffer_;
    this->rdbuf(&buffer_);
}

DiagOstreamPrepend::DiagOstreamPrepend( void )
: ostream( &dummyStreamBuf( this ) ),
  pOstr_( NULL ),
  prependNow_( true ),
  date_( false ),
  clock_( false ),
  buffer_( this ),
  prependString_( NULL ),
  nCharsAllocated_( 0 )
{
    //ostream& thisAsOStream = *this;
    //thisAsOStream = &buffer_;
    this->rdbuf(&buffer_);
}

DiagOstreamPrepend::~DiagOstreamPrepend()
{
    if( prependString_ )
        free( prependString_ );
}

void DiagOstreamPrepend::stream( ostream* pOstr )
{
    pOstr_ = pOstr;
}

void    DiagOstreamPrepend::streamData( const char* pBuf, size_t nChars )
{
    char* buffer = (char*)malloc( nChars + 1 );
    size_t  n = 0;

    for( size_t i = 0; i < nChars; ++i )
    {
        if( prependNow_ )
        {
            doPrepend();
            prependNow_ = false;
        }

        buffer[ n++ ] = pBuf[ i ];

        if( pBuf[ i ] == '\n' )
        {
            prependNow_ = true;
            outputCharacters( buffer, n );
            n = 0;
        }
    }

    if( n != 0 )
        outputCharacters( buffer, n );

    free( buffer );
}

void    DiagOstreamPrepend::outputCharacters( const char* pBuf, size_t nChars )
{
    if( pOstr_ )
        pOstr_->write( pBuf, nChars );
}

void    DiagOstreamPrepend::prependDate( bool value )
{
    date_ = value;
}

void    DiagOstreamPrepend::prependClock( bool value )
{
    clock_ = value;
}

void    DiagOstreamPrepend::doPrepend( void )
{
    if( date_ )
    {
        time_t  t = time( NULL );
        //  The -1 on the length is to avoid outputting the newline
        outputCharacters( ctime( &t ), strlen( ctime( &t ) ) - 1 );
        outputCharacters( "  " , 2 );
    }

    if( clock_ )
    {
        double  clockValue = (double)clock() / CLOCKS_PER_SEC;

        char    str[ 20 ];
        sprintf( str, "%8.2f  ", clockValue );
        outputCharacters( str, strlen( str ) );
    }

    if( prependString_ )
        outputCharacters( prependString_, strlen( prependString_ ) );
}

// static
std::streambuf&  DiagOstreamPrepend::dummyStreamBuf( DiagOstreamPrepend* pOwner )
{
    //  This is a dummy streambuf that is only used to keep
    //  the ostream constructor happy.

    static  DiagPrependBuffer   str_( pOwner );

    return str_;
}

bool    DiagOstreamPrepend::hasStream() const
{
    return pOstr_ != NULL;
}

void    DiagOstreamPrepend::nSpacesToIndent( int nSpaces )
{
    if( prependString_ == NULL )
    {
        nCharsAllocated_ = 20;
        prependString_ = (char*)malloc( nCharsAllocated_ );
        prependString_[ 0 ] = '\0';
    }
    else if( strlen( prependString_ ) + nSpaces >= nCharsAllocated_ )
    {
        nCharsAllocated_ = nCharsAllocated_ * 2 + nSpaces + 1;

        prependString_ = (char*)realloc( prependString_, nCharsAllocated_ );
    }

    if( nSpaces > 0 )
    {
        for( int i = 0; i < nSpaces; ++i )
            strcat( prependString_, " " );
    }
    else
    {
        int currentLength = strlen( prependString_ );
        int newLength = currentLength + nSpaces;

        if( newLength > 0 )
            prependString_[ newLength ] = '\0';
        else
            prependString_[ 0 ] = '\0';
    }
}

/* End FILTER.CPP *************************************************/

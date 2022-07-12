/*
 * O S T R . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <sstream>
#include <iostream>

#include "base/ostr.hpp"
#include "base/persist.hpp"
// #include "internal/ostrbuff.hpp"

PerOstream::PerOstream( ostream& ostr )
: ostr_( ostr )
{
    Persistence::instance().registerOpenOstream();
}

PerOstream::~PerOstream()
{
    Persistence::instance().registerCloseOstream();
}

void    PerOstream::write( const char* data, size_t length )
{
    ostr_.write( data, length );

    if( logWrite() )
    {
        PER_WRITE_INDENT_STREAM( "$" );
        for( size_t i = 0; i < length; ++i )
        {
            PER_WRITE_INDENT_STREAM( std::hex << (int)data[ i ] << " " << std::dec );
        }
        PER_WRITE_INDENT_STREAM( "$" << std::endl );
    }
}

// static
bool&    PerOstream::logWrite()
{
    static  bool    logWrite_ = false;

    return logWrite_;
}

// PerOstreamBuffer* PerOstream::pFilterBuffer()
// {
//     pBuffer_ = _NEW( PerOstreamBuffer( this ) );
//     return pBuffer_;
// }

void    PerOstream::filterCharacters( const char* pBuf, size_t nChars )
{
    outputCharacters( pBuf, nChars );
}

void    PerOstream::outputCharacters( const char* pBuf, size_t nChars )
{
    ostr_.write( pBuf, nChars );
}

/* End FILTER.CPP *************************************************/

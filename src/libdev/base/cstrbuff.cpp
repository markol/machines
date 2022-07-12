/*
 * C S T R B U F F . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <iostream>
#include <cstring>

#include "base/internal/cstrbuff.hpp"

BaseCircularStreamBuffer::BaseCircularStreamBuffer( size_t bufferSize )
: pBuffer_( _NEW_ARRAY( char, bufferSize ) ),
  bufferSize_( bufferSize ),
  bufferWrapped_( false ),
  nCharactersOutput_( 0 )
{
    PRE( bufferSize != 0 );

    setp( pBuffer_, pBuffer_ + bufferSize );

}

BaseCircularStreamBuffer::~BaseCircularStreamBuffer()
{

    _DELETE_ARRAY( pBuffer_ );
}

// virtual
void    BaseCircularStreamBuffer::clear()
{
    setp( pBuffer_, pBuffer_ + bufferSize_ );
    bufferWrapped_ = false;
    nCharactersOutput_ = 0;
}

// virtual
size_t    BaseCircularStreamBuffer::nCharactersInBuffer() const
{
    size_t  result;

    if( bufferWrapped_ )
        result = bufferSize_;
    else
        result = pptr() - pbase();

    return result;
}

// virtual
int BaseCircularStreamBuffer::overflow( int c )
{
    char    c1 = (char)c;

    do_sputn( &c1, 1 );

    return !EOF;
}

// virtual
int BaseCircularStreamBuffer::underflow()
{
    return EOF;
}

// virtual
int        BaseCircularStreamBuffer::do_sgetn( char *buf, int len )
{
    size_t  result;

    ASSERT( nCharactersOutput_ <= nCharactersInBuffer(), logic_error() );

    if( nCharactersOutput_ == nCharactersInBuffer() )
    {
        nCharactersOutput_ = 0;
        result = 0;
    }
    else
    {
        if( bufferWrapped_ )
        {
            char* pStart = pptr() + nCharactersOutput_;

            if( pStart < epptr() )
            {
                size_t nCharactersToOutput = std::min( len, _STATIC_CAST( int, epptr() - pStart ) );

                memcpy( buf, pStart, nCharactersToOutput );
                nCharactersOutput_ += nCharactersToOutput;
                result = nCharactersToOutput;
            }
            else
            {
                pStart -= bufferSize_;

                size_t nCharactersToOutput = std::min( len, _STATIC_CAST( int, pptr() - pStart ) );

                memcpy( buf, pStart, nCharactersToOutput );
                nCharactersOutput_ += nCharactersToOutput;
                result = nCharactersToOutput;
            }
        }
        else
        {
            size_t nCharactersToOutput = std::min( len, _STATIC_CAST( int, nCharactersInBuffer() - nCharactersOutput_ ) );
            memcpy( buf, pBuffer_ + nCharactersOutput_, nCharactersToOutput );
            nCharactersOutput_ += nCharactersToOutput;
            result = nCharactersToOutput;
        }
    }

    return result;
}

// virtual
int        BaseCircularStreamBuffer::do_sputn( char const *buf, int len )
{
    size_t  nWriteToEnd = epptr() - pptr();
    size_t  nWriteToStart = len - nWriteToEnd;

    bufferWrapped_ = true;

    size_t nWritten = 0;

    char*   pStart = pptr();
    char*   pLastWritten = pptr();

    if( pStart == epptr() )
        pStart = pBuffer_;

    while( nWritten != len )
    {
        ASSERT_INFO( nWritten );
        ASSERT_INFO( len );
        ASSERT( nWritten < len, logic_error() );

        size_t nToWrite = std::min( (int)( len - nWritten ), (int)( epptr() - pStart ) );

        ASSERT_INFO( len );
        ASSERT_INFO( nWritten );
        ASSERT_INFO( (void*)epptr() );
        ASSERT_INFO( (void*)pStart );
        ASSERT( nToWrite != 0, logic_error() );

        ASSERT( pStart >= pBuffer_, logic_error() );
        ASSERT( pStart < pBuffer_ + bufferSize_, logic_error() );
        ASSERT( pStart + nToWrite >= pBuffer_, logic_error() );
        ASSERT( pStart + nToWrite <= pBuffer_ + bufferSize_, logic_error() );

        memcpy( pStart, buf + nWritten, nToWrite );

        nWritten += nToWrite;
        pLastWritten = pStart + nToWrite;

        pStart = pBuffer_;
    }

    setp( pLastWritten, epptr() );

    return len;
}

// virtual
int        BaseCircularStreamBuffer::sync()
{
    //  Dummy sync - always succeed

    return !EOF;
}

/* End CSTRBUFF.CPP *************************************************/

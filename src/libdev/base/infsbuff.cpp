/*
 * I S T R B U F F . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <cstring>

#include "base/internal/infsbuff.hpp"

BaseInfiniteStreamBuffer::BaseInfiniteStreamBuffer()
: pBuffer_( _NEW_ARRAY( char, bufferIncrement() ) ),
  bufferSize_( bufferIncrement() ),
  nCharactersOutput_( 0 )
{
    setp( pBuffer_, pBuffer_ + bufferSize_ );

}

BaseInfiniteStreamBuffer::~BaseInfiniteStreamBuffer()
{

    _DELETE_ARRAY( pBuffer_ );
}

// virtual
void    BaseInfiniteStreamBuffer::clear( void )
{
    setp( pBuffer_, pBuffer_ + bufferSize_ );
    nCharactersOutput_ = 0;
}

// virtual
int BaseInfiniteStreamBuffer::overflow( int c )
{
//    cout << "overflow " << (char)c << endl;
    char    c1 = (char)c;

    do_sputn( &c1, 1 );

    return !EOF;
}

// virtual
int BaseInfiniteStreamBuffer::underflow( void )
{
//    cout << "underflow" << endl;
    return EOF;
}

// virtual
int        BaseInfiniteStreamBuffer::do_sgetn( char *buf, int len )
{
    size_t  result;

    ASSERT_INFO( nCharactersOutput_ );
    ASSERT_INFO( bufferSize_ );
    ASSERT_INFO( nCharactersInBuffer() );

    ASSERT( nCharactersOutput_ <= bufferSize_, logic_error() );
    ASSERT( nCharactersOutput_ <= nCharactersInBuffer(), logic_error() );

    if( nCharactersOutput_ == nCharactersInBuffer() )
    {
        nCharactersOutput_ = 0;
        result = 0;
    }
    else
    {
        size_t nCharactersToOutput = std::min( len,
          _STATIC_CAST( int, nCharactersInBuffer() - nCharactersOutput_ ) );

        memcpy( buf, pbase() + nCharactersOutput_, nCharactersToOutput );
        nCharactersOutput_ += nCharactersToOutput;
        result = nCharactersToOutput;
    }

    return result;
}

// virtual
int        BaseInfiniteStreamBuffer::do_sputn( char const *buf, int len )
{
    POST_DATA( size_t originalNCharactersInBuffer = nCharactersInBuffer() );
    POST_DATA( size_t originalBufferSize = bufferSize_ );

    //  There is insufficient space in the current buffer. We must increase
    //  it's size.

    //  This is not particularly efficient, this should probably be replaced by
    //  some malloc / realloc based code.

    size_t  newBufferSize = bufferSize_ + len + bufferIncrement();

    char*   pNewBuffer = _NEW_ARRAY( char, newBufferSize );

    //  Copy in the original data
    memcpy( pNewBuffer, pBuffer_, nCharactersInBuffer() );

    //  Copy in the new data
    memcpy( pNewBuffer + nCharactersInBuffer(), buf, len );

    //  Setup the streambuf to work with the new buffer
    size_t  newNCharactersInBuffer = nCharactersInBuffer() + len;
    setp( pNewBuffer, pNewBuffer + newBufferSize );
    pbump( newNCharactersInBuffer );

    _DELETE_ARRAY( pBuffer_ );

    pBuffer_ = pNewBuffer;
    bufferSize_ = newBufferSize;

    POST_INFO( originalNCharactersInBuffer );
    POST_INFO( len );
    POST_INFO( nCharactersInBuffer() );

    POST( originalNCharactersInBuffer + len == nCharactersInBuffer() );
    POST( bufferSize_ >= originalBufferSize + bufferIncrement() );

    return len;
}

// virtual
int        BaseInfiniteStreamBuffer::sync()
{
    //  Dummy sync - always succeed

    return !EOF;
}

size_t  BaseInfiniteStreamBuffer::nCharactersInBuffer( void ) const
{
    return pptr() - pbase();
}

// static
size_t  BaseInfiniteStreamBuffer::bufferIncrement( void )
{
    return 1000;
}

/* End ISTRBUFF.CPP *************************************************/

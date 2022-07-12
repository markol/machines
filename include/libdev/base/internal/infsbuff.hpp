/*
 * I S T R B U F F . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

/*
    BaseInfiniteStreamBuffer

    This is a support class for BaseLogBuffer. The class IS-A
    streambuf which puts its output into a buffer that will
    increase its size as necessary.
*/

#ifndef _ISTRBUFF_HPP
#define _ISTRBUFF_HPP

#include "base/base.hpp"

#include "base/internal/lstrbuff.hpp"

class BaseInfiniteStreamBuffer : public BaseLogStreamBuffer
{
public:
    BaseInfiniteStreamBuffer();
    ~BaseInfiniteStreamBuffer();

    virtual void    clear();

    virtual int     overflow( int c );
    virtual int     underflow();

    virtual int     do_sgetn( char *buf, int len );
    virtual int     do_sputn( char const *buf, int len );
    virtual int     sync();

private:
    // Operation deliberately revoked
    BaseInfiniteStreamBuffer( const BaseInfiniteStreamBuffer& );

    // Operation deliberately revoked
    BaseInfiniteStreamBuffer& operator =( const BaseInfiniteStreamBuffer& );

    // Operation deliberately revoked
    bool operator ==( const BaseInfiniteStreamBuffer& );

    virtual size_t  nCharactersInBuffer() const;
    static  size_t  bufferIncrement();

    char*   pBuffer_;
    size_t  bufferSize_;
    size_t  nCharactersOutput_;
};


#endif

/* End ISTRBUFF.HPP *************************************************/

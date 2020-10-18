/*
 * C S T R B U F F . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

/*
    BaseCircularStreamBuffer

    This is a support class for BaseLogBuffer. The class IS-A
    streambuf which puts its output into a circular buffer of
    fixed size.
*/

#ifndef _CSTRBUFF_HPP
#define _CSTRBUFF_HPP

#include "base/base.hpp"

#include "base/internal/lstrbuff.hpp"

class BaseCircularStreamBuffer : public BaseLogStreamBuffer
{
public:
    BaseCircularStreamBuffer( size_t nCharactersInBuffer );
    ~BaseCircularStreamBuffer( void );

    virtual void    clear( void );
    virtual size_t  nCharactersInBuffer( void ) const;

    virtual int     overflow( int c );
    virtual int     underflow();

    virtual int     do_sgetn( char *buf, int len );
    virtual int     do_sputn( char const *buf, int len );
    virtual int     sync();

private:
    // Operation deliberately revoked
    BaseCircularStreamBuffer( const BaseCircularStreamBuffer& );

    // Operation deliberately revoked
    BaseCircularStreamBuffer& operator =( const BaseCircularStreamBuffer& );

    // Operation deliberately revoked
    bool operator ==( const BaseCircularStreamBuffer& );

    char*   pBuffer_;
    size_t  bufferSize_;
    bool    bufferWrapped_;
    size_t  nCharactersOutput_;
};


#endif

/* End CSTRBUFF.HPP *************************************************/

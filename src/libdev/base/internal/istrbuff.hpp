/*
 * I S T R B U F F . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

/*
    PerIstreamBuffer

    Internal support class for PerIstream.
*/

#ifndef _PER_ISTRBUFF_HPP
#define _PER_ISTRBUFF_HPP

#include <iostream>

#include "base/base.hpp"

class PerIstream;

class PerIstreamBuffer : public std::streambuf
{
public:
    PerIstreamBuffer( PerIstream* pStream );
    ~PerIstreamBuffer();

    virtual int     overflow( int c );
    virtual int     underflow();
//
    virtual int     do_sgetn( char *buf, int len );
    virtual int     do_sputn( char const *buf, int len );
//     virtual int     sync();
//
//     CLASS_INVARIANT
//     {
//     }

private:
    // Operation deliberately revoked
    PerIstreamBuffer( const PerIstreamBuffer& );

    // Operation deliberately revoked
    PerIstreamBuffer& operator =( const PerIstreamBuffer& );

    // Operation deliberately revoked
    bool operator ==( const PerIstreamBuffer& );

    PerIstream*  pFilter_;
};


#endif

/* End ISTRBUFF.HPP *************************************************/

/*
 * O S T R B U F F . H P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

/*
    PerOstreamBuffer
    
    Internal support class for UtlFilterOstream.
*/

#ifndef _UTL_FILTBUFF_HPP
#define _UTL_FILTBUFF_HPP

#include <iostream.h>

#include "base/base.hpp"

class PerOstream;

class PerOstreamBuffer : public streambuf
{
public:
    PerOstreamBuffer( PerOstream* pFilter );
    ~PerOstreamBuffer();

    virtual int     overflow( int c );
    virtual int     underflow();

    virtual int     do_sgetn( char *buf, int len );
    virtual int     do_sputn( char const *buf, int len );
    virtual int     sync();
    
    CLASS_INVARIANT
    {
        INVARIANT( this != NULL );
    }

private:
    // Operation deliberately revoked
    PerOstreamBuffer( const PerOstreamBuffer& );

    // Operation deliberately revoked
    PerOstreamBuffer& operator =( const PerOstreamBuffer& );

    // Operation deliberately revoked
    bool operator ==( const PerOstreamBuffer& );

    PerOstream*  pFilter_;
};


#endif

/* End OSTRBUFF.HPP *************************************************/

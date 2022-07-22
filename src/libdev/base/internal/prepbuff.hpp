/*
 * F I L T B U F F . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

/*
    DiagPrependBuffer
*/

#ifndef _UTL_FILTBUFF_HPP
#define _UTL_FILTBUFF_HPP

#include <iostream>

#include "base/base.hpp"

class UtlFilterOstream;
class DiagOstreamPrepend;

class DiagPrependBuffer : public std::streambuf
{
public:
    DiagPrependBuffer( DiagOstreamPrepend* pFilter );
    ~DiagPrependBuffer();

    virtual int     overflow( int c );
    virtual int     underflow();

    virtual int     do_sgetn( char *buf, int len );
    virtual int     do_sputn( char const *buf, int len );
    virtual int     sync();

private:
    // Operation deliberately revoked
    DiagPrependBuffer( const DiagPrependBuffer& );

    // Operation deliberately revoked
    DiagPrependBuffer& operator =( const DiagPrependBuffer& );

    // Operation deliberately revoked
    bool operator ==( const DiagPrependBuffer& );

    DiagOstreamPrepend*  pFilter_;
};


#endif

/* End FILTBUFF.HPP *************************************************/

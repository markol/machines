/*
 * F I L T B U F F . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

/*
    BaseFilterBuffer

    Internal support class for BaseFilterOstream.
*/

#ifndef _BASE_FILTBUFF_HPP
#define _BASE_FILTBUFF_HPP

#include <iostream>

#include "base/base.hpp"

class BaseFilterOstream;

class BaseFilterBuffer : public std::streambuf
{
public:
    BaseFilterBuffer( BaseFilterOstream* pFilter );
    ~BaseFilterBuffer();

    virtual int     overflow( int c );
    virtual int     underflow();

    virtual int     do_sgetn( char *buf, int len );
    virtual int     do_sputn( char const *buf, int len );
    virtual int     sync();

    void CLASS_INVARIANT
    {
        INVARIANT( this != NULL );
    }

private:
    // Operation deliberately revoked
    BaseFilterBuffer( const BaseFilterBuffer& );

    // Operation deliberately revoked
    BaseFilterBuffer& operator =( const BaseFilterBuffer& );

    // Operation deliberately revoked
    bool operator ==( const BaseFilterBuffer& );

    BaseFilterOstream*  pFilter_;
};


#endif

/* End FILTBUFF.HPP *************************************************/

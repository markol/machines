/*
 * L S T R B U F F . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

/*
    BaseLogStreamBuffer

    This is a support class for BaseLogBuffer. The class is an
    abstract base class that IS-A streambuf and adds some
    functionality necessary for BaseLogBuffer.
*/

#ifndef _LSTRBUFF_HPP
#define _LSTRBUFF_HPP

#include <iostream>

#include "base/base.hpp"

class BaseLogStreamBuffer : public std::streambuf
{
public:
    BaseLogStreamBuffer();
    virtual ~BaseLogStreamBuffer();

    virtual void    clear() = 0;
    virtual size_t  nCharactersInBuffer() const = 0;

private:
    // Operation deliberately revoked
    BaseLogStreamBuffer( const BaseLogStreamBuffer& );

    // Operation deliberately revoked
    BaseLogStreamBuffer& operator =( const BaseLogStreamBuffer& );

    // Operation deliberately revoked
    bool operator ==( const BaseLogStreamBuffer& );

};


#endif

/* End LSTRBUFF.HPP *************************************************/

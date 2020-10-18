/*
 * L O G B U F F . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

/*
    BaseLogBuffer

    Implement a memory resident buffer that will allow
    data to be streamed to it. There are
    two types of logging buffer available, 'infinite' ( i.e.
    limited only by available memory ) buffers and finite
    buffers where the number of characters stored can be set.
*/

#ifndef _LOGSTREA_HPP
#define _LOGSTREA_HPP

#include <iostream>
#include <string>

#include "base/base.hpp"

class BaseLogStreamBuffer;

class BaseLogBuffer : public std::ostringstream
{
public:
    //  Create an 'infinite' (limited only by memory)  buffer
    BaseLogBuffer( void );

    //  Create a finite buffer which will store the last
    //  bufferSize characters
    BaseLogBuffer( size_t bufferSize );

    ~BaseLogBuffer( void );

    //  Clear the buffer of all characters
    void    clear( void );

    std::string dataAsString( void ) const;

    //  Note that writing the buffer out does not clear it
    friend ostream& operator <<( ostream& o, const BaseLogBuffer& t );

private:
    // Operation deliberately revoked
    BaseLogBuffer( const BaseLogBuffer& );

    // Operation deliberately revoked
    BaseLogBuffer& operator =( const BaseLogBuffer& );

    // Operation deliberately revoked
    bool operator ==( const BaseLogBuffer& );

    BaseLogStreamBuffer* pStreamBuffer_;

    BaseLogStreamBuffer* pInfiniteStreamBuffer( void );
    BaseLogStreamBuffer* pCircularStreamBuffer( size_t bufferSize );
};


#endif

/* End LOGSTREA.HPP *************************************************/

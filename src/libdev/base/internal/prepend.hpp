/*
 * P R E P E N D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    DiagOstreamPrepend

    Prepend useful bits of information to a diagnostic output stream
*/

#ifndef _BASE_PREPEND_HPP
#define _BASE_PREPEND_HPP

#include <iostream>
#include <string>

#include "base/base.hpp"
#include "base/internal/prepbuff.hpp"

class UtlFilterBuffer;

class DiagOstreamPrepend : public ostream
{
public:
    DiagOstreamPrepend( ostream* pOstr );
    DiagOstreamPrepend();

    void stream( ostream* pOstr );

    bool    hasStream() const;

    void    prependDate( bool value );
    void    prependClock( bool value );

    //  Adjust the number of spaces by which we will
    //  indent the output from the stream. This call
    //  is cumulative i.e. a call will a value of 2 will
    //  indent by 2 more spaces than were previously being
    //  used.
    void    nSpacesToIndent( int nSpaces );

    virtual ~DiagOstreamPrepend();

protected:

    void    outputCharacters( const char* pBuf, size_t nChars );

private:
    // Operation deliberately revoked
    DiagOstreamPrepend( const DiagOstreamPrepend& );

    // Operation deliberately revoked
    DiagOstreamPrepend& operator =( const DiagOstreamPrepend& );

    // Operation deliberately revoked
    bool operator ==( const DiagOstreamPrepend& );

    friend class DiagPrependBuffer;

    void    streamData( const char* pBuf, size_t nChars );
    void    doPrepend();

    DiagPrependBuffer   buffer_;
    ostream* pOstr_;

    bool    date_;
    bool    clock_;

    bool    prependNow_;

    char*   prependString_;
    size_t  nCharsAllocated_;

    static  std::streambuf&  dummyStreamBuf( DiagOstreamPrepend* pOwner );
};


#endif

/* End FILTER.HPP *************************************************/

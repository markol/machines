/*
 * O S T R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PerOstream

*/

#ifndef _PER_OSTR_HPP
#define _PER_OSTR_HPP

#include <iostream>

#include "base/base.hpp"

class PerOstreamBuffer;

class PerOstream
{
public:
    //  Note that ostr must be capable of accepting binary data
    PerOstream( ostream& ostr );

    //  Not yet implemented
    // PerOstream( const char* fileName );

    virtual ~PerOstream( void );

    void    write( const char*, size_t length );

    static  bool&   logWrite();

protected:

    void    outputCharacters( const char* pBuf, size_t nChars );

private:
    // Operation deliberately revoked
    PerOstream( const PerOstream& );

    // Operation deliberately revoked
    PerOstream& operator =( const PerOstream& );

    // Operation deliberately revoked
    bool operator ==( const PerOstream& );

    friend class PerOstreamBuffer;

    void    filterCharacters( const char* pBuf, size_t nChars );

//    virtual void    doFilterCharacters( const char* pBuf, size_t nChars ) = 0;

    PerOstreamBuffer* pFilterBuffer( void );

    PerOstreamBuffer*   pBuffer_;
    ostream& ostr_;
};


#endif

/* End OSTR.HPP *************************************************/

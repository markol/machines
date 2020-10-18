/*
 * W I N D S T R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    BaseWindowStream

    A stream which sends its output to a separate debug window process
*/

#ifndef _BASE_WINDSTR_HPP
#define _BASE_WINDSTR_HPP

#include "base/base.hpp"
#include "base/private/filter.hpp"

class BaseWindowStream : public BaseFilterOstream
// Canonical form revoked
{
public:
    BaseWindowStream( const char* windowName );
    ~BaseWindowStream( void );

    void CLASS_INVARIANT;
// 	ostream& ostr();
// 	void sendCBMessage();
    friend ostream& operator <<( ostream& o, const BaseWindowStream& t );

private:
    BaseWindowStream( const BaseWindowStream& );
    BaseWindowStream& operator =( const BaseWindowStream& );
    bool operator ==( const BaseWindowStream& );

    virtual void    doFilterCharacters( const char* pBuf, size_t nChars );
};


#endif

/* End WINDSTR.HPP *************************************************/

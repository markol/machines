/*
 * D U A L S T R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    BaseDualOstream

    Create a single ostream out of two ostreams. Anything written
    to the BaseDualOstream will get written to both the supplied
    ostreams.
*/

#ifndef _BASE_DUALSTR_HPP
#define _BASE_DUALSTR_HPP

#include "base/base.hpp"
#include "base/private/filter.hpp"

class BaseDualOstream : public BaseFilterOstream
// Canonical form revoked
{
public:
    BaseDualOstream( ostream&, ostream& );
    ~BaseDualOstream( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const BaseDualOstream& t );

    BaseDualOstream( const BaseDualOstream& );
    BaseDualOstream& operator =( const BaseDualOstream& );

    virtual void    doFilterCharacters( const char* pBuf, size_t nChars );

    ostream&    stream1_;
    ostream&    stream2_;
    
};


#endif

/* End DUALSTR.HPP **************************************************/

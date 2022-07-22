/*
 * F I L T E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    BaseFilterOstream

    An abstract base class to be used to filter data being
    sent to an ostream. Overload the filterCharacters function
    to do whatever processing you require on the data being
    fed in. You should then call outputCharacters with the
    transformed data.
*/

#ifndef _BASE_FILTER_HPP
#define _BASE_FILTER_HPP

#include <iostream>
#include <string>

#include "base/base.hpp"

class BaseFilterBuffer;

class BaseFilterOstream : public ostream
{
public:
    BaseFilterOstream();
    BaseFilterOstream( ostream& ostr );

    virtual ~BaseFilterOstream();

    //  Set a new output stream
    void stream( ostream& ostr );

protected:

    void    outputCharacters( const char* pBuf, size_t nChars );
    //  PRE( output stream set );

private:
    // Operation deliberately revoked
    BaseFilterOstream( const BaseFilterOstream& );

    // Operation deliberately revoked
    BaseFilterOstream& operator =( const BaseFilterOstream& );

    // Operation deliberately revoked
    bool operator ==( const BaseFilterOstream& );

    friend class BaseFilterBuffer;

    void    filterCharacters( const char* pBuf, size_t nChars );

    virtual void    doFilterCharacters( const char* pBuf, size_t nChars ) = 0;

    BaseFilterBuffer* pFilterBuffer();

    BaseFilterBuffer*   pBuffer_;
    ostream* pOstr_;
};


#endif

/* End FILTER.HPP *************************************************/

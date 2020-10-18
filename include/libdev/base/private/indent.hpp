/*
 * I N D E N T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    BaseIndentOstream

    A brief description of the class should go in here
*/

#ifndef _BASE_INDENT_HPP
#define _BASE_INDENT_HPP

#include "base/base.hpp"

#include "base/private/filter.hpp"

class BaseIndentOstream : public BaseFilterOstream
{
public:
    BaseIndentOstream( ostream& ostr, const std::string& indentString );
    ~BaseIndentOstream( void );

    //  Set a new output stream
    void stream( ostream& ostr );

    //  Set the indent string
    void indentString( const std::string&, size_t repeatCount = 1 );

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    BaseIndentOstream( const BaseIndentOstream& );

    // Operation deliberately revoked
    BaseIndentOstream& operator =( const BaseIndentOstream& );

    // Operation deliberately revoked
    bool operator ==( const BaseIndentOstream& );

    virtual void    doFilterCharacters( const char* pBuf, size_t nChars );

    std::string  indentString_;
    bool    indent_;
};


#endif

/* End INDENT.HPP ***************************************************/

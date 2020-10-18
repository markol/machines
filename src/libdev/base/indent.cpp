/*
 * I N D E N T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/private/indent.hpp"

BaseIndentOstream::BaseIndentOstream( ostream& ostr, const std::string& indentString )
: BaseFilterOstream( ostr ),
  indentString_( indentString ),
  indent_( true )
{

    TEST_INVARIANT;
}

BaseIndentOstream::~BaseIndentOstream()
{
    TEST_INVARIANT;

}

void BaseIndentOstream::stream( ostream& ostr )
{
    BaseFilterOstream::stream( ostr );
}

void BaseIndentOstream::indentString( const std::string& str, size_t repeatCount )
{
    indentString_ = "";

    for( size_t i = 0; i < repeatCount; ++i )
        indentString_ += str;
}

void    BaseIndentOstream::doFilterCharacters( const char* pBuf, size_t nChars )
{
    for( size_t i = 0; i < nChars; ++i )
    {
        if( indent_ )
        {
            outputCharacters( indentString_.c_str(), indentString_.length() );
            indent_ = false;
        }

        if( pBuf[ i ] == '\n' )
            indent_ = true;

        outputCharacters( &pBuf[ i ], 1 );
    }
}

void BaseIndentOstream::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


/* End INDENT.CPP ***************************************************/

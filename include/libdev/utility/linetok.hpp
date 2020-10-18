/*
 * L I N E T O K . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    UtlLineTokeniser

    A simple line and token based parsing system. Comments
    are started by '//' and continue until the end of the line.
    Blank lines are ignored.
*/

#ifndef _UTILITY_LINETOK_HPP
#define _UTILITY_LINETOK_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "system/pathname.hpp"

#include <string>

class UtlLineTokeniser
{
public:
    //  Initialise the parse with the input stream it's working from
    //  The current line number defaults to 1. The stream must have an
    //  associated name for the line tokeniser to pass back using the
    //  filename function.
    UtlLineTokeniser( istream&, const SysPathName& );
	// This ctor ensures that the first line is parsed with the passed delimiters.
	UtlLineTokeniser( istream&, const SysPathName&, const std::string& delimiters );

    UtlLineTokeniser( const SysPathName& fileName );
	// This ctor ensures that the first line is parsed with the passed delimiters.
	UtlLineTokeniser( const SysPathName& fileName, const std::string& delimiters );

    //  Return the name of the file that the current line was read from
    SysPathName fileName() const;

    //  Set the current line number, this does **not** change the actual line being parsed
	//  but is similar to #line.
    void    lineNumber( size_t currentLineNumber );
    //  PRE( not finished() );

    //  Get the current line number
    size_t  lineNumber( void ) const;
    //  PRE( not finished() );

    //  Parse in the next non blank line into the internal buffers
    void    parseNextLine( void );
    //  PRE( not finished() );

    // Forces the current line to be tokenised again when parseNextLine is called again.
    void    repeatLine( void );

    //  Returns true if there are no more lines to be
    //  read from the input stream
    bool    finished( void ) const;

    //  Return the current line untokenised
    const   std::string& line( void ) const;
    //  PRE( not finished() );

    typedef ctl_vector< std::string >    Tokens;

    //  Returns the current set of tokens. Tokens are delimited by white space
    //  and token delimiters
    const Tokens& tokens( void ) const;
    //  PRE( not finished() );

    //  Add a character that will be treated as white space in the input stream.
    //  White space characters delimit tokens and are also compressed - a
    //  stream of white space characters is equivalent to one white space
    //  character.
    void addWhiteSpace( char c );
    //  POST( isWhiteSpace( c ) );

    //  Remove a character that will be treated as white space.
    void removeWhiteSpace( char c );
    //  POST( not isWhiteSpace( c ) );

    //  Add a character that will be treated as a token delimiter in the
    //  input stream. Token delimiters are not compressed in the same way
    //  that white space is - i.e. several token delimiters in a row do
    //  not get compressed into one, they will produce a series of zero
    //  length tokens.
    void addTokenDelimiter( char c );
    //  POST( isTokenDelimiter( c ) );

    void removeTokenDelimiter( char c );
    //  POST( not isTokenDelimiter( c ) );

    //  Fixed tokens are sequences of characters that will always be
    //  regarded as separate tokens even if they aren't separated by
    //  any white space or token delimiters. Typical fixed tokens would
    //  be "{" and "}".
    void addFixedToken( const std::string& token );

    void removeFixedToken( const std::string& token );

    ~UtlLineTokeniser( void );

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    UtlLineTokeniser( const UtlLineTokeniser& );

    // Operation deliberately revoked
    UtlLineTokeniser& operator =( const UtlLineTokeniser& );

    // Operation deliberately revoked
    bool operator ==( const UtlLineTokeniser& );

    void    getNextLine( void );
    void    splitIntoTokens( void );
    bool    startOfComment( const std::string& line, size_t i );
    bool    matchesFixedToken( const std::string& line, size_t* pIndex );

    bool    isWhiteSpace( char c );
    bool    isTokenDelimiter( char c );
    void    foundInclude();

    void    finishedFile();

    //  Order dependancy - the three stream members must be
    //  defined in this order.
    //  All three of these streams are necessary to handle the
    //  different constructors. In the future it might be possible
    //  to remove the constructors that take a pathname and simplify
    //  matters a little.
    std::ifstream    ifstr_;
    std::istream&    istr_;
    std::istream*    pCurrentStream_;

    SysPathName currentFileName_;
    bool        streamExhausted_;
    std::string  line_;
    Tokens  tokens_;
    size_t  lineNumber_;
    std::string  whiteSpaceCharacters_;
    std::string  tokenDelimiterCharacters_;
    bool    repeatCurrentLine_;

    friend void utlDummyFunction1( void );

    struct FileData
    {
        SysPathName fileName_;
        istream*    pIstream_;
        size_t      lineNumber_;
    };

    ctl_vector< FileData >   fileDataStack_;

    typedef ctl_vector< std::string >    FixedTokens;

    FixedTokens fixedTokens_;
};


#endif

/* End LINETOK.HPP ***************************************************/

/*
 * L I N E T O K . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"

#include "utility/linetok.hpp"

#include <ctype.h>
#include <algorithm>

UtlLineTokeniser::UtlLineTokeniser(
    istream& istr,
    const SysPathName& fileName )
: istr_( istr ),
  pCurrentStream_( &istr_ ),
  streamExhausted_( false ),
  lineNumber_( 0 ),
  whiteSpaceCharacters_( " \t\n" ),
  repeatCurrentLine_( false ),
  currentFileName_( fileName )
{
    fileDataStack_.reserve( 32 );
    fixedTokens_.reserve( 64 );
    tokens_.reserve( 64 );

    parseNextLine();

    TEST_INVARIANT;
}

UtlLineTokeniser::UtlLineTokeniser(
    istream& istr,
    const SysPathName& fileName,
    const std::string & delimiters )
: istr_( istr ),
  pCurrentStream_( &istr_ ),
  streamExhausted_( false ),
  lineNumber_( 0 ),
  whiteSpaceCharacters_( " \t\n" ),
  tokenDelimiterCharacters_( delimiters ),
  repeatCurrentLine_( false ),
  currentFileName_( fileName )
{
    fileDataStack_.reserve( 32 );
    fixedTokens_.reserve( 64 );
    tokens_.reserve( 64 );

    parseNextLine();

    TEST_INVARIANT;
}

UtlLineTokeniser::UtlLineTokeniser( const SysPathName& fileName )
: ifstr_( fileName.pathname().c_str() ),
  istr_( ifstr_ ),
  pCurrentStream_( &ifstr_ ),
  streamExhausted_( false ),
  lineNumber_( 0 ),
  whiteSpaceCharacters_( " \t\n" ),
  currentFileName_( fileName ),
  repeatCurrentLine_( false )
{
    PRE( fileName.existsAsFile() );

    parseNextLine();

    TEST_INVARIANT;
}

UtlLineTokeniser::UtlLineTokeniser( const SysPathName& fileName, const std::string & delimiters )
: ifstr_( fileName.pathname().c_str() ),
  istr_( ifstr_ ),
  pCurrentStream_( &ifstr_ ),
  streamExhausted_( false ),
  lineNumber_( 0 ),
  whiteSpaceCharacters_( " \t\n" ),
  currentFileName_( fileName ),
  tokenDelimiterCharacters_( delimiters ),
  repeatCurrentLine_( false )
{
    PRE( fileName.existsAsFile() );

    parseNextLine();

    TEST_INVARIANT;
}

UtlLineTokeniser::~UtlLineTokeniser()
{
    TEST_INVARIANT;
}

void    UtlLineTokeniser::parseNextLine()
{
    PRE( not finished() );

    if( repeatCurrentLine_ )
	{
        repeatCurrentLine_ = false;
		splitIntoTokens();
	}
    else
    {
        bool    finished = false;

        while( not finished )
        {
            getNextLine();

            if( strncmp( line_.c_str(), "#include", 8 ) == 0 )
            {
                foundInclude();
            }
            else if( line_.length() == 0 )
            {
                if( fileDataStack_.empty() )
                {
                    finished = true;
                    streamExhausted_ = true;
                }

                finishedFile();

            }
            else
            {
                if( tokens_.size() != 0 )
                    finished = true;
            }
        }
    }
}

//  Get the next line from the file ( blank or non blank ) and split it into tokens.
void    UtlLineTokeniser::getNextLine()
{
    PRE( not finished() );

    //  We use the buffer so we aren't continually pushing
    //  single characters onto the end of the string

    char    buffer[ 100 ];
    size_t  bufferIndex = 0;

    bool    finished = false;

    line_ = std::string ();

    while( not finished )
    {
        char    ch = EOF;
        ch = pCurrentStream_->get();//pCurrentStream_->get( ch );

        if( ch == EOF or pCurrentStream_->eof() )
        {
            finished = true;
        }
        else
        {
            if ( ch  == '\n' )
                finished = true;


            if( bufferIndex == sizeof( buffer ) - 1 )
            {
                buffer[ bufferIndex ] = '\0';
                line_ += buffer;
                bufferIndex = 0;
            }

            buffer[ bufferIndex++ ] = ch;
        }
    }

    buffer[ bufferIndex++ ] = '\0';
    line_ += buffer;
    // Get rid of problematic carriage return
    line_.erase( std::remove(line_.begin(), line_.end(), '\r'), line_.end());
    ++lineNumber_;

    splitIntoTokens();
}

bool    UtlLineTokeniser::finished() const
{
    return streamExhausted_;
}

void    UtlLineTokeniser::repeatLine()
{
    repeatCurrentLine_ = true;
}

void    UtlLineTokeniser::lineNumber( size_t currentLineNumber )
{
    PRE( not finished() );

    lineNumber_ = currentLineNumber;
}

size_t  UtlLineTokeniser::lineNumber() const
{
    PRE( not finished() );

    return lineNumber_;
}

const UtlLineTokeniser::Tokens& UtlLineTokeniser::tokens() const
{
    PRE( not finished() );

    return tokens_;
}

const std::string &    UtlLineTokeniser::line() const
{
    PRE( not finished() );

    return line_;
}

void    UtlLineTokeniser::splitIntoTokens()
{
    enum State { IN_TOKEN, NOT_IN_TOKEN, READ_DELIMITER };

    tokens_.erase( tokens_.begin(), tokens_.end() );

    State   state = NOT_IN_TOKEN;

    bool    finished = false;
    size_t  index = 0;

    while( index < line_.length() and not finished )
    {
        if( startOfComment( line_, index ) )
        {
            finished = true;
        }
        else if( matchesFixedToken( line_, &index ) )
        {
            //  index will have been updated to point past the token
            state = NOT_IN_TOKEN;
        }
        else
        {
            if( state == IN_TOKEN )
            {
                if( isWhiteSpace( line_[ index ] ) )
                {
                    state = NOT_IN_TOKEN;
                }
                else if( isTokenDelimiter( line_[ index ] ) )
                {
                    state = READ_DELIMITER;
                }
                else
                    tokens_.back() += line_[ index ];
            }
            else
            {
                if( isTokenDelimiter( line_[ index ] ) )
                {
                    if( state == READ_DELIMITER )
                        tokens_.push_back( std::string () );

                    state = READ_DELIMITER;
                }
                else if( not isWhiteSpace( line_[ index ] ) )
                {
                    tokens_.push_back( std::string () );
                    tokens_.back() += line_[ index ];
                    state = IN_TOKEN;
                }
            }

            ++index;
        }
    }
}

bool    UtlLineTokeniser::startOfComment( const std::string & line, size_t i )
{
    bool    isComment = false;

    if( line[ i ] == '/' )
    {
        //  This might be the beginning of a comment - check the next character
        size_t j = i;
        ++j;

        if( j != line.length() )
            if( line[ j ] == '/' )
                isComment = true;
    }

    return isComment;
}

bool    UtlLineTokeniser::matchesFixedToken( const std::string & line, size_t* pIndex )
{
    bool    foundMatch = false;

    FixedTokens::const_iterator matchIterator;

    for( FixedTokens::const_iterator i = fixedTokens_.begin();
        i != fixedTokens_.end() and not foundMatch; ++i )
    {
        if( (*i).length() < line.length() - *pIndex )
        {
            if( (*i) == line.substr( *pIndex, (*i).length() ) )
            {
                foundMatch = true;
                matchIterator = i;
            }
        }
    }

    if( foundMatch )
    {
        tokens_.push_back( *matchIterator );
        *pIndex += (*matchIterator).length();
    }

    return foundMatch;
}

void UtlLineTokeniser::addFixedToken( const std::string & token )
{
    fixedTokens_.push_back( token );
}

void UtlLineTokeniser::removeFixedToken( const std::string & token )
{
    fixedTokens_.erase( remove( fixedTokens_.begin(), fixedTokens_.end(), token ), fixedTokens_.end() );
}

bool    UtlLineTokeniser::isWhiteSpace( char c )
{
    for( size_t i = 0; i < whiteSpaceCharacters_.length(); ++i )
        if( c == whiteSpaceCharacters_[ i ] )
            return true;

    return false;
}

bool    UtlLineTokeniser::isTokenDelimiter( char c )
{
    for( size_t i = 0; i < tokenDelimiterCharacters_.length(); ++i )
        if( c == tokenDelimiterCharacters_[ i ] )
            return true;

    return false;
}

void UtlLineTokeniser::addWhiteSpace( char c )
{
    PRE( not isWhiteSpace( c ) );
    PRE( not isTokenDelimiter( c ) );

    tokenDelimiterCharacters_ += c;

    POST( isWhiteSpace( c ) );
}

void UtlLineTokeniser::removeWhiteSpace( char c )
{
    bool    finished = false;

    for( size_t i = 0; i < whiteSpaceCharacters_.length() and not finished; ++i )
        if( c == whiteSpaceCharacters_[ i ] )
        {
            //whiteSpaceCharacters_.remove( i );
            whiteSpaceCharacters_.erase( whiteSpaceCharacters_.begin() + i );
            finished = true;
        }

    POST( not isWhiteSpace( c ) );
}

void UtlLineTokeniser::addTokenDelimiter( char c )
{
    PRE( not isWhiteSpace( c ) );
    PRE( not isTokenDelimiter( c ) );

    tokenDelimiterCharacters_ += c;

    POST( isTokenDelimiter( c ) );
}

void UtlLineTokeniser::removeTokenDelimiter( char c )
{
    bool    finished = false;

    for( size_t i = 0; i < tokenDelimiterCharacters_.length() and not finished; ++i )
        if( c == tokenDelimiterCharacters_[ i ] )
        {
            //tokenDelimiterCharacters_.remove( i );
            tokenDelimiterCharacters_.erase( tokenDelimiterCharacters_.begin() + i );
            finished = true;
        }

    POST( not isTokenDelimiter( c ) );
}

void UtlLineTokeniser::foundInclude()
{
    //  Strip the quotes from the filename
    std::string  newFileName = tokens()[ 1 ].substr( 1, tokens()[ 1 ].length() - 2 );

    FileData    data;
    data.fileName_ = fileName();
    data.pIstream_ = pCurrentStream_;
    data.lineNumber_ = lineNumber();

    fileDataStack_.push_back( data );

    currentFileName_ = string(newFileName);
    pCurrentStream_ = _NEW( std::ifstream( newFileName.c_str() ) );
    lineNumber_ = 0;

}

void UtlLineTokeniser::finishedFile()
{
    if( not fileDataStack_.empty() )
    {
        _DELETE( pCurrentStream_ );

        currentFileName_ = fileDataStack_.back().fileName_;
        pCurrentStream_ = fileDataStack_.back().pIstream_;
        lineNumber_ = fileDataStack_.back().lineNumber_;

        fileDataStack_.pop_back();
    }
}

SysPathName UtlLineTokeniser::fileName() const
{
    return currentFileName_;
}

void UtlLineTokeniser::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


/* End PARSER.CPP ***************************************************/

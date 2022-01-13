/*
 * P R E P O S T . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#ifndef NDEBUG

#include <ctime>

#include <string>
#include <cstring>
#include <exception>

#include "profiler/profiler.hpp"
#include "base/internal/diagstrs.hpp"
#include "base/internal/crashi.hpp"

//////////////////////////////////////////////////////////////////////////////////

BaseAssertion::AssertionAction_fn_AssertionInfo    BaseAssertion::assertionHandler_ = NULL;

//////////////////////////////////////////////////////////////////////////////////

WEAK_SYMBOL void BaseAssertion::preconditionFail( const char* exprStr, const char* file, const char* line )
{
	std::string	failString( exprStr );
	failString += ", ";
	failString += file;
	failString += ":";
	failString += line;

	assertFail( failString.c_str(), domain_error( "precondition failed" ) );
}

//////////////////////////////////////////////////////////////////////////////////

WEAK_SYMBOL void BaseAssertion::postconditionFail( const char* exprStr, const char* file, const char* line )
{
	std::string	failString = exprStr;
	failString += ", ";
	failString += file;
	failString += ":";
	failString += line;

    BaseAssertion::writeLoggedInfo( std::cout );

	assertFail( failString.c_str(), range_error( "postcondition failed" ) );
}

//////////////////////////////////////////////////////////////////////////////////

WEAK_SYMBOL void BaseAssertion::invariantFail( const char* exprStr, const char* file, const char* line,
    const char* calledFromFile, const char* calledFromLine )
{
	std::string	failString = exprStr;

	failString += ", ";
	failString += file;
	failString += ":";
	failString += line;
	failString += " called from ";
	failString += calledFromFile;
	failString += ":";
	failString += calledFromLine;

	assertFail( failString.c_str(), domain_error( "invariant failed" ) );
}

//////////////////////////////////////////////////////////////////////////////////

WEAK_SYMBOL void BaseAssertion::assertFileExists( const char* fileName, const char* file, const char* line )
{
#ifdef NDEBUG
    return;
#endif

    bool    assertionFailed = false;
    std::string  errorText;

    if( !validFileName( fileName ) )
    {
        assertionFailed = true;
        errorText = "File name invalid";
    }
    else
    {
    	unsigned attr;
    	std::ifstream f(fileName);
	    if( !f.good() )
        {
            assertionFailed = true;
            errorText = "File not found";
        }

        /*else
        {
            if( ( attr & _A_SUBDIR ) == _A_SUBDIR )
            {
                assertionFailed = true;
                errorText = "File not found (this is actually a directory)";
            }
        }*/
    }

    if( assertionFailed )
    {
        std::string  failString( fileName );
    	failString += ", ";
    	failString += file;
    	failString += ":";
    	failString += line;

#ifdef NDEBUG
        cout << errorText << " " << failString << endl;
        cerr << errorText << " " << failString << endl;
#else

    	assertFail( failString.c_str(), runtime_error( errorText.c_str() ) );
#endif
    }
}

//////////////////////////////////////////////////////////////////////////////////

// This function made sure that filenames weren't too long. It DID NOT assert any other validity.
// Completely useless on modern systems

bool BaseAssertion::validFileName( const char* fileName )
{
    return true;
}

//////////////////////////////////////////////////////////////////////////////////

WEAK_SYMBOL void BaseAssertion::assertFail( const char *msg, const xmsg& xms )
{
    //  Need a semaphore on this in case one ASSERT triggers others
    //  as the program is exiting

    static  bool    first = true;

    if( first )
    {
        first = false;

    	_RAISE( xms );
		std::string assertFileName( "assert.log" );
		char* pAssertEnv = getenv("CB_ASSERT_TO");
		if( pAssertEnv )
			assertFileName = std::string( pAssertEnv );

        std::ofstream ofs( assertFileName.c_str() );

        time_t  t = time( NULL );
        ofs << ctime( &t );

        ofs << std::endl;
        writeLoggedInfo( ofs );
        ofs << std::endl;

        ofs << xms.what() << ": " << msg << std::endl << std::endl;

        bool writeStackAnchor = true;
        ulong lineNumber = 0;
        const char* extraString = NULL;

        ofs << std::endl;
        ofs << "------ Stack trace start ------------------------------" << std::endl;
        ProProfiler::instance().traceStack( ofs, writeStackAnchor, lineNumber, extraString );
        ofs << "------ Stack trace finish -----------------------------" << std::endl << std::endl;

        ofs << ctime( &t );
        ofs.close();

        writeLoggedInfo( std::cerr );
        std::cerr << std::endl;
        std::cerr << xms.what() << ": ";
        std::cerr << msg << std::endl << std::endl;

    }

    AssertionAction result = ASSERT_FAIL;

    if( assertionHandler_ )
    {
        AssertionInfo   a;

        result = assertionHandler_( a );
    }

    if( result == ASSERT_FAIL )
    {
        //  Make sure that all the diagnostic streams have been closed

        DiagStreams::instance().close();

        ProProfiler::instance().closeOutputStream();

        BaseCrashInternal::instance().handleCrash();

        abort();
    }
}

//  TBD: Make this less of a hack - get this into some internal class
bool inAssertionInfo_ = false;

//  True iff we are currently in an ASSERT, PRE or POST
// static
bool BaseAssertion::inAssertion()
{
    return BaseAssertion::AssertionLock::instance().isLocked();
}

//  True iff we are currently in an ASSERT_INFO, PRE_INFO or POST_INFO
// static
bool BaseAssertion::inAssertionInfo()
{
    return inAssertionInfo_;
}

//  True iff we are currently in an ASSERT_DATA, PRE_DATA or POST_DATA
//  (Not yet propely implemented - always returns false)
// static
bool BaseAssertion::inAssertionData()
{
    return false;
}

//////////////////////////////////////////////////////////////////////////////////

//  TBD: Make this less of a hack
size_t  BaseAssertion::index_ = 0;
BaseAssertion::InfoEntry	BaseAssertion::info_[ BaseAssertion::N_SAVED_ENTRIES ];

size_t	BaseAssertion::nextIndex( void )
{
    size_t result = index_;

    index_ = ( index_ + 1 ) % N_SAVED_ENTRIES;

    //  TBD: This is a hack - it works because this function is only called from the LOG_INFO macro
    inAssertionInfo_ = true;

    return result;
}

void    BaseAssertion::logFilePositionInfo( size_t index, const char* file, size_t line, InfoType type )
{
    info_[ index ].logFilePositionInfo( file, line, type );

    //  TBD: This is a hack - it works because this function is only called from the LOG_INFO macro
    inAssertionInfo_ = false;
}

std::ostringstream&	BaseAssertion::str( size_t index )
{
    return  info_[ index ].str();
}

void	BaseAssertion::writeLoggedInfo( ostream& o )
{
    for( size_t i = 0; i < N_SAVED_ENTRIES; ++i )
    {
        size_t  j = ( index_ + i ) % N_SAVED_ENTRIES;

        if( info_[ j ].containsData() )
            o << info_[ j ];
    }
}

//////////////////////////////////////////////////////////////////////////////////

void BaseAssertion::set_assertion_handler( AssertionAction_fn_AssertionInfo fn )
{
    assertionHandler_ = fn;
}

//////////////////////////////////////////////////////////////////////////////////

BaseAssertion::InfoEntry::InfoEntry( void )
: stream_( dataBuffer_ ),
  lineNumber_( 0 ),
  containsData_( false )
{
     dataBuffer_[ 0 ] = '\0';
     fileNameBuffer_[ 0 ] = '\0';
     stream_.precision( 18 );
}

//////////////////////////////////////////////////////////////////////////////////

std::ostringstream& BaseAssertion::InfoEntry::str( void )
{
    stream_.str("");
    stream_.clear();
    stream_.rdbuf()->pubseekoff( 0, std:: ios::beg, std::ios::in | std::ios::out );

    return stream_;
}

//////////////////////////////////////////////////////////////////////////////////

void    BaseAssertion::InfoEntry::logFilePositionInfo( const char* file, size_t line, InfoType type )
{
    strncpy( fileNameBuffer_, file, MAX_FILENAME_LENGTH );
    fileNameBuffer_[ MAX_FILENAME_LENGTH - 1 ] = '\0';
    lineNumber_ = line;
    type_ = type;

    containsData_ = true;
}

//////////////////////////////////////////////////////////////////////////////////

bool	BaseAssertion::InfoEntry::containsData( void ) const
{
    return containsData_;
}

//////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const BaseAssertion::InfoEntry& i )
{
    /*char buffer[ BaseAssertion::InfoEntry::MAX_DATA_LENGTH + 1 ];

    if( i.stream_. pcount() == 0 )
    {
        //  TBD: This is a bit of a hack and is only necessary because
        //  of my lack of understanding of the streams - Bob
        strncpy( buffer, i.dataBuffer_, BaseAssertion::InfoEntry::MAX_DATA_LENGTH );
        buffer[ BaseAssertion::InfoEntry::MAX_DATA_LENGTH ] = '\0';
    }
    else
    {
        strncpy( buffer, i.dataBuffer_, i.stream_.pcount() );
        buffer[ i.stream_.pcount() ] = '\0';
    }

    o << buffer << "\t";*/
    o << i.stream_.str() << "\t";
    o << i.fileNameBuffer_ << ", ";
    o << i.lineNumber_ << "  ";

    switch( i.type_ )
    {
        case BaseAssertion::PRE_TYPE:
            o << "( PRE )";
            break;
        case BaseAssertion::POST_TYPE:
            o << "( POST )";
            break;
        case BaseAssertion::ASSERT_TYPE:
            o << "( ASSERT )";
            break;
        case BaseAssertion::INVARIANT_TYPE:
            o << "( INVARIANT )";
            break;
    }

    o << std::endl;

    return o;
}

BaseAssertion::AssertionInfo::AssertionInfo( void )
{
}

#endif

/* End PREPOST.CPP ***************************************************/

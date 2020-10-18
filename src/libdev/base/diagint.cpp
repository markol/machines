/*
 * D I A G I N T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include <signal.h>
#include <time.h>

#include "base/private/diagint.hpp"
#include "base/internal/diagstrs.hpp"
#include "base/internal/crashi.hpp"
#include "base/tracker.hpp"
#include "profiler/profiler.hpp"

//  An exception to our normal "no globals" rule. These are only plain data types
//  so won't give us any memory allocation problems. They are needed to allow
//  various diagnostics to be turned on and off efficiently.

bool    DiagInternal::trackAllObjects_ = false;
bool    DiagInternal::checkCountedPointers_ = false;
bool    DiagInternal::checkInvariants_ = false;

// static
bool	DiagInternal::streamEnabled( DiagStreamType type )
{
    PRE( type < DiagStreams::instance().nStreams() );

    return DiagStreams::instance().diagStreamEnabled_[ type ];
}

// static
void	DiagInternal::enableStream( DiagStreamType type )
{
    PRE( type < DiagStreams::instance().nStreams() );

    if( DiagStreams::instance().diagStreams_[ type ].hasDestination() )
        DiagStreams::instance().diagStreamEnabled_[ type ] = true;
}

// static
void	DiagInternal::disableStream( DiagStreamType type )
{
    PRE( type < DiagStreams::instance().nStreams() );

    DiagStreams::instance().diagStreamEnabled_[ type ] = false;
}

// static
DiagPointerTracker&	DiagInternal::countedPointerTracker()
{
    static  DiagPointerTracker	countedPointerTracker_;

    return countedPointerTracker_;
}

// static
bool DiagInternal::checkFpException()
{
    static bool result = getenv( "CB_FP_EXCEPTION" ) != NULL;

    return result;
}

// static
void DiagInternal::disableFPException()
{
    //  This does not appear to disable the exceptions properly - it
    //  needs more investigation

    if( checkFpException() )
    {
        signal( SIGFPE, SIG_IGN );
    }
}

// static
void DiagInternal::enableFPException()
{
    if( checkFpException() )
    {
        signal( SIGFPE, signalHandler );
    }
}

// static
void    DiagInternal::initialiseSignalHandler( void )
{
    if( checkFpException() )
        signal( SIGFPE, signalHandler );

    signal( SIGILL, signalHandler );
    signal( SIGINT, signalHandler );
    signal( SIGSEGV, signalHandler );
}

// static
void    DiagInternal::signalHandler( int sig )
{
    std::string  text;

    switch( sig )
    {
        case SIGABRT:
            text = "abnormal termination, such as caused by the  abort function";
            break;
#ifdef _WIN32
        case SIGBREAK:
#else
        case SIGKILL:
#endif // WINDOWS
            text = "an interactive attention (CTRL/BREAK on keyboard) is signalled";
            break;
        case SIGFPE:
            text = "an erroneous floating-point operation occurs (such as division by zero, overflow and underflow)";
            break;
        case SIGILL:
            text = "illegal instruction encountered";
            break;
        case SIGINT:
            text = "an interactive attention (CTRL/C on keyboard) is signalled";
            break;
        case SIGSEGV:
            text = "an illegal memory reference is detected";
            break;
        case SIGTERM:
            text = "a termination request is sent to the program";
            break;
/*        case SIGUSR1:
            text = "OS/2 process flag A via DosFlagProcess";
            break;
        case SIGUSR2:
            text = "OS/2 process flag B via DosFlagProcess";
            break; */
    }

	std::string crashFileName( "crash.log" );
	char* pCrashEnv = getenv("CB_CRASH_TO");
	if( pCrashEnv )
		crashFileName = std::string( pCrashEnv );

    std::ofstream ostr( crashFileName.c_str() );
    ostr << text << std::endl;
    time_t  t = time( NULL );
    ostr << ctime( &t );
    ostr << std::endl;

    #ifndef NDEBUG
        BaseAssertion::writeLoggedInfo( ostr );
    #endif

    ostr << std::endl;
    ostr << "------ Stack trace start ------------------------------" << std::endl;
    ProProfiler::instance().traceStack( ostr, true, 0, NULL );
    ostr << "------ Stack trace finish -----------------------------" << std::endl << std::endl;

    ostr << std::endl;
    ostr << ctime( &t );
    ostr << std::endl;

    DiagStreams::instance().close();

    ProProfiler::instance().closeOutputStream();

    BaseCrashInternal::instance().handleCrash();
}

/* End DIAGINT.CPP **************************************************/

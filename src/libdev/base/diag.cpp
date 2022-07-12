/*
 * D I A G . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "base/diag.hpp"
#include "base/tracker.hpp"
#include "base/private/diagint.hpp"
#include "base/internal/diagstr.hpp"
#include "base/internal/diagstrs.hpp"
#include "base/internal/crashi.hpp"

#include "ctl/private/ptrvalid.hpp"
#include <cstring>

Diag& Diag::instance()
{
    static  Diag    instance_;

    return instance_;
}

Diag::Diag()
{
    DiagStreams::instance();

#ifndef NDEBUG
    BaseTrace::instance();
#endif

    const char* pStr = getenv( "CB_OBJECT_TRACKING" );

    if( pStr )
    {
        if( strcmp( pStr, "all" ) == 0 )
        {
            DiagInternal::trackAllObjects_ = true;
        }
    }

    pStr = getenv( "CB_COUNTED_POINTER_CHECKING" );

    if( pStr )
    {
        if( strcmp( pStr, "on" ) == 0 )
        {
            DiagInternal::checkCountedPointers_ = true;
        }
    }

    pStr = getenv( "CB_INVARIANT_CHECKING" );

    if( pStr )
    {
        if( strcmp( pStr, "on" ) == 0 )
        {
            DiagInternal::checkInvariants_ = true;
        }
    }

    for( size_t i = 0; i < DiagStreams::instance().nStreams(); ++i )
    {
        char envName[ 200 ];

        strcpy( envName, "CB_" );
        strcat( envName, DiagInternal::streamText( i ) );
        strcat( envName, "_TO" );

        char defaultSetting[ 200 ];
        defaultSetting[ 0 ] = '\0';

        if( strcmp( DiagInternal::streamText( i ), "memory" ) == 0 )
            strcpy( defaultSetting, "memory.log" );

        if( strcmp( DiagInternal::streamText( i ), "where" ) == 0 )
            strcpy( defaultSetting, "cout" );

        if( strcmp( DiagInternal::streamText( i ), "trace" ) == 0 )
            strcpy( defaultSetting, "trace.log" );

        DiagStreams::instance().diagStreams_[ i ].setup( envName, defaultSetting );

        if( DiagStreams::instance().diagStreams_[ i ].hasDestination() )
            DiagStreams::instance().diagStreamEnabled_[ i ] = true;
    }

#ifndef PRODUCTION
    DiagInternal::initialiseSignalHandler();
#endif
}

Diag::~Diag()
{
}

ostream& Diag::stream( DiagStreamType type )
{
    return DiagStreams::instance().diagStreams_[ type ].ostr();
}

void Diag::indent( DiagStreamType type, int nSpaces )
{
    DiagStreams::instance().diagStreams_[ type ].indent( nSpaces );
}

const char*	Diag::getenv( const char* name )
{
    return ::getenv( name );
}

ostream& Diag::bobStream()
{
    return stream( DIAG_BOB );
}

ostream& Diag::halStream()
{
    return stream( DIAG_HAL );
}

ostream& Diag::iainStream()
{
    return stream( DIAG_IAIN );
}

ostream& Diag::jerryStream()
{
    return stream( DIAG_JERRY );
}

ostream& Diag::jonStream()
{
    return stream( DIAG_JON );
}

ostream& Diag::richardStream()
{
    return stream( DIAG_RICHARD );
}

ostream& Diag::yueaiStream()
{
    return stream( DIAG_YUEAI );
}

ostream& Diag::lionelStream()
{
    return stream( DIAG_LIONEL );
}

ostream& Diag::ceriStream()
{
    return stream( DIAG_CERI );
}

ostream& Diag::neilStream()
{
    return stream( DIAG_NEIL );
}

ostream& Diag::wayneStream()
{
    return stream( DIAG_WAYNE );
}

ostream& Diag::miscStream()
{
    return stream( DIAG_MISC );
}

ostream& Diag::whereStream()
{
    return stream( DIAG_WHERE );
}

ostream& Diag::perReadStream()
{
    return stream( DIAG_PER_READ );
}

ostream& Diag::perWriteStream()
{
    return stream( DIAG_PER_WRITE );
}

ostream& Diag::traceStream()
{
    return stream( DIAG_TRACE );
}

ostream& Diag::danielStream()
{
    return stream( DIAG_DANIEL );
}

ostream& Diag::motseqStream()
{
    return stream( DIAG_MOTSEQ );
}

ostream& Diag::cs2pathStream()
{
    return stream( DIAG_CS2PATH );
}

ostream& Diag::cs2vgraStream()
{
    return stream( DIAG_CS2VGRA );
}

ostream& Diag::observerStream()
{
    return stream( DIAG_OBSERVER );
}

ostream& Diag::pathProfileStream()
{
    return stream( DIAG_PATH_PROFILE );
}


ostream& Diag::memoryStream()
{
    //  The memory stream is a special case - we have to be careful about
    //  recursion since in the process of setting up the memory stream it
    //  will call _NEW which will then attempt to log itself to the memory
    //  stream. Hence the alreadyInFunction variable to avoid recursion.

    static bool alreadyInFunction = false;

    if( alreadyInFunction )
    {
        static std::ofstream ostr;
        return ostr;
    }

    alreadyInFunction = true;

    ostream& result = stream( DIAG_MEMORY );

    alreadyInFunction = false;

    return result;
}

ostream& Diag::renderStream()
{
    return stream( DIAG_RENDER );
}

ostream& Diag::soundStream()
{
    return stream( DIAG_SOUND );
}

ostream& Diag::networkStream()
{
    return stream( DIAG_NETWORK );
}

ostream& Diag::recRecordStream()
{
    return stream( DIAG_REC_RECORD );
}

ostream& Diag::recPlaybackStream()
{
    return stream( DIAG_REC_PLAYBACK );
}

ostream& Diag::netAnalysisStream()
{
    return stream( DIAG_NET_ANALYSIS );
}

ostream& Diag::aStarStream()
{
    return stream( DIAG_A_STAR );
}

ostream& Diag::linearTravelStream()
{
    return stream( DIAG_LINEAR_TRAVEL );
}

ostream& Diag::planetSurfaceStream()
{
    return stream( DIAG_PLANET_SURFACE );
}

ostream& Diag::physMotionStream()
{
    return stream( DIAG_PHYS_MOTION );
}

ostream& Diag::networkErrorsStream()
{
    return stream( DIAG_NETWORK_ERRORS );
}

ostream& Diag::mexGrid2dStream()
{
    return stream( DIAG_MEX_GRID2D );
}

ostream& Diag::configSpaceStream()
{
    return stream( DIAG_CONFIG_SPACE );
}

void    Diag::invariantCheckingActive( bool flag )
{
    DiagInternal::checkInvariants_ = flag;
}

bool    Diag::invariantCheckingActive() const
{
    return DiagInternal::checkInvariants_;
}

const char*	DiagInternal::streamText( size_t index )
{
	const char* result;

	DiagStreamType type = _STATIC_CAST( DiagStreamType, index );

	switch( type )
	{
		case DIAG_BOB:
			result = "BOB";
			break;
		case DIAG_HAL:
			result = "HAL";
			break;
		case DIAG_IAIN:
			result = "IAIN";
			break;
		case DIAG_JERRY:
			result = "JERRY";
			break;
		case DIAG_RICHARD:
			result = "RICHARD";
			break;
		case DIAG_JON:
			result = "JON";
			break;
		case DIAG_YUEAI:
			result = "YUEAI";
			break;
		case DIAG_LIONEL:
			result = "LIONEL";
			break;
		case DIAG_CERI:
			result = "CERI";
			break;
		case DIAG_NEIL:
			result = "NEIL";
			break;
		case DIAG_MISC:
			result = "MISC";
			break;
		case DIAG_WHERE:
			result = "WHERE";
			break;
		case DIAG_TRACE:
			result = "TRACE";
			break;
		case DIAG_MEMORY:
			result = "MEMORY";
			break;
		case DIAG_RENDER:
			result = "RENDER";
			break;
		case DIAG_PER_WRITE:
			result = "PER_WRITE";
			break;
		case DIAG_PER_READ:
			result = "PER_READ";
			break;
		case DIAG_NETWORK:
			result = "NETWORK";
			break;
		case DIAG_WAYNE:
			result = "WAYNE";
			break;
        case DIAG_DANIEL:
            result = "DANIEL";
            break;
        case DIAG_MOTSEQ:
            result = "MOTSEQ";
            break;
        case DIAG_CS2PATH:
            result = "CS2PATH";
            break;
        case DIAG_CS2VGRA:
            result = "CS2VGRA";
            break;
        case DIAG_OBSERVER:
            result = "OBSERVER";
            break;
        case DIAG_PATH_PROFILE:
            result = "PATH_PROFILE";
            break;
        case DIAG_SOUND:
            result = "SOUND";
            break;
        case DIAG_REC_RECORD:
            result = "REC_RECORD";
            break;
        case DIAG_REC_PLAYBACK:
            result = "REC_PLAYBACK";
            break;
        case DIAG_NET_ANALYSIS:
            result = "NET_ANALYSIS";
            break;
        case DIAG_A_STAR:
            result = "A_STAR";
            break;
        case DIAG_LINEAR_TRAVEL:
            result = "LINEAR_TRAVEL";
            break;
        case DIAG_PLANET_SURFACE:
            result = "PLANET_SURFACE";
            break;
        case DIAG_PHYS_MOTION:
            result = "PHYS_MOTION";
            break;
        case DIAG_NETWORK_ERRORS:
            result = "NETWORK_ERRORS";
            break;
        case DIAG_MEX_GRID2D:
            result = "MEX_GRID2D";
            break;
        case DIAG_CONFIG_SPACE:
            result = "CONFIG_SPACE";
            break;
        default:
        #ifndef NDEBUG
            WHERE;
        #endif
            std::cout << "Unknown stream type" << std::endl;
            break;
	}

	return result;
}

void Diag::forceCrash() const
{
    DiagInternal::signalHandler( 0 );

    abort();
}

void Diag::addCrashFunction( PFn pFn )
{
    BaseCrashInternal::instance().addCrashFunction( pFn );
}

//  TBD: Put this into a class somewhere. We really need to make
//  sure that DiagInternal is properly internal - it is in the
//  private directory at the moment.

static size_t fpExceptionCount = 0;

void Diag::disableFPException()
{
    ++fpExceptionCount;

    DiagInternal::disableFPException();
}

void Diag::enableFPException()
{
    PRE( fpExceptionCount != 0 );

    --fpExceptionCount;

    if( fpExceptionCount == 0 )
        DiagInternal::enableFPException();
}

void Diag::addCrashStream( std::ofstream& str )
{
    BaseCrashInternal::instance().addCrashStream( str );
}

void Diag::removeCrashStream( std::ofstream& str )
{
    BaseCrashInternal::instance().addCrashStream( str );
}

//////////////////////////////////////////////////////////////////////

/* End DIAG.CPP *****************************************************/


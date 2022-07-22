/*
 * D I A G . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    Diag

    Useful diagnostic streams / functions. Please note that these should not be used
    for output that will appear in the final version of the product since
    these streams will not be present in the final release version. They are
    intended as debugging aids. Full documentation on their use can be
    found on the software server.

    How to add another stream:

    Please note that these instructions must be followed precisely to avoid
    having to recompile _everything_. Assume we are trying to add a stream
    with the code xxxx.

    1.  Cheat this header file ( cheat1 diag )
    2.  Add the DIAG_xxxx enumeration to DiagStreamType. This value must go
        after the last DIAG_ entry and before N_DIAG_STREAMS.
    3.  Add macros for
            xxxx_STREAM
            xxxx_WHERE
            xxxx_INSPECT
            xxxx_INDENT
            xxxx_STREAM_ENABLED
            xxxx_STREAM_ENABLE
            xxxx_STREAM_DISABLE
        in the appropriate places.
    4.  Add the xxxxStream method to the Diag class below
    5.  Finish the cheat of this header file ( cheat2 diag )
    6.  Add the xxxxStream method to diag.cpp
    7.  Add the enumeration value to string conversion in
        DiagInternal::streamText in diag.cpp
    9.  Note the changes that have been made in basver.cpp
    10. Force a recompile of the entire base library. This
        is absolutely necessary because of the cheating that
        has gone on.
    11. Check that the new streams work using the test application.
    12. Release the library on your machine and try recompiling another
        library. Make sure that the other library does not do a complete
        recompile. If it does, you have messed up the cheating and will
        need to redo it. Do not release base back to the server until
        it passes this test.
    13. Release base back to the server
*/

#ifndef _BASE_DIAG_HPP
#define _BASE_DIAG_HPP

#include "base/base.hpp"

#include <iostream>
#include <fstream>

enum DiagStreamType {
	DIAG_BOB,
	DIAG_HAL,
	DIAG_IAIN,
	DIAG_JERRY,
	DIAG_RICHARD,
	DIAG_JON,
	DIAG_YUEAI,
    DIAG_LIONEL,
    DIAG_CERI,
    DIAG_NEIL,

	DIAG_MISC,
	DIAG_WHERE,
	DIAG_TRACE,
	DIAG_MEMORY,
	DIAG_RENDER,

    DIAG_PER_READ,
    DIAG_PER_WRITE,

	DIAG_NETWORK,

    DIAG_WAYNE,

    DIAG_DANIEL,
    DIAG_MOTSEQ,    // MachLogMotionSequencer
    DIAG_CS2PATH,   // Config space pathfinding stuff (domain and normal)
    DIAG_CS2VGRA,   // Visibility graph diagnostics
	DIAG_OBSERVER,	// trapping observer pattern problems - stream added to due volume of output

    DIAG_PATH_PROFILE,    // Debugging the path profile across the planet surface

    DIAG_SOUND,

    DIAG_REC_RECORD,
    DIAG_REC_PLAYBACK,

	DIAG_NET_ANALYSIS,

    DIAG_A_STAR,

    DIAG_LINEAR_TRAVEL,     // Stream for use by PhysLinearTravelPlan

	DIAG_PLANET_SURFACE,	// Used for reporting errors in a planet surface
    DIAG_PHYS_MOTION,       // Physical motion stream

    DIAG_NETWORK_ERRORS,       // Specifically to help us spot network errors (such as loss of synchronicity)

    DIAG_MEX_GRID2D,

    DIAG_CONFIG_SPACE,

    //  Extra streams should be added immediately before this line

    DIAG_N_DIAG_STREAMS          //  Do not use this value in any code. It is liable to change without warning or recompiles
	};

//  This has to be included after DiagStreamType
#include "base/private/diagint.hpp"

#ifdef PRODUCTION
    #define DEBUG_STREAM( type, exp )
#else
    #define DEBUG_STREAM( type, exp ) DiagInternal::streamEnabled( type ) ? ( Diag::instance().stream( type ) << exp ), 0 : 1
#endif

#ifdef PRODUCTION
    #define DEBUG_INDENT( type, nSpaces )
#else
    #define DEBUG_INDENT( type, nSpaces ) DiagInternal::streamEnabled( type ) ? ( Diag::instance().indent( type, nSpaces ) ), 0 : 1
#endif

#ifdef PRODUCTION
    #define DEBUG_STREAM_ENABLED( type )    false
#else
    #define DEBUG_STREAM_ENABLED( type )    ( DiagInternal::streamEnabled( type ) )
#endif

#ifdef PRODUCTION
    #define DEBUG_STREAM_ENABLE( type )
    #define DEBUG_STREAM_DISABLE( type )
#else
    #define DEBUG_STREAM_ENABLE( type )    ( DiagInternal::enableStream( type ) )
    #define DEBUG_STREAM_DISABLE( type )   ( DiagInternal::disableStream( type ) )
#endif

//  Please use the macros wherever possible

#define	BOB_STREAM( exp )               DEBUG_STREAM( DIAG_BOB, exp )
#define	BOB_WHERE                       BOB_STREAM( WHERE_STR << std::endl )
#define BOB_INSPECT( var )              BOB_STREAM( #var " :\t" << (var) << std::endl )
#define BOB_INDENT( nSpaces )           DEBUG_INDENT( DIAG_BOB, nSpaces )
#define BOB_STREAM_ENABLED              DEBUG_STREAM_ENABLED( DIAG_BOB )
#define BOB_STREAM_ENABLE               DEBUG_STREAM_ENABLE( DIAG_BOB )
#define BOB_STREAM_DISABLE              DEBUG_STREAM_DISABLE( DIAG_BOB )

#define	HAL_STREAM( exp )               DEBUG_STREAM( DIAG_HAL, exp )
#define	HAL_WHERE                       HAL_STREAM( WHERE_STR << std::endl )
#define HAL_INSPECT( var )              HAL_STREAM( #var " :\t" << (var) << std::endl )
#define HAL_INDENT( nSpaces )           DEBUG_INDENT( DIAG_HAL, nSpaces )
#define HAL_STREAM_ENABLED              DEBUG_STREAM_ENABLED( DIAG_HAL )
#define HAL_STREAM_ENABLE               DEBUG_STREAM_ENABLE( DIAG_HAL )
#define HAL_STREAM_DISABLE              DEBUG_STREAM_DISABLE( DIAG_HAL )

#define	IAIN_STREAM( exp )              DEBUG_STREAM( DIAG_IAIN, exp )
#define IAIN_WHERE                      IAIN_STREAM( WHERE_STR << std::endl )
#define IAIN_INSPECT( var )             IAIN_STREAM( #var " :\t" << (var) << std::endl )
#define IAIN_INDENT( nSpaces )          DEBUG_INDENT( DIAG_IAIN, nSpaces )
#define IAIN_STREAM_ENABLED             DEBUG_STREAM_ENABLED( DIAG_IAIN )
#define IAIN_STREAM_ENABLE              DEBUG_STREAM_ENABLE( DIAG_IAIN )
#define IAIN_STREAM_DISABLE             DEBUG_STREAM_DISABLE( DIAG_IAIN )

#define	JERRY_STREAM( exp )             DEBUG_STREAM( DIAG_JERRY, exp )
#define JERRY_WHERE                     JERRY_STREAM( WHERE_STR << std::endl )
#define JERRY_INSPECT( var )            JERRY_STREAM( #var " :\t" << (var) << std::endl )
#define JERRY_INDENT( nSpaces )         DEBUG_INDENT( DIAG_JERRY, nSpaces )
#define JERRY_STREAM_ENABLED            DEBUG_STREAM_ENABLED( DIAG_JERRY )
#define JERRY_STREAM_ENABLE             DEBUG_STREAM_ENABLE( DIAG_JERRY )
#define JERRY_STREAM_DISABLE            DEBUG_STREAM_DISABLE( DIAG_JERRY )

#define	JON_STREAM( exp )               DEBUG_STREAM( DIAG_JON, exp )
#define JON_WHERE                       JON_STREAM( WHERE_STR << std::endl )
#define JON_INSPECT( var )              JON_STREAM( #var " :\t" << (var) << std::endl )
#define JON_INDENT( nSpaces )           DEBUG_INDENT( DIAG_JON, nSpaces )
#define JON_STREAM_ENABLED              DEBUG_STREAM_ENABLED( DIAG_JON )
#define JON_STREAM_ENABLE               DEBUG_STREAM_ENABLE( DIAG_JON )
#define JON_STREAM_DISABLE              DEBUG_STREAM_DISABLE( DIAG_JON )

#define	RICHARD_STREAM( exp )           DEBUG_STREAM( DIAG_RICHARD, exp )
#define RICHARD_WHERE                   RICHARD_STREAM( WHERE_STR << std::endl )
#define RICHARD_INSPECT( var )          RICHARD_STREAM( #var " :\t" << (var) << std::endl )
#define RICHARD_INDENT( nSpaces )       DEBUG_INDENT( DIAG_RICHARD, nSpaces )
#define RICHARD_STREAM_ENABLED          DEBUG_STREAM_ENABLED( DIAG_RICHARD )
#define RICHARD_STREAM_ENABLE           DEBUG_STREAM_ENABLE( DIAG_RICHARD )
#define RICHARD_STREAM_DISABLE          DEBUG_STREAM_DISABLE( DIAG_RICHARD )

#define	YUEAI_STREAM( exp )             DEBUG_STREAM( DIAG_YUEAI, exp )
#define YUEAI_WHERE                     YUEAI_STREAM( WHERE_STR << std::endl )
#define YUEAI_INSPECT( var )            YUEAI_STREAM( #var " :\t" << (var) << std::endl )
#define YUEAI_INDENT( nSpaces )         DEBUG_INDENT( DIAG_YUEAI, nSpaces )
#define YUEAI_STREAM_ENABLED            DEBUG_STREAM_ENABLED( DIAG_YUEAI )
#define YUEAI_STREAM_ENABLE             DEBUG_STREAM_ENABLE( DIAG_YUEAI )
#define YUEAI_STREAM_DISABLE            DEBUG_STREAM_DISABLE( DIAG_YUEAI )

#define	LIONEL_STREAM( exp )            DEBUG_STREAM( DIAG_LIONEL, exp )
#define LIONEL_WHERE                    LIONEL_STREAM( WHERE_STR << std::endl )
#define LIONEL_INSPECT( var )           LIONEL_STREAM( #var " :\t" << (var) << std::endl )
#define LIONEL_INDENT( nSpaces )        DEBUG_INDENT( DIAG_LIONEL, nSpaces )
#define LIONEL_STREAM_ENABLED           DEBUG_STREAM_ENABLED( DIAG_LIONEL )
#define LIONEL_STREAM_ENABLE            DEBUG_STREAM_ENABLE( DIAG_LIONEL )
#define LIONEL_STREAM_DISABLE           DEBUG_STREAM_DISABLE( DIAG_LIONEL )

#define	CERI_STREAM( exp )              DEBUG_STREAM( DIAG_CERI, exp )
#define CERI_WHERE                      CERI_STREAM( WHERE_STR << std::endl )
#define CERI_INSPECT( var )             CERI_STREAM( #var " :\t" << (var) << std::endl )
#define CERI_INDENT( nSpaces )          DEBUG_INDENT( DIAG_CERI, nSpaces )
#define CERI_STREAM_ENABLED             DEBUG_STREAM_ENABLED( DIAG_CERI )
#define CERI_STREAM_ENABLE              DEBUG_STREAM_ENABLE( DIAG_CERI )
#define CERI_STREAM_DISABLE             DEBUG_STREAM_DISABLE( DIAG_CERI )

#define	NEIL_STREAM( exp )              DEBUG_STREAM( DIAG_NEIL, exp )
#define NEIL_WHERE                      NEIL_STREAM( WHERE_STR << std::endl )
#define NEIL_INSPECT( var )             NEIL_STREAM( #var " :\t" << (var) << std::endl )
#define NEIL_INDENT( nSpaces )          DEBUG_INDENT( DIAG_NEIL, nSpaces )
#define NEIL_STREAM_ENABLED             DEBUG_STREAM_ENABLED( DIAG_NEIL )
#define NEIL_STREAM_ENABLE              DEBUG_STREAM_ENABLE( DIAG_NEIL )
#define NEIL_STREAM_DISABLE             DEBUG_STREAM_DISABLE( DIAG_NEIL )

#define	WAYNE_STREAM( exp )             DEBUG_STREAM( DIAG_WAYNE, exp )
#define WAYNE_WHERE                     WAYNE_STREAM( WHERE_STR << std::endl )
#define WAYNE_INSPECT( var )            WAYNE_STREAM( #var " :\t" << (var) << std::endl )
#define WAYNE_INDENT( nSpaces )         DEBUG_INDENT( DIAG_WAYNE, nSpaces )
#define WAYNE_STREAM_ENABLED            DEBUG_STREAM_ENABLED( DIAG_WAYNE )
#define WAYNE_STREAM_ENABLE             DEBUG_STREAM_ENABLE( DIAG_WAYNE )
#define WAYNE_STREAM_DISABLE            DEBUG_STREAM_DISABLE( DIAG_WAYNE )

#define DANIEL_STREAM( exp )            DEBUG_STREAM( DIAG_DANIEL, exp )
#define DANIEL_WHERE                    DANIEL_STREAM( WHERE_STR << std::endl )
#define DANIEL_INSPECT( var )           DANIEL_STREAM( #var " :\t" << (var) << std::endl )
#define DANIEL_INDENT( nSpaces )        DEBUG_INDENT( DIAG_DANIEL, nSpaces )
#define DANIEL_STREAM_ENABLED           DEBUG_STREAM_ENABLED( DIAG_DANIEL )
#define DANIEL_STREAM_ENABLE            DEBUG_STREAM_ENABLE( DIAG_DANIEL )
#define DANIEL_STREAM_DISABLE           DEBUG_STREAM_DISABLE( DIAG_DANIEL )

#define	MISC_STREAM( exp )              DEBUG_STREAM( DIAG_MISC, exp )
#define MISC_WHERE                      MISC_STREAM( WHERE_STR << std::endl )
#define MISC_INSPECT( var )             MISC_STREAM( #var " :\t" << (var) << std::endl )
#define MISC_INDENT( nSpaces )          DEBUG_INDENT( DIAG_MISC, nSpaces )
#define MISC_STREAM_ENABLED             DEBUG_STREAM_ENABLED( DIAG_MISC )
#define MISC_STREAM_ENABLE              DEBUG_STREAM_ENABLE( DIAG_MISC )
#define MISC_STREAM_DISABLE             DEBUG_STREAM_DISABLE( DIAG_MISC )

#define	WHERE_STREAM( exp )             DEBUG_STREAM( DIAG_WHERE, exp )
#define WHERE_WHERE                     WHERE_STREAM( WHERE_STR << std::endl )
#define WHERE_INSPECT( var )            WHERE_STREAM( #var " :\t" << (var) << std::endl )
#define WHERE_INDENT( nSpaces )         DEBUG_INDENT( DIAG_WHERE, nSpaces )
#define WHERE_STREAM_ENABLED            DEBUG_STREAM_ENABLED( DIAG_WHERE )
#define WHERE_STREAM_ENABLE             DEBUG_STREAM_ENABLE( DIAG_WHERE )
#define WHERE_STREAM_DISABLE            DEBUG_STREAM_DISABLE( DIAG_WHERE )

#define	TRACE_STREAM( exp )             DEBUG_STREAM( DIAG_TRACE, exp )
#define TRACE_WHERE                     TRACE_STREAM( WHERE_STR << std::endl )
#define TRACE_INSPECT( var )            TRACE_STREAM( #var " :\t" << (var) << std::endl )
#define TRACE_INDENT( nSpaces )         DEBUG_INDENT( DIAG_TRACE, nSpaces )
#define TRACE_STREAM_ENABLED            DEBUG_STREAM_ENABLED( DIAG_TRACE )
#define TRACE_STREAM_ENABLE             DEBUG_STREAM_ENABLE( DIAG_TRACE )
#define TRACE_STREAM_DISABLE            DEBUG_STREAM_DISABLE( DIAG_TRACE )

#define	MEMORY_STREAM( exp )            DEBUG_STREAM( DIAG_MEMORY, exp )
#define MEMORY_WHERE                    MEMORY_STREAM( WHERE_STR << std::endl )
#define MEMORY_INSPECT( var )           MEMORY_STREAM( #var " :\t" << (var) << std::endl )
#define MEMORY_INDENT( nSpaces )        DEBUG_INDENT( DIAG_MEMORY, nSpaces )
#define MEMORY_STREAM_ENABLED           DEBUG_STREAM_ENABLED( DIAG_MEMORY )
#define MEMORY_STREAM_ENABLE            DEBUG_STREAM_ENABLE( DIAG_MEMORY )
#define MEMORY_STREAM_DISABLE           DEBUG_STREAM_DISABLE( DIAG_MEMORY )

#define	RENDER_STREAM( exp )            DEBUG_STREAM( DIAG_RENDER, exp )
#define RENDER_WHERE                    RENDER_STREAM( WHERE_STR << std::endl )
#define RENDER_INSPECT( var )           RENDER_STREAM( #var " :\t" << (var) << std::endl )
#define RENDER_INDENT( nSpaces )        DEBUG_INDENT( DIAG_RENDER, nSpaces )
#define RENDER_STREAM_ENABLED           DEBUG_STREAM_ENABLED( DIAG_RENDER )
#define RENDER_STREAM_ENABLE            DEBUG_STREAM_ENABLE( DIAG_RENDER )
#define RENDER_STREAM_DISABLE           DEBUG_STREAM_DISABLE( DIAG_RENDER )

#define	NETWORK_STREAM( exp )           DEBUG_STREAM( DIAG_NETWORK, exp )
#define NETWORK_WHERE                   NETWORK_STREAM( WHERE_STR << std::endl )
#define NETWORK_INSPECT( var )          NETWORK_STREAM( #var " :\t" << (var) << std::endl )
#define NETWORK_INDENT( nSpaces )       DEBUG_INDENT( DIAG_NETWORK, nSpaces )
#define NETWORK_STREAM_ENABLED          DEBUG_STREAM_ENABLED( DIAG_NETWORK )
#define NETWORK_STREAM_ENABLE           DEBUG_STREAM_ENABLE( DIAG_NETWORK )
#define NETWORK_STREAM_DISABLE          DEBUG_STREAM_DISABLE( DIAG_NETWORK )

#define	PER_READ_STREAM( exp )          DEBUG_STREAM( DIAG_PER_READ, exp )
#define PER_READ_WHERE                  PER_READ_STREAM( WHERE_STR << std::endl )
#define PER_READ_INSPECT( var )         PER_READ_STREAM( #var " :\t" << (var) << std::endl )
#define PER_READ_INDENT( nSpaces )      DEBUG_INDENT( DIAG_PER_READ, nSpaces )
#define PER_READ_STREAM_ENABLED         DEBUG_STREAM_ENABLED( DIAG_PER_READ )
#define PER_READ_STREAM_ENABLE          DEBUG_STREAM_ENABLE( DIAG_PER_READ )
#define PER_READ_STREAM_DISABLE         DEBUG_STREAM_DISABLE( DIAG_PER_READ )

#define	PER_WRITE_STREAM( exp )         DEBUG_STREAM( DIAG_PER_WRITE, exp )
#define PER_WRITE_WHERE                 PER_WRITE_STREAM( WHERE_STR << std::endl )
#define PER_WRITE_INSPECT( var )        PER_WRITE_STREAM( #var " :\t" << (var) << std::endl )
#define PER_WRITE_INDENT( nSpaces )     DEBUG_INDENT( DIAG_PER_WRITE, nSpaces )
#define PER_WRITE_STREAM_ENABLED        DEBUG_STREAM_ENABLED( DIAG_PER_WRITE )
#define PER_WRITE_STREAM_ENABLE         DEBUG_STREAM_ENABLE( DIAG_PER_WRITE )
#define PER_WRITE_STREAM_DISABLE        DEBUG_STREAM_DISABLE( DIAG_PER_WRITE )

#define MOTSEQ_STREAM( exp )            DEBUG_STREAM( DIAG_MOTSEQ, exp )
#define MOTSEQ_WHERE                    MOTSEQ_STREAM( WHERE_STR << std::endl )
#define MOTSEQ_INSPECT( var )           MOTSEQ_STREAM( #var " :\t" << (var) << std::endl )
#define MOTSEQ_INDENT( nSpaces )        DEBUG_INDENT( DIAG_MOTSEQ, nSpaces )
#define MOTSEQ_STREAM_ENABLED           DEBUG_STREAM_ENABLED( DIAG_MOTSEQ )
#define MOTSEQ_STREAM_ENABLE            DEBUG_STREAM_ENABLE( DIAG_MOTSEQ )
#define MOTSEQ_STREAM_DISABLE           DEBUG_STREAM_DISABLE( DIAG_MOTSEQ )

#define CS2PATH_STREAM( exp )           DEBUG_STREAM( DIAG_CS2PATH, exp )
#define CS2PATH_WHERE                   CS2PATH_STREAM( WHERE_STR << std::endl )
#define CS2PATH_INSPECT( var )          CS2PATH_STREAM( #var " :\t" << (var) << std::endl )
#define CS2PATH_INDENT( nSpaces )       DEBUG_INDENT( DIAG_CS2PATH, nSpaces )
#define CS2PATH_STREAM_ENABLED          DEBUG_STREAM_ENABLED( DIAG_CS2PATH )
#define CS2PATH_STREAM_ENABLE           DEBUG_STREAM_ENABLE( DIAG_CS2PATH )
#define CS2PATH_STREAM_DISABLE          DEBUG_STREAM_DISABLE( DIAG_CS2PATH )

#define CS2VGRA_STREAM( exp )           DEBUG_STREAM( DIAG_CS2VGRA, exp )
#define CS2VGRA_WHERE                   CS2VGRA_STREAM( WHERE_STR << std::endl )
#define CS2VGRA_INSPECT( var )          CS2VGRA_STREAM( #var " :\t" << (var) << std::endl )
#define CS2VGRA_INDENT( nSpaces )       DEBUG_INDENT( DIAG_CS2VGRA, nSpaces )
#define CS2VGRA_STREAM_ENABLED          DEBUG_STREAM_ENABLED( DIAG_CS2VGRA )
#define CS2VGRA_STREAM_ENABLE           DEBUG_STREAM_ENABLE( DIAG_CS2VGRA )
#define CS2VGRA_STREAM_DISABLE          DEBUG_STREAM_DISABLE( DIAG_CS2VGRA )

#define OBSERVER_STREAM( exp )          DEBUG_STREAM( DIAG_OBSERVER, exp )
#define OBSERVER_WHERE                  OBSERVER_STREAM( WHERE_STR << std::endl )
#define OBSERVER_INSPECT( var )         OBSERVER_STREAM( #var " :\t" << (var) << std::endl )
#define OBSERVER_INDENT( nSpaces )      DEBUG_INDENT( DIAG_OBSERVER, nSpaces )
#define OBSERVER_STREAM_ENABLED         DEBUG_STREAM_ENABLED( DIAG_OBSERVER )
#define OBSERVER_STREAM_ENABLE          DEBUG_STREAM_ENABLE( DIAG_OBSERVER )
#define OBSERVER_STREAM_DISABLE         DEBUG_STREAM_DISABLE( DIAG_OBSERVER )

#define PATH_PROFILE_STREAM( exp )      DEBUG_STREAM( DIAG_PATH_PROFILE, exp )
#define PATH_PROFILE_WHERE              PATH_PROFILE_STREAM( WHERE_STR << std::endl )
#define PATH_PROFILE_INSPECT( var )     PATH_PROFILE_STREAM( #var " :\t" << (var) << std::endl )
#define PATH_PROFILE_INDENT( nSpaces )  DEBUG_INDENT( DIAG_PATH_PROFILE, nSpaces )
#define PATH_PROFILE_STREAM_ENABLED     DEBUG_STREAM_ENABLED( DIAG_PATH_PROFILE )
#define PATH_PROFILE_STREAM_ENABLE      DEBUG_STREAM_ENABLE( DIAG_PATH_PROFILE )
#define PATH_PROFILE_STREAM_DISABLE     DEBUG_STREAM_DISABLE( DIAG_PATH_PROFILE )

#define SOUND_STREAM( exp )             DEBUG_STREAM( DIAG_SOUND, exp )
#define SOUND_WHERE                     SOUND_STREAM( WHERE_STR << std::endl )
#define SOUND_INSPECT( var )            SOUND_STREAM( #var " :\t" << (var) << std::endl )
#define SOUND_INDENT( nSpaces )         DEBUG_INDENT( DIAG_SOUND, nSpaces )
#define SOUND_STREAM_ENABLED            DEBUG_STREAM_ENABLED( DIAG_SOUND )
#define SOUND_STREAM_ENABLE             DEBUG_STREAM_ENABLE( DIAG_SOUND )
#define SOUND_STREAM_DISABLE            DEBUG_STREAM_DISABLE( DIAG_SOUND )

#define REC_RECORD_STREAM( exp )        DEBUG_STREAM( DIAG_REC_RECORD, exp )
#define REC_RECORD_WHERE                REC_RECORD_STREAM( WHERE_STR << std::endl )
#define REC_RECORD_INSPECT( var )       REC_RECORD_STREAM( #var " :\t" << (var) << std::endl )
#define REC_RECORD_INDENT( nSpaces )    DEBUG_INDENT( DIAG_REC_RECORD, nSpaces )
#define REC_RECORD_STREAM_ENABLED       DEBUG_STREAM_ENABLED( DIAG_REC_RECORD )
#define REC_RECORD_STREAM_ENABLE        DEBUG_STREAM_ENABLE( DIAG_REC_RECORD )
#define REC_RECORD_STREAM_DISABLE       DEBUG_STREAM_DISABLE( DIAG_REC_RECORD )

#define REC_PLAYBACK_STREAM( exp )      DEBUG_STREAM( DIAG_REC_PLAYBACK, exp )
#define REC_PLAYBACK_WHERE              REC_PLAYBACK_STREAM( WHERE_STR << std::endl )
#define REC_PLAYBACK_INSPECT( var )     REC_PLAYBACK_STREAM( #var " :\t" << (var) << std::endl )
#define REC_PLAYBACK_INDENT( nSpaces )  DEBUG_INDENT( DIAG_REC_PLAYBACK, nSpaces )
#define REC_PLAYBACK_STREAM_ENABLED     DEBUG_STREAM_ENABLED( DIAG_REC_PLAYBACK )
#define REC_PLAYBACK_STREAM_ENABLE      DEBUG_STREAM_ENABLE( DIAG_REC_PLAYBACK )
#define REC_PLAYBACK_STREAM_DISABLE     DEBUG_STREAM_DISABLE( DIAG_REC_PLAYBACK )

#define NET_ANALYSIS_STREAM( exp )      DEBUG_STREAM( DIAG_NET_ANALYSIS, exp )
#define NET_ANALYSIS_WHERE              NET_ANALYSIS_STREAM( WHERE_STR << std::endl )
#define NET_ANALYSIS_INSPECT( var )     NET_ANALYSIS_STREAM( #var " :\t" << (var) << std::endl )
#define NET_ANALYSIS_INDENT( nSpaces )  DEBUG_INDENT( DIAG_NET_ANALYSIS, nSpaces )
#define NET_ANALYSIS_STREAM_ENABLED     DEBUG_STREAM_ENABLED( DIAG_NET_ANALYSIS )
#define NET_ANALYSIS_STREAM_ENABLE      DEBUG_STREAM_ENABLE( DIAG_NET_ANALYSIS )
#define NET_ANALYSIS_STREAM_DISABLE     DEBUG_STREAM_DISABLE( DIAG_NET_ANALYSIS )

#define A_STAR_STREAM( exp )            DEBUG_STREAM( DIAG_A_STAR, exp )
#define A_STAR_WHERE                    A_STAR_STREAM( WHERE_STR << std::endl )
#define A_STAR_INSPECT( var )           A_STAR_STREAM( #var " :\t" << (var) << std::endl )
#define A_STAR_INDENT( nSpaces )        DEBUG_INDENT( DIAG_A_STAR, nSpaces )
#define A_STAR_STREAM_ENABLED           DEBUG_STREAM_ENABLED( DIAG_A_STAR )
#define A_STAR_STREAM_ENABLE            DEBUG_STREAM_ENABLE( DIAG_A_STAR )
#define A_STAR_STREAM_DISABLE           DEBUG_STREAM_DISABLE( DIAG_A_STAR )

#define LINEAR_TRAVEL_STREAM( exp )     DEBUG_STREAM( DIAG_LINEAR_TRAVEL, exp )
#define LINEAR_TRAVEL_WHERE             LINEAR_TRAVEL_STREAM( WHERE_STR << std::endl )
#define LINEAR_TRAVEL_INSPECT( var )    LINEAR_TRAVEL_STREAM( #var " :\t" << (var) << std::endl )
#define LINEAR_TRAVEL_INDENT( nSpaces ) DEBUG_INDENT( DIAG_LINEAR_TRAVEL, nSpaces )
#define LINEAR_TRAVEL_STREAM_ENABLED    DEBUG_STREAM_ENABLED( DIAG_LINEAR_TRAVEL )
#define LINEAR_TRAVEL_STREAM_ENABLE     DEBUG_STREAM_ENABLE( DIAG_LINEAR_TRAVEL )
#define LINEAR_TRAVEL_STREAM_DISABLE    DEBUG_STREAM_DISABLE( DIAG_LINEAR_TRAVEL )

#define PLANET_SURFACE_STREAM( exp )     DEBUG_STREAM( DIAG_PLANET_SURFACE, exp )
#define PLANET_SURFACE_WHERE             PLANET_SURFACE_STREAM( WHERE_STR << std::endl )
#define PLANET_SURFACE_INSPECT( var )    PLANET_SURFACE_STREAM( #var " :\t" << (var) << std::endl )
#define PLANET_SURFACE_INDENT( nSpaces ) DEBUG_INDENT( DIAG_PLANET_SURFACE, nSpaces )
#define PLANET_SURFACE_STREAM_ENABLED    DEBUG_STREAM_ENABLED( DIAG_PLANET_SURFACE )
#define PLANET_SURFACE_STREAM_ENABLE     DEBUG_STREAM_ENABLE( DIAG_PLANET_SURFACE )
#define PLANET_SURFACE_STREAM_DISABLE    DEBUG_STREAM_DISABLE( DIAG_PLANET_SURFACE )

#define PHYS_MOTION_STREAM( exp )     DEBUG_STREAM( DIAG_PHYS_MOTION, exp )
#define PHYS_MOTION_WHERE             PHYS_MOTION_STREAM( WHERE_STR << std::endl )
#define PHYS_MOTION_INSPECT( var )    PHYS_MOTION_STREAM( #var " :\t" << (var) << std::endl )
#define PHYS_MOTION_INDENT( nSpaces ) DEBUG_INDENT( DIAG_PHYS_MOTION, nSpaces )
#define PHYS_MOTION_STREAM_ENABLED    DEBUG_STREAM_ENABLED( DIAG_PHYS_MOTION )
#define PHYS_MOTION_STREAM_ENABLE     DEBUG_STREAM_ENABLE( DIAG_PHYS_MOTION )
#define PHYS_MOTION_STREAM_DISABLE    DEBUG_STREAM_DISABLE( DIAG_PHYS_MOTION )

#define	NETWORK_ERRORS_STREAM( exp )           DEBUG_STREAM( DIAG_NETWORK_ERRORS, exp )
#define NETWORK_ERRORS_WHERE                   NETWORK_ERRORS_STREAM( WHERE_STR << std::endl )
#define NETWORK_ERRORS_INSPECT( var )          NETWORK_ERRORS_STREAM( #var " :\t" << (var) << std::endl )
#define NETWORK_ERRORS_INDENT( nSpaces )       DEBUG_INDENT( DIAG_NETWORK_ERRORS, nSpaces )
#define NETWORK_ERRORS_STREAM_ENABLED          DEBUG_STREAM_ENABLED( DIAG_NETWORK_ERRORS )
#define NETWORK_ERRORS_STREAM_ENABLE           DEBUG_STREAM_ENABLE( DIAG_NETWORK_ERRORS )
#define NETWORK_ERRORS_STREAM_DISABLE          DEBUG_STREAM_DISABLE( DIAG_NETWORK_ERRORS )

#define MEX_GRID2D_STREAM( exp )      DEBUG_STREAM( DIAG_MEX_GRID2D, exp )
#define MEX_GRID2D_WHERE              MEX_GRID2D_STREAM( WHERE_STR << std::endl )
#define MEX_GRID2D_INSPECT( var )     MEX_GRID2D_STREAM( #var " :\t" << (var) << std::endl )
#define MEX_GRID2D_INDENT( nSpaces )  DEBUG_INDENT( DIAG_MEX_GRID2D, nSpaces )
#define MEX_GRID2D_STREAM_ENABLED     DEBUG_STREAM_ENABLED( DIAG_MEX_GRID2D )
#define MEX_GRID2D_STREAM_ENABLE      DEBUG_STREAM_ENABLE( DIAG_MEX_GRID2D )
#define MEX_GRID2D_STREAM_DISABLE     DEBUG_STREAM_DISABLE( DIAG_MEX_GRID2D )

#define CONFIG_SPACE_STREAM( exp )      DEBUG_STREAM( DIAG_CONFIG_SPACE, exp )
#define CONFIG_SPACE_WHERE              CONFIG_SPACE_STREAM( WHERE_STR << std::endl )
#define CONFIG_SPACE_INSPECT( var )     CONFIG_SPACE_STREAM( #var " :\t" << (var) << std::endl )
#define CONFIG_SPACE_INDENT( nSpaces )  DEBUG_INDENT( DIAG_CONFIG_SPACE, nSpaces )
#define CONFIG_SPACE_STREAM_ENABLED     DEBUG_STREAM_ENABLED( DIAG_CONFIG_SPACE )
#define CONFIG_SPACE_STREAM_ENABLE      DEBUG_STREAM_ENABLE( DIAG_CONFIG_SPACE )
#define CONFIG_SPACE_STREAM_DISABLE     DEBUG_STREAM_DISABLE( DIAG_CONFIG_SPACE )

// class DiagStreamData;
class DiagStream;

using std::ostream;

class Diag
{
public:

    //  Try and set up the instance of Diag before doing any
    //  memory allocation or debugging output. Note that this
    //  is done automatically by the AFX library.

    static Diag& instance();

    ostream& stream( DiagStreamType );

    void indent( DiagStreamType, int nSpaces );

    //  People's own streams
	ostream& bobStream();
	ostream& halStream();
	ostream& iainStream();
	ostream& jerryStream();
	ostream& jonStream();
	ostream& richardStream();
	ostream& yueaiStream();
	ostream& lionelStream();
	ostream& ceriStream();
	ostream& neilStream();
	ostream& wayneStream();
    ostream& danielStream();

	ostream& miscStream();

	ostream& whereStream();
	ostream& traceStream();
	ostream& memoryStream();



    //  Library streams
	ostream& renderStream();
	ostream& networkStream();

    ostream& motseqStream();
    ostream& cs2pathStream();
    ostream& cs2vgraStream();
    ostream& observerStream();

	ostream& perReadStream();
	ostream& perWriteStream();

	ostream& pathProfileStream();

	ostream& soundStream();

	ostream& recRecordStream();
	ostream& recPlaybackStream();

	ostream& netAnalysisStream();
	ostream& aStarStream();
	ostream& linearTravelStream();

	ostream& planetSurfaceStream();
	ostream& physMotionStream();

	ostream& networkErrorsStream();

	ostream& mexGrid2dStream();
	ostream& configSpaceStream();

	const char*	getenv( const char* name );

    //  Set / get the invariant checking flag.
    void    invariantCheckingActive( bool flag );
    bool    invariantCheckingActive() const;

    //  Force a crash and a crash.log to be written
    void    forceCrash() const;

    typedef void (*PFn)();
    //  Add a function to the list of functions to be
    //  called should a crash or an assertion occur.
    void addCrashFunction( PFn );

    // Add / remove a stream to the list of streams to be closed
    // should a crash or an assertion occur
    void addCrashStream( std::ofstream& );
    void removeCrashStream( std::ofstream& );

    void disableFPException();
    void enableFPException();

private:
    Diag();
    ~Diag();

    // Operation deliberately revoked
    Diag( const Diag& );

    // Operation deliberately revoked
    Diag& operator =( const Diag& );

    // Operation deliberately revoked
    bool operator ==( const Diag& );
};

// Commented out because they do not currently work - Bob 28 Oct 1998

// //  Put the disable / enable macros around any third
// //  party functions that cause floating point exceptions
//
// #ifdef PRODUCTION
//     #define DIAG_DISABLE_FP_EXCEPTION
//     #define DIAG_ENABLE_FP_EXCEPTION
// #else
//     #define DIAG_DISABLE_FP_EXCEPTION Diag::instance().disableFPException()
//     #define DIAG_ENABLE_FP_EXCEPTION Diag::instance().enableFPException()
// #endif

#endif

/* End DIAG.HPP *****************************************************/

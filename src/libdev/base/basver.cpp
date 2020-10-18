/*
* B A S V E R . C P P
* (c) Charybdis Limited, 1995. All Rights Reserved
*/

/*
    Mission statement:
    
    Obtain a flat language across different compiler / hardware platforms.
    
    Add general diagnostic and debugging functions
*/

/*
	HISTORY

	01.00	First Release
			20 October 1995  Bob Archer
            
    01.01   HeapPtr class added
			23 October 1995  Bob Archer

    01.02   watc1000.hpp and watc1050.hpp moved to the base library from clibplus
            Uses and HeapPtr classes moved to the system library
            11 December 1995  Bob Archer
            
    01.03   The implies macro has been rewritten so that it only evaluates
            its second argument if its first argument is true.

            If you are using the PRE and POSTcondition macros, you will
            also need to include the stdlib library.

            The memory debugging code has been moved into here.
            
            The environment variable 'memlog' can now be used to control
            the memory debugging information that is output.

            There is now an ASSERT_FILE_EXISTS macro which prints out the
            name of the file if it doesn't exist.
            23 February 1996    Bob Archer
            
    01.04   suppress.hpp added. This contains code necessary to suppress
            warnings given by inadequate compilers.
            6 March 1996    Bob Archer

    01.05   DbgMemChkCheckAllMemory added to memory checking code. This
            will check the header blocks of all memory allocated for
            corruption.
            4 April 1996    Bob Archer
    
	01.06	Template files rearranged into new standard format
            ( .ctp, .itp, .ctf and .itf files )
            19 May 1996                         Bob Archer

	01.07	Introduced mutual exclusion on ASSERTion testing and firing.
            13 June 1996                         Jerry Swan

    01.08   More memory allocation data is now saved and output.
            DbgMemChkWriteMemoryAllocationData added to memchk.hpp
            16 June 1996                         Bob Archer

    01.09   Added DbgMemChkMask and  DbgMemChkLogAllAllocatedMemory
            to allow regions of code to be checked for memory leaks.
            20 June 1996                         Bob Archer

    01.10   DbgMemChkData class added.
            DbgMemChkAllocationData function added
            20 June 1996                         Bob Archer

    01.11   ASSERT_FILE_EXISTS now works in the release version of
            the library. It does not halt the program if the file
            does not exists but it does print a warning to cout
            and cerr.
            29 June 1996                         Bob Archer

    01.12   ASSERT_FILE_EXISTS now just returns in the release
            version of the library.
            DbgMemChkRegisterFileData is now only called for _DELETE
            and _DELETE_ARRAY if the pointers being deleted are not 0.
            The delete and delete[] operators are not called if the
            pointer is zero which was giving incorrect file data.
            
            15th July 1996                      Bob Archer

    01.13   All assertion code moved from stdlib to base. Use of
            assert now deprecated, ASSERT should be used instead.
            PRE_INFO, POST_INFO & ASSERT_INFO added.
            ASSERT_ALWAYS renamed to ASSERT_FAIL    
            BaseAssertion class added largely to act as a namespace
            for the various assertion functions.
            Invariants are now included if the _TEST_INVARIANTS
            symbol is set, this replaces the old _STRICT symbol.
            
            6th August 1996                     Bob Archer

    01.14   BaseLogBuffer added
            19th September 1996                 Bob Archer

    01.15   #define for if( 0 ) ; else for
    
            added to allow Watcom to simulate the new for scope rules.
            
            #pragma warning 368 9;
            
            also added to disable "conditional expression in if
            statement is always false" warning 

            30th September 1996                 Bob Archer

    01.16   The CLASS_INVARIANT macro has had the void removed from the
            front of it so that it can be used in the .hpp file and in
            the .cpp file. Version 01.08 of genclass will use the macro
            correctly.
            
            The ASSERT_FILE_EXISTS call will now check that the pathname
            it is given is a valid DOS pathname. If it isn't, it will fail
            with an invalid pathname error rather than a file not found error.
            
            15th October 1996                   Bob Archer

    01.17   Added a virtual destructor to BaseLogStreamBuffer so that the
            destructors of derived classes get called correctly.
            4th November 1996                   Bob Archer

    01.18   The AssertionAction names FAIL and IGNORE have been changed to
            ASSERT_FAIL and ASSERT_IGNORE to stop clashes with windows
            #defines.

    01.19   Disabled "'sizeof' operand contains compiler generated
            information" warning
            9th December 1996                   Bob Archer

    01.20   BaseLogBuffer now has dataAsString function to get the
            data back as a string.
            23rd January 1997                   Bob Archer

    01.21   When an assertion fails, the abort function is called to
            allow the failure to be trapped by the debugger. Previously
            the exit function was being used which the debugger was taking
            as normal program termination.
            27th February 1997                  Bob Archer

    01.22   ASSERT_FILE_EXISTS( "" ) will now fail.
            14th March 1997                  Bob Archer

    01.23   #define explicit added to base.hpp to allow the explicit
            keyword to be used even though it isn't supported by the
            compiler yet.
            ASSERT_BAD_CASE_INFO macro added.
            19th March 1997                     Bob Archer

    01.24   compiler.hpp will now set up a macro to indicate the operating
            system that is being used. One of _DOSAPP or _WIN95APP will
            be defined.

    01.25   Bug in ASSERT_FILE_EXISTS fixed so that filenames containing
            the '..' sequence are correctly tested. Previously they would
            always give an illegal file name assertion.
            5th April 1997                      Bob Archer

    01.26   Tracing code added.
            BaseDebugTimer added.
            13th July 1997                      Bob Archer

    01.27   Removed the definition of ZERO from base.hpp. Removed isValid
            for pointers and references.
            Disable "assuming unary 'operator &' not overloaded for type" warning.
            Diagnostic streams added - see diag.hpp            
            The memchk environment variable has now been renamed the cb_memory
            environment variable.
            Increased the number of saved _INFO entries from 20 to 50.
            WHERE_LOG removed.
            less_ptr added. This should be used for comparisons on pointers.
            MEMCHK environment variable changed to CB_MEMORY environment variable.
            25th July 1997                      Bob Archer

    01.28   DbgMemCheckBlockValid function now just returns true. The call to this
            in _DELETE should really be taken out but this will cause a complete
            recompile.
            5th August 1997                      Bob Archer

    01.29   The list of memory blocks that have not been freed now includes
            information about whether each block was allocated with _NEW or
            _NEW_ARRAY. Persistence code added.
            25th August 1997                    Bob Archer

    01.30   DbgMemCheckBlockValid removed completely.
            BaseDebugTimer made faster - it now uses a uint64 class to save
            the value of the pentium timer,. This is only converted to a
            double when requested. The fastest way to use this is therefore
            to save lots of BaseDebugTimer::Time variables and only print
            them out when necessary.

    01.31   DEFAULT_ASSERT_BAD_CASE( exp ) macro added.
            BaseDebugTimer moved into the Utl library. Display pentium time
            function taken out of the debugging stream.
            Using the persistence mechanism to write out a non const scalar
            type now works correctly - previously it gave an ambiguous
            function error.
            27th October 1997                   Bob Archer

    01.32   The new and delete calls in DiagOstreamPrepend and
            BaseAppendOstream have been changed so they don't show up as
            memory errors on program exit. This also means that setting
            cb_memory=logall works properly and does not crash owing to
            recursion.
            2nd November 1997                   Bob Archer

    01.33   simonStream and SIMON_STREAM added
            4th November 1997                   Bob Archer

    01.34   yueaiStream and YUEAI_STREAM added
            Assert.log now puts the date and time at the end of the
            file as well as at the beginning.
            13th November 1997                   Bob Archer

    01.35   Proper output operator written for DbgMemChkData
            10th December 1997                  Bob Archer

    01.36   Invariants are now turned on by default. #define
            _NO_TEST_INVARIANTS to stop them being called.
            28th December 1997                  Bob Archer

    01.37   ASSERT_COMPILE_TIME macro added to prepost.hpp. Bugfix
            with memory stream to stop it chopping off the end of the file.
            9th January 1998                    Bob Archer

    01.38   DiagStream class added.
            13th January 1998                   Bob Archer

    01.39   Fixed a problem with declaring template classes persistent.
            The various << and >> operators were being declared inline
            leading to problems when they weren't defined in the same
            translation unit.
            15th January 1998                   Bob Archer

    01.40   Moved UtlIndentOstream and UtlFilterOstream into base so that
            they can be used by the persistence debugging code. Typedefs
            exist in the utility library to avoid breaking code.
            11th February 1998                   Bob Archer

    01.41   New version of persistence code.
            16th February 1998                  Bob Archer

    01.42   New version of persistence code.
            SIMON_STREAM removed.
            CERI_STREAM added.
            Timing code added to persistence read.
            Invariant checking can now be turned on and off using
            the CB_INVARIENT_CHECKING environment variable (default is on).
            Invariant checking can also be controlled using the 
            Diag::instance().invariantCheckingActive functions.
            Counted pointer checking is now off by default but can be turned
            on using CB_COUNTED_POINTER_CHECKING.
            5th March 1998                      Bob Archer

    01.43   Persistence added for enumerated types
            10th March 1998                     Bob Archer

    01.43a  NON_INTRUSIVE persistence macros added to allow persistence
            to be "bolted onto" classes where we can't get at the hpp
            30th March 1998                     Bob Archer

    01.44   DEBUG_STREAM speed increased when the stream isn't active
            7th April 1998                      Bob Archer

    01.45   Initialisation order problems fixed. These were leading to
            false memory leaks on tracker.cpp.
            Optional stack tracing for memory allocations added.
            17th April 1998                     Bob Archer

    01.46   xxx_WHERE and xx_INSPECT macros added to diag.hpp to help
            with debugging.
            21st April 1998

	01.47	DIAG_NETWORK added.
			22nd April 1998						Hal Angseesing.

    01.48   On an assertion the stack trace is now written out to assert.log
            Persistence optimisations added.
            8th May 1998                        Bob Archer

    01.49   Signal handling code added to Diag so that out of range memory
            accesses are trapped and crach.log file written containing stack
            traceback.
            The signal handling code also closes all debug streams that are
            open thus avoiding the problem of debug streams not being flushed
            on a memory error.
            10th May 1998                       Bob Archer

    01.50   Bug fixed in persistence writing code which stopped objects being
            logged as written in the release version.
            19th May 1998                       Bob Archer

    01.51   DiagInternal::signalHandler now closes Profiler output stream.
            20th May 1998                       Bob Archer

    01.52   When an assertion fails all output streams are now closed.
            8th June 1998                       Bob Archer

    01.53   Whenever a debugging stream is closed under control it is marked
            as such. This is to make it easy to spot when debugging information
            has been lost.
            The option to send debug output to the recdbg window has been added.
            Set "CB_xxxx_TO=window" to achieve this.
            12th June 1998                      Bob Archer

    01.54   BaseDualOstream has been added. This should not be used directly - the
            utility library alias should be used instead.
            15th June 1998                      Bob Archer

    01.55   WAYNE_STREAM added
            3 July 1998                         Bob Archer

    01.56   DANIEL_STREAM added
            MOTSEQ_STREAM added
            CS2PATH_STREAM added
            CS2VGRA_STREAM added
            debugstr.hpp is now directly included in diag.hpp
            24 July 1998                       Bob Archer

    01.57   OBSERVER_STREAM added
            25 August 1998                       Hal Angseesing

    01.58   PATH_PROFILE_STREAM added
            2 September 1998                    Bob Archer

    01.59   The InfoEntry outout operator now correctly uses pcount
            to write out the correct number of characters from the
            internal data buffer. Before it was possible to overrun
            the buffer and write out the data following.
            4 September 1998                    Bob Archer

    01.60   diag.hpp tidied up. All macros added for all debug
            streams. xxx_INDENT macros added to allow indentation
            of a debugging stream.
            10 September 1998                   Bob Archer

    01.61   Added a new environment variable CB_ASSERT_TO=filename may now be used to override
			the default file name for assert.log - if the env var. has not been defined then
			it will default to assert.log as normal.
            11 September 1998                   Hal Angseesing

    01.62   Added a new environment variable CB_CRASH_TO=filename may now be used to override
			the default file name for crash.log - if the env var. has not been defined then
			it will default to crash.log as normal.
            11 September 1998                   Hal Angseesing

    01.63   All diagnostic streams now have a xxxx_STREAM_ENABLED macro available.
            DIAG_SOUND disgnostic stream added.
            14 September 1998                   Bob Archer

    01.64   Diag::forceCrash now calls abort
            17 September 1998                   Bob Archer

    01.65   The floating point precision for the xxxx_INFO streams 
            is now set to 18 decimal places.
            18 September 1998                   Bob Archer

    01.66   xxxx_STREAM_ENABLED macros now collapse to false for
            the production version
            20 September 1998                   Bob Archer

    01.67   REC_RECORD_STREAM and REC_PLAYBACK_STREAM added
            7 October 1998                      Bob Archer

    01.68   It is now possible to specify functions to be called 
            if the system crashes or asserts out using Diag::addCrashFunction.
            7 October 1998                      Bob Archer

    01.69
            static  bool BaseAssertion::inAssertion();
            static  bool BaseAssertion::inAssertionInfo();
            static  bool BaseAssertion::inAssertionData(); all added
            12 October 1998                     Bob Archer

    01.70   Improved assertion failure messages in persistence code.
            14 October 1998                     Bob Archer

    01.71   Removed the following from the header files:
                PerIstream( const char* fileName );
                PerOstream( const char* fileName );
                
            The functions were not implemented which caused link
            errors. They now cause a compile error
            15 October 1998                     Bob Archer

    01.72   Fixed a bug whereby xxx_INFO data were not always written out
            21 October 1998                     Bob Archer

    01.73   If the environment variable "CB_FP_EXCEPTION" is set to anything
            then floating point exceptions will be checked for.
            This should not be used at the moment as there are some DirectX
            calls which trigger this.
            28 October 1998                     Bob Archer

    01.74   Added NET_ANALYSIS stream which (when active) will output some useful
			numbers from various networking libraries.
            28 October 1998                     Hal Angseesing

    01.75   A_STAR diagnostic stream added
            30 October 1998                     Bob Archer

    01.76   LINEAR_TRAVEL diagnostic stream added
            6 November 1998                     Bob Archer

    01.77   If more than 200 characters were written to one of the ASSERT_INFO
            streams, the stream would be put into a fail state. This means
            that the next time we tried writing to it the write would fail
            i.e. once one of the buffers had had 200 characters written any
            more information put in there using xxxx_INFO would not overwrite
            the existing information. The state of the stream is now cleared
            before it is written to.
            6 November 1998                     Bob Archer

    01.78   Added Diag::addCrashStream and Diag::removeCrashStream functions
            to allow user defined streams to be automatically closed on crash
            or assertions.
            9 November 1998                    Bob Archer

    01.79   The DEBUG_INDENT function had a bug in whereby the '\0' at the end
            of the stream was not correctly allowed for. This caused some
            memory to be overwritten when we reached an indent level of 20
            spaces. This bug has now been fixed.
            PerReporter class added to allow persistence reading to be
            progress monitored. By creating a class derived from PerReporter
            a client get can updates on the persistence read.
            18 November 1998                    Bob Archer

    01.80   The persistence code now asserts out if you try and havbe more than
            one PerOstream or PerIstream open at once.
            21 November 1998                    Bob Archer


    01.81   Added the PLANET_SURFACE stream - this stream should be used for all validation 
			results on the planet surface.
            15 December 1998                    Hal Angseesing

    01.82   PHYS_MOTION_STREAM added for tracking details of the physical motion.
            15 Jan 1999                         Bob Archer

    01.83   xxxx_STREAM_ENABLE and xxxx_STREAM_DISABLE macros added for all streams. 
            These allow stream output to be enabled or disabled. 
            Note:
                The CB_xxxx_TO environment variable must have been set up
                The stream will be enabled by default
                It is not possible to change the destination of a stream
            1 Feb 1999                          Bob Archer

    01.84   Added the NETWORK_ERRORS stream - this stream should be used specifically for network errors
			(loss of synchronicity and not for general networking).
            1 Feb 1999                    Hal Angseesing

    01.85   Added MEX_GRID2D_STREAM (a class so error prone it deserves 
            stream of its own)
            11 Feb 1999                         Bob Archer

    01.86   Added CONFIG_SPACE_STREAM
            12 Feb 1999                         Bob Archer
*/

//////////////////////////////////////////////////////////////////////

#define	LIBRARY	"BAS"
#define	VERSION	"01.86"

static unsigned char verString[] = "$:VERSION:$ " LIBRARY "  " VERSION;
static unsigned char dateString[] = "$:DATETIME:$ " LIBRARY "  " __DATE__ " " __TIME__;

//////////////////////////////////////////////////////////////////////

#include <stdio.h>

void printVersion()
{
	printf( "%s\n", verString );
	printf( "%s\n", dateString );
}

//////////////////////////////////////////////////////////////////////

/* End BASVER.CPP ***************************************************/
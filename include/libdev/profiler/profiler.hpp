/*
 * P R O F I L E R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    ProProfiler

    Allow the profiling to be set up.
*/

#ifndef _PROFILER_PROFILER_HPP
#define _PROFILER_PROFILER_HPP

#include "base/base.hpp"

class BaseLogBuffer;

//TODO: handle timer
static size_t	count;
static size_t	call_stack;

static uint32  next_update_time_ls;
static uint32  next_update_time_ms;
static uint32  time_increment_ls;
static uint32  time_increment_ms;

static bool   profiling_enabled;

class ProProfiler
// Canonical form revoked
{
public:
    //  Singleton class
    static ProProfiler& instance( void );
    ~ProProfiler( void );

    //  The default trace interval is 50ms
    void    traceInterval( double timeSeconds );
    //  PRE( not traceIntervalFixed() );

    //  Profiling is disabled by default

    void    enableProfiling();
    //  POST( traceIntervalFixed() );
    void    disableProfiling();

    //true iff profiling is enabled
    bool isProfilingEnabled() const;

    //Set/get the memory profiling flag. Purely a convenient flag for client use.
    //Has no effect. Defaults to false.
    void isMemoryProfilingEnabled( bool isEnabled );
    bool isMemoryProfilingEnabled() const;

    //  The trace interval cannot be changed once
    //  profiling has been enabled
    bool    traceIntervalFixed() const;

    //  Set the next time a trace will be done as an offset
    //  from the current time
    void    nextTraceTime( double offsetSeconds );

    ostream& outputStream();
    void    closeOutputStream();

    //void    writeStack( ostream& ) const;

    //Prints the current call stack in standard form to outStream.
    //If doTraceAnchor is true, the name of the anchor function and its address is prepended.
    //The line number is added. (Zero implies not known).
    //If extraString is non empty, it is added.
    void traceStack( ostream& outStream, bool doTraceAnchor, ulong lineNumber, const char* extraString );

    //Export the address of the call stack
    const size_t* pCallStack() const;

    //Current number of entries in the call stack
    size_t nCallStackEntries() const;

    //Trace the supplied call stack pCallStack with nStackFrames frames to outStream
    void traceStack( ostream& outStream, bool doTraceAnchor, size_t nStackFrames, const size_t* pCallStack,
                     ulong lineNumber, const char* extraString );

    //Enable/disable/test whether all calls to traceStack are written to an internal buffer instead
    //of designated stream
    void isBufferingOutput( bool is );
    bool isBufferingOutput() const;

    //Clear the internal buffer
    void clearBuffer();
    //PRE( isBufferingOutput() );

    //Flush the internal buffer to outStream
    void writeBuffer( ostream& outStream );
    //PRE( isBufferingOutput() );

    //Set to induce acrash on a call to print the stack when profiling.
    //Used to help trap hanging bugs.
    void crashOnPrint( bool crash );
    bool crashOnPrint();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const ProProfiler& t );

private:
    ProProfiler( const ProProfiler& );
    ProProfiler& operator =( const ProProfiler& );
    bool operator ==( const ProProfiler& );

    ProProfiler( void );

    void    setupTraceInterval() const;
    void    calibrate();

    friend void ProProfilerAnchor();
    void	registerAnchor( const char* anchorFunctionName );

    //  Convert a double value into a 64 bit integer value
    void    doubleToUint64( double value, uint32* pLs, uint32* pMs ) const;

    double      traceIntervalSeconds_;
    double      ticksPerSecond_;
    bool        traceIntervalFixed_;
    std::ofstream    outputStream_;
    bool        memoryProfilingOn_;

    //  Stored as a char* so I don't have to pull in the string class
    char*       anchorFunctionName_;
    void*       anchorFunctionAddress_;

    bool isBufferingOutput_;
    BaseLogBuffer* pMemoryBuffer_;
    bool crashOnPrint_; //true if a crash should be induced on call from EPI/PRO to print the stack
    long unsigned int threadId;
};


#endif

/* End PROFILER.HPP *************************************************/

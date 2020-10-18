/*
 * S T K T R A C E . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    ProStackTracer

    This class exists purely to write out the stack. Because the class
    has an output operator it can be used in the normal way with the
    debugging stream macros i.e.:
    
        BOB_STREAM( ProStackTracer() );
    
    The advantage of this over:
    
        ProProfiler::instance().traceStack( Diag::instance().bobStream(), true, 0, "" );

    Is that the stack trace will only be calculated if the stream is active
*/

#ifndef _PROFILER_STKTRACE_HPP
#define _PROFILER_STKTRACE_HPP

#include "base/base.hpp"

class ProStackTracer
// Canonical form revoked
{
public:
    ProStackTracer( void );
    ~ProStackTracer( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const ProStackTracer& t );

    ProStackTracer( const ProStackTracer& );
    ProStackTracer& operator =( const ProStackTracer& );

};


#endif

/* End STKTRACE.HPP *************************************************/

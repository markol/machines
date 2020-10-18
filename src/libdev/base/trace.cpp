/*
 * T R A C E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>
#include <stdio.h>

#include "base/base.hpp"

#include "base/private/trace.hpp"

// static
BaseTrace& BaseTrace::instance()
{
    static BaseTrace instance_;
    return instance_;
}

BaseTrace::BaseTrace()
: trace_( false )
{
    const char* pC = getenv( "CB_TRACE" );

    if( pC )
    {
        trace_ = true;
    }

}

BaseTrace::~BaseTrace()
{
}

void BaseTrace::trace( bool flag )
{
    trace_ = flag;
}

void    BaseTrace::log( const char* fileName, size_t line )
{
    Diag::instance().traceStream() << fileName << ", " << line << std::endl;
}


/* End TRACE.CPP ****************************************************/

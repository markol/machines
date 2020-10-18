/*
 * S T K T R A C E . C P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "profiler/stktrace.hpp"
#include "profiler/profiler.hpp"

ProStackTracer::ProStackTracer()
{

    TEST_INVARIANT;
}

ProStackTracer::~ProStackTracer()
{
    TEST_INVARIANT;

}

void ProStackTracer::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const ProStackTracer& )
{
    ProProfiler::instance().traceStack( o, true, 0, "" );

    return o;
}

/* End STKTRACE.CPP *************************************************/

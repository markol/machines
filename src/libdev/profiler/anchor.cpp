/*
 * A N C H O R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//	This function must be in a separate file so that it doesn't
//  get inlined, and the file must be compiled with the -ep (call
//  prolog hook routine) and -ee (call epilog hook routine)
//  options enabled.

#include "profiler/profiler.hpp"

void ProProfilerAnchor()
{
	//	The text passed here must match the name of the function

	ProProfiler::instance().registerAnchor( "ProProfilerAnchor" );
}

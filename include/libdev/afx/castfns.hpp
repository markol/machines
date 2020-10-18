//
// C A S T F N S . H P P
// (c) Charybdis Limited, 1996. All Rights Reserved
//
// These functions depend on the name of the most-derived application
// class.  Hence, this file *must* be compiled by the client, and is
// *not* compiled into the framework library.  Include it in one
// client cpp file, so it will get compiled into the program.  (The
// framework library itself references these functions, so they are
// definitely necessary.)

#include "afx/osapp.hpp"

#ifndef FRAMEWORK_OSAPP
#error Use of the framework lib: FRAMEWORK_OSAPP must be defined (this should be done by osapp.hpp).
#endif

// static
AfxApp& AfxApp::abstractInstance()
{
	return MOST_DERIVED_APP::instance();
}

#if defined _DOSAPP
  #define METHOD_NAME dosInstance
#elif defined _WIN95APP
  #define METHOD_NAME win95Instance
#elif defined _SDLAPP
  #define METHOD_NAME sdlInstance
#else
  #error Use of the framework lib: one of DOSAPP, WIN95APP, etc. must be defined.
#endif

// static
FRAMEWORK_OSAPP& FRAMEWORK_OSAPP::METHOD_NAME()
{
	return MOST_DERIVED_APP::instance();
}

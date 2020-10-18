//
// O S A P P . H P P
// (c) Charybdis Limited, 1996. All Rights Reserved
//
// The framework isolates client applications from OS-specific
// considerations.  Here are the nuts-and-bolts of the mechanism
// for selecting the target platform.  A client needs to:
//
//       1) Define one of WIN95APP, DOSAPP.
//   2) Define MOST_DERIVED_APP to be the name of your app class
//      (the one written at step 4 below).
//       3) Include this file - no other *app*.hpp files are required.
//       4) Derive a concrete class from AfxOSSpecificApp<T>.
//   5) With the above macros in force, include castfns.hpp in one
//      and only one of your .cpp files.  (Without it you'll get
//      undefined references to the functions in castfns.hpp).
//       6) Don't write a main or WinMain - they're both in the lib.

#ifndef _FRAMEWORK_OSAPP_INCLUDED
#define _FRAMEWORK_OSAPP_INCLUDED

//  Base now sets up _DOSAPP or _WIN95APP

#include "base/base.hpp"

#include "afx/singapp.hpp"

#ifndef MOST_DERIVED_APP
  #error Use of the framework lib: you must define MOST_DERIVED_APP.
#endif

// Select the sub-class appropriate for the platform specified by the
// client code.
#if defined _SDLAPP
  #include "afx/sdlapp.hpp"
  #define FRAMEWORK_OSAPP AfxSdlApp
#elif defined _DOSAPP
  #include "afx/dosapp.hpp"
  #define FRAMEWORK_OSAPP AfxDosApp
#elif defined _WIN95APP
  #include "afx/win95app.hpp"
  #define FRAMEWORK_OSAPP AfxWin95App
#else
  #error Use of the framework lib: one of DOSAPP, WIN95APP, etc. must be defined.
#endif

// Derive your application class from this.  The macros above ensure
// that the name of this class is the same, regardless of the target OS.
// The template parameter DERIVED_APP ought to be the same as the macro
// MOST_DERIVED_APP; I can't think of a way to verify this.
template < class DERIVED_APP >
class AfxOSSpecificApp :
        public  FRAMEWORK_OSAPP,                                // a #define above
        protected AfxSingletonApp<DERIVED_APP>            // prevent casting to this type
{
public:
        AfxSingletonApp<DERIVED_APP>::instance;
};

#endif /* _FRAMEWORK_OSAPP_INCLUDED */

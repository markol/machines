//
// A P P . H P P 
// (c) Charybdis Limited, 1996. All Rights Reserved
//
// An AfxApp object represents a running application.  It provides 
// consistent access to the environment of program invocation, 
// regardless of the actual operating system (OS) which started 
// the application.  Thus, client sub-classes are isolated from 
// certain OS dependencies.  The AfxApp class is "in charge of" the 
// program, in the sense that the top-most flow-of-control is the 
// responsibility of this class and clients do not have a 
// traditional main function.
//
// See the file osapp.hpp for instructions on using the library.

#ifndef _FRAMEWORK_BASE_APP_INCLUDED
#define _FRAMEWORK_BASE_APP_INCLUDED

//#include <string.hpp>
#include "stdlib/string.hpp"
#include "base/base.hpp"
#include "afx/invkargs.hpp"

// Note: WinMain is deliberately *not* a friend of this class
// because:
//   1) If it were, we need to drag in lots of Windows header
//      files to define all the crappy Windows data types.
//      Without them, the compilation will be faster.
//   2) This code ought to compile on a machine without the
//      Windows header files (this goes for any OS actually).
class AfxApp
{
public:
        // 8-bit values
        enum ExitStatus
        {
                EXIT_OK         = 0x00,
                USER_BREAK      = 0x01
                // etc
        };

        // This class's raison d'etre: run the application.
        ExitStatus run();
        virtual void finish() = 0;
        virtual bool isFinished() const = 0;
        
        // AfxApplications can provide a name.  This is useful for window titles.       
        virtual const string& name() const;

        // Get the arguments that were supplied at program invocation.
        // This does *not* include the program name as the first element
        // (a la DOS & Unix) because it does not seem to be available
        // under Windows95.
        const AfxInvokeArgs& invokeArgs() const { return args_; }

        static AfxApp& abstractInstance();

        // For testing, this takes the place of an output library.
        virtual void testPrint(const char*) const = 0;

protected:
        AfxApp();
        virtual ~AfxApp();

        // Hooks for OS-specific and client-specific parts of 
        // initialisation and termination.
        virtual bool OSStartup ()     = 0;
        virtual void OSShutdown()     = 0;
        virtual void loopCycle()      = 0;
        virtual bool clientStartup () = 0;
        virtual void clientShutdown() = 0;
        
        // A hook via which the AFX requests that the client redraws its display.
        // This occurs when the app is exposed under a windowing system.  Non-
        // windowed (DOS) sub-classes are required to provide this functionality
        // just for the sake of consistency.
        virtual void updateDisplay() = 0;

        ExitStatus exitStatus() const;
        void exitStatus( ExitStatus );

        void setArgs(const AfxInvokeArgs&);

private:
        // These are called only by this class from the run method.
        // The default implementations call the client and OS specific
        // start-up and shutdown methods.
        virtual bool startup ();
        virtual void coreLoop()  = 0;
        virtual void shutdown();
        
        // This is a hook which is called when clientStartup has succeeded.
        virtual void clientStarted()  {}

        ExitStatus      exitStatus_;
        AfxInvokeArgs   args_;
};

#endif //_FRAMEWORK_BASE_APP_INCLUDED


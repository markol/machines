/*
 * D I A G I N T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
	Diagnostic code used internally by the base library. This class
    breaks a number of the normal rules regarding public data members
    and static variables. This is because access to these variables
    needs to be very efficient as they are used to turn off diagnostics
    that have a significant performance impact on the code.
*/

#ifndef _BASE_DIAGINT_HPP
#define _BASE_DIAGINT_HPP

#include <stddef.h>
#include <iostream>

#include "base/base.hpp"

class DiagPointerTracker;

using std::ostream;

class DiagInternal
{
public:

	static	bool	streamEnabled( DiagStreamType );
    static  void    enableStream( DiagStreamType );
    static  void    disableStream( DiagStreamType );

	static	size_t	nStreams();
	static	const char*	streamText( size_t index );

	static	ostream& stream( DiagStreamType type );

    static  DiagPointerTracker&	countedPointerTracker();

    static  void setDestination( DiagStreamType type, const char* dest );

    static  bool checkFpException();

    static  void disableFPException();
    static  void enableFPException();

    //  Data members
    static  bool    checkCountedPointers_;
	static	bool	trackAllObjects_;
	static	bool	checkInvariants_;

private:
	DiagInternal();
	~DiagInternal();

    friend class Diag;

    static  void    initialiseSignalHandler( void );
    static  void    signalHandler( int sig );
};

#endif

/* End DIAGINT.HPP *****************************************************/

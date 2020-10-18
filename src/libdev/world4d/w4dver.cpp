/*
 * R E N V E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
	HISTORY

	00.01	Spun off from Render library
			15 May 1997  					Bob Archer

	00.02	Added W4dSubject and W4dObserver classes to support observer pattern.
            Modified to use PhysRelative/AbsoluteTime instead of PhysTime.
			13 Aug 1997  					Jon Green
            
*/

#include <stdio.h>

#define	LIBRARY	"W4D"
#define	VERSION	"00.02"

static unsigned char verString[] = "$:VERSION:$ " LIBRARY "  " VERSION;
static unsigned char dateString[] = "$:DATETIME:$ " LIBRARY "  " __DATE__ " " __TIME__;

void printVersion( void )
{
    printf( "%s\n", verString );
    printf( "%s\n", dateString );    
}

/* End CTLVER.CPP ***************************************************/

/*
 * M A C V E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
	HISTORY

	00.90	Development started
			24 March 1997  		Bob Archer
	00.91	MachPhysData classes now started to being used.
			01/09/1997			Hal Angseesing
	00.92	The new sub-classes from MachPhys have been incorrporated.
			All of the main operantions have also been implemented.
			07/11/1997			Hal Angseesing
*/

// machine must be in initial position before
// MotionSeq ctor is called

// rename copyofdales.pob to dales.psf
// rename dales/textures.old to dales/textures

//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#define	LIBRARY	"MAC"
#define	VERSION	"00.92"

static unsigned char verString[] = "$:VERSION:$ " LIBRARY "  " VERSION;
static unsigned char dateString[] = "$:DATETIME:$ " LIBRARY "  " __DATE__ " " __TIME__;

void printVersion()
{
    printf( "%s\n", verString );
    printf( "%s\n", dateString );    
}

/* End DEVVER.CPP ***************************************************/
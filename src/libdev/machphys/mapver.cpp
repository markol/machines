/*
 * M A P V E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
	HISTORY

	00.90	Development started
			24 March 1997  		Bob Archer

	00.91	Modified to use PhysRelative/AbsoluteTime
			13 Aug 1997  		Jon Green

	00.92	Modified to use MachPhysData classes
			Constructor on MachMachine altered to pass in body level
			which is stored. This should be purely internal and not
			affect any other clients.
			21 Aug 1997  		Hal Angseesing
*/

//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#define	LIBRARY	"MAP"
#define	VERSION	"00.92"

static unsigned char verString[] = "$:VERSION:$ " LIBRARY "  " VERSION;
static unsigned char dateString[] = "$:DATETIME:$ " LIBRARY "  " __DATE__ " " __TIME__;

void printVersion( void )
{
    printf( "%s\n", verString );
    printf( "%s\n", dateString );    
}

/* End DEVVER.CPP ***************************************************/
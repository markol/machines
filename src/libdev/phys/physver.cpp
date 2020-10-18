/*
* P H Y S V E R . C P P
* (c) Charybdis Limited, 1997. All Rights Reserved
*/

/*
	HISTORY

	01.00	First Release
			14 March 1997  Jonathan Green

	01.01	Replaced PhysTime with PhysRelativeTime and PhysAbsoluteTime
			13 Aug 1997  Jonathan Green
*/

#define	LIBRARY	"PHYS"
#define	VERSION	"01.01"

#pragma off(unreferenced);

static unsigned char verString[] = "$:VERSION:$ " LIBRARY "  " VERSION;
static unsigned char dateString[] = "$:DATETIME:$ " LIBRARY "  " __DATE__ " " __TIME__;

#pragma on(unreferenced);

void printVersion()
{
}

/* End PHYSVER.CPP ***************************************************/
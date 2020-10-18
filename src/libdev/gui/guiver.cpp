/*
 * G U I V E R . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/* //////////////////////////////////////////////////////////////// */

/*
	HISTORY

	01.00	First Release
			8 November 1997		Jerry Swan

	01.01	

	Removed GuiContainer and made GuiDisplayable the base class.
	Made GuiDisplayable dtor protected to enforce ( as far as possible ) 
		that subclasses be heap objects.
	Coord passed to doHandleMouseXXX are now relative, rather than absolute.
	Keyboard and mouse clicks events now originate from the Afx Event queue.
	Rewrote the insertion processing for GuiResourceString.
	Implemented "Windows style" button behaviour.
	Enforced the containment of MachGui classes within parent boundaries.
	Replaced the recursive contains etc methods with iteration.
	3 Febuary 1998		Jerry Swan

	01.02
	Fixed a bug in GuiDisplayable::absoluteCoord( const Gui::Coord& ) that
	positioned children incorrectly.
	5 Febuary 1998		Jerry Swan
 */

/* //////////////////////////////////////////////////////////////// */

#define	LIBRARY	"GUI"
#define	VERSION	"01.02"

/* //////////////////////////////////////////////////////////////// */

#pragma off(unreferenced);

static unsigned char verString[] = "$:VERSION:$ " LIBRARY "  " VERSION;
static unsigned char dateString[] = "$:DATETIME:$ " LIBRARY "  " __DATE__ " " __TIME__;

#pragma on(unreferenced);

void printVersion()
{
}

/* //////////////////////////////////////////////////////////////// */

/* End GUIVER.CPP ***************************************************/

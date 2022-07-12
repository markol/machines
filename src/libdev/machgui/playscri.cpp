/*
 * P L A Y S C R I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/playscri.hpp"
#include "machgui/internal/strings.hpp"

MachGuiPlayerScore::MachGuiPlayerScore( GuiDisplayable *pParent, const Gui::Coord& topLeft )
:	MachGuiPlayerNameList( pParent, Gui::Boundary( topLeft, 135, 77 ), "gui/menu/largefnt.bmp", IDS_MENU_STSSCORE , true ),
   	statDisplay_( pParent, Gui::Coord( topLeft.x()+59, topLeft.y()+35 ), MachPhys::N_RACES )
{

    TEST_INVARIANT;
}

MachGuiPlayerScore::~MachGuiPlayerScore()
{
    TEST_INVARIANT;

}

// virtual
void MachGuiPlayerScore::update()
{
	if( statDisplay_.redraw() )
	{
		statDisplay_.update();
	}
}

// virtual
void MachGuiPlayerScore::setStatistics( int stat1, int stat2, int stat3, int stat4 )
{
	statDisplay_.setStatistics( stat1, stat2, stat3, stat4 );	
}

void MachGuiPlayerScore::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiPlayerScore& t )
{

    o << "MachGuiPlayerScore " << (void*)&t << " start" << std::endl;
    o << "MachGuiPlayerScore " << (void*)&t << " end" << std::endl;

    return o;
}

/* End PLAYSCRI.CPP *************************************************/

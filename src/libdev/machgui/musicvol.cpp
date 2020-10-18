/*
 * M U S I C V O L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/musicvol.hpp"
#include "device/cd.hpp"

unsigned char pPromptTextKey7[] = { 203, 32, 138, 78, 52, 240, 106, 192, 132, 20, 149, 70, 38, 8, 247, 202, 203, 195, 70, 99, 65, 142, 88, 15, 98, 242, 234, 237, 147, 254, 13, 5, 4, 131, 232, 193, 76, 176, 161, 210, 108, 74, 124, 200, 9, 33, 87, 13, 143, 161, 193, 228, 174 };

MachGuiMusicVolumeSlideBar::MachGuiMusicVolumeSlideBar( MachGuiStartupScreens* pStartupScreens, GuiDisplayable* pParent, Gui::Coord topLeft, size_t width )
:	MachGuiSlideBar( pStartupScreens, pParent, topLeft, width, 0, 100 )
{
   	if ( DevCD::instance().supportsVolumeControl() )
	{
		size_t theVol =	DevCD::instance().volume();
		RICHARD_STREAM("Volume initialized to " << theVol << std::endl);
		value( theVol );
	}

    TEST_INVARIANT;
}

MachGuiMusicVolumeSlideBar::~MachGuiMusicVolumeSlideBar()
{
    TEST_INVARIANT;

}

void MachGuiMusicVolumeSlideBar::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// virtual
void MachGuiMusicVolumeSlideBar::valueChanged( float value )
{
	RICHARD_STREAM("CD Vol changing with value " << value << std::endl)	;
   	if ( DevCD::instance().supportsVolumeControl() )
		DevCD::instance().volume( value + 0.5 /*stop rounding errors from slowly reducing volume*/ );
}

ostream& operator <<( ostream& o, const MachGuiMusicVolumeSlideBar& t )
{

    o << "MachGuiMusicVolumeSlideBar " << (void*)&t << " start" << std::endl;
    o << "MachGuiMusicVolumeSlideBar " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MUSICVOL.CPP *************************************************/

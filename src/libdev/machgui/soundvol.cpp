/*
 * S O U N D V O L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/soundvol.hpp"
#include "sound/soundmix.hpp"

MachGuiSoundVolumeSlideBar::MachGuiSoundVolumeSlideBar( MachGuiStartupScreens* pStartupScreens, GuiDisplayable* pParent, Gui::Coord topLeft, size_t width )
: 	MachGuiSlideBar( pStartupScreens, pParent, topLeft, width )
{
	WAYNE_STREAM( "Sound vol ctor initial volume " << SndMixer::instance().masterSampleVolume() << std::endl );
	Snd::Volume initialVolume = SndMixer::instance().masterSampleVolume();
	minMax( 0, 100 );
	value( initialVolume );

    TEST_INVARIANT;
}

MachGuiSoundVolumeSlideBar::~MachGuiSoundVolumeSlideBar()
{
    TEST_INVARIANT;
}

void MachGuiSoundVolumeSlideBar::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachGuiSoundVolumeSlideBar::valueChanged( float value )
{
	WAYNE_STREAM( "MachGuiSoundVolumeSlideBar::valueChanged: " <<  value << std::endl );
	SndMixer::instance().masterSampleVolume( value + 0.5 /*stop rounding errors from slowly reducing volume*/);
}

ostream& operator <<( ostream& o, const MachGuiSoundVolumeSlideBar& t )
{

    o << "MachGuiSoundVolumeSlideBar " << (void*)&t << " start" << std::endl;
    o << "MachGuiSoundVolumeSlideBar " << (void*)&t << " end" << std::endl;

    return o;
}

/* End SOUNDVOL.CPP *************************************************/

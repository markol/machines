/*
 * S O U N D V O L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiSoundVolumeSlideBar

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_SOUNDVOL_HPP
#define _MACHGUI_SOUNDVOL_HPP

#include "base/base.hpp"
#include "machgui/slidebar.hpp"

class MachGuiStartupScreens;

class MachGuiSoundVolumeSlideBar : public MachGuiSlideBar
// Canonical form revoked
{
public:
    MachGuiSoundVolumeSlideBar( MachGuiStartupScreens* pStartupScreens, GuiDisplayable* pParent, Gui::Coord topLeft, size_t width );
    ~MachGuiSoundVolumeSlideBar( void );

	// Sets new sound FX volume. Called when slide button is moved.
	virtual void valueChanged( float value );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiSoundVolumeSlideBar& t );

    MachGuiSoundVolumeSlideBar( const MachGuiSoundVolumeSlideBar& );
    MachGuiSoundVolumeSlideBar& operator =( const MachGuiSoundVolumeSlideBar& );
};


#endif

/* End SOUNDVOL.HPP *************************************************/

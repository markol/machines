/*
 * G A M M A C O R . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    MachGuiGammaCorrectionSlideBar

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_GAMMACOR_HPP
#define _MACHGUI_GAMMACOR_HPP

#include "base/base.hpp"
#include "machgui/slidebar.hpp"

class MachGuiStartupScreens;

class MachGuiGammaCorrectionSlideBar : public MachGuiSlideBar
// Canonical form revoked
{
public:
    MachGuiGammaCorrectionSlideBar( MachGuiStartupScreens* pStartupScreens, GuiDisplayable* pParent, Gui::Coord topLeft, size_t width, 
   				 					float minVal, float maxVal );
    ~MachGuiGammaCorrectionSlideBar();

    void CLASS_INVARIANT;

	virtual void valueChanged( float value );

private:
    friend ostream& operator <<( ostream& o, const MachGuiGammaCorrectionSlideBar& t );

    MachGuiGammaCorrectionSlideBar( const MachGuiGammaCorrectionSlideBar& );
    MachGuiGammaCorrectionSlideBar& operator =( const MachGuiGammaCorrectionSlideBar& );

};


#endif

/* End GAMMACOR.HPP *************************************************/

/*
 * G A M M A C O R . C P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/gammacor.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "render/display.hpp"
#include "render/device.hpp"

unsigned char pPromptTextKey4[] = { 232, 186, 25, 98, 183, 104, 237, 167, 239, 2, 162, 148, 53, 32, 245, 241, 119, 175, 189, 175, 48, 216, 19, 250, 184, 167, 252, 170, 21, 71, 133, 250, 60, 106, 0, 6, 176, 62, 106, 178, 116, 140, 23 };

MachGuiGammaCorrectionSlideBar::MachGuiGammaCorrectionSlideBar( MachGuiStartupScreens* pStartupScreens, GuiDisplayable* pParent, Gui::Coord topLeft, size_t width, 
   				 												float minVal, float maxVal )
:	MachGuiSlideBar( pStartupScreens, pParent, topLeft, width, minVal, maxVal )
{
	// Setup with current gamma correction setting
	RenDisplay* pDisplay = W4dManager::instance().sceneManager()->pDevice()->display();

	value( pDisplay->gammaCorrection() );
	
    TEST_INVARIANT;
}

MachGuiGammaCorrectionSlideBar::~MachGuiGammaCorrectionSlideBar()
{
    TEST_INVARIANT;

}

void MachGuiGammaCorrectionSlideBar::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiGammaCorrectionSlideBar& t )
{

    o << "MachGuiGammaCorrectionSlideBar " << (void*)&t << " start" << std::endl;
    o << "MachGuiGammaCorrectionSlideBar " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
void MachGuiGammaCorrectionSlideBar::valueChanged( float value )
{
	RenDisplay* pDisplay = W4dManager::instance().sceneManager()->pDevice()->display();

	pDisplay->gammaCorrection( value );
}

/* End GAMMACOR.CPP *************************************************/

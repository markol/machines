/*
 * T E X T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiText

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_TEXT_HPP
#define _MACHGUI_TEXT_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "gui/displaya.hpp"
#include "gui/font.hpp"

class MachGuiStartupScreens;

class MachGuiText : public GuiDisplayable
{
public:
	MachGuiText( GuiDisplayable* pParent, MachGuiStartupScreens* pStartupScreens, size_t width, const string& str );
	MachGuiText( GuiDisplayable* pParent, MachGuiStartupScreens* pStartupScreens, size_t width, const string& str, const string& font );

	static size_t reqHeight();
	
	virtual void doDisplay();

	void textOffset( int x, int y );

private:
	string text_;
	MachGuiStartupScreens* pStartupScreens_;
	int textOffsetX_;
	int textOffsetY_;
	GuiBmpFont font_;
};


#endif

/* End TEXT.HPP *****************************************************/

/*
 * S C R O L B A R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiVerticalScrollBar

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_SCROLBAR_HPP
#define _MACHGUI_SCROLBAR_HPP

#include "base/base.hpp"
#include "gui/scrolbar.hpp"

class MachGuiVerticalScrollBar : public GuiVerticalScrollBar
{
public:
	MachGuiVerticalScrollBar( GuiDisplayable* pParent, const Gui::Box& area, GuiSimpleScrollableList* pList );

	// Creates bar with two buttons. Bit of a hack really but...
	static void createWholeBar( GuiDisplayable* pParent, const Gui::Coord& topLeft, size_t height, GuiSimpleScrollableList* pList );

protected:
	virtual void doDisplayBar();
	virtual void doDisplayBox( const Gui::Box& absoluteBox );
};

#endif

/* End SCROLBAR.HPP *************************************************/

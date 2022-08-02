/*
 * T E X T D I S P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDisplayText

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_TEXTDISP_HPP
#define _MACHGUI_TEXTDISP_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"
#include "gui/font.hpp"

class MachGuiStartupScreens;

class MachGuiDisplayText : public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiDisplayText( MachGuiStartupScreens* pParent, const Gui::Box& box, unsigned int stringId, const GuiBmpFont& );
    ~MachGuiDisplayText();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiDisplayText& t );

    MachGuiDisplayText( const MachGuiDisplayText& );
    MachGuiDisplayText& operator =( const MachGuiDisplayText& );

	GuiBmpFont font_;
	unsigned int stringId_;
	MachGuiStartupScreens* pStartupScreens_;
};


#endif

/* End TEXTDISP.HPP *************************************************/

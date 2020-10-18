/*
 * S C R L T E X T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiScrollableText

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_SCRLTEXT_HPP
#define _MACHGUI_SCRLTEXT_HPP

#include "base/base.hpp"
#include "gui/scrolist.hpp"

class MachGuiStartupScreens;

class MachGuiScrollableText	: public GuiSimpleScrollableList
// Canonical form revoked
{
public:
    MachGuiScrollableText( 	MachGuiStartupScreens* pParent, const Gui::Box& );
    MachGuiScrollableText( 	MachGuiStartupScreens* pParent, const Gui::Box&, uint stringId );
	MachGuiScrollableText( 	MachGuiStartupScreens* pParent, const Gui::Box&, const string& text );
	MachGuiScrollableText( 	MachGuiStartupScreens* pParent, const Gui::Box&, uint columnWidth, const string& text );
	virtual ~MachGuiScrollableText( void );

	void setText( uint stringId );
	void setText( const string& );

	virtual void doDisplay();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiScrollableText& t );

    MachGuiScrollableText( const MachGuiScrollableText& );
    MachGuiScrollableText& operator =( const MachGuiScrollableText& );
    
    MachGuiStartupScreens* pStartupScreens_;	
};


#endif

/* End SCRLTEXT.HPP *************************************************/

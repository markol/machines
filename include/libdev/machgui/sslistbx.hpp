/*
 * S S L I S T B X . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiSingleSelectionListBox

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_SSLISTBX_HPP
#define _MACHGUI_SSLISTBX_HPP

#include "base/base.hpp"
#include "gui/sslistbx.hpp"
#include "machgui/focusctl.hpp"
#include "ctl/pvector.hpp"

class MachGuiStartupScreens;
class MachGuiSingleSelectionListBoxItem;

class MachGuiSingleSelectionListBox : public GuiSingleSelectionListBox, public MachGuiFocusCapableControl
// Canonical form revoked
{
public:
    MachGuiSingleSelectionListBox(  MachGuiStartupScreens* pParent, const Gui::Box&, 
    								size_t horizontalSpacing, size_t verticalSpacing,
    								size_t scrollInc );
    ~MachGuiSingleSelectionListBox();

    void CLASS_INVARIANT;

	void addListItem( MachGuiSingleSelectionListBoxItem* );
	void removeListItem( MachGuiSingleSelectionListBoxItem* );

	virtual bool doHandleNavigationKey( NavKey, MachGuiFocusCapableControl** );
	virtual void hasFocus( bool );

	virtual bool isDisabled() const;

	ctl_pvector< MachGuiSingleSelectionListBoxItem >& listItems();
		
protected:
	virtual void doDisplay();

	virtual void doNavSelectNewItem( MachGuiSingleSelectionListBoxItem* );

private:
    friend ostream& operator <<( ostream& o, const MachGuiSingleSelectionListBox& t );

    MachGuiSingleSelectionListBox( const MachGuiSingleSelectionListBox& );
    MachGuiSingleSelectionListBox& operator =( const MachGuiSingleSelectionListBox& );

	MachGuiStartupScreens* pStartupScreens_;
	ctl_pvector< MachGuiSingleSelectionListBoxItem > listItems_;
};


#endif

/* End SSLISTBX.HPP *************************************************/

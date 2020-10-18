/*
 * D R O P D O W N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDropDownListBox

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_DROPDOWN_HPP
#define _MACHGUI_DROPDOWN_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "machgui/sslistit.hpp"
#include "machgui/sslistbx.hpp"
#include "machgui/autodel.hpp"

class MachGuiDropDownListBoxItem;

class MachGuiDropDownListBox : public GuiSingleSelectionListBox, public MachGuiAutoDeleteDisplayable
{
public:
	MachGuiDropDownListBox( MachGuiStartupScreens* pParent, const Gui::Box& box, 
							size_t horizontalSpacing, size_t verticalSpacing,
							size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText );

	MachGuiDropDownListBox( MachGuiStartupScreens* pParent, const Gui::Box& box, 
							size_t horizontalSpacing, size_t verticalSpacing,
							size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText, bool whiteFont );

	virtual ~MachGuiDropDownListBox();

	virtual bool containsMousePointer();
	virtual void update();

protected:
	virtual void doDisplay();

	virtual void itemSelected( const string& text ) = 0;

	virtual MachGuiDropDownListBoxItem* createListBoxItem(	MachGuiStartupScreens* pStartupScreens, 
															MachGuiDropDownListBox* pListBox, 
															size_t width, 
															const string& text,
															bool whiteFont );
	friend class MachGuiDropDownListBoxItem;
			
private:
	friend ostream& operator <<( ostream& o, const MachGuiDropDownListBox& t );
    void CLASS_INVARIANT;

	// Data members...
	ctl_vector<string> itemText_;
	size_t itemWidth_;
	double timeInterval_;
	double timeStart_;	
	size_t nextItem_;
	bool whiteFont_;
};

class MachGuiDropDownListBoxItem : public MachGuiSingleSelectionListBoxItem
{
public:
	MachGuiDropDownListBoxItem( MachGuiStartupScreens* pStartupScreens, 
								MachGuiDropDownListBox* pListBox, 
								size_t width, 
								const string& text );

	MachGuiDropDownListBoxItem( MachGuiStartupScreens* pStartupScreens, 
								MachGuiDropDownListBox* pListBox, 
								size_t width, 
								const string& text,
								bool whiteFont );

protected:
	virtual void doDisplay();

	virtual void select();

	static GuiBmpFont getWhiteFont();

private:
	// Data members...
	MachGuiDropDownListBox* pListBox_;
	bool whiteFont_;
};

#endif

/* End DROPDOWN.HPP *************************************************/

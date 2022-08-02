/*
 * N U M P D R O P . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    MachGuiNumPlayersDropDownListBoxCreator

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_NUMPDROP_HPP
#define _MACHGUI_NUMPDROP_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "gui/displaya.hpp"
#include "gui/restring.hpp"
#include "machgui/dropdwnc.hpp"

class MachGuiNumPlayersDropDownListBoxCreator : public MachGuiDropDownListBoxCreator
// Canonical form revoked
{
public:
    MachGuiNumPlayersDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens*, int width, const GuiStrings& availText );
	MachGuiNumPlayersDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens*, int width, const GuiStrings& availText, bool whiteFont );
    MachGuiNumPlayersDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens*, const Gui::Coord& relCoord, int width, const GuiStrings& availText, bool whiteFont, bool border );
    ~MachGuiNumPlayersDropDownListBoxCreator();

    void CLASS_INVARIANT;

protected:
	virtual MachGuiDropDownList* createDropDownList( 	MachGuiStartupScreens* pParent, const Gui::Box& box, 
					    								size_t horizontalSpacing, size_t verticalSpacing,
														size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
														MachGuiDropDownListBoxCreator* );

	virtual bool doHandleNavigationKey( NavKey, MachGuiFocusCapableControl** );

private:
    friend ostream& operator <<( ostream& o, const MachGuiNumPlayersDropDownListBoxCreator& t );

    MachGuiNumPlayersDropDownListBoxCreator( const MachGuiNumPlayersDropDownListBoxCreator& );
    MachGuiNumPlayersDropDownListBoxCreator& operator =( const MachGuiNumPlayersDropDownListBoxCreator& );
};

class MachGuiNumPlayersDropDownList : public MachGuiDropDownList
{
public:
	MachGuiNumPlayersDropDownList(	MachGuiStartupScreens* pParent, const Gui::Box& box, 
						    		size_t horizontalSpacing, size_t verticalSpacing,
									size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
									MachGuiDropDownListBoxCreator* );

	MachGuiNumPlayersDropDownList(	MachGuiStartupScreens* pParent, const Gui::Box& box, 
						    		size_t horizontalSpacing, size_t verticalSpacing,
									size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
									MachGuiDropDownListBoxCreator*, bool whiteFont );
protected:

	virtual MachGuiDropDownListBoxItem* createListBoxItem(	MachGuiStartupScreens* pStartupScreens, 
															MachGuiDropDownListBox* pListBox, 
															size_t width, 
															const string& text,
															bool whiteFont );
};

class MachGuiNumPlayersDropDownListBoxItem : public MachGuiDropDownListBoxItem
{
public:
	MachGuiNumPlayersDropDownListBoxItem(	MachGuiStartupScreens* pStartupScreens, 
											MachGuiDropDownListBox* pListBox, 
											size_t width, 
											const string& text,
											bool whiteFont );
protected:
	virtual void select();
};

#endif

/* End NUMPDROP.HPP ************************************************/

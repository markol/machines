/*
 * D D R A W D R O P . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    MachGuiDDrawDropDownListBoxCreator

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_DDRAWDROP_HPP
#define _MACHGUI_DDRAWDROP_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "gui/displaya.hpp"
#include "gui/restring.hpp"
#include "machgui/dropdwnc.hpp"

class MachGuiDropDownCallback
{
	public:
		virtual void callBack() = 0;
};

class MachGuiDDrawDropDownListBoxCreator : public MachGuiDropDownListBoxCreator
// Canonical form revoked
{
public:
    MachGuiDDrawDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens*, int width, const GuiStrings& availText, MachGuiDropDownCallback* pCallbackHandler );
	MachGuiDDrawDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens*, int width, const GuiStrings& availText, bool whiteFont, MachGuiDropDownCallback* pCallbackHandler );
    MachGuiDDrawDropDownListBoxCreator( GuiDisplayable* pParent, MachGuiStartupScreens*, const Gui::Coord& relCoord, int width, const GuiStrings& availText, bool whiteFont, bool border, MachGuiDropDownCallback* pCallbackHandler );
    ~MachGuiDDrawDropDownListBoxCreator( void );

    void CLASS_INVARIANT;

protected:
	virtual MachGuiDropDownList* createDropDownList( 	MachGuiStartupScreens* pParent, const Gui::Box& box, 
					    								size_t horizontalSpacing, size_t verticalSpacing,
														size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
														MachGuiDropDownListBoxCreator* );

	virtual bool doHandleNavigationKey( NavKey navKey, MachGuiFocusCapableControl** pFocusCapableControl );

private:
    friend ostream& operator <<( ostream& o, const MachGuiDDrawDropDownListBoxCreator& t );

    MachGuiDDrawDropDownListBoxCreator( const MachGuiDDrawDropDownListBoxCreator& );
    MachGuiDDrawDropDownListBoxCreator& operator =( const MachGuiDDrawDropDownListBoxCreator& );

	MachGuiDropDownCallback* callbackHandler_;
};

class MachGuiDDrawDropDownList : public MachGuiDropDownList
{
public:
	MachGuiDDrawDropDownList(	MachGuiStartupScreens* pParent, const Gui::Box& box, 
					    		size_t horizontalSpacing, size_t verticalSpacing,
								size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
								MachGuiDropDownListBoxCreator*, MachGuiDropDownCallback* callbackHandler );

	MachGuiDDrawDropDownList(	MachGuiStartupScreens* pParent, const Gui::Box& box, 
					    		size_t horizontalSpacing, size_t verticalSpacing,
								size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
								MachGuiDropDownListBoxCreator*, bool whiteFont, MachGuiDropDownCallback* callbackHandler );
protected:

	virtual MachGuiDropDownListBoxItem* createListBoxItem(	MachGuiStartupScreens* pStartupScreens, 
															MachGuiDropDownListBox* pListBox, 
															size_t width, 
															const string& text,
															bool whiteFont );

private:
	MachGuiDropDownCallback* callbackHandler_;
};

class MachGuiDDrawDropDownListBoxItem : public MachGuiDropDownListBoxItem
{
public:
	MachGuiDDrawDropDownListBoxItem(MachGuiStartupScreens* pStartupScreens, 
									MachGuiDropDownListBox* pListBox, 
									size_t width, 
									const string& text,
									bool whiteFont,
									MachGuiDropDownCallback* callbackHandler );
protected:
	virtual void select();

private:
	MachGuiDropDownCallback* callbackHandler_;

};

#endif

/* End DDRAWDROP.HPP ************************************************/

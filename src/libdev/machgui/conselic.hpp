/*
 * C O N S E L I C . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachConstructionSelectIcon

    Represents an icon that appears in the ConstructCommandMenu.
    Clicking the icon selects a particular construction, and supplies
    its type, subtype and hardware level info to the active command.
*/

#ifndef _MACHGUI_CONSELIC_HPP
#define _MACHGUI_CONSELIC_HPP

#include "base/base.hpp"

#include "gui/icon.hpp"
#include "machphys/machphys.hpp"

//Forward refs
class MachInGameScreen;
class MachLogConstructionItem;

//Orthodox canonical (revoked)
class MachConstructionSelectIcon : public GuiIcon
{
public:
    //ctor. pParent is the immediate parent displayable.
    //The in game screen can be accessed from pInGameScreen.
    //The icon is to represent a construction of type objectType,
    //with subType defined, and hardware level hwLevel.
    MachConstructionSelectIcon( GuiDisplayable* pParent,
                                MachInGameScreen* pInGameScreen,
                                const MachLogConstructionItem& );

    //dtor.
    virtual ~MachConstructionSelectIcon();

	// inherited from GuiButton...
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachConstructionSelectIcon& t );

	static size_t reqWidth();
	static size_t reqHeight();

protected:
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& mouseEvent );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& mouseEvent );	

private:
    // Operations deliberately revoked
    MachConstructionSelectIcon( const MachConstructionSelectIcon& );
    MachConstructionSelectIcon& operator =( const MachConstructionSelectIcon& );
    bool operator ==( const MachConstructionSelectIcon& );

    //Data members
    MachInGameScreen* pInGameScreen_; // The in game screen
    const MachLogConstructionItem& consItem_; // The construction item
};


#endif

/* End CONSELIC.HPP *************************************************/

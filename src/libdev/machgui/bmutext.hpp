/*
 * B M U T E X T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiBmuText

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_BMUTEXT_HPP
#define _MACHGUI_BMUTEXT_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"
#include "gui/icon.hpp"
#include "system/pathname.hpp"

class MachContinentMap;
class MachInGameScreen;
class GuiMouseEvent;
typedef std::pair< SysPathName, SysPathName > SysPathNames;

class MachGuiBmuText : public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiBmuText( GuiDisplayable* pParent, const Gui::Coord& relPos );
    ~MachGuiBmuText();

    void CLASS_INVARIANT;

	void refresh();

protected:
	virtual void doDisplay();

	virtual void doHandleMouseClickEvent( const GuiMouseEvent& rel );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& rel );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& rel );
	virtual void doHandleContainsMouseEvent( const GuiMouseEvent& rel );

private:
    MachGuiBmuText( const MachGuiBmuText& );
    MachGuiBmuText& operator =( const MachGuiBmuText& );
    bool operator ==( const MachGuiBmuText& );

    friend ostream& operator <<( ostream& o, const MachGuiBmuText& t );

	size_t curValue_;
	size_t maxValue_;
};

class MachGuiBmuButton : public GuiBitmapButtonWithFilledBorder
// cannonical from revoked
{
public:
	MachGuiBmuButton(	GuiDisplayable* pParent,
						const Gui::Coord& rel,
						const SysPathName& bitmap,
						MachGuiBmuText* pBmuText,
						MachContinentMap* pContinentMap,
						MachInGameScreen *pInGameScreen );
	virtual ~MachGuiBmuButton();

protected:
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );

private:
	// Operations revoked
	MachGuiBmuButton( const MachGuiBmuButton& );
	MachGuiBmuButton& operator =( const MachGuiBmuButton& );
	bool operator ==( const MachGuiBmuButton& ) const;

	// Data members
	MachGuiBmuText* pBmuText_;
	MachContinentMap* pContinentMap_;
	MachInGameScreen* pInGameScreen_;
};

#endif

/* End BMUTEXT.HPP **************************************************/

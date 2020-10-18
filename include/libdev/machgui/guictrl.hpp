/*
 * G U I C T R L . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACH_GUICTRL_HPP
#define _MACH_GUICTRL_HPP

#include "gui/displaya.hpp"
#include "gui/icon.hpp"
#include "machgui/gui.hpp"

class MachInGameScreen;

class MachGuiIconWithCounter : public GuiBitmapButtonWithFilledBorder
// cannonical from revoked
{
public:
	MachGuiIconWithCounter( GuiDisplayable *pParent, const Gui::Coord& rel, const SysPathName& bitmap, MachInGameScreen* pInGameScreen );
	virtual ~MachGuiIconWithCounter();

protected:
	virtual void doDisplayInteriorEnabled( const Gui::Coord& abs );

	string getRootDirectory() const;

	size_t currentValue_;
	MachInGameScreen *	pInGameScreen_;	

private:
	MachGuiIconWithCounter( const MachGuiIconWithCounter& );
	MachGuiIconWithCounter& operator =( const MachGuiIconWithCounter& );
	bool operator ==( const MachGuiIconWithCounter& ) const;
};

class MachMachinesIcon : public MachGuiIconWithCounter
// cannonical from revoked
{
public:
	MachMachinesIcon( GuiDisplayable *pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen );
	virtual ~MachMachinesIcon();

	void refresh();

	// Make the button pop up without calling the doBeRelease 
	// functionality ( doBeReleased changes control panel context ).
	void forceUp();

	// Change button bitmap to match race coloue
	void loadGame();

protected:
	// inherited from GuiIcon...
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );

private:
	MachMachinesIcon( const MachMachinesIcon& );
	MachMachinesIcon& operator =( const MachMachinesIcon& );
	bool operator ==( const MachMachinesIcon& ) const;

	// Data members...
	bool forcingUp_;
};


class MachConstructionsIcon : public MachGuiIconWithCounter
// cannonical from revoked
{
public:
	MachConstructionsIcon( GuiDisplayable *pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen );
	virtual ~MachConstructionsIcon();

	void refresh();

	// Make the button pop up without calling the doBeRelease 
	// functionality ( doBeReleased changes control panel context ).
	void forceUp();

	// Change button bitmap to match race coloue
	void loadGame();

protected:
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );
		
private:
	MachConstructionsIcon( const MachConstructionsIcon& );
	MachConstructionsIcon& operator =( const MachConstructionsIcon& );
	bool operator ==( const MachConstructionsIcon& ) const;

	// Data members...
	bool forcingUp_;
};

/* //////////////////////////////////////////////////////////////// */

class MachSquadronIcon : public MachGuiIconWithCounter
// cannonical from revoked
{
public:
	MachSquadronIcon( GuiDisplayable *pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen );
	virtual ~MachSquadronIcon();

	void update();

	// Change button bitmap to match race coloue
	void loadGame();

protected:
	// inherited from GuiIcon...
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );

private:
	MachSquadronIcon( const MachSquadronIcon& );
	MachSquadronIcon& operator =( const MachSquadronIcon& );
	bool operator ==( const MachSquadronIcon& ) const;

	MachInGameScreen *	pInGameScreen_;
};

/* //////////////////////////////////////////////////////////////// */

#endif // #ifndef _MACH_GUICTRL_HPP

/* End GUICTRL.HPP **************************************************/
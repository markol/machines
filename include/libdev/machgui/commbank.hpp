/*
 * C O M M B A N K . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACH_COMMBANK_HPP
#define _MACH_COMMBANK_HPP

#include "gui/icon.hpp"
#include "gui/iconseq.hpp"

#include "machgui/gui.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachInGameScreen;
class MachGuiCommand;

class MachCommandIcons : public GuiScrollableIconSequence
// cannonical from revoked
{
public:

	MachCommandIcons( 	GuiDisplayable* pParent,
						const Gui::Coord& rel,
						MachInGameScreen* pInGameScreen );

	virtual ~MachCommandIcons();

	void change();

	static size_t reqWidth();
	static size_t reqHeight();

protected:
	virtual const Coords& coords() const;
	virtual void doDisplay();

private:
	MachCommandIcons( const MachCommandIcons& );
	MachCommandIcons& operator =( const MachCommandIcons& );
	bool operator ==( const MachCommandIcons& ) const;

	static const Coords& staticCoords();

	///////////////////////////////

	unsigned	offset_;
    MachInGameScreen* pInGameScreen_;
};

class MachSmallCommandIcons : public GuiScrollableIconSequence
// cannonical from revoked
{
public:

	MachSmallCommandIcons( 	GuiDisplayable* pParent,
							const Gui::Coord& rel,
							MachInGameScreen* pInGameScreen );

	virtual ~MachSmallCommandIcons();

	void change();

	static size_t reqWidth();
	static size_t reqHeight();

protected:
	virtual const Coords& coords() const;
	virtual void doDisplay();

private:
	MachSmallCommandIcons( const MachSmallCommandIcons& );
	MachSmallCommandIcons& operator =( const MachSmallCommandIcons& );
	bool operator ==( const MachSmallCommandIcons& ) const;

	static const Coords& staticCoords();

	///////////////////////////////

	unsigned	offset_;
    MachInGameScreen* pInGameScreen_;
};

/* //////////////////////////////////////////////////////////////// */

class MachCommandIcon : public GuiBitmapButtonWithFilledBorder
// cannonical form revoked
{
public:

	MachCommandIcon( 	GuiDisplayable *pParent,
						const Gui::Coord& rel,
                        const MachGuiCommand& command,
						MachInGameScreen * pInGameScreen );

	virtual ~MachCommandIcon();

    ///////////////////////////////////////////////////////
    // Inherited from GuiDisplayable
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );

    ///////////////////////////////////////////////////////

	static size_t reqWidth();
	static size_t reqHeight();

	const MachGuiCommand* pCommand() const;

protected:
	MachInGameScreen& inGameScreen();
	const MachInGameScreen& inGameScreen() const;

    //Inherited from GuiButton
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );

    //Inherited from GuiDisplayable
    virtual bool isEligableForVisibility() const;

private:
	MachCommandIcon( const MachCommandIcon& );
	MachCommandIcon& operator =( const MachCommandIcon& );
	bool operator ==( const MachCommandIcon& ) const;

    //Helper function used in initialisation list
    std::pair< SysPathName, SysPathName > bitmapPaths( const MachGuiCommand& command ) const;

	MachInGameScreen *	pInGameScreen_;
    const MachGuiCommand* pCommand_; //The command instance to be activated on clicking the icon
};

/* //////////////////////////////////////////////////////////////// */

#endif // #ifndef _MACH_COMMBANK_HPP

/* End COMMBANK.HPP *************************************************/

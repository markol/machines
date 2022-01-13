/*
 * S Q U A D R O N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _MACHGUI_SQUADRON_HPP
#define _MACHGUI_SQUADRON_HPP

#include "device/butevent.hpp"
#include "gui/icon.hpp"
#include "gui/scrolist.hpp"
#include "world4d/observer.hpp"
#include "machgui/gui.hpp"

/*
 *   MachGuiSquadronBank allows selection of a squadron from a
 *   collection of numbered squadron icons.
 */

class MachInGameScreen;
class MachGuiSquadron;
class MachGuiSquadronAdminIcon;
class MachSquadronIcon;
//class DevButtonEvent;
class DevKeyToCommandTranslator;


class MachGuiSquadronBank : public GuiSimpleScrollableList
// cannonical form revoked
{
public:
    MachGuiSquadronBank( GuiDisplayable* pParent, const Gui::Coord& rel, MachSquadronIcon* pNavButton, MachInGameScreen* pInGameScreen );
    virtual ~MachGuiSquadronBank();

	enum { N_SQUADRON_ICONS = 10 };

	static size_t reqWidth();
	static size_t reqHeight();

	void update();

	// Called every time there is a buttonEvent. Returns true if the buttonEvent was
	// processed (e.g. buttonEvent caused squad to be selected).
	bool processButtonEvent( const DevButtonEvent& buttonEvent );

	// Called before a game is played but after the game has been loaded.
	void loadGame();
	// Called before a game is unloaded to remove any links between gui components and
	// game objects that are about to be deleted.
	void unloadGame();

private:
    MachGuiSquadronBank( const MachGuiSquadronBank& );
    MachGuiSquadronBank& operator =( const MachGuiSquadronBank& );
    bool operator ==( const MachGuiSquadronBank& );

	enum Command { 	CREATE_SQUAD0, CREATE_SQUAD1, CREATE_SQUAD2, CREATE_SQUAD3, CREATE_SQUAD4,  
					CREATE_SQUAD5, CREATE_SQUAD6, CREATE_SQUAD7, CREATE_SQUAD8, CREATE_SQUAD9,
					SELECT_SQUAD0, SELECT_SQUAD1, SELECT_SQUAD2, SELECT_SQUAD3, SELECT_SQUAD4, 
					SELECT_SQUAD5, SELECT_SQUAD6, SELECT_SQUAD7, SELECT_SQUAD8, SELECT_SQUAD9 }; 

	void createSquad( size_t squadIndex );
	void selectSquad( size_t squadIndex );

    //Data members
	MachInGameScreen* pInGameScreen_;
	MachSquadronIcon* pNavButton_;
	DevKeyToCommandTranslator* pKeyTranslator_;
};

//////////////////////////////////////////////////////////////////////

class MachGuiSquadronBankIcon
: public GuiIcon, public W4dObserver
// cannonical form revoked
{
public:

    MachGuiSquadronBankIcon( MachGuiSquadron * pParent, 
    						const Gui::Coord& rel, 
    						unsigned squadronIndex,
    						MachInGameScreen * pInGameScreen,
    						MachGuiSquadronBank*  );
	// PRE( squadronIndex >= 0 );
	// PRE( squadronIndex < MachGuiSquadronIcon::N_SQUADRON_ICONS );

    virtual ~MachGuiSquadronBankIcon();

	static size_t reqWidth();
	static size_t reqHeight();

	void update();

	// Start observing squadrons before game is played
	void loadGame();
	void unloadGame();

protected:
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );

	virtual void doDisplayInteriorEnabled( const Gui::Coord& abs );

	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, 
	                         int clientData = 0 );
    virtual void domainDeleted( W4dDomain* pDomain );

private:
    MachGuiSquadronBankIcon( const MachGuiSquadronBankIcon& );
    MachGuiSquadronBankIcon& operator =( const MachGuiSquadronBankIcon& );
    bool operator ==( const MachGuiSquadronBankIcon& );

	static const SysPathName& iconBitmaps( unsigned squadronIndex );
	// PRE( squadronIndex <= MachGuiSquadronIcon::N_SQUADRON_ICONS );

	size_t squadronIndex_;
	size_t numInSquad_;
	MachInGameScreen* pInGameScreen_;		 
	MachGuiSquadronBank* pBank_;
	bool inDoBeReleased_;
};

//////////////////////////////////////////////////////////////////////

#endif	// #ifndef _MACHGUI_SQUADRON_HPP

/* End SQUADRON.HPP *************************************************/

/*
 * M A C H N A V . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiNavigatorBase

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_MACHNAV_HPP
#define _MACHGUI_MACHNAV_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"

class MachInGameScreen;
class MachGuiNavButton;
class MachGuiSelectAllNavButton;
class MachGuiNavButtonWithCounter;
class MachGuiNavRow;
class DevKeyToCommandTranslator;

class MachGuiNavigatorBase : public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiNavigatorBase( GuiDisplayable *pParent, const Gui::Box& box, MachInGameScreen*  );
    ~MachGuiNavigatorBase( void );

	static size_t reqWidth();
	static size_t reqHeight();

	bool remainVisible();
	void remainVisible( bool );

	void makeVisible( bool ); // isVisible is not virtual so to add extra functionality this function has been written

	virtual void deselectAll() = 0;
	virtual void selectAll() = 0;

	// Pass button event to navigator for "hot key" stuff. True returned indicates
	// that navigator used button event.
	bool processButtonEvent( const DevButtonEvent& buttonEvent );

protected:
	virtual void update() = 0;
	virtual void doDisplay();
	virtual bool machineNav();

	static void updateButtonNumber( MachGuiNavButtonWithCounter* pButton, size_t value, MachGuiNavRow* pRow );

	void selectAll( bool keepSelection );
	void viewNext();

	// Data members...
	bool updateReq_;
	DevKeyToCommandTranslator* pKeyTranslator_;
	MachInGameScreen* pInGameScreen_;
	
	enum CommandId { SELECT_ALL, SELECT_ALL_KEEP_SELECTION, VIEW_NEXT };

private:
    MachGuiNavigatorBase( const MachGuiNavigatorBase& );
    MachGuiNavigatorBase& operator =( const MachGuiNavigatorBase& );
    bool operator ==( const MachGuiNavigatorBase& );

	// Data members...
	bool remainVisible_;
	bool cycleIndex_;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * X X X . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiMachineNavigator

    A brief description of the class should go in here
*/


#include "machlog/mlnotif.hpp"

class MachLogResearchTree;

class MachGuiNavRow;
class MachGuiNavButton;
class MachGuiSelectAllNavButton;

class MachGuiMachineNavigator : public MachGuiNavigatorBase, public MachLogNotifiable
// Canonical form revoked
{
public:
    MachGuiMachineNavigator( GuiDisplayable *pParent, const Gui::Coord& rel, MachInGameScreen* );
    ~MachGuiMachineNavigator( void );

	void update();
	
	void buildMachineIcons();
	void refreshMachineIcons();
	void removeAllIcons();
	void updateMachineIconBitmaps();
	
	virtual void notifiableBeNotified();

	virtual void deselectAll();
	virtual void selectAll();

	// Do any pre-game initialisation. Stuff that couldn't be done in the constructor
	void loadGame();

	// Reset navigator ready for a new game to be loaded
	void unloadGame();

protected:
	virtual bool machineNav();

private:
    MachGuiMachineNavigator( const MachGuiMachineNavigator& );
    MachGuiMachineNavigator& operator =( const MachGuiMachineNavigator& );
    bool operator ==( const MachGuiMachineNavigator& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiMachineNavigator& t );

	// Data members...
	MachGuiNavRow* pRow1_;
	MachGuiNavRow* pRow2_;
	MachGuiNavRow* pRow3_;
	MachGuiNavRow* pRow4_;
	MachGuiNavRow* pRow5_;
	MachGuiNavRow* pRow6_;
	
	MachGuiNavButton* pGruntButton_;
	MachGuiNavButton* pAssassinButton_;
	MachGuiNavButton* pBallistaButton_;
	MachGuiNavButton* pKnightButton_;
	MachGuiNavButton* pNinjaButton_;
	MachGuiNavButton* pBossButton_;
	MachGuiNavButton* pOverseerButton_;
	MachGuiNavButton* pCommanderButton_;
	MachGuiNavButton* pLabTechButton_;
	MachGuiNavButton* pTechBoyButton_;
	MachGuiNavButton* pBrainBoxButton_;
	MachGuiNavButton* pDozerButton_;
	MachGuiNavButton* pBuilderButton_;
	MachGuiNavButton* pBehemothButton_;
	MachGuiNavButton* pGeoLocatorButton_;
	MachGuiNavButton* pSpyLocatorButton_;
	MachGuiNavButton* pResourceCarrierButton_;
	MachGuiNavButton* pAPCButton_;
	
	MachGuiSelectAllNavButton* pAggressorSelectAll_;
	MachGuiSelectAllNavButton* pAdministratorSelectAll_;
	MachGuiSelectAllNavButton* pConstructorSelectAll_;
	MachGuiSelectAllNavButton* pTechSelectAll_;
	MachGuiSelectAllNavButton* pLocatorSelectAll_;
	MachGuiSelectAllNavButton* pTransportSelectAll_;
	
	MachLogResearchTree* pResearchTree_;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * X X X . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiConstructionNavigator

    A brief description of the class should go in here
*/


#include "machlog/mlnotif.hpp"

class MachLogConstructionTree;

class MachGuiNavRow;
class MachGuiNavButton;
class MachGuiSelectAllNavButton;


class MachGuiConstructionNavigator : public MachGuiNavigatorBase, public MachLogNotifiable
// Canonical form revoked
{
public:
   
    MachGuiConstructionNavigator( GuiDisplayable *pParent, const Gui::Coord& rel, MachInGameScreen* );
    ~MachGuiConstructionNavigator( void );

	void update();
	
	void buildConstructIcons();
	void refreshConstructIcons();
	void removeAllIcons();
	void updateConstructionIconBitmaps();
	
	virtual void notifiableBeNotified();

	virtual void deselectAll();
	virtual void selectAll();

	// Do any pre-game initialisation. Stuff that couldn't be done in the constructor
	void loadGame();

	// Reset navigator ready for a new game to be loaded
	void unloadGame();

private:
    MachGuiConstructionNavigator( const MachGuiConstructionNavigator& );
    MachGuiConstructionNavigator& operator =( const MachGuiConstructionNavigator& );
    bool operator ==( const MachGuiConstructionNavigator& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiConstructionNavigator& t );

	// Data members...
	MachGuiNavRow* pRow1_;
	MachGuiNavRow* pRow2_;
	MachGuiNavRow* pRow3_;
	MachGuiNavRow* pRow4_;
	MachGuiNavRow* pRow5_;
	MachGuiNavRow* pRow6_;
	
	MachGuiNavButton* pPodButton_;
	MachGuiNavButton* pTurretButton_;
	MachGuiNavButton* pSentryButton_;
	MachGuiNavButton* pLauncherButton_;
	MachGuiNavButton* pICBMButton_;
	MachGuiNavButton* pCivFacButton_;
	MachGuiNavButton* pMilFacButton_;
	MachGuiNavButton* pCivLabButton_;
	MachGuiNavButton* pMilLabButton_;
	MachGuiNavButton* pSmelterButton_;
	MachGuiNavButton* pMineButton_;
	MachGuiNavButton* pGarrisonButton_;
	MachGuiNavButton* pBeaconButton_;
	
	MachGuiSelectAllNavButton* pPodSelectAll_;
	MachGuiSelectAllNavButton* pMissileSelectAll_;
	MachGuiSelectAllNavButton* pFactorySelectAll_;
	MachGuiSelectAllNavButton* pLabSelectAll_;
	MachGuiSelectAllNavButton* pSmelterSelectAll_;
	MachGuiSelectAllNavButton* pGarrisonSelectAll_;
		
	MachLogConstructionTree* pConstructionTree_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

/* End MACHNAV.HPP **************************************************/

/*
 * F S T P E R S N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiFirstPerson

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_FSTPERSN_HPP
#define _MACHGUI_FSTPERSN_HPP

#include "base/base.hpp"
#include "gui/root.hpp"
#include "world4d/observer.hpp"
#include "world4d/subject.hpp"
#include "machphys/machphys.hpp"
 
class W4dSceneManager;
class W4dRoot;
class MachGuiFirstPersonImpl;
class GuiMouseEvent;
class MachActor;
class MachInGameScreen;
class W4dDomain;

class MachGuiFirstPerson : public GuiRoot, public W4dObserver
// Canonical form revoked
{
public:
    MachGuiFirstPerson( W4dSceneManager* pSceneManager, W4dRoot* pRoot, MachInGameScreen* );
    ~MachGuiFirstPerson();

    void CLASS_INVARIANT;

	bool switchToMenus() const;
	bool switchToInGame() const;
	void resetSwitchToMenus();
	void resetSwitchToInGame();

	// Set the actor that we are controlling
	void setActor( MachActor* );

	// Set MachActor* to NULL.
	void resetActor();

	// Checks the actor type to make sure first person is an option. 
	bool okayToSwitchTo1stPerson();

	// Set up viewport for first person stuff.
	void setFirstPerson3DViewport();

	// Called after the app has been given windows focus ( WM_APPACTIVATE ).
	void activate();

	// Called when the ingame reolution has changed
	void resolutionChange();
	
protected:
	void displayCompass();
	void displayHealthArmour();
	void displayWeapons();
	bool displayWeapon( MachPhys::Mounting, const Gui::Coord& topLeft, const GuiBitmap& weaponBmp, double changeEndTime, int& weaponFrame );
	void updateWeaponAnimEndTime( double& );
	void loadWeaponBmps();
	GuiBitmap getWeaponBmp( MachPhys::WeaponType );
	void doWeaponSelect();

	// Load bitmaps in preperation for rendering. Too slow reading them off disk.
	void loadBitmaps();

	// Unload bitmaps when leaving first person to conserve memory usage.
	void unloadBitmaps();

	virtual void doDisplay();  
	virtual void update();
	virtual void doBecomeRoot();
	virtual void doBecomeNotRoot();
	virtual bool doHandleRightClickEvent( const GuiMouseEvent& );
	virtual bool doHandleKeyEvent( const GuiKeyEvent& );
	virtual void doHandleMouseClickEvent( const GuiMouseEvent& rel );

    //Inherited from W4dObserver
	//true iff this observer is to exist in this subject's list of observers
	//following this call. This will typically be implemented using double dispatch.
	//The clientData is of interest only if event == CLIENT_SPECIFIC.Interpretation
	//is client defined.
	virtual bool beNotified( W4dSubject*, W4dSubject::NotificationEvent, int clientData );

    //Informs observer that an observed domain is being deleted.
    //This observer need not call the W4dDomain::detach() method - this
    //will be done automatically.
    virtual void domainDeleted( W4dDomain* );

	enum Command { 	FOWARD, BACKWARD, 
					TURNLEFT, TURNRIGHT, TURNLEFTFAST, TURNRIGHTFAST, 
					FIRE, CENTREHEAD, WEAPONSELECT, 
					TURNHEADLEFT, TURNHEADRIGHT, TURNHEADLEFTFAST, TURNHEADRIGHTFAST, 
					LOOKUP, LOOKDOWN, LOOKUPFAST, LOOKDOWNFAST,
                    COMMAND_SELECT_NEXT, COMMAND_SELECT_PREV,
                    COMMAND_ORDER_ATTACK, COMMAND_ORDER_FOLLOW, COMMAND_ORDER_MOVE,
					NUM_COMMANDS /*must come last*/ };

private:
    friend ostream& operator <<( ostream& o, const MachGuiFirstPerson& t );

    void embodyActor();
    void exitActor();

    //Sort out the first person camera attachment to the 1st person entity
    void attachCamera();
    void detachCamera();

    //revoked
    MachGuiFirstPerson( const MachGuiFirstPerson& );
    MachGuiFirstPerson& operator =( const MachGuiFirstPerson& );

	// Data members...
	MachGuiFirstPersonImpl* pImpl_;
};


#endif

/* End FSTPERSN.HPP *************************************************/

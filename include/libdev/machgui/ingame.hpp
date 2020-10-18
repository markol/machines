/*
 * I N G A M E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachInGameScreen

    Top level GuiDisplayable for the in game screen.
*/

#ifndef _MACHGUI_INGAME_HPP
#define _MACHGUI_INGAME_HPP

#include "base/base.hpp"
#include "base/istrrep.hpp"
#include "world4d/observer.hpp"
#include "gui/root.hpp"
#include "machgui/gui.hpp"
#include "machgui/igcurs2d.hpp"

//Forward refs
class MachInGameScreenImpl;
class MachGuiCommand;
class MachWorldViewWindow;
class MachGuiCorral;
class MachPromptText;
class MachGuiDefconCommand;
class MachGuiSelfDestructCommand;
class MachGuiIonAttackCommand;
class MachGuiNukeAttackCommand;
class MachCameras;
class W4dSceneManager;
class W4dRoot;
class MachActor;
class MachGuiControlPanel;
template < class T > class ctl_pvector;

//orthodox canonical (revoked)
class MachInGameScreen
: public GuiRoot,
  public W4dObserver
{
public:

    //The scene manager for the 3d window is pSceneManager
	MachInGameScreen( W4dSceneManager* pSceneManager, W4dRoot* pRoot, BaseProgressReporter* pReporter );

	virtual ~MachInGameScreen();

    //Create the cameras, initialises button display values and map
    void loadGame( const string& planet );

	// Save info
	void saveGame( PerOstream& outStream );

	// Load saved game. There is no need to call "loadGame" as well!
	void loadSavedGame( const string& planet, PerIstream& inStream );

    //Delete the game's cameras
    void unloadGame();

    //Export components
	MachGuiCorral& corral();
	MachWorldViewWindow& worldViewWindow();
    MachPromptText& promptTextWindow();

    //True if component is to be displayed
    bool isCorralVisible() const;
	bool isCorralSingleIconVisible() const;
	bool isCommandIconsVisible() const;
	bool isSmallCommandIconsVisible() const;
    bool isMachineNavigationVisible() const;
    bool isConstructionNavigationVisible() const;
    bool isSquadronBankVisible() const;

    typedef ctl_pvector< MachActor > Actors;    	
    typedef ctl_pvector< MachGuiCommand > Commands;    	

    //The collection of selected actors
    const Actors& selectedActors() const;

    //True if actor is currently in the selected set
    bool isSelected( MachActor& actor ) const;

    //Add pActor to the collection of selected actors    
    void select( MachActor* pActor );
    //PRE( not isSelected( *pActor ) );

    //Add Actors to the collection of selected actors    
    void select( const Actors& actors );

    //Remove pActor from the collection of selected actors    
    void deselect( MachActor* pActor );
    //PRE( isSelected( *pActor ) );

    //Remove Actors from the collection of selected actors    
    void deselect( const Actors& actors );
	
    //Deselect all currently selected actors
    void deselectAll();

    //The list of all available commands
    const Commands& allCommands() const;

    //The default command
    MachGuiCommand& defaultCommand() const;

    //Make the default command active, cancelling any other active command
    void activateDefaultCommand();

    //Set/get the active command.
    void activeCommand( const MachGuiCommand& command );
    MachGuiCommand& activeCommand() const;

    //Flags any active command to be cancelled on next update.
    //This deferred cancellation is udeful for situations in which the
    //need to cancel the active command arises potentially when methods of
    //the command are on the call stack.
    void cancelActiveCommand();

    //Set/get the prompt text related to cursor position
    void cursorPromptText( const string& prompt );
    void cursorPromptText( const string& prompt, bool restartScroll );
    const string& cursorPromptText() const;

    //Clear any prompt text associated with the cursor position
    void clearCursorPromptText();

    //Set/get the prompt text related to current command state
    void commandPromptText( const string& prompt );
    const string& commandPromptText() const;

    //Clear any prompt text associated with the current command state
    void clearCommandPromptText();

    //set the current cursor
    void cursor2d( MachGui::Cursor2dType type );
	void cursor2d( MachGui::Cursor2dType type, MachInGameCursors2d::CursorSize curSize );

    //Update the command bank, if displayed
    void updateCommandIcons();

    //Access top context settings. It would be unusual for clients to use these methods.
    //This class is responsible for toggling display/activity of components.
    bool isMainMenuContext() const;
    bool isSquadronContext() const;
    bool isConstructCommandContext() const;
    bool isBuildCommandContext() const;
    bool isHardwareResearchContext() const;
    bool isSoftwareResearchContext() const;
	bool isSingleFactoryContext() const;
    bool isMachineNavigationContext() const;
    bool isConstructionNavigationContext() const;

    //Enter contexts for the menus
    void resetContext(); //Clear all context flags
    void mainMenuContext();
    void squadronContext();
    void constructCommandContext();
    void buildCommandContext();
    void hardwareResearchContext();
    void softwareResearchContext();
	void singleFactoryContext();
    void constructionNavigationContext();
    void machineNavigationContext();
	void mainMenuOrSingleFactoryContext();

    //Set the control panel context. Force update if you are switching to the same context
	//but want the code to re-evaluate the position of gui controls.
    void currentContext( MachGui::ControlPanelContext newContext, bool forceUpdate = false );

    //The current control panel context
    MachGui::ControlPanelContext currentContext() const;

    //The 3d window scene manager
    W4dSceneManager& sceneManager() const;

    //Set/get the cursor filter to be used in checking for objects under the cursor.
    //This filter has the same meaning as the filter argument to W4dDomain::findNearerEntity().
    void cursorFilter( int filter );
    int cursorFilter() const;

    //Inherited from GuiRoot
    //Perform any per-frame processing after event processing
    virtual void update();

	// Bits of the update method whose ordering in the loop cycle does not matter.
	// This CPU work can be pipelined between the 3D and 2D rendering.
	void asynchronousUpdate();

	void updateCameras();

	// Indicate if next command button pressed should be applied to a squad
	bool applyCommandToSquadron();
	void applyCommandToSquadron( bool );

	// Get at the different cameras viewing the WorldViewWindow
	MachCameras* cameras();
	// PRE( pCameras_ );

	// Highlight the actor ( i.e. surround with yellow box in world view window ), and
	// display information about this actor in corral ( only if no other actors are selected ).
	void highlightActor( MachActor* );
	void removeHighlightedActor();
	void displayActorPromptText( MachActor* );
	
	// Set the viewport to the fullscreen or to the world-view window, respectivley.
	void setGuiViewport();
	void setWorldViewViewport();

	// If true then the world view window is currently involved in a rubber band selection
	bool rubberBandSelectionHappening() const;

	enum GameState { PLAYING, WON, LOST };

	// Returns if game has been won/lost or is still being played. There is a slight
	// delay in this returning the actual gameState to enable victory/defeat music to be played.
	GameState gameState() const;
	// Same as above only no delay.
	GameState actualGameState() const;

	// Called after a WM_ACTIVATE message to ensure that the screen is re-drawn.
	void activate();

	MachGuiDefconCommand* defconCommand();
	MachGuiSelfDestructCommand* selfDestructCommand();
	MachGuiIonAttackCommand* ionAttackCommand();
	MachGuiNukeAttackCommand* nukeAttackCommand();

	// Switch FOW on or off. When playing skirmish levels or multiplayer 
	// this is a setable option
	void fogOfWarOn( bool fog );

	// MachGuiStartupScreens should query this to find out if a key has been
	// pressed that causes the ingame menus to be displayed.
	bool switchToMenus() const;
	void resetSwitchToMenus();
	// Cause menus to be displayed.
	void switchToMenus( bool );

	// Don't bother going through all the menu screens to exit
	void instantExit( bool );
	bool instantExit() const;

	// Go into machines head and control in 1st person.
	void switchToInHead();

	void checkSwitchGuiRoot();

	// When a game is in progress and we have been in the menu screens this function
	// is called to return to the in game screen.
	void switchBackToInGame();

	// True if the inGame gui is currently in first person context.
	bool inFirstPerson() const;

	// Position of mouse cursor when right button was pressed.
	const Gui::Coord& rightClickMousePos() const;

	// Called by the first person code because the ingame screen still needs to do a certain amount
	// of updates. e.g. the map must continue to unfog the fog-of-war. Because the first person main object
	// is also a GuiRoot it gets the GuiManager::update calls and the MachInGameScreen stop getting update
	// calls, hence the need for this function.
	void updateWhilstInFirstPerson();

	// Get at control panel.
	MachGuiControlPanel& controlPanel();
	// PRE( pControlPanel_ );

	// Switch control panel on and off
	bool controlPanelOn() const;
	void controlPanelOn( bool );

	// Call when the ingame resolution has been changed
	void resolutionChange();

	// Get an overview of what is in the corral
	enum CorralState { 	CORRAL_EMPTY = 0x0000, 
						CORRAL_ONEENTRY = 0x0001, 
						CORRAL_ALLMACHINES = 0x0002, 
						CORRAL_ALLCONSTRUCTIONS = 0x0004, 
						CORRAL_ALLFACTORIES = 0x0008,
						CORRAL_ALLCONSTRUCTORS = 0x0010,
						CORRAL_ALLAPCS = 0x0020,
						CORRAL_SOMEMACHINES = 0x0040,
						CORRAL_SOMECONSTRUCTIONS = 0x0080,
						CORRAL_SOMEENEMY = 0x0100,
						CORRAL_SOMEFRIENDLY = 0x0200 };

	typedef int CorralStateBitfield;
	CorralStateBitfield corralState() const;
	
	// Called to initiate a screen shot.  If hight-quality rendering options 
	// are supported but not in use (e.g. antialiasing), this sets said options, 
	// sets isRenderingScreenShot, then just continues.  In the absence of 
	// fancy rendering options this writes the shot immediately.
	void initiateScreenShot();
	bool isRenderingScreenShot() const;
	
	// If isRenderingScreenShot is true, then this should be called to
	// do the actual saving of the shot.
	// PRE(isRenderingScreenShot()); POST(!isRenderingScreenShot());
	void finalizeScreenShot();
	
	Gui::Box getWorldViewWindowVisibleArea() const;	
	
	// Used to determine if "Network Busy" message should be displayed
	bool isNetworkStuffed() const;
	
	void disableFirstPerson( bool );
	bool isFirstPersonDisabled() const;
	
protected:
	// Update whether the game has been lost or won
	void updateGameState();

	// inherited from GuiDisplayable...
	virtual void doDisplay();
	
	// inherited from GuiRoot...
	virtual void doBecomeRoot();
	virtual void doBecomeNotRoot();

	// Respond to any key presses. Pass key events on to Motion classes.
	bool doHandleKeyEvent( const GuiKeyEvent& e );
	// Any key presses considered none permenant
	bool doHandleKeyEventHacks(	const GuiKeyEvent& e );

	virtual void doHandleContainsMouseEvent( const GuiMouseEvent& );

	void updateChildVisible();

    //True if the root is handling right mouse clicks, and they should not
    //be dispatched according to location
    virtual bool doHandleRightClickEvent( const GuiMouseEvent& );

    //Inherited from W4dObserver
	//true iff this observer is to exist in this subject's list of observers
	//following this call. This will typically be implemented using double dispatch.
	//The clientData is of interest only if event == CLIENT_SPECIFIC.Interpretation
	//is client defined.
	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

    //Informs observer that an observed domain is being deleted.
    //This observer need not call the W4dDomain::detach() method - this
    //will be done automatically.
    virtual void domainDeleted( W4dDomain* pDomain );

	// Helper methods for actor prompt text
	bool addPromptTextMachineInfo( MachActor* pActor, string& prompt );
	bool addPromptTextConstructionInfo( MachActor* pActor, string& prompt );
	bool addPromptTextArtefactInfo( MachActor* pActor, string& prompt );
	bool addPromptTextDebrisInfo( MachActor* pActor, string& prompt );
	bool addPromptTextOreHolographInfo( MachActor* pActor, string& prompt );

	// Should control panel be displayed ( certain modes require the control panel to be 
	// revealed, i.e. construction menu ).
	bool displayControlPanel() const;
	
private:
	// When an actor is selected, this should be called to ensure that
	// the navigator is dismissed at the correct time
	void checkDismissNavigator();
	
    //Initialise the allCommands collection
    void initialiseAllCommands();

    //Do some of the deselection work
    void unselect( MachActor* pActor );

    //Add/remove display of various banks where appropriate.
    void setupActorBank();

	// Create/Update camera scroll regions of screen
	void setupCameraScrollAreas();

	// Create/Update prompt text ( called after resolution change )
	void setupPromptText();

	void setupNavigators();

	void setupCorralAndCommandIcons();

	void updateCorralState();
	
	// Physically save a screen shot, cf. initiateScreenShot and finalizeScreenShot.
	void saveScreenShot();

    //Data members...
	MachInGameScreenImpl* pImpl_;
};


#endif

/* End INGAME.HPP ***************************************************/

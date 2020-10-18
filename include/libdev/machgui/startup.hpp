/*
 * S T A R T U P . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiStartupScreens

    A brief description of the class should go in here
*/

#ifndef _MACHINES_STARTUP_HPP
#define _MACHINES_STARTUP_HPP

#include "base/base.hpp"
#include "base/istrrep.hpp"
#include "gui/root.hpp"
#include "device/keyboard.hpp"
#include "gui/restring.hpp"

class MachGuiStartupScreensImpl;
class W4dSceneManager;
class RenAnimCursor2d;
class MachInGameScreen;
class W4dRoot;
class GuiKeyEvent;
class AniSmacker;
class MachGuiStartupData;
class MachGuiMessageBroker;
class MachGuiAutoDeleteDisplayable;
class MachGuiMessageBoxResponder;
class MachGuiDbSavedGame;
class MachLogGameCreationData;
class MexTransform3d;
class MachGuiFocusCapableControl;
class MachGuiDbScenario;

class MachGuiStartupScreens	: public GuiRoot
// Canonical form revoked
{
public:
    MachGuiStartupScreens( W4dSceneManager* pSceneManager, W4dRoot* pRoot, BaseProgressReporter* );
    ~MachGuiStartupScreens( void );

	void setGuiViewport();

	enum ButtonEvent { NO_IMP, SINGLEPLAYER, EXIT, STARTGAME, CAMPAIGN, MOVIES, MULTIPLAYER, CONTINUE, JOIN, CREATE, SHOWGAMES,
					   LOADGAME, SAVEGAME, OPTIONS, SKIRMISH, SETTINGS, BE_OK, IMREADY, START, BRIEFING, HOTKEYS, RESTART, BE_NEW, BE_DELETE,
					   HINTS, OBJECTIVES, BE_CANCEL, BE_STATISTICS, BE_DUMMY_OK, BE_DUMMY_EXIT };

	void buttonAction( ButtonEvent );
	void buttonAction( ButtonEvent, const string& wavFile );

	void updateGui();
	void displayGui();

	bool finishApp();

	void loopCycle();

	// Called after a WM_ACTIVATE windows message
	void activate();

	// Get the bitmap that is forming the backdrop for all the buttons etc ( different backdrop
	// for each context ).
	const GuiBitmap& backdrop() const;

	// Returns the ButtonEvent enum for the last menu button that was pressed.
	ButtonEvent lastButtonEvent() const;

	// Update the startup screens backdrop. This is an image of a circuit board.
	void changeBackdrop( const char* );

	// Switch mouse cursor on/off
	void cursorOn( bool );

	enum Music { DONT_PLAY_CD = -1, MENU_MUSIC = 0, VICTORY_MUSIC = 1, DEFEAT_MUSIC = 2, LOADING_MUSIC = 3, DEFAULT_INGAME_MUSIC = 8 };

	// Indicate what music you'd like to hear.
	void desiredCdTrack( Music );

	// Holdall for data setup by startup screens.
	MachGuiStartupData* startupData();

	enum Context { 	CTX_LEGALSCREEN,
					CTX_DEMO,
					CTX_PROBEACCLAIMLOGO,
					CTX_CHARYBDISLOGO,
					CTX_INTROANIMATION,
					// Standard contexts
					CTX_MAINMENU,
					CTX_SINGLEPLAYER,
					CTX_CAMPAIGN,
					CTX_VICTORY,
					CTX_DEFEAT,
					CTX_MULTIPLAYER,
					CTX_JOIN,
					CTX_IMREADY,
					CTX_OPTIONS,
					CTX_IGOPTIONS,
					CTX_INGAMEOP,
					CTX_HOTKEYS,
					CTX_SAVE,
					CTX_IGLOAD,
					CTX_LOAD,
					CTX_BRIEFING,
					CTX_IGBRIEFING,
					CTX_DEBRIEFING,   // Switches to one of the 3 debriefing contexts below
					CTX_CADEBRIEFING, // Campaign debrief
					CTX_SKDEBRIEFING, // Skirmish debrief
					CTX_MPDEBRIEFING, // Multiplayer debrief
					CTX_SCENARIO,
					CTX_SKIRMISH,
					CTX_SETTINGS,
					CTX_LOADINGEXE,	  // Initial context to indicate that exe is loading
					CTX_VICTORYFLIC,  // Flic played when you win
					CTX_DEFEATFLIC,	  // Flic played when you lose
					CTX_ENTRYFLIC,	  // Flic played just before the game starts
					CTX_CASTATISTICS, // Campaign statistics
					CTX_SKSTATISTICS, // Skirmish statistics
					CTX_MPSTATISTICS, // Multi-player statistics
					CTX_POSTLOADINGANIMATION, // Short animation played just after preload
					// Special contexts
					CTX_FINISH,
					CTX_GAME,
					CTX_MULTI_GAME,
					CTX_SKIRMISH_GAME,
					CTX_BACKTOGAME,
					CTX_TRANSITION,
					CTX_TRANSITIONEND,
					// Context specifically for Demo version of game
					CTX_SPLASH1, 	// Splash screen for Demo
					CTX_SPLASH2, 	//   "		"	   "	"
					// New context, added here so file could be cheated
					CTX_CAINGAMEOPTIONS,	// Campaign ingame options screen
					CTX_SKINGAMEOPTIONS,	// Skirmish ingame options screen
					CTX_MPINGAMEOPTIONS		// Multiplayer ingame options screen
					};

	// Change to new context. Note that a context can refuse to give up focus if mandatory info is missing for example.
	void switchContext( Context );

	// Get the context that we are currently in.
	Context currentContext() const;

	MachGuiMessageBroker& messageBroker();
	// PRE( pMessageBroker_ );

	// Register a gui displayable that is present only if the mouse if inside it's
	// boundary ( generally drop-down list boxes ). If the mouse moves outside
	// the boundary then it is automatically deleted.
	void registerAutoDeleteGuiElement( MachGuiAutoDeleteDisplayable* );
	void unregisterAutoDeleteGuiElement();

	// Display a message on the screen. This will have to be dismissed before any other gui elements
	// can be interacted with.
	void displayMsgBox( uint stringResId );
	void displayMsgBox( uint stringResId, const GuiStrings& );
	// PRE( pMsgBox_ == NULL )

	// Display a message on the screen with OK and Cancel buttons. This will have to be dismissed before
	// any other gui elements can be interacted with. The message box responder will be told about which
	// button is pressed. The message box responder must be created on the heap.
	void displayMsgBox( uint stringResId, MachGuiMessageBoxResponder* );
	void displayMsgBox( uint stringResId, MachGuiMessageBoxResponder*, const GuiStrings& );
	void displayMsgBox( uint stringResId, MachGuiMessageBoxResponder*, bool yesNo );
	// PRE( pMsgBox_ == NULL );
	// PRE( pMsgBoxResponder_ == NULL );

	// Displays message box that can have a callback( responder ) attached to it but does not
	// have a cancel button
	void displayOKMsgBox( uint stringResId, MachGuiMessageBoxResponder* );

	// Find out if message box is on the screen
	bool msgBoxIsBeingDisplayed() const;

	// Get at the ingame screen gui root.
	MachInGameScreen& inGameScreen();

	// Set/Get the type of game that is being played.
	enum GameType { NOGAME, CAMPAIGNGAME, SKIRMISHGAME, MULTIGAME };
	GameType gameType() const;
	void gameType( GameType );

	// Restart the game that is being played. This should NOT be called for Multiplayer games. A game must
	// be being played in order for this to work.
	void restartGame();
	// PRE( gameType_ != NOGAME );
	// PRE( gameType_ != MULTIGAME );

	// Unload the game that is currently being played. It is safe to call this method even if a game is
	// not being played.
	void unloadGame();
	// POST( gameType_ == NOGAME );

	// Load up a previously saved game.
	void loadSavedGame( MachGuiDbSavedGame* );
	// PRE( gameType_ == NOGAME );

	// Add smacker animation to list of smacker animations
   	void addSmackerAnimation( AniSmacker* animation );
   	// PRE( anim != NULL )

 	// clears list of smacker files
	void clearAllSmackerAnimations( void );

	// plays any non full screen smacker animations associated with screen
	void playSmackerAnimations( void );

	//these will be called if there is a MachGuiSystemMessageHandler and the appropriate
	//network system message is isssued.
	bool	handleHostMessage();
	bool	handleDestroyPlayerMessage( const string& );
	bool	handleSessionLostMessage();

	void contextFinish();
	//the lobbying code exits in a special way hence the specific function.
	void contextFinishFromLobby();

	//Initialise the CD and sound volume to the values specified in the registry
	void initialiseVolumes();

	// If menu screens are using something other than 640x480 then what is the top left offset...
	int xMenuOffset() const;
	int yMenuOffset() const;

	void blitBackdrop( const Gui::Box&, const Gui::Coord& );

	// Focus capable controls allow for keyboard navigation of the menus
	void addFocusCapableControl( MachGuiFocusCapableControl* );
	void removeFocusCapableControl( MachGuiFocusCapableControl* );
	void messageBoxHasFocus( bool );

	//this is set to true when games are terminating due to loss of host
	//Slightly different flow (such as message boxes) are displayed.
	bool ignoreHostLostSystemMessage() const;
	void ignoreHostLostSystemMessage( bool );

protected:
	bool doHandleFocusCapableControls( const GuiKeyEvent& e );

	virtual void doDisplay();
	virtual void update();
	virtual bool doHandleRightClickEvent( const GuiMouseEvent& );
	virtual void doBecomeRoot();
	virtual void doBecomeNotRoot();

	void loopCycleInGame();
	void loopCycleStartupScreens();
	void loopCyclePlayingAnimation(); //When playing fullscreen flic

	// If startup screens has been in a particular context for too long ( e.g. legal screen )
	// then automatically switch context.
	void checkContextTimeout();

	void checkSwitchGuiRoot();
	void switchGuiRootToGame();
	void switchGuiRootToMultiGame();
	void switchGuiRootToSkirmishGame();

	void updateSound( const MexTransform3d& transf );

    //An appropriate ear position in global coords to use for updating 3d sound
    MexTransform3d earTransform() const;

    //Start playing a fullscreen smacker animation filename. Specify "fast" if you
	//want the animation to run at the fastest speed the CPU can go.
    void startPlayingAnimation( const SysPathName& filename, bool fast );
    void startPlayingAnimation( const SysPathName& filename, bool fast, bool frontBuffer, const Gui::Coord& pos );

    //Stop playing any smacker animation
    void stopPlayingAnimation();

    //Get the next frame of the smacker animation into the back buffer.
    void playAnimationFrame();

    //True if not playing an animation, or it has finished
    bool animationFinished();

	virtual bool doHandleKeyEvent( const GuiKeyEvent& e );
	virtual bool doHandleCharEvent( const DevButtonEvent& e );

	void updateCdAudio();

	bool isContextFlic() const;

	static void releaseCachedMemory();

	// Setup victory condition in MachLogGameCreationData. Basically converts from a MachGui enum
	// to a MachLog enum.
	void setVictoryCondition( MachLogGameCreationData& );

	// When a game is won, returns the next scenario which should be selected
	MachGuiDbScenario* workOutWhichScenarioToDefaultTo();
	//PRE( MachGuiDatabase::instance().hasCurrentPlayer() );
	//PRE( MachGuiDatabase::instance().currentPlayer().nPlayerScenarios() != 0 );

private:
    MachGuiStartupScreens( const MachGuiStartupScreens& );
    MachGuiStartupScreens& operator =( const MachGuiStartupScreens& );
    bool operator ==( const MachGuiStartupScreens& );

	void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiStartupScreens& t );

	RenAnimCursor2d* loadCursor( const string& baseName, int nFrames );

	struct ContextSwitchInfo
	{
		Context	curContext_;
		ButtonEvent buttonEvent_;
		Context	newContext_;
		bool playTransition_;
	};
	struct ContextTimeoutInfo
	{
		enum Type { TIMEOUT, FLIC_FINISHED };
		Context curContext_;
		Context newContext_;
		Type type_;
		size_t timeInfo_;
	};
	struct ContextKeypressInfo
	{
		Context curContext_;
		DevKey::ScanCode scanCode_;
		Context newContext_;
		bool playTransition_;
		ButtonEvent simButtonEvent_;
	};
	struct ContextFlic
	{
		Context context_;
		const char* flicPath_;
	};

	static ContextSwitchInfo* getContextSwitchInfo();
	static ContextTimeoutInfo* getContextTimeoutInfo();
	static ContextKeypressInfo* getContextKeypressInfo();

	void contextAnimation();
	//void contextFinish();
	void contextGame();
	void contextVictory();
	void contextDefeat();
	void contextLogo();

	static string getContextStrName( Context context );

	MachGuiStartupScreensImpl * pImpl_;
};

#endif

/* End STARTUP.HPP **************************************************/

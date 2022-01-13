/*
 * S T A R T U P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>
#include "machgui/startup.hpp"
#include "machgui/internal/startui.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "machgui/ingame.hpp"
#include "machgui/cameras.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/autodel.hpp"
#include "machgui/playerit.hpp"
#include "machgui/msgbox.hpp"
#include "machgui/messbrok.hpp"
#include "machgui/sslistit.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/dbsystem.hpp"
#include "machgui/dbplanet.hpp"
#include "machgui/dbplyscn.hpp"
#include "machgui/database.hpp"
#include "machgui/dbplayer.hpp"
#include "machgui/dbsavgam.hpp"
#include "machgui/dbhandlr.hpp"
#include "machgui/lsgextra.hpp"
#include "machgui/sysmess.hpp"
#include "machgui/chatmsgs.hpp"
#include "machgui/focusctl.hpp"
#include "machgui/dispnoti.hpp"

// Context helpers
#include "machgui/ctxmulti.hpp"
#include "machgui/ctxjoin.hpp"
#include "machgui/ctxsingl.hpp"
#include "machgui/ctxmmenu.hpp"
#include "machgui/ctxready.hpp"
#include "machgui/ctxingam.hpp"
#include "machgui/ctxskirm.hpp"
#include "machgui/ctxhotky.hpp"
#include "machgui/ctxbrief.hpp"
#include "machgui/ctxcamp.hpp"
#include "machgui/ctxsave.hpp"
#include "machgui/ctxload.hpp"
#include "machgui/ctxdebrf.hpp"
#include "machgui/ctxscenr.hpp"
#include "machgui/ctxsetng.hpp"
#include "machgui/ctxoptns.hpp"
#include "machgui/ctxstats.hpp"

#include "machgui/internal/strings.hpp"
#include "profiler/profiler.hpp"
#include "recorder/recorder.hpp"
#include "mathex/random.hpp"
#include "ani/AniSmacker.hpp"
#include "ani/AniSmackerCutscene.hpp"
#include "gui/event.hpp"
#include "gui/manager.hpp"
#include "gui/painter.hpp"
#include "gui/displaya.hpp"
#include "gui/scrolist.hpp"
#include "gui/editbox.hpp"
#include "gui/sslistbx.hpp"
#include "gui/scrolbut.hpp"
#include "gui/scrolbar.hpp"
#include "gui/font.hpp"
#include "gui/image.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/manager.hpp"
#include "world4d/camera.hpp"
#include "world4d/soundman.hpp"
#include "system/pathname.hpp"
#include "render/animcurs.hpp"
#include "render/device.hpp"
#include "render/display.hpp"
#include "render/surface.hpp"
#include "sound/soundmix.hpp"
#include "sound/sndwavid.hpp"
#include "sound/smpparam.hpp"
#include "mathex/transf3d.hpp"
#include "sim/manager.hpp"
#include "machphys/plansurf.hpp"
#include "machlog/races.hpp"
#include "machlog/planet.hpp"
#include "machlog/network.hpp"
#include "machlog/stats.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/creatdat.hpp"
#include "machlog/p1remman.hpp"
#include "machlog/score.hpp"
#include "network/node.hpp"
#include "network/session.hpp"
#include "device/cd.hpp"
#include "device/butevent.hpp"
#include "afx/resource.hpp"
#include "system/winapi.hpp"
#include <fstream>
#include "network/netnet.hpp"
#include "network/session.hpp"
#include "phys/phys.hpp"
#include "system/registry.hpp"
#include "device/mouse.hpp"
#include "base/cballoc.hpp"

class LoadGameProgressIndicator : public BaseProgressReporter
{
public:
	LoadGameProgressIndicator( MachGuiStartupScreens* pStartupScreens )
	:	lowerLimit_( 0.0 ),
		upperLimit_( 1.0 ),
		lastDone_( 0 ),
		pStartupScreens_( pStartupScreens )
	{}

    virtual size_t report( size_t done, size_t maxDone )
	{
		if ( done == lastDone_) return 0;
		const double minx = 275 + pStartupScreens_->xMenuOffset();
		const double maxx = 361 + pStartupScreens_->xMenuOffset();
		const double miny = 250 + pStartupScreens_->yMenuOffset();
		const double maxy = 254 + pStartupScreens_->yMenuOffset();
		const double width = maxx - minx + 1;
		const double height = maxy-miny+1;
		const double limitRange = upperLimit_ - lowerLimit_;
		const double percentComplete = (((double)done/(double)maxDone)*limitRange)+lowerLimit_;
		const double displayWidth = std::min((percentComplete*width)+5,width);

		const double red 	= 255.0/255.0;
		const double green 	= 252.0/255.0;
		const double blue 	= 223.0/255.0;

		RenDevice::current()->frontSurface().filledRectangle( Ren::Rect(minx,miny,displayWidth,height), RenColour(red,green,blue) );
		RenDevice::current()->display()->flipBuffers();
		//keep the ping messages alive.
		//This will also remove the messages from the directplay external message queue
		//Allowing continued processing.
		if( MachLogNetwork::instance().isNetworkGame() )
		{
			//Note that we poll the messages directly reatherthan calling MLNetwork::update
			NetNetwork::instance().pollMessages();
//			MachLogNetwork::instance().node().transmitCompoundMessage();
			SysWindowsAPI::sleep( 0 );
			SysWindowsAPI::peekMessage();
/*			if( MachLogNetwork::instance().node().lastPingAllTime() > 2.5 )
			{
				MachLogNetwork::instance().node().pingAll();
			}*/
		}
		lastDone_ = done;
		return 0;
	}

	void setLimits( double lower, double upper )
	{
		lowerLimit_ = lower;
		upperLimit_ = upper;
	}

private:
	double 	lowerLimit_;
	double 	upperLimit_;
	size_t  lastDone_;
	MachGuiStartupScreens* pStartupScreens_;
};


#define CB_MachGuiStartupScreens_DEPIMPL() \
		CB_DEPIMPL(	RenAnimCursor2d*, pMenuCursor_ );	\
		CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ ); \
		CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );	\
		CB_DEPIMPL(	MachGuiStartupScreens::Context, contextAfterFlic_ ); \
		CB_DEPIMPL(	MachGuiStartupScreens::Context, contextBeforeFlic_ ); \
		CB_DEPIMPL(	GuiBitmap*, pBackdrop_ ); \
		CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ ); \
		CB_DEPIMPL(	bool, switchGuiRoot_ ); \
		CB_DEPIMPL(	bool, finishApp_ ); \
		CB_DEPIMPL(	W4dRoot*, pW4dRoot_ ); \
		CB_DEPIMPL(	PhysAbsoluteTime, contextTimer_ ); \
		CB_DEPIMPL(	AniSmacker*, pPlayingSmacker_ ); \
		CB_DEPIMPL(	MachGuiStartupScreens::Music, playingCdTrack_ ); \
		CB_DEPIMPL(	MachGuiStartupScreens::Music, desiredCdTrack_ ); \
		CB_DEPIMPL(	PhysAbsoluteTime, cdCheckTime_ );	\
		CB_DEPIMPL(	bool, endGame_ ); \
		CB_DEPIMPL(	ButtonEvent, lastButtonEvent_ );	\
		CB_DEPIMPL(	MachGuiMessageBox*, pMsgBox_ ); \
		CB_DEPIMPL(	MachGuiMessageBoxResponder*, pMsgBoxResponder_ ); \
		CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ ); \
		CB_DEPIMPL(	MachGuiStartupScreenContext*, pCurrContext_ ); \
		CB_DEPIMPL(	MachGuiStartupData*, pStartupData_ ); \
		CB_DEPIMPL(	MachGuiMessageBroker*, pMessageBroker_ ); \
		CB_DEPIMPL(	MachGuiAutoDeleteDisplayable*, pMustContainMouse_ ); \
		CB_DEPIMPL(	MachGuiStartupScreensImpl::SmackerAnims, smackerAnims_ ); \
		CB_DEPIMPL(	GuiDisplayable*, pCharFocus_ ); \
		CB_DEPIMPL( MachGuiDispositionChangeNotifiable*, pDispositionNotifiable_ ); \
		CB_DEPIMPL(	bool, ignoreHostLostSystemMessage_ );

MachGuiStartupScreens::MachGuiStartupScreens( W4dSceneManager* pSceneManager, W4dRoot* pRoot, BaseProgressReporter* pReporter )
: 	GuiRoot( Gui::Box( 0, 0, pSceneManager->pDevice()->windowWidth(),
    	                     pSceneManager->pDevice()->windowHeight() ) ),
	pImpl_( NULL )
{
	pImpl_ = _NEW( MachGuiStartupScreensImpl );

	CB_MachGuiStartupScreens_DEPIMPL();

	pSceneManager_ = pSceneManager;
	pBackdrop_ = NULL;
	finishApp_ = false;
	switchGuiRoot_ = false;
	pW4dRoot_ = pRoot;
    pPlayingSmacker_ = NULL;
	playingCdTrack_ = LOADING_MUSIC;
	desiredCdTrack_ = LOADING_MUSIC;
    context_ = CTX_LEGALSCREEN;
	pCurrContext_ = NULL;
	pMustContainMouse_ = NULL;
	pMsgBox_ = NULL;
	pMsgBoxResponder_ = NULL;
	gameType_ = NOGAME;
	pCharFocus_ = NULL;
	pDispositionNotifiable_ = _NEW( MachGuiDispositionChangeNotifiable( this ) );
	ignoreHostLostSystemMessage_ = false;
	cdCheckTime_ = 0.0;


    pMenuCursor_ = loadCursor( "large/arrow", 1 );
	pMenuCursor_->origin(0,0);

	pReporter->report( 1, 100 ); // 1% of gui stuff done

	pInGameScreen_ = _NEW( MachInGameScreen( pSceneManager, pW4dRoot_, pReporter ) );

	pReporter->report( 70, 100 ); // 70% of gui stuff done

 	switchContext( CTX_LOADINGEXE );

	becomeRoot();
	GuiManager::instance().keyboardFocus( this );
	GuiManager::instance().charFocus( this );

	pReporter->report( 75, 100 ); // 75% of gui stuff done

	pStartupData_ = _NEW( MachGuiStartupData( this ) );

    //Assign the database interface handler into machlog
    MachLogRaces::instance().databaseHandler( &MachGuiDatabase::instance().handler() );

	pReporter->report( 80, 100 ); // 80% of gui stuff done

	//even if the game is not a network game we can still give it systemmessagehandler. The Network library OWNS the
	//pointer wrt to _DELETEs. Other handlers may be reassigned safely.
	NetNetwork::instance().systemMessageHandler( _NEW( MachGuiSystemMessageHandler( this ) ) );
	// Set up network message broker ( routes network messages to the appropriate code ).
	pMessageBroker_ = _NEW( MachGuiMessageBroker( pStartupData_ ) );
	MachLogNetwork::instance().setBroker( pMessageBroker_ );
	SysRegistry::instance().setStringValue( "", "version", "1.0" );

	MachGuiInGameChatMessages::instance().initialise( pMessageBroker_, this );

	initialiseVolumes();

	pReporter->report( 100, 100 ); // 100% of gui stuff done
	if( getenv("CB_DISABLE_NETWORK_TO") )
	{
		NETWORK_STREAM_DISABLE;
	}

	// Not so efficient but needed in all menu screens for OpenGL version
	redrawEveryFrame(true);

    TEST_INVARIANT;
}

MachGuiStartupScreens::~MachGuiStartupScreens()
{
    TEST_INVARIANT;

	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL(	AniSmacker*, pPlayingSmacker_ );
	CB_DEPIMPL(	MachGuiStartupData*, pStartupData_ );
	CB_DEPIMPL( MachGuiDispositionChangeNotifiable*, pDispositionNotifiable_ );

	//Bullet proof function - doesn't matter if notifiable has already been unregistered.
	MachLogRaces::instance().unregisterDispositionChangeNotifiable( pDispositionNotifiable_ );
	_DELETE( pDispositionNotifiable_ );

	_DELETE( pInGameScreen_ );
    _DELETE( pPlayingSmacker_ );
	_DELETE( pStartupData_ );

	clearAllSmackerAnimations();

	releaseCachedMemory();
	_DELETE( pImpl_->pMenuCursor_ );
	MachGui::releaseInGameBmpMemory();

	_DELETE( pImpl_->pMessageBroker_ );

	_DELETE( pImpl_ );
}

void MachGuiStartupScreens::loopCycle()
{
	CB_DEPIMPL(	AniSmacker*, pPlayingSmacker_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );

    //Trace memory every minute if CB_MINMEM set
    static bool traceMemoryPerMinute = getenv( "CB_MINMEM" ) != NULL;

    if( traceMemoryPerMinute )
    {
        static PhysAbsoluteTime lastTraceTime = Phys::time();
        if( Phys::time() - lastTraceTime > 60.0 )
        {
            cbAllocTrace( Diag::instance().danielStream(), "Loop cycle at 1 minute interval",  CB_ALLOC_DETAIL_BLOCKS );
            lastTraceTime = Phys::time();
        }
    }

	//we should update multiplayer messages EVERY frame regardless of context (i.e in animations in various handshaking screens etc)
	//if we do this here then the checkSwitchGuiRoot will work correctly wrt to loading screen stuff (I hope!)
	if( MachLogNetwork::instance().isNetworkGame() /*and
		not startupData()->terminalMultiPlayerGameProblem()*/ )
	{
		MachLogNetwork::instance().update();
	}

	checkSwitchGuiRoot();

	if ( context_ == CTX_GAME or
		 context_ == CTX_MULTI_GAME or
		 context_ == CTX_SKIRMISH_GAME )
	{
		loopCycleInGame();
	}
	else if( pPlayingSmacker_ )
	{
        loopCyclePlayingAnimation();
	}
    else
	{
		loopCycleStartupScreens();
	}
}

void MachGuiStartupScreens::updateGui()
{
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );

	// This is a useful marker for making sense of the render log.  Please leave.
	RENDER_STREAM("Starting MachGuiStartupScreens::updateGui()\n");
	RENDER_INDENT(2);

	bool inGame = ( context_ == CTX_GAME or
		 			context_ == CTX_MULTI_GAME or
		 			context_ == CTX_SKIRMISH_GAME );

	// This is a bit of a hack.  Setting the viewports correctly depends
	// upon where this method is invoked in the order of things.
	if ( inGame )
		pInGameScreen_->setGuiViewport();
	else
		setGuiViewport();

	// Update Gui components...
	GuiManager::instance().update();

	if ( inGame )
		pInGameScreen_->setWorldViewViewport();

	RENDER_STREAM("Done MachGuiStartupScreens::updateGui()\n");
	RENDER_INDENT(-2);
}

void MachGuiStartupScreens::displayGui()
{
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );

	bool inGame = ( context_ == CTX_GAME or
		 			context_ == CTX_MULTI_GAME or
		 			context_ == CTX_SKIRMISH_GAME );

	// This is a bit of a hack.  Setting the viewports correctly depends
	// upon where this method is invoked in the order of things.
	if ( inGame )
		pInGameScreen_->setGuiViewport();
	else
	    setGuiViewport();

	// Draw any updated Gui components...
	GuiManager::instance().display();

	if ( inGame )
		pInGameScreen_->setWorldViewViewport();
}

void MachGuiStartupScreens::checkSwitchGuiRoot()
{
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );
	CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ );
	CB_DEPIMPL(	bool, switchGuiRoot_ );
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL(	bool, isGamePaused_ );

	if ( switchGuiRoot_ )
	{
		DEBUG_STREAM( DIAG_NEIL, "Switching GUI root" << std::endl );

		switchGuiRoot_ = false;

		if ( context_ == CTX_GAME )
		{
			switchGuiRootToGame();
 		}
		else if ( context_ == CTX_MULTI_GAME )
		{
			switchGuiRootToMultiGame();
		}
		else if ( context_ == CTX_SKIRMISH_GAME )
		{
			switchGuiRootToSkirmishGame();
		}
		else if ( context_ == CTX_BACKTOGAME )
		{
			// Free up any cached memory used by menus.
			releaseCachedMemory();

			// Make the ingame screen or first person screen the gui root.
			pInGameScreen_->switchBackToInGame();

			// Restart the sim manager to get all the machines moving again.
			if ( gameType_ != MULTIGAME )
			{
				// Resume sim if the game wasn't paused when we switched into the menu screens
				if ( not isGamePaused_ )
					SimManager::instance().resume();

				if ( gameType_ == CAMPAIGNGAME )
					context_ = CTX_GAME;
				else
					context_ = CTX_SKIRMISH_GAME;
			}
			else
			{
				context_ = CTX_MULTI_GAME;
			}
		}
		else
		{
			// Make startup screens the gui root so that it gets drawn on screen.
			becomeRoot();
			GuiManager::instance().keyboardFocus( this );

			// Don't suspend if playing multiplayer game otherwise your gona get your ass
			// kicked whilst your changing settings etc.
			if ( gameType_ != MULTIGAME )
			{
				SimManager::instance().suspend();
			}
		}
	}
	else
	{
		if (context_ == CTX_GAME or
			context_ == CTX_SKIRMISH_GAME or
			context_ == CTX_MULTI_GAME )
		{
			pInGameScreen_->checkSwitchGuiRoot();
		}
	}
}

void MachGuiStartupScreens::switchGuiRootToGame()
{
	CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ );
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL(	RenAnimCursor2d*, pMenuCursor_ );
	CB_DEPIMPL( MachGuiDispositionChangeNotifiable*, pDispositionNotifiable_ );

	PRE( gameType_ == NOGAME );

	// Display loading bmp
	GuiBitmap loadingBmp = Gui::bitmap( "gui/menu/loading.bmp" );
	loadingBmp.enableColourKeying();
	GuiBitmap frontBuffer = W4dManager::instance().sceneManager()->pDevice()->frontSurface();
	frontBuffer.simpleBlit( loadingBmp, xMenuOffset(), yMenuOffset() );
	// For double buffering call it twice to draw on both front and back buffers
	RenDevice::current()->display()->flipBuffers();
	frontBuffer.simpleBlit( loadingBmp, xMenuOffset(), yMenuOffset() );

	LoadGameProgressIndicator progressIndicator(this);

	// Free up any cached memory used by menus.
	releaseCachedMemory();

	progressIndicator.report( 10, 100 );

	ASSERT( startupData()->scenario(), "no scenario has been selected for loading");

    //Clear the list of set flags in the database handler
    MachGuiDatabase::instance().handler().clearSetFlags();
	MachGuiDatabase::instance().currentScenario( startupData()->scenario() );

	progressIndicator.report( 12, 100 );

	string planet = startupData()->scenario()->planetFile();
	string scenario = startupData()->scenario()->name() + ".scn";

	DEBUG_STREAM( DIAG_NEIL, "load planet " << planet << std::endl );
	DEBUG_STREAM( DIAG_NEIL, "load scenario " << scenario << std::endl );

	SimManager::instance().suspend();
	SimManager::instance().resetTime();

	progressIndicator.report( 13, 100 );

	progressIndicator.setLimits( 0.13, 0.7 );

	// Load the game
	MachLogRaces::instance().loadGame( pSceneManager_, planet, scenario, &progressIndicator );
	MachLogRaces::instance().registerDispositionChangeNotifiable( pDispositionNotifiable_ );

	progressIndicator.setLimits( 0.0, 1.0 );

	progressIndicator.report( 70, 100 );

    pInGameScreen_->loadGame( planet );

	progressIndicator.report( 85, 100 );

	// Campaign games should always start with FOW
	pInGameScreen_->fogOfWarOn( true );

	GuiManager::instance().keyboardFocus( pInGameScreen_ );

    pSceneManager_->pDevice()->display()->useCursor( pMenuCursor_ );

	gameType_ = CAMPAIGNGAME;

	progressIndicator.report( 100, 100 );

	pInGameScreen_->becomeRoot();
	pInGameScreen_->activate();
	SimManager::instance().resume();

    //Touch all our own allocated memory to avoid thrashing after the game starts
    cbTouchAll();

	//  Give the AI a head start. This avoids chugging when we first get into the game
    for( size_t i = 0; i < 10; ++i )
    	SimManager::instance().cycle();
}

void MachGuiStartupScreens::switchGuiRootToSkirmishGame()
{
	CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ );
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL(	RenAnimCursor2d*, pMenuCursor_ );
	CB_DEPIMPL( MachGuiDispositionChangeNotifiable*, pDispositionNotifiable_ );

	PRE( gameType_ == NOGAME );

	// Display loading bmp
	GuiBitmap loadingBmp = Gui::bitmap( "gui/menu/loading.bmp" );
	loadingBmp.enableColourKeying();
	GuiBitmap frontBuffer = W4dManager::instance().sceneManager()->pDevice()->frontSurface();
	frontBuffer.simpleBlit( loadingBmp, xMenuOffset(), yMenuOffset() );
	// For double buffering call it twice to draw on both front and back buffers
	RenDevice::current()->display()->flipBuffers();
	frontBuffer.simpleBlit( loadingBmp, xMenuOffset(), yMenuOffset() );

	if( getenv( "CB_PROFILE_PLANET_LOADING" ) )
    	ProProfiler::instance().enableProfiling();

	LoadGameProgressIndicator progressIndicator(this);

	// Free up any cached memory used by menus.
	releaseCachedMemory();

	ASSERT( startupData()->scenario(), "no scenario has been selected for loading");

	string planet = startupData()->scenario()->planetFile();
	string scenario = startupData()->scenario()->name() + ".scn";

	DEBUG_STREAM( DIAG_NEIL, "load planet " << planet << std::endl );
	DEBUG_STREAM( DIAG_NEIL, "load scenario " << scenario << std::endl );

    //Clear the list of set flags in the database handler
    MachGuiDatabase::instance().handler().clearSetFlags();
	MachGuiDatabase::instance().currentScenario( startupData()->scenario() );

	SimManager::instance().suspend();
	SimManager::instance().resetTime();

	progressIndicator.report( 10, 100 );

	// Place user defined data in creation data arrays here
	MachLogGameCreationData gameData;

	gameData.randomStarts( startupData()->startingPosition() );
	gameData.resourcesAvailable( startupData()->resources() );
	gameData.startingResources( startupData()->startingResources() );
	gameData.technologyLevel( startupData()->techLevel() );
	setVictoryCondition( gameData );

	// Setup info about players
	MachLogGameCreationData::PlayersCreationData creationData;

	progressIndicator.report( 15, 100 );

	// Initialise all player info to NOT_DEFINED
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
	{
		MachLogGameCreationData::PlayerCreationData pcd;
		pcd.type_ = MachLog::NOT_DEFINED;
		pcd.colour_ = i;
		creationData.push_back( pcd );
	}
	int numberOfAIRaces = startupData()->numPlayers() - 1;
	// Setup AI races and PC race
	if( gameData.randomStarts() == MachLog::RANDOM_START_LOCATIONS )
	{
		MexBasicRandom random( MexBasicRandom::constructSeededFromTime() );
		int playerNewColour = mexRandomInt( &random, 0, 4 );
		bool usedColour[ MachPhys::N_RACES ] = { false, false, false, false };
		creationData[playerNewColour].type_ = MachLog::PC_LOCAL;
		creationData[playerNewColour].colour_ = startupData()->playerRace();
		usedColour[ startupData()->playerRace() ] = true;
		while ( numberOfAIRaces != 0 )
		{
			int AIRace = mexRandomInt( &random, 0, 4 );
			if( creationData[ AIRace ].type_ == MachLog::NOT_DEFINED )
			{
				--numberOfAIRaces;
				creationData[ AIRace ].type_ = MachLog::AI_LOCAL;
				int i = 0;
				while( usedColour[ i ] ) ++i;
				creationData[ AIRace ].colour_ = (MachPhys::Race)i;
				usedColour[i] = true;
			}
		}
	}
	else
	{
		for( int i = 0; i < 4; ++i )
		{
			if ( (MachPhys::Race)i == startupData()->playerRace() )
			{
				creationData[i].type_ = MachLog::PC_LOCAL;
			}
			else if ( numberOfAIRaces != 0 )
			{
				--numberOfAIRaces;
				creationData[i].type_ = MachLog::AI_LOCAL;
			}
		}
	}
	gameData.playersCreationData( creationData );

	progressIndicator.report( 20, 100 );

	progressIndicator.setLimits( 0.2, 0.7 );

	// Load the game
	MachLogRaces::instance().loadGame( pSceneManager_, planet, scenario, gameData, &progressIndicator );
	MachLogRaces::instance().registerDispositionChangeNotifiable( pDispositionNotifiable_ );

	progressIndicator.setLimits( 0.0, 1.0 );

	progressIndicator.report( 70, 100 );

	// Initialise inGameScreen ( i.e. load correct map ).
    pInGameScreen_->loadGame( planet );

	progressIndicator.report( 85, 100 );

	// FOW is setable for skirmish games
	pInGameScreen_->fogOfWarOn( startupData()->fogOfWar() );

	GuiManager::instance().keyboardFocus( pInGameScreen_ );

//    pSceneManager_->pDevice()->display()->useCursor( pMenuCursor_ );

	gameType_ = SKIRMISHGAME;

	progressIndicator.report( 100, 100 );

    pSceneManager_->pDevice()->display()->useCursor( pMenuCursor_ );

	pInGameScreen_->becomeRoot();
	pInGameScreen_->activate();
	SimManager::instance().resume();

	if( getenv( "CB_PROFILE_PLANET_LOADING" ) )
        ProProfiler::instance().disableProfiling();

    //Touch all our own allocated memory to avoid thrashing after the game starts
    cbTouchAll();

    //  Give the AI a head start. This avoids chugging when we first get into the game
    for( size_t i = 0; i < 10; ++i )
    	SimManager::instance().cycle();
}

void MachGuiStartupScreens::switchGuiRootToMultiGame()
{
	CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ );
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL(	RenAnimCursor2d*, pMenuCursor_ );
	CB_DEPIMPL( MachGuiDispositionChangeNotifiable*, pDispositionNotifiable_ );

	PRE( gameType_ == NOGAME );
	HAL_STREAM("MachGuiStartupScreens::switchGuiRootToMultiGame\n" );

	// Display loading bmp
	GuiBitmap loadingBmp = Gui::bitmap( "gui/menu/loading.bmp" );
	loadingBmp.enableColourKeying();
	GuiBitmap frontBuffer = W4dManager::instance().sceneManager()->pDevice()->frontSurface();
	frontBuffer.simpleBlit( loadingBmp, xMenuOffset(), yMenuOffset() );
	// For double buffering call it twice to draw on both front and back buffers
	RenDevice::current()->display()->flipBuffers();
	frontBuffer.simpleBlit( loadingBmp, xMenuOffset(), yMenuOffset() );

	LoadGameProgressIndicator progressIndicator(this);

	// Free up any cached memory used by menus.
	releaseCachedMemory();

	startupData()->initMachLogNetwork();

	// Remove all chat messages from last multiplayer game
	MachGuiInGameChatMessages::instance().clearAllMessages();

	progressIndicator.report( 5, 100 );

	SimManager::instance().suspend();
	SimManager::instance().resetTime();

	string planet = startupData()->scenario()->planetFile();
	string scenario = startupData()->scenario()->name() + ".scn";

    //Clear the list of set flags in the database handler
    MachGuiDatabase::instance().handler().clearSetFlags();

	DEBUG_STREAM( DIAG_NETWORK, "MachLogRaces::instance().loadGame()" << std::endl );

	progressIndicator.report( 10, 100 );

	//Place user defined data in creation data arrays here
	MachLogGameCreationData gameData;

	gameData.randomStarts( startupData()->startingPosition() );
	gameData.resourcesAvailable( startupData()->resources() );
	gameData.startingResources( startupData()->startingResources() );
	gameData.technologyLevel( startupData()->techLevel() );
	setVictoryCondition( gameData );

	// Setup info about players
	MachLogGameCreationData::PlayersCreationData creationData;

	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
	{
		MachLogGameCreationData::PlayerCreationData pcd;
		pcd.type_ = MachLog::NOT_DEFINED;
		pcd.colour_ = i;
		creationData.push_back( pcd );
	}

	progressIndicator.report( 12, 100 );

	bool colourUsed[ MachPhys::N_RACES ] = {false,false,false,false };
	size_t numberOfPlayers = 0;
	for( int i = 0; i < 4; ++i )
	{
		MachGuiStartupData::PlayerInfo& playerInfo = startupData()->players()[i];
		HAL_STREAM("Setting up with player element " << playerInfo << std::endl );
		switch( playerInfo.status_ )
		{
			case MachGuiStartupData::PlayerInfo::HUMAN:
			{
				numberOfPlayers++;
				if( MachLogNetwork::instance().localRace() == playerInfo.race_ )
				{
					creationData[ playerInfo.race_ ].type_ = MachLog::PC_LOCAL;
					colourUsed[ playerInfo.race_ ] = true;
				}
				else
				{
					creationData[playerInfo.race_].type_ = MachLog::PC_REMOTE;
					colourUsed[ playerInfo.race_ ] = true;
				}

			}
			break;
			case MachGuiStartupData::PlayerInfo::COMPUTER:
			{
				numberOfPlayers++;
				if( MachLogNetwork::instance().isNodeLogicalHost() )
				{
					creationData[playerInfo.race_].type_ = MachLog::AI_LOCAL;
					colourUsed[ playerInfo.race_ ] = true;
				}
				else
				{
					creationData[playerInfo.race_].type_ = MachLog::AI_REMOTE;
					colourUsed[ playerInfo.race_ ] = true;
				}
			}
			break;
		}
	}

	NetNetwork::instance().messageThrottlingActive( true );
	NetNetwork::instance().autoAdjustMaxPacketsPerSecond( numberOfPlayers );


	progressIndicator.report( 15, 100 );

	//now we need to remark the colours of any unused ones.
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
	{
		if( creationData[i].type_ == MachLog::NOT_DEFINED )
		{
			//if the default colour for this element has been sued then we need to assign an unsed colour
			//so that MLScenario will cope correctly.
			if( colourUsed[ creationData[i].colour_ ] )
			{
				//find unsed colour
				for( MachPhys::Race j = MachPhys::RED; j < MachPhys::N_RACES; ++((int&)j) )
					if( not colourUsed[j] )
					{
						creationData[i].colour_ = j;
						colourUsed[j] = true;
						break;
					}
			}
		}
	}

	progressIndicator.report( 17, 100 );

	//if the game requires random starting locations then we can simply rejig the colours here
	//the random seed will have been passed through from the host so that all the random numbers
	//will be the same on all nodes.
	if( gameData.randomStarts() == MachLog::RANDOM_START_LOCATIONS )
	{
		//create a backup of the original data
		MachLogGameCreationData::PlayersCreationData originalCreationData;
		for( int i = 0; i < 4; ++i )
			originalCreationData.push_back( creationData[ i ] );
		//seed the random number generator with the value from startup
		MexBasicRandom randomGen;
		randomGen.seed( startupData()->randomStartSeed() );
		bool usedColour[4] = { false, false, false, false };
		//reassign the colours at random to give us random starting positions.
		for( int i = 0; i < 4; ++i )
		{
			int j = mexRandomInt( &randomGen, 0, 4 );
			while( usedColour[j] ) j = mexRandomInt( &randomGen, 0, 4 );
			originalCreationData[i].colour_ = (MachPhys::Race)j;
			usedColour[j] = true;
		}
		//reset the creation data array as all the values will be overwritten.
		for( int i = 0; i < 4; ++i )
			creationData[i].type_ = MachLog::NOT_DEFINED;
		//store back into cretionData array but swap index for colour as they are parsed the other way around.
		for( int i = 0; i < 4; ++i )
		{
			if( originalCreationData[i].type_ != MachLog::NOT_DEFINED )
			{
				creationData[ originalCreationData[i].colour_ ].type_ = originalCreationData[i].type_;
				creationData[ originalCreationData[i].colour_ ].colour_ = (MachPhys::Race)i;
			}
		}
	}

	gameData.playersCreationData( creationData );

	progressIndicator.report( 20, 100 );

	progressIndicator.setLimits( 0.2, 0.5 );

	// Load the game
	MachLogRaces::instance().loadGame( pSceneManager_, planet, scenario, gameData, &progressIndicator );
	MachLogRaces::instance().registerDispositionChangeNotifiable( pDispositionNotifiable_ );

	progressIndicator.setLimits( 0.0, 1.0 );

	progressIndicator.report( 50, 100 );

	DEBUG_STREAM( DIAG_NETWORK, "pInGameScreen_->loadGame()" << std::endl );

	// Initialise inGameScreen ( i.e. load correct map ).
    pInGameScreen_->loadGame( planet );

	progressIndicator.report( 60, 100 );

	// FOW is setable for multiplayer games
	pInGameScreen_->fogOfWarOn( startupData()->fogOfWar() );
	// First person can be disabled in multiplayer games
	pInGameScreen_->disableFirstPerson( startupData()->disableFirstPerson() );

	GuiManager::instance().keyboardFocus( pInGameScreen_ );

	DEBUG_STREAM( DIAG_NETWORK, "messageBroker().sendReadyMessage()" << std::endl );

	MachLogNetwork::instance().messageBroker().sendReadyMessage();
	SysWindowsAPI::peekMessage();
	SysWindowsAPI::sleep( 100 );

	progressIndicator.report( 70, 100 );

	float progress = 70;

	if( MachLogNetwork::instance().isNodeLogicalHost() )
	{
		bool allOk = false;
		PhysAbsoluteTime startTime = Phys::time();
		PhysAbsoluteTime lastStartMessageTime = Phys::time();
		while( not allOk and ( Phys::time() - startTime < MachLogRaces::instance().stats().connectionTime() ) )
		{
			bool checkAllOk = true;
			MachLogNetwork::instance().update();
   			for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++i )
				if( not MachLogNetwork::instance().ready( i ) )
					checkAllOk = false;
			allOk = checkAllOk;
			if( checkAllOk )
			{
				DEBUG_STREAM( DIAG_NETWORK, "checkAllOk" << std::endl );

				SimManager::instance().resetTime();
				MachLogNetwork::instance().messageBroker().sendResyncTimeMessage();
				MachLogNetwork::instance().messageBroker().sendStartGameMessage();
			}
			else
			{
				SysWindowsAPI::peekMessage();
				SysWindowsAPI::sleep( 100 );
				DEBUG_STREAM( DIAG_NETWORK, "Looking for ready status: at " << Phys::time() << " " );
				for(MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++i )
					DEBUG_STREAM( DIAG_NETWORK, i << " ready " << MachLogNetwork::instance().ready( i ) << " " );
				DEBUG_STREAM( DIAG_NETWORK, "\n" );
				if( Phys::time() - lastStartMessageTime > 3.0 )
				{
					lastStartMessageTime = Phys::time();
					messageBroker().sendStartMessage();
				}

				// Update progress bar just to show something is happening
				progress += 0.2;
				if ( progress > 90.0 )
					progress = 90.0;
				progressIndicator.report( (int)progress, 100 );
			}
		}
		SimManager::instance().resetTime();
		MachLogNetwork::instance().messageBroker().sendResyncTimeMessage();
		MachLogNetwork::instance().messageBroker().sendStartGameMessage();
		//Sleep to allow for lag
		SysWindowsAPI::sleep( 500 );
	}
	else
	{
		//if we aren't host then reissue the ready message every second
		SimManager::instance().suspend();
		// Following code ensures that the game starts at the same time for everyone...
		// Timeout and enter game after 60secs regardless of who is ready. This at least
		// gives the option of quitting the game if things have gone wrong without having to
		// kill the process.
		for( int i = 0; i < 60 and SimManager::instance().isSuspended(); ++i )
		{
			SysWindowsAPI::peekMessage();
			SysWindowsAPI::sleep( 1000 );
			MachLogNetwork::instance().messageBroker().sendReadyMessage();
			MachLogNetwork::instance().update();

			// Update progress bar just to show something is happening
			progress += 1.0;
			if ( progress > 90.0 )
				progress = 90.0;
			progressIndicator.report( (int)progress, 100 );
		}
	}

	progressIndicator.report( 90, 100 );

    pSceneManager_->pDevice()->display()->useCursor( pMenuCursor_ );

	gameType_ = MULTIGAME;

	progressIndicator.report( 100, 100 );

	DEBUG_STREAM( DIAG_NETWORK, "pInGameScreen_->becomeRoot()" << std::endl );

	pInGameScreen_->becomeRoot();
	pInGameScreen_->activate();

	DEBUG_STREAM( DIAG_NETWORK, "SimManager::instance().resume()" << std::endl );

	SimManager::instance().resume();

    //Touch all our own allocated memory to avoid thrashing after the game starts
    cbTouchAll();

	//  Give the AI a head start. This avoids chugging when we first get into the game
    for( size_t i = 0; i < 10; ++i )
    	SimManager::instance().cycle();
}

void MachGuiStartupScreens::setVictoryCondition( MachLogGameCreationData& gameData )
{
	switch ( startupData()->victoryCondition() )
	{
		case MachGuiStartupData::VC_DEFAULT:
			gameData.victoryCondition( MachLog::VICTORY_DEFAULT );
			break;
		case MachGuiStartupData::VC_TOTALANNIHILATION:
			gameData.victoryCondition( MachLog::VICTORY_ANNIHILATION );
			break;
		case MachGuiStartupData::VC_DESTROYPOD:
			gameData.victoryCondition( MachLog::VICTORY_POD );
			break;
		case MachGuiStartupData::VC_TIMED5:
			gameData.victoryCondition( MachLog::VICTORY_TIMER );
			gameData.timerTickAt( 300 );
			break;
		case MachGuiStartupData::VC_TIMED10:
			gameData.victoryCondition( MachLog::VICTORY_TIMER );
			gameData.timerTickAt( 600 );
			break;
		case MachGuiStartupData::VC_TIMED15:
			gameData.victoryCondition( MachLog::VICTORY_TIMER );
			gameData.timerTickAt( 900 );
			break;
		case MachGuiStartupData::VC_TIMED30:
			gameData.victoryCondition( MachLog::VICTORY_TIMER );
			gameData.timerTickAt( 1800 );
			break;
		case MachGuiStartupData::VC_TIMED45:
			gameData.victoryCondition( MachLog::VICTORY_TIMER );
			gameData.timerTickAt( 2700 );
			break;
		case MachGuiStartupData::VC_TIMED60:
			gameData.victoryCondition( MachLog::VICTORY_TIMER );
			gameData.timerTickAt( 3600 );
			break;
		case MachGuiStartupData::VC_TIMED90:
			gameData.victoryCondition( MachLog::VICTORY_TIMER );
			gameData.timerTickAt( 5400 );
			break;
		case MachGuiStartupData::VC_TIMED120:
			gameData.victoryCondition( MachLog::VICTORY_TIMER );
			gameData.timerTickAt( 7200 );
			break;
		case MachGuiStartupData::VC_TIMED180:
			gameData.victoryCondition( MachLog::VICTORY_TIMER );
			gameData.timerTickAt( 10800 );
			break;
	};
}

void MachGuiStartupScreens::buttonAction( ButtonEvent be )
{
	if ( be != NO_IMP )
	{
		buttonAction( be, "gui/sounds/click.wav" );
	}
	else
	{
		buttonAction( be, "gui/sounds/clickbad.wav" );
	}
}

void MachGuiStartupScreens::buttonAction( ButtonEvent be, const string& wavFile )
{
	CB_MachGuiStartupScreens_DEPIMPL();

	// Update last button event
	lastButtonEvent_ = be;

	// Play sound
	MachGuiSoundManager::instance().playSound( wavFile );

	// Dismiss message box?
	if ( pMsgBox_ and ( be == BE_OK or be == BE_CANCEL ) )
	{
		// Restore character focus
   		if ( pCharFocus_ )
		{
   	    	GuiManager::instance().charFocus( pCharFocus_ );
   		}

		bool deleteMsgBox = true;

		// Do we need to tell a message box responder about the button press?
		if ( pMsgBoxResponder_ )
		{
			if ( be == BE_OK )
				deleteMsgBox = pMsgBoxResponder_->okPressed();
			else
				deleteMsgBox = pMsgBoxResponder_->cancelPressed();

			if ( deleteMsgBox )
			{
				_DELETE( pMsgBoxResponder_ );
				pMsgBoxResponder_ = NULL;
			}
		}

		if ( deleteMsgBox )
		{
			_DELETE( pMsgBox_ );
			pMsgBox_ = NULL;
			// Redraw whole screen ( we can't be sure what GuiDisplayables the message box
			// displayed over ).
			changed();
		}
		return;
	}

	// General processing, transitions between screens.
	bool found = false;
	size_t loop = 0;

	while ( not found and getContextSwitchInfo()[loop].curContext_ != CTX_FINISH )
	{
		ContextSwitchInfo& csi = getContextSwitchInfo()[loop];

		if ( csi.curContext_ == context_ and
			 csi.buttonEvent_ == be )
		{
			if ( csi.playTransition_ and pStartupData_->transitionFlicsOn() )
			{
				contextBeforeFlic_ = context_;
				contextAfterFlic_ = csi.newContext_;
				switchContext( CTX_TRANSITION );
			}
			else
			{
				switchContext( csi.newContext_ );
			}
			found = true;
		}

		++loop;
	}

	// Default processing did not make use of the button event therefore allow current
	// context to process it.
	if ( not found and pCurrContext_ )
	{
		pCurrContext_->buttonEvent( be );
	}
}

void MachGuiStartupScreens::doDisplay()
{
	CB_DEPIMPL(	GuiBitmap*, pBackdrop_ );
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( Context, context_ );

	if ( context_ != CTX_LOADINGEXE and context_ != CTX_POSTLOADINGANIMATION )
	{
		const RenDisplay::Mode& mode = pSceneManager_->pDevice()->display()->currentMode();

		if ( mode.width() != 640 or mode.height() != 480 )
		{
			// Blit black on to backbuffer to stop previous screen from showing through black borders
			RenDevice::current()->backSurface().filledRectangle(  Ren::Rect(0,0,xMenuOffset(),mode.height()), RenColour::black() );
			RenDevice::current()->backSurface().filledRectangle(  Ren::Rect(0,0,mode.width(),yMenuOffset()), RenColour::black() );
			RenDevice::current()->backSurface().filledRectangle(  Ren::Rect(0,mode.height()-yMenuOffset(),mode.width(),mode.height()), RenColour::black() );
			RenDevice::current()->backSurface().filledRectangle(  Ren::Rect(mode.width()-xMenuOffset(),0,xMenuOffset(),mode.height()), RenColour::black() );
		}
	}

	if ( pBackdrop_ )
	{
		GuiPainter::instance().blit( *pBackdrop_, Gui::Coord( xMenuOffset(), yMenuOffset() ) );
	}
}

bool MachGuiStartupScreens::finishApp()
{
	CB_DEPIMPL(	bool, finishApp_ );

	return finishApp_;
}

void MachGuiStartupScreens::switchContext( Context newContext )
{
	CB_DEPIMPL(	MachGuiStartupScreenContext*, pCurrContext_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );
	CB_DEPIMPL(	bool, switchGuiRoot_ );
	CB_DEPIMPL(	GuiBitmap*, pBackdrop_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, contextAfterFlic_ );
	CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ );
	CB_DEPIMPL(	PhysAbsoluteTime, contextTimer_ );
	CB_DEPIMPL(	bool, isGamePaused_ );

	NETWORK_STREAM("MachGuiStartupScreens::switchContext current " << context_ << " new context " << newContext << std::endl );
	NETWORK_INDENT( 2 );

	DEBUG_STREAM( DIAG_NEIL, "MachGuiStartupScreens::switchContext enter (" << newContext << ")" << std::endl );

	// Switch away from context. You can abort a context change at this point if, for example, not
	// enough info has been entered to proceed to the next screen.
	NETWORK_STREAM("(void*)pCurrContext_ " << (void*)pCurrContext_ << std::endl );
	if ( pCurrContext_ )
	{
		if ( not pCurrContext_->okayToSwitchContext() )
		{
			NETWORK_STREAM("not ok to switch context returning\n" );
			NETWORK_INDENT( -2 );
			return;
		}
	}

	switch ( context_ )
	{
		case CTX_GAME:
		case CTX_MULTI_GAME:
		case CTX_SKIRMISH_GAME:
			switchGuiRoot_ = true;
			isGamePaused_ = SimManager::instance().isSuspended();
			break;
		case CTX_LOADINGEXE:
			// Blit picture on to backbuffer to stop horrible screen flash when
			// post loading animation starts playing.
// No longer needed, caused some junk to appear on screen
//			RenDevice::current()->backSurface().simpleBlit( RenDevice::current()->frontSurface() );
			break;
	}

	// Clean up controls from last context
	deleteAllChildren();
	_DELETE( pBackdrop_ );
	pBackdrop_ = NULL;
	_DELETE( pCurrContext_ );
	pCurrContext_ = NULL;

    //Stop playing any animation
    stopPlayingAnimation();
	clearAllSmackerAnimations();

	// Set new context
	context_ = newContext;

	// Switch to context
	switch ( context_ )
	{
		case CTX_MAINMENU:
			pCurrContext_ = _NEW( MachGuiCtxMainMenu( this ) );
			break;
		case CTX_SINGLEPLAYER:
			pCurrContext_ = _NEW( MachGuiCtxSinglePlayer( this ) );
			break;
		case CTX_JOIN:
			pCurrContext_ = _NEW( MachGuiCtxJoin( this ) );
			break;
		case CTX_IMREADY:
			pCurrContext_ = _NEW( MachGuiCtxImReady( this ) );
			break;
		case CTX_CAMPAIGN:
			pCurrContext_ = _NEW( MachGuiCtxCampaign( this ) );
			break;
		case CTX_MULTIPLAYER:
			pCurrContext_ = _NEW( MachGuiCtxMultiplayer( this ) );
			break;
		case CTX_HOTKEYS:
			pCurrContext_ = _NEW( MachGuiCtxHotKeys( this ) );
			break;
		case CTX_SAVE:
			pCurrContext_ = _NEW( MachGuiCtxSave( this ) );
			break;
		case CTX_LOAD:
		case CTX_IGLOAD:
			pCurrContext_ = _NEW( MachGuiCtxLoad( this ) );
			break;
		case CTX_BRIEFING:
		case CTX_IGBRIEFING:
			pCurrContext_ = _NEW( MachGuiCtxBriefing( this ) );
			break;
		case CTX_CADEBRIEFING:
		case CTX_MPDEBRIEFING:
		case CTX_SKDEBRIEFING:
			pCurrContext_ = _NEW( MachGuiCtxDeBriefing( this ) );
			break;
		case CTX_CASTATISTICS:
		case CTX_MPSTATISTICS:
		case CTX_SKSTATISTICS:
			pCurrContext_ = _NEW( MachGuiCtxStatistics( this ) );
			break;
		case CTX_SCENARIO:
			pCurrContext_ = _NEW( MachGuiCtxScenario( this ) );
			break;
		case CTX_SKIRMISH:
			pCurrContext_ = _NEW( MachGuiCtxSkirmish( this ) );
			break;
		case CTX_SETTINGS:
			pCurrContext_ = _NEW( MachGuiCtxSettings( this ) );
			break;
		case CTX_INGAMEOP:
			pCurrContext_ = _NEW( MachGuiCtxInGameOptions( this ) );
			switch ( gameType_ )
			{
				case MULTIGAME:
					context_ = CTX_MPINGAMEOPTIONS;
					break;
				case CAMPAIGNGAME:
					context_ = CTX_CAINGAMEOPTIONS;
					break;
				case SKIRMISHGAME:
					context_ = CTX_SKINGAMEOPTIONS;
					break;
				DEFAULT_ASSERT_BAD_CASE( gameType_ );
			}
			break;
		case CTX_OPTIONS:
		case CTX_IGOPTIONS:
			pCurrContext_ = _NEW( MachGuiCtxOptions( this ) );
			break;
		case CTX_GAME:
		case CTX_MULTI_GAME:
		case CTX_SKIRMISH_GAME:
		case CTX_BACKTOGAME:
			contextGame();
			break;
		case CTX_PROBEACCLAIMLOGO:
		case CTX_CHARYBDISLOGO:
		case CTX_LEGALSCREEN:
	#ifdef DEMO
 		case CTX_SPLASH1:
		case CTX_SPLASH2:
	#endif
			contextLogo();
			break;
		case CTX_VICTORY:
			contextVictory();
			break;
		case CTX_DEFEAT:
			contextDefeat();
			break;
		case CTX_FINISH:
			contextFinish();
			break;
		case CTX_TRANSITION:
		case CTX_INTROANIMATION:
		case CTX_POSTLOADINGANIMATION:
		case CTX_VICTORYFLIC:
		case CTX_DEFEATFLIC:
		case CTX_ENTRYFLIC:
			contextAnimation();
			break;
		case CTX_TRANSITIONEND:
			switchContext( contextAfterFlic_ );
			break;
		case CTX_DEBRIEFING:
			switch ( gameType_ )
			{
				case MULTIGAME:
					switchContext( CTX_MPDEBRIEFING );
					break;
				case CAMPAIGNGAME:
					switchContext( CTX_CADEBRIEFING );
					break;
				case SKIRMISHGAME:
					switchContext( CTX_SKDEBRIEFING );
					break;
				DEFAULT_ASSERT_BAD_CASE( gameType_ );
			}
			break;
	}

	// Reset context timer. This times how long you've been in a particular context.
	contextTimer_ = Phys::time();
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("MachGuiStartupScreens::switchContext DONE\n" );

	DEBUG_STREAM( DIAG_NEIL, "MachGuiStartupScreens::switchContext leave (" << newContext << ")" << std::endl );
}



//virtual
void MachGuiStartupScreens::update()
{
	CB_DEPIMPL(	MachGuiAutoDeleteDisplayable*, pMustContainMouse_ );
	CB_DEPIMPL(	MachGuiStartupScreenContext*, pCurrContext_ );
	CB_DEPIMPL(	MachGuiMessageBox*, pMsgBox_ );

	// Check that mouse is contained within the auto delete gui. See header for description
	// of this functionality.
	if ( pMustContainMouse_ )
	{
		if ( not pMustContainMouse_->containsMousePointer() )
		{
			_DELETE( pMustContainMouse_ );
			pMustContainMouse_ = NULL;
		}
		else
		{
			pMustContainMouse_->update();
		}
	}

	// Update the current context if a message box is not being displayed.
	if ( pCurrContext_ and not pMsgBox_ )
	{
		pCurrContext_->update();
	}

	if ( pMsgBox_ )
		pMsgBox_->update();
}

//virtual
bool MachGuiStartupScreens::doHandleRightClickEvent( const GuiMouseEvent& )
{
	return true;
}

void MachGuiStartupScreens::setGuiViewport()
{
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );

    //Set the viewport boundary to the entire screen.
    RenDevice& device = *pSceneManager_->pDevice();
    const int w = device.windowWidth();
    const int h = device.windowHeight();
	device.setViewport(0, 0, w, h);
}

//virtual
void MachGuiStartupScreens::doBecomeRoot()
{
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );

	// Use 640x480 menu resolution
	if ( not SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Lock Resolution", SysRegistry::CURRENT_USER ) )
	{
//		pSceneManager_->pDevice()->display()->useMode(640, 480, 0);

		const RenDisplay::Mode& mode = pSceneManager_->pDevice()->display()->currentMode();
		DevMouse::instance().scaleCoordinates(mode.width(), mode.height());
	}

	absoluteCoord( Gui::Coord( xMenuOffset(),yMenuOffset() ) );
}

//virtual
void MachGuiStartupScreens::doBecomeNotRoot()
{
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL(	int, inGameResolutionWidth_ );
	CB_DEPIMPL(	int, inGameResolutionHeight_ );
	CB_DEPIMPL(	int, inGameResolutionRate_ );

	// Switch to ingame resolution
	int oldWidth = inGameResolutionWidth_;
	int oldHeight = inGameResolutionHeight_;

	if( SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Lock Resolution", SysRegistry::CURRENT_USER ) == 0 )
	{

		inGameResolutionWidth_ = SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Width", SysRegistry::CURRENT_USER );
		inGameResolutionHeight_ = SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Height", SysRegistry::CURRENT_USER );
        inGameResolutionRate_ = SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Refresh Rate", SysRegistry::CURRENT_USER );

		// Check that minimum resolution is specified
		if ( inGameResolutionWidth_ < 640 or inGameResolutionHeight_ < 480 )
		{
			inGameResolutionWidth_ = 640;
			inGameResolutionHeight_ = 480;
		}

		if ( oldWidth != inGameResolutionWidth_ or oldHeight != inGameResolutionHeight_ )
		{
			pInGameScreen_->resolutionChange();
		}

//		pSceneManager_->pDevice()->display()->useMode(inGameResolutionWidth_, inGameResolutionHeight_, inGameResolutionRate_);

		const RenDisplay::Mode& mode = pSceneManager_->pDevice()->display()->currentMode();
		DevMouse::instance().scaleCoordinates(mode.width(), mode.height());
	}
	//Stop all playing sounds
	W4dSoundManager::instance().stopAll();
}


RenAnimCursor2d* MachGuiStartupScreens::loadCursor( const string& baseName, int nFrames )
{
    //Get the back buffer
    const RenSurface& backBuffer = Gui::backBuffer();

    //Create the main cursor
    RenAnimCursor2d* pCursor = _NEW( RenAnimCursor2d );

    //Create and add the surface for each frame
    int width = 0;
    int height = 0;
    char index = '1';
    for( int i = 0; i < nFrames; ++i )
    {
        //Construct the full name
        string name = "gui/cursor/";
        name += baseName;
        if( nFrames > 1 )
            name += index;
        name += ".bmp";
        ++index;

        //Create the surface
        RenSurface surface = RenSurface::createSharedSurface( name, backBuffer );
        surface.enableColourKeying();

        //Get the surface dimensions
        width = surface.width();
        height = surface.height();

        //Add to the cursor
        pCursor->addFrame( surface );
    }

    //Set the origin
    pCursor->origin( width / 2, height / 2 );

    //set the frame rate
    pCursor->targetFrequency( 10 );

    return pCursor;
}

void MachGuiStartupScreens::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiStartupScreens& t )
{

    o << "MachGuiStartupScreens " << (void*)&t << " start" << std::endl;
    o << "MachGuiStartupScreens " << (void*)&t << " end" << std::endl;

    return o;
}

void MachGuiStartupScreens::loopCycleStartupScreens()
{
	CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ );
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );

	updateCdAudio();

	// Even though we are in the startup screens if there is a multi player
	// game going on then we need to keep updating our race. If we are the host then we
	// should continue to resync.
	if ( gameType_ == MULTIGAME )
	{
    	SimManager::instance().cycle();
		MachLogRaces::instance().specialActorUpdate();
	}

	if (pSceneManager_->pDevice()->startFrame())
	{
		checkContextTimeout();

		// Update does not do any drawing. It is called first because it locks the mouse
		// position for the duration of the render cycle, therefore any 3D and 2D cursors
		// appear in the same position.
		updateGui();

		pSceneManager_->pDevice()->start2D();
	  	displayGui();
		playSmackerAnimations();
		pSceneManager_->pDevice()->end2D();

		pSceneManager_->pDevice()->endFrame();
	}

	// Make sure new gui sounds are played
	SndMixer::instance().update();
	MachGuiSoundManager::instance().update();
}

void MachGuiStartupScreens::checkContextTimeout()
{
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );
	CB_DEPIMPL(	PhysAbsoluteTime, contextTimer_ );

	PhysAbsoluteTime newTime = Phys::time();

	bool found = false;
	size_t loop = 0;

	while ( not found and getContextTimeoutInfo()[loop].curContext_ != CTX_FINISH )
	{
		// Have we found the correct entry in the timeout table...
		if ( getContextTimeoutInfo()[loop].curContext_ == context_ )
		{
			// Are we dealing with a TIMEOUT event?
			if ( getContextTimeoutInfo()[loop].type_ == ContextTimeoutInfo::TIMEOUT )
			{
				if ( newTime - contextTimer_  > getContextTimeoutInfo()[loop].timeInfo_ )
				{
					// Switch to new context if timeout has occured
					switchContext( getContextTimeoutInfo()[loop].newContext_ );
				}
			}
			// Are we dealing with a FLIC_FINISHED event ( animation ended )?
			else if ( getContextTimeoutInfo()[loop].type_ == ContextTimeoutInfo::FLIC_FINISHED )
			{
				if ( animationFinished() )
				{
					// Switch to new context if animation has ended
					switchContext( getContextTimeoutInfo()[loop].newContext_ );
				}
			}
			found = true;
		}

		++loop;
	}
}

void MachGuiStartupScreens::loopCycleInGame()
{
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );

 	PRE( context_ == CTX_GAME or
 		 context_ == CTX_MULTI_GAME or
 		 context_ == CTX_SKIRMISH_GAME );

#ifndef PRODUCTION

	/*
	//CERI_CODE_CERI_CODE_CERI_CODE_CERI_CODE_CERI_CODE_CERI_CODE_CERI_CODE_CERI_CODE_
	// Note - shut your face, Armstrong.

	const RecRecorder& recorder = RecRecorder::instance();
	if( recorder.state() == RecRecorder::PLAYING
		and recorder.percentageComplete() >= 99.97 )
	{
		ProProfiler::instance().enableProfiling();
	}
	//CERI_CODE_CERI_CODE_CERI_CODE_CERI_CODE_CERI_CODE_CERI_CODE_CERI_CODE_CERI_CODE_
	*/

    static bool firstTime = true;
    static double proChugTime = 0.0;
    static PhysAbsoluteTime cycleStartTime = Phys::time();
    if( firstTime )
    {
        firstTime = false;
        if( getenv("CB_CHUG") != NULL )
        {
            proChugTime = atof( getenv("CB_CHUG") ) / 1000.0;
            ProProfiler::instance().isBufferingOutput( true );
        }

		if( getenv( "CB_PROFILE_STARTGAME" ) )
            ProProfiler::instance().enableProfiling();
    }

    if( proChugTime != 0.0 )
    {
        PhysAbsoluteTime now = Phys::time();
        if( ProProfiler::instance().isProfilingEnabled()  and  (now - cycleStartTime > proChugTime) )
        {
            ProProfiler::instance().outputStream() << std::endl << "CHUG on frame " << W4dManager::instance().frameNumber() << " frame time " << (now - cycleStartTime) << std::endl;
            ProProfiler::instance().writeBuffer( ProProfiler::instance().outputStream() );
        }
        else
            ProProfiler::instance().clearBuffer();

        cycleStartTime = now;
    }
#endif //#ifndef PRODUCTION

    //Update world4d
    W4dManager& w4dManager = W4dManager::instance();
    w4dManager.update();

    //Advances the time and does simulation updates
	// If we're rendering a high-quality screen shot, then don't advance time.
	if (!pInGameScreen_->isRenderingScreenShot())
	{
    	SimManager::instance().cycle();
		MachLogRaces::instance().specialActorUpdate();
	}

	//Update networking, including remote first person handlers
    if( MachLogNetwork::instance().isNetworkGame() )
        MachLogRaces::instance().remoteFirstPersonManager().update();

	// If time is suspended for profiling, make the general position ID
	// change, so that W4dEntity evaluates transforms is if time were advancing.
	static const bool timeSuspended = getenv("SUSPEND_TIME") != NULL;
	if (timeSuspended)
		W4dManager::instance().generateGeneralPositionId();

	if (pSceneManager_->pDevice()->startFrame())
	{
		// Hack.... If in first person then we need to reverse the updateGui() and updateCameras() calls.
		if ( pInGameScreen_->inFirstPerson() )
		{
			// Update does not do any drawing. It is called first because it locks the mouse
			// position for the duration of the render cycle, therefore any 3D and 2D cursors
			// appear in the same position.
	    	// JLG : moved this from if block below, because 1st person needs updating before the
	    	// cameras.
			updateGui();

			// Updates camera domains and does PhysMotionControl update stuff.
			pInGameScreen_->updateCameras();
		}
		else
		{
			pInGameScreen_->updateCameras();

			updateGui();
		}

	    //Render the scene
	    w4dManager.render();

		// The first 2D blit sometimes has to wait for the 3D rendering to finish
		// (it's asynchronous).  Do as much work as possible between 2D and 3D.
		// TBD: it would still be advantageous to move even more stuff here.
		// RENDER_STREAM("Starting pipelined loopCycle stuff.\n");
		// RENDER_INDENT(2);
		// DevTimer timer;
		pInGameScreen_->asynchronousUpdate();
	    updateSound( earTransform() );
		updateCdAudio();
		// RENDER_STREAM("Did " << timer.timeNoRecord() * 1000 << "ms worth of stuff between 3D and 2D\n");
		// RENDER_INDENT(-2);

		pSceneManager_->pDevice()->start2D();
	  	displayGui();
		pSceneManager_->pDevice()->end2D();

		pSceneManager_->pDevice()->endFrame();

		// If we just rendered a high-quality screen shot, we need to save it.
		if (pInGameScreen_->isRenderingScreenShot())
			pInGameScreen_->finalizeScreenShot();
	}

	// Check for switch to in-game options, won or lost
	if ( pInGameScreen_->switchToMenus() or pInGameScreen_->gameState() != MachInGameScreen::PLAYING )
	{
		//Stop all playing 3D sounds 'cus we are going into menus
		W4dSoundManager::instance().stopAll();
		// Clean up any gui sounds that are currently playing
		MachGuiSoundManager::instance().clearAll();

		switch ( pInGameScreen_->gameState() )
		{
			case MachInGameScreen::PLAYING :
				switchContext( CTX_INGAMEOP );
				break;
			case MachInGameScreen::WON :
				switchContext( CTX_VICTORYFLIC );
				break;
			case MachInGameScreen::LOST :
				if( MachLogNetwork::instance().isNetworkGame() )
				{
					messageBroker().sendIveLostMessage( startupData()->playerName() );
				}
				switchContext( CTX_DEFEATFLIC );
				break;
 		}
	}

	//if we are the host then resync every 5 seconds or so.
	//the resync will hopefully take care of the effective ping rates as well to get a correct time.
	static PhysAbsoluteTime lastSyncTime = 0;
	if( MachLogNetwork::instance().isNodeLogicalHost() )
	{
		PhysAbsoluteTime now = SimManager::instance().currentTime();
		if( ( now - lastSyncTime ) > 5 )
		{
			MachLogNetwork::instance().messageBroker().sendResyncTimeMessage();
			lastSyncTime = now;
		}
	}

	// Instant exit from ingame screen
	if ( pInGameScreen_->instantExit() )
	{
		unloadGame();
		W4dSoundManager::instance().unloadAll();
		//if we got into this screen via a lobby session then we need to terminate correctly at this point.
		if( MachLogNetwork::instance().isNetworkGame() )
		{
			MachLogNetwork::instance().terminateAndReset();
		}
		contextFinishFromLobby();
	}
}

void MachGuiStartupScreens::updateSound( const MexTransform3d& transf )
{
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );

    if( pInGameScreen_->actualGameState() == MachInGameScreen::PLAYING )
    {
	    W4dSoundManager::instance().updateVolumes( transf );
    }
}

//virtual
bool MachGuiStartupScreens::doHandleKeyEvent( const GuiKeyEvent& e )
{
	CB_DEPIMPL(	GuiDisplayable*, pCharFocus_ );
	CB_DEPIMPL(	MachGuiMessageBoxResponder*, pMsgBoxResponder_ );
	CB_DEPIMPL(	MachGuiMessageBox*, pMsgBox_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );
	CB_DEPIMPL(	MachGuiStartupScreenContext*, pCurrContext_ );
	CB_DEPIMPL(	ButtonEvent, lastButtonEvent_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, contextAfterFlic_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, contextBeforeFlic_ );
	CB_DEPIMPL(	MachGuiStartupData*, pStartupData_ );

	bool processed = false;

	if (e.state() == Gui::PRESSED )
	{
		// Save menu screen shot
		if ( e.key() == DevKey::F12 and e.isShiftPressed() and e.isCtrlPressed() )
		{
			Gui::writeScreenAsBmp( "menu" );
		}

		// Do we have a control with focus that can respond to the key press?
		processed = doHandleFocusCapableControls( e );

		// Are we dismissing a message box?
		if ( not processed and pMsgBox_ )
		{
			processed = true;

			if ( e.key() == DevKey::ESCAPE or e.key() == DevKey::ENTER )
			{
				// Restore character focus
		   		if ( pCharFocus_ )
				{
		   	    	GuiManager::instance().charFocus( pCharFocus_ );
		   		}

				bool deleteMsgBox = true;

				// Do we need to tell a message box responder about the key press?
				if ( pMsgBoxResponder_ )
				{
					if ( e.key() == DevKey::ENTER )
						deleteMsgBox = pMsgBoxResponder_->okPressed();
					else
						deleteMsgBox = pMsgBoxResponder_->cancelPressed();

					if ( deleteMsgBox )
					{
						_DELETE( pMsgBoxResponder_ );
						pMsgBoxResponder_ = NULL;
					}
				}

				if ( deleteMsgBox )
				{
					_DELETE( pMsgBox_ );
					pMsgBox_ = NULL;
					// Redraw whole screen ( we can't be sure what GuiDisplayables the message box
					// displayed over ).
					changed();
				}
			}
		}

		size_t loop = 0;

		// General processing for flics. Space and Escape cause flic to abort ( forced timeout!! )
		if ( ( isContextFlic() and e.key() == DevKey::ESCAPE ) or
			 ( isContextFlic() and e.key() == DevKey::SPACE ) )
		{
			while ( not processed and getContextTimeoutInfo()[loop].curContext_ != CTX_FINISH )
			{
				// Have we found the correct entry in the timeout table...
				if ( getContextTimeoutInfo()[loop].curContext_ == context_ )
				{
					// Switch to new context
					switchContext( getContextTimeoutInfo()[loop].newContext_ );
					processed = true;
				}

				++loop;
			}
		}

		// More specific processing...
		loop = 0;

		while ( not processed and getContextKeypressInfo()[loop].curContext_ != CTX_FINISH )
		{
			ContextKeypressInfo& cki = getContextKeypressInfo()[loop];

			// Have we found the correct entry in the keypress table...
			if ( cki.curContext_ == context_ and e.key() == cki.scanCode_ )
			{
				// Switch to new context if correct keypress has occured...
				// When pressing a key we are simulating an on screen button being pressed,
				// store the button value that we are pressing.
				lastButtonEvent_ = cki.simButtonEvent_;

				if ( lastButtonEvent_ != NO_IMP )
				{
					//Set up the sound to be played. Only play sound if keypress is equivalent
					//to pressing an onscreen button
					MachGuiSoundManager::instance().playSound( "gui/sounds/click.wav" );
				}

				// Are we going to play a screen transition? If so store info so that when flic ends
				// we know what context to switch to.
				if ( cki.playTransition_ and pStartupData_->transitionFlicsOn() )
				{
					contextBeforeFlic_ = context_;
					contextAfterFlic_ = cki.newContext_;
					switchContext( CTX_TRANSITION );
				}
				else
				{
					switchContext( cki.newContext_ );
				}
				processed = true;
			}

			++loop;
		}
	}

	// Still no one has processed the button event? Allow current context object to have a go...
	if ( not processed and pCurrContext_ )
	{
		processed = pCurrContext_->doHandleKeyEvent( e );
	}

	return processed;
}

MexTransform3d MachGuiStartupScreens::earTransform() const
{
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );

    //get the current camera. Location to use for ear depends on whether in zenith view.
 	MexTransform3d xform = pSceneManager_->currentCamera()->globalTransform();
    MexPoint3d earPosition;

    static bool isZenithActive = not pInGameScreen_->cameras()->isZenithCameraActive();
    if(isZenithActive != pInGameScreen_->cameras()->isZenithCameraActive())
    {
	    isZenithActive = pInGameScreen_->cameras()->isZenithCameraActive();
   	    W4dSoundManager::instance().scalingActivate(isZenithActive);
	}

	if( isZenithActive )
	{
		MexEulerAngles theAngles;
		xform.rotation(&theAngles);
		MexDegrees theDegrees(theAngles.elevation());

        MexVec3 lineOfSight = xform.xBasis();
        const MexPoint3d& eyeLocation = xform.position();

        //Find the terrain height at this 2d location, and choose a z coordinate that is
        //the lower of the eye height and 75m.
		MATHEX_SCALAR earTop = pInGameScreen_->cameras()->zenithMaximumEarHeight();
		MATHEX_SCALAR earBottom = pInGameScreen_->cameras()->zenithMinimumEarHeight();
		MATHEX_SCALAR eyeTop = pInGameScreen_->cameras()->zenithMaximumHeight();
		MATHEX_SCALAR eyeBottom = pInGameScreen_->cameras()->zenithMinimumHeight();

        //Find the intersection of the line-of-sight with z=0
        MATHEX_SCALAR lambda = -eyeLocation.z() / lineOfSight.z();

	    MATHEX_SCALAR xEar = eyeLocation.x() + lambda * lineOfSight.x();
        MATHEX_SCALAR yEar = eyeLocation.y() + lambda * lineOfSight.y();
		MATHEX_SCALAR zEar = 0;

		//If we are operating in terms of 2d sound then scale the zenith ear
		if(W4dSoundManager::instance().currentSoundDimensions() == Snd::TWO_D)
		{
	        zEar = MachLogPlanet::instance().surface()->terrainHeight( xEar, yEar );
	        zEar += 75.0;
	        if( eyeLocation.z() < zEar )
    	        zEar = eyeLocation.z();
			MATHEX_SCALAR eyePercentage = (zEar - eyeBottom)/(eyeTop-eyeBottom);
   			zEar = ((earTop-earBottom) * eyePercentage) + earBottom;
	        //Calculate and set the amount of scaling required by the zenith view
    	    MATHEX_SCALAR scaleMax = W4dSoundManager::instance().maxScaleFactor();
        	MATHEX_SCALAR scaleMin = W4dSoundManager::instance().minScaleFactor();
	        MATHEX_SCALAR scale = ((scaleMax-scaleMin) * eyePercentage) + scaleMin;
    	    W4dSoundManager::instance().setScaleFactor(scale);
	        earPosition.setPoint( xEar, yEar, zEar );
		}
		else if(W4dSoundManager::instance().currentSoundDimensions() == Snd::THREE_D)
		{
			earPosition = eyeLocation;
			MexPoint3d pointBelowCamera = MachLogPlanet::instance().surface()->terrainHeight( eyeLocation.x(), eyeLocation.y() );

			MATHEX_SCALAR distanceFromGround = eyeLocation.euclidianDistance(pointBelowCamera);
			if(distanceFromGround > eyeLocation.z() && eyeLocation.z() > 0)
			{
				distanceFromGround = eyeLocation.z();
			}
			MexVec3 scaledLineofsight(lineOfSight);
			scaledLineofsight *= distanceFromGround;
			earPosition += scaledLineofsight;
		}

		xform.position(earPosition);
    }
	return xform;
}

void MachGuiStartupScreens::startPlayingAnimation( const SysPathName& filename, bool fast )
{
	CB_DEPIMPL(	AniSmacker*, pPlayingSmacker_ );

    //Stop any currently playing animation
    if( pPlayingSmacker_ )
        stopPlayingAnimation();

    //Construct a smacker player
//    HWND targetWindow = RenDevice::current()->display()->window();
//    pPlayingSmacker_ = _NEW( AniSmacker( filename, targetWindow, xMenuOffset(), yMenuOffset(), fast ) );
    //pPlayingSmacker_ = _NEW( AniSmacker( filename, xMenuOffset(), yMenuOffset(), fast ) );
    pPlayingSmacker_ = new AniSmackerRegular(filename, xMenuOffset(), yMenuOffset(), fast);

	// Remove mouse pointer
    cursorOn( false );
}

void MachGuiStartupScreens::startPlayingAnimation( const SysPathName& filename, bool fast, bool frontBuffer, const Gui::Coord& pos, bool isCutscene )
{
	CB_DEPIMPL(	AniSmacker*, pPlayingSmacker_ );

    //Stop any currently playing animation
    if( pPlayingSmacker_ )
        stopPlayingAnimation();

    //Construct a smacker player
//    HWND targetWindow = RenDevice::current()->display()->window();
//    pPlayingSmacker_ = _NEW( AniSmacker( filename, targetWindow, pos.x() + xMenuOffset(), pos.y() + yMenuOffset(), fast ) );
    //pPlayingSmacker_ = _NEW( AniSmacker( filename, pos.x() + xMenuOffset(), pos.y() + yMenuOffset(), fast ) );

    if (not isCutscene) {
        pPlayingSmacker_ = new AniSmackerRegular(filename, pos.x() + xMenuOffset(), pos.y() + yMenuOffset(), fast);
    }
    else {
        const auto& displayMode = RenDevice::current()->display()->currentMode();
        pPlayingSmacker_ = new AniSmackerCutscene(filename, 0, 0, displayMode.width(), displayMode.height());
    }

	if ( frontBuffer )
		pPlayingSmacker_->useFrontBuffer( frontBuffer );

	// Remove mouse pointer
    cursorOn( false );
}

void MachGuiStartupScreens::stopPlayingAnimation()
{
	CB_DEPIMPL(	AniSmacker*, pPlayingSmacker_ );

    //Stop any currently playing animation
    if( pPlayingSmacker_ )
    {
        _DELETE( pPlayingSmacker_ );
        pPlayingSmacker_  = NULL;
    }
}

void MachGuiStartupScreens::loopCyclePlayingAnimation()
{
	CB_DEPIMPL(	AniSmacker*, pPlayingSmacker_ );
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );

    PRE( pPlayingSmacker_ != NULL );

	updateCdAudio();

    //Get key events. False return indicates should quit the animation.
    if( not GuiManager::instance().update()  and  pPlayingSmacker_ != NULL )
        stopPlayingAnimation();

    //The animation may already have been finished due to a keyboard event
    if( animationFinished() )
    {
        stopPlayingAnimation();
    }
    else
    {
		bool started = true;
		if ( not pPlayingSmacker_->useFrontBuffer() )
			started = pSceneManager_->pDevice()->startFrame();

		if ( started )
		{
        	pPlayingSmacker_->playNextFrame( RenDevice::current() );

    		if ( not pPlayingSmacker_->useFrontBuffer() )
		  		pSceneManager_->pDevice()->endFrame();
		}
    }
}

bool MachGuiStartupScreens::animationFinished()
{
	CB_DEPIMPL(	AniSmacker*, pPlayingSmacker_ );

    return pPlayingSmacker_ == NULL  or  pPlayingSmacker_->isFinished();
}

void MachGuiStartupScreens::updateCdAudio()
{
	CB_DEPIMPL(	MachGuiStartupScreens::Music, playingCdTrack_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Music, desiredCdTrack_ );
	CB_DEPIMPL(	PhysAbsoluteTime, cdCheckTime_ );
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );

	DevCD::instance().update();
	// Check to see if we are playing a game that has just finished. If so, play
	// either victory or defeat music.
	if ( context_ == CTX_GAME or
		 context_ == CTX_MULTI_GAME or
		 context_ == CTX_SKIRMISH_GAME )
	{
		if ( not MachGuiDatabase::instance().currentScenario().isTrainingScenario() )
		{
			if ( pInGameScreen_->actualGameState() == MachInGameScreen::WON )
			{
				desiredCdTrack_ = VICTORY_MUSIC;
			}
			else if ( pInGameScreen_->actualGameState() == MachInGameScreen::LOST )
			{
				desiredCdTrack_ = DEFEAT_MUSIC;
			}
		}
	}

	// Don't hang around switching to desired track!!!
	if ( playingCdTrack_ != desiredCdTrack_ )
	{
		// Stop any currently playing tunes
		if ( DevCD::instance().isPlayingAudioCd() )
		{
			DevCD::instance().stopPlaying();
		}

		if ( desiredCdTrack_ != DONT_PLAY_CD )
		{
			if ( DevCD::instance().isAudioCDPresent() )
			{
				// Start to play desired track
				bool repeat =  not ( desiredCdTrack_ == VICTORY_MUSIC or desiredCdTrack_ == DEFEAT_MUSIC );

				if ( desiredCdTrack_ + 1 < DevCD::instance().numberOfTracks() )
				{
					if ( desiredCdTrack_ >= LOADING_MUSIC )
					{
						DevCD::instance().randomPlay( LOADING_MUSIC + 1, DevCD::instance().numberOfTracks() - 1, desiredCdTrack_ + 1 );
					}
					else
					{
						DevCD::instance().play( desiredCdTrack_ + 1, repeat );
					}
				}

				cdCheckTime_ =  Phys::time();
			}
		}

		playingCdTrack_ = desiredCdTrack_;
	}
	else if ( desiredCdTrack_ != DONT_PLAY_CD ) // Check, once in a while, whether the cd has stopped. Restart the desired track
	{
		if ( Phys::time() - cdCheckTime_ > 60.0 )
		{
			cdCheckTime_ =  Phys::time();

			// Check to see if we need to restart the track. Maybe the cd was ejected from the drive?
			if ( not DevCD::instance().isPlayingAudioCd() )
			{
				if ( DevCD::instance().isAudioCDPresent() )
				{
					// Start to play desired track
					bool repeat =  not ( desiredCdTrack_ == VICTORY_MUSIC or desiredCdTrack_ == DEFEAT_MUSIC );

					if ( desiredCdTrack_ + 1 < DevCD::instance().numberOfTracks() )
					{
						if ( desiredCdTrack_ >= LOADING_MUSIC )
						{
							DevCD::instance().randomPlay( LOADING_MUSIC + 1, DevCD::instance().numberOfTracks() - 1, desiredCdTrack_ + 1 );
						}
						else
						{
							DevCD::instance().play( desiredCdTrack_ + 1, repeat );
						}
					}
				}
			}
		}
	}
}

void MachGuiStartupScreens::changeBackdrop( const char* image )
{
	CB_DEPIMPL(	GuiBitmap*, pBackdrop_ );

	pBackdrop_ = _NEW( GuiBitmap( Gui::bitmap( SysPathName( image ) ) ) );
	changed();
}

void MachGuiStartupScreens::cursorOn( bool on )
{
	CB_DEPIMPL(	RenAnimCursor2d*, pMenuCursor_ );
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );

	if ( on )
	{
	    pSceneManager_->pDevice()->display()->useCursor( pMenuCursor_ );
	}
	else
	{
		pSceneManager_->pDevice()->display()->useCursor( NULL ); // Shouldn't really use NULL but there's no other interface
	}
}

void MachGuiStartupScreens::activate()
{
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );

	if ( context_ == CTX_GAME or
		 context_ == CTX_MULTI_GAME or
		 context_ == CTX_SKIRMISH_GAME )
	{
		pInGameScreen_->activate();
	}
	else
	{
		// If app has just got focus back then render everything 4 times. This
		// seems to fix some graphic gliches.
		useFourTimesRender() = true;
		changed();
	}
}

//virtual
bool MachGuiStartupScreens::doHandleCharEvent( const DevButtonEvent& e )
{
	DEBUG_STREAM( DIAG_NEIL, "MachGuiStartupScreens::doHandleCharEvent " << e.getChar() << std::endl );

	return false;
}

const GuiBitmap& MachGuiStartupScreens::backdrop() const
{
	CB_DEPIMPL(	GuiBitmap*, pBackdrop_ );
	CB_DEPIMPL(	MachGuiMessageBox*, pMsgBox_ );

	PRE( pBackdrop_ );

	if ( pMsgBox_ )
		return pMsgBox_->image();

	return *pBackdrop_;
}

void MachGuiStartupScreens::contextAnimation()
{
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Music, desiredCdTrack_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, contextAfterFlic_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, contextBeforeFlic_ );
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );

	if ( context_ == CTX_TRANSITION )
	{
		string ctxBeforeName = getContextStrName( contextBeforeFlic_ );
		string ctxAfterName = getContextStrName( contextAfterFlic_ );
		string flicName = string("flics/") + ctxBeforeName + ctxAfterName + string(".smk");
		SysPathName sysFlicName( flicName );
		// Only play menu transition if it is on the hard disk
		if ( sysFlicName.existsAsFile() )
		{
			startPlayingAnimation( sysFlicName, true, true, Gui::Coord(0,0) );
		}
	}
	else if ( context_ == CTX_INTROANIMATION )
	{
		SysPathName sysFlicName( "flics/animatic.smk" );
		string cdRomDrive;
		bool flicExists = false;

		// Try playing into anim off hard-disk
		if ( sysFlicName.existsAsFile() )
		{
            startPlayingAnimation( sysFlicName, false, true, Gui::Coord(5,104), true );
			flicExists = true;
		}
		// Try playing intro anim off CD-ROM
		else
		{
		 	// Make sure the cd is stopped before accessing files on it.
			if ( DevCD::instance().isPlayingAudioCd() )
			{
				DevCD::instance().stopPlaying();
			}

			if ( MachGui::getCDRomDriveContainingFile( cdRomDrive, "flics/animatic.smk" ) )
			{
				SysPathName sysCDFlicName( cdRomDrive + "flics/animatic.smk" );
                startPlayingAnimation( sysCDFlicName, false, true, Gui::Coord(5,104), true );
				flicExists = true;
			}
		}

		if ( flicExists )
		{
			desiredCdTrack_ = DONT_PLAY_CD;

			// Clear the entire screen to stop previous screen from showing through black borders.
			RenDevice::current()->clearAllSurfaces(RenColour::black());
		}
	}
	else if ( context_ == CTX_POSTLOADINGANIMATION )
	{
		SysPathName sysFlicName( "flics/postload.smk" );
		string cdRomDrive;
		bool flicExists = false;

		// Try playing intro anim off hard-disk
		if ( sysFlicName.existsAsFile() )
		{
			startPlayingAnimation( sysFlicName, false, true, Gui::Coord(32,129) );
			flicExists = true;
		}
		// Try playing intro anim off CD-ROM
		else
		{
		 	// Make sure the cd is stopped before accessing files on it.
			if ( DevCD::instance().isPlayingAudioCd() )
			{
				DevCD::instance().stopPlaying();
			}

			if ( MachGui::getCDRomDriveContainingFile( cdRomDrive, "flics/postload.smk" ) )
			{
				SysPathName sysCDFlicName( cdRomDrive + "flics/postload.smk" );
				startPlayingAnimation( sysCDFlicName, false, true, Gui::Coord(32,129) );
				desiredCdTrack_ = DONT_PLAY_CD; // Don't play music if we're streaming video off CD
				flicExists = true;
			}
		}
	}
	else if ( context_ == CTX_ENTRYFLIC	or
			  context_ == CTX_VICTORYFLIC or
		      context_ == CTX_DEFEATFLIC )
	{
		string flicName;

		// Get relevant flic
		if ( context_ == CTX_ENTRYFLIC )
		{
			flicName = MachGuiDatabase::instance().currentScenario().entryFlic();
		}
		else if ( context_ == CTX_VICTORYFLIC )
		{
			flicName = MachGuiDatabase::instance().currentScenario().winFlic();
			contextVictory();
		}
		else if ( context_ == CTX_DEFEATFLIC )
		{
			flicName = MachGuiDatabase::instance().currentScenario().loseFlic();
			contextDefeat();
		}

		SysPathName sysFlicName( flicName );
		string cdRomDrive;
		bool flicExists = false;

		// Try playing intro anim off hard-disk
		if ( sysFlicName.existsAsFile() )
		{
            startPlayingAnimation( sysFlicName, false, true, Gui::Coord(5,104), true );
			flicExists = true;
		}
		// Try playing intro anim off CD-ROM
		else
		{
		 	// Make sure the cd is stopped before accessing files on it.
			if ( DevCD::instance().isPlayingAudioCd() )
			{
				DevCD::instance().stopPlaying();
			}

			if ( MachGui::getCDRomDriveContainingFile( cdRomDrive, flicName ) )
			{
				SysPathName sysCDFlicName( cdRomDrive + flicName );
                startPlayingAnimation( sysCDFlicName, false, true, Gui::Coord(5,104), true );
				flicExists = true;
			}
		}

		if ( flicExists )
		{
			desiredCdTrack_ = DONT_PLAY_CD; // Don't play music

			// Clear the entire screen to stop previous screen from showing through black borders.
			RenDevice::current()->clearAllSurfaces(RenColour::black());
		}
	}
}

void MachGuiStartupScreens::contextFinish()
{
	CB_DEPIMPL(	bool, finishApp_ );
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );

	finishApp_ = true;
	// Make sure the cd is stopped before exiting
	if ( DevCD::instance().isPlayingAudioCd() )
	{
		DevCD::instance().stopPlaying();
	}

	// Remove menu screen before app ends to stop it from flashing up momentarily
	const RenDisplay::Mode& mode = pSceneManager_->pDevice()->display()->currentMode();
	RenDevice::current()->backSurface().filledRectangle( Ren::Rect(0,0,mode.width(),mode.height()), RenColour::black() );
}

void MachGuiStartupScreens::contextGame()
{
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );
	CB_DEPIMPL(	bool, switchGuiRoot_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Music, desiredCdTrack_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );

	switchGuiRoot_ = true;
	cursorOn( false );
    //desiredCdTrack_ = MachGuiDatabase::instance().currentScenario().musicTrack();
    desiredCdTrack_ = _STATIC_CAST(MachGuiStartupScreens::Music,
        MachGuiDatabase::instance().currentScenario().musicTrack() );
	pInGameScreen_->resetSwitchToMenus();
}

void MachGuiStartupScreens::contextVictory()
{
//	CB_DEPIMPL(	MachGuiStartupScreens::Music, desiredCdTrack_ );
//
//	changeBackdrop( "gui/menu/victory.bmp" );
//	cursorOn( false );
//	desiredCdTrack_ = VICTORY_MUSIC;

	// Update player database...
	if ( gameType() == CAMPAIGNGAME )
	{
		MachGuiDbPlayer& player = MachGuiDatabase::instance().currentPlayer();
		MachGuiDbPlayerScenario& playerScenarioInfo = player.playerScenario( startupData()->scenario() );
		MachLogRaces& races = MachLogRaces::instance();
		MachPhys::Race playerRace = races.pcController().race();
		int score = MachLogRaces::instance().score( playerRace ).grossScore();
		playerScenarioInfo.upDate( true, score );

        //If required, save the surviving machines list for the specified races
        MachGuiDatabaseHandler& dbHandler = MachGuiDatabase::instance().handler();
        MachGuiDbScenario& dbScenario = playerScenarioInfo.scenario();
        for( uint i = 0; i != dbScenario.nRacesToHaveSurvivingMachinesSaved(); ++i )
        {
            MachPhys::Race race = dbScenario.raceToHaveSurvivingMachinesSaved( i );
            MachLogDatabaseHandler::Units units;
            units.reserve( 10 );
            dbHandler.raceUnits( race, &units );
            playerScenarioInfo.raceSurvivingUnits( race, units );
        }

        //Store any set flags in the player scenario
        playerScenarioInfo.clearSetFlags();
        for( uint i = 0; i != dbHandler.nSetFlags(); ++i )
            playerScenarioInfo.setFlag( dbHandler.setFlag( i ), true );

		// This forces the scenario tree to be refreshed...
		MachGuiDatabase::instance().currentPlayer( &MachGuiDatabase::instance().currentPlayer() );

		// Move default selection to next scenario on scenario selection screen
		MachGuiDbScenario* pNextScenario = workOutWhichScenarioToDefaultTo();
		MachGuiDatabase::instance().currentPlayer().lastSelectedScenario( pNextScenario );

		MachGuiDatabase::instance().writeDatabase();
	}
	else if ( gameType() == SKIRMISHGAME )
	{
		// Update the fact that this skirmish game has been played and won
		startupData()->scenario()->hasBeenWon( true );
		MachGuiDatabase::instance().writeDatabase();
	}
}


void MachGuiStartupScreens::contextDefeat()
{
//	CB_DEPIMPL(	MachGuiStartupScreens::Music, desiredCdTrack_ );
//
//	changeBackdrop( "gui/menu/defeat.bmp" );
//	cursorOn( false );
//	desiredCdTrack_ = DEFEAT_MUSIC;

	// Update player database...
	if ( gameType() == CAMPAIGNGAME )
	{
		MachGuiDbPlayer& player = MachGuiDatabase::instance().currentPlayer();
		MachGuiDbPlayerScenario& playerScenarioInfo = player.playerScenario( startupData()->scenario() );
		MachLogRaces& races = MachLogRaces::instance();
        MachPhys::Race playerRace = races.pcController().race();
	    int score = MachLogRaces::instance().score( playerRace ).grossScore();
		playerScenarioInfo.upDate( false, score );
		MachGuiDatabase::instance().writeDatabase();
	}
}

void MachGuiStartupScreens::contextLogo()
{
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );
	CB_DEPIMPL(	MachGuiStartupScreens::Music, desiredCdTrack_ );

	switch ( context_ )
	{
		case CTX_PROBEACCLAIMLOGO:
			changeBackdrop( "gui/menu/acclaim.bmp" );
			break;
		case CTX_CHARYBDISLOGO:
			changeBackdrop( "gui/menu/charlogo.bmp" );
			break;
		case CTX_SPLASH1:
			changeBackdrop( "gui/menu/splash1.bmp" );
			break;
		case CTX_SPLASH2:
			changeBackdrop( "gui/menu/splash2.bmp" );
			break;
		case CTX_LEGALSCREEN:
			changeBackdrop( "gui/menu/legal.bmp" );
			break;
		DEFAULT_ASSERT_BAD_CASE( context_ );
	}

    cursorOn( false );
	if ( desiredCdTrack_ != LOADING_MUSIC )
		desiredCdTrack_ = MENU_MUSIC;
}

MachGuiStartupScreens::ButtonEvent MachGuiStartupScreens::lastButtonEvent() const
{
	CB_DEPIMPL(	ButtonEvent, lastButtonEvent_ );

	return lastButtonEvent_;
}

MachGuiStartupData* MachGuiStartupScreens::startupData()
{
	NETWORK_STREAM("MachGuiStartupScreens::startupData " << (void*)this << std::endl );
	CB_DEPIMPL(	MachGuiStartupData*, pStartupData_ );

	NETWORK_STREAM(" returning " << (void*)pStartupData_ << std::endl );
	return pStartupData_;
}

void MachGuiStartupScreens::desiredCdTrack( MachGuiStartupScreens::Music cdTrack )
{
	CB_DEPIMPL(	MachGuiStartupScreens::Music, desiredCdTrack_ );

	desiredCdTrack_ = cdTrack;
}

MachGuiMessageBroker& MachGuiStartupScreens::messageBroker()
{
	CB_DEPIMPL(	MachGuiMessageBroker*, pMessageBroker_ );

	PRE( pMessageBroker_ );

	return *pMessageBroker_;
}

void MachGuiStartupScreens::registerAutoDeleteGuiElement( MachGuiAutoDeleteDisplayable* pMustContainMouse )
{
	CB_DEPIMPL(	MachGuiAutoDeleteDisplayable*, pMustContainMouse_ );

	pMustContainMouse_ = pMustContainMouse;
}

void MachGuiStartupScreens::unregisterAutoDeleteGuiElement()
{
	CB_DEPIMPL(	MachGuiAutoDeleteDisplayable*, pMustContainMouse_ );

	pMustContainMouse_ = NULL;
}

void MachGuiStartupScreens::addSmackerAnimation( AniSmacker* animation )
{
	CB_DEPIMPL(	MachGuiStartupScreensImpl::SmackerAnims, smackerAnims_ );

	PRE( animation != NULL );
    smackerAnims_.push_back( animation );
}

void MachGuiStartupScreens::clearAllSmackerAnimations( void )
{
	CB_DEPIMPL(	MachGuiStartupScreensImpl::SmackerAnims, smackerAnims_ );

	for (   MachGuiStartupScreensImpl::SmackerAnims::iterator i = smackerAnims_.begin();
            i != smackerAnims_.end();
			++i )
	{
		_DELETE( *i );
	}

	smackerAnims_.erase( smackerAnims_.begin(), smackerAnims_.end() );
}

void MachGuiStartupScreens::playSmackerAnimations( void )
{
	CB_DEPIMPL(	MachGuiStartupScreensImpl::SmackerAnims, smackerAnims_ );

	// Cycle through list of smacker files setup by a particular context
	// and play next frame of animation ( looping if necessary ).
	for (   MachGuiStartupScreensImpl::SmackerAnims::iterator i = smackerAnims_.begin();
			i != smackerAnims_.end();
 			++i )
 	{
		AniSmacker* pCurrentAnimation = *i;
 		// loop animation
		pCurrentAnimation->playNextFrame( RenDevice::current() );
 		if ( pCurrentAnimation->isFinished() )
			pCurrentAnimation->rewind();
	}
}

//static
void MachGuiStartupScreens::releaseCachedMemory()
{
	GuiBmpFont::releaseFontMemory();
	MachGuiPlayerColour::releaseBmpMemory();
	MachGui::releaseMenuBmpMemory();
	MachGuiDatabase::instance().clearAllTextData();
}

void MachGuiStartupScreens::displayMsgBox( uint stringResId )
{
	CB_DEPIMPL(	MachGuiMessageBox*, pMsgBox_ );
	CB_DEPIMPL(	GuiDisplayable*, pCharFocus_ );

	PRE( pMsgBox_ == NULL );

	// Store control with char focus for duration of message box. We don't want
	// controls to respond to key presses whilst a message box is displayed.
	pCharFocus_ = NULL;
	if ( GuiManager::instance().charFocusExists() )
	{
		pCharFocus_ = &GuiManager::instance().charFocus();
	}
	GuiManager::instance().removeCharFocus();

	pMsgBox_ = _NEW( MachGuiMessageBox( this, stringResId, MachGuiMessageBox::MBOK ) );
}

void MachGuiStartupScreens::displayMsgBox( uint stringResId, const GuiStrings& strs )
{
	CB_DEPIMPL(	MachGuiMessageBox*, pMsgBox_ );
	CB_DEPIMPL(	GuiDisplayable*, pCharFocus_ );

	PRE( pMsgBox_ == NULL );

	// Store control with char focus for duration of message box. We don't want
	// controls to respond to key presses whilst a message box is displayed.
	pCharFocus_ = NULL;
	if ( GuiManager::instance().charFocusExists() )
	{
		pCharFocus_ = &GuiManager::instance().charFocus();
	}
	GuiManager::instance().removeCharFocus();

	pMsgBox_ = _NEW( MachGuiMessageBox( this, stringResId, MachGuiMessageBox::MBOK, strs ) );
}

void MachGuiStartupScreens::displayMsgBox( uint stringResId, MachGuiMessageBoxResponder* pResponder )
{
	displayMsgBox( stringResId, pResponder, false );
}

void MachGuiStartupScreens::displayMsgBox( uint stringResId, MachGuiMessageBoxResponder* pResponder, bool yesNo )
{
	CB_DEPIMPL(	MachGuiMessageBox*, pMsgBox_ );
	CB_DEPIMPL(	MachGuiMessageBoxResponder*, pMsgBoxResponder_ );
	CB_DEPIMPL(	GuiDisplayable*, pCharFocus_ );

	PRE( pMsgBox_ == NULL );
	PRE( pMsgBoxResponder_ == NULL );

	// Store control with char focus for duration of message box. We don't want
	// controls to respond to key presses whilst a message box is displayed.
	pCharFocus_ = NULL;
	if ( GuiManager::instance().charFocusExists() )
	{
		pCharFocus_ = &GuiManager::instance().charFocus();
	}
	GuiManager::instance().removeCharFocus();

	if ( yesNo )
	{
		pMsgBox_ = _NEW( MachGuiMessageBox( this, stringResId, MachGuiMessageBox::MBYESNO ) );
	}
	else
	{
		pMsgBox_ = _NEW( MachGuiMessageBox( this, stringResId, MachGuiMessageBox::MBOKCANCEL ) );
	}

	pMsgBoxResponder_ = pResponder;
}

void MachGuiStartupScreens::displayMsgBox( uint stringResId, MachGuiMessageBoxResponder* pResponder, const GuiStrings& strs )
{
	CB_DEPIMPL(	MachGuiMessageBox*, pMsgBox_ );
	CB_DEPIMPL(	MachGuiMessageBoxResponder*, pMsgBoxResponder_ );
	CB_DEPIMPL(	GuiDisplayable*, pCharFocus_ );

	PRE( pMsgBox_ == NULL );
	PRE( pMsgBoxResponder_ == NULL );

	// Store control with char focus for duration of message box. We don't want
	// controls to respond to key presses whilst a message box is displayed.
	pCharFocus_ = NULL;
	if ( GuiManager::instance().charFocusExists() )
	{
		pCharFocus_ = &GuiManager::instance().charFocus();
	}
	GuiManager::instance().removeCharFocus();

	pMsgBox_ = _NEW( MachGuiMessageBox( this, stringResId, MachGuiMessageBox::MBOKCANCEL, strs ) );
	pMsgBoxResponder_ = pResponder;
}

void MachGuiStartupScreens::displayOKMsgBox( uint stringResId, MachGuiMessageBoxResponder* pResponder )
{
	CB_DEPIMPL(	MachGuiMessageBox*, pMsgBox_ );
	CB_DEPIMPL(	MachGuiMessageBoxResponder*, pMsgBoxResponder_ );
	CB_DEPIMPL(	GuiDisplayable*, pCharFocus_ );

	PRE( pMsgBox_ == NULL );
	PRE( pMsgBoxResponder_ == NULL );

	// Store control with char focus for duration of message box. We don't want
	// controls to respond to key presses whilst a message box is displayed.
	pCharFocus_ = NULL;
	if ( GuiManager::instance().charFocusExists() )
	{
		pCharFocus_ = &GuiManager::instance().charFocus();
	}
	GuiManager::instance().removeCharFocus();

	pMsgBox_ = _NEW( MachGuiMessageBox( this, stringResId, MachGuiMessageBox::MBOK ) );
	pMsgBoxResponder_ = pResponder;
}

MachGuiStartupScreens::GameType MachGuiStartupScreens::gameType() const
{
	CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ );

	return gameType_;
}

void MachGuiStartupScreens::gameType( MachGuiStartupScreens::GameType newGameType )
{
	CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ );

	gameType_ = newGameType;
}

MachInGameScreen& MachGuiStartupScreens::inGameScreen()
{
	CB_DEPIMPL(	MachInGameScreen*, pInGameScreen_ );

	PRE( pInGameScreen_ );

	return *pInGameScreen_;
}

MachGuiStartupScreens::Context MachGuiStartupScreens::currentContext() const
{
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );

	return context_;
}

void MachGuiStartupScreens::restartGame()
{
	CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ );

	PRE( gameType_ != NOGAME );
	PRE( gameType_ != MULTIGAME );

	// Store because this gets reset in unloadGame()
	GameType gt = gameType_;

	unloadGame();

    //Ensure current scenario set
	MachGuiDatabase::instance().currentScenario( startupData()->scenario() );

	if ( gt == CAMPAIGNGAME )
	{
		switchContext( CTX_GAME );
	}
	else
	{
		switchContext( CTX_SKIRMISH_GAME );
	}
}

void MachGuiStartupScreens::unloadGame()
{
	CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ );
	CB_DEPIMPL( MachGuiDispositionChangeNotifiable*, pDispositionNotifiable_ );

	MachLogRaces::instance().unregisterDispositionChangeNotifiable( pDispositionNotifiable_ );

    //Clear any set flags in the database handler
    MachGuiDatabase::instance().handler().clearSetFlags();

	if ( gameType() != NOGAME )
	{
		if ( gameType() == MachGuiStartupScreens::MULTIGAME )
		{
			NetNetwork::instance().messageThrottlingActive( false );
			// Disconnect from network ( keep protocol!! )
			// Store connection type because terminateAndReset sets it to "".
			string ct = startupData()->connectionType();
			MachLogNetwork::instance().terminateAndReset();
			startupData()->connectionType( ct );
		}

		inGameScreen().unloadGame();

        MachLogRaces::instance().unloadGame();

        //Clear the current scenario in the database
        MachGuiDatabase::instance().currentScenario( NULL );

		gameType( MachGuiStartupScreens::NOGAME );
	}

	POST( gameType_ == NOGAME );
}

void MachGuiStartupScreens::loadSavedGame( MachGuiDbSavedGame* pSavedGame )
{
	CB_DEPIMPL(	MachGuiStartupScreens::GameType, gameType_ );
	CB_DEPIMPL(	W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachGuiDispositionChangeNotifiable*, pDispositionNotifiable_ );

	PRE( gameType_ == NOGAME );

    pSceneManager_->pDevice()->display()->useCursor( NULL );

	// Display loading bmp
	GuiBitmap loadingBmp = Gui::bitmap( "gui/menu/loading.bmp" );
	loadingBmp.enableColourKeying();
	GuiBitmap frontBuffer = W4dManager::instance().sceneManager()->pDevice()->frontSurface();
	frontBuffer.simpleBlit( loadingBmp, xMenuOffset(), yMenuOffset() );
	// For double buffering call it twice to draw on both front and back buffers
	RenDevice::current()->display()->flipBuffers();
	frontBuffer.simpleBlit( loadingBmp, xMenuOffset(), yMenuOffset() );

	LoadGameProgressIndicator progressIndicator(this);

    //Store the current scenario
    startupData()->scenario( &pSavedGame->scenario() );
    MachGuiDatabase::instance().currentScenario( &pSavedGame->scenario() );

	//Clear the list of set flags in the database handler
    MachGuiDatabase::instance().handler().clearSetFlags();

	// Store current player
	if ( pSavedGame->hasPlayer() )
	{
		MachGuiDatabase::instance().currentPlayer( &pSavedGame->player() );
	}
	else
	{
		MachGuiDatabase::instance().clearCurrentPlayer();
	}

	progressIndicator.report( 10, 100 );

	progressIndicator.setLimits( 0.1, 1.0 );

    //Extract the planet name
    const string& planetName = pSavedGame->scenario().planetFile();
	string scenarioFileName = pSavedGame->scenario().name() + ".scn";

	MachGuiLoadSaveGameExtras lsgExtras( &inGameScreen() );
	MachLogRaces::instance().loadSavedGame( pSceneManager_, planetName, scenarioFileName,
	                                        pSavedGame->externalFileName(), &lsgExtras,
	                                        &progressIndicator );

	MachLogRaces::instance().registerDispositionChangeNotifiable( pDispositionNotifiable_ );

	if ( pSavedGame->isCampaignGame() )
	{
		gameType( MachGuiStartupScreens::CAMPAIGNGAME );
	}
	else
	{
		gameType( MachGuiStartupScreens::SKIRMISHGAME );
	}

	// Copy front buffer to back buffer so that complete progress bar is shown when
	// buffers are flipped.
	GuiPainter::instance().blit( frontBuffer );
}

// static
string MachGuiStartupScreens::getContextStrName( MachGuiStartupScreens::Context context )
{
	switch ( context )
	{
		case CTX_MAINMENU:
			return "sa";
		case CTX_SINGLEPLAYER:
			return "si";
		case CTX_CAMPAIGN:
			return "sm";
		case CTX_MULTIPLAYER:
			return "sb";
		case CTX_JOIN:
			return "sc";
		case CTX_IMREADY:
			return "sd";
		case CTX_OPTIONS:
		case CTX_IGOPTIONS:
			return "sf";
		case CTX_INGAMEOP:
		case CTX_CAINGAMEOPTIONS:
		case CTX_SKINGAMEOPTIONS:
		case CTX_MPINGAMEOPTIONS:
			return "se";
		case CTX_HOTKEYS:
			return "so";
		case CTX_SAVE:
			return "sh";
		case CTX_IGLOAD:
		case CTX_LOAD:
			return "sg";
		case CTX_BRIEFING:
		case CTX_IGBRIEFING:
			return "sk";
		case CTX_CADEBRIEFING:
		case CTX_SKDEBRIEFING:
		case CTX_MPDEBRIEFING:
			return "sj";
		case CTX_SCENARIO:
			return "sl";
		case CTX_SKIRMISH:
			return "sn";
		case CTX_SETTINGS:
			return "sp";
		case CTX_CASTATISTICS:
		case CTX_MPSTATISTICS:
		case CTX_SKSTATISTICS:
			return "sq";
		case CTX_FINISH:
		{
			MexBasicRandom randNum = MexBasicRandom::constructSeededFromTime();
			int randomInt = mexRandomInt( &randNum, 1, 4 );
			if ( randomInt == 1 )
				return "end1";
			else if ( randomInt == 2 )
				return "end2";
			else
				return "end3";
		}
	};

	return "notfound";
}

#ifdef DEMO
//static
MachGuiStartupScreens::ContextSwitchInfo* MachGuiStartupScreens::getContextSwitchInfo()
{
	static ContextSwitchInfo csi[] = { CTX_MULTIPLAYER,		JOIN, 			CTX_JOIN,					false,
									   CTX_MULTIPLAYER, 	EXIT, 			CTX_MAINMENU,				false,
									   CTX_JOIN, 			EXIT, 			CTX_MULTIPLAYER,			false,
									   CTX_JOIN, 			CREATE, 		CTX_IMREADY,				false,
									   CTX_JOIN, 			JOIN, 			CTX_IMREADY,				false,
									   CTX_IMREADY,			EXIT,			CTX_JOIN,					false,
									   CTX_IMREADY,			SETTINGS,		CTX_SETTINGS,				false,
									   CTX_SETTINGS,		EXIT,			CTX_IMREADY,				false,
									   CTX_SETTINGS,		BE_OK,			CTX_IMREADY,				false,
									   CTX_MAINMENU, 		EXIT, 			CTX_SPLASH1,				false,
									   CTX_MAINMENU, 		BE_DUMMY_OK,	CTX_SPLASH1,				false,
									   CTX_MAINMENU, 		CAMPAIGN,		CTX_SCENARIO,				false,
									   CTX_MAINMENU, 		SKIRMISH, 		CTX_SKIRMISH,				false,
									   CTX_MAINMENU, 		OPTIONS, 		CTX_OPTIONS,				false,
									   CTX_MAINMENU, 		MULTIPLAYER, 	CTX_MULTIPLAYER,			false,
									   CTX_SCENARIO, 		EXIT,  			CTX_MAINMENU,				false,
									   CTX_SCENARIO, 		BE_OK, 			CTX_BRIEFING,				false,
									   CTX_BRIEFING,		EXIT,			CTX_SCENARIO,				false,
									   CTX_BRIEFING,		STARTGAME,		CTX_GAME,					false,
									   CTX_IGBRIEFING,		EXIT,			CTX_INGAMEOP,				false,
									   CTX_IGBRIEFING,		STARTGAME,		CTX_BACKTOGAME,				false,
									   CTX_SKIRMISH,		EXIT,			CTX_MAINMENU,				false,
									   CTX_SKIRMISH,		BE_OK,			CTX_SKIRMISH_GAME, 			false,

									   CTX_OPTIONS,  		BE_OK, 			CTX_MAINMENU,	  			false,
									   CTX_OPTIONS,  		EXIT,			CTX_MAINMENU,				false,

									   CTX_CAINGAMEOPTIONS,	EXIT,			CTX_SCENARIO,				false,
									   CTX_CAINGAMEOPTIONS,	CONTINUE,		CTX_BACKTOGAME,				false,
									   CTX_CAINGAMEOPTIONS,	OPTIONS, 		CTX_IGOPTIONS,				false,
									   CTX_CAINGAMEOPTIONS,	BRIEFING,		CTX_IGBRIEFING,				false,

									   CTX_SKINGAMEOPTIONS,	EXIT,			CTX_SKIRMISH, 				false,
									   CTX_SKINGAMEOPTIONS,	CONTINUE,		CTX_BACKTOGAME,				false,
									   CTX_SKINGAMEOPTIONS,	OPTIONS, 		CTX_IGOPTIONS,				false,
									   CTX_SKINGAMEOPTIONS,	BRIEFING,		CTX_IGBRIEFING,				false,

									   CTX_MPINGAMEOPTIONS,	EXIT,			CTX_JOIN,					false,
									   CTX_MPINGAMEOPTIONS,	CONTINUE,		CTX_BACKTOGAME,				false,
									   CTX_MPINGAMEOPTIONS,	OPTIONS, 		CTX_IGOPTIONS,				false,
									   CTX_MPINGAMEOPTIONS,	BRIEFING,		CTX_IGBRIEFING,				false,

									   CTX_CAINGAMEOPTIONS, EXIT,			CTX_MAINMENU,				false,
									   CTX_CAINGAMEOPTIONS,	CONTINUE,		CTX_BACKTOGAME,				false,
									   CTX_CAINGAMEOPTIONS,	BRIEFING,		CTX_IGBRIEFING,				false,
									   CTX_SKINGAMEOPTIONS, EXIT,			CTX_MAINMENU,				false,
									   CTX_SKINGAMEOPTIONS,	CONTINUE,		CTX_BACKTOGAME,				false,
									   CTX_SKINGAMEOPTIONS,	BRIEFING,		CTX_IGBRIEFING,				false,

									   CTX_MPINGAMEOPTIONS, EXIT,			CTX_MAINMENU,				false,
									   CTX_MPINGAMEOPTIONS,	CONTINUE,		CTX_BACKTOGAME,				false,
									   CTX_MPINGAMEOPTIONS,	BRIEFING,		CTX_IGBRIEFING,				false,

									   CTX_IGOPTIONS,  		BE_OK, 			CTX_INGAMEOP,				false,
									   CTX_IGOPTIONS,  		EXIT,			CTX_INGAMEOP,				false,
									   CTX_CADEBRIEFING,	CONTINUE,		CTX_SCENARIO,			    false,
									   CTX_SKDEBRIEFING,	CONTINUE,		CTX_SKIRMISH,			    false,
									   CTX_MPDEBRIEFING,	CONTINUE,		CTX_JOIN,				    false,
									   CTX_FINISH,			EXIT, 			CTX_FINISH,				    false	 /*This line must come last*/ };

	return csi;
}
#else
//static
MachGuiStartupScreens::ContextSwitchInfo* MachGuiStartupScreens::getContextSwitchInfo()
{
	static ContextSwitchInfo csi[] = { CTX_SINGLEPLAYER, 	EXIT, 			CTX_MAINMENU,				true,
									   CTX_SINGLEPLAYER, 	CAMPAIGN,  		CTX_CAMPAIGN,				true,
									   CTX_SINGLEPLAYER, 	SKIRMISH,  		CTX_SKIRMISH,				true,
									   CTX_SINGLEPLAYER, 	LOADGAME,		CTX_LOAD,					true,
									   CTX_MULTIPLAYER,		JOIN, 			CTX_JOIN,					true,
									   CTX_MULTIPLAYER, 	EXIT, 			CTX_MAINMENU,				true,
									   CTX_JOIN, 			EXIT, 			CTX_MULTIPLAYER,			true,
									   CTX_JOIN, 			CREATE, 		CTX_IMREADY,				true,
									   CTX_JOIN, 			JOIN, 			CTX_IMREADY,				true,
									   CTX_IMREADY,			EXIT,			CTX_JOIN,					true,
									   CTX_IMREADY,			SETTINGS,		CTX_SETTINGS,				true,
									   CTX_SETTINGS,		EXIT,			CTX_IMREADY,				true,
									   CTX_SETTINGS,		BE_OK,			CTX_IMREADY,				true,
									   CTX_MAINMENU, 		EXIT, 			CTX_FINISH,					true,
									   CTX_MAINMENU, 		BE_DUMMY_OK,	CTX_FINISH,					true,
									   CTX_MAINMENU, 		SINGLEPLAYER,	CTX_SINGLEPLAYER,			true,
									   CTX_MAINMENU, 		OPTIONS, 		CTX_OPTIONS,				true,
									   CTX_MAINMENU, 		MULTIPLAYER, 	CTX_MULTIPLAYER,			true,
									   CTX_CAMPAIGN, 		EXIT,  			CTX_SINGLEPLAYER,			true,
									   CTX_CAMPAIGN, 		BE_OK, 			CTX_SCENARIO,				true,
									   CTX_SCENARIO, 		EXIT,  			CTX_CAMPAIGN,				true,
									   CTX_SCENARIO, 		BE_OK, 			CTX_BRIEFING,				true,
									   CTX_BRIEFING,		EXIT,			CTX_SCENARIO,				true,
									   CTX_BRIEFING,		STARTGAME,		CTX_ENTRYFLIC,				false,
									   CTX_IGBRIEFING,		EXIT,			CTX_INGAMEOP,				true,
									   CTX_IGBRIEFING,		STARTGAME,		CTX_BACKTOGAME,				false,
									   CTX_SKIRMISH,		EXIT,			CTX_SINGLEPLAYER,			true,
									   CTX_SKIRMISH,		BE_OK,			CTX_SKIRMISH_GAME, 			false,

									   CTX_CAINGAMEOPTIONS,	EXIT,			CTX_SCENARIO,				true,
									   CTX_CAINGAMEOPTIONS,	CONTINUE,		CTX_BACKTOGAME,				false,
									   CTX_CAINGAMEOPTIONS,	OPTIONS, 		CTX_IGOPTIONS,				true,
									   CTX_CAINGAMEOPTIONS,	LOADGAME, 		CTX_IGLOAD,					true,
									   CTX_CAINGAMEOPTIONS,	SAVEGAME, 		CTX_SAVE,					true,
									   CTX_CAINGAMEOPTIONS,	BRIEFING,		CTX_IGBRIEFING,				true,
									   CTX_CAINGAMEOPTIONS,	HOTKEYS,		CTX_HOTKEYS,				true,

									   CTX_SKINGAMEOPTIONS,	EXIT,			CTX_SKIRMISH, 				true,
									   CTX_SKINGAMEOPTIONS,	CONTINUE,		CTX_BACKTOGAME,				false,
									   CTX_SKINGAMEOPTIONS,	OPTIONS, 		CTX_IGOPTIONS,				true,
									   CTX_SKINGAMEOPTIONS,	LOADGAME, 		CTX_IGLOAD,					true,
									   CTX_SKINGAMEOPTIONS,	SAVEGAME, 		CTX_SAVE,					true,
									   CTX_SKINGAMEOPTIONS,	BRIEFING,		CTX_IGBRIEFING,				true,
									   CTX_SKINGAMEOPTIONS,	HOTKEYS,		CTX_HOTKEYS,				true,

									   CTX_MPINGAMEOPTIONS,	EXIT,			CTX_JOIN,					true,
									   CTX_MPINGAMEOPTIONS,	CONTINUE,		CTX_BACKTOGAME,				false,
									   CTX_MPINGAMEOPTIONS,	OPTIONS, 		CTX_IGOPTIONS,				true,
									   CTX_MPINGAMEOPTIONS,	LOADGAME, 		CTX_IGLOAD,					true,
									   CTX_MPINGAMEOPTIONS,	SAVEGAME, 		CTX_SAVE,					true,
									   CTX_MPINGAMEOPTIONS,	BRIEFING,		CTX_IGBRIEFING,				true,
									   CTX_MPINGAMEOPTIONS,	HOTKEYS,		CTX_HOTKEYS,				true,

									   CTX_OPTIONS,  		BE_OK, 			CTX_MAINMENU,	  			true,
									   CTX_OPTIONS,  		EXIT,			CTX_MAINMENU,				true,
									   CTX_IGOPTIONS,  		BE_OK, 			CTX_INGAMEOP,				true,
									   CTX_IGOPTIONS,  		EXIT,			CTX_INGAMEOP,				true,
									   CTX_HOTKEYS,			EXIT,			CTX_INGAMEOP,				true,
									   CTX_LOAD,			EXIT,			CTX_SINGLEPLAYER,			true,
									   CTX_LOAD,			BE_OK, 			CTX_BACKTOGAME,		   		false,
									   CTX_IGLOAD,			EXIT,			CTX_INGAMEOP,				true,
									   CTX_IGLOAD,			BE_OK, 			CTX_BACKTOGAME,		   		false,
									   CTX_SAVE,			EXIT,			CTX_INGAMEOP,				true,
									   CTX_SAVE,			BE_OK, 			CTX_INGAMEOP,				true,
									   CTX_CADEBRIEFING,	CONTINUE,		CTX_SCENARIO,				true,
									   CTX_SKDEBRIEFING,	CONTINUE,		CTX_SKIRMISH,				true,
									   CTX_MPDEBRIEFING,	CONTINUE,		CTX_JOIN,					true,
									   CTX_CADEBRIEFING,	BE_STATISTICS,	CTX_CASTATISTICS,			true,
									   CTX_SKDEBRIEFING,	BE_STATISTICS,	CTX_SKSTATISTICS,			true,
									   CTX_MPDEBRIEFING,	BE_STATISTICS,	CTX_MPSTATISTICS,			true,
									   CTX_CASTATISTICS,	EXIT,			CTX_CADEBRIEFING,			true,
									   CTX_SKSTATISTICS,	EXIT,			CTX_SKDEBRIEFING,			true,
   									   CTX_MPSTATISTICS,	EXIT,			CTX_MPDEBRIEFING,			true,
									   CTX_FINISH,			EXIT, 			CTX_FINISH,					false /*This line must come last*/ };

	return csi;
}
#endif

#ifdef DEMO
//static
MachGuiStartupScreens::ContextTimeoutInfo* MachGuiStartupScreens::getContextTimeoutInfo()
{
	static ContextTimeoutInfo cti[] = {	CTX_LEGALSCREEN,				CTX_PROBEACCLAIMLOGO, 	ContextTimeoutInfo::TIMEOUT,		5,
										CTX_PROBEACCLAIMLOGO,			CTX_INTROANIMATION, 	ContextTimeoutInfo::TIMEOUT, 		5,
									    CTX_INTROANIMATION,				CTX_MAINMENU,			ContextTimeoutInfo::FLIC_FINISHED, 	0,
										CTX_MAINMENU, 					CTX_PROBEACCLAIMLOGO,	ContextTimeoutInfo::TIMEOUT, 		300,
									    CTX_DEMO,						CTX_PROBEACCLAIMLOGO,	ContextTimeoutInfo::TIMEOUT, 		2,
										CTX_TRANSITION, 				CTX_TRANSITIONEND, 		ContextTimeoutInfo::FLIC_FINISHED, 	0,
								    	CTX_SPLASH1,					CTX_SPLASH2,			ContextTimeoutInfo::TIMEOUT, 		5,
									    CTX_SPLASH2,					CTX_FINISH,				ContextTimeoutInfo::TIMEOUT, 		5,
										CTX_LOADINGEXE,					CTX_PROBEACCLAIMLOGO,	ContextTimeoutInfo::TIMEOUT, 		0,
										CTX_VICTORYFLIC,				CTX_DEBRIEFING,			ContextTimeoutInfo::FLIC_FINISHED, 	0,
										CTX_DEFEATFLIC,					CTX_DEBRIEFING,	  		ContextTimeoutInfo::FLIC_FINISHED, 	0,
										CTX_FINISH, 					CTX_FINISH, 			ContextTimeoutInfo::FLIC_FINISHED, 	0 /*This line must come last*/ };

	return cti;
}
#else
//static
MachGuiStartupScreens::ContextTimeoutInfo* MachGuiStartupScreens::getContextTimeoutInfo()
{
	static ContextTimeoutInfo cti[] = {	CTX_LEGALSCREEN,				CTX_PROBEACCLAIMLOGO, 	ContextTimeoutInfo::TIMEOUT,		5,
										CTX_PROBEACCLAIMLOGO,			CTX_CHARYBDISLOGO, 		ContextTimeoutInfo::TIMEOUT, 		5,
									    CTX_CHARYBDISLOGO,				CTX_INTROANIMATION,		ContextTimeoutInfo::TIMEOUT,		5,
									    CTX_INTROANIMATION,				CTX_MAINMENU,			ContextTimeoutInfo::FLIC_FINISHED, 	0,
									    CTX_MAINMENU, 					CTX_PROBEACCLAIMLOGO,	ContextTimeoutInfo::TIMEOUT, 		300,
									    CTX_DEMO,						CTX_PROBEACCLAIMLOGO,	ContextTimeoutInfo::TIMEOUT, 		2,
									    CTX_VICTORY,  					CTX_DEBRIEFING,  		ContextTimeoutInfo::TIMEOUT, 		19,
										CTX_DEFEAT,  					CTX_DEBRIEFING,  		ContextTimeoutInfo::TIMEOUT, 		14,
										CTX_TRANSITION, 				CTX_TRANSITIONEND, 		ContextTimeoutInfo::FLIC_FINISHED, 	0,
										CTX_POSTLOADINGANIMATION,		CTX_LEGALSCREEN,		ContextTimeoutInfo::FLIC_FINISHED, 	0,
										CTX_LOADINGEXE,					CTX_POSTLOADINGANIMATION,ContextTimeoutInfo::TIMEOUT, 		0,
										CTX_VICTORYFLIC,				CTX_DEBRIEFING,			ContextTimeoutInfo::FLIC_FINISHED, 	0,
										CTX_DEFEATFLIC,					CTX_DEBRIEFING,	  		ContextTimeoutInfo::FLIC_FINISHED, 	0,
										CTX_ENTRYFLIC,					CTX_GAME,				ContextTimeoutInfo::FLIC_FINISHED, 	0,
 										CTX_FINISH, 					CTX_FINISH, 			ContextTimeoutInfo::FLIC_FINISHED, 	0 /*This line must come last*/ };

	return cti;
}
#endif

bool MachGuiStartupScreens::isContextFlic() const
{
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );

	switch ( context_ )
	{
		case CTX_INTROANIMATION:
		case CTX_TRANSITION:
		case CTX_POSTLOADINGANIMATION:
		case CTX_VICTORYFLIC:
		case CTX_DEFEATFLIC:
		case CTX_ENTRYFLIC:
			return true;
	}

	return false;
}

#ifdef DEMO
//static
MachGuiStartupScreens::ContextKeypressInfo* MachGuiStartupScreens::getContextKeypressInfo()
{
	static ContextKeypressInfo cki[] = { CTX_PROBEACCLAIMLOGO,			DevKey::ESCAPE,			CTX_MAINMENU,		false,	NO_IMP,
										 CTX_PROBEACCLAIMLOGO,			DevKey::SPACE,			CTX_MAINMENU,		false,	NO_IMP,
										 CTX_LEGALSCREEN,				DevKey::ESCAPE,			CTX_PROBEACCLAIMLOGO,false, NO_IMP,
										 CTX_LEGALSCREEN,				DevKey::SPACE,			CTX_PROBEACCLAIMLOGO,false, NO_IMP,
										 CTX_MULTIPLAYER,				DevKey::ENTER,			CTX_JOIN,			false,  JOIN,
										 CTX_MULTIPLAYER,				DevKey::ESCAPE,			CTX_MAINMENU,		false,	EXIT,
										 CTX_IGOPTIONS,  		 		DevKey::ENTER,			CTX_INGAMEOP,		false,	BE_OK,
									     CTX_IGOPTIONS,  				DevKey::ESCAPE,			CTX_INGAMEOP,		false,	EXIT,
									     CTX_JOIN,						DevKey::ESCAPE,			CTX_MULTIPLAYER,	false,	EXIT,
										 CTX_SETTINGS,	   				DevKey::ESCAPE, 		CTX_IMREADY,		false,	EXIT,
									   	 CTX_SETTINGS,					DevKey::ENTER,			CTX_IMREADY,		false,	BE_OK,
										 CTX_SKIRMISH,					DevKey::ESCAPE,			CTX_MAINMENU,		false,	EXIT,
										 CTX_SKIRMISH,					DevKey::ENTER,			CTX_SKIRMISH_GAME, 	false,	BE_OK,
								  	     CTX_SCENARIO, 					DevKey::ESCAPE,  		CTX_MAINMENU,		false, 	EXIT,
									     CTX_SCENARIO, 					DevKey::ENTER, 			CTX_BRIEFING,		false,	BE_OK,
									     CTX_BRIEFING,					DevKey::ESCAPE,			CTX_SCENARIO,		false,	EXIT,
									     CTX_BRIEFING,					DevKey::ENTER,			CTX_GAME,			false,	STARTGAME,
					 					 CTX_IGBRIEFING,				DevKey::ESCAPE,			CTX_INGAMEOP,		false,	EXIT,
										 CTX_IGBRIEFING,				DevKey::ENTER,			CTX_BACKTOGAME,		false,	STARTGAME,
										 CTX_IMREADY,					DevKey::ESCAPE,			CTX_JOIN,			false,	EXIT,
										 CTX_CADEBRIEFING,				DevKey::ENTER,			CTX_SCENARIO,		false,  CONTINUE,
									   	 CTX_SKDEBRIEFING,				DevKey::ENTER,			CTX_SKIRMISH,		false,	CONTINUE,
									   	 CTX_MPDEBRIEFING,				DevKey::ENTER,			CTX_JOIN,			false,	CONTINUE,
									   	 CTX_CASTATISTICS,				DevKey::ENTER,			CTX_CADEBRIEFING,	false, 	EXIT,
									   	 CTX_CASTATISTICS,				DevKey::ESCAPE,			CTX_CADEBRIEFING,	false, 	EXIT,
									   	 CTX_SKSTATISTICS,				DevKey::ENTER,			CTX_SKDEBRIEFING,	false,	EXIT,
									   	 CTX_SKSTATISTICS,				DevKey::ESCAPE,			CTX_SKDEBRIEFING,	false,	EXIT,
   									   	 CTX_MPSTATISTICS,				DevKey::ENTER,			CTX_MPDEBRIEFING,	false,	EXIT,
   									   	 CTX_MPSTATISTICS,				DevKey::ESCAPE,			CTX_MPDEBRIEFING,	false,	EXIT,
									     CTX_CAINGAMEOPTIONS,			DevKey::ENTER,			CTX_BACKTOGAME,		false, 	CONTINUE,
									     CTX_CAINGAMEOPTIONS,			DevKey::ESCAPE,			CTX_SCENARIO,		false, 	EXIT,
									     CTX_SKINGAMEOPTIONS,			DevKey::ENTER,			CTX_BACKTOGAME,		false, 	CONTINUE,
									     CTX_SKINGAMEOPTIONS,			DevKey::ESCAPE,			CTX_SKIRMISH,		false, 	EXIT,
									     CTX_MPINGAMEOPTIONS,			DevKey::ENTER,			CTX_BACKTOGAME,		false, 	CONTINUE,
   									   	 CTX_MPINGAMEOPTIONS,			DevKey::ESCAPE,			CTX_JOIN,			false, 	EXIT,
   									   	 CTX_MAINMENU, 					DevKey::ESCAPE,			CTX_MAINMENU,		false,	BE_DUMMY_OK,
										 CTX_SPLASH1,					DevKey::ESCAPE,			CTX_SPLASH2,		false,	EXIT,
										 CTX_SPLASH1,					DevKey::SPACE,			CTX_SPLASH2,		false,	EXIT,
										 CTX_SPLASH1,					DevKey::ENTER,			CTX_SPLASH2,		false,	EXIT,
										 CTX_SPLASH2,					DevKey::ESCAPE,			CTX_FINISH,			false,	EXIT,
										 CTX_SPLASH2,					DevKey::ENTER,			CTX_FINISH,			false,	EXIT,
										 CTX_SPLASH2,					DevKey::SPACE,			CTX_FINISH,			false,	EXIT,
										 CTX_FINISH, 					DevKey::SPACE,			CTX_FINISH,			false,	NO_IMP /*This line must come last*/	};

	return cki;
}
#else
//static
MachGuiStartupScreens::ContextKeypressInfo* MachGuiStartupScreens::getContextKeypressInfo()
{
	static ContextKeypressInfo cki[] = { CTX_DEFEAT,					DevKey::ESCAPE,			CTX_DEBRIEFING,		false,	NO_IMP,
										 CTX_VICTORY,					DevKey::ESCAPE,			CTX_DEBRIEFING,		false,	NO_IMP,
										 CTX_LEGALSCREEN,				DevKey::ESCAPE,			CTX_PROBEACCLAIMLOGO,false, NO_IMP,
										 CTX_LEGALSCREEN,				DevKey::SPACE,			CTX_PROBEACCLAIMLOGO,false, NO_IMP,
										 CTX_PROBEACCLAIMLOGO,			DevKey::ESCAPE,			CTX_CHARYBDISLOGO,	false,	NO_IMP,
										 CTX_CHARYBDISLOGO,				DevKey::ESCAPE,			CTX_INTROANIMATION,	false,	NO_IMP,
										 CTX_DEFEAT,					DevKey::SPACE,			CTX_DEBRIEFING,		false,	NO_IMP,
										 CTX_VICTORY,					DevKey::SPACE,			CTX_DEBRIEFING,		false,	NO_IMP,
										 CTX_PROBEACCLAIMLOGO,			DevKey::SPACE,			CTX_CHARYBDISLOGO,	false,	NO_IMP,
										 CTX_CHARYBDISLOGO,				DevKey::SPACE,			CTX_INTROANIMATION,	false,	NO_IMP,
										 CTX_MULTIPLAYER,				DevKey::ENTER,			CTX_JOIN,			true,   JOIN,
										 CTX_MULTIPLAYER,				DevKey::ESCAPE,			CTX_MAINMENU,		true,	EXIT,
										 CTX_OPTIONS,  					DevKey::ENTER,			CTX_MAINMENU,		true,   BE_OK,
									     CTX_OPTIONS,  				    DevKey::ESCAPE,			CTX_MAINMENU,		true, 	EXIT,
									     CTX_IGOPTIONS,  		 		DevKey::ENTER,			CTX_INGAMEOP,		true,	BE_OK,
									     CTX_IGOPTIONS,  				DevKey::ESCAPE,			CTX_INGAMEOP,		true,	EXIT,
									     CTX_SINGLEPLAYER,  			DevKey::ESCAPE,			CTX_MAINMENU,		true, 	EXIT,
										 CTX_JOIN,						DevKey::ESCAPE,			CTX_MULTIPLAYER,	true,	EXIT,
										 CTX_SETTINGS,	   				DevKey::ESCAPE, 		CTX_IMREADY,		true,	EXIT,
									   	 CTX_SETTINGS,					DevKey::ENTER,			CTX_IMREADY,		true,	BE_OK,
										 CTX_SKIRMISH,					DevKey::ESCAPE,			CTX_SINGLEPLAYER,	true,	EXIT,
										 CTX_SKIRMISH,					DevKey::ENTER,			CTX_SKIRMISH_GAME, 	false,	BE_OK,
								  	     CTX_CAMPAIGN, 					DevKey::ESCAPE,  		CTX_SINGLEPLAYER,	true, 	EXIT,
									     CTX_CAMPAIGN, 					DevKey::ENTER, 			CTX_SCENARIO,		true,	BE_OK,
									     CTX_SCENARIO, 					DevKey::ESCAPE,  		CTX_CAMPAIGN,		true, 	EXIT,
									     CTX_SCENARIO, 					DevKey::ENTER, 			CTX_BRIEFING,		true,	BE_OK,
									     CTX_BRIEFING,					DevKey::ESCAPE,			CTX_SCENARIO,		true,	EXIT,
									     CTX_BRIEFING,					DevKey::ENTER,			CTX_ENTRYFLIC,		false,	STARTGAME,
					 					 CTX_IGBRIEFING,				DevKey::ESCAPE,			CTX_INGAMEOP,		true,	EXIT,
										 CTX_IGBRIEFING,				DevKey::ENTER,			CTX_BACKTOGAME,		false,	STARTGAME,
										 CTX_IMREADY,					DevKey::ESCAPE,			CTX_JOIN,			true,	EXIT,
										 CTX_IGLOAD,					DevKey::ESCAPE,			CTX_INGAMEOP,		true,	EXIT,
										 CTX_IGLOAD,					DevKey::ENTER,			CTX_BACKTOGAME,		false,	BE_OK,
										 CTX_LOAD,						DevKey::ESCAPE,			CTX_SINGLEPLAYER, 	true,	EXIT,
										 CTX_LOAD,						DevKey::ENTER,			CTX_BACKTOGAME, 	false,	BE_OK,
										 CTX_SAVE,						DevKey::ESCAPE,			CTX_INGAMEOP, 		true,	EXIT,
										 CTX_SAVE,						DevKey::ENTER,			CTX_INGAMEOP, 		true,	BE_OK,
									   	 CTX_CADEBRIEFING,				DevKey::ENTER,			CTX_SCENARIO,		true,  	CONTINUE,
									   	 CTX_SKDEBRIEFING,				DevKey::ENTER,			CTX_SKIRMISH,		true,	CONTINUE,
									   	 CTX_MPDEBRIEFING,				DevKey::ENTER,			CTX_JOIN,			true,	CONTINUE,
									   	 CTX_CASTATISTICS,				DevKey::ENTER,			CTX_CADEBRIEFING,	true, 	EXIT,
									   	 CTX_CASTATISTICS,				DevKey::ESCAPE,			CTX_CADEBRIEFING,	true, 	EXIT,
									   	 CTX_SKSTATISTICS,				DevKey::ENTER,			CTX_SKDEBRIEFING,	true,	EXIT,
									   	 CTX_SKSTATISTICS,				DevKey::ESCAPE,			CTX_SKDEBRIEFING,	true,	EXIT,
   									   	 CTX_MPSTATISTICS,				DevKey::ENTER,			CTX_MPDEBRIEFING,	true,	EXIT,
   									   	 CTX_MPSTATISTICS,				DevKey::ESCAPE,			CTX_MPDEBRIEFING,	true,	EXIT,
									     CTX_CAINGAMEOPTIONS,			DevKey::ENTER,			CTX_BACKTOGAME,		false, 	CONTINUE,
										 CTX_SKINGAMEOPTIONS,			DevKey::ENTER,			CTX_BACKTOGAME,		false, 	CONTINUE,
										 CTX_MPINGAMEOPTIONS,			DevKey::ENTER,			CTX_BACKTOGAME,		false, 	CONTINUE,
										 CTX_HOTKEYS,					DevKey::ENTER,			CTX_INGAMEOP,		true,	EXIT,
   									   	 CTX_HOTKEYS,					DevKey::ESCAPE,			CTX_INGAMEOP,		true,	EXIT,
										 CTX_MAINMENU, 					DevKey::ESCAPE,			CTX_MAINMENU,		false,	BE_DUMMY_OK,
										 CTX_FINISH, 					DevKey::ESCAPE,			CTX_FINISH,			false,	NO_IMP /*This line must come last*/	};

	return cki;
}
#endif

//if the host has been lost and directX is asking me to be the host
bool	MachGuiStartupScreens::handleHostMessage()
{
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );
	CB_DEPIMPL(	bool, ignoreHostLostSystemMessage_ );

	NETWORK_STREAM("MachGuiStartupScreens::handleHostMessage\n" );
	switch( context_ )
	{
		//if we are on the ready screen then we can just switch back a level.
		//this is the only case that we know about
		case CTX_IMREADY:
		case CTX_SETTINGS:
			NETWORK_STREAM(" i'm ready or settings so going back to multiplayer\n" );
			if( not ignoreHostLostSystemMessage_ )
			{
				ignoreHostLostSystemMessage_ = true;
				buttonAction( MachGuiStartupScreens::EXIT );
				ignoreHostLostSystemMessage_ = false;
			}
			return true;
	}
	return false;
}

bool	MachGuiStartupScreens::handleDestroyPlayerMessage( const string& name )
{
	CB_DEPIMPL(	MachGuiStartupScreens::Context, context_ );
	CB_DEPIMPL(	MachGuiStartupData*, pStartupData_ );
	CB_DEPIMPL(	MachGuiStartupScreenContext*, pCurrContext_ );
	CB_DEPIMPL(	bool, ignoreHostLostSystemMessage_ );

	NETWORK_STREAM("MachGuiStartupScreens::handleDestroyPlayerMessage\n" );
	switch( context_ )
	{
		case CTX_IMREADY:
		case CTX_SETTINGS:
			NETWORK_STREAM(" am I host? " << pStartupData_->isHost() << std::endl );
			//if I'm the host then update player list and send around to everybody else.
			//otherwise this message will be handled by the host as so I don't have to do anything (unless host dropped)
			if( pStartupData_->isHost() )
			{
				for( int i = 0; i < 4; ++i )
					if( pStartupData_->players()[i].getDisplayName() == name )
						pStartupData_->players()[i].reset();
				//TBD: get rid of this cast (it is safe but a bit crap).
				if( context_ == CTX_IMREADY )
					_STATIC_CAST( MachGuiCtxImReady*, pCurrContext_ )->createPlayerList();
				pStartupData_->sendUpdatePlayersMessage();
			}
			else
			{
				NETWORK_STREAM(" I am not host so just determining if it was the host that fell over\n" );
				//determine which player went out.
				bool doSwitchAway = false;
				for( int i = 0; i < 4; ++i )
					if( pStartupData_->players()[i].getDisplayName() == name )
						if( pStartupData_->players()[i].host_ )

							doSwitchAway = true;

				//it was the host that went out so we need to switch to CTX_MULTIPLAYER
				if( doSwitchAway )
				{
					NETWORK_STREAM(" it was the host that went so goinf to pretend that I pressed exit\n" );
					ignoreHostLostSystemMessage_ = true;
					buttonAction( MachGuiStartupScreens::EXIT );
					ignoreHostLostSystemMessage_ = false;
					return true;
				}
				//not the host that dropped so we are ok for now.
			}
			return true;
	}
	return false;
}

class MachGuiSessionLostMessageBoxResponder : public MachGuiMessageBoxResponder
{
public:
	MachGuiSessionLostMessageBoxResponder( MachGuiStartupScreens* pStartupScreens )
	:	pStartupScreens_( pStartupScreens )
	{};

	virtual bool okPressed()
	{
// Changed, do not exit the game
//		pStartupScreens_->contextFinishFromLobby();
		return true;
	}

private:
	MachGuiStartupScreens* pStartupScreens_;
};

bool	MachGuiStartupScreens::handleSessionLostMessage()
{
	CB_DEPIMPL(	MachGuiMessageBoxResponder*, pMsgBoxResponder_ );
	if( pMsgBoxResponder_ )
	{
		_DELETE( pMsgBoxResponder_ );
		pMsgBoxResponder_ = NULL;
	}
	// Display message box. Must choose game to join.
 	pMsgBoxResponder_ = _NEW( MachGuiSessionLostMessageBoxResponder( this ) );
 	displayMsgBox( IDS_MULTI_ERROR_SESSION_LOST_MENUS );
	return true;
}

bool MachGuiStartupScreens::msgBoxIsBeingDisplayed() const
{
	CB_MachGuiStartupScreens_DEPIMPL();

	return (bool)pMsgBox_;
}

void MachGuiStartupScreens::initialiseVolumes()
{
	SysRegistry::KeyHandle handle;

	int initialVolume = 0;
	int theSize = 0;

	//If there is key in the registry for the sound volume
	if(SysRegistry::instance().onlyOpenKey( "Options\\Sound", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS)
	{
		//Set the intial volume to the registry value
		initialVolume = SysRegistry::instance().queryIntegerValue( "Options\\Sound", "Volume", SysRegistry::CURRENT_USER);
		SndMixer::instance().masterSampleVolume(initialVolume);
		SOUND_STREAM("Setting sound initialVolume to " << initialVolume << std::endl);
	}
	else
	{
		SOUND_STREAM("No sound key" << std::endl);
	}

	//If there is key in the registry for the CD volume
	if(SysRegistry::instance().onlyOpenKey( "Options\\CD", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS)
	{
		//Set the intial CD volume to the registry value
		initialVolume = SysRegistry::instance().queryIntegerValue( "Options\\CD", "Volume", SysRegistry::CURRENT_USER);
		DevCD::instance().volume(initialVolume);
		SOUND_STREAM("Setting CD initialVolume to " << initialVolume << std::endl);
	}
	else
	{
		SOUND_STREAM("No CD key" << std::endl);
	}
}

int MachGuiStartupScreens::xMenuOffset() const
{
	CB_MachGuiStartupScreens_DEPIMPL();

	const RenDisplay::Mode& mode = pSceneManager_->pDevice()->display()->currentMode();

	int xOffset = ( mode.width() - 640 ) / 2;

	return xOffset;
}

int MachGuiStartupScreens::yMenuOffset() const
{
	CB_MachGuiStartupScreens_DEPIMPL();

	const RenDisplay::Mode& mode = pSceneManager_->pDevice()->display()->currentMode();

	int yOffset = ( mode.height() - 480 ) / 2;

	return yOffset;
}

void MachGuiStartupScreens::blitBackdrop( const Gui::Box& box, const Gui::Coord& coord )
{
	GuiPainter::instance().blit( backdrop(),
								 Gui::Box(  Gui::Coord( box.minCorner().x() - xMenuOffset(),
														box.minCorner().y() - yMenuOffset() ),
											box.maxCorner().x() - box.minCorner().x(),
											box.maxCorner().y() - box.minCorner().y() ),
								 coord );
}

void MachGuiStartupScreens::addFocusCapableControl( MachGuiFocusCapableControl* pFocusCtrl )
{
	CB_DEPIMPL(	MachGuiStartupScreensImpl::FocusCapableControls, focusCapableControls_ );

	focusCapableControls_.push_back( pFocusCtrl );

	// Focus defaults to first control registered
	if ( focusCapableControls_.size() == 1 )
	{
		pFocusCtrl->hasFocus( true );
	}
}

void MachGuiStartupScreens::removeFocusCapableControl( MachGuiFocusCapableControl* pFocusCtrl )
{
	CB_DEPIMPL(	MachGuiStartupScreensImpl::FocusCapableControls, focusCapableControls_ );

	MachGuiStartupScreensImpl::FocusCapableControls::iterator i = find( focusCapableControls_.begin(), focusCapableControls_.end(), pFocusCtrl );

	ASSERT( i != focusCapableControls_.end(), "Trying to remove focusCapableControl which does not exist" );

	focusCapableControls_.erase( i );
}

bool MachGuiStartupScreens::doHandleFocusCapableControls( const GuiKeyEvent& e )
{
	CB_DEPIMPL(	MachGuiStartupScreensImpl::FocusCapableControls, focusCapableControls_ );
	CB_DEPIMPL(	MachGuiAutoDeleteDisplayable*, pMustContainMouse_ );

	bool processed = false;

	if (e.key() == DevKey::LEFT_ARROW or
		e.key() == DevKey::RIGHT_ARROW )
	{
		if ( GuiManager::instance().charFocusExists() and
			 &GuiManager::instance().charFocus() != this	)
		{
			// Char focus control often want to use Left and Right arrow, therefore
			// navigation with these keys is disabled.
			return false;
		}
	}

	// Do we have a control with focus that can respond to the key press?
	if (e.key() == DevKey::TAB or
		e.key() == DevKey::UP_ARROW or
		e.key() == DevKey::DOWN_ARROW or
		e.key() == DevKey::LEFT_ARROW or
		e.key() == DevKey::RIGHT_ARROW or
		e.key() == DevKey::ENTER or
		e.key() == DevKey::ESCAPE )
	{
		MachGuiFocusCapableControl* pFocusControl = NULL;
		MachGuiFocusCapableControl* pNextFocusControl = NULL;
		MachGuiFocusCapableControl* pPreviousFocusControl = NULL;
		MachGuiFocusCapableControl* pCurrentFocusControl = NULL;
		MachGuiFocusCapableControl* pEscapeFocusControl = NULL;
		MachGuiFocusCapableControl* pDefaultFocusControl = NULL;

		// Find control with escape focus
		for (   MachGuiStartupScreensImpl::FocusCapableControls::iterator escDefIter = focusCapableControls_.begin();
        		escDefIter != focusCapableControls_.end();
				++escDefIter )
		{
			if ( (*escDefIter)->isEscapeControl() and not (*escDefIter)->isDisabled() )
			{
				pEscapeFocusControl = (*escDefIter);
			}
			if ( (*escDefIter)->isDefaultControl() and not (*escDefIter)->isDisabled() )
			{
				pDefaultFocusControl = (*escDefIter);
			}
		}

		// Find control with focus
		for (   MachGuiStartupScreensImpl::FocusCapableControls::iterator i = focusCapableControls_.begin();
        		i != focusCapableControls_.end() and not pFocusControl;
				++i )
		{
			// Store previous focus control for navigation purposes
			if ( pCurrentFocusControl and not pCurrentFocusControl->isDisabled() )
			{
				pPreviousFocusControl = pCurrentFocusControl;
			}

			pCurrentFocusControl = *i;

			if ( pCurrentFocusControl->isFocusControl() )
			{
				pFocusControl = *i;

				// Store next and previous focus control for default navigation purposes.
				// Find next control that is not disabled
				++i;
				while ( i != focusCapableControls_.end() and not pNextFocusControl )
				{
					if ( not (*i)->isDisabled() )
					{
						pNextFocusControl = (*i);
					}
					else
					{
						++i;
					}
				}
				// Need to loop back to begining
				if ( not pNextFocusControl )
				{
					for (   MachGuiStartupScreensImpl::FocusCapableControls::iterator nextIter = focusCapableControls_.begin();
	        				nextIter != focusCapableControls_.end() and not pNextFocusControl;
							++nextIter )
					{
						if ( not (*nextIter)->isDisabled() )
						{
							pNextFocusControl = (*nextIter);
						}
					}
				}

				// Haven't got a previous focus control for navigation yet, so iterate through
				// list of controls and find the last non-disabled control.
				if ( not pPreviousFocusControl )
				{
					for (   MachGuiStartupScreensImpl::FocusCapableControls::iterator prevIter = focusCapableControls_.begin();
	        				prevIter != focusCapableControls_.end();
							++prevIter )
					{
						if ( not (*prevIter)->isDisabled() )
						{
							pPreviousFocusControl = (*prevIter);
						}
					}
				}
			}
		}

		// Escape key pressed?
		if ( e.key() == DevKey::ESCAPE )
		{
			// Do we have a control with escape focus?
			if ( pEscapeFocusControl and not pEscapeFocusControl->isDisabled() )
			{
				processed = pEscapeFocusControl->executeControl();
			}
		}
		else if ( pFocusControl )
		{
			ASSERT( pNextFocusControl, "We found a focus control so we should have found a next focus control" );
			ASSERT( pPreviousFocusControl, "We found a focus control so we should have found a previous focus control" );

			if ( e.key() == DevKey::ENTER )
			{
				// Enter key executes focus control
				if ( not pFocusControl->isDisabled() )
				{
					processed = pFocusControl->executeControl();
				}

				// If the control with focus didn't use the Enter key then give the default key a chance
				if ( not processed and pDefaultFocusControl and not pDefaultFocusControl->isDisabled() )
				{
					processed = pDefaultFocusControl->executeControl();
				}
			}
			else // Navigation
			{
				processed = true;

				MachGuiFocusCapableControl::NavKey navKey = MachGuiFocusCapableControl::DOWN_ARROW;
				switch ( e.key() )
				{
					case DevKey::UP_ARROW:
						navKey = MachGuiFocusCapableControl::UP_ARROW;
						break;
					case DevKey::DOWN_ARROW:
						navKey = MachGuiFocusCapableControl::DOWN_ARROW;
						break;
					case DevKey::RIGHT_ARROW:
						navKey = MachGuiFocusCapableControl::RIGHT_ARROW;
						break;
					case DevKey::LEFT_ARROW:
						navKey = MachGuiFocusCapableControl::LEFT_ARROW;
						break;
					case DevKey::TAB:
						if ( e.isShiftPressed() )
						{
							navKey = MachGuiFocusCapableControl::TAB_BACKWARD;
						}
						else
						{
							navKey = MachGuiFocusCapableControl::TAB_FOWARD;
						}
						break;
					DEFAULT_ASSERT_BAD_CASE( e.key() );
				}

				// Allow current focus control to control the navigation otherwise dropback to
				// default navigation behaivour
				MachGuiFocusCapableControl* pNewFocusControl = NULL;
				if ( pFocusControl->doHandleNavigationKey( navKey, &pNewFocusControl ) )
				{
					if ( pNewFocusControl )
					{
						pFocusControl->hasFocus( false );
						pNewFocusControl->hasFocus( true );
						MachGuiSoundManager::instance().playSound( "gui/sounds/navkey.wav" );
					}
				}
				else
				{
					pFocusControl->hasFocus( false );
					MachGuiSoundManager::instance().playSound( "gui/sounds/navkey.wav" );

					switch ( navKey )
					{
						case MachGuiFocusCapableControl::UP_ARROW:
						case MachGuiFocusCapableControl::LEFT_ARROW:
						case MachGuiFocusCapableControl::TAB_BACKWARD:
							pPreviousFocusControl->hasFocus( true );
							break;
						case MachGuiFocusCapableControl::DOWN_ARROW:
						case MachGuiFocusCapableControl::RIGHT_ARROW:
						case MachGuiFocusCapableControl::TAB_FOWARD:
							pNextFocusControl->hasFocus( true );
							break;
					}
				}
			}
		}
	}

	// If a drop down list was displayed and keyboard navigation was used then delete the drop down list
	if ( processed )
	{
		_DELETE( pMustContainMouse_ );
		pMustContainMouse_ = NULL;
	}

	return processed;
}

void MachGuiStartupScreens::messageBoxHasFocus( bool newValue )
{
	CB_DEPIMPL(	MachGuiStartupScreensImpl::FocusCapableControls, focusCapableControls_ );

	for (   MachGuiStartupScreensImpl::FocusCapableControls::iterator iter = focusCapableControls_.begin();
			iter != focusCapableControls_.end();
			++iter )
	{
		(*iter)->msgBoxIsDisplayed( newValue );
	}
}

MachGuiDbScenario* MachGuiStartupScreens::workOutWhichScenarioToDefaultTo()
{
	PRE( MachGuiDatabase::instance().hasCurrentPlayer() );
	PRE( MachGuiDatabase::instance().currentPlayer().nPlayerScenarios() != 0 );

	uint numSystems = MachGuiDatabase::instance().nCampaignSystems();
	bool selectNextItem = false;

	// Default scenario to last one played then search for one after this one
	MachGuiDbScenario* pDefaultScenario = &MachGuiDatabase::instance().currentPlayer().mostRecentPlayerScenario().scenario();

	if ( MachGuiDatabase::instance().currentPlayer().mostRecentPlayerScenario().hasWon() )
	{
		// Iterate through systems
		for ( uint loop = 0; loop < numSystems; ++loop )
		{
	    	MachGuiDbSystem& system = MachGuiDatabase::instance().campaignSystem( loop );

			// Iterate through planets
			uint numPlanets = system.nPlanets();

			for ( uint loop = 0; loop < numPlanets; ++loop )
			{
	    		MachGuiDbPlanet& planet = system.planet( loop );

				// Iterate through scenarios
				uint numScenarios = planet.nScenarios();

				for ( uint loop = 0; loop < numScenarios; ++loop )
				{
	    			MachGuiDbScenario& scenario = planet.scenario( loop );

					if ( selectNextItem and scenario.isAvailable() )
					{
						pDefaultScenario = &scenario;
						return pDefaultScenario;	 // Found default scenario, no point continuing
					}
					else if ( pDefaultScenario == &scenario )
					{
						selectNextItem = true;
					}
				}
			}
		}
	}

	return pDefaultScenario;
}

void MachGuiStartupScreens::contextFinishFromLobby()
{
	deleteAllChildren();
	contextFinish();
}

bool MachGuiStartupScreens::ignoreHostLostSystemMessage() const
{
	CB_DEPIMPL(	bool, ignoreHostLostSystemMessage_ );
	return ignoreHostLostSystemMessage_;
}

void MachGuiStartupScreens::ignoreHostLostSystemMessage(bool value)
{
	CB_DEPIMPL(	bool, ignoreHostLostSystemMessage_ );
	ignoreHostLostSystemMessage_ = value;
}
/* End STARTUP.CPP **************************************************/

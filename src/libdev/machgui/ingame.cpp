/*
 * I N G A M E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "machgui/ingame.hpp"
#include "machgui/hwrebank.hpp"
#include "machgui/cameras.hpp"
#include "machgui/squadron.hpp"
#include "machgui/gui.hpp"
#include "machgui/guictrl.hpp"
#include "machgui/corral.hpp"
#include "machgui/commbank.hpp"
#include "machgui/worldvie.hpp"
#include "machgui/consmenu.hpp"
#include "machgui/hwremenu.hpp"
#include "machgui/promtext.hpp"
#include "machgui/igcurs2d.hpp"
#include "machgui/prodbank.hpp"
#include "machgui/squadron.hpp"
#include "machgui/bmutext.hpp"
#include "machgui/oneicon.hpp"
#include "machgui/bildmenu.hpp"
#include "machgui/machnav.hpp"
#include "machgui/map.hpp"
#include "machgui/actnamid.hpp"
#include "machgui/fstpersn.hpp"
#include "machgui/ctrladon.hpp"
#include "machgui/camscrol.hpp"
#include "machgui/controlp.hpp"
#include "machgui/maparea.hpp"
#include "machgui/chatmsgs.hpp"
#include "machgui/chatmsgd.hpp"
#include "machgui/internal/igameimp.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "machgui/database.hpp"
#include "machgui/dbscenar.hpp"

// Commands
#include "machgui/cmdattac.hpp"
#include "machgui/cmdtrech.hpp"
#include "machgui/cmdmove.hpp"
#include "machgui/cmddeplo.hpp"
#include "machgui/cmdconst.hpp"
#include "machgui/cmdlocto.hpp"
#include "machgui/cmdlmine.hpp"
#include "machgui/cmdrefil.hpp"
#include "machgui/cmdheal.hpp"
#include "machgui/cmdpatro.hpp"
#include "machgui/cmddefau.hpp"
#include "machgui/cmdbuild.hpp"
#include "machgui/cmdresea.hpp"
#include "machgui/cmdscav.hpp"
#include "machgui/cmdtrans.hpp"
#include "machgui/cmdpicku.hpp"
#include "machgui/cmddestr.hpp"
#include "machgui/cmdfrmsq.hpp"
#include "machgui/cmddecon.hpp"
#include "machgui/cmdrecyc.hpp"
#include "machgui/cmdrepar.hpp"
#include "machgui/cmdcaptr.hpp"
#include "machgui/cmdstop.hpp"
#include "machgui/cmddefcn.hpp"
#include "machgui/cmdstand.hpp"
#include "machgui/cmdassem.hpp"
#include "machgui/cmdionat.hpp"
#include "machgui/cmdnukat.hpp"
#include "machgui/cmdcamo.hpp"

#include "machlog/machlog.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/factory.hpp"
#include "machlog/debris.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/pod.hpp"
#include "machlog/stats.hpp"
#include "machlog/vmman.hpp"
#include "machlog/network.hpp"
#include "machlog/mine.hpp"
#include "machlog/minesite.hpp"
#include "machlog/strategy.hpp"
#include "machlog/technici.hpp"
#include "machlog/oreholo.hpp"
#include "machlog/apc.hpp"
#include "machphys/techdata.hpp"
#include "machphys/apcdata.hpp"
#include "machphys/rcardata.hpp"
#include "gui/gui.hpp"
#include "gui/event.hpp"
#include "gui/manager.hpp"
#include "gui/painter.hpp"
#include "gui/restring.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/domain.hpp"
#include "world4d/soundman.hpp"
#include "render/device.hpp"
#include "render/display.hpp"
#include "render/capable.hpp"
#include "afx/resource.hpp"
#include "machgui/internal/strings.hpp"
#include "sim/manager.hpp"
#include "network/node.hpp"
#include "device/time.hpp"
#include "system/registry.hpp"
#include <algorithm>
#include <stdio.h>

// MachInGameScreen GuiRoot is made very big (10000x10000) to cope with all possible screen resolutions.
MachInGameScreen::MachInGameScreen( W4dSceneManager* pSceneManager, W4dRoot* pRoot, BaseProgressReporter* pReporter )
: 	GuiRoot( Gui::Box( 0, 0, 10000/*pSceneManager->pDevice()->windowWidth()*/, 10000/*pSceneManager->pDevice()->windowHeight()*/ ) ),
	pImpl_( _NEW( MachInGameScreenImpl() ) )
{
	CB_DEPIMPL( bool, inFirstPerson_ );
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );
	CB_DEPIMPL( MachGuiFirstPerson*, pFirstPerson_ );
	CB_DEPIMPL( MachActor*, pPromptTextActor_ );
	CB_DEPIMPL( MachGuiControlPanelAddOn*, pControlPanelAddOn_ );
	CB_DEPIMPL( MachCameras*, pCameras_ );
	CB_DEPIMPL( MachWorldViewWindow*, pWorldViewWindow_ );
  	CB_DEPIMPL( MachGuiBmuButton*, pBmuButton_ );
	CB_DEPIMPL( GameState, gameState_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( W4dRoot*, pRoot_ );
	CB_DEPIMPL( Actors, selectedActors_ );
    CB_DEPIMPL( Commands, allCommands_ );
	CB_DEPIMPL( AfxResourceLib*, pStringResourceLib_ );
	CB_DEPIMPL( MachMachinesIcon*, pMachinesIcon_ );
	CB_DEPIMPL( MachConstructionsIcon*, pConstructionsIcon_ );
	CB_DEPIMPL( MachSquadronIcon*, pSquadronIcon_ );
	CB_DEPIMPL( MachGuiCorralSingleIcon*, pCorralSingleIcon_ );
	CB_DEPIMPL( MachGuiCorral*, pCorral_ );
	CB_DEPIMPL( MachSmallCommandIcons*, pSmallCommandIcons_ );
	CB_DEPIMPL( MachCommandIcons*, pCommandIcons_ );
    CB_DEPIMPL( MachInGameCursors2d*, pCursors2d_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );
	CB_DEPIMPL( MachGuiMapArea*, pMapArea_ );
	#ifndef PRODUCTION
	CB_DEPIMPL( bool, showCurrentMachine_ );
	CB_DEPIMPL( bool, showNetworkStuffed_ );
	string showStuffedString;
	showNetworkStuffed_ = ( NULL != getenv("CB_SHOW_NETWORK_STUFFED") );
	#endif

	// Store sceneManager and root
	pSceneManager_ = pSceneManager;
	pRoot_ = pRoot;

	//Set reasonable collection allocation size
    selectedActors_.reserve( 64 );
    allCommands_.reserve( 32 );

	// Set up area to hold map and other buttons at top of control panel
	pMapArea_ = _NEW( MachGuiMapArea( this, Gui::Boundary(0,0,MachGui::controlPanelOutXPos(), MachGui::mapAreaHeight() ) ) );
	pReporter->report( 10, 100 ); // 10% done

	// Set up sliding control panel section
	pControlPanel_ = _NEW( MachGuiControlPanel( this, Gui::Boundary(0, MachGui::mapAreaHeight(), MachGui::controlPanelOutXPos(),2000 ), this ) );
	pReporter->report( 15, 100 ); // 15% done

	// Create cameras
    pCameras_ = _NEW( MachCameras( pSceneManager, pRoot ) );
	pReporter->report( 20, 100 ); // 20% done

    //Load the string table resource file
    pStringResourceLib_ = _NEW( AfxResourceLib( SysPathName( "machstrg.xml" ) ) );
    GuiResourceString::resource( pStringResourceLib_ );
	pReporter->report( 25, 100 ); // 25% done

    //Construct the set of available commands
    initialiseAllCommands();
	pReporter->report( 28, 100 ); // 28% done

	//Set the viewport boundary for the world view window
    RenDevice& device = *pSceneManager->pDevice();
	const int w = device.windowWidth();
    const int h = device.windowHeight();

    //Create the world view window
	pWorldViewWindow_ = _NEW( MachWorldViewWindow( this, Gui::Boundary( MachGui::controlPanelOutXPos(), 0,	2000, 2000 ), pCameras_ ) );
	pReporter->report( 30, 100 ); // 30% done

	// Construct add-on piece of control panel ( top-right )
    pControlPanelAddOn_ = _NEW( MachGuiControlPanelAddOn( this, Gui::Coord( MachGui::controlPanelOutXPos(), 0 ), this ) );
	pReporter->report( 35, 100 ); // 35% done

    //Construct continent map
	pContinentMap_ = _NEW( MachContinentMap( pMapArea_, Gui::Coord( 2, 1 ), pCameras_, this ) );
	pReporter->report( 40, 100 ); // 40% done

	//Construct buttons down side of continent map
    Gui::XCoord x = pContinentMap_->absoluteBoundary().maxCorner().x();

	pBmuButton_ = _NEW( MachGuiBmuButton( pMapArea_, Gui::Coord( x, 0 ), SysPathName( "gui/navigate/bmus.bmp" ), pContinentMap_->bmuText(), pContinentMap_, this ) );
	pMachinesIcon_ = _NEW( MachMachinesIcon( pMapArea_, Gui::Coord( x, pBmuButton_->absoluteBoundary().maxCorner().y() - 1 ), this ) );
	pConstructionsIcon_ = _NEW( MachConstructionsIcon( pMapArea_, Gui::Coord( x, pMachinesIcon_->absoluteBoundary().maxCorner().y() - 1 ), this ) );
	pSquadronIcon_ = _NEW( MachSquadronIcon( pMapArea_, Gui::Coord( x, pConstructionsIcon_->absoluteBoundary().maxCorner().y() - 1 ), this ) );
	pBmuButton_->setLayer( GuiDisplayable::LAYER2 );
	pMachinesIcon_->setLayer( GuiDisplayable::LAYER2 );
	pConstructionsIcon_->setLayer( GuiDisplayable::LAYER2 );
	pSquadronIcon_->setLayer( GuiDisplayable::LAYER2 );
 	pReporter->report( 50, 100 ); // 50% done

	//Construct single icon corral
	pCorralSingleIcon_ = _NEW( MachGuiCorralSingleIcon( pControlPanel_, Gui::Coord( 1,0 ), this ) );

	//Construct small command icons
	Gui::Coord smallCommandCoord( 2, pCorralSingleIcon_->height() + 2 );
	pSmallCommandIcons_= _NEW( MachSmallCommandIcons( pControlPanel_, smallCommandCoord, this ) );
	pReporter->report( 55, 100 ); // 55% done

	// Create navigators
	setupNavigators();
	pReporter->report( 60, 100 ); // 60% done

	//Reset the context to main menu context
	resetContext();

	//Load up the 2d cursors
    pCursors2d_ = _NEW( MachInGameCursors2d( this ) );
	pReporter->report( 65, 100 ); // 65% done

	//Create first person interface
	pFirstPerson_ = _NEW( MachGuiFirstPerson( pSceneManager, pRoot, this ) );
	pReporter->report( 70, 100 ); // 70% done

	useFastSecondDisplay( false );

	#ifndef PRODUCTION
	showCurrentMachine_ = false;
	#endif

	DEBUG_STREAM( DIAG_NEIL, "MachInGameScreen::CTOR exit" << std::endl );
}

MachInGameScreen::~MachInGameScreen()
{
	CB_DEPIMPL( Commands, allCommands_ );
	CB_DEPIMPL( MachGuiCommand*, pActiveCommand_ );
	CB_DEPIMPL( MachGuiCommand*, pDefaultCommand_ );
	CB_DEPIMPL( MachInGameCursors2d*, pCursors2d_ );
	CB_DEPIMPL( MachCameras*, pCameras_ );
	CB_DEPIMPL( MachGuiFirstPerson*, pFirstPerson_ );
	CB_DEPIMPL( AfxResourceLib*, pStringResourceLib_ );

	DEBUG_STREAM( DIAG_NEIL, "MachInGameScreen::DTOR enter" << std::endl );

    //NB All the child GuiDisplayables are deleted automatically

	//Delete the commands
    for( Commands::iterator it = allCommands_.begin(); it != allCommands_.end(); ++it )
        _DELETE( *it );

    //Delete various bits and pieces
    _DELETE( pActiveCommand_ );
    _DELETE( pDefaultCommand_ );
    _DELETE( pCursors2d_ );
	_DELETE( pCameras_ );
	_DELETE( pFirstPerson_ );

    //Unload the string table resource file
    GuiResourceString::clearResource();
    _DELETE( pStringResourceLib_ );

	// Delete implementation class
	_DELETE( pImpl_ );

	DEBUG_STREAM( DIAG_NEIL, "MachInGameScreen::DTOR leave" << std::endl );
}

void MachInGameScreen::doBecomeRoot()
{
	CB_DEPIMPL( MachInGameCursors2d*, pCursors2d_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );
	CB_DEPIMPL( MachProductionBank*, pProductionBank_ );
	CB_DEPIMPL( MachHWResearchBank*, pHWResearchBank_ );
	CB_DEPIMPL( bool, resolutionChanged_ );
	CB_DEPIMPL( MachGuiInGameChatMessagesDisplay*, pChatMessageDisplay_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( int, redrawMapCounter_ );
	CB_DEPIMPL( MachCameras*, pCameras_ );

	// Reposition debug text
	pSceneManager_->pDevice()->debugTextCoords(204, 0);

	if ( resolutionChanged_ )
	{
		// Setup areas of screen that cause camera to scroll
		setupCameraScrollAreas();
		// Reposition prompt text
		setupPromptText();
		// Reposition control panel decals
		pControlPanel_->setupDecalCoords();
		// Reposition navigators
		setupNavigators();
		// Resize corral and reposition command icons
		setupCorralAndCommandIcons();
		// Clean up production and hw-research banks before resetting context, this
		// will ensure that they are recreated.
		_DELETE( pProductionBank_ );
	    pProductionBank_ = NULL;
		_DELETE( pHWResearchBank_ );
	    pHWResearchBank_ = NULL;
		// Change context to same context but get code to recreate the
		// gui controls relevant to that context.
		currentContext( currentContext(), true );

		// Reset resolution changed flag
		resolutionChanged_ = false;
	}

	// Change to menu cursor.
	pCursors2d_->forceCursor( MachGui::MENU_CURSOR );

	// Remove any previously displayed prompt text
	clearCursorPromptText();
	clearCommandPromptText();

	// Make sure that we redraw all chat messages
	if ( pChatMessageDisplay_ )
		pChatMessageDisplay_->forceUpdate();

	// Fully redraw map for next 3 frames
	redrawMapCounter_ = 3;

	// Configure pitch up/down keys for ground camera
	pCameras_->reversePitchUpDownKeys( SysRegistry::instance().queryIntegerValue( "Options\\Reverse UpDown Keys", "on", SysRegistry::CURRENT_USER ) );
}

void MachInGameScreen::doBecomeNotRoot()
{
	//Stop all playing sounds
	W4dSoundManager::instance().stopAll();
	// Clean up any gui sounds that are currently playing
	MachGuiSoundManager::instance().clearAll();
}

MachWorldViewWindow& MachInGameScreen::worldViewWindow()
{
	CB_DEPIMPL( MachWorldViewWindow*, pWorldViewWindow_ );

    PRE( pWorldViewWindow_ != NULL );

    return *pWorldViewWindow_;
}

MachGuiCorral& MachInGameScreen::corral()
{
	CB_DEPIMPL( MachGuiCorral*, pCorral_ );

    return *pCorral_;
}

const MachInGameScreen::Actors& MachInGameScreen::selectedActors() const
{
	CB_DEPIMPL( Actors, selectedActors_	);

    return selectedActors_;
}

bool MachInGameScreen::isSelected( MachActor& actor ) const
{
    return actor.selectionState() == MachLog::SELECTED;
}

void MachInGameScreen::select( MachActor* pActor )
{
	CB_DEPIMPL( Actors, selectedActors_	);
	CB_DEPIMPL( MachGuiCorralSingleIcon*, pCorralSingleIcon_ );
	CB_DEPIMPL(	MachGuiFirstPerson*, pFirstPerson_ );
	CB_DEPIMPL(	bool, commandBankNeedsUpdating_ );

	DEBUG_STREAM( DIAG_NEIL, "MachInGameScreen::select( MachActor* pActor )" );

    PRE( not isSelected( *pActor ) );

    //Mark actor as selected
    pActor->selectionState( MachLog::SELECTED );

    //Add to local collection
    selectedActors_.push_back( pActor );

	updateCorralState();

    //Become an observer, in case it gets deleted etc
    pActor->attach( this );

    //Add to corral
    corral().add( pActor );

	// Update single icon corral and first person
	if ( selectedActors_.size() == 1 )
	{
		pCorralSingleIcon_->setActor( pActor, true );

		// Tell first person control which actor has been selected
		pFirstPerson_->setActor( pActor );
	}
	else
	{
		pCorralSingleIcon_->clear();
		pFirstPerson_->resetActor();
	}

    //Command buttons will need processing
    commandBankNeedsUpdating_ = true;

	// Reset the self-destruct icon
	MachGuiSelfDestructCommand::resetButtonState();

    //Any active command should be cancelled - but deferred to next update
    cancelActiveCommand();

    // Selection has changed therefore we no longer have a squad
    applyCommandToSquadron( false );

	checkDismissNavigator();

	//Add/remove any associated banks
    setupActorBank();

	DEBUG_STREAM( DIAG_NEIL, "leaving MachInGameScreen::select( MachActor* pActor )" );
}

void MachInGameScreen::select( const Actors& actors )
{
	CB_DEPIMPL( Actors, selectedActors_	);
	CB_DEPIMPL( MachGuiCorralSingleIcon*, pCorralSingleIcon_ );
	CB_DEPIMPL(	MachGuiFirstPerson*, pFirstPerson_ );
	CB_DEPIMPL(	bool, commandBankNeedsUpdating_ );

	DEBUG_STREAM( DIAG_NEIL, "MachInGameScreen::select( const Actors& actors )" );

	for ( Actors::const_iterator iter = actors.begin(); iter != actors.end(); ++iter )
	{
		MachActor *pActor = (*iter);

		// Check to see if actor is already selected!!
    	ASSERT( not isSelected( *pActor ), "actor already selected" );

	    //Mark actor as selected
	    pActor->selectionState( MachLog::SELECTED );

	    //Add to local collection
	    selectedActors_.push_back( pActor );

	    //Become an observer, in case it gets deleted etc
	    pActor->attach( this );
	}

	//Add to corral
	corral().add( actors );

	updateCorralState();

	// Update single icon corral and first person
	if ( selectedActors_.size() == 1 )
	{
		// Get first actor
		MachActor* pActor = selectedActors_.front();

		// Update single icon corral
		pCorralSingleIcon_->setActor( pActor, true );

		// Tell first person control which actor has been selected
		pFirstPerson_->setActor( pActor );
	}
	else
	{
		pCorralSingleIcon_->clear();
		pFirstPerson_->resetActor();
	}

    //Command buttons will need processing
    commandBankNeedsUpdating_ = true;

	// Reset the self-destruct icon
	MachGuiSelfDestructCommand::resetButtonState();

    //Any active command should be cancelled - but deferred to next update
    cancelActiveCommand();

    // Selection has changed therefore we no longer have a squad
    applyCommandToSquadron( false );

	checkDismissNavigator();

	//Add/remove any associated banks
    setupActorBank();

	DEBUG_STREAM( DIAG_NEIL, "leaving MachInGameScreen::select( const Actors& actors )" );
}

void MachInGameScreen::deselect( const Actors& actors )
{
	CB_DEPIMPL( Actors, selectedActors_	);
	CB_DEPIMPL( MachGuiCorralSingleIcon*, pCorralSingleIcon_ );
	CB_DEPIMPL(	MachGuiFirstPerson*, pFirstPerson_ );
	CB_DEPIMPL(	bool, commandBankNeedsUpdating_ );

	removeHighlightedActor();

	for ( Actors::const_iterator iter = actors.begin(); iter != actors.end(); ++iter )
	{
		MachActor *pActor = (*iter);

		// Check to see if actor is selected. Cannot deselect an unselected actor!!
		ASSERT( isSelected( *pActor ), "actor not selected" );

		//Cease being an observer
	    pActor->detach( this );

	    //Mark actor as not selected
	    pActor->selectionState( MachLog::NOT_SELECTED );

	    //Remove from local collection
	    Actors::iterator it = find( selectedActors_.begin(), selectedActors_.end(), pActor );
	    ASSERT( it != selectedActors_.end(), "" );

	    selectedActors_.erase( it );
	}

	//Remove from corral
	corral().remove( actors );

	updateCorralState();

	// Update single icon corral and first person
	if ( selectedActors_.size() == 1 )
	{
		// Get first actor
		MachActor* pActor = selectedActors_.front();

		// Update single icon corral
		pCorralSingleIcon_->setActor( pActor, true );

		// Tell first person control which actor has been selected
		pFirstPerson_->setActor( pActor );
	}
	else
	{
		pCorralSingleIcon_->clear();
		pFirstPerson_->resetActor();
	}

    //Command buttons will need processing
    commandBankNeedsUpdating_ = true;

	// Reset the self-destruct icon
	MachGuiSelfDestructCommand::resetButtonState();

    //Any active command should be cancelled - but deferred to next update
    cancelActiveCommand();

    // Selection has changed therefore we no longer have a squad
    applyCommandToSquadron( false );

	checkDismissNavigator();

	//Add/remove any associated banks
    setupActorBank();

	//Reset the defcon command's "movement direction" (which way it will move from the middle setting)
	defconCommand()->resetDirectionFromDefConNormal();
}

void MachInGameScreen::deselect( MachActor* pActor )
{
    PRE( isSelected( *pActor ) );

    //Cease being an observer
    pActor->detach( this );

    //Do the bulk of the work
    unselect( pActor );
}

void MachInGameScreen::unselect( MachActor* pActor )
{
	CB_DEPIMPL( Actors, selectedActors_	);
	CB_DEPIMPL( MachGuiCorralSingleIcon*, pCorralSingleIcon_ );
	CB_DEPIMPL(	MachGuiFirstPerson*, pFirstPerson_ );
	CB_DEPIMPL(	bool, commandBankNeedsUpdating_ );

	removeHighlightedActor();

    //Mark actor as not selected
    pActor->selectionState( MachLog::NOT_SELECTED );

    //Remove from local collection
    Actors::iterator it = find( selectedActors_.begin(), selectedActors_.end(), pActor );
    ASSERT( it != selectedActors_.end(), "" );

    selectedActors_.erase( it );

    //Remove from corral
    corral().remove( pActor );

	updateCorralState();

	// Update single icon corral and first person
	if ( selectedActors_.size() == 1 )
	{
		// Get first actor
		MachActor* pActor = selectedActors_.front();

		// Update single icon corral
		pCorralSingleIcon_->setActor( pActor, true );

		// Tell first person control which actor has been selected
		pFirstPerson_->setActor( pActor );
	}
	else
	{
		pCorralSingleIcon_->clear();
		pFirstPerson_->resetActor();

		if( selectedActors_.size() == 0 )
		{
			//Reset the defcon command's "movement direction"
			defconCommand()->resetDirectionFromDefConNormal();
		}
	}

    //Command buttons will need processing
    commandBankNeedsUpdating_ = true;

	// Reset the self-destruct icon
	MachGuiSelfDestructCommand::resetButtonState();

    //Any active command should be cancelled - but deferred to next update
    cancelActiveCommand();

    // Selection has changed therefore we no longer have a squad
    applyCommandToSquadron( false );

	checkDismissNavigator();

    //Add/remove any associated banks
    setupActorBank();
}

void MachInGameScreen::checkDismissNavigator()
{
	CB_DEPIMPL( MachMachinesIcon*, pMachinesIcon_ );
	CB_DEPIMPL( MachConstructionsIcon*, pConstructionsIcon_ );
	CB_DEPIMPL(	MachGuiMachineNavigator*, pMachineNavigation_ );
	CB_DEPIMPL(	MachGuiConstructionNavigator*, pConstructionNavigation_ );

	// Called when actor is selected/deselected.

	// Check to see if selection should cause navigator to be dismissed
	if ( not isMachineNavigationContext() or not pMachineNavigation_->remainVisible() )
	{
		if ( not isConstructionNavigationContext() or not pConstructionNavigation_->remainVisible() )
		{
			pMachinesIcon_->forceUp(); // Make sure machines navigation button pops up as we are leaving navigation mode
			pConstructionsIcon_->forceUp();

			mainMenuOrSingleFactoryContext();
		}
	}
	// Any other selections will cause the navigator to dismiss unless the navigator resets the
	// remain visible flag to true
	pMachineNavigation_->remainVisible( false );
	pConstructionNavigation_->remainVisible( false );
}

void MachInGameScreen::deselectAll()
{
	CB_DEPIMPL( Actors, selectedActors_	);
	CB_DEPIMPL(	bool, commandBankNeedsUpdating_ );
	CB_DEPIMPL( MachGuiCorralSingleIcon*, pCorralSingleIcon_ );
	CB_DEPIMPL(	MachGuiFirstPerson*, pFirstPerson_ );

    removeHighlightedActor();

	for ( Actors::iterator iter = selectedActors_.begin(); iter != selectedActors_.end(); ++iter )
    {
    	MachActor* pActor = *iter;

	    //Cease being an observer
	    pActor->detach( this );

	    //Mark actor as not selected
	    pActor->selectionState( MachLog::NOT_SELECTED );
	}

	//Remove from corral
   	corral().remove( selectedActors_ );

	// Clean up list of selected actors
	selectedActors_.erase( selectedActors_.begin(), selectedActors_.end()  );

	updateCorralState();

    //Command buttons will need processing
    commandBankNeedsUpdating_ = true;

	// Reset the self-destruct icon
	MachGuiSelfDestructCommand::resetButtonState();

    //Any active command should be cancelled - but deferred to next update
    cancelActiveCommand();

    // Selection has changed therefore we no longer have a squad
    applyCommandToSquadron( false );

	checkDismissNavigator();

    //Add/remove any associated banks
    setupActorBank();

	// Remove reference to selected actors from 1st person and single icon corral
	pCorralSingleIcon_->clear();
	pFirstPerson_->resetActor();
}

W4dSceneManager& MachInGameScreen::sceneManager() const
{
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );

    return *pSceneManager_;
}

MachGuiDefconCommand* MachInGameScreen::defconCommand()
{
	CB_DEPIMPL( MachGuiDefconCommand*, pDefconCommand_ );

	return pDefconCommand_;
}

MachGuiSelfDestructCommand* MachInGameScreen::selfDestructCommand()
{
	CB_DEPIMPL( MachGuiSelfDestructCommand*, pSelfDestructCommand_ );

	return pSelfDestructCommand_;
}

void MachInGameScreen::initialiseAllCommands()
{
	CB_DEPIMPL( MachGuiCommand*, pDefaultCommand_ );
	CB_DEPIMPL(	MachGuiDefconCommand*, pDefconCommand_ );
	CB_DEPIMPL(	MachGuiSelfDestructCommand*, pSelfDestructCommand_ );
	CB_DEPIMPL(	MachGuiIonAttackCommand*, pIonAttackCommand_ );
	CB_DEPIMPL(	MachGuiNukeAttackCommand*, pNukeAttackCommand_ );
	CB_DEPIMPL(	Commands, allCommands_ );

	DEBUG_STREAM( DIAG_NEIL, "MachInGameScreen::initialiseAllCommands" << std::endl << std::flush );
    pDefaultCommand_ = _NEW( MachGuiDefaultCommand( this ) );

	// Commands with special icon processing
	pSelfDestructCommand_ 	= _NEW( MachGuiSelfDestructCommand( this ) );
	pIonAttackCommand_ 		= _NEW( MachGuiIonAttackCommand( this ) );
	pNukeAttackCommand_ 	= _NEW( MachGuiNukeAttackCommand( this ) );
	pDefconCommand_			= _NEW( MachGuiDefconCommand( this ) );

	allCommands_.push_back( pSelfDestructCommand_ );
    allCommands_.push_back( _NEW( MachGuiMoveCommand( this ) ) );
	allCommands_.push_back( _NEW( MachGuiStopCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiAttackCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiConstructCommand( this ) ) );
	allCommands_.push_back( _NEW( MachGuiRepairCommand( this ) ) );
	allCommands_.push_back( _NEW( MachGuiCaptureCommand( this ) ) );
	allCommands_.push_back( _NEW( MachGuiDeconstructCommand( this ) ) );
	allCommands_.push_back( _NEW( MachGuiRecycleCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiLocateToCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiDropLandMineCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiRefillLandMineCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiDeployCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiPatrolCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiTransportCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiPickUpCommand( this ) ) );
	allCommands_.push_back( _NEW( MachGuiScavengeCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiTreacheryCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiFormSquadronCommand( this ) ) );
    allCommands_.push_back( _NEW( MachGuiHealCommand( this ) ) );
	allCommands_.push_back( _NEW( MachGuiStandGroundCommand( this ) ) );
	allCommands_.push_back( _NEW( MachGuiAssemblyPointCommand( this ) ) );
	allCommands_.push_back( pIonAttackCommand_ );
   	allCommands_.push_back( pNukeAttackCommand_ );
	allCommands_.push_back( pDefconCommand_ );
	allCommands_.push_back( _NEW( MachGuiCamouflageCommand( this ) ) );

	DEBUG_STREAM( DIAG_NEIL, "allcommands.size " << allCommands_.size() << std::endl << std::flush );
}

const MachInGameScreen::Commands& MachInGameScreen::allCommands() const
{
	CB_DEPIMPL(	Commands, allCommands_ );

    return allCommands_;
}

MachGuiCommand& MachInGameScreen::defaultCommand() const
{
	CB_DEPIMPL( MachGuiCommand*, pDefaultCommand_ );

    PRE( pDefaultCommand_ != NULL );

    return *pDefaultCommand_;
}

void MachInGameScreen::activateDefaultCommand()
{
	CB_DEPIMPL( MachGuiCommand*, pActiveCommand_ );
	CB_DEPIMPL( bool, cancelActiveCommand_ );

    //Cancel any existing command
    if( pActiveCommand_ != NULL )
    {
        pActiveCommand_->finish();
        _DELETE( pActiveCommand_ );
        pActiveCommand_ = NULL;
    }

    //Ensure the command prompt text is cleared
    clearCommandPromptText();

    //Any outstanding request hereby fulfilled
    cancelActiveCommand_ = false;
}

void MachInGameScreen::activeCommand( const MachGuiCommand& command )
{
    PRE( command.isVisible() );

	CB_DEPIMPL( MachGuiCommand*, pActiveCommand_ );

	// Can't issue commands if game is paused or network is busy
	if ( SimManager::instance().isSuspended() or isNetworkStuffed() )
	{
		MachGuiSoundManager::instance().playSound( "gui/sounds/clickbad.wav" );
	}
	else
	{
		//Make a fresh copy of the required command
		MachGuiCommand* pNewCommand = command.clone();

	    //Cancel any existing command
	    if( pActiveCommand_ != NULL )
	    {
	        pActiveCommand_->finish();
	        _DELETE( pActiveCommand_ );
	        pActiveCommand_ = NULL;
	    }

	    //Store copy of the required command
	    pActiveCommand_ = pNewCommand;

	    //Load the string for the command
	    GuiResourceString::Id id = pActiveCommand_->commandPromptStringid();
	    GuiResourceString prompt( id );

		// Set the command prompt
		commandPromptText( prompt.asString() );

	    //Do start processing
	    pActiveCommand_->start();
	}
}

MachGuiCommand& MachInGameScreen::activeCommand() const
{
	CB_DEPIMPL( MachGuiCommand*, pActiveCommand_ );
	CB_DEPIMPL( MachGuiCommand*, pDefaultCommand_ );

    PRE( pActiveCommand_ != NULL or pDefaultCommand_ != NULL );
    return ( pActiveCommand_ != NULL ? *pActiveCommand_ : *pDefaultCommand_ );
}

void MachInGameScreen::cursorFilter( int filter )
{
	CB_DEPIMPL( W4dDomain::EntityFilter, cursorFilter_ );

    //cursorFilter_ = filter;
    cursorFilter_ = _STATIC_CAST(W4dDomain::EntityFilter, filter);
}

int MachInGameScreen::cursorFilter() const
{
	CB_DEPIMPL( W4dDomain::EntityFilter, cursorFilter_ );

    return cursorFilter_;
}

void MachInGameScreen::highlightActor( MachActor* pHighlightActor )
{
	CB_DEPIMPL( Actors, selectedActors_ );
	CB_DEPIMPL( MachActor*, pHighlightedActor_ );
	CB_DEPIMPL( MachGuiCorralSingleIcon*, pCorralSingleIcon_ );

    if ( selectedActors_.empty() )
	{
		pHighlightedActor_ =  pHighlightActor;
		pCorralSingleIcon_->setActor( pHighlightedActor_ );
	}
}

bool MachInGameScreen::addPromptTextMachineInfo( MachActor* pActor, GuiString& prompt )
{
	bool processed = false;
	MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

	if ( pActor->objectIsMachine() and pActor->race() == playerRace  )
	{
		MachLogMachine& machine = pActor->asMachine();
 		char buffer[20];

		// Get HP and Armour info
//		GuiResourceString hpText( IDS_HEALTHPOINTS, GuiString( itoa( machine.hp(), buffer, 10 ) ) );
//		GuiResourceString apText( IDS_ARMOURPOINTS, GuiString( itoa( machine.armour(), buffer, 10 ) ) );
        sprintf(buffer, "%d", machine.hp());
        GuiResourceString hpText( IDS_HEALTHPOINTS, GuiString( buffer ) );
        sprintf(buffer, "%d", machine.armour());
		GuiResourceString apText( IDS_ARMOURPOINTS, GuiString( buffer ) );

		prompt += "\n" + hpText.asString() + " " + apText.asString();

		// For technicians add on how clever they are onto the prompt text
		if ( pActor->objectType() == MachLog::TECHNICIAN )
		{
//			GuiResourceString rpText( IDS_RESEARCHPOINTS, GuiString( itoa( pActor->asTechnician().data().researchRate(), buffer, 10 ) ) );
			sprintf(buffer, "%d", pActor->asTechnician().data().researchRate());
			GuiResourceString rpText( IDS_RESEARCHPOINTS, GuiString( buffer ) );
			prompt += " " + rpText.asString();
		}
		// Resource carrier info
		else if ( pActor->objectType() == MachLog::RESOURCE_CARRIER and
				  pActor->asResourceCarrier().isNormalResourceCarrier() )
		{
			char buffer2[20];
//			itoa( pActor->asResourceCarrier().data().capacity(), buffer, 10 );
//			itoa( pActor->asResourceCarrier().amountCarried(), buffer2, 10 );
            sprintf(buffer, "%d", pActor->asResourceCarrier().data().capacity());
            sprintf(buffer2, "%d", pActor->asResourceCarrier().amountCarried());

			GuiResourceString bmuText( IDS_BMUPOINTS, buffer2 );
			prompt += " " + bmuText.asString();
			prompt += "/";
			prompt += buffer;
		}
		// For APC add on contents
		else if ( pActor->objectType() == MachLog::APC )
		{
			GuiResourceString apcStartText( IDS_APCINFO_START );
			GuiResourceString apcEndText( IDS_APCINFO_END );
			GuiResourceString administratorText( IDS_APCINFO_ADMINISTRATOR );
			GuiResourceString aggressorText( IDS_APCINFO_AGGRESSOR );
			GuiResourceString civilianText( IDS_APCINFO_CONSTRUCTOR );
			GuiResourceString spaceText( IDS_APCINFO_SPACE );

			bool first = true;
			int count = 0;
			MachLogAPC& apc = pActor->asAPC();

			for ( MachLogAPC::Machines::const_iterator iter = apc.machines().begin(); iter != apc.machines().end() and count < 10; ++iter )
			{
				if ( first )
				{
					prompt += " " + apcStartText.asString();
					first = false;
				}

				switch ( (*iter)->objectType() )
				{
					case MachLog::ADMINISTRATOR:
						prompt += administratorText.asString();
						break;
					case MachLog::AGGRESSOR:
						prompt += aggressorText.asString();
						break;
					case MachLog::APC:
						ASSERT_FAIL( "APCs are not allowed inside APCs" );
						break;
					default:
						prompt += civilianText.asString();
						break;
				}

				++count;
				// If more than 10 machines in APC then end with ... to indicate more machines.
				if ( count == 10 and count != apc.machines().size() )
				{
					prompt += spaceText.asString();
				}
			}

			if ( not first )
			{
				prompt += apcEndText.asString();
			}
		}

		// Add on a string describing the machines current operation
		if ( not pActor->isIdle() )
		{
			GuiResourceString opString( 9000/*IDS_xxx values for ops start at 9000*/ + pActor->strategy().currentOperationType() );
			prompt += " " + opString.asString();
		}

		processed = true;
	}
	return processed;
}

bool MachInGameScreen::addPromptTextConstructionInfo( MachActor* pActor, GuiString& prompt )
{
	bool processed = false;
	char buffer[20];
	MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

	// Display incomplete construction info
	if ( pActor->objectIsConstruction() and not pActor->asConstruction().isComplete() and pActor->race() == playerRace )
	{
		// Special case for incomplete mines, they still show number of BMUs in mine.
		// Add BMUs left in mine onto prompt text
		if ( pActor->objectType() == MachLog::MINE )
		{
			MachPhys::BuildingMaterialUnits bmusLeft = 0;

			if ( pActor->asMine().hasMineralSite() )
			{
				bmusLeft = _CONST_CAST( const MachLogMine&, pActor->asMine() ).mineralSite().amountOfOre();
			}

			// Add BMU info
//			GuiResourceString bmuText( IDS_BMUPOINTS, GuiString( itoa( bmusLeft, buffer, 10 ) ) );
            sprintf(buffer, "%d", bmusLeft);
			GuiResourceString bmuText( IDS_BMUPOINTS, GuiString( buffer ) );
			prompt += " " + bmuText.asString();
		}

		// Percentage complete info
//		GuiResourceString percentCompleteText( IDS_PERCENTAGECOMPLETE, GuiString( itoa( pActor->asConstruction().percentageComplete(), buffer, 10 ) ) );
		sprintf(buffer, "%d", pActor->asConstruction().percentageComplete());
		GuiResourceString percentCompleteText( IDS_PERCENTAGECOMPLETE, GuiString( buffer ) );
		prompt += ", " + percentCompleteText.asString();

		processed = true;
	}
	// Display HP + AP for fristd::endly constructions
	else if ( pActor->objectIsConstruction() and pActor->race() == playerRace )
	{
		// Get HP and Armour info
//		GuiResourceString hpText( IDS_HEALTHPOINTS, GuiString( itoa( pActor->hp(), buffer, 10 ) ) );
//		GuiResourceString apText( IDS_ARMOURPOINTS, GuiString( itoa( pActor->armour(), buffer, 10 ) ) );
        sprintf(buffer, "%d", pActor->hp());
        GuiResourceString hpText( IDS_HEALTHPOINTS, GuiString( buffer ) );
        sprintf(buffer, "%d", pActor->armour());
        GuiResourceString apText( IDS_ARMOURPOINTS, GuiString( buffer ) );

		prompt += "\n" + hpText.asString() + " " + apText.asString();

		// Add BMU's left in mine onto prompt text
		if ( pActor->objectType() == MachLog::MINE )
		{
			MachPhys::BuildingMaterialUnits bmusLeft = 0;

			if ( pActor->asMine().hasMineralSite() )
			{
				bmusLeft = _CONST_CAST( const MachLogMine&, pActor->asMine() ).mineralSite().amountOfOre();
			}

			// Add BMU info
//			GuiResourceString bmuText( IDS_BMUPOINTS, GuiString( itoa( bmusLeft, buffer, 10 ) ) );
//			GuiResourceString bmuMinedText( IDS_BMUPOINTSMINED, GuiString( itoa( pActor->asMine().ore(), buffer, 10 ) ) );
			sprintf(buffer, "%d", bmusLeft);
			GuiResourceString bmuText( IDS_BMUPOINTS, GuiString( buffer ) );
			sprintf(buffer, "%d", pActor->asMine().ore());
			GuiResourceString bmuMinedText( IDS_BMUPOINTSMINED, GuiString( buffer ) );
			prompt += " " + bmuText.asString();
			prompt += " " + bmuMinedText.asString();
		}
		// Add Research Rate info for Hardware Labs
		else if ( pActor->objectType() == MachLog::HARDWARE_LAB )
		{
			// Add Research Rate info
//			GuiResourceString rpText( IDS_RESEARCHPOINTS, GuiString( itoa( pActor->asHardwareLab().totalResearchRate(), buffer, 10 ) ) );
            sprintf(buffer, "%d", pActor->asHardwareLab().totalResearchRate());
			GuiResourceString rpText( IDS_RESEARCHPOINTS, GuiString( buffer ) );
			prompt += " " + rpText.asString();
		}

		// Add on a string describing the current operation
		if ( not pActor->isIdle() )
		{
			GuiResourceString opString( 9000/*IDS_xxx values for ops start at 9000*/ + pActor->strategy().currentOperationType() );
			prompt += " " + opString.asString();
		}

		processed = true;
	}

	return processed;
}

bool MachInGameScreen::addPromptTextArtefactInfo( MachActor* pActor, GuiString& prompt )
{
	bool processed = false;

	if ( pActor->objectIsArtefact() )
	{
	 	char buffer[20];

		// Get HP and Armour info
//		GuiResourceString hpText( IDS_HEALTHPOINTS, GuiString( itoa( pActor->hp(), buffer, 10 ) ) );
//		GuiResourceString apText( IDS_ARMOURPOINTS, GuiString( itoa( pActor->armour(), buffer, 10 ) ) );
        sprintf(buffer, "%d", pActor->hp());
        GuiResourceString hpText( IDS_HEALTHPOINTS, GuiString( buffer ) );
        sprintf(buffer, "%d", pActor->armour());
		GuiResourceString apText( IDS_ARMOURPOINTS, GuiString( buffer ) );
		prompt += " " + hpText.asString() + " " + apText.asString();

		processed = true;
	}

	return processed;
}

bool MachInGameScreen::addPromptTextDebrisInfo( MachActor* pActor, GuiString& prompt )
{
	bool processed = false;

	if ( pActor->objectIsDebris() )
	{
	 	char buffer[20];

		// Display BMU value for Debris
//		GuiResourceString bmuText( IDS_BMUPOINTS, GuiString( itoa( pActor->asDebris().quantity(), buffer, 10 ) ) );
        sprintf(buffer, "%d", pActor->asDebris().quantity());
		GuiResourceString bmuText( IDS_BMUPOINTS, GuiString( buffer ) );
		prompt += " " + bmuText.asString();

		processed = true;
	}

	return processed;
}

bool MachInGameScreen::addPromptTextOreHolographInfo( MachActor* pActor, GuiString& prompt )
{
	bool processed = false;

	if ( pActor->objectIsOreHolograph() )
	{
	 	char buffer[20];

		// BMU's in mineral site
// 		GuiResourceString bmuText( IDS_BMUPOINTS, GuiString( itoa( pActor->asOreHolograph().mineralSite().amountOfOre(), buffer, 10 ) ) );
 		sprintf(buffer, "%d", pActor->asOreHolograph().mineralSite().amountOfOre());
 		GuiResourceString bmuText( IDS_BMUPOINTS, GuiString( buffer ) );

		prompt += " " + bmuText.asString();

		processed = true;
	}

	return processed;
}

void MachInGameScreen::displayActorPromptText( MachActor* pActor )
{
	CB_DEPIMPL( MachActor*, pPromptTextActor_ );

	GuiResourceString::Id stringId = 0;
    MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();
	GuiString weaponName;

	if ( pActor->race() == playerRace and MachLogActorStringIdRestorer::isExceptionToRule( pActor, &stringId ) )
	{}
	else
	{
		stringId = MachLogActorStringIdRestorer::stringId( pActor->objectType(), pActor->subType() );

		// Add hwLevel onto stringId
	 	if ( pActor->objectIsMachine() )
		{
			MachLogMachine& machine = pActor->asMachine();
	 		stringId += machine.hwLevel();
		}
		else if ( pActor->objectIsConstruction() )
		{
			MachLogConstruction& construction = pActor->asConstruction();
	 		stringId += construction.level();
		}

		// Get weapon name, if any ( must belong to player to see weapon info ).
		if ( pActor->objectIsCanAttack() and pActor->race() == playerRace )
		{
			// If actor is POD then we must check that the ion cannon is active.
			if ( ( pActor->objectType() == MachLog::POD and pActor->asPod().hasIonCannon() ) or
				 pActor->objectType() != MachLog::POD	)
			{
				MachLogCanAttack& canAttack = pActor->asCanAttack();
				GuiResourceString weapon( MachLogActorStringIdRestorer::weaponStringId( canAttack.weaponCombo() ) );
				weaponName = weapon.asString();
			}
		}
	}

    //Load the resource string
	GuiResourceString name( stringId );

	GuiString prompt;

	if ( weaponName.length() == 0 )
	{
		GuiResourceString text( IDS_HIGHLIGHT_PROMPT, name.asString() );
	    prompt = text.asString();
	}
	else
	{
		GuiStrings strings;
        strings.reserve( 2 );
		strings.push_back( name.asString() );
		strings.push_back( weaponName );
		GuiResourceString text( IDS_HIGHLIGHT_WITH_WEAPON_PROMPT, strings );
	    prompt = text.asString();
	}

	// Add aditional prompt text info based on actor type
	if ( addPromptTextMachineInfo( pActor, prompt ) )
	{}
	else if ( addPromptTextConstructionInfo( pActor, prompt ) )
	{}
	else if ( addPromptTextArtefactInfo( pActor, prompt ) )
	{}
	else if ( addPromptTextDebrisInfo( pActor, prompt ) )
	{}
	else if ( addPromptTextOreHolographInfo( pActor, prompt ) )
	{}

	//Set the cursor prompt
    cursorPromptText( prompt, not (pPromptTextActor_ == pActor) );

	pPromptTextActor_ = pActor;
}

void MachInGameScreen::removeHighlightedActor()
{
	CB_DEPIMPL( MachActor*, pHighlightedActor_ );
	CB_DEPIMPL( MachGuiCorralSingleIcon*, pCorralSingleIcon_ );

	if ( pHighlightedActor_ )
	{
		pHighlightedActor_ =  NULL;
		pCorralSingleIcon_->clear();
	}
}

void MachInGameScreen::asynchronousUpdate()
{
	CB_DEPIMPL( MachPromptText*, pPromptText_ );
	CB_DEPIMPL( MachGuiControlPanelAddOn*, pControlPanelAddOn_ );
	CB_DEPIMPL(	MachWorldViewWindow*, pWorldViewWindow_ );
	CB_DEPIMPL( bool, cancelActiveCommand_ );
	CB_DEPIMPL( bool, commandBankNeedsUpdating_ );
	CB_DEPIMPL( Actors, selectedActors_ );
	CB_DEPIMPL( Commands, allCommands_ );
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );
	CB_DEPIMPL( MachProductionBank*, pProductionBank_ );
	CB_DEPIMPL( MachHWResearchBank*, pHWResearchBank_ );
	CB_DEPIMPL( MachMachinesIcon*, pMachinesIcon_ );
	CB_DEPIMPL( MachConstructionsIcon*, pConstructionsIcon_ );
	CB_DEPIMPL( MachGuiMachineNavigator*, pMachineNavigation_ );
	CB_DEPIMPL( MachGuiConstructionNavigator*, pConstructionNavigation_ );
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachGuiCommand*, pActiveCommand_ );
	CB_DEPIMPL( bool, controlPanelOn_ );
	CB_DEPIMPL( int, controlPanelXPos_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );
	CB_DEPIMPL( MachGuiMapArea*, pMapArea_ );
	CB_DEPIMPL( int, redrawMapCounter_ );
	CB_DEPIMPL( double, networkStuffedStartTime_ );

#ifndef PRODUCTION
	CB_DEPIMPL( bool, showCurrentMachine_ );
	CB_DEPIMPL( bool, showNetworkPing_ );
	CB_DEPIMPL( bool, showNetworkStuffed_ );
#endif

	if ( actualGameState() != PLAYING )
	{
		pSceneManager_->pDevice()->display()->useCursor( NULL ); // Shouldn't really use NULL but there's no other interface
		SimManager::instance().suspend();
	}

    //If we have been requested to cancel the active command, do so now
    if( cancelActiveCommand_ )
        activateDefaultCommand();

    //If we have a non-default active command, and its interaction is complete,
    //execute it and restore default command.
    if( pActiveCommand_ != NULL  and  pActiveCommand_->isInteractionComplete() )
    {
        //If not done so, execute it
        if( not pActiveCommand_->isExecuted() )
		{
			pActiveCommand_->apply();
			commandBankNeedsUpdating_ = true;
		}

        activateDefaultCommand();
    }

    //If required, update the command bank
    if( commandBankNeedsUpdating_ )
    {
		//Ensure don't do it again next time
        commandBankNeedsUpdating_ = false;

		//Run along the commands, setting the visibility flag.
        //However, if no selections, or first is foreign race, then no command should
        //be visible.
        MachLogRaces& races = MachLogRaces::instance();
        bool forceInvisible =
            selectedActors_.size() == 0  or
            not races.hasPCRace() or
            selectedActors_.front()->race() != races.pcController().race();

        for( Commands::iterator it = allCommands_.begin(); it != allCommands_.end(); ++it )
        {
            MachGuiCommand* pCommand = *it;
            pCommand->isVisible( forceInvisible ? false : pCommand->actorsCanExecute() );
        }

        //Update the command bank if displayed
        updateCommandIcons();

		// Display command text for single factory and research contexts
		if ( controlPanelContext_ == MachGui::SINGLE_FACTORY )
        {
			GuiResourceString promptString( IDS_BUILD_START );
			commandPromptText( promptString.asString() );
		}
		else if ( controlPanelContext_ == MachGui::HARDWARE_RESEARCH )
		{
			GuiResourceString promptString( IDS_RESEARCHLEVEL_START );
			commandPromptText( promptString.asString() );
		}
    }

    //If we have a production bank, update it
    if( pProductionBank_ != NULL )
        pProductionBank_->updateProgress();

    //If we have a hw research menu, update it
    if( pHWResearchBank_ != NULL )
        pHWResearchBank_->updateProgress();

    //Update some things only every 20 frames
    static size_t frameCounter = 19;
    frameCounter = (++frameCounter) % 20;
    switch( frameCounter )
	{
		case 0:
			MachLogVoiceMailManager::instance().update();
			break;
		case 3:
			pMachinesIcon_->refresh();
			break;
		case 7:
			pConstructionsIcon_->refresh();
			break;
		case 11:
			pMachineNavigation_->update();
			break;
		case 15:
			pConstructionNavigation_->update();
			break;
		case 19:
			updateGameState();
			break;
	}

	// Refresh the continent map. This is not done every frame. When it is redrawn is
	// controlled internally by the continent map.
	if ( redrawMapCounter_ != 0 )
	{
		pContinentMap_->forceUpdate();
		--redrawMapCounter_;
	}
	else
	{
		pContinentMap_->refresh();
	}

	// Make sure new gui sounds are played
	MachGuiSoundManager::instance().update();

	// Update time network has been stuffed for
	if ( MachLogNetwork::instance().isNetworkGame() )
	{
		if ( not NetNetwork::instance().imStuffed() )
		{
			networkStuffedStartTime_ = DevTime::instance().time();
		}
	}

	#ifndef PRODUCTION
	if( showNetworkStuffed_ )
	{
		NetNetwork::instance().outputCurrentStatistics( pSceneManager_->out() );
		if( NetNetwork::instance().imStuffed() )
			NetNetwork::instance().outputStuffedInfo( pSceneManager_->out() );
	}
	if( showCurrentMachine_ and selectedActors().size() > 0 )
		pSceneManager_->out() << *selectedActors().front() << std::endl;
	if( showNetworkPing_ and MachLogNetwork::instance().isNetworkGame() )
	{
		const NetNode& node = MachLogNetwork::instance().node();
/*		const NetNode::Ping& pingInfo = node.pingInformation();
		for( NetNode::Ping::const_iterator i = pingInfo.begin(); i != pingInfo.end(); ++i )
		{
			if( (*i).first != node.nodeUid() )
			{
				pSceneManager_->out() << "Current ping : " << (*i).first.nodeName() << " " << (*i).second << std::endl;
			}
		}*/
	}
	#endif
}

// virtual
void MachInGameScreen::update()
{
	CB_DEPIMPL( MachPromptText*, pPromptText_ );
	CB_DEPIMPL( MachGuiControlPanelAddOn*, pControlPanelAddOn_ );
	CB_DEPIMPL(	MachWorldViewWindow*, pWorldViewWindow_ );
	CB_DEPIMPL( bool, cancelActiveCommand_ );
	CB_DEPIMPL( bool, commandBankNeedsUpdating_ );
	CB_DEPIMPL( Actors, selectedActors_ );
	CB_DEPIMPL( Commands, allCommands_ );
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );
	CB_DEPIMPL( MachProductionBank*, pProductionBank_ );
	CB_DEPIMPL( MachHWResearchBank*, pHWResearchBank_ );
	CB_DEPIMPL( MachMachinesIcon*, pMachinesIcon_ );
	CB_DEPIMPL( MachConstructionsIcon*, pConstructionsIcon_ );
	CB_DEPIMPL( MachGuiMachineNavigator*, pMachineNavigation_ );
	CB_DEPIMPL( MachGuiConstructionNavigator*, pConstructionNavigation_ );
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachGuiCommand*, pActiveCommand_ );
	CB_DEPIMPL( bool, controlPanelOn_ );
	CB_DEPIMPL( int, controlPanelXPos_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );
	CB_DEPIMPL( MachGuiMapArea*, pMapArea_ );
	CB_DEPIMPL( int, redrawMapCounter_ );
	CB_DEPIMPL( double, networkStuffedStartTime_ );

#ifndef PRODUCTION
	CB_DEPIMPL( bool, showCurrentMachine_ );
	CB_DEPIMPL( bool, showNetworkPing_ );
	CB_DEPIMPL( bool, showNetworkStuffed_ );
#endif

	bool controlPanelMoved = false;
	// Scroll control panel...
	if ( ( controlPanelOn_ or displayControlPanel() ) and controlPanelXPos_ != MachGui::controlPanelOutXPos() )
	{
		// Trigger sound if control panel about to slide out
		if ( controlPanelXPos_ == MachGui::controlPanelInXPos() )
			MachGuiSoundManager::instance().playSound( "gui/sounds/ctrlpon.wav" );

		controlPanelXPos_ += MachGui::controlPanelSlideOutSpeed();
		controlPanelXPos_ = std::min( controlPanelXPos_, MachGui::controlPanelOutXPos() );

		controlPanelMoved = true;
	}
	else if ( not controlPanelOn_ and not displayControlPanel() and controlPanelXPos_ != MachGui::controlPanelInXPos() )
	{
		// Trigger sound if control panel about to slide in
		if ( controlPanelXPos_ == MachGui::controlPanelOutXPos() )
			MachGuiSoundManager::instance().playSound( "gui/sounds/ctrlpoff.wav" );

		controlPanelXPos_ -= MachGui::controlPanelSlideInSpeed();
		controlPanelXPos_ = std::max( controlPanelXPos_, MachGui::controlPanelInXPos() );

		controlPanelMoved = true;
	}

//	if ( controlPanelMoved ) //TODO: now panel is redrawn every frame
	{
		// Move control panel
		Gui::Coord newTopLeft = controlPanel().relativeCoord();
		newTopLeft.x( controlPanelXPos_ - MachGui::controlPanelOutXPos() );
		positionChildRelative( pControlPanel_, newTopLeft );
		pControlPanel_->changed();

		// Move prompt text
		Gui::Coord promptNewTopLeft = pPromptText_->relativeCoord();
		promptNewTopLeft.x( controlPanelXPos_ + MachGui::promptTextXOffset() );
		positionChildRelative( pPromptText_, promptNewTopLeft );

		// Move WorldViewWindow
		positionChildRelative( pWorldViewWindow_, Gui::Coord( controlPanelXPos_, 0 ) );
	}

	// Tell map area to update every frame
	pMapArea_->controlPanelSliding( controlPanelXPos_ != MachGui::controlPanelOutXPos() );
	pMapArea_->changed(); // Force redraw of map area

	// World view window is responsible for drawing rubber band when
	// selecting large numbers of actors.
	pWorldViewWindow_->update();

	// Update the visibility of any child GuiDisplayables
	updateChildVisible();

	// Any control panel sections that have been invalidated will be
	// redrawn now before child GuiDisplayables are draw.
	pControlPanel_->redrawAllAreas();
}

MachCameras* MachInGameScreen::cameras()
{
	CB_DEPIMPL( MachCameras*, pCameras_ );

	PRE( pCameras_ );

	return pCameras_;
}

//virtual
bool MachInGameScreen::beNotified
(
    W4dSubject* pSubject, W4dSubject::NotificationEvent event, int clientData
)
{
	CB_DEPIMPL( bool, commandBankNeedsUpdating_ );
	CB_DEPIMPL( MachGuiMachineNavigator*, pMachineNavigation_ );
	CB_DEPIMPL( MachGuiConstructionNavigator*, pConstructionNavigation_ );
	CB_DEPIMPL( MachGuiCorralSingleIcon*, pCorralSingleIcon_ );
	CB_DEPIMPL( MachGuiCorral*, pCorral_ );

    bool cancelSelection = false;

    //We want to deselect the actor if being deleted, or entering an APC
    switch( event )
    {
        case W4dSubject::DELETED:
            cancelSelection = true;
            break;

        case W4dSubject::CLIENT_SPECIFIC:
            if ( clientData == MachLog::TELEPORTED_OUT_OF_WORLD or
            	 clientData == MachLog::ENTERED_WORLD or
            	 clientData == MachLog::RACE_CHANGED or
            	 clientData == MachLog::CHANGED_CAMOUFLAGE_STATUS or
            	 clientData == MachLog::CHANGED_MINERALS_CARRIED )
			{
				// Need to grey out or show the icon in the corral when leaving/entering APC
				pCorralSingleIcon_->setActor( selectedActors().front(), true );
   				pCorral_->changed();
				commandBankNeedsUpdating_ = true;
			}
			else if ( clientData == MachLog::WEAPON_AVAILABILITY_CHANGED )
			{
				commandBankNeedsUpdating_ = true;
			}
            break;
    }

    //Cancel the selection if required
    if( cancelSelection )
    {
        //Since we only observe MachActors, this cast, although unfortunate,
        //should be safe.
        MachActor* pActor = _STATIC_CAST( MachActor*, pSubject );

		// Stop the navigator from being dismissed just because we are unselecting
		// a deleted actor
		if ( isMachineNavigationContext() )
			pMachineNavigation_->remainVisible( true );

		if ( isConstructionNavigationContext() )
			pConstructionNavigation_->remainVisible( true );

        //Do the work, but don't cancel the observer relation - let caller do that
        unselect( pActor );
    }

    return not cancelSelection;
}

bool MachInGameScreen::applyCommandToSquadron()
{
	CB_DEPIMPL( bool, applyCommandToSquadron_ );

	return applyCommandToSquadron_;
}

void MachInGameScreen::applyCommandToSquadron( bool apply )
{
	CB_DEPIMPL( bool, applyCommandToSquadron_ );

	applyCommandToSquadron_ = apply;
}

//virtual
void MachInGameScreen::domainDeleted( W4dDomain* )
{
    //Do nothing
}

//virtual
bool MachInGameScreen::doHandleRightClickEvent( const GuiMouseEvent& event )
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( Gui::Coord, rightClickMousePos_ );
	CB_DEPIMPL( MachGuiMachineNavigator*, pMachineNavigation_ );
	CB_DEPIMPL( MachGuiConstructionNavigator*, pConstructionNavigation_ );
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );

	static double pressReleaseTimer;

    if( event.rightButton() == Gui::PRESSED )
    {
		pressReleaseTimer = DevTime::instance().time();
		rightClickMousePos_	= event.coord();
    }
	else
	{
		bool shouldDeselectActors = false;

		// If we release the right mouse button in under 0.5sec and we're not
		// over the map then deselect all selected actors, reset the intelligent
		// cursors etc.
		if ( DevTime::instance().time() - pressReleaseTimer < 0.5 and
			 not pContinentMap_->absoluteBoundary().contains( event.coord() ) )
		{
			shouldDeselectActors = true;
		}
		// ...else if we release the right mouse button in under 0.1sec and we are
		// over the map then deselect all selected actors, reset the intelligent
		// cursors etc.
		else if ( 	DevTime::instance().time() - pressReleaseTimer < 0.1 and
			 		pContinentMap_->absoluteBoundary().contains( event.coord() ) )
		{
			shouldDeselectActors = true;
		}

		if ( shouldDeselectActors )
		{
			// First check if constructor menu is displayed. If so, dismiss this instead of
			// cleaning out corral
			if ( isConstructCommandContext() )
			{
				mainMenuContext();
				activateDefaultCommand();
			}
			else
			{
				// Navigators are not dismissed via a right click.
				pMachineNavigation_->remainVisible( true );
				pConstructionNavigation_->remainVisible( true );
				// Remove green selection from around any nav buttons.
				pMachineNavigation_->deselectAll();
				pConstructionNavigation_->deselectAll();

		        deselectAll();

		        //Revert to default intelligent command
		        activateDefaultCommand();
			}
		}
	}

    return true;
}

bool MachInGameScreen::isMainMenuContext() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );

    return controlPanelContext_ == MachGui::MAIN_MENU;
}

bool MachInGameScreen::isSquadronContext() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );

    return controlPanelContext_ == MachGui::SQUADRON_MENU;
}

bool MachInGameScreen::isConstructCommandContext() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );

    return controlPanelContext_ == MachGui::CONSTRUCT_COMMAND;
}

bool MachInGameScreen::isBuildCommandContext() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );

    return controlPanelContext_ == MachGui::BUILD_COMMAND;
}

bool MachInGameScreen::isHardwareResearchContext() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );

    return controlPanelContext_ == MachGui::HARDWARE_RESEARCH;
}

bool MachInGameScreen::isSoftwareResearchContext() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );

    return controlPanelContext_ == MachGui::SOFTWARE_RESEARCH;
}

bool MachInGameScreen::isSingleFactoryContext() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );

    return controlPanelContext_ == MachGui::SINGLE_FACTORY;
}

void MachInGameScreen::mainMenuContext()
{
    currentContext( MachGui::MAIN_MENU );
}

void MachInGameScreen::squadronContext()
{
    currentContext( MachGui::SQUADRON_MENU );
}

void MachInGameScreen::constructCommandContext()
{
    currentContext( MachGui::CONSTRUCT_COMMAND );
}

void MachInGameScreen::buildCommandContext()
{
    currentContext( MachGui::BUILD_COMMAND );
}

void MachInGameScreen::hardwareResearchContext()
{
    currentContext( MachGui::HARDWARE_RESEARCH );
}

void MachInGameScreen::softwareResearchContext()
{
    currentContext( MachGui::SOFTWARE_RESEARCH );
}

void MachInGameScreen::singleFactoryContext()
{
    currentContext( MachGui::SINGLE_FACTORY );
}


MachGui::ControlPanelContext MachInGameScreen::currentContext() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );

    return controlPanelContext_;
}

void MachInGameScreen::mainMenuOrSingleFactoryContext()
{
	// Check to see if we have one selected factory. If we do then we switch straight into
	// build menu.
	const MachInGameScreen::Actors& selectionSet = selectedActors();
    MachLogRaces& races = MachLogRaces::instance();
	if (selectionSet.size() == 1  and 	// Only consider when one actor selected
		races.hasPCRace() and // PC race must exist
    	selectionSet.front()->race() == races.pcController().race() ) // factory must belong to PC race
	{
		if ( selectionSet.front()->objectType() == MachLog::FACTORY ) // actor is factory
		{
			singleFactoryContext();
		}
		else if ( selectionSet.front()->objectType() == MachLog::HARDWARE_LAB )
		{
			hardwareResearchContext();
		}
		else
		{
			mainMenuContext();
		}
	}
	else
	{
		mainMenuContext();
	}
}

void MachInGameScreen::currentContext( MachGui::ControlPanelContext newContext, bool forceChange /*= false*/ )
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );
	CB_DEPIMPL( bool, commandBankNeedsUpdating_ );
	CB_DEPIMPL( MachMachinesIcon*, pMachinesIcon_ );
	CB_DEPIMPL( MachConstructionsIcon*, pConstructionsIcon_ );
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );
	CB_DEPIMPL( MachConstructMenu*, pConstructMenu_ );
	CB_DEPIMPL( MachBuildMenu*, pBuildMenu_ );
	CB_DEPIMPL( MachSmallCommandIcons*, pSmallCommandIcons_ );
	CB_DEPIMPL( MachProductionBank*, pProductionBank_ );
	CB_DEPIMPL( MachHWResearchBank*, pHWResearchBank_ );
	CB_DEPIMPL( MachHWResearchMenu*, pHWResearchMenu_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );

    //Check not already in it
    if( controlPanelContext_ != newContext or forceChange )
    {
        //Reset all other contexts
		MachGui::ControlPanelContext oldContext = controlPanelContext_;
        resetContext();
        controlPanelContext_ = newContext;
        commandBankNeedsUpdating_ = true;
        setupActorBank();

		// Make navigator buttons popup if we are switching to a different context
		if ( oldContext == MachGui::MACHINE_NAVIGATION_MENU )
			pMachinesIcon_->forceUp();
		else if ( oldContext == MachGui::CONSTRUCTION_NAVIGATION_MENU )
			pConstructionsIcon_->forceUp();

        //Do any context specific menu creations etc
        switch( newContext )
        {
			case MachGui::CONSTRUCT_COMMAND:
            {
                //Construct the construction selection menu
                pConstructMenu_ = _NEW( MachConstructMenu( 	pControlPanel_,
                											Gui::Coord( 1,0 ),
                                                           	this ) );
                pConstructMenu_->initialise();
			    break;
            }

            case MachGui::SINGLE_FACTORY:
            {
                //Construct the build menu
                Gui::Coord sciCoord = pSmallCommandIcons_->relativeBoundary( *pControlPanel_ ).maxCorner();

				size_t yPos = sciCoord.y() + 2;

                pBuildMenu_ = _NEW( MachBuildMenu( 	pControlPanel_,
													pProductionBank_,
                    								Gui::Coord( 1, yPos ),
                                                   	this ) );
				pBuildMenu_->initialise();

				GuiResourceString promptString( IDS_BUILD_START );
				commandPromptText( promptString.asString() );
				clearCursorPromptText();

			    break;
            }

            case MachGui::HARDWARE_RESEARCH:
            {
                //Construct the hw research menu
                Gui::Coord sciCoord = pSmallCommandIcons_->relativeBoundary( *pControlPanel_ ).maxCorner();

				size_t yPos = sciCoord.y() + 2;

                pHWResearchMenu_ = _NEW( MachHWResearchMenu(pControlPanel_,
                											pHWResearchBank_,
                                   							Gui::Coord( 1, yPos ),
                                                        	this ) );
				pHWResearchMenu_->initialise();

				GuiResourceString promptString( IDS_RESEARCHLEVEL_START );
				commandPromptText( promptString.asString() );
				clearCursorPromptText();

				break;
            }
        }
    }
}

void MachInGameScreen::resetContext()
{
	CB_DEPIMPL( MachConstructMenu*, pConstructMenu_ );
	CB_DEPIMPL( MachBuildMenu*, pBuildMenu_ );
	CB_DEPIMPL( MachHWResearchMenu*, pHWResearchMenu_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );

	DEBUG_STREAM( DIAG_NEIL, "MachInGameScreen::resetContext" );

    //Delete temporary menus
    if( pConstructMenu_ != NULL )
    {
		pControlPanel_->redrawArea( *pConstructMenu_ );
        _DELETE( pConstructMenu_ );
        pConstructMenu_ = NULL;
    }

    if( pBuildMenu_ != NULL )
    {
		pControlPanel_->redrawArea( *pBuildMenu_ );
         _DELETE( pBuildMenu_ );
        pBuildMenu_ = NULL;
    }

    if( pHWResearchMenu_ != NULL )
    {
		pControlPanel_->redrawArea( *pHWResearchMenu_ );
        _DELETE( pHWResearchMenu_ );
        pHWResearchMenu_ = NULL;
    }
}

void MachInGameScreen::updateCommandIcons()
{
	CB_DEPIMPL( MachCommandIcons*, pCommandIcons_ );
	CB_DEPIMPL( MachSmallCommandIcons*, pSmallCommandIcons_ );
	CB_DEPIMPL( Actors, selectedActors_ );

	if( pCommandIcons_ != NULL )
		pCommandIcons_->change();

	if ( pSmallCommandIcons_ != NULL )
		pSmallCommandIcons_->change();

	// Update special command icons
	defconCommand()->update( selectedActors_ );
	MachGuiSelfDestructCommand::update( selectedActors_ );
	MachGuiIonAttackCommand::update( selectedActors_ );
	MachGuiNukeAttackCommand::update( selectedActors_ );
}

bool MachInGameScreen::isCorralVisible() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );
	CB_DEPIMPL( Actors, selectedActors_ );
	CB_DEPIMPL( int, controlPanelXPos_ );

    bool visible = controlPanelContext_ == MachGui::MAIN_MENU  or
		           controlPanelContext_ == MachGui::SQUADRON_MENU  or
		           controlPanelContext_ == MachGui::FORM_SQUADRON_COMMAND  or
		           controlPanelContext_ == MachGui::MACHINE_NAVIGATION_MENU or
   		           controlPanelContext_ == MachGui::CONSTRUCTION_NAVIGATION_MENU;

	// Only visible if there is more than one actor selected
	visible &= ( selectedActors_.size() > 1 );

	// Only visible if control panel is not in the process of sliding in/out
	visible &= controlPanelXPos_ == MachGui::controlPanelOutXPos();

	return visible;
}

bool MachInGameScreen::isCorralSingleIconVisible() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );
	CB_DEPIMPL( Actors, selectedActors_ );
	CB_DEPIMPL( MachActor*, pHighlightedActor_ );
	CB_DEPIMPL( int, controlPanelXPos_ );

    bool visible = controlPanelContext_ == MachGui::MAIN_MENU  or
		           controlPanelContext_ == MachGui::SQUADRON_MENU  or
		           controlPanelContext_ == MachGui::FORM_SQUADRON_COMMAND  or
		           controlPanelContext_ == MachGui::MACHINE_NAVIGATION_MENU or
		           controlPanelContext_ == MachGui::CONSTRUCTION_NAVIGATION_MENU or
		           controlPanelContext_ == MachGui::SINGLE_FACTORY or
		           controlPanelContext_ == MachGui::HARDWARE_RESEARCH;

	visible &= ( selectedActors_.size() == 1 or ( pHighlightedActor_ and selectedActors_.empty() ) );

	// Only visible if control panel is not in the process of sliding in/out
	visible &= controlPanelXPos_ == MachGui::controlPanelOutXPos();

	return visible;
}

bool MachInGameScreen::isCommandIconsVisible() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );
	CB_DEPIMPL( int, controlPanelXPos_ );

	bool visible = controlPanelContext_ == MachGui::MAIN_MENU;

	// Only visible if control panel is not in the process of sliding in/out
	visible &= controlPanelXPos_ == MachGui::controlPanelOutXPos();

    return visible;
}

bool MachInGameScreen::isSmallCommandIconsVisible() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );
	CB_DEPIMPL( int, controlPanelXPos_ );

	bool visible = ( controlPanelContext_ == MachGui::SINGLE_FACTORY or
    		  		 controlPanelContext_ == MachGui::HARDWARE_RESEARCH );

	// Only visible if control panel is not in the process of sliding in/out
	visible &= controlPanelXPos_ == MachGui::controlPanelOutXPos();

    return visible;
}

bool MachInGameScreen::isMachineNavigationVisible() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );
	CB_DEPIMPL( int, controlPanelXPos_ );

	bool visible = controlPanelContext_ == MachGui::MACHINE_NAVIGATION_MENU;

	// Only visible if control panel is not in the process of sliding in/out
	visible &= controlPanelXPos_ == MachGui::controlPanelOutXPos();

    return visible;
}

bool MachInGameScreen::isConstructionNavigationVisible() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );
	CB_DEPIMPL( int, controlPanelXPos_ );

	bool visible = controlPanelContext_ == MachGui::CONSTRUCTION_NAVIGATION_MENU;

	// Only visible if control panel is not in the process of sliding in/out
	visible &= controlPanelXPos_ == MachGui::controlPanelOutXPos();

    return visible;
}

bool MachInGameScreen::isSquadronBankVisible() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );
	CB_DEPIMPL( int, controlPanelXPos_ );

	bool visible = controlPanelContext_ == MachGui::FORM_SQUADRON_COMMAND  or
		           controlPanelContext_ == MachGui::SQUADRON_MENU;

	// Only visible if control panel is not in the process of sliding in/out
	visible &= controlPanelXPos_ == MachGui::controlPanelOutXPos();

    return visible;
}

//virtual
void MachInGameScreen::doDisplay()
{}

void MachInGameScreen::cancelActiveCommand()
{
	CB_DEPIMPL( bool, cancelActiveCommand_ );
    cancelActiveCommand_ = true;
}

void MachInGameScreen::cursorPromptText( const string& prompt )
{
	CB_DEPIMPL( MachPromptText*, pPromptText_ );

    PRE( pPromptText_ != NULL );

    pPromptText_->cursorPromptText( prompt );
}

void MachInGameScreen::cursorPromptText( const string& prompt, bool restartScroll )
{
	CB_DEPIMPL( MachPromptText*, pPromptText_ );

    PRE( pPromptText_ != NULL );
    pPromptText_->cursorPromptText( prompt, restartScroll );
}

const string& MachInGameScreen::cursorPromptText() const
{
	CB_DEPIMPL( MachPromptText*, pPromptText_ );

    PRE( pPromptText_ != NULL );
    return pPromptText_->cursorPromptText();
}

void MachInGameScreen::clearCursorPromptText()
{
	CB_DEPIMPL( MachPromptText*, pPromptText_ );

    PRE( pPromptText_ != NULL );
    pPromptText_->clearCursorPromptText();
}

void MachInGameScreen::commandPromptText( const string& prompt )
{
	CB_DEPIMPL( MachPromptText*, pPromptText_ );

    PRE( pPromptText_ != NULL );
    pPromptText_->commandPromptText( prompt );
}

const string& MachInGameScreen::commandPromptText() const
{
	CB_DEPIMPL( MachPromptText*, pPromptText_ );

    PRE( pPromptText_ != NULL );
    return pPromptText_->commandPromptText();
}

void MachInGameScreen::clearCommandPromptText()
{
	CB_DEPIMPL( MachPromptText*, pPromptText_ );

    PRE( pPromptText_ != NULL );
    pPromptText_->clearCommandPromptText();
}

MachPromptText& MachInGameScreen::promptTextWindow()
{
	CB_DEPIMPL( MachPromptText*, pPromptText_ );

    PRE( pPromptText_ != NULL );
    return *pPromptText_;
}

void MachInGameScreen::machineNavigationContext()
{
	currentContext( MachGui::MACHINE_NAVIGATION_MENU );
}

bool MachInGameScreen::isMachineNavigationContext() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );

    return controlPanelContext_ == MachGui::MACHINE_NAVIGATION_MENU;
}

void MachInGameScreen::constructionNavigationContext()
{
    currentContext( MachGui::CONSTRUCTION_NAVIGATION_MENU );
}

bool MachInGameScreen::isConstructionNavigationContext() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );

    return controlPanelContext_ == MachGui::CONSTRUCTION_NAVIGATION_MENU;
}

void MachInGameScreen::cursor2d( MachGui::Cursor2dType type )
{
	CB_DEPIMPL( MachInGameCursors2d*, pCursors2d_ );

    PRE( pCursors2d_ != NULL );

	if ( actualGameState() == PLAYING )
	{
		pCursors2d_->cursorSize( MachInGameCursors2d::LARGECURSORS );
	    pCursors2d_->cursor( type );
	}
}

void MachInGameScreen::cursor2d( MachGui::Cursor2dType type, MachInGameCursors2d::CursorSize curSize )
{
	CB_DEPIMPL( MachInGameCursors2d*, pCursors2d_ );

    PRE( pCursors2d_ != NULL );

	if ( actualGameState() == PLAYING )
	{
		pCursors2d_->cursorSize( curSize );
    	pCursors2d_->cursor( type );
	}
}

void MachInGameScreen::setupActorBank()
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );
	CB_DEPIMPL( Actors, selectedActors_ );
	CB_DEPIMPL( MachProductionBank*, pProductionBank_ );
 	CB_DEPIMPL( MachHWResearchBank*, pHWResearchBank_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );

    //Get the fristd::endly race id
    MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

    //check for a single selected actor, and get its pointer
    MachActor* pSelectedActor = ( selectedActors_.size() == 1 ? selectedActors_.front() : NULL );
    MachLogFactory* pFactory = NULL;
    MachLogHardwareLab* pHardwareLab = NULL;

    //Use flags to indicate whether a given bank should be displayed
    bool showProductionBank = false;
    bool showHWResearchBank = false;

    if( pSelectedActor != NULL )
    {
        //See if the factory production bank is appropriate.
        //Must have a fristd::endly, complete factory, and be in the main or build command menus
        if( pSelectedActor->objectType() == MachLog::FACTORY )
            pFactory = &pSelectedActor->asFactory();

        showProductionBank = pFactory != NULL                                  and
                             (controlPanelContext_ == MachGui::MAIN_MENU or
                              controlPanelContext_ == MachGui::SINGLE_FACTORY)  and
                             pFactory->race() == playerRace;

        //See if the hw research bank is appropriate.
        //Must have a fristd::endly, complete hw lab, and be in the main or hw research menus
        if( pSelectedActor->objectType() == MachLog::HARDWARE_LAB )
            pHardwareLab = &pSelectedActor->asHardwareLab();

        showHWResearchBank = pHardwareLab != NULL                                  and
                             (controlPanelContext_ == MachGui::MAIN_MENU or
                              controlPanelContext_ == MachGui::HARDWARE_RESEARCH)  and
                             pHardwareLab->race() == playerRace;
    }

    //Delete any unwanted banks, also watching for banks for an old actor
    if( pProductionBank_ != NULL  and
        ( (not showProductionBank) or ( pFactory != &pProductionBank_->factory() ) ) )
    {
		pControlPanel_->redrawArea( *pProductionBank_ );
        _DELETE( pProductionBank_ );
        pProductionBank_ = NULL;

		DEBUG_STREAM( DIAG_NEIL, "Deleting production bank"  << std::endl << std::flush );
    }

    if( pHWResearchBank_ != NULL  and
        ( (not showHWResearchBank) or ( pHardwareLab != &pHWResearchBank_->hardwareLab() ) ) )
    {
		pControlPanel_->redrawArea( *pHWResearchBank_ );
		_DELETE( pHWResearchBank_ );
        pHWResearchBank_ = NULL;

		DEBUG_STREAM( DIAG_NEIL, "Deleting research bank"  << std::endl << std::flush );
    }

    //Construct any required banks
    if( showProductionBank and pProductionBank_ == NULL )
    {
        Gui::Boundary bankArea( 1, pControlPanel_->getVisibleHeight() - MachProductionBank::height(), pControlPanel_->width(), pControlPanel_->getVisibleHeight() );
        pProductionBank_ = _NEW( MachProductionBank( pControlPanel_, bankArea, pFactory, this ) );

		DEBUG_STREAM( DIAG_NEIL, "Creating production bank" << std::endl << std::flush );
    }
    else if( showHWResearchBank and  pHWResearchBank_ == NULL )
    {
        Gui::Boundary bankArea( 1, pControlPanel_->getVisibleHeight() - MachHWResearchBank::reqHeight(), pControlPanel_->width(), pControlPanel_->getVisibleHeight() );
        pHWResearchBank_ = _NEW( MachHWResearchBank( pControlPanel_, bankArea, pHardwareLab, this ) );

		DEBUG_STREAM( DIAG_NEIL, "Creating research bank" << std::endl << std::flush );
    }
}

void MachInGameScreen::updateCameras()
{
	CB_DEPIMPL( MachCameras*, pCameras_ );
	pCameras_->updateCameras();
}


void MachInGameScreen::updateChildVisible()
{
	CB_DEPIMPL( MachGuiMachineNavigator*, pMachineNavigation_ );
	CB_DEPIMPL( MachGuiConstructionNavigator*, pConstructionNavigation_ );
	CB_DEPIMPL( MachGuiCorralSingleIcon*, pCorralSingleIcon_ );
	CB_DEPIMPL( MachGuiCorral*, pCorral_ );
	CB_DEPIMPL( MachGuiSquadronBank*, pSquadronBank_ );
	CB_DEPIMPL( MachCommandIcons*, pCommandIcons_ );
	CB_DEPIMPL( MachSmallCommandIcons*, pSmallCommandIcons_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );
	CB_DEPIMPL( MachProductionBank*, pProductionBank_ );
	CB_DEPIMPL( MachHWResearchBank*, pHWResearchBank_ );
	CB_DEPIMPL( MachConstructMenu*, pConstructMenu_ );
	CB_DEPIMPL( MachBuildMenu*, pBuildMenu_ );
	CB_DEPIMPL( MachHWResearchMenu*, pHWResearchMenu_ );
	CB_DEPIMPL( int, controlPanelXPos_ );

	// Make navigator visible/invisible
	if ( not isMachineNavigationVisible() and pMachineNavigation_->isVisible() )
	{
		pMachineNavigation_->makeVisible( false );
		pControlPanel_->redrawArea( *pMachineNavigation_ );
	}
	else if ( isMachineNavigationVisible() and not pMachineNavigation_->isVisible() )
	{
		pMachineNavigation_->makeVisible( true );
	}

	// Make construction navigation visible/invisble
	if ( not isConstructionNavigationVisible() and pConstructionNavigation_->isVisible() )
	{
		pConstructionNavigation_->makeVisible( false );
	    pControlPanel_->redrawArea( *pConstructionNavigation_ );
	}
	else if ( isConstructionNavigationVisible() and not pConstructionNavigation_->isVisible() )
	{
		pConstructionNavigation_->makeVisible( true );
	}

	// Make single icon corral visible/invisible
	if ( not isCorralSingleIconVisible() and pCorralSingleIcon_->isVisible() )
	{
		pCorralSingleIcon_->isVisible( false );
		pControlPanel_->redrawArea( *pCorralSingleIcon_ );
	}
	else if ( isCorralSingleIconVisible() and not pCorralSingleIcon_->isVisible() )
	{
		pCorralSingleIcon_->isVisible( true );
	}

	// Make multi-icon corral visible/invisible
	if ( not isCorralVisible() and pCorral_->isVisible() )
	{
		pCorral_->isVisible( false );
		pControlPanel_->redrawArea( *pCorral_ );
	}
	else if ( isCorralVisible() and not pCorral_->isVisible() )
	{
		pCorral_->isVisible( true );
	}

	// Make squadron bank visible/invisible
	if ( not isSquadronBankVisible() and pSquadronBank_->isVisible() )
	{
		pSquadronBank_->isVisible( false );
		pControlPanel_->redrawArea( *pSquadronBank_ );
	}
	else if ( isSquadronBankVisible() and not pSquadronBank_->isVisible() )
	{
		pSquadronBank_->isVisible( true );
	}

	// Make command icons visible/invisible
	if ( not isCommandIconsVisible() and pCommandIcons_->isVisible() )
	{
		pCommandIcons_->isVisible( false );
		pControlPanel_->redrawArea( *pCommandIcons_ );
	}
	else if ( isCommandIconsVisible() and not pCommandIcons_->isVisible() )
	{
		pCommandIcons_->isVisible( true );
	}

	// Make small command icons visible/invisible
	if ( not isSmallCommandIconsVisible() and pSmallCommandIcons_->isVisible() )
	{
		pSmallCommandIcons_->isVisible( false );
		pControlPanel_->redrawArea( *pSmallCommandIcons_ );
	}
	else if ( isSmallCommandIconsVisible() and not pSmallCommandIcons_->isVisible() )
	{
		pSmallCommandIcons_->isVisible( true );
	}

	// Make various bits and bobs visible/invisible
	// Only visible if control panel is not in the process of sliding in/out
	bool visible = controlPanelXPos_ == MachGui::controlPanelOutXPos();

	// Make production bank visible/invisible
	if ( pProductionBank_ )
	{
		if ( visible != pProductionBank_->isVisible() )
		{
			pProductionBank_->isVisible( visible );
		}
	}
	// Make hardware research bank visible/invisible
	if ( pHWResearchBank_ )
	{
		if ( visible != pHWResearchBank_->isVisible() )
		{
			pHWResearchBank_->isVisible( visible );
		}
	}
	// Make construction menu visible/invisible
	if ( pConstructMenu_ )
	{
		if ( visible != pConstructMenu_->isVisible() )
		{
			pConstructMenu_->isVisible( visible );
		}
	}
	// Make build menu visible/invisible
	if ( pBuildMenu_ )
	{
		if ( visible != pBuildMenu_->isVisible() )
		{
			pBuildMenu_->isVisible( visible );
		}
	}
	// Make research menu visible/invisible
	if ( pHWResearchMenu_ )
	{
		if ( visible != pHWResearchMenu_->isVisible() )
		{
			pHWResearchMenu_->isVisible( visible );
		}
	}
}

//virtual
void MachInGameScreen::doHandleContainsMouseEvent( const GuiMouseEvent& /*event*/ )
{}

void MachInGameScreen::setGuiViewport()
{
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );

    //Set the viewport boundary to the entire screen.
    RenDevice& device = *pSceneManager_->pDevice();
    const int w = device.windowWidth();
    const int h = device.windowHeight();
	device.setViewport(0, 0, w, h);
}

void MachInGameScreen::setWorldViewViewport()
{
	CB_DEPIMPL( bool, inFirstPerson_ );
	CB_DEPIMPL( MachGuiFirstPerson*, pFirstPerson_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );

	if ( inFirstPerson_ )
	{
		pFirstPerson_->setFirstPerson3DViewport();
    }
    else
    {
	    //Set the viewport boundary for the world view window
		Gui::Coord topLeftCoord = controlPanel().relativeCoord();

	    RenDevice& device = *pSceneManager_->pDevice();
	    const int w = device.windowWidth();
	    const int h = device.windowHeight();
		device.setViewport( pControlPanel_->width() + topLeftCoord.x(), 0, w - ( pControlPanel_->width() + topLeftCoord.x() ), h );
	}
}

bool MachInGameScreen::rubberBandSelectionHappening() const
{
	CB_DEPIMPL( MachWorldViewWindow*, pWorldViewWindow_ );

	return pWorldViewWindow_->rubberBandSelectionHappening();
}


void MachInGameScreen::loadGame( const string& planet )
{
	CB_DEPIMPL( MachCameras*, pCameras_ );
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );
	CB_DEPIMPL( MachWorldViewWindow*, pWorldViewWindow_ );
	CB_DEPIMPL( MachGuiSquadronBank*, pSquadronBank_ );
	CB_DEPIMPL( MachGuiMachineNavigator*, pMachineNavigation_ );
	CB_DEPIMPL( MachGuiConstructionNavigator*, pConstructionNavigation_ );
	CB_DEPIMPL(	MachGuiBmuButton*, pBmuButton_ );
	CB_DEPIMPL( MachInGameScreen::GameState, gameState_ );
	CB_DEPIMPL( MachMachinesIcon*, pMachinesIcon_ );
	CB_DEPIMPL( MachConstructionsIcon*, pConstructionsIcon_ );
	CB_DEPIMPL( MachSquadronIcon*, pSquadronIcon_ );
	CB_DEPIMPL( MachGuiInGameChatMessagesDisplay*, pChatMessageDisplay_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );

	// Don't allow any of the button setting below to trigger their sounds
	MachGuiSoundManager::instance().delaySounds();

    pCameras_->loadGame();
    pContinentMap_->loadGame( planet );
    pWorldViewWindow_->loadGame();
	pSquadronBank_->loadGame();
	pMachineNavigation_->loadGame();
	pConstructionNavigation_->loadGame();
	pMachinesIcon_->loadGame();
	pConstructionsIcon_->loadGame();
	pSquadronIcon_->loadGame();
	pControlPanel_->setupDecalBitmaps();

    pBmuButton_->setDepressed( true ); // button should start depressed.

	// Switch to main menu context.
	mainMenuContext();

	if ( MachLogNetwork::instance().isNetworkGame() )
	{
		const int chatMessagesX = 202;
		const int chatMessagesY = 0;

		pChatMessageDisplay_ = _NEW( MachGuiInGameChatMessagesDisplay( 	this,
																		Gui::Box( Gui::Coord( chatMessagesX, chatMessagesY ),
																				   MachGuiInGameChatMessages::reqWidth(),
																				   MachGuiInGameChatMessages::reqHeight() ),
																		&worldViewWindow() ) );
	}

	gameState_ = PLAYING;
}

void MachInGameScreen::unloadGame()
{
	CB_DEPIMPL( MachCameras*, pCameras_ );
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );
	CB_DEPIMPL( MachWorldViewWindow*, pWorldViewWindow_ );
	CB_DEPIMPL( MachGuiSquadronBank*, pSquadronBank_ );
	CB_DEPIMPL( MachGuiMachineNavigator*, pMachineNavigation_ );
	CB_DEPIMPL( MachGuiConstructionNavigator*, pConstructionNavigation_ );
	CB_DEPIMPL( bool, inFirstPerson_ );
	CB_DEPIMPL( MachGuiInGameChatMessagesDisplay*, pChatMessageDisplay_ );

	// Unselect any actors
	deselectAll();

    //Clear any current command. Otherwise we can get crashes -eg the construct command has
    //a physical building in the world which it deletes on destruction, so we must delete it
    //in advance of deleting the world to avoid double delete.
    activateDefaultCommand();

    pWorldViewWindow_->unloadGame();
    pCameras_->unloadGame();
    pContinentMap_->unloadGame();
	pSquadronBank_->unloadGame();
	pMachineNavigation_->unloadGame();
	pConstructionNavigation_->unloadGame();
	inFirstPerson_ = false;

	// Clean up chat message area
	_DELETE( pChatMessageDisplay_ );
	pChatMessageDisplay_ = NULL;
}

void MachInGameScreen::updateGameState()
{
	CB_DEPIMPL( MachInGameScreen::GameState, gameState_ );
	CB_DEPIMPL( PhysAbsoluteTime, gameStateTimer_ );

	if ( gameState_ == PLAYING ) // Only interested in switching the state if the game has not yet been won/lost
	{
	    MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();
		if( MachLogRaces::instance().hasLost( playerRace ) )
		{
            // Keep host working on until ctrl is pressed TODO: another elegant solution?
            if(MachLogNetwork::instance().isNetworkGame() and
                MachLogNetwork::instance().isNodeLogicalHost() and
                not DevKeyboard::instance().ctrlPressed())
                return;
			gameState_ = LOST;
			gameStateTimer_ = Phys::time();
            W4dSoundManager::instance().stopAll();
		}
		else if( MachLogRaces::instance().hasWon( playerRace ) )
		{
			gameState_ = WON;
			gameStateTimer_ = Phys::time();
            W4dSoundManager::instance().stopAll();
		}
	}
}

MachInGameScreen::GameState MachInGameScreen::gameState() const
{
	CB_DEPIMPL( MachInGameScreen::GameState, gameState_ );
	CB_DEPIMPL( PhysAbsoluteTime, gameStateTimer_ );

	GameState retVal = PLAYING;

	if ( gameState_ != PLAYING )
	{
		PhysAbsoluteTime now = Phys::time();
		// Only return that game has been lost/won after it has been in this state for a few seconds.
		// This enables pod destruction animation time to run and music to play.
		if ( MachGuiDatabase::instance().currentScenario().isTrainingScenario() and now - gameStateTimer_ > 5.0 )
		{
			retVal = gameState_;
		}
		else if ( gameState_ == WON and now - gameStateTimer_ > 19.0 )
		{
			retVal = gameState_;
		}
		else if ( gameState_ == LOST and now - gameStateTimer_ > 14.0 )
		{
			retVal = gameState_;
		}
	}

	return retVal;
}

void MachInGameScreen::activate()
{
    // De-pImpl_ variables used within this function.
	CB_DEPIMPL( bool, inFirstPerson_ );
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );
	CB_DEPIMPL( MachGuiFirstPerson*, pFirstPerson_ );

	if ( not inFirstPerson_ )
	{
		pContinentMap_->forceUpdate();
		changed();
	}
	else
	{
		pFirstPerson_->activate();
	}
}

void MachInGameScreen::fogOfWarOn( bool fog )
{
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );

	PRE( pContinentMap_ );

	pContinentMap_->fogOfWarOn( fog );
}

bool MachInGameScreen::switchToMenus() const
{
	CB_DEPIMPL( bool, switchToMenus_ );
	CB_DEPIMPL( MachGuiFirstPerson*, pFirstPerson_ );

	return switchToMenus_ or pFirstPerson_->switchToMenus();
}

void MachInGameScreen::resetSwitchToMenus()
{
	CB_DEPIMPL( bool, switchToMenus_ );
	CB_DEPIMPL( MachGuiFirstPerson*, pFirstPerson_ );

	switchToMenus_ = false;
	pFirstPerson_->resetSwitchToMenus();
}

void MachInGameScreen::switchToInHead()
{
	CB_DEPIMPL( bool, switchGuiRoot_ );

	switchGuiRoot_ = true;
}

void MachInGameScreen::checkSwitchGuiRoot()
{
	CB_DEPIMPL( bool, inFirstPerson_ );
	CB_DEPIMPL( MachGuiFirstPerson*, pFirstPerson_ );
	CB_DEPIMPL( bool, switchGuiRoot_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachWorldViewWindow*, pWorldViewWindow_ );

	if ( inFirstPerson_ )
	{
		// Switch to ingame gui?
		if ( pFirstPerson_->switchToInGame() )
		{
			inFirstPerson_ = false;
			pFirstPerson_->resetSwitchToInGame();

			// Make inGameScreen Gui root.
			becomeRoot();

			// Give inGameScreen keyboard focus.
			GuiManager::instance().keyboardFocus( this );

			// Redraw all gui elements.
			activate();
		}
	}
	else
	{
		// Switch to first person?
		if ( switchGuiRoot_ )
		{
			// Reset request flag.
		 	switchGuiRoot_ = false;

			// Is the selected actor a valid 1st person actor?
			if ( pFirstPerson_->okayToSwitchTo1stPerson() )
			{
				// Switch Gui root
				pFirstPerson_->becomeRoot();

				// Give 1st person the keyboard focus.
				GuiManager::instance().keyboardFocus( pFirstPerson_ );

				// Remove the menu cursor.
				pSceneManager_->pDevice()->display()->useCursor( NULL );

				// Remove blue selection box from highlighted actor before switching to 1st person
				if ( pWorldViewWindow_->haveHighlightedActor() )
				{
					if( MachLogRaces::instance().actorExists( pWorldViewWindow_->highlightedActorId() ) )
					{
						MachActor& highlightedActor = MachLogRaces::instance().actor( pWorldViewWindow_->highlightedActorId() );

						if ( highlightedActor.selectionState() == MachLog::HIGHLIGHTED )
						{
						    highlightedActor.selectionState( MachLog::NOT_SELECTED );
						}
					}
				}

				inFirstPerson_ = true;
			}
		}
	}
}

void MachInGameScreen::switchBackToInGame()
{
	CB_DEPIMPL( bool, inFirstPerson_ );
	CB_DEPIMPL( MachGuiFirstPerson*, pFirstPerson_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );

	// Were we in first person? Is it still valid to switch to first person?
	// In a multiplayer game the game continues whilst the user is in the menu
	// screens therefore it is possible that the actor that was embodied has been
	// destroyed in the mean time.
	if ( inFirstPerson_ and
		 pFirstPerson_->okayToSwitchTo1stPerson() )
	{
		// Switch Gui root.
		pFirstPerson_->becomeRoot();

		// Give 1st person the keyboard focus.
		GuiManager::instance().keyboardFocus( pFirstPerson_ );

		// Remove the menu cursor.
		pSceneManager_->pDevice()->display()->useCursor( NULL );
	}
	else
	{
		inFirstPerson_ = false;

		// Make inGameScreen Gui root.
		becomeRoot();

		// Give inGameScreen keyboard focus.
		GuiManager::instance().keyboardFocus( this );

		// Redraw all gui elements.
		activate();
	}
}

bool MachInGameScreen::inFirstPerson() const
{
    // De-pImpl_ variables used within this function.
	CB_DEPIMPL( bool, inFirstPerson_ );

	return inFirstPerson_;
}

const Gui::Coord& MachInGameScreen::rightClickMousePos() const
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( Gui::Coord, rightClickMousePos_ );

	return rightClickMousePos_;
}

void MachInGameScreen::updateWhilstInFirstPerson()
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );
	CB_DEPIMPL( double, networkStuffedStartTime_ );

	if ( actualGameState() != PLAYING )
	{
		SimManager::instance().suspend();
	}

	pContinentMap_->refresh();

    //Update some things only every 20 frames
    static size_t frameCounter = 19;
    frameCounter = (++frameCounter) % 20;
    if( frameCounter == 0 )
	{
		MachLogVoiceMailManager::instance().update();
		updateGameState();
	}

	// Update time network has been stuffed for
	if ( MachLogNetwork::instance().isNetworkGame() )
	{
		if ( not NetNetwork::instance().imStuffed() )
		{
			networkStuffedStartTime_ = DevTime::instance().time();
		}
	}
}

void MachInGameScreen::switchToMenus( bool switchTo )
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( bool, switchToMenus_ );

	switchToMenus_ = switchTo;
}

void MachInGameScreen::saveGame( PerOstream& outStream )
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );
	CB_DEPIMPL( MachCameras*, pCameras_ );

    pCameras_->saveGame( outStream );
    pContinentMap_->saveGame( outStream );
}

void MachInGameScreen::loadSavedGame( const string& planet, PerIstream& inStream )
{
	// De-pImpl_ variables used within this function.
	CB_DEPIMPL( MachContinentMap*, pContinentMap_ );
	CB_DEPIMPL( MachCameras*, pCameras_ );
	CB_DEPIMPL( MachWorldViewWindow*, pWorldViewWindow_ );
	CB_DEPIMPL( MachGuiSquadronBank*, pSquadronBank_ );
	CB_DEPIMPL( MachGuiMachineNavigator*, pMachineNavigation_ );
	CB_DEPIMPL( MachGuiConstructionNavigator*, pConstructionNavigation_ );
  	CB_DEPIMPL( MachGuiBmuButton*, pBmuButton_ );
	CB_DEPIMPL( GameState, gameState_ );
	CB_DEPIMPL( MachMachinesIcon*, pMachinesIcon_ );
	CB_DEPIMPL( MachConstructionsIcon*, pConstructionsIcon_ );
	CB_DEPIMPL( MachSquadronIcon*, pSquadronIcon_ );
	CB_DEPIMPL( MachGuiInGameChatMessagesDisplay*, pChatMessageDisplay_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );

	// Don't allow any of the button setting below to trigger their sounds
	MachGuiSoundManager::instance().delaySounds();

	pCameras_->loadSavedGame( inStream );
    pContinentMap_->loadSavedGame( planet, inStream );
    pWorldViewWindow_->loadGame();
	pSquadronBank_->loadGame();
	pMachineNavigation_->loadGame();
	pConstructionNavigation_->loadGame();
	pMachinesIcon_->loadGame();
	pConstructionsIcon_->loadGame();
	pSquadronIcon_->loadGame();
	pControlPanel_->setupDecalBitmaps();

	// button should start depressed.
    pBmuButton_->setDepressed( true );

	// Switch to main menu context.
	mainMenuContext();

	if ( MachLogNetwork::instance().isNetworkGame() )
	{
		pChatMessageDisplay_ = _NEW( MachGuiInGameChatMessagesDisplay( this, Gui::Box( 202, 0, 640, 37 ) ) );
	}

	gameState_ = PLAYING;
}

MachGuiControlPanel& MachInGameScreen::controlPanel()
{
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );

	PRE( pControlPanel_ );

	return *pControlPanel_;
}

bool MachInGameScreen::displayControlPanel() const
{
	CB_DEPIMPL( MachGui::ControlPanelContext, controlPanelContext_ );

	bool returnVal =   controlPanelContext_ == MachGui::SQUADRON_MENU  or
				       controlPanelContext_ == MachGui::FORM_SQUADRON_COMMAND  or
			           controlPanelContext_ == MachGui::MACHINE_NAVIGATION_MENU or
			           controlPanelContext_ == MachGui::CONSTRUCTION_NAVIGATION_MENU or
			           controlPanelContext_ == MachGui::SINGLE_FACTORY or
			           controlPanelContext_ == MachGui::HARDWARE_RESEARCH or
			           controlPanelContext_ == MachGui::CONSTRUCT_COMMAND;

	if ( not returnVal )
	{
		if ( corralState() & CORRAL_ALLCONSTRUCTORS and
			 corralState() & CORRAL_ALLMACHINES and
			 corralState() & CORRAL_SOMEFRIENDLY )
		{
			returnVal = true;
		}
	}

	return returnVal;
}

void MachInGameScreen::setupCameraScrollAreas()
{
	CB_DEPIMPL( MachGuiCameraScrollArea*, pTopCameraScrollArea_ );
	CB_DEPIMPL( MachGuiCameraScrollArea*, pBottomCameraScrollArea_ );
	CB_DEPIMPL( MachGuiCameraScrollArea*, pLeftCameraScrollArea_ );
	CB_DEPIMPL( MachGuiCameraScrollArea*, pRightCameraScrollArea_ );
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachCameras*, pCameras_ );

	_DELETE( pTopCameraScrollArea_ );
	_DELETE( pBottomCameraScrollArea_ );
	_DELETE( pLeftCameraScrollArea_ );
	_DELETE( pRightCameraScrollArea_ );

    RenDevice& device = *pSceneManager_->pDevice();
	const int w = device.windowWidth();
    const int h = device.windowHeight();

	pTopCameraScrollArea_ 	= _NEW( MachGuiCameraScrollArea( this, Gui::Box( 0,0,w,1 ), MachGuiCameraScrollArea::Top, pCameras_, this ) );
	pBottomCameraScrollArea_= _NEW( MachGuiCameraScrollArea( this, Gui::Box( 0,h-1,w,h ), MachGuiCameraScrollArea::Bottom, pCameras_, this ) );
	pLeftCameraScrollArea_ 	= _NEW( MachGuiCameraScrollArea( this, Gui::Box( 0,0,1,h ), MachGuiCameraScrollArea::Left, pCameras_, this ) );
	pRightCameraScrollArea_ = _NEW( MachGuiCameraScrollArea( this, Gui::Box( w-1,0,w,h ), MachGuiCameraScrollArea::Right, pCameras_, this ) );
}

void MachInGameScreen::setupPromptText()
{
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachCameras*, pCameras_ );
	CB_DEPIMPL( MachPromptText*, pPromptText_ );
	CB_DEPIMPL( int, controlPanelXPos_ );

    RenDevice& device = *pSceneManager_->pDevice();
	const int w = device.windowWidth();
    const int h = device.windowHeight();

    //Construct the prompt text displayable
	if ( pPromptText_ == NULL )
	{
    	pPromptText_ = _NEW( MachPromptText(this,
    										Gui::Boundary( controlPanelXPos_, h + MachGui::promptTextYOffset(), w, h ),
    										pCameras_,
    										&worldViewWindow() ) );
	}

	// Move prompt text
	Gui::Coord promptNewTopLeft( controlPanelXPos_ + MachGui::promptTextXOffset(), h + MachGui::promptTextYOffset() );
	positionChildRelative( pPromptText_, promptNewTopLeft );
}

void MachInGameScreen::setupNavigators()
{
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachGuiMachineNavigator*, pMachineNavigation_ );
	CB_DEPIMPL( MachGuiConstructionNavigator*, pConstructionNavigation_ );
	CB_DEPIMPL( MachGuiSquadronBank*, pSquadronBank_ );
	CB_DEPIMPL( MachSquadronIcon*, pSquadronIcon_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );

	if ( not pMachineNavigation_ )
	{
		// Work out relative coord for all navigators
		Gui::Coord navTopLeft( 1, pControlPanel_->getVisibleHeight() - MachGuiNavigatorBase::reqHeight() );

		//Construct machine navigator
		pMachineNavigation_ = _NEW( MachGuiMachineNavigator( pControlPanel_, navTopLeft, this ) );

		//Construct construction navigator
		pConstructionNavigation_ = _NEW( MachGuiConstructionNavigator( pControlPanel_, navTopLeft, this ) );

		//Construct squadron bank
	    pSquadronBank_ = _NEW( MachGuiSquadronBank( pControlPanel_, navTopLeft, pSquadronIcon_, this ) );
	}
	else
	{
		pControlPanel_->repositionNavigators( pMachineNavigation_, pConstructionNavigation_, pSquadronBank_ );
	}
}

void MachInGameScreen::setupCorralAndCommandIcons()
{
	CB_DEPIMPL( MachGuiCorral*, pCorral_ );
	CB_DEPIMPL( MachCommandIcons*, pCommandIcons_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );
	CB_DEPIMPL( Actors, selectedActors_ );

	_DELETE( pCorral_ );
	_DELETE( pCommandIcons_ );

	//Construct corral
	pCorral_ = _NEW( MachGuiCorral( pControlPanel_, Gui::Coord( 1,0 ), this ) );
	pCorral_->initialise();
	pCorral_->add( selectedActors_ );

	//Construct command icons
	Gui::Coord commandCoord( 2, pCorral_->height() + 2 );
 	pCommandIcons_ = _NEW( MachCommandIcons( pControlPanel_, commandCoord, this ) );
}

void MachInGameScreen::resolutionChange()
{
	CB_DEPIMPL( bool, resolutionChanged_ );
	CB_DEPIMPL( MachGuiFirstPerson*, pFirstPerson_ );

	resolutionChanged_ = true;
	pFirstPerson_->resolutionChange();
}

MachInGameScreen::CorralStateBitfield MachInGameScreen::corralState() const
{
	CB_DEPIMPL( CorralStateBitfield, corralState_ );

	return corralState_;
}

void MachInGameScreen::updateCorralState()
{
	CB_DEPIMPL( CorralStateBitfield, corralState_ );
	CB_DEPIMPL( Actors, selectedActors_ );

	corralState_ = CORRAL_EMPTY;

	NEIL_STREAM( "MachInGameScreen::updateCorralState()" << std::endl );
	NEIL_STREAM( "selectedActors_.size() " << selectedActors_.size() << std::endl );

	if ( selectedActors_.size() > 0 )
	{
		bool allMachines = true;
		bool allConstructions = true;
		bool allFactories = true;
		bool allConstructors = true;
		bool allAPCs = true;
		bool someMachines = false;
		bool someConstructions = false;
		bool someFriendly = false;
		bool someEnemy = false;

	    MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

		if ( selectedActors_.size() == 1 )
		{
			corralState_ = CORRAL_ONEENTRY;
			NEIL_STREAM( "   CORRAL_ONEENTRY" << std::endl );
		}

		for ( Actors::iterator iter = selectedActors_.begin(); iter != selectedActors_.end(); ++iter )
	    {
	    	MachActor* pActor = *iter;

			// Work out if actor is enemy or fristd::endly
			if ( pActor->race() == playerRace )
			{
				someFriendly = true;
			}
			else
			{
				someEnemy = true;
			}

			if ( pActor->objectIsMachine() )
			{
				NEIL_STREAM( "   pActor->objectIsMachine()" << std::endl );

				allConstructions = false;
				someMachines = true;

				if ( pActor->objectType() != MachLog::APC )
				{
					allAPCs = false;
				}
				if ( pActor->objectType() != MachLog::CONSTRUCTOR )
				{
					allConstructors = false;
				}
			}
			else if ( pActor->objectIsConstruction() )
			{
				NEIL_STREAM( "   pActor->objectIsConstruction()" << std::endl );

				allMachines = false;
				someConstructions = true;

				if ( pActor->objectType() != MachLog::FACTORY )
				{
					allFactories = false;
				}
			}
		}


		if ( allMachines )
		{
			corralState_ |= CORRAL_ALLMACHINES;
			NEIL_STREAM( "   CORRAL_ALLMACHINES" << std::endl );
		}
		if ( allConstructions )
		{
			corralState_ |= CORRAL_ALLCONSTRUCTIONS;
			NEIL_STREAM( "   CORRAL_ALLCONSTRUCTIONS" << std::endl );
		}
		if ( allFactories and someConstructions )
		{
			corralState_ |= CORRAL_ALLFACTORIES;
			NEIL_STREAM( "   CORRAL_ALLFACTORIES" << std::endl );
		}
		if ( allConstructors and someMachines )
		{
			corralState_ |= CORRAL_ALLCONSTRUCTORS;
			NEIL_STREAM( "   CORRAL_ALLCONSTRUCTORS" << std::endl );
		}
		if ( allAPCs and someMachines )
		{
			corralState_ |= CORRAL_ALLAPCS;
			NEIL_STREAM( "   CORRAL_ALLAPCS" << std::endl );
		}
		if ( someMachines )
		{
			corralState_ |= CORRAL_SOMEMACHINES;
			NEIL_STREAM( "   CORRAL_SOMEMACHINES" << std::endl );
		}
		if ( someConstructions )
		{
			corralState_ |= CORRAL_SOMECONSTRUCTIONS;
			NEIL_STREAM( "   CORRAL_SOMECONSTRUCTIONS" << std::endl );
		}
		if ( someEnemy )
		{
			corralState_ |= CORRAL_SOMEENEMY;
			NEIL_STREAM( "   CORRAL_SOMEENEMY" << std::endl );
		}
		if ( someFriendly )
		{
			corralState_ |= CORRAL_SOMEFRIENDLY;
			NEIL_STREAM( "   CORRAL_SOMEFRIENDLY" << std::endl );
		}

	}

	NEIL_STREAM( "corralState_ " << corralState_ << std::endl );
}

bool MachInGameScreen::controlPanelOn() const
{
	CB_DEPIMPL( bool, controlPanelOn_ );

	return controlPanelOn_;
}

void MachInGameScreen::controlPanelOn( bool on )
{
	CB_DEPIMPL( bool, controlPanelOn_ );

	controlPanelOn_ = on;
}

void MachInGameScreen::initiateScreenShot()
{
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( bool, renderingScreenShot_ );

    RenDevice& device = *pSceneManager_->pDevice();

	// It's quite possible that other high-quality rendering options could
	// be turned on, in addition to anti-aliasing.
	if (device.capabilities().supportsEdgeAntiAliasing() && !device.antiAliasingOn())
	{
		device.antiAliasingOn(true);
		renderingScreenShot_ = true;
	}
	else
	{
		saveScreenShot();
	}
}

bool MachInGameScreen::isRenderingScreenShot() const
{
	return pImpl_->renderingScreenShot_;
}

void MachInGameScreen::finalizeScreenShot()
{
	PRE(isRenderingScreenShot());
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( bool, renderingScreenShot_ );

	saveScreenShot();

	// This carefully avoids turning anti-aliasing off when some other client
	// of RenDevice has turned it on (perhaps at the player's behest).
    RenDevice& device = *pSceneManager_->pDevice();
	device.antiAliasingOn(false);
	renderingScreenShot_ = false;

	POST(!isRenderingScreenShot());
}

// Can't use Gui::writeScreenAsBmp() because we need to save the front buffer.
void MachInGameScreen::saveScreenShot()
{
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );

    bool    gotBmpPathName = false;
    SysPathName bmpPathName;
    size_t  count = 0;

	// Search for the first numbered file which doesn't exist already.
	while ( not gotBmpPathName )
	{
		char buffer[20];

        sprintf( buffer, "%04ld", count );

        //bmpPathName = string( "mach" ) + buffer + ".bmp";
        bmpPathName = SysPathName(string( "mach" ) + buffer + ".bmp");

        if( not bmpPathName.existsAsFile() )
            gotBmpPathName = true;

        ++count;
	}

    RenDevice& device = *pSceneManager_->pDevice();
	RenSurface front  = device.frontSurface();
	front.saveAsBmp( RenSurface::Rect( 0, 0, front.width(), front.height() ), bmpPathName );
}

Gui::Box MachInGameScreen::getWorldViewWindowVisibleArea() const
{
	CB_DEPIMPL( W4dSceneManager*, pSceneManager_ );
	CB_DEPIMPL( MachGuiControlPanel*, pControlPanel_ );

	Gui::Coord topLeftCoord = pControlPanel_->relativeCoord();

    RenDevice& device = *pSceneManager_->pDevice();
    const int w = device.windowWidth();
    const int h = device.windowHeight();
	Gui::Box visibleArea( pControlPanel_->width() + topLeftCoord.x(), 0, w, h );

	return visibleArea;
}

MachGuiIonAttackCommand* MachInGameScreen::ionAttackCommand()
{
	CB_DEPIMPL(	MachGuiIonAttackCommand*, pIonAttackCommand_ );

	return pIonAttackCommand_;
}

MachGuiNukeAttackCommand* MachInGameScreen::nukeAttackCommand()
{
	CB_DEPIMPL(	MachGuiNukeAttackCommand*, pNukeAttackCommand_ );

	return pNukeAttackCommand_;
}

MachInGameScreen::GameState MachInGameScreen::actualGameState() const
{
	CB_DEPIMPL( MachInGameScreen::GameState, gameState_ );
	CB_DEPIMPL( PhysAbsoluteTime, gameStateTimer_ );

	GameState retVal = PLAYING;

	if ( gameState_ != PLAYING )
	{
		PhysAbsoluteTime now = Phys::time();
		// Only return that game has been lost/won after it has been in this state for a few seconds.
		// This enables pod destruction animation time to run and music to play.
		if ( now - gameStateTimer_ > 2.0 )
		{
			retVal = gameState_;
		}
	}

	return retVal;
}

bool MachInGameScreen::isNetworkStuffed() const
{
	CB_DEPIMPL( double, networkStuffedStartTime_ );

	bool retValue = false;

	if ( MachLogNetwork::instance().isNetworkGame() )
	{
		double now = DevTime::instance().time();

		retValue = now - networkStuffedStartTime_ > 2.0; // Network has been stuffed for over two seconds
	}

	return retValue;
}

void MachInGameScreen::disableFirstPerson( bool newValue )
{
	CB_DEPIMPL( bool, disableFirstPerson_ );

	disableFirstPerson_ = newValue;
}

bool MachInGameScreen::isFirstPersonDisabled() const
{
	CB_DEPIMPL( bool, disableFirstPerson_ );

	bool retValue = false;

	if ( MachLogNetwork::instance().isNetworkGame() and disableFirstPerson_ )
	{
		retValue = true;
	}

	return retValue;
}

void MachInGameScreen::instantExit( bool newValue )
{
	CB_DEPIMPL( bool, instantExit_ );

	instantExit_ = newValue;
}

bool MachInGameScreen::instantExit() const
{
	CB_DEPIMPL( bool, instantExit_ );

	return instantExit_;
}

/* End INGAME.CPP ***************************************************/

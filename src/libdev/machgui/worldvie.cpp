/*
 * W O R L D V I E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/worldvie.hpp"
#include "machgui/ingame.hpp"
#include "machgui/gui.hpp"
#include "machgui/command.hpp"
#include "machgui/cameras.hpp"
#include "machgui/ctrladon.hpp"
#include "machgui/database.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "machlog/races.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/actor.hpp"
#include "machlog/cntrl_pc.hpp"
#include "gui/event.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/camera.hpp"
#include "world4d/domain.hpp"
#include "world4d/entity.hpp"
#include "world4d/generic.hpp"
#include "world4d/bbxselec.hpp"
#include "render/device.hpp"
#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/epsilon.hpp"
#include "utility/id.hpp"
#include "system/pathname.hpp"
#include "gui/painter.hpp"
#include "device/keytrans.hpp"
#include "profiler/profiler.hpp"
#include "render/camera.hpp"
#include "sim/manager.hpp"

MachWorldViewWindow::MachWorldViewWindow( 	MachInGameScreen * pParent,
											const Gui::Boundary& relativeBox,
											MachCameras* pCameras )
:   GuiDisplayable( pParent, relativeBox ),
    pInGameScreen_( pParent ),
    haveHighlightedActor_( false ),
	pCameras_( pCameras ),
	mouseButtonPressed_( false ),
	rubberBandCamera_( INVALID ),
	rubberBanding_( false )
{
	pKeyTranslator_ = _NEW( DevKeyToCommandTranslator() );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_Z, SELECT_ONSCREEN_MACHINES, DevKeyToCommand::RELEASED, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_X, SELECT_ONSCREEN_CONSTRUCTIONS, DevKeyToCommand::RELEASED, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_Z, SELECT_ONSCREEN_MACHINES_KEEP_SEL, DevKeyToCommand::PRESSED, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_X, SELECT_ONSCREEN_CONSTRUCTIONS_KEEP_SEL, DevKeyToCommand::PRESSED, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->initEventQueue();

	selectedEntities_.reserve( 20 );

	useFastSecondDisplay( false );

	redrawEveryFrame( true );
}


MachWorldViewWindow::~MachWorldViewWindow()
{
	_DELETE( pKeyTranslator_ );
}

//virtual
void MachWorldViewWindow::doHandleMouseClickEvent( const GuiMouseEvent& event )
{
	PRE( pCameras_ );

    //Only interested in left mouse clicks over the window
    mouseButtonPressed_ = event.leftButton() == Gui::PRESSED;

    if( mouseButtonPressed_)
    {
        //get the 3d line
		Gui::Coord absCoord( event.coord() );
		absCoord += absoluteBoundary().minCorner();
		startRubberBand_ = absCoord;

		// Store camera used at beginning of rubber banding operation
		if ( pCameras_->isZenithCameraActive() )
			rubberBandCamera_ = ZENITH;
		else if ( pCameras_->isGroundCameraActive() )
			rubberBandCamera_ = GROUND;
		else
			rubberBandCamera_ = INVALID;

		MexLine3d cursorLine = cameraThroughCursorLine( absCoord, 1000.0 );

		// If we are rubber banding in zenith mode then we need to fix the
		// rubber band start point to a point at ground level in the planet.
		if ( rubberBandCamera_ == ZENITH )
		{
			// Work out the 3D position when the cursor line hits z = 0
	 		MexVec3 cursorLineUnitVec = cursorLine.unitDirectionVector();
			MATHEX_SCALAR ratio = cursorLine.end1().z() / cursorLineUnitVec.z();
			MATHEX_SCALAR x2 = cursorLine.end1().x() - ( ratio *  cursorLineUnitVec.x() );
			MATHEX_SCALAR y2 = cursorLine.end1().y() - ( ratio *  cursorLineUnitVec.y() );

			startRubberBand3DPos_ = MexPoint3d( x2, y2, 0 );
		}
    }
	else
	{
		if ( rubberBanding_ )
		{
			// Select all actors inside rubber band. Must press ctrl to keep already selected items
			updateActorsSelectedViaRubberBand( false, DevKeyboard::instance().ctrlPressed() );
			pCameras_->resumeMotion();
			rubberBanding_ = false;
		}
		else
		{
			Gui::Coord absCoord( event.coord() );
			absCoord += absoluteBoundary().minCorner();

			// No rubber banding was taking place so let the commands respond to
			// the mouse button release
			MexLine3d cursorLine = cameraThroughCursorLine( absCoord, 1000.0 );

        	//Dispatch the click
	        dispatchCursor( cursorLine, true, event.isCtrlPressed(), event.isShiftPressed(), event.isAltPressed() );
		}
	}
}

MexLine3d MachWorldViewWindow::cameraThroughCursorLine
(
    const Gui::Coord& cursorScreenPoint, MATHEX_SCALAR length
) const
{

    //Get the scene manager
    W4dSceneManager& sceneManager = pInGameScreen_->sceneManager();

	// The viewport must be correctly set to correspond to this window before
	// we can use the RenDevice methods.
	pInGameScreen_->setWorldViewViewport();

    //Hence get corresponding world position
    MexPoint3d cursorWorldPos = sceneManager.pDevice()->screenToCamera( cursorScreenPoint );

	// Reset the viewport correctly for GUI drawing.  TBD: a save/restore or
	// push/pop idiom would be much more robust.
	pInGameScreen_->setGuiViewport();

    //Construct a line from the camera origin through the cursor point in world coordinates
    const MexTransform3d& cameraTransform = sceneManager.currentCamera()->globalTransform();
    MexPoint3d cameraOrigin( cameraTransform.position() );
    cameraTransform.transform( &cursorWorldPos );
    MexLine3d shortLine( cameraOrigin, cursorWorldPos );

    //Now create one of length 1000m
    MexPoint3d farPoint = shortLine.pointAtDistance( length );
    MexLine3d theLine( cameraOrigin, farPoint, length );

    return theLine;
}

void MachWorldViewWindow::dispatchCursor
(
    const MexLine3d& cursorLine, bool click, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    //Get the active command
    MachGuiCommand& activeCommand = pInGameScreen_->activeCommand();

    //Get the scene manager
    W4dSceneManager& sceneManager = pInGameScreen_->sceneManager();

    //Get the domain in which the camera is located
    W4dDomain* pCameraDomain = sceneManager.currentCamera()->containingDomain();

    //Default 2d cursor is arrow
    MachGui::Cursor2dType cursor2d = MachGui::MENU_CURSOR;

    //Get the pointer of any currently highlighted actor
    MachActor* pOldHighlightedActor = NULL;
    MachActor* pNewHighlightedActor = NULL;
    if( haveHighlightedActor_ )
    {
        MachLogRaces& races = MachLogRaces::instance();
        if( races.actorExists( highLightedActorId_ ) )
            pOldHighlightedActor = &races.actor( highLightedActorId_ );
    }

    //Find any entity intersecting the line
    W4dEntity* pEntity;
    MATHEX_SCALAR distance = 0.0;
	bool hit = false;


	hit = pCameraDomain->findNearerEntity(	cursorLine, cursorLine.length(),
                                            W4dEntity::nextCheckId(),
                                            W4dEntity::MEDIUM, &pEntity, &distance,
                                            pInGameScreen_->cursorFilter() );

	if ( distance <= 0.0 or distance > ( RenDevice::current()->fogEnd() * 0.9 ) /* After 90% fog distance things aren't visible*/ )
	{
		hit = false;
	}

    //If we got something, dispatch it
    if( hit )
    {
        //Check for a hit on an actor ( only if we are not in FOW )
        UtlId hitId = pEntity->id();
        MachLogRaces& races = MachLogRaces::instance();

        if( not MachGuiCommand::cursorInFogOfWar() and races.actorExists( hitId ) )
        {
            //Get the picked actor
            MachActor& hitActor = races.actor( hitId );

			// Check that actor has not recently been blown up
			// so that bounding box isn't put round an explosion
			if ( not hitActor.isDead() )
			{
				// Display prompt text for actor
				pInGameScreen_->displayActorPromptText( &hitActor );

	            //Dispatch the hit to the active command
	            if( click )
				{
					// Can't dispatch commands if game is paused or network is busy
					if ( SimManager::instance().isSuspended() or pInGameScreen_->isNetworkStuffed() )
					{
						// Only allow selecting when paused
						if ( activeCommand.cursorOnActor( &hitActor, ctrlPressed, shiftPressed, altPressed ) == MachGui::SELECT_CURSOR )
						{
							activeCommand.pickOnActor( &hitActor, ctrlPressed, shiftPressed, altPressed );
						}
						else
						{
							MachGuiSoundManager::instance().playSound( "gui/sounds/clickbad.wav" );
						}
					}
					else
					{
	                	activeCommand.pickOnActor( &hitActor, ctrlPressed, shiftPressed, altPressed );
					}
				}
	            else
				{
	                cursor2d = activeCommand.cursorOnActor( &hitActor, ctrlPressed, shiftPressed, altPressed );
				}

	            //If the actor under the cursor is not selected, highlight it
	            if( hitActor.selectionState() != MachLog::SELECTED and hitActor.selectableType() == MachLog::FULLY_SELECTABLE )
	                pNewHighlightedActor = &hitActor;
        	}
        }
        else
        {
			pInGameScreen_->clearCursorPromptText();

            //Assume hit on terrain. Get the point at the hit distance.
            MexPoint3d hitPoint = cursorLine.pointAtDistance( distance );

            //Add the 6 cm drop value on the terrain
            hitPoint.z( hitPoint.z() + 0.06 );

            //Dispatch it
            if( click )
			{
				// Can't issue commands if game is paused or network is busy
				if ( SimManager::instance().isSuspended() or pInGameScreen_->isNetworkStuffed() )
				{
					MachGuiSoundManager::instance().playSound( "gui/sounds/clickbad.wav" );
				}
				else
				{
			        activeCommand.pickOnTerrain( hitPoint, ctrlPressed, shiftPressed, altPressed );
				}
			}
            else
			{
                cursor2d = activeCommand.cursorOnTerrain( hitPoint, ctrlPressed, shiftPressed, altPressed );
			}
        }
    }
	else
	{
		pInGameScreen_->clearCursorPromptText();
	}

    //Update the highlight state of any new/previous actor
    if( pOldHighlightedActor  and
        pOldHighlightedActor != pNewHighlightedActor  and
        pOldHighlightedActor->selectionState() == MachLog::HIGHLIGHTED )
	{
	    pOldHighlightedActor->selectionState( MachLog::NOT_SELECTED );
		pInGameScreen_->removeHighlightedActor();
	}

    haveHighlightedActor_ = pNewHighlightedActor != NULL;

    if(haveHighlightedActor_ )
    {
        //Cache the highlighted id so we know which one to unhighlight next time
        highLightedActorId_ = pNewHighlightedActor->id();

        //Change to highlight state if not already in that state
        if( pNewHighlightedActor->selectionState() != MachLog::HIGHLIGHTED )
		{
			pNewHighlightedActor->selectionState( MachLog::HIGHLIGHTED );
		}

		pInGameScreen_->highlightActor( pNewHighlightedActor );
    }

    //If not a click, update the 2d cursor shape
    if( not click )
        pInGameScreen_->cursor2d( cursor2d );
}

//virtual
void MachWorldViewWindow::doHandleMouseEnterEvent( const GuiMouseEvent& event )
{
    //Action same as for position update
    doHandleContainsMouseEvent( event );
}

//virtual
void MachWorldViewWindow::doHandleMouseExitEvent( const GuiMouseEvent& )
{
    //Switch to menu cursor
    pInGameScreen_->cursor2d( MachGui::MENU_CURSOR );

	// Remove highlight from around actor
	if ( haveHighlightedActor_ )
	{
		MachLogRaces& races = MachLogRaces::instance();
		if( races.actorExists( highLightedActorId_ ) )
            races.actor( highLightedActorId_ ).selectionState( MachLog::NOT_SELECTED );
		pInGameScreen_->removeHighlightedActor();
		pInGameScreen_->clearCursorPromptText();
		haveHighlightedActor_ = false;
	}
}

//virtual
void MachWorldViewWindow::doHandleContainsMouseEvent( const GuiMouseEvent& event )
{
    //get the 3d line
	Gui::Coord absCoord( event.coord() );
	absCoord += absoluteBoundary().minCorner();

    MexLine3d cursorLine = cameraThroughCursorLine( absCoord, 1000.0 );

    //Dispatch the click
    dispatchCursor( cursorLine, false, event.isCtrlPressed(), event.isShiftPressed(),
                    event.isAltPressed() );

	// Do special right hand mouse click funtionality if right hand mouse button is pressed
	// and above functions were not processed.
	if ( event.rightButton() == Gui::PRESSED )
	{
		doMouseRightClickCameraMotion( event );
	}
}

void MachWorldViewWindow::doMouseRightClickCameraMotion( const GuiMouseEvent& event )
{
	Gui::Coord clickCoord = pInGameScreen_->rightClickMousePos();
	// Make original right click pos relative to world view window
	clickCoord.x( clickCoord.x() - absoluteBoundary().minCorner().x() );
	clickCoord.y( clickCoord.y() - absoluteBoundary().minCorner().y() );

	MexLine3d line( MexPoint3d( clickCoord.x(), clickCoord.y(), 0 ),
					MexPoint3d( event.coord().x(), event.coord().y(), 0 ) );

	// Has mouse moved far enough away from original position to perform camera
	// motion. Must have moved at least 10 pixels.
	if ( line.length() > 10 )
	{
		MexLine3d hLine(MexPoint3d( clickCoord.x(), 0, 0 ),
						MexPoint3d( event.coord().x(), 0, 0 ) );
		MexLine3d vLine(MexPoint3d( 0, clickCoord.y(), 0 ),
						MexPoint3d( 0, event.coord().y(), 0 ) );

		if ( hLine.length() > vLine.length() )
		{
			if ( event.coord().x() > clickCoord.x() )
			{
				pCameras_->scroll( MachCameras::RIGHT, event );
			}
			else
			{
				pCameras_->scroll( MachCameras::LEFT, event );
			}
		}
		else
		{
			if ( event.coord().y() > clickCoord.y() )
			{
				pCameras_->scroll( MachCameras::DOWN, event );
			}
			else
			{
				pCameras_->scroll( MachCameras::UP, event );
			}
		}
	}
}

void MachWorldViewWindow::update()
{
	// Cause re-draw if mouse button pressed to show rubber band.
	if ( mouseButtonPressed_ )
	{
		// Check that mouse button is still pressed.
		if ( DevMouse::instance().leftButton() )
		{
			// Check current camera
			RubberBandCamera camera;
			if ( pCameras_->isZenithCameraActive() )
				camera = ZENITH;
			else if ( pCameras_->isGroundCameraActive() )
				camera = GROUND;
			else
			{
				// Camera has changed to invalid camera
				mouseButtonPressed_ = false;
				rubberBanding_ = false;
				unhighlightActorsSelectedViaRubberBand();
				pCameras_->resumeMotion();
				return;
			}

			if ( camera != rubberBandCamera_ )
			{
				// Camera has changed midway through rubber banding. Cancel rubber banding.
				mouseButtonPressed_ = false;
				rubberBanding_ = false;
				unhighlightActorsSelectedViaRubberBand();
				pCameras_->resumeMotion();
				return;
			}

			if ( rubberBandCamera_ == ZENITH )
			{
				// Work out new startRubberBand_ based on startRubberBand3DPos_. This takes account of the
				// fact that scrolling may have happened.

				// The viewport must be correctly set to correspond to this window before
				// we can use the RenDevice methods.
				pInGameScreen_->setWorldViewViewport();

				W4dSceneManager& sceneManager = pInGameScreen_->sceneManager();
			    const MexTransform3d& cameraTransform = sceneManager.currentCamera()->globalTransform();
				MexPoint3d posIn3DWorld = startRubberBand3DPos_;
				cameraTransform.transformInverse( &posIn3DWorld );
				startRubberBand_ = RenDevice::current()->cameraToScreen( posIn3DWorld );

				// Reset the viewport correctly for GUI drawing.  TBD: a save/restore or
				// push/pop idiom would be much more robust.
				pInGameScreen_->setGuiViewport();
			}

			endRubberBand_.x( DevMouse::instance().position().first );
			endRubberBand_.y( DevMouse::instance().position().second );

			if ( not rubberBanding_ )
			{
				// Check that start and end points are far enough apart to start rubber banding.
				// Note : used MexLine3d because there doesn't appear to be a "length" function
				// for MexLine2d.
				MexLine3d line( MexPoint3d( startRubberBand_.x(), startRubberBand_.y(), 0 ),
								MexPoint3d( endRubberBand_.x(), endRubberBand_.y(), 0 ) );
				if ( line.length() > 10 )
				{
					rubberBanding_ = true;
					if ( rubberBandCamera_ == GROUND )
						pCameras_->freezeMotion(); //Scrolling is not allowed in Ground camera when rubber banding
				}
			}

			if ( rubberBanding_ )
			{
				// Select all actors inside rubber band. Must press ctrl to keep already selected items
				updateActorsSelectedViaRubberBand( true, DevKeyboard::instance().ctrlPressed() );
			}
		}
		else
		{
			if ( rubberBanding_ )
			{
				// Select all actors inside rubber band. Must press ctrl to keep already selected items
				updateActorsSelectedViaRubberBand( false, DevKeyboard::instance().ctrlPressed() );
				pCameras_->resumeMotion();
				rubberBanding_ = false;
			}
			mouseButtonPressed_ = false;
		}
	}
}

void MachWorldViewWindow::unhighlightActorsSelectedViaRubberBand()
{
	MachLogRaces& races = MachLogRaces::instance();

	for ( 	ctl_vector< W4dId >::iterator iter = selectedEntities_.begin();
			iter != selectedEntities_.end();
			++iter )
	{
		if( races.actorExists( *iter ) )
		{
			if ( races.actor( *iter ).selectionState() == MachLog::HIGHLIGHTED )
           		races.actor( *iter ).selectionState( MachLog::NOT_SELECTED );
		}
	}

	selectedEntities_.erase( selectedEntities_.begin(), selectedEntities_.end() );
}

void MachWorldViewWindow::updateActorsSelectedViaRubberBand( bool highlightOnly, bool keepSelection, bool inclMachines /*= true*/, bool inclConstructions /*= true*/ )
{
	PRE( inclMachines or inclConstructions );

	if ( not highlightOnly )
	{
		// Unselect all currently selected item if keepSelection is not true
		if ( not keepSelection )
		{
			pInGameScreen_->deselectAll();
		}
	}

	unhighlightActorsSelectedViaRubberBand();

	// The viewport must be correctly set to correspond to this window before
	// we can use the RenDevice methods.
	pInGameScreen_->setWorldViewViewport();

	W4dBoundingBoxSelector::Entities entitiesInRubberBand;
	entitiesInRubberBand.reserve( 20 );

	// Make sure rubber band area is big enough to continue. A small rubber band area will result
	// in an ASSERT in W4dBoundingBoxSelector.
	if ( Mathex::abs( startRubberBand_.x() - endRubberBand_.x() ) > MexEpsilon::instance() and
		 Mathex::abs( startRubberBand_.y() - endRubberBand_.y() ) > MexEpsilon::instance() )
	{
		// Sort rubber band points into clockwise order	and select entities inside bounding box
		if ( ( endRubberBand_.x() > startRubberBand_.x() and endRubberBand_.y() > startRubberBand_.y() ) or
			 ( endRubberBand_.x() < startRubberBand_.x() and endRubberBand_.y() < startRubberBand_.y() ) )
		{
			W4dSceneManager& sceneManager = pInGameScreen_->sceneManager();

		    MexPoint3d rbPoint1 = sceneManager.pDevice()->screenToCamera( startRubberBand_ );
			MexPoint3d rbPoint2 = sceneManager.pDevice()->screenToCamera( MexPoint2d( endRubberBand_.x(), startRubberBand_.y() ) );
			MexPoint3d rbPoint3 = sceneManager.pDevice()->screenToCamera( endRubberBand_ );
			MexPoint3d rbPoint4 = sceneManager.pDevice()->screenToCamera( MexPoint2d( startRubberBand_.x(), endRubberBand_.y() ) );

			MexQuad3d boundingBox( rbPoint1, rbPoint2, rbPoint3, rbPoint4 );
			W4dBoundingBoxSelector selector( *sceneManager.currentCamera(), boundingBox );

			selector.clipAtFogDistance( rubberBandCamera_ == GROUND );

		    entitiesInRubberBand = selector.selectedEntities();
		}
		else
		{
			W4dSceneManager& sceneManager = pInGameScreen_->sceneManager();

		    MexPoint3d rbPoint1 = sceneManager.pDevice()->screenToCamera( startRubberBand_ );
			MexPoint3d rbPoint2 = sceneManager.pDevice()->screenToCamera( MexPoint2d( startRubberBand_.x(), endRubberBand_.y() ) );
			MexPoint3d rbPoint3 = sceneManager.pDevice()->screenToCamera( endRubberBand_ );
			MexPoint3d rbPoint4 = sceneManager.pDevice()->screenToCamera( MexPoint2d( endRubberBand_.x(), startRubberBand_.y() ) );

			MexQuad3d boundingBox( rbPoint1, rbPoint2, rbPoint3, rbPoint4 );
			W4dBoundingBoxSelector selector( *sceneManager.currentCamera(), boundingBox );

			selector.clipAtFogDistance( rubberBandCamera_ == GROUND );

		    entitiesInRubberBand = selector.selectedEntities();
		}
	}

	// Reset the viewport correctly for GUI drawing.  TBD: a save/restore or
	// push/pop idiom would be much more robust.
	pInGameScreen_->setGuiViewport();

	MachLogRaces& races = MachLogRaces::instance();
    MachPhys::Race playerRace = races.pcController().race();

	// Actors which are going to be selected into the corral
	MachInGameScreen::Actors selectMachines_;
	MachInGameScreen::Actors selectCanAttackMachines_;
	MachInGameScreen::Actors selectConstructions_;
	MachInGameScreen::Actors selectCanAttackConstructions_;
	selectMachines_.reserve( 20 );
	selectConstructions_.reserve( 10 );
	selectCanAttackMachines_.reserve( 20 );
	selectCanAttackConstructions_.reserve( 10 );

	for ( 	W4dBoundingBoxSelector::Entities::iterator iter = entitiesInRubberBand.begin();
			iter != entitiesInRubberBand.end();
			++iter )
	{
		W4dEntity* pEntity = *iter;
		if( races.actorExists( pEntity->id() ) )
		{
			MachActor& actor = races.actor( pEntity->id() );

			// Only select actors belonging to player
			if ( actor.race() == playerRace and actor.selectableType() == MachLog::FULLY_SELECTABLE and
				 ( ( inclMachines and actor.objectIsMachine() ) or ( inclConstructions and actor.objectIsConstruction() ) ) )
			{
				if ( highlightOnly ) // Just highlight the actors for now because the rubber-banding hasn't finished
				{
					if ( actor.selectionState() != MachLog::SELECTED )
					{
	            		actor.selectionState( MachLog::HIGHLIGHTED );
						selectedEntities_.push_back( pEntity->id() );
					}
				}
				else // Select actors into corral
				{
					if ( actor.selectionState() != MachLog::SELECTED )
					{
						if ( actor.objectIsMachine() )
						{
							if ( actor.objectIsCanAttack() )
							{
								selectCanAttackMachines_.push_back( &actor );
							}
							else
							{
								selectMachines_.push_back( &actor );
							}
						}
						else
						{
							if ( actor.objectIsCanAttack() )
							{
								selectCanAttackConstructions_.push_back( &actor );
							}
							else
							{
								selectConstructions_.push_back( &actor );
							}
						}
					}
				}
			}
		}
	}

	if ( not highlightOnly )
	{
		selectedEntities_.erase( selectedEntities_.begin(), selectedEntities_.end() );

		// Put actors into corral.
		// Can attack machines at front of corral
		if ( selectCanAttackMachines_.size() != 0 )
		{
			pInGameScreen_->select( selectCanAttackMachines_ );
		}
		// Civilian Machines
		if ( selectMachines_.size() != 0 )
		{
			pInGameScreen_->select( selectMachines_ );
		}
		// Missile emplacements and other can attack constructions
		if ( selectCanAttackConstructions_.size() != 0 )
		{
			pInGameScreen_->select( selectCanAttackConstructions_ );
		}
		// Constructions at end of corral.
		if ( selectConstructions_.size() != 0 )
		{
			pInGameScreen_->select( selectConstructions_ );
		}
	}
}

// virtual
void MachWorldViewWindow::doDisplay()
{
	// Draw rubber band
	if ( rubberBanding_ )
	{
		// The viewport must be correctly set to correspond to this window before
		// we can use the RenDevice methods.
		pInGameScreen_->setWorldViewViewport();

		GuiPainter::instance().hollowRectangle( Gui::Box( startRubberBand_, endRubberBand_ ), Gui::GREEN(), 1 );

		// Reset the viewport correctly for GUI drawing.  TBD: a save/restore or
		// push/pop idiom would be much more robust.
		pInGameScreen_->setGuiViewport();
	}

	if ( SimManager::instance().isSuspended() )
	{
		GuiBitmap pausedBmp;

		switch ( pInGameScreen_->actualGameState() )
		{
			case MachInGameScreen::WON:
				if ( MachGuiDatabase::instance().currentScenario().isTrainingScenario() )
				{
					pausedBmp = Gui::bitmap( "gui/misc/complete.bmp" );
				}
				else
				{
					pausedBmp = Gui::bitmap( "gui/misc/victory.bmp" );
				}
				break;
			case MachInGameScreen::LOST:
				if ( MachGuiDatabase::instance().currentScenario().isTrainingScenario() )
				{
					pausedBmp = Gui::bitmap( "gui/misc/failed.bmp" );
				}
				else
				{
					pausedBmp = Gui::bitmap( "gui/misc/defeat.bmp" );
				}
				break;
			default:
				pausedBmp = Gui::bitmap( "gui/misc/paused.bmp" );
				break;
		}

		pausedBmp.enableColourKeying();

		Gui::Coord topLeftCoord = absoluteBoundary().minCorner();

	    RenDevice& device = *pInGameScreen_->sceneManager().pDevice();
	    const int w = device.windowWidth();
	    const int h = device.windowHeight();
		int xPos = ( ( w - topLeftCoord.x() ) / 2 ) - ( pausedBmp.width() / 2 );
		int yPos = ( ( h - topLeftCoord.y() ) / 2 ) - ( pausedBmp.height() / 2 );

		GuiPainter::instance().blit( pausedBmp, Gui::Coord(xPos+topLeftCoord.x(), yPos+topLeftCoord.y()) );
	}
	else if ( pInGameScreen_->isNetworkStuffed() )
	{
		// Display "Network Busy" message ( top right )
		GuiBitmap pausedBmp = Gui::bitmap( "gui/misc/netbusy.bmp" );
		pausedBmp.enableColourKeying();

	    RenDevice& device = *pInGameScreen_->sceneManager().pDevice();
	    const int w = device.windowWidth();
		int xPos = w - pausedBmp.width();

		GuiPainter::instance().blit( pausedBmp, Gui::Coord( xPos, 0 ) );
	}
}

bool MachWorldViewWindow::rubberBandSelectionHappening() const
{
	return rubberBanding_;
}

void MachWorldViewWindow::loadGame()
{}

void MachWorldViewWindow::unloadGame()
{}

bool MachWorldViewWindow::processButtonEvent( const DevButtonEvent& buttonEvent )
{
	if ( buttonEvent.action() == DevButtonEvent::RELEASE or rubberBanding_ )
		return false;

	typedef DevKeyToCommand::CommandId CommandId;

	CommandId commandId;
	bool processed = pKeyTranslator_->translate( buttonEvent, &commandId );

	if ( processed )
	{
		// Make rubber band that is same size as worldview window
		Gui::Box worldviewArea = pInGameScreen_->getWorldViewWindowVisibleArea();

		startRubberBand_ = worldviewArea.minCorner();;
		endRubberBand_ = worldviewArea.maxCorner();

		switch ( commandId )
		{
			case SELECT_ONSCREEN_MACHINES:
				updateActorsSelectedViaRubberBand( false, false, true, false );
				break;
			case SELECT_ONSCREEN_CONSTRUCTIONS:
				updateActorsSelectedViaRubberBand( false, false, false, true );
				break;
			case SELECT_ONSCREEN_MACHINES_KEEP_SEL:
				updateActorsSelectedViaRubberBand( false, true, true, false );
				break;
			case SELECT_ONSCREEN_CONSTRUCTIONS_KEEP_SEL:
				updateActorsSelectedViaRubberBand( false, true, false, true );
				break;
		}
	}

	return processed;
}

UtlId MachWorldViewWindow::highlightedActorId() const
{
	return highLightedActorId_;
}

bool MachWorldViewWindow::haveHighlightedActor() const
{
	return haveHighlightedActor_;
}

/* End WORLDVIE.CPP *************************************************/

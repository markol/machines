/*
 * C A M E R A S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cameras.hpp"
#include "device/butevent.hpp"
#include "device/keytrans.hpp"
#include "mathex/poly2d.hpp"
#include "phys/motchunk.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/mcentity.hpp"
#include "world4d/manager.hpp"
#include "world4d/root.hpp"
#include "world4d/soundman.hpp"
#include "phys/mcfly.hpp"
#include "phys/mcground.hpp"
#include "phys/mczenith.hpp"
#include "phys/motchunk.hpp"
#include "machlog/camera.hpp"
#include "machlog/races.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/motconst.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/planet.hpp"
#include "machphys/machine.hpp"
#include "machphys/machdata.hpp"
#include "machphys/plansurf.hpp"
#include "envirnmt/planet.hpp"
#include "render/device.hpp"
#include "utility/linetok.hpp"
#include "gui/event.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>

static void readZenithDataFile( MATHEX_SCALAR* pZenithMinHeight, MATHEX_SCALAR* pZenithMaxHeight, MATHEX_SCALAR* pZenithElevation,
								MATHEX_SCALAR* pZenithEarMinHeight, MATHEX_SCALAR* pZenithEarMaxHeight );

MachCameras::MachCameras( W4dSceneManager* pSceneManager, W4dRoot* pRoot )
:	pSceneManager_( pSceneManager ),
	pRoot_( pRoot ),
   	pCurrentCamera_(NULL),
	pGroundControl_(NULL),
	pFreeControl_(NULL),
	pZenithControl_(NULL),
	pFirstPersonControl_(NULL),
	pGroundCamera_(NULL),
	pFreeCamera_(NULL),
	pZenithCamera_(NULL),
	pFirstPersonCamera_(NULL),
	zenithMinimumHeight_(0.0),
	zenithMaximumHeight_(1.0),
	zenithMinimumEarHeight_(0.0),
	zenithMaximumEarHeight_(1.0),
	cameraMoved_( false ),
	pGroundConstraint_( NULL ),
	pZenithConstraint_( NULL ),
	groundCameraMoved_( true )
{
 	pKeyTranslator_ = _NEW( DevKeyToCommandTranslator() );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAD_2, ZENITHVIEW, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAD_0, GROUNDVIEW, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F5, SAVEVIEW1, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F6, SAVEVIEW2, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F7, SAVEVIEW3, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F8, SAVEVIEW4, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F5, RESTOREVIEW1, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F6, RESTOREVIEW2, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F7, RESTOREVIEW3, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F8, RESTOREVIEW4, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_H, RESTOREDEFAULTCAMERAPOS, DevKeyToCommand::RELEASED, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_H, RESTORELASTCAMERAPOS, DevKeyToCommand::PRESSED, DevKeyToCommand::PRESSED, DevKeyToCommand::RELEASED ) );

	pKeyTranslator_->initEventQueue();

    TEST_INVARIANT;
}

MachCameras::~MachCameras()
{
    TEST_INVARIANT;

	DEBUG_STREAM( DIAG_NEIL, "MachCameras::~MachCameras() enter" << std::endl );

	_DELETE( pKeyTranslator_ );
	_DELETE( pGroundControl_ );
	_DELETE( pFreeControl_ );
	_DELETE( pZenithControl_ );
	_DELETE( pFirstPersonControl_ );
	_DELETE( pGroundCamera_ );
	_DELETE( pFreeCamera_ );
	_DELETE( pZenithCamera_ );
	_DELETE( pFirstPersonCamera_ );

	DEBUG_STREAM( DIAG_NEIL, "MachCameras::~MachCameras() leave" << std::endl );
}

void MachCameras::CLASS_INVARIANT
{
   	INVARIANT( this != NULL );
	INVARIANT(pSceneManager_->currentCamera() == pCurrentCamera_);
}

void MachCameras::loadGame()
{
	MachLogRaces& races = MachLogRaces::instance();
    MachPhys::Race playerRace = races.pcController().race();

	MexTransform3d eyeTransform( 	MexEulerAngles( MexDegrees( 45 ), 0.0, 0.0 ) ,
									MexPoint3d( 0.0, 0.0, 2.0 ) );
	MexTransform3d groundTransform( MexEulerAngles( MexDegrees( 111 ), 0.0, 0.0 ),
									MexPoint3d( 150.0, 150.0, 1.5 ) );

    MATHEX_SCALAR  zenithMinHeight = 20.0;
    MATHEX_SCALAR  zenithMaxHeight = 250.0;
    MATHEX_SCALAR  zenithElevation = 60.0;
    MATHEX_SCALAR  zenithMinEarHeight = 20.0;
    MATHEX_SCALAR  zenithMaxEarHeight = 250.0;

    readZenithDataFile( &zenithMinHeight, &zenithMaxHeight, &zenithElevation, &zenithMinEarHeight, &zenithMaxEarHeight );

	MexTransform3d zenithTransform( MexEulerAngles( MexDegrees( -120 ), MexDegrees( zenithElevation ), 0.0 ),
									MexPoint3d( 150.0, 150.0, 100.0 ) );

	zenithMinimumHeight_ = zenithMinHeight;
	zenithMaximumHeight_ = zenithMaxHeight;
	zenithMinimumEarHeight_ = zenithMinEarHeight;
	zenithMaximumEarHeight_ = zenithMaxEarHeight;

	switch ( races.cameraInfo( playerRace ).type_ )
	{
		case MachLogCamera::GROUND:
			groundTransform = races.cameraInfo( playerRace ).position_;
			break;
		case MachLogCamera::ZENITH:
			zenithTransform = races.cameraInfo( playerRace ).position_;
			break;
		DEFAULT_ASSERT_BAD_CASE( (int)races.cameraInfo( playerRace ).type_);
	}

	pFirstPersonCamera_	= _NEW(MachLogCamera(pSceneManager_, pRoot_, eyeTransform, 	MachLogCamera::FIRST_PERSON));
	pGroundCamera_  	= _NEW(MachLogCamera(pSceneManager_, pRoot_, groundTransform, MachLogCamera::GROUND));
	pZenithCamera_  	= _NEW(MachLogCamera(pSceneManager_, pRoot_, zenithTransform, MachLogCamera::ZENITH));
	pFreeCamera_		= _NEW(MachLogCamera(pSceneManager_, pRoot_, groundTransform, MachLogCamera::FREE_MOVING));

	pGroundConstraint_	= _NEW(MachLogGroundCameraMotionConstraint( pGroundCamera_ ));
	pZenithConstraint_ 	= _NEW(MachLogZenithCameraMotionConstraint( pZenithCamera_ ) );

	pGroundControl_ 	= _NEW(PhysGroundFlyControl( _NEW( W4dMotionControlledEntity( pGroundCamera_ ) ), pGroundConstraint_ ) );
	pFirstPersonControl_= _NEW(PhysFlyControl( _NEW( W4dMotionControlledEntity( pFirstPersonCamera_ ) ) ) );
	pFreeControl_ 		= _NEW(PhysFlyControl( _NEW( W4dMotionControlledEntity( pFreeCamera_ ) ), _NEW( MachLogPlanetCameraConstraint() ) ) );

    pZenithConstraint_->minHeight( zenithMinHeight );
    pZenithConstraint_->maxHeight( zenithMaxHeight );

	pZenithControl_ = _NEW( PhysZenithFlyControl( _NEW( W4dMotionControlledEntity( pZenithCamera_ ) ), pZenithConstraint_ ) );

	// override standard key translator for now because currently keys do not control machine.
	pFirstPersonControl_->setKeyTranslator( _NEW( DevKeyToCommandTranslator() ) );

	if ( races.cameraInfo( playerRace ).type_ == MachLogCamera::ZENITH )
	{
	    // Initially use the zenith camera.
		pZenithControl_->enableInput();
		pCurrentCamera_ = pZenithCamera_;
		pSceneManager_->useCamera( pZenithCamera_ );
        reduceFog();
	}
	else
	{
	    // Initially use the ground camera.
		pGroundControl_->enableInput();
		pCurrentCamera_ = pGroundCamera_;
		pSceneManager_->useCamera( pGroundCamera_ );
        restoreFog();
	}

	pFreeControl_->metresPerSecond(1);
	pFreeControl_->degreesPerSecond(10);
	pZenithControl_->metresPerSecond(10);

    //Ensure the cameras are in the correct domain
    pGroundCamera_->update();
    pFreeCamera_->update();
    pZenithCamera_->update();

	pGroundControl_->initEventQueue();
	pFreeControl_->initEventQueue();
	pZenithControl_->initEventQueue();
	pFirstPersonControl_->initEventQueue();

	// Fill save camera structures with info about initial camera position
	saveCamera( &save1_ );
	saveCamera( &save2_ );
	saveCamera( &save3_ );
	saveCamera( &save4_ );
	saveCamera( &startCameraPos_ );
	saveCamera( &lastCameraPos_ );

	cameraMoved_ = false;
	groundCameraMoved_ = true;
}

void MachCameras::saveGame( PerOstream& outStream )
{
	if ( isGroundCameraActive() )
	{
		// Save camera type
		MachLogCamera::Type cameraType = MachLogCamera::GROUND;
		PER_WRITE_RAW_OBJECT( outStream, cameraType );
		MexTransform3d trans = pGroundCamera_->globalTransform();
		// Save camera pos
		PER_WRITE_RAW_OBJECT( outStream, trans );
	}
	else
	{
		// Save camera type
		MachLogCamera::Type cameraType = MachLogCamera::ZENITH;
		PER_WRITE_RAW_OBJECT( outStream, cameraType );

		// Save camera position data
        MATHEX_SCALAR zoomDistance;
        MATHEX_SCALAR x;
        MATHEX_SCALAR y;
        MexRadians heading;
        pZenithConstraint_->cameraPositionData( &zoomDistance, &x, &y, &heading );
		PER_WRITE_RAW_OBJECT( outStream, zoomDistance );
		PER_WRITE_RAW_OBJECT( outStream, x );
		PER_WRITE_RAW_OBJECT( outStream, y );
		PER_WRITE_RAW_OBJECT( outStream, heading );
	}

	// Write out saved camera positions
	PER_WRITE_RAW_OBJECT( outStream, save1_.saved_ );
	PER_WRITE_RAW_OBJECT( outStream, save1_.position_ );
	PER_WRITE_RAW_OBJECT( outStream, save2_.saved_ );
	PER_WRITE_RAW_OBJECT( outStream, save2_.position_ );
	PER_WRITE_RAW_OBJECT( outStream, save3_.saved_ );
	PER_WRITE_RAW_OBJECT( outStream, save3_.position_ );
	PER_WRITE_RAW_OBJECT( outStream, save4_.saved_ );
	PER_WRITE_RAW_OBJECT( outStream, save4_.position_ );
	PER_WRITE_RAW_OBJECT( outStream, startCameraPos_.saved_ );
	PER_WRITE_RAW_OBJECT( outStream, startCameraPos_.position_ );
	PER_WRITE_RAW_OBJECT( outStream, lastCameraPos_.saved_ );
	PER_WRITE_RAW_OBJECT( outStream, lastCameraPos_.position_ );
}

void MachCameras::loadSavedGame( PerIstream& inStream )
{
	loadGame();

	// Load camera type
	int cameraType;
	PER_READ_RAW_OBJECT( inStream, cameraType );

	if ( cameraType == MachLogCamera::GROUND )
	{
		// Switch to ground camera
		useGroundCamera();
		MexTransform3d trans;
		// Read camera pos
		PER_READ_RAW_OBJECT( inStream, trans );
		pGroundCamera_->globalTransform( trans );
	}
	else
	{
		// Switch to zenith camera
		useZenithCamera();

		// Read camera position data
        MATHEX_SCALAR zoomDistance;
        MATHEX_SCALAR x;
        MATHEX_SCALAR y;
        MexRadians heading;
		PER_READ_RAW_OBJECT( inStream, zoomDistance );
		PER_READ_RAW_OBJECT( inStream, x );
		PER_READ_RAW_OBJECT( inStream, y );
		PER_READ_RAW_OBJECT( inStream, heading );

        pZenithConstraint_->cameraPositionData( zoomDistance, x, y, heading );
	}

	// Read in saved camera positions
	PER_READ_RAW_OBJECT( inStream, (int&)save1_.saved_ );
	PER_READ_RAW_OBJECT( inStream, save1_.position_ );
	PER_READ_RAW_OBJECT( inStream, (int&)save2_.saved_ );
	PER_READ_RAW_OBJECT( inStream, save2_.position_ );
	PER_READ_RAW_OBJECT( inStream, (int&)save3_.saved_ );
	PER_READ_RAW_OBJECT( inStream, save3_.position_ );
	PER_READ_RAW_OBJECT( inStream, (int&)save4_.saved_ );
	PER_READ_RAW_OBJECT( inStream, save4_.position_ );
	PER_READ_RAW_OBJECT( inStream, (int&)startCameraPos_.saved_ );
	PER_READ_RAW_OBJECT( inStream, startCameraPos_.position_ );
	PER_READ_RAW_OBJECT( inStream, (int&)lastCameraPos_.saved_ );
	PER_READ_RAW_OBJECT( inStream, lastCameraPos_.position_ );
}

void MachCameras::unloadGame()
{
	_DELETE( pGroundControl_ );
	_DELETE( pFreeControl_ );
	_DELETE( pZenithControl_ );
	_DELETE( pFirstPersonControl_ );
	_DELETE( pGroundCamera_ );
	_DELETE( pFreeCamera_ );
	_DELETE( pZenithCamera_ );
	_DELETE( pFirstPersonCamera_ );
	pGroundControl_ = NULL;
	pFreeControl_ = NULL;
	pZenithControl_ = NULL;
	pFirstPersonControl_ = NULL;
	pGroundCamera_ = NULL;
	pFreeCamera_ = NULL;
	pZenithCamera_ = NULL;
	pFirstPersonCamera_ = NULL;

   	pCurrentCamera_ = NULL;
}

void MachCameras::useCamera(MachLogCamera* c)
{
	MachLogCamera* oldCamera = pCurrentCamera_;
	pCurrentCamera_ = c;
	pSceneManager_->useCamera(pCurrentCamera_);
	pCurrentCamera_->enable(oldCamera);

    TEST_INVARIANT;
}

MachLogCamera* MachCameras::currentCamera()
{
	PRE( pCurrentCamera_ != NULL );

	return pCurrentCamera_;
}

bool MachCameras::processButtonEvent( const DevButtonEvent& buttonEvent )
{
	typedef DevKeyToCommand::CommandId CommandId;

	// Give motion control chance to accept buttonEvent
	bool processed = PhysMotionControl::processButtonEvent( buttonEvent );

	// Inidicate that camera has moved so that it can be saved in the update method.
	if ( processed and ( isGroundCameraActive() or isZenithCameraActive() ) )
	{
		cameraMoved_ = true;
		groundCameraMoved_ = true;
	}

	if ( not processed and buttonEvent.action() == DevButtonEvent::PRESS  )
	{
		// Now process all commands that motion control is not interested in
		CommandId commandId;
		processed = pKeyTranslator_->translate( buttonEvent, &commandId );

		if ( processed )
		{
			switch ( commandId )
			{
				case ZENITHVIEW:
					useZenithCamera();
					break;
				case GROUNDVIEW:
					useGroundCamera();
					break;
				case FIRSTPERSONVIEW:
					use1stPersonCamera();
					break;
				case SAVEVIEW1:
					saveCamera( &save1_ );
					break;
				case SAVEVIEW2:
					saveCamera( &save2_ );
					break;
				case SAVEVIEW3:
					saveCamera( &save3_ );
					break;
				case SAVEVIEW4:
					saveCamera( &save4_ );
					break;
				case RESTOREVIEW1:
					restoreCamera( save1_ );
					break;
				case RESTOREVIEW2:
					restoreCamera( save2_ );
					break;
				case RESTOREVIEW3:
					restoreCamera( save3_ );
					break;
				case RESTOREVIEW4:
					restoreCamera( save4_ );
					break;
				case RESTOREDEFAULTCAMERAPOS:
					restoreCamera( startCameraPos_ );
					break;
				case RESTORELASTCAMERAPOS:
					restoreCamera( lastCameraPos_ );
					break;
			}
		}
	}

	return processed;
}

void MachCameras::switchToZenith( const MexPoint3d& lookAt )
{
	// Dense fog in the zenith view is silly because it obscures the
    // ground.  Cruft the value to make it less dense.
    reduceFog();

    pZenithControl_->snapTo( MexTransform3d( lookAt ) );
	useCamera( pZenithCamera_ );
	pZenithCamera_->update();
    pZenithControl_->enableInput();
}

void MachCameras::updateCameras()
{
	if ( isGroundCameraActive() )
	{
		pGroundControl_->update();
		pGroundCamera_->update();
	}
	else if ( is1stPersonCameraActive() )
	{
		pFirstPersonCamera_->update();
	}
	else if ( isZenithCameraActive() )
	{
		pZenithControl_->update();
		pZenithCamera_->update();
	}

	if ( cameraMoved_ )
	{
		cameraMoved_ = false;
		saveCamera( &lastCameraPos_ );
	}
}

void MachCameras::reduceFog()
{
    MachLogPlanet& malPlanet = MachLogPlanet::instance();

	if (malPlanet.hasSurface())
	{
	    MachPhysPlanetSurface* physPlanet = malPlanet.surface();

	    if (physPlanet->hasEnvironment())
	    {
		    EnvPlanetEnvironment& env = physPlanet->environment();
			env.fogZenith(true);
		}
	}
}

void MachCameras::restoreFog()
{
    MachLogPlanet& malPlanet = MachLogPlanet::instance();

	if (malPlanet.hasSurface())
	{
	    MachPhysPlanetSurface* physPlanet = malPlanet.surface();

	    if (physPlanet->hasEnvironment())
	    {
		    EnvPlanetEnvironment& env = physPlanet->environment();
			env.fogZenith(false);
		}
	}
}

void MachCameras::saveCamera( CameraSave* pCameraSave )
{
	if ( pGroundControl_->inputEnabled() )
	{
		pCameraSave->saved_ = CameraSave::GROUNDVIEW;
		pCameraSave->position_ = pGroundCamera_->globalTransform();
	}
	else if ( pZenithControl_->inputEnabled()  )
	{
		pCameraSave->saved_ = CameraSave::ZENITHVIEW;

        //We need tp store the actual zenith view parameters.
        //We do this using the transform.
        MATHEX_SCALAR zoomDistance;
        MATHEX_SCALAR x;
        MATHEX_SCALAR y;
        MexRadians heading;
        pZenithConstraint_->cameraPositionData( &zoomDistance, &x, &y, &heading );
        MexTransform3d zoomStoreTransform( MexEulerAngles( heading, 0.0, 0.0 ), MexPoint3d( x, y, zoomDistance ) );
		pCameraSave->position_ = zoomStoreTransform;
	}
}

void MachCameras::restoreCamera( const CameraSave& cameraSave )
{
	if ( cameraSave.saved_ == CameraSave::GROUNDVIEW )
	{
		restoreFog();

		pGroundControl_->snapTo( cameraSave.position_ );
		useCamera( pGroundCamera_ );
		pGroundCamera_->update();
		pGroundControl_->enableInput();
	}
	else if ( cameraSave.saved_ == CameraSave::ZENITHVIEW )
	{
		reduceFog();

        //Extract the zenith view parameters from the saved transform
        MATHEX_SCALAR zoomDistance = cameraSave.position_.position().z();
        MATHEX_SCALAR x = cameraSave.position_.position().x();
        MATHEX_SCALAR y = cameraSave.position_.position().y();
        MexEulerAngles angles;
        cameraSave.position_.rotation( &angles);
        MexRadians heading = angles.azimuth();

        //Use the zenith motion constraint to update the view position parameters
		pZenithConstraint_->cameraPositionData( zoomDistance, x, y, heading );

        //Make sure we use and update this camera
		useCamera( pZenithCamera_ );
		pZenithCamera_->update();
		pZenithControl_->enableInput();
	}
}

// Works out position of zenith camera so that the centre of the screen is the
// co-ords specified by lookAt.
MexPoint3d MachCameras::zenithLookAt( const MexPoint3d& lookAt )
{
	MexTransform3d zenithPos = pZenithCamera_->globalTransform();
	MATHEX_SCALAR zenithZ = zenithPos.position().z();

	MexVec3 xBasis = zenithPos.xBasis();
	MexVec3 zVec( 0, 0, 1 );
	MATHEX_SCALAR dotRes = xBasis.dotProduct( zVec );

	MATHEX_SCALAR scale = -zenithZ / dotRes;

	xBasis *= scale;

	MexPoint3d newZenithPos = lookAt;
	newZenithPos -= xBasis;

	return newZenithPos;
}

// Make camera move to new position ( assuming camera can move without
// ending up inside an obstacle etc ).
void MachCameras::moveTo( const MexPoint2d& newPos )
{
	pZenithControl_->snapTo( MexPoint3d( newPos.x(), newPos.y(), pZenithCamera_->globalTransform().position().z() ) );
	pGroundControl_->snapTo( MexPoint3d( newPos.x(), newPos.y(), pGroundCamera_->globalTransform().position().z() ) );
	pFreeControl_->snapTo( MexPoint3d( newPos.x(), newPos.y(), pFreeCamera_->globalTransform().position().z() ) );

	// Make sure camera domain is updated.
	updateCameras();
}

// Make camera move into a position where it can look at the MachActor.
void MachCameras::lookAt( const MachActor& actor )
{
	MexTransform3d actorTrans = actor.globalTransform();
	MexPoint3d actorPos = actorTrans.position();

	if ( isZenithCameraActive() )
	{
		switchToZenith( actorPos );
	}
	else if ( isGroundCameraActive() )
	{
		if ( actor.objectIsMachine() ) // Only stay in ground view if actor is a machine
		{
			// Don't want to position camera directly on top of the actor, so
			// work out a position just behind the actor
			MexTransform3d newPos;
			bool machineIsGlider = false;

			// To get gliders into view in ground camera we need to move further behind the machine
			if ( actor.asMachine().physMachine().machineData().locomotionType() == MachPhys::GLIDER )
			{
			   	actor.globalTransform().transform( MexPoint3d( -40.0, 0.0, 0.0 ), &newPos );
				machineIsGlider = true;
			}
			else
			{
			   	actor.globalTransform().transform( MexPoint3d( -25.0, 0.0, 0.0 ), &newPos );
			}

			// Check new position...
			if ( pGroundCamera_->newPositionIsValid( newPos ) )
			{
                //  Make sure that the roll and elevation for the ground camera are zero
                MexEulerAngles  angles;
                newPos.rotation( &angles );
                angles.roll( 0.0 );
                angles.elevation( 0.0 );
                newPos.rotation( angles );

				if ( machineIsGlider )
				{
					pGroundConstraint_->zTerrainDelta( pGroundConstraint_->maxHeight() );
				}
				else
				{
					pGroundConstraint_->zTerrainDelta( pGroundConstraint_->minHeight() + 1.0 );
				}

		   		pGroundControl_->snapTo( newPos );
				pGroundCamera_->update(); // Make sure camera is in correct domain
			}
			else // Ground camera couldn't be positioned behind the actor, switch to zenith instead
			{
				switchToZenith( actorPos );
			}
		}
		else // switch to zenith camera to view construction
		{
			switchToZenith( actorPos );
		}
	}
}

void MachCameras::scroll( ScrollDir scrollDir, const GuiMouseEvent& event )
{
	if ( pCurrentCamera_ == pZenithCamera_ )
	{
		switch ( scrollDir )
		{
			case LEFT:
				if ( event.rightButton() == Gui::PRESSED )
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::ROTATE_LEFT );
				else
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::SLIDE_LEFT );
				break;
			case RIGHT:
				if ( event.rightButton() == Gui::PRESSED )
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::ROTATE_RIGHT );
				else
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::SLIDE_RIGHT );
				break;
			case UP:
				if ( event.rightButton() == Gui::PRESSED )
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::DOWN );
				else
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::FOWARD );
				break;
			case DOWN:
				if ( event.rightButton() == Gui::PRESSED )
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::UP );
				else
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::BACKWARD );
				break;
		}
	}
	else if ( pCurrentCamera_ == pGroundCamera_ )
	{
			switch ( scrollDir )
		{
			case LEFT:
				if ( event.rightButton() == Gui::PRESSED )
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::SLIDE_LEFT );
				else
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::ROTATE_LEFT );
				break;
			case RIGHT:
				if ( event.rightButton() == Gui::PRESSED )
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::SLIDE_RIGHT );
				else
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::ROTATE_RIGHT );
				break;
			case UP:
				if ( event.rightButton() == Gui::PRESSED )
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::UP );
				else
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::FOWARD );
				break;
			case DOWN:
				if ( event.rightButton() == Gui::PRESSED )
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::DOWN );
				else
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::BACKWARD );
				break;
		}
	}
}

void MachCameras::scrollWithWheel(const Gui::ScrollState wheelDir, const double step)
{
    constexpr auto zoomIn  = Gui::ScrollState::SCROLL_UP;
    constexpr auto zoomOut = Gui::ScrollState::SCROLL_DOWN;

    if ( pCurrentCamera_ == pZenithCamera_ )
    {
        auto  zenithMotion     = PhysMotion{ pZenithControl_->motion_ };
        auto  zenithTransform  = MexTransform3d{ pZenithCamera_->globalTransform() };

        if (wheelDir == zoomIn)
        {
            double newClimb = std::fabs(zenithMotion.climb()) * -1.0;
            zenithMotion.climb(newClimb);
            zenithMotion.deltaClimb(step * -1.0);
        }
        else if (wheelDir == zoomOut)
        {
            double newClimb = std::fabs(zenithMotion.climb());
            zenithMotion.climb(newClimb);
            zenithMotion.deltaClimb(step);
        }

        pZenithControl_->pMotionConstraint_->move(zenithTransform, zenithMotion, step / pZenithControl_->metresPerSecond());
        pZenithCamera_->globalTransform(zenithTransform);
    }
    else if ( pCurrentCamera_ == pGroundCamera_ )
    {
        auto  groundMotion    = PhysMotion{ pGroundControl_->motion_ };
        auto  groundTransform = MexTransform3d{ pGroundCamera_->globalTransform() };

        // GROUND CAM is inverted directions
        if (wheelDir == zoomOut)
        {
            double newClimb = std::fabs(groundMotion.climb()) * -1.0;
            groundMotion.climb(newClimb);
            groundMotion.deltaClimb(step * -1.0);
        }
        else if (wheelDir == zoomIn)
        {
            double newClimb = std::fabs(groundMotion.climb());
            groundMotion.climb(newClimb);
            groundMotion.deltaClimb(step);
        }

        pGroundControl_->pMotionConstraint_->move(groundTransform, groundMotion, step / pGroundControl_->metresPerSecond());
        pGroundCamera_->globalTransform(groundTransform);
    }
}

bool MachCameras::isZenithCameraActive() const
{
	return ( pCurrentCamera_ == pZenithCamera_ );
}

bool MachCameras::isGroundCameraActive() const
{
	return ( pCurrentCamera_ == pGroundCamera_ );
}

bool MachCameras::is1stPersonCameraActive() const
{
	return 	( pCurrentCamera_ == pFirstPersonCamera_ );
}


void MachCameras::freezeMotion()
{
	if ( not pZenithControl_->motionFrozen() )
		pZenithControl_->freezeMotion();
	if ( not pGroundControl_->motionFrozen() )
		pGroundControl_->freezeMotion();
	if ( not pFreeControl_->motionFrozen() )
		pFreeControl_->freezeMotion();
}

void MachCameras::resumeMotion()
{
	if ( pZenithControl_->motionFrozen() )
		pZenithControl_->resumeMotion();
	if ( pGroundControl_->motionFrozen() )
		pGroundControl_->resumeMotion();
	if ( pFreeControl_->motionFrozen() )
		pFreeControl_->resumeMotion();
}

bool MachCameras::motionFrozen() const
{
	bool frozen = false;

	if ( isZenithCameraActive() )
		frozen = pZenithControl_->motionFrozen();
	else if ( isGroundCameraActive() )
		frozen = pGroundControl_->motionFrozen();
	else if ( is1stPersonCameraActive()	)
		frozen = pFirstPersonControl_->motionFrozen();

	return frozen;
}

ostream& operator <<( ostream& o, const MachCameras& t )
{

    o << "MachCameras " << (void*)&t << " start" << std::endl;
    o << "MachCameras " << (void*)&t << " end" << std::endl;

    return o;
}

//  TBD: Make this into a proper member function (don't want to touch
//  the header file at the moment). Alternatively we might drop the
//  settable heights and viewing angle anyway.

void readZenithDataFile( MATHEX_SCALAR* pZenithMinHeight, MATHEX_SCALAR* pZenithMaxHeight, MATHEX_SCALAR* pZenithElevation,
						 MATHEX_SCALAR* pZenithEarMinHeight, MATHEX_SCALAR* pZenithEarMaxHeight )
{
    const SysPathName fileName( "data/zenith.dat" );

    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< std::istream > pIstream;

    if( SysMetaFile::useMetaFile() )
    {
        //pIstream = _NEW( SysMetaFileIstream( metaFile, fileName, std::ios::text ) );
        pIstream = std::auto_ptr< std::istream >(
            _NEW( SysMetaFileIstream( metaFile, fileName, std::ios::in ) ) );
    }
    else
    {
        ASSERT_FILE_EXISTS( fileName.c_str() );
        //pIstream = _NEW( std::ifstream( fileName.c_str(), std::ios::text | std::ios::in ) );
        pIstream =  std::auto_ptr< std::istream >(
            _NEW( std::ifstream( fileName.c_str(), std::ios::in | std::ios::in ) ));
    }

    UtlLineTokeniser    tokeniser( *pIstream, fileName );

    while( not tokeniser.finished() )
    {
        ASSERT_INFO( tokeniser.tokens().size() );
        ASSERT( tokeniser.tokens().size() == 2, "" );

        MATHEX_SCALAR value = atof( tokeniser.tokens()[ 1 ].c_str() );

        if( tokeniser.tokens()[ 0 ] == "MIN_HEIGHT" )
        {
            *pZenithMinHeight = value;
        }
        else if( tokeniser.tokens()[ 0 ] == "MAX_HEIGHT" )
        {
            *pZenithMaxHeight = value;
        }
        else if( tokeniser.tokens()[ 0 ] == "ELEVATION" )
        {
            *pZenithElevation = value;
        }
        else if( tokeniser.tokens()[ 0 ] == "MIN_EAR_HEIGHT" )
        {
            *pZenithEarMinHeight = value;
        }
        else if( tokeniser.tokens()[ 0 ] == "MAX_EAR_HEIGHT" )
        {
            *pZenithEarMaxHeight = value;
        }
        else if( tokeniser.tokens()[ 0 ] == "SCALE_FACTOR_MAX" )
		{
			W4dSoundManager::instance().setMaxScaleFactor(value);
		}
		else if( tokeniser.tokens()[ 0 ] == "SCALE_FACTOR_MIN" )
		{
		    W4dSoundManager::instance().setMinScaleFactor(value);
		}
        else
        {
            ASSERT_INFO( tokeniser.tokens()[ 0 ] );
            ASSERT_FAIL( "Illegal token" );
        }

        tokeniser.parseNextLine();
    }
}

void MachCameras::lookAt( const MexPoint2d& newPos )
{
	if ( isZenithCameraActive() )
	{
		switchToZenith( newPos );
	}
	else if ( isGroundCameraActive() )
	{
	   	pGroundControl_->snapTo( MexPoint3d( newPos.x(), newPos.y(), pGroundCamera_->globalTransform().position().z() ) );
		pGroundCamera_->update(); // Make sure camera is in correct domain
	}
	else if ( is1stPersonCameraActive() )
	{
		pFirstPersonControl_->snapTo( MexPoint3d( newPos.x(), newPos.y(), pFirstPersonCamera_->globalTransform().position().z() ) );
		pFirstPersonCamera_->update();	// Make sure camera is in correct domain
	}
}

MATHEX_SCALAR MachCameras::zenithMinimumHeight() const
{
	return zenithMinimumHeight_;
}

MATHEX_SCALAR MachCameras::zenithMaximumHeight() const
{
	return zenithMaximumHeight_;
}

MATHEX_SCALAR MachCameras::zenithMinimumEarHeight() const
{
	return zenithMinimumEarHeight_;
}

MATHEX_SCALAR MachCameras::zenithMaximumEarHeight() const
{
	return zenithMaximumEarHeight_;
}

void MachCameras::use1stPersonCamera()
{
	useCamera( pFirstPersonCamera_ );
	pFirstPersonCamera_->update();
	pFirstPersonControl_->enableInput();
	restoreFog();
	groundCameraMoved_ = true;
}

void MachCameras::useGroundCamera()
{

    if( not isGroundCameraActive() )
    {
		// Switching from zenith camera, store it's position. If we switch straight
		// back to zenith then the camera will jump back to it's last position otherwise
		// we will have to calculate a new zenith camera position based on the ground
		// camera position which is less accurate.
		if ( isZenithCameraActive() )
		{
			saveCamera( &lastZenithPos_ );
			groundCameraMoved_ = false;
		}

    	// Get current camera position
		MexTransform3d cameraTrans = pCurrentCamera_->globalTransform();

		// Remove any roll and pitch ( caused by machine going up hill or exploding )
		MexEulerAngles angles = cameraTrans.rotationAsEulerAngles();
		angles.roll( 0 );
		angles.elevation( 0 );

		cameraTrans.rotation( angles );

		// Set ground camera to new position.
		pGroundControl_->snapTo( cameraTrans );
		useCamera( pGroundCamera_ );
		pGroundCamera_->update();
        pGroundControl_->enableInput();
        restoreFog();
    }

}

void MachCameras::useZenithCamera()
{
	if ( not isZenithCameraActive() )
	{
		if ( isGroundCameraActive() )
		{
			// If the ground camera hasn't moved then put zenith camera back to it's
			// last position, else work out a new zenith position based on the new ground
			// position.
			if ( not groundCameraMoved_ )
			{
				restoreCamera( lastZenithPos_ );
			}
			else
			{
				MexTransform3d newPos;
				// Get zenith camera position data
		        MATHEX_SCALAR zoomDistance;
		        MATHEX_SCALAR x;
		        MATHEX_SCALAR y;
		        MexRadians heading;
		        pZenithConstraint_->cameraPositionData( &zoomDistance, &x, &y, &heading );

		   		pCurrentCamera_->globalTransform().transform( MexPoint3d( zoomDistance * 0.7, 0.0, 0.0 ), &newPos );
				switchToZenith( newPos.position() );
			}
		}
		else
		{
			MexPoint3d newPos = pCurrentCamera_->globalTransform().position();
			switchToZenith( newPos );
		}
	}
}

Gui::Coord MachCameras::positionOnTerrainThatZenithCameraIsLookingAt() const
{
	PRE( isZenithCameraActive() );

	MATHEX_SCALAR zoomDistance;
    MATHEX_SCALAR x;
    MATHEX_SCALAR y;
    MexRadians heading;
    pZenithConstraint_->cameraPositionData( &zoomDistance, &x, &y, &heading );

	Gui::Coord pos( x, y );

	return pos;
}

void MachCameras::reversePitchUpDownKeys( bool newValue )
{
	pGroundControl_->reversePitchUpDownKeys( newValue );
}

/* End CAMERAS.CPP **************************************************/

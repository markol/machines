/*
 * C A M E R A S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/cameras.hpp"
#include "device/butevent.hpp"
#include "device/keytrans.hpp"
#include "mathex/point2d.hpp"
#include "mathex/poly2d.hpp"
#include "phys/mcfly.hpp"
#include "phys/mcground.hpp"
#include "phys/mczenith.hpp"
#include "phys/motchunk.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/mcentity.hpp"
#include "world4d/manager.hpp"
#include "world4d/envirnmt.hpp"
#include "world4d/root.hpp"
#include "machlog/camera.hpp"
#include "machlog/races.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/motconst.hpp"
#include "machphys/machine.hpp"
#include "machphys/locomoti.hpp"
#include "machphys/mcmovinf.hpp"

#include "render/device.hpp"
#include "utility/linetok.hpp"
#include "gui/event.hpp"

static void readZenithDataFile( MATHEX_SCALAR* pZenithMinHeight, MATHEX_SCALAR* pZenithMaxHeight, MATHEX_SCALAR* pZenithElevation );

// static
MachCameras& MachCameras::instance()
{
    static MachCameras instance_;
    return instance_;
}

MachCameras::MachCameras( void )
:	machineIndex_( 0 ),
	pThirdPerson_( NULL ),
   	pCurrentCamera_( NULL),
	pKeyTranslator_( NULL ),
	pGroundControl_( NULL ),
	pFreeControl_( NULL ),
	pZenithControl_( NULL ),
	pSuperHighZenithControl_( NULL ),
	pEyeControl_( NULL ),
	pGroundCamera_( NULL ),
	pFreeCamera_( NULL ),
	pZenithCamera_( NULL ),
	pSuperHighZenithCamera_( NULL ),
	pEyeCamera_( NULL ),
	pSceneManager_( NULL ),
	pRoot_( NULL )
{
}

// NOTE This method must be called before any other public method
void MachCameras::initialise( W4dSceneManager* pSceneManager, W4dRoot* pRoot )
{
	pSceneManager_ = pSceneManager;
	pRoot_ = pRoot;

 	pKeyTranslator_ = _NEW( DevKeyToCommandTranslator() );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F2, ZENITHVIEW, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F3, SUPERHIGHZENITHVIEW, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F1, GROUNDVIEW, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F4, FIRSTPERSONVIEW, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
//	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAD_3, THIRDPERSONVIEW, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
// 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F5, SAVEVIEW1, DevKeyToCommand::PRESSED, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
// 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F6, SAVEVIEW2, DevKeyToCommand::PRESSED, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
//	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F7, SAVEVIEW3, DevKeyToCommand::PRESSED, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
//	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F8, SAVEVIEW4, DevKeyToCommand::PRESSED, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
//	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F5, RESTOREVIEW1, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
//	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F6, RESTOREVIEW2, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
//	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F7, RESTOREVIEW3, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
//	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::F8, RESTOREVIEW4, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->initEventQueue();

	MexTransform3d eyeTransform( 	MexEulerAngles( MexDegrees( 45 ), 0.0, 0.0 ) ,
									MexPoint3d( 0.0, 0.0, 2.0 ) );
	MexTransform3d groundTransform( MexEulerAngles( MexDegrees( 111 ), 0.0, 0.0 ),
									MexPoint3d( 150.0, 150.0, 1.5 ) );

    MATHEX_SCALAR  zenithMinHeight = 20.0;
    MATHEX_SCALAR  zenithMaxHeight = 250.0;
    MATHEX_SCALAR  zenithElevation = 60.0;

    MATHEX_SCALAR  superHighZenithMinHeight = 20.0;
    MATHEX_SCALAR  superHighZenithMaxHeight = 50000.0;
    MATHEX_SCALAR  superHighZenithElevation = 90.0;

    readZenithDataFile( &zenithMinHeight, &zenithMaxHeight, &zenithElevation );

	MexTransform3d zenithTransform( MexEulerAngles( MexDegrees( -120 ), MexDegrees( zenithElevation ), 0.0 ),
									MexPoint3d( 150.0, 150.0, 100.0 ) );

	MexTransform3d superHighZenithTransform( MexEulerAngles( MexDegrees( -90 ), MexDegrees( superHighZenithElevation ), 0.0 ),
									MexPoint3d( 150.0, 150.0, 100.0 ) );

	pEyeCamera_		= _NEW(MachLogCamera(pSceneManager, pRoot, eyeTransform, 	MachLogCamera::THIRD_PERSON));
	pGroundCamera_  = _NEW(MachLogCamera(pSceneManager, pRoot, groundTransform, MachLogCamera::GROUND));
	pZenithCamera_  = _NEW(MachLogCamera(pSceneManager, pRoot, zenithTransform, MachLogCamera::ZENITH));
	pSuperHighZenithCamera_  = _NEW(MachLogCamera(pSceneManager, pRoot, superHighZenithTransform, MachLogCamera::ZENITH));
	pSuperHighZenithCamera_->yonClipDistance( 50000 );
	pFreeCamera_	= _NEW(MachLogCamera(pSceneManager, pRoot, groundTransform, MachLogCamera::FREE_MOVING));

	pGroundControl_ = _NEW( PhysGroundFlyControl( _NEW( W4dMotionControlledEntity( pGroundCamera_ ) ) ) );
	pEyeControl_ 	= _NEW( PhysFlyControl( 	  _NEW( W4dMotionControlledEntity( pEyeCamera_ ) ) ) );
	pFreeControl_ 	= _NEW( PhysFlyControl( 	  _NEW( W4dMotionControlledEntity( pFreeCamera_ ) ) ) );
	pZenithControl_ = _NEW( PhysZenithFlyControl( _NEW( W4dMotionControlledEntity( pZenithCamera_ ) ) ) );
	pSuperHighZenithControl_ = _NEW( PhysZenithFlyControl( _NEW( W4dMotionControlledEntity( pSuperHighZenithCamera_ ) ) ) );

	// override standard key translator for now because currently keys do not control machine.
	pEyeControl_->setKeyTranslator( _NEW( DevKeyToCommandTranslator() ) );

    // Initially use the ground camera.
	pGroundControl_->enableInput();
	pCurrentCamera_ = pGroundCamera_;
	pSceneManager->useCamera( pGroundCamera_ );

	pEyeControl_->metresPerSecond(2);
	pEyeControl_->degreesPerSecond(10);
	pFreeControl_->metresPerSecond(1);
	pFreeControl_->degreesPerSecond(10);
	pZenithControl_->metresPerSecond(10);
	pSuperHighZenithControl_->metresPerSecond(10);

    //Ensure the cameras are in the correct domain
    pEyeCamera_->update();
    pGroundCamera_->update();
    pFreeCamera_->update();
    pZenithCamera_->update();
    pSuperHighZenithCamera_->update();

	pGroundControl_->initEventQueue();
	pFreeControl_->initEventQueue();
	pZenithControl_->initEventQueue();
	pSuperHighZenithControl_->initEventQueue();
	pEyeControl_->initEventQueue();

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
	_DELETE( pSuperHighZenithControl_ );
	_DELETE( pEyeControl_ );
	_DELETE( pGroundCamera_ );
	_DELETE( pFreeCamera_ );
	_DELETE( pZenithCamera_ );
	_DELETE( pSuperHighZenithCamera_ );
	_DELETE( pEyeCamera_ );

	DEBUG_STREAM( DIAG_NEIL, "MachCameras::~MachCameras() leave" << std::endl );
}

void MachCameras::CLASS_INVARIANT
{
   	INVARIANT( this != NULL );
	INVARIANT(pSceneManager_->currentCamera() == pCurrentCamera_);
}

void MachCameras::useCamera(MachLogCamera* c)
{
	MachLogCamera* oldCamera = pCurrentCamera_;
	pCurrentCamera_ = c;
	pSceneManager_->useCamera(pCurrentCamera_);
//	pCurrentCamera_->update();
	pCurrentCamera_->enable(oldCamera);

    TEST_INVARIANT;
}

MachLogCamera* MachCameras::currentCamera()
{
	PRE( pCurrentCamera_ != NULL );

	return pCurrentCamera_;
}

MachLogCamera::Type MachCameras::currentCameraType()
{
	// Returns camera type base on which camera pCurrentCamera_ is pointing to
	// Note: Can only return a type that MachLogCamera knows about ( e.g. no SuperHighZenithMode )

	PRE( pCurrentCamera_ != NULL );

	MachLogCamera::Type cameraType;

	if ( pCurrentCamera_ == pGroundCamera_ )
	{
		cameraType = MachLogCamera::GROUND;
	}
	else if ( pCurrentCamera_ == pFreeCamera_ )
	{
		cameraType = MachLogCamera::FREE_MOVING;
	}
	else if ( pCurrentCamera_ == pZenithCamera_ )
	{
		cameraType = MachLogCamera::ZENITH;
	}
	else if ( pCurrentCamera_ == pEyeCamera_ )
	{
		cameraType = MachLogCamera::FIRST_PERSON;
	}
	else
	{
		// TBD change this to something more sensible
		cameraType = MachLogCamera::THIRD_PERSON;
	}
	return cameraType;
}

bool MachCameras::processButtonEvent( const DevButtonEvent& buttonEvent )
{
	typedef DevKeyToCommand::CommandId CommandId;

	// Give motion control chance to accept buttonEvent
	bool processed = PhysMotionControl::processButtonEvent( buttonEvent );
	if ( not processed )
		processed = MachLogMachineThirdPerson::processButtonEvent( buttonEvent );

	if ( not processed  )
	{
		// Now process all commands that motion control is not interested in
		CommandId commandId;
		processed = pKeyTranslator_->translate( buttonEvent, &commandId );

		if ( processed )
		{
			switch ( commandId )
			{
				case ZENITHVIEW:
					switchToZenithView();
					break;
				case SUPERHIGHZENITHVIEW:
					switchToSuperHighZenithView();
					break;
				case GROUNDVIEW:
					switchToGroundView();
					break;
				case THIRDPERSONVIEW:
					switchToThirdPersonView();
					break;
				case FIRSTPERSONVIEW:
					switchToFirstPersonView();
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

    pZenithControl_->snapTo( zenithLookAt( lookAt ) );
	useCamera( pZenithCamera_ );
	pZenithCamera_->update();
    pZenithControl_->enableInput();
}

void MachCameras::switchToSuperHighZenith( const MexPoint3d& lookAt )
{
	// Dense fog in the zenith view is silly because it obscures the
    // ground.  Cruft the value to make it less dense.
    reduceFog();

    pSuperHighZenithControl_->snapTo( zenithLookAt( lookAt ) );
	useCamera( pSuperHighZenithCamera_ );
	pSuperHighZenithCamera_->update();
    pSuperHighZenithControl_->enableInput();
}

void MachCameras::updateCameras()
{
	if ( pCurrentCamera_ == pGroundCamera_ )
	{
		pGroundControl_->update();
		pGroundCamera_->update();
	}
	else if ( pCurrentCamera_ == pEyeCamera_ )
	{
		pEyeControl_->update();
		check3rdCameraDomain();
		pEyeCamera_->update();
	}
	else if ( pCurrentCamera_ == pFreeCamera_ )
	{
		pFreeControl_->update();
		pFreeCamera_->update();
	}
	else if ( pCurrentCamera_ == pZenithCamera_ )
	{
		pZenithControl_->update();
		pZenithCamera_->update();
	}
	else if ( pCurrentCamera_ == pSuperHighZenithCamera_ )
	{
		pSuperHighZenithControl_->update();
		pSuperHighZenithCamera_->update();
	}
}

void MachCameras::switchThirdPerson( MachLogMachine* pNewMachine )
{
	PRE( pNewMachine );

	restoreFog();

	if ( pThirdPerson_ )
	{
		MachLogMachine* pCurrentMachine = _CONST_CAST( MachLogMachine*, pThirdPerson_->machine() );
	    //pCurrentMachine->isIn3rdPersonView( false );
	}

    //Set up a motion constraint for the camera to sit in 3rd person over the
	//next machine in the race
	pThirdPerson_ = _NEW( MachLogMachineThirdPerson( pNewMachine ) );  // Last pThirdPerson_ will be deleted by constraint
    PhysThirdPersonCameraConstraint* p3rdConstraint = _NEW( MachLogThirdPersonCameraConstraint(  pThirdPerson_, pEyeCamera_ ) );
    //pNewMachine->isIn3rdPersonView( true );
    pEyeControl_->imposeConstraint( p3rdConstraint );
	pEyeControl_->enableInput();
	pEyeControl_->update();
	useCamera( pEyeCamera_ );
	check3rdCameraDomain();
	pEyeCamera_->update();
}

MachLogMachine& MachCameras::currentMachine()
{
	MachLogRaces::Machines& machines = MachLogRaces::instance().machines();

	if( machineIndex_ >= machines.size() )
		machineIndex_ = 0;

	size_t counter = machineIndex_;
	MachLogMachine * pLogMachine = NULL;
	MachLogRaces::Machines::iterator it = machines.begin();
	for( ; it != machines.end(); ++it )
	{
		if( counter-- == 0 )
		{
			pLogMachine = *it;
			break;
		}
	}

	ASSERT( it != machines.end(), logic_error() );
	ASSERT( pLogMachine != NULL, logic_error() );

	return *pLogMachine;
}

void MachCameras::check3rdCameraDomain()
{
    W4dDomain* pDomain = NULL;

	if ( pThirdPerson_ )
	{
	    //Get the current machine, and check in 3rd person view
	    MachLogMachine* pMachine = _CONST_CAST( MachLogMachine*, pThirdPerson_->machine() );
	    /*if( pMachine->isIn3rdPersonView() )
	    {
	        //Get the motion sequencer, and see if using a space domain.
	        //If so this is the domain for the camera
	        MachLogMachineMotionSequencer& sequencer = pMachine->motionSeq();
	        if( sequencer.isUsingSpaceDomain() )
	            pDomain = &(sequencer.spaceDomain());

	    	//tell the camera whether to use it or not
			pEyeCamera_->forceDomain( pDomain );
	    } */
	}
}

void MachCameras::reduceFog()
{
	W4dEnvironment* env = pSceneManager_->environment();
	if (env)
		env->fogOn(false);
}

void MachCameras::restoreFog()
{
	W4dEnvironment* env = pSceneManager_->environment();
	if (env)
		env->fogOn(true);
}

void MachCameras::saveCamera( CameraSave* pCameraSave )
{
	if ( pGroundControl_->inputEnabled() )
	{
		pCameraSave->saved_ = CameraSave::GROUNDVIEW;
		pCameraSave->position_ = pGroundCamera_->globalTransform();
		pCameraSave->pMachine_ = NULL;
	}
	else if ( pZenithControl_->inputEnabled()  )
	{
		pCameraSave->saved_ = CameraSave::ZENITHVIEW;
		pCameraSave->position_ = pZenithCamera_->globalTransform();
		pCameraSave->pMachine_ = NULL;
	}
	else if ( pSuperHighZenithControl_->inputEnabled()  )
	{
		pCameraSave->saved_ = CameraSave::SUPERHIGHZENITHVIEW;
		pCameraSave->position_ = pSuperHighZenithCamera_->globalTransform();
		pCameraSave->pMachine_ = NULL;
	}
	else if ( pEyeControl_->inputEnabled()  )
	{
		pCameraSave->saved_ = CameraSave::THIRDPERSONVIEW;
		pCameraSave->pMachine_ = pThirdPerson_->machine();
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

		pZenithControl_->snapTo( cameraSave.position_ );
		useCamera( pZenithCamera_ );
		pZenithCamera_->update();
		pZenithControl_->enableInput();
	}
	else if ( cameraSave.saved_ == CameraSave::SUPERHIGHZENITHVIEW )
	{
		reduceFog();

		pSuperHighZenithControl_->snapTo( cameraSave.position_ );
		useCamera( pSuperHighZenithCamera_ );
		pSuperHighZenithCamera_->update();
		pSuperHighZenithControl_->enableInput();
	}
	else if ( cameraSave.saved_ == CameraSave::THIRDPERSONVIEW )
	{
		if ( MachLogRaces::instance().machineExists( cameraSave.pMachine_ ) )
		{
			switchThirdPerson( _CONST_CAST( MachLogMachine*, cameraSave.pMachine_ ) );
		}
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
	pSuperHighZenithControl_->snapTo( MexPoint3d( newPos.x(), newPos.y(), pSuperHighZenithCamera_->globalTransform().position().z() ) );
	pGroundControl_->snapTo( MexPoint3d( newPos.x(), newPos.y(), pGroundCamera_->globalTransform().position().z() ) );
	pFreeControl_->snapTo( MexPoint3d( newPos.x(), newPos.y(), pFreeCamera_->globalTransform().position().z() ) );
	pEyeControl_->snapTo( MexPoint3d( newPos.x(), newPos.y(), pEyeCamera_->globalTransform().position().z() ) );

	// Make sure camera domain is updated.
	updateCameras();
}

// Make camera move into a position where it can look at the MachActor.
void MachCameras::lookAt( const MachActor& actor )
{
	MexTransform3d actorTrans = actor.globalTransform();
	MexPoint3d actorPos = actorTrans.position();

	if ( pCurrentCamera_ == pZenithCamera_ )
	{
		switchToZenith( actorPos );
	}
	else if ( pCurrentCamera_ == pSuperHighZenithCamera_ )
	{
		switchToSuperHighZenith( actorPos );
	}
	else if ( pCurrentCamera_ == pGroundCamera_ )
	{
		if ( actor.objectIsMachine() ) // Only stay in ground view if actor is a machine
		{
			// Don't want to position camera directly on top of the actor, so
			// work out a position just behind the actor
			MexTransform3d newPos;
		   	actor.globalTransform().transform( MexPoint3d( -10.0, 0.0, 1.5 ), &newPos );

		   	pGroundControl_->snapTo( newPos );
			pGroundCamera_->update(); // Make sure camera is in correct domain
		}
		else // switch to zenith camera to view construction
		{
			switchToZenith( actorPos );
		}
	}
	else if ( pCurrentCamera_ == pEyeCamera_ )
	{
		if ( actor.objectIsMachine() ) // Only stay in 3rd person if actor is a machine
			switchThirdPerson( _CONST_CAST( MachLogMachine*, &actor.asMachine() ) );
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
				if ( event.isShiftPressed() )
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::ROTATE_LEFT );
				else
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::SLIDE_LEFT );
				break;
			case RIGHT:
				if ( event.isShiftPressed() )
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::ROTATE_RIGHT );
				else
					pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::SLIDE_RIGHT );
				break;
			case UP:
				pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::FOWARD );
				break;
			case DOWN:
				pZenithControl_->forceCommandOn( PhysMotionControlWithTrans::BACKWARD );
				break;
		}
	}
	else if ( pCurrentCamera_ == pSuperHighZenithCamera_ )
	{
		switch ( scrollDir )
		{
			case LEFT:
				if ( event.isShiftPressed() )
					pSuperHighZenithControl_->forceCommandOn( PhysMotionControlWithTrans::ROTATE_LEFT );
				else
					pSuperHighZenithControl_->forceCommandOn( PhysMotionControlWithTrans::SLIDE_LEFT );
				break;
			case RIGHT:
				if ( event.isShiftPressed() )
					pSuperHighZenithControl_->forceCommandOn( PhysMotionControlWithTrans::ROTATE_RIGHT );
				else
					pSuperHighZenithControl_->forceCommandOn( PhysMotionControlWithTrans::SLIDE_RIGHT );
				break;
			case UP:
				pSuperHighZenithControl_->forceCommandOn( PhysMotionControlWithTrans::FOWARD );
				break;
			case DOWN:
				pSuperHighZenithControl_->forceCommandOn( PhysMotionControlWithTrans::BACKWARD );
				break;
		}
	}
	else if ( pCurrentCamera_ == pGroundCamera_ )
	{
			switch ( scrollDir )
		{
			case LEFT:
				if ( event.isShiftPressed() )
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::SLIDE_LEFT );
				else
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::ROTATE_LEFT );
				break;
			case RIGHT:
				if ( event.isShiftPressed() )
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::SLIDE_RIGHT );
				else
					pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::ROTATE_RIGHT );
				break;
			case UP:
				pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::FOWARD );
				break;
			case DOWN:
				pGroundControl_->forceCommandOn( PhysMotionControlWithTrans::BACKWARD );
				break;
		}
	}
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

void readZenithDataFile( MATHEX_SCALAR* pZenithMinHeight, MATHEX_SCALAR* pZenithMaxHeight, MATHEX_SCALAR* pZenithElevation )
{
    SysPathName fileName( "zenith.dat" );

    if( fileName.existsAsFile() )
    {
        UtlLineTokeniser tokeniser( fileName );

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
            else
            {
                //ASSERT_INFO( tokeniser.tokens()[ 0 ] );
                //ASSERT_FAIL( "Illegal token" );
            }

            tokeniser.parseNextLine();
        }
    }
}

void MachCameras::switchToZenithView( void )
{
    if( pSceneManager_->currentCamera() != pZenithCamera_ )
    {
		MexPoint3d oldPos;
		if ( pCurrentCamera_ == pGroundCamera_ )
			oldPos = pCurrentCamera_->globalTransform().position();
		else if ( pCurrentCamera_ == pEyeCamera_ )
		{
			const MachPhysMachine& physMach = pThirdPerson_->machine()->physMachine();
			oldPos = physMach.globalTransform().position();
		}

		switchToZenith( oldPos );
    }
}

void MachCameras::switchToSuperHighZenithView( void )
{
    if( pSceneManager_->currentCamera() != pSuperHighZenithCamera_ )
    {
		MexPoint3d oldPos;
		if ( pCurrentCamera_ == pGroundCamera_ )
			oldPos = pCurrentCamera_->globalTransform().position();
		else if ( pCurrentCamera_ == pEyeCamera_ )
		{
			const MachPhysMachine& physMach = pThirdPerson_->machine()->physMachine();
			oldPos = physMach.globalTransform().position();
		}

		switchToSuperHighZenith( oldPos );
    }
}

void MachCameras::switchToGroundView( void )
{
    if( pCurrentCamera_ != pGroundCamera_ )
    {
        pGroundControl_->snapTo( pCurrentCamera_->globalTransform() );
		useCamera( pGroundCamera_ );
		pGroundCamera_->update();
        pGroundControl_->enableInput();
        restoreFog();
    }
}

void MachCameras::switchToThirdPersonView( void )
{
	// Only change third person once every 1/3 second.
	if ( lastThirdPersonChange_.time() > 0.333 )
	{
		machineIndex_ = ++machineIndex_ % MachLogRaces::instance().machines().size();
    	switchThirdPerson( &currentMachine() );
		lastThirdPersonChange_.time(0);
        restoreFog();
	}
}

void MachCameras::switchToFirstPersonView( void )
{
	useCamera( pFreeCamera_ );
	pFreeCamera_->update();
    pFreeControl_->enableInput();
	restoreFog();
}

/* End CAMERAS.CPP **************************************************/

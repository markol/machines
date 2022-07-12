/*
 * E D I T C A M R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/editcamr.hpp"
#include <fstream>
#include "device/butevent.hpp"
#include "system/pathname.hpp"
#include "world4d/domain.hpp"
#include "world4d/scenemgr.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/degrees.hpp"
#include "render/colour.hpp"
#include "machlog/camera.hpp"
#include "machlog/plandoms.hpp"
#include "planeted/cameras.hpp"
#include "planeted/camrmrkr.hpp"
#include "planeted/editactor.hpp"
#include "planeted/race.hpp"
#include "planeted/scenfile.hpp"

PedCameraEditor::PedCameraEditor(void )
:	pRedMarker_( NULL ),
	pBlueMarker_( NULL ),
	pGreenMarker_( NULL ),
	pYellowMarker_( NULL ),
	pCurrentMarker_( NULL ),
	race_( MachPhys::RED )
{

    TEST_INVARIANT;
}

// virtual
void PedCameraEditor::initialise( W4dSceneManager* pScene, MachLogPlanet* pPlanet )
{
	PedEditorMode::initialise( pScene, pPlanet );

	// Set all camera markers to be at the current camera position
	setMarkerToCamera( &pRedMarker_, MachPhys::RED );
	setMarkerToCamera( &pBlueMarker_, MachPhys::BLUE );
	setMarkerToCamera( &pGreenMarker_, MachPhys::GREEN );
	setMarkerToCamera( &pYellowMarker_, MachPhys::YELLOW );
	pCurrentMarker_ = pRedMarker_;
}

PedCameraEditor::~PedCameraEditor()
{
    TEST_INVARIANT;
}

void PedCameraEditor::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
void PedCameraEditor::processInput( const DevButtonEvent& devButtonEvent )
{
	// Process keyboard/mouse events

	if ( devButtonEvent.action() == DevButtonEvent::PRESS and active_ )
	{
		if ( devButtonEvent.scanCode() == DevKey::KEY_D )
		{
			processDrop();
		}
		else if	( devButtonEvent.scanCode() == DevKey::KEY_C )
		{
			pCurrentMarker_ = processChangeRace( pCurrentMarker_ );
			setCameraToMarker( *pCurrentMarker_ );
		}
	}
}

//virtual
void PedCameraEditor::changingMode()
{
	PedEditorMode::changingMode();
}

//virtual
void PedCameraEditor::preRenderUpdate()
{
}

//virtual
void PedCameraEditor::displayKeyboardCtrls()
{
	PRE( pSceneManager_ != NULL );

	pSceneManager_->out() << "D : mark camera position for this race" << std::endl;
	pSceneManager_->out() << "C: show next race's camera position" << std::endl;
}

//virtual
void PedCameraEditor::displayModeInfo()
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

	pSceneManager_->out() << "MODE : Camera Edit" << std::endl;
	pSceneManager_->out() << "Race: " << race_ << std::endl;
}

//virtual
void PedCameraEditor::readScnFile( PedScenarioFile& scenarioFile )
{
 	MachPhys::Race race = MachPhys::RED;
	MexPoint3d origin;

	for( uint nRace = 0; nRace < PedRace::nRaces(); ++nRace )
	{
		const PedScenarioFile::Camera& scnCamera = scenarioFile.camera( race );

		pCurrentMarker_ = setMarkerForRace( race );

		// Only reinitialise position if new position is not the origin
		if(	not ( scnCamera.transform.position() == origin ) )
		{
			W4dDomain* pDomain = MachLogPlanetDomains::pDomainAt( scnCamera.transform.position() );


			_DELETE( pCurrentMarker_ );

			// Calculate local transform from global transform and parent global transform
			MexTransform3d localResultTransform;
			const MexTransform3d parentGlobalTransform = pDomain->globalTransform();
			parentGlobalTransform.transformInverse( scnCamera.transform, &localResultTransform );

			RenColour markerColour = PedRace::colour( race );
			// Create new marker at local transform calculated above
			pCurrentMarker_ = _NEW( PedCameraMarker( pDomain, localResultTransform, scnCamera.type, markerColour ) );
			pCurrentMarker_->draw();
			race = PedRace::next( race );
		}
	}
}

//virtual
void PedCameraEditor::writeScnFile( PedScenarioFile& scenarioFile )
{
	static MachPhys::Race currentRace = MachPhys::RED;
	static PedCameraMarker* pCurrentMarker = pRedMarker_;
	for( uint nRace = 0; nRace < PedRace::nRaces(); ++nRace )
	{
		PedScenarioFile::Camera& scnCamera = scenarioFile.camera( currentRace );
		scnCamera.transform = pCurrentMarker->globalTransform();
		scnCamera.type = pCurrentMarker->cameraType();
	 	pCurrentMarker = processChangeRace( pCurrentMarker );
		currentRace = PedRace::next( currentRace );
	}
}

// virtual
void PedCameraEditor::activateMode()
{
	PedEditorMode::activateMode();
	pCurrentMarker_ = setMarkerForRace( race_ );
	setCameraToMarker( *pCurrentMarker_ );
}

ostream& operator <<( ostream& o, const PedCameraEditor& t )
{

    o << "PedCameraEditor " << (void*)&t << " start" << std::endl;
    o << "PedCameraEditor " << (void*)&t << " end" << std::endl;

    return o;
}

void PedCameraEditor::processDrop()
{
	setMarkerToCamera( &pCurrentMarker_, race_ );
}

PedCameraMarker* PedCameraEditor::processChangeRace( PedCameraMarker* pCurrentMarker )
{
	// Set pCurrentMarker to the next race - note race must rotate in the same order
	// as in PedRace::next(...)
	PedCameraMarker* pTempMarker;

	if ( pCurrentMarker == pRedMarker_ )
	{
		pTempMarker = pBlueMarker_;
		race_ = MachPhys::BLUE;
	}
	else if ( pCurrentMarker == pBlueMarker_ )
	{
		pTempMarker = pGreenMarker_;
		race_ = MachPhys::GREEN;
	}
	else if ( pCurrentMarker == pGreenMarker_ )
	{
		pTempMarker = pYellowMarker_;
		race_ = MachPhys::YELLOW;
	}
	else if ( pCurrentMarker == pYellowMarker_ )
	{
		pTempMarker = pRedMarker_;
		race_ = MachPhys::RED;
	}
	POST( pTempMarker != NULL );

	return pTempMarker;	// ok to do this as it is assigned to point to a valid member marker
}

void PedCameraEditor::setCameraToMarker( const PedCameraMarker& marker )
{
	// Set camera postion to be that of current camera marker
	// Set camera view mode
	switch( marker.cameraType() )
	{
		case( MachLogCamera::GROUND ):	MachCameras::instance().switchToGroundView(); break;
		case( MachLogCamera::ZENITH ): 	MachCameras::instance().switchToZenithView(); break;
	}
	const MexTransform3d& newCameraTransform = marker.globalTransform();
	MachLogCamera* pCurrentCamera = MachCameras::instance().currentCamera();
    W4dDomain* pDomain = pCurrentCamera->containingDomain();

    //Get the camera's current global 2d position, and hence the domain it should be in
    W4dDomain* pCorrectDomain = MachLogPlanetDomains::pDomainAt( newCameraTransform.position() );

    //If not attached to the right domain, attach it now
    if( pDomain != pCorrectDomain )
    {
        pCurrentCamera->attachTo( pCorrectDomain );
        pDomain = pCorrectDomain;
    }
	// Set camera postion to be that of current camera marker
	pCurrentCamera->globalTransform( newCameraTransform );
}

void PedCameraEditor::setMarkerToCamera( PedCameraMarker** ppCameraMarker, MachPhys::Race race )
{
	// Create a camera marker at the current camera position

	// Check camera mode is valid
	MachLogCamera::Type cameraType = MachCameras::instance().currentCameraType();
	if ( ( cameraType == MachLogCamera::GROUND ) or ( cameraType == MachLogCamera::ZENITH ) )
	{
		// Get the local transform for the camera
		MachLogCamera* pCurrentCamera = MachCameras::instance().currentCamera();
		const MexTransform3d&  cameraLocalTransform = pCurrentCamera->localTransform();

		// Get the camera's parent entity
		W4dEntity* pParent = pCurrentCamera->pParent();

		// Use the transform and parent to attach a camera marker in the correct position
		RenColour markerColour = PedRace::colour( race );
		_DELETE( *ppCameraMarker );
		*ppCameraMarker = _NEW( PedCameraMarker( pParent, cameraLocalTransform, cameraType, markerColour ) );
		(*ppCameraMarker)->draw();
	}
	else
	{
		// Remove old warnings
		warnings_.erase( warnings_.begin(), warnings_.end() );

		string msg("Warning : cannot set marker in current camera mode ");
		warnings_.push_back( msg );
	}
}

PedCameraMarker* PedCameraEditor::setMarkerForRace( MachPhys::Race race )
{
	PedCameraMarker* pMarker = pRedMarker_;
	switch( race )
	{
		case( MachPhys::RED ) 	:	pMarker = pRedMarker_; break;
		case( MachPhys::BLUE ) 	:	pMarker = pBlueMarker_; break;
		case( MachPhys::GREEN ) :	pMarker = pGreenMarker_; break;
		case( MachPhys::YELLOW ):	pMarker = pYellowMarker_; break;
	}
	return pMarker;
}

/* End EDITCAMR.CPP *************************************************/

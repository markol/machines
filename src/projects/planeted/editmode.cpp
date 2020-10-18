/*
 * E D I T M O D E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/editmode.hpp"
#include "mathex/transf3d.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/camera.hpp"
#include "render/device.hpp"

PedEditorMode::PedEditorMode()
: 	pSceneManager_(NULL),
	pPlanet_(NULL),
	active_(false)
{

    TEST_INVARIANT;
}

PedEditorMode::~PedEditorMode()
{
    TEST_INVARIANT;

}

void PedEditorMode::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedEditorMode& t )
{

    o << "PedEditorMode " << (void*)&t << " start" << std::endl;
    o << "PedEditorMode " << (void*)&t << " end" << std::endl;

    return o;
}

void PedEditorMode::initialise( W4dSceneManager* pSceneManager, MachLogPlanet* pPlanet )
{
	PRE( pPlanet != NULL );
	PRE( pSceneManager != NULL );

	pSceneManager_ = pSceneManager;
	pPlanet_ = pPlanet;
}

MexLine3d PedEditorMode::cameraThroughCursorLine( const MexPoint2d& cursorScreenPoint )
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

    // Get corresponding world pos from cursor pos
    MexPoint3d cursorWorldPos = pSceneManager_->pDevice()->screenToCamera( cursorScreenPoint );

    //Construct a line from the camera origin through the cursor point in world coordinates
    const MexTransform3d& cameraTransform = pSceneManager_->currentCamera()->globalTransform();
    MexPoint3d cameraOrigin( cameraTransform.position() );
    cameraTransform.transform( &cursorWorldPos );
    MexLine3d shortLine( cameraOrigin, cursorWorldPos );

    //Now create one of length 500000m
    MATHEX_SCALAR lineLength = 500000.0;
    MexPoint3d farPoint = shortLine.pointAtDistance( lineLength );

	TEST_INVARIANT;

    return MexLine3d( cameraOrigin, farPoint, lineLength );
}

// virtual
void PedEditorMode::activateMode( void )
{
	active_ = true;
}

// virtual
void PedEditorMode::changingMode( void )
{
	active_ = false;
}

// virtual
void PedEditorMode::validate()
{}

// virtual
void PedEditorMode::displayWarningMsgs()
{
	for (	WarningMsgs::iterator warnIter = warnings_.begin();
			warnIter != warnings_.end();
			++warnIter )
	{
		pSceneManager_->out() << *warnIter << std::endl;
	}
}


/* End EDITMODE.CPP *************************************************/

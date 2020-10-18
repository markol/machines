/*
 * I G C U R S 2 D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/igcurs2d.hpp"
#include "machgui/ingame.hpp"
#include "machgui/gui.hpp"

#include "gui/gui.hpp"
#include "world4d/scenemgr.hpp"
#include "render/animcurs.hpp"
#include "render/device.hpp"
#include "render/display.hpp"
#include "render/surface.hpp"
#include "system/pathname.hpp"

#include <stdlib.h>

MachInGameCursors2d::MachInGameCursors2d( MachInGameScreen* pInGameScreen )
:   pMenuCursor_( NULL ),
    pEnterBuildingCursor_( NULL ),
    pAttackCursor_( NULL ),
    pConstructCursor_( NULL ),
    pLocateToCursor_( NULL ),
    pPatrolCursor_( NULL ),
    pDeployCursor_( NULL ),
    pPickUpCursor_( NULL ),
	pScavengeCursor_( NULL ),
    pTransportCursor_( NULL ),
    pEnterAPCCursor_( NULL ),
    pFollowCursor_( NULL ),
	pDropLandMineCursor_( NULL ),
	pHealCursor_( NULL ),
	pDeconstructCursor_( NULL ),
	pRecycleCursor_( NULL ),
	pRepairCursor_( NULL ),
	pCaptureCursor_( NULL ),
	pJoinConstructCursor_( NULL ),
	pAssemblePointCursor_( NULL ),
	pScrollWestCursor_( NULL ),
	pScrollEastCursor_( NULL ),
	pScrollNorthCursor_( NULL ),
	pScrollSouthCursor_( NULL ),
	pScrollNorthWestCursor_( NULL ),
	pScrollNorthEastCursor_( NULL ),
	pScrollSouthWestCursor_( NULL ),
	pScrollSouthEastCursor_( NULL ),
	pChooseConstructionCursor_( NULL ),
	pTreacheryCursor_( NULL ),
	currentType_( MachGui::MOVETO_CURSOR ),
    pInGameScreen_( pInGameScreen ),
	cursorSize_( LARGECURSORS )
{
    //Load the cursor bitmaps
    loadCursors();

    TEST_INVARIANT;
}

MachInGameCursors2d::~MachInGameCursors2d()
{
    _DELETE( pMenuCursor_ );
    _DELETE( pMoveCursor_[LARGECURSORS] );
    _DELETE( pMoveCursor_[SMALLCURSORS] );
    _DELETE( pEnterBuildingCursor_ );
    _DELETE( pNoEntryCursor_[LARGECURSORS] );
    _DELETE( pNoEntryCursor_[SMALLCURSORS] );
    _DELETE( pAttackCursor_ );
    _DELETE( pConstructCursor_ );
    _DELETE( pLocateToCursor_ );
    _DELETE( pPatrolCursor_ );
    _DELETE( pDeployCursor_ );
    _DELETE( pPickUpCursor_ );
	_DELETE( pScavengeCursor_ );
    _DELETE( pTransportCursor_ );
    _DELETE( pEnterAPCCursor_ );
    _DELETE( pFollowCursor_ );
    _DELETE( pSelectCursor_[LARGECURSORS] );
	_DELETE( pSelectCursor_[SMALLCURSORS] );
    _DELETE( pDropLandMineCursor_ );
    _DELETE( pHealCursor_ );
	_DELETE( pDeconstructCursor_ );
	_DELETE( pRecycleCursor_ );
	_DELETE( pRepairCursor_ );
	_DELETE( pCaptureCursor_ );
	_DELETE( pJoinConstructCursor_ );
	_DELETE( pAssemblePointCursor_ );
	_DELETE( pScrollSouthCursor_ );
	_DELETE( pScrollNorthCursor_ );
	_DELETE( pScrollEastCursor_ );
	_DELETE( pScrollWestCursor_ );
	_DELETE( pScrollNorthWestCursor_ );
	_DELETE( pScrollNorthEastCursor_ );
	_DELETE( pScrollSouthWestCursor_ );
	_DELETE( pScrollSouthEastCursor_ );
	_DELETE( pChooseConstructionCursor_ );
	_DELETE( pTreacheryCursor_ );

	_DELETE( pIonAttackCursor_ );
	_DELETE( pNukeAttackCursor_ );

    TEST_INVARIANT;
}

void MachInGameCursors2d::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachInGameCursors2d& t )
{

    o << "MachInGameCursors2d " << (void*)&t << " start" << std::endl;
    o << "MachInGameCursors2d " << (void*)&t << " end" << std::endl;

    return o;
}

void MachInGameCursors2d::loadCursors()
{
    //Get the back buffer
    const GuiBitmap& backBuffer = Gui::backBuffer();

    pMenuCursor_ 			= loadCursor( "large/arrow", 	1 );
    pEnterBuildingCursor_ 	= loadCursor( "large/enter", 	6 );
    pAttackCursor_ 			= loadCursor( "large/attack", 	2 );
	pIonAttackCursor_ 		= loadCursor( "large/ioncan", 	10 );
    pConstructCursor_ 		= loadCursor( "large/arrow", 	1 );
    pLocateToCursor_ 		= loadCursor( "large/locat", 	5 );
    pPatrolCursor_ 			= loadCursor( "large/patrol", 	6 );
    pDeployCursor_ 			= loadCursor( "large/deploy", 	2 );
    pPickUpCursor_ 			= loadCursor( "large/pickup", 	2 );
	pScavengeCursor_		= loadCursor( "large/scav",   	10 );
    pTransportCursor_ 		= loadCursor( "large/putdn", 	2 );
    pEnterAPCCursor_ 		= loadCursor( "large/entapc", 	6 );
    pFollowCursor_ 			= loadCursor( "large/follow", 	4 );
    pDropLandMineCursor_ 	= loadCursor( "large/landmin",	2 );
    pHealCursor_ 			= loadCursor( "large/heal", 	6 );
	pDeconstructCursor_ 	= loadCursor( "large/deconst",	9 );
	pRecycleCursor_ 		= loadCursor( "large/recycle",	4 );
	pRepairCursor_ 			= loadCursor( "large/repair",	1 );
	pCaptureCursor_ 		= loadCursor( "large/capture",	8 );
	pJoinConstructCursor_ 	= loadCursor( "large/jconst", 	11 );
	pAssemblePointCursor_ 	= loadCursor( "large/assembl",	4 );
	pNukeAttackCursor_ 		= loadCursor( "large/nukattk",	5 );
	pScrollSouthCursor_		= loadCursor( "large/scrls",	1 );
	pScrollNorthCursor_		= loadCursor( "large/scrln",	1 );
	pScrollEastCursor_		= loadCursor( "large/scrle",	1 );
	pScrollWestCursor_		= loadCursor( "large/scrlw",	1 );
	pScrollNorthWestCursor_ = loadCursor( "large/scrlnw",	1 );
	pScrollNorthEastCursor_ = loadCursor( "large/scrlne",	1 );
	pScrollSouthWestCursor_ = loadCursor( "large/scrlsw",	1 );
	pScrollSouthEastCursor_ = loadCursor( "large/scrlse",	1 );
	pChooseConstructionCursor_ = loadCursor( "large/const", 11 );
	pTreacheryCursor_ 		= loadCursor( "large/treach", 	5 );

    pSelectCursor_[LARGECURSORS]		= loadCursor( "large/select", 1 );
	pSelectCursor_[SMALLCURSORS]		= loadCursor( "small/select", 1 );
    pNoEntryCursor_[LARGECURSORS] 		= loadCursor( "large/noentry",1 );
	pNoEntryCursor_[SMALLCURSORS] 		= loadCursor( "small/noentry",1 );
    pMoveCursor_[LARGECURSORS] 			= loadCursor( "large/move", 4 );
	pMoveCursor_[SMALLCURSORS] 			= loadCursor( "small/move", 4 );

    //The menu cursor and construct cursors has its origin at top left
    pMenuCursor_->origin( 0, 0 );
    pConstructCursor_->origin( 0, 0 );
	pScrollSouthCursor_->origin( 6, 13 );
	pScrollNorthCursor_->origin( 6, 0 );
	pScrollEastCursor_->origin( 13, 6 );
	pScrollWestCursor_->origin( 0, 6 );
	pScrollNorthWestCursor_->origin( 0, 0 );
	pScrollNorthEastCursor_->origin( 14,0 );
	pScrollSouthWestCursor_->origin( 0, 14 );
	pScrollSouthEastCursor_->origin( 14,14 );
	pIonAttackCursor_->origin( 27,38 );
	pScavengeCursor_->origin( 8,15 );
	pLocateToCursor_->origin( 8,14 );
	pTreacheryCursor_->origin( 4,4 );
}

RenAnimCursor2d* MachInGameCursors2d::loadCursor( const string& baseName, int nFrames )
{

	ASSERT( nFrames < 100, "No more than 99 frames allowed for animated cursors - this is the current arbitrary limit." );

    //Get the back buffer
    const GuiBitmap& backBuffer = Gui::backBuffer();

    //Create the main cursor
    RenAnimCursor2d* pCursor = _NEW( RenAnimCursor2d );

    //Create and add the surface for each frame
    int width = 0;
    int height = 0;
    //char index = '1';

	char buffer[3];

    for( int i = 0; i < nFrames; ++i )
    {
        //Construct the full name
        string name = "gui/cursor/";
        name += baseName;
        if( nFrames > 1 )
        {
			int frameNumber = i+1;
//			name += itoa( frameNumber, buffer, 10 );
            sprintf(buffer, "%d", frameNumber);
            name += buffer;
        }
        name += ".bmp";
        //++index;

        //Create the surface
        GuiBitmap surface = Gui::bitmap( SysPathName( name ) );
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

void MachInGameCursors2d::forceCursor( MachGui::Cursor2dType type )
{
    //decide which cursor to use
    RenAnimCursor2d* pNewCursor = NULL;

    switch( type )
    {
        case MachGui::MENU_CURSOR:
            pNewCursor = pMenuCursor_;
            break;
        case MachGui::INVALID_CURSOR:
            pNewCursor = pNoEntryCursor_[cursorSize_];
            break;
        case MachGui::MOVETO_CURSOR:
            pNewCursor = pMoveCursor_[cursorSize_];
            break;
        case MachGui::ATTACK_CURSOR:
            pNewCursor = pAttackCursor_;
            break;
		case MachGui::ION_ATTACK_CURSOR:
            pNewCursor = pIonAttackCursor_;
            break;
		case MachGui::NUKE_ATTACK_CURSOR:
            pNewCursor = pNukeAttackCursor_;
            break;
        case MachGui::CONSTRUCT_CURSOR:
            pNewCursor = pConstructCursor_;
            break;
        case MachGui::LOCATETO_CURSOR:
            pNewCursor = pLocateToCursor_;
            break;
        case MachGui::PATROL_CURSOR:
            pNewCursor = pPatrolCursor_;
            break;
        case MachGui::DEPLOY_CURSOR:
            pNewCursor = pDeployCursor_;
            break;
        case MachGui::PICKUP_CURSOR:
            pNewCursor = pPickUpCursor_;
            break;
		case MachGui::SCAVENGE_CURSOR:
            pNewCursor = pScavengeCursor_;
            break;
        case MachGui::TRANSPORT_CURSOR:
            pNewCursor = pTransportCursor_;
            break;
        case MachGui::ENTER_BUILDING_CURSOR:
            pNewCursor = pEnterBuildingCursor_;
            break;
        case MachGui::ENTER_APC_CURSOR:
            pNewCursor = pEnterAPCCursor_;
            break;
        case MachGui::FOLLOW_CURSOR:
            pNewCursor = pFollowCursor_;
            break;
        case MachGui::SELECT_CURSOR:
            pNewCursor = pSelectCursor_[cursorSize_];
            break;
        case MachGui::DROPLANDMINE_CURSOR:
            pNewCursor = pDropLandMineCursor_;
            break;
        case MachGui::HEAL_CURSOR:
            pNewCursor = pHealCursor_;
            break;
		case MachGui::DECONSTRUCT_CURSOR:
		    pNewCursor = pDeconstructCursor_;
            break;
		case MachGui::RECYCLE_CURSOR:
		    pNewCursor = pRecycleCursor_;
            break;
		case MachGui::REPAIR_CURSOR:
		    pNewCursor = pRepairCursor_;
            break;
		case MachGui::CAPTURE_CURSOR:
		    pNewCursor = pCaptureCursor_;
            break;
		case MachGui::JOINCONSTRUCT_CURSOR:
		    pNewCursor = pJoinConstructCursor_;
            break;
		case MachGui::ASSEMBLEPOINT_CURSOR:
		    pNewCursor = pAssemblePointCursor_;
            break;
		case MachGui::SCROLL_W_CURSOR:
		    pNewCursor = pScrollWestCursor_;
            break;
		case MachGui::SCROLL_E_CURSOR:
		    pNewCursor = pScrollEastCursor_;
            break;
		case MachGui::SCROLL_N_CURSOR:
		    pNewCursor = pScrollNorthCursor_;
            break;
		case MachGui::SCROLL_S_CURSOR:
		    pNewCursor = pScrollSouthCursor_;
            break;
		case MachGui::SCROLL_NW_CURSOR:
		    pNewCursor = pScrollNorthWestCursor_;
            break;
		case MachGui::SCROLL_NE_CURSOR:
		    pNewCursor = pScrollNorthEastCursor_;
            break;
		case MachGui::SCROLL_SE_CURSOR:
		    pNewCursor = pScrollSouthEastCursor_;
            break;
		case MachGui::SCROLL_SW_CURSOR:
		    pNewCursor = pScrollSouthWestCursor_;
            break;
		case MachGui::CHOOSE_CONST_CURSOR:
			pNewCursor = pChooseConstructionCursor_;
			break;
		case MachGui::TREACHERY_CURSOR:
			pNewCursor = pTreacheryCursor_;
			break;
    }

    //Get the display handle
    W4dSceneManager& sceneManager = pInGameScreen_->sceneManager();
    RenDisplay* pDisplay = sceneManager.pDevice()->display();

    //Use the selected cursor
    if( pNewCursor == NULL )
        pNewCursor = pMenuCursor_;

    pDisplay->useCursor( pNewCursor );

    //Store which one we are using
    currentType_ = type;
}

void MachInGameCursors2d::cursor( MachGui::Cursor2dType type )
{
    //Check not already using it
    if( type != currentType_ )
    {
		forceCursor( type );
    }
}

MachGui::Cursor2dType MachInGameCursors2d::type() const
{
    return currentType_;
}

void MachInGameCursors2d::cursorSize( CursorSize curSize )
{
	cursorSize_ = curSize;
}

/* End IGCURS2D.CPP *************************************************/

/*
 * E D I T T I L E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "planeted/edittile.hpp"
#include "planeted/availods.hpp"
#include "planeted/tilemrkr.hpp"

#include "device/keyboard.hpp"
#include "device/mouse.hpp"
#include "device/eventq.hpp"

#include "mathex/eulerang.hpp"
#include "mathex/quatern.hpp"

#include "world4d/domain.hpp"
#include "world4d/entity.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/camera.hpp"

#include "render/device.hpp"

#include "machlog/planet.hpp"

#include "machphys/plansurf.hpp"
#include "machphys/terrtile.hpp"
#include "machphys/tiledata.hpp"
#include "machphys/marker.hpp"

#include "ctl/list.hpp"

#include "utility/linetok.hpp"
#include "planeted/editart.hpp"

PedTileEditor::PedTileEditor( PedArtefactEditor& artefactEditor )
:   pHighlightTile_(NULL),
	artefactEditor_( artefactEditor )
{

    TEST_INVARIANT;

	POST( selectedTiles_.empty() );
}

PedTileEditor::~PedTileEditor()
{
    TEST_INVARIANT;

	// No need to delete pSelIndicator_ as this is done by it's parent
   for (SelectedTiles::iterator iter = selectedTiles_.begin();
		 iter != selectedTiles_.end();
		 ++iter )
	{
		SelectedTile* pSelTile = *iter;
		//_DELETE( pSelTile->pSelIndicator_ );
		_DELETE( pSelTile );
	}
}

void PedTileEditor::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedTileEditor& t )
{
    o << "PedTileEditor " << (void*)&t << " start" << std::endl;
    o << "PedTileEditor " << (void*)&t << " end" << std::endl;

    return o;
}

// virtual
void PedTileEditor::readCspFile( const SysPathName& )
{}

// virtual
void PedTileEditor::processInput( const DevButtonEvent& devButtonEvent )
{
	if ( devButtonEvent.action() == DevButtonEvent::PRESS and active_ )
	{
	    if ( devButtonEvent.scanCode() == DevKey::LEFT_MOUSE )
		{
			if ( not devButtonEvent.wasCtrlPressed() )
			{
				clearAllSelectedTiles();
			}
	      	processSelection( devButtonEvent );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_L )
		{
			processRotation( true );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_K )
		{
			processRotation( false );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_U )
		{
			processHeightChange( true, devButtonEvent.wasShiftPressed() );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_D )
		{
			processHeightChange( false, devButtonEvent.wasShiftPressed() );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_I )
		{
			processCycleTile( PREV );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_O )
		{
			processCycleTile( NEXT );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_P )
		{
			processCycleTile( CURRENT );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_C and
				  not devButtonEvent.wasShiftPressed() )
		{
			processAttatchCeilingArtefact( true );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_C and
				  devButtonEvent.wasShiftPressed() )
		{
			processAttatchCeilingArtefact( false );
		}
	}
}

void PedTileEditor::highlightTile( void )
{
	PRE( pSceneManager_ != NULL );

	// Clear last highlighted tile
	_DELETE( pHighlightTile_ );
	pHighlightTile_ = NULL;

	// Highlight tile that mouse pointer is over
	MexLine3d cursorLine = cameraThroughCursorLine( MexPoint2d( DevMouse::instance().position().first,
																DevMouse::instance().position().second ) );
	W4dDomain* pCameraDomain = pSceneManager_->currentCamera()->containingDomain();

	//Find any entity intersecting the line
	W4dEntity* pEntity;
	MATHEX_SCALAR distance;
	bool result = pCameraDomain->findNearerEntity( 	cursorLine,
													cursorLine.length(),
                                               		W4dEntity::nextCheckId(),
                                               		W4dEntity::MEDIUM,
                                               		&pEntity, &distance );

   	if( result and not isSelected( _REINTERPRET_CAST( MachPhysTerrainTile* , pEntity ) ) )
	{
		// Display a box around the selected tile
   		pHighlightTile_ = _NEW( 	PedTileMarker(  pEntity,
   													MexTransform3d(),
					  								pEntity->boundingVolume(),
					  								false ) );
	}
}

bool PedTileEditor::isSelected( MachPhysTerrainTile* pTile )
{
	for (SelectedTiles::iterator iter = selectedTiles_.begin();
		 iter != selectedTiles_.end();
		 ++iter )
	{
		SelectedTile* pSelTile = *iter;
		if (pSelTile->pSelectedTile_ == pTile)
		{
			return true;
		}
	}
	return false;
}

void PedTileEditor::clearAllSelectedTiles()
{
	for (SelectedTiles::iterator iter = selectedTiles_.begin();
		 iter != selectedTiles_.end();
		 ++iter )
	{
		SelectedTile* pSelTile = *iter;
		_DELETE( pSelTile->pSelIndicator_ );
		_DELETE( pSelTile );
	}

	selectedTiles_.erase( selectedTiles_.begin(), selectedTiles_.end() );
}

void PedTileEditor::processSelection( const DevButtonEvent& devButtonEvent )
{
	MexLine3d cursorLine = cameraThroughCursorLine( devButtonEvent.cursorCoords() );
	W4dDomain* pCameraDomain = pSceneManager_->currentCamera()->containingDomain();

	//Find any entity intersecting the line
	W4dEntity* pEntity;
	MATHEX_SCALAR distance;
	bool result = pCameraDomain->findNearerEntity( 	cursorLine,
													cursorLine.length(),
                                               		W4dEntity::nextCheckId(),
                                               		W4dEntity::MEDIUM,
                                               		&pEntity, &distance );
   	if( result )
	{
   		size_t xPos = 0;
		size_t yPos = 0;

		MachPhysPlanetSurface* pSurface = pPlanet_->surface();

		MexPoint3d terrPos( pSurface->xTileSpacing() / 2.0, pSurface->yTileSpacing() / 2.0, 0.0 );

		bool found = false;

		while ( not found )
		{
			MachPhysTerrainTile* pTile = pSurface->pTile( terrPos );

			if ( pTile == pEntity )
			{
		  		found = true;

				if ( not isSelected( pTile ) )
				{
					SelectedTile* pNewSelTile = _NEW( SelectedTile() );
					pNewSelTile->xPos_ = xPos;
					pNewSelTile->yPos_ = yPos;
					pNewSelTile->pSelectedTile_ = pTile;
					// Display a box around the selected tile
					pNewSelTile->pSelIndicator_ = _NEW( PedTileMarker( pTile,
	   													MexTransform3d(),
						  								pTile->boundingVolume(),
						  								true ) );


					selectedTiles_.push_back( pNewSelTile );
				}
				else // Remove selected tile
				{
					for (SelectedTiles::iterator iter = selectedTiles_.begin();
						 iter != selectedTiles_.end();
			 			 ++iter )
					{
						SelectedTile* pSelTile = *iter;
						if ( pSelTile->pSelectedTile_ == pTile )
						{
							_DELETE( pSelTile->pSelIndicator_ );
							_DELETE( pSelTile );
							selectedTiles_.erase( iter );
							break;
						}
					}
				}
			}
			else
			{
				++xPos;

				MexPoint3d startPoint( 0.0, 0.0, 0.0 );
				MexPoint3d endPoint( pSurface->xTileSpacing(), 0.0, 0.0 );

				MexVec3 nextTileVec( startPoint, endPoint );
				terrPos += nextTileVec;
				if ( xPos >= pSurface->nXTiles() )
				{
					++yPos;

					MexPoint3d endPoint( 0.0, pSurface->yTileSpacing(), 0.0 );

					MexVec3 nextTileVec( startPoint, endPoint );
					terrPos += nextTileVec;
					terrPos.x(  pSurface->xTileSpacing() / 2.0 );
					xPos = 0;
				}
			}
		}
	}
}

void PedTileEditor::processRotation( bool clockwise )
{
	// Ignore keypress if no selected tile
	if ( not selectedTiles_.empty() )
	{
		for (SelectedTiles::iterator iter = selectedTiles_.begin();
			 iter != selectedTiles_.end();
			 ++iter )
		{
			SelectedTile* pSelTile = *iter;

			MexTransform3d rotateTrans = pSelTile->pSelectedTile_->localTransform();
			MexTransform3d rotateZ90( MexEulerAngles( MexDegrees( clockwise ? -90 : 90 ), 0, 0 ) );
			rotateTrans.transform( rotateZ90 );
			pSelTile->pSelectedTile_->localTransform( rotateTrans );
		}
	}
}

void PedTileEditor::processHeightChange( bool up, bool shiftPressed )
{
	// Ignore keypress if no selected tiles
	if ( not selectedTiles_.empty() )
	{
		for (SelectedTiles::iterator iter = selectedTiles_.begin();
			 iter != selectedTiles_.end();
			 ++iter )
		{
			SelectedTile* pSelTile = *iter;

			int heightMag = shiftPressed ? 10 : 1;

			MexTransform3d heightTrans = pSelTile->pSelectedTile_->localTransform();
			MexTransform3d oldTrans = pSelTile->pSelectedTile_->localTransform();

			heightTrans.translate( MexPoint3d( 0, 0, up ? heightMag : -heightMag ) );
			pSelTile->pSelectedTile_->localTransform( heightTrans );

			MexPoint3d tileActualPos = pSelTile->pSelectedTile_->globalTransform().position();

			// Currently no minimum z-axis position...
			//if ( tileActualPos.z() < 0.0 )
			//{
			//	pSelectedTile_->localTransform( oldTrans );
			//}
		}
	}
}


void PedTileEditor::processCycleTile( CycleDir dir )
{
	PRE( pPlanet_ != NULL );

	// Ignore keypress if no selected tiles
	if ( not selectedTiles_.empty() )
	{
		SysPathName newLod;
		switch (dir)
		{
		case NEXT:
			newLod = PedAvailableTileLods::instance().next();
			break;
		case PREV:
			newLod = PedAvailableTileLods::instance().prev();
			break;
		case CURRENT:
			newLod = PedAvailableTileLods::instance().current();
			break;
		}

		for (SelectedTiles::iterator iter = selectedTiles_.begin();
			 iter != selectedTiles_.end();
			 ++iter )
		{
			SelectedTile*& pSelTile = *iter;

			SelectedTile* pTileReplacement = _NEW( SelectedTile() );

			_DELETE( pSelTile->pSelIndicator_ );
			pSelTile->pSelIndicator_ = NULL;

			MachPhysPlanetSurface* pSurface = pPlanet_->surface();

		   	pTileReplacement->pSelectedTile_ = pSurface->replaceTile( pSelTile->xPos_, pSelTile->yPos_, newLod );

	 		pTileReplacement->pSelIndicator_ = _NEW( 	PedTileMarker( pTileReplacement->pSelectedTile_,
	   													MexTransform3d(),
						  								pTileReplacement->pSelectedTile_->boundingVolume(),
						  								true ) );
			pTileReplacement->xPos_ = pSelTile->xPos_;
			pTileReplacement->yPos_ = pSelTile->yPos_;

			pSelTile = pTileReplacement;
		}
   	}
}

void PedTileEditor::displayTileCoords( void )
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

		if ( selectedTiles_.empty() )
	{
		pSceneManager_->out() << "No selected tiles" << std::endl;
	}
	else if ( selectedTiles_.size() == 1 )
	{
		SelectedTile* pOnlySelTile = selectedTiles_.front();

		// Display grid position of selected til
		pSceneManager_->out() << "Selected Tile (" << pOnlySelTile->xPos_ << "," << pOnlySelTile->yPos_ << ")" << std::endl;

		// Calculate and display height
		const MexTransform3d& localTrans = pOnlySelTile->pSelectedTile_->localTransform();
		const MexPoint3d& localPos = localTrans.position();
		pSceneManager_->out() << "Tile Height (" << localPos.z() << ")" << std::endl;

		// Calculate and display rotation
		MexEulerAngles eulerAngle = localTrans.rotationAsEulerAngles();
		MexDegrees 	   degrees = eulerAngle.azimuth();

		int intRotation = 0;
		if ( degrees.asScalar() > 85.0 and degrees.asScalar() < 95.0 )
			intRotation = 3;
		else if ( ( degrees.asScalar() > -185.0 and degrees.asScalar() < -175.0 ) or
		    	  ( degrees.asScalar() > 175.0 and degrees.asScalar() < 185.0 ) )
			intRotation = 2;
		else if ( degrees.asScalar() > -95.0 and degrees.asScalar() < -85.0 )
			intRotation = 1;

		pSceneManager_->out() << "Tile Rotation (" << intRotation << ")" << std::endl;

		// Display .lod file used
		pSceneManager_->out() << "Tile File (" <<  pOnlySelTile->pSelectedTile_->lodFileName() << ")" << std::endl;

		//pSceneManager_->out() << onlySelTile.pSelectedTile_->tileData();
	}
	else
	{
		pSceneManager_->out() << "More than one selected tile" << std::endl;
	}

	TEST_INVARIANT;
}

// virtual
void PedTileEditor::displayKeyboardCtrls( void )
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

 	pSceneManager_->out() << "I : previous tile" << std::endl;
	pSceneManager_->out() << "O : next tile" << std::endl;
	pSceneManager_->out() << "P : current tile" << std::endl;
	pSceneManager_->out() << "L : rotate tile clockwise" << std::endl;
	pSceneManager_->out() << "K : rotate tile anti-clockwise" << std::endl;
	pSceneManager_->out() << "U : tile up (1m)" << std::endl;
	pSceneManager_->out() << "D : tile down (1m)" << std::endl;
	pSceneManager_->out() << "Shift + U : tile up (10m)" << std::endl;
	pSceneManager_->out() << "Shift + D : tile down (10m)" << std::endl;

	TEST_INVARIANT;
}

// virtual
void PedTileEditor::preRenderUpdate( void )
{
	highlightTile();
}

// virtual
void PedTileEditor::displayModeInfo( void )
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

	pSceneManager_->out() << "MODE : Tile Edit" << std::endl;
	displayTileCoords();
	pSceneManager_->out() << "C (+ shift): create(hide) tile ceilings" << std::endl;

}

// virtual
void PedTileEditor::changingMode( void )
{
	PedEditorMode::changingMode();

	clearAllSelectedTiles();

	// Clear last highlighted tile
	_DELETE( pHighlightTile_ );
	pHighlightTile_ = NULL;
}

// virtual
void PedTileEditor::writeCspFile( std::ofstream& /*out*/ )
{}

void PedTileEditor::processAttatchCeilingArtefact( bool attatch )
{
	clearAllSelectedTiles();

	// Clear last highlighted tile
	_DELETE( pHighlightTile_ );
	pHighlightTile_ = NULL;

	MachPhysPlanetSurface* pSurface = pPlanet_->surface();
	MachPhysTerrainTile* terrainTile = NULL;

	size_t xTiles = pSurface->nXTiles();
	size_t yTiles = pSurface->nYTiles();

	for( uint x = 0; x < xTiles; ++x )
	{
		for( uint y = 0; y < yTiles; ++y )
		{
			const MachPhysTerrainTile& terrainTile = pSurface->tile( x, y );
			if( attatch )
			{
				// Delete ceiling artefact first if one exists
				artefactEditor_.deleteCeilingArtefact( terrainTile ) ;
				artefactEditor_.createCeilingArtefact( terrainTile ) ;
 			}
			else
			{
				artefactEditor_.deleteCeilingArtefact( terrainTile ) ;
			}
 		}
	}
}


/* End EDITTILE.CPP *************************************************/

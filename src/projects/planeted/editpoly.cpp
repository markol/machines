/*
 * E D I T P O L Y . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/editpoly.hpp"

#include "planeted/polygon.hpp"
#include "planeted/vetxmrkr.hpp"

#include "device/keyboard.hpp"
#include "device/eventq.hpp"
#include "device/mouse.hpp"

#include "mathex/eulerang.hpp"
#include "mathex/quatern.hpp"

#include "world4d/domain.hpp"
#include "world4d/entity.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/camera.hpp"

#include "render/device.hpp"

#include "machlog/planet.hpp"

#include "machphys/plansurf.hpp"

#include "utility/linetok.hpp"

PedPolygonEditor::PedPolygonEditor()
:	pHighlightVertex_(NULL),
	pSelectedVertex_(NULL),
	pSelectedPolygon_(NULL),
	pSelectedPoint_(NULL),
	pPastePolygon_(NULL),
	mouseDrag_(false),
	hidePolygons_(false),
	flattenPolygons_(false)
{

    TEST_INVARIANT;
}

PedPolygonEditor::~PedPolygonEditor()
{
    TEST_INVARIANT;

}

void PedPolygonEditor::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedPolygonEditor& t )
{

    o << "PedPolygonEditor " << (void*)&t << " start" << std::endl;
    o << "PedPolygonEditor " << (void*)&t << " end" << std::endl;

    return o;
}

void PedPolygonEditor::processInput( const DevButtonEvent& devButtonEvent )
{
	if ( devButtonEvent.action() == DevButtonEvent::PRESS and active_ )
	{
	    if ( devButtonEvent.scanCode() == DevKey::LEFT_MOUSE and
	    	 pHighlightVertex_ and
	    	 not mouseDrag_)
		{
			processSelectPolygon();
		}

		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_D and
		     		pHighlightVertex_ )
		{
			processDropPolygon();
		}
  		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_X and
			 		devButtonEvent.wasShiftPressed() and
					not devButtonEvent.wasCtrlPressed() and
		     		pSelectedPolygon_ )
		{
			// Delete polygon
			processDeletePolygon();
		}
	   	else if ( 	devButtonEvent.scanCode() == DevKey::KEY_X and
			 		not devButtonEvent.wasShiftPressed() and
					devButtonEvent.wasCtrlPressed() and
		     		pSelectedPolygon_ )
		{
			// Cut polygon
			copyVerticies_ = pSelectedPolygon_->verticies();
			processDeletePolygon();
		}

		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_I and
			 		devButtonEvent.wasShiftPressed()
			 		and not polygons_.empty() )
		{
			processPrevPolygon();
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_I and
			 		not devButtonEvent.wasShiftPressed()
			 		and not polygons_.empty() )
		{
			processPrevVertex();
		}
		else if	( 	devButtonEvent.scanCode() == DevKey::KEY_O and
			 		not devButtonEvent.wasShiftPressed()
			 		and not polygons_.empty() )
		{
			processNextVertex();
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_O and
			 		devButtonEvent.wasShiftPressed()
			 		and not polygons_.empty() )
		{
			processNextPolygon();
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_H )
		{
			if ( devButtonEvent.wasShiftPressed() )
				processPolygonRight();
			else
				processVertexRight();
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_G )
		{
			if ( devButtonEvent.wasShiftPressed() )
				processPolygonLeft();
			else
				processVertexLeft();
	 	}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_Y )
		{
			if ( devButtonEvent.wasShiftPressed() )
				processPolygonUp();
			else
				processVertexUp();
		}
	    else if ( devButtonEvent.scanCode() == DevKey::KEY_B )
	    {
			if ( devButtonEvent.wasShiftPressed() )
				processPolygonDown();
			else
				processVertexDown();
		}
		else if (	devButtonEvent.scanCode() == DevKey::KEY_V and
				    not devButtonEvent.wasShiftPressed() and
					devButtonEvent.wasCtrlPressed() )
		{
			processPaste( true );
		}
		else if (	devButtonEvent.scanCode() == DevKey::KEY_C and
				    not devButtonEvent.wasShiftPressed() and
					devButtonEvent.wasCtrlPressed() and
					pSelectedPolygon_ )
		{
			copyVerticies_ = pSelectedPolygon_->verticies();
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_F )
		{
			processFlattenPolygons();
		}
	}

	if (devButtonEvent.action() == DevButtonEvent::RELEASE and
	    devButtonEvent.scanCode() == DevKey::LEFT_MOUSE )
	{
		mouseDrag_ = false;
	}

	if (devButtonEvent.action() == DevButtonEvent::RELEASE and
		devButtonEvent.scanCode() == DevKey::KEY_V and
	   	pPastePolygon_ and
	   	active_ )
	{
		processPaste( false );
	}
}

void PedPolygonEditor::processPaste( bool tempPaste )
{
	_DELETE( pPastePolygon_ );
	pPastePolygon_ = NULL;

	if ( not copyVerticies_.empty() and
		 pHighlightVertex_ )
	{
		MexPoint2d dropPoint = pHighlightVertex_->position();
		MexPoint2d firstPoint = copyVerticies_[0];

		// Shift copied verticies to drop point
		MexVec2 moveVec( dropPoint.x() - firstPoint.x(), dropPoint.y() - firstPoint.y() );

		for (   PolyVerticies::iterator verIter = copyVerticies_.begin();
				verIter != copyVerticies_.end();
				++verIter )
		{
			(*verIter) += moveVec;
		}

		if ( tempPaste )
			pPastePolygon_ = createPolygon( copyVerticies_, 100, false );
		else
		{
			polygons_.push_back( createPolygon( copyVerticies_, 100, false ) );
			numberPolygons();
		}
	}
}

void PedPolygonEditor::highlightVertex( void )
{
	PRE( pSceneManager_ != NULL );

	// Clear last highlighted vertex
   	_DELETE( pHighlightVertex_ );
	pHighlightVertex_ = NULL;

	// Highlight vertex mouse cursor is over
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

   	if( result )
	{
		const MexTransform3d& globalTrans = pEntity->globalTransform();
		const MexPoint3d& globalPos = globalTrans.position();

			MexPoint3d intersectPoint = cursorLine.pointAtDistance( distance );
        MATHEX_SCALAR height = intersectPoint.z();

		size_t vx =  intersectPoint.x();
		size_t vy =  intersectPoint.y();

		vx -= vx % vertexSpacingX();
		vy -= vy % vertexSpacingY();

	   	pHighlightVertex_ = _NEW( PedVertexMarker(vx, vy, true /*, globalPos.z()*/ ) );
	}
}

// virtual
void PedPolygonEditor::preRenderUpdate( void )
{
	highlightVertex();
	updatePolygon();

	// Update position of temp paste polygon ( shows where a paste will occur )
	if ( pPastePolygon_ )
		processPaste( true );
}

void PedPolygonEditor::displayVertexCoords( void )
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

	if ( pHighlightVertex_ )
		pSceneManager_->out() << "Cursor Pos (" << pHighlightVertex_->position().x() << ","
							  << pHighlightVertex_->position().y() << ")" << std::endl;

	TEST_INVARIANT;
}

// virtual
void PedPolygonEditor::changingMode( void )
{
	PedEditorMode::changingMode();

	clearSelectedPolygon();

	// Clear last highlighted vertex
   	_DELETE( pHighlightVertex_ );
	pHighlightVertex_ = NULL;

	_DELETE (pPastePolygon_ );
	pPastePolygon_ = NULL;

	hidePolygons( hidePolygons_ );
}

void PedPolygonEditor::clearSelectedPolygon()
{
	// Remove old selected vertex
	_DELETE( pSelectedVertex_ );
	pSelectedVertex_ = NULL;

	if ( pSelectedPolygon_ )
	{
		// Set the polygon back to red
		pSelectedPolygon_->select( false );
		pSelectedPolygon_->refreshMesh();
		pSelectedPolygon_ = NULL;
		pSelectedPoint_ = NULL;
	}
}

// virtual
void PedPolygonEditor::processSelectPolygon()
{
	mouseDrag_ = true;
	bool selectNewPolygon = true;

	// Check to see if we are selecting the same vertex again...
	if ( pSelectedVertex_ )
	{
		if ( pSelectedVertex_->position() == pHighlightVertex_->position() )
		{
			selectNewPolygon = false;
		}
	}

	if ( selectNewPolygon )
   	{
   		clearSelectedPolygon();

		bool found = false;

		// Iterate through each polygon trying to find the polygon who's
		// vertex matches the one highlighted by the cursor
		for ( Polygons::iterator obIter = polygons_.begin();
			  obIter != polygons_.end() and not found;
			  ++obIter )
		{
			for ( PolyVerticies::iterator verIter = (*obIter)->verticies().begin();
				  verIter != (*obIter)->verticies().end() and not found;
				  ++verIter )
			{
				// Have we found the highlighted vertex?
				if ( *verIter == pHighlightVertex_->position() )
				{
					// Select the polygon and the vertex being pointed at...
 					pSelectedPolygon_ = *obIter;
					pSelectedPolygon_->select( true );
					pSelectedPolygon_->refreshMesh();

					pSelectedPoint_ = &(*verIter);
					pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );

					found = true;
	   			}
			}
		}
	}
}

void PedPolygonEditor::processDropPolygon()
{
	polygons_.push_back( createDefaultPolygon() );
}

void PedPolygonEditor::processDeletePolygon()
{
	for (	Polygons::iterator obIter = polygons_.begin();
			obIter != polygons_.end();
			++obIter )
	{
		if ( *obIter == pSelectedPolygon_ )
		{
			polygons_.erase( obIter );

			_DELETE( pHighlightVertex_ );
			pHighlightVertex_ = NULL;
			_DELETE( pSelectedPolygon_ );
			pSelectedPolygon_ = NULL;
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = NULL;
			pSelectedPoint_ = NULL;

			break;
   		}
	}
}

void PedPolygonEditor::processPrevPolygon()
{
	if ( not pSelectedPolygon_ )
	{
		selectFirstPolygon();
	}
	else
	{
		PedPolygon* pLastOb = polygons_.back();

		for (	Polygons::iterator obIter = polygons_.begin();
				obIter != polygons_.end();
				++obIter )
		{
			if ( *obIter == pSelectedPolygon_ )
			{
				clearSelectedPolygon();

				pSelectedPolygon_ = pLastOb;
				pSelectedPolygon_->select( true );
				pSelectedPolygon_->refreshMesh();

				pSelectedPoint_ = &pSelectedPolygon_->verticies()[0];
				pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );

				break;
			}

			pLastOb = *obIter;
		}
	}
}

void PedPolygonEditor::processPrevVertex()
{
	if ( not pSelectedPolygon_ )
	{
		selectFirstPolygon();
	}
	else
	{
		MexPoint2d* pLastVertex = &pSelectedPolygon_->verticies().back();

		for (	PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
				verIter != pSelectedPolygon_->verticies().end();
				++verIter )
		{
			if ( *verIter == pSelectedVertex_->position() )
			{
				_DELETE( pSelectedVertex_ );
				pSelectedPoint_ = pLastVertex;
				pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );

				break;
			}

			pLastVertex = &(*verIter);
		}
	}
}

void PedPolygonEditor::selectFirstPolygon()
{
	pSelectedPolygon_ = polygons_[0];
	pSelectedPolygon_->select( true );
	pSelectedPolygon_->refreshMesh();

	pSelectedPoint_ = &pSelectedPolygon_->verticies()[0];
	pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
}

void PedPolygonEditor::processNextPolygon()
{
	if ( not pSelectedPolygon_ )
	{
		selectFirstPolygon();
	}
	else
	{
		PedPolygon* pLastOb = polygons_.front();

		for (	Polygons::iterator obIter = polygons_.end();
				obIter != polygons_.begin(); /*intentionally empty*/ )
		{
			--obIter;
			if ( *obIter == pSelectedPolygon_ )
			{
				clearSelectedPolygon();

				pSelectedPolygon_ = pLastOb;
				pSelectedPolygon_->select( true );
				pSelectedPolygon_->refreshMesh();

				pSelectedPoint_ = &pSelectedPolygon_->verticies()[0];
				pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );

				break;
			}

			pLastOb = *obIter;
		}
	}
}

void PedPolygonEditor::processNextVertex()
{
	if ( not pSelectedPolygon_ )
	{
		selectFirstPolygon();
	}
	else
	{
		MexPoint2d* pLastVertex = &pSelectedPolygon_->verticies().front();

		for (	PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().end();
				verIter != pSelectedPolygon_->verticies().begin();
				/* intentionally empty */)
		{
			--verIter;
			if ( *verIter == pSelectedVertex_->position() )
			{
				_DELETE( pSelectedVertex_ );
				pSelectedPoint_ = pLastVertex;
				pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );

				break;
			}

			pLastVertex = &(*verIter);
		}
	}
}

void PedPolygonEditor::processPolygonRight()
{
	if ( pSelectedPolygon_ )
	{
		bool dryRunOk = true;

		// Check if shift is going to work
		for (   PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
				verIter != pSelectedPolygon_->verticies().end() and dryRunOk;
				++verIter )
		{
			MexPoint2d checkPoint = *verIter;
			checkPoint += MexVec2( vertexSpacingX(), 0 );
			if ( checkPoint.x() > pPlanet_->surface()->xMax() )
			{
				dryRunOk = false;
			}
		}

		// Now do for real ( if okay )
		if ( dryRunOk )
		{
			for (   PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
					verIter != pSelectedPolygon_->verticies().end();
					++verIter )
			{
				*verIter += MexVec2( vertexSpacingX(), 0 );
			}
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}
}

void PedPolygonEditor::processPolygonLeft()
{
	if ( pSelectedPolygon_ )
	{
		bool dryRunOk = true;

		// Check if shift is going to work
		for (   PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
				verIter != pSelectedPolygon_->verticies().end() and dryRunOk;
				++verIter )
		{
			MexPoint2d checkPoint = *verIter;
			checkPoint -= MexVec2( vertexSpacingX(), 0 );
			if ( checkPoint.x() < pPlanet_->surface()->xMin() )
			{
				dryRunOk = false;
			}
		}

		// Now do for real ( if okay )
		if ( dryRunOk )
		{
			for (   PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
					verIter != pSelectedPolygon_->verticies().end();
					++verIter )
			{
				*verIter -= MexVec2( vertexSpacingX(), 0 );
			}
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}
}

void PedPolygonEditor::processPolygonUp()
{
	if ( pSelectedPolygon_ )
	{
		bool dryRunOk = true;

		// Check if shift is going to work
		for (   PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
				verIter != pSelectedPolygon_->verticies().end() and dryRunOk;
				++verIter )
		{
			MexPoint2d checkPoint = *verIter;
			checkPoint -= MexVec2( 0, vertexSpacingY() );
			if ( checkPoint.y() < pPlanet_->surface()->yMin() )
			{
				dryRunOk = false;
			}
		}

		// Now do for real ( if okay )
		if ( dryRunOk )
		{
			for (   PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
					verIter != pSelectedPolygon_->verticies().end();
					++verIter )
			{
				*verIter -= MexVec2( 0, vertexSpacingY() );
			}
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}
}

void PedPolygonEditor::processPolygonDown()
{
	if ( pSelectedPolygon_ )
	{
		bool dryRunOk = true;

		// Check if shift is going to work
		for (   PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
				verIter != pSelectedPolygon_->verticies().end() and dryRunOk;
				++verIter )
		{
			MexPoint2d checkPoint = *verIter;
			checkPoint += MexVec2( 0, vertexSpacingY() );
			if ( checkPoint.y() > pPlanet_->surface()->yMax() )
			{
				dryRunOk = false;
			}
		}

		// Now do for real ( if okay )
		if ( dryRunOk )
		{
			for (   PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
					verIter != pSelectedPolygon_->verticies().end();
					++verIter )
			{
				*verIter += MexVec2( 0, vertexSpacingY() );
			}
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}
}

size_t PedPolygonEditor::vertexSpacingX() const
{
	MachPhysPlanetSurface* pSurface = pPlanet_->surface();
	return /*pSurface->xTileSpacing() /*/ pSurface->nTileXVertices();
}

size_t PedPolygonEditor::vertexSpacingY() const
{
	MachPhysPlanetSurface* pSurface = pPlanet_->surface();
	return /*pSurface->xTileSpacing() /*/ pSurface->nTileYVertices();
}

// virtual
void PedPolygonEditor::activateMode( void )
{
	PedEditorMode::activateMode();

	hidePolygons( false );
}

void PedPolygonEditor::hidePolygons( bool hide )
{
	for (	Polygons::iterator obIter = polygons_.begin();
			obIter != polygons_.end();
			++obIter )
	{
		(*obIter)->hide( hide );
	}
}

// virtual
void PedPolygonEditor::writeCspFile( std::ofstream& out )
{
	for (	Polygons::iterator polyIter = polygons_.begin();
			polyIter != polygons_.end();
			++polyIter )
	{
		(*polyIter)->save( out );
	}
}

void PedPolygonEditor::processFlattenPolygons( void )
{
	flattenPolygons_ = not flattenPolygons_;
	for (	Polygons::iterator polyIter = polygons_.begin();
			polyIter != polygons_.end();
			++polyIter )
	{
		(*polyIter)->flatten( flattenPolygons_ );
		(*polyIter)->refreshMesh();
	}
}

const PedPolygonEditor::Polygons& PedPolygonEditor::polygons() const
{
	return polygons_;
}

void PedPolygonEditor::numberPolygons()
{
	size_t count = 0;
	for ( Polygons::iterator polyIter = polygons_.begin();
		  polyIter != polygons_.end();
		  ++polyIter )
	{
		(*polyIter)->uniqueId( count++ );
	}
}

bool PedPolygonEditor::polygonsHidden() const
{
	return hidePolygons_;
}

/* End EDITPOLY.CPP *************************************************/

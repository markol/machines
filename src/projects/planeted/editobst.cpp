/*
 * E D I T O B S T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/editobst.hpp"
#include "stdlib/string.hpp"
#include <fstream>
#include "ctl/utility.hpp"
#include "utility/linetok.hpp"
#include "device/keyboard.hpp"
#include "device/eventq.hpp"
#include "device/mouse.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/quatern.hpp"
#include "mathex/transf3d.hpp"
#include "world4d/domain.hpp"
#include "world4d/entity.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/camera.hpp"
#include "render/device.hpp"
#include "machlog/planet.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/surfchk.hpp"
#include "planeted/vpolymark.hpp"
#include "planeted/vpolymark.hpp"
#include "planeted/vetxmrkr.hpp"

PedObstacleEditor::PedObstacleEditor()
: 	displayVerticalPolygons_( false )
{
    TEST_INVARIANT;
}

PedObstacleEditor::~PedObstacleEditor()
{
	//Remove all verticalPolygons marker that were dynamically created
	for( VerticalPolygons::iterator i = verticalPolygons_.begin(); i < verticalPolygons_.end(); ++i )
	{
		_DELETE( *i );
	}

    TEST_INVARIANT;

}

void PedObstacleEditor::initialise( W4dSceneManager* pSceneManager, MachLogPlanet* pPlanet )
{
	PRE( pPlanet != NULL );
	PRE( pSceneManager != NULL );

	PedEditorMode::initialise( pSceneManager, pPlanet );

	processComputeVerticalPolygons();
	processDisplayVerticalPolygons( false );
}

void PedObstacleEditor::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedObstacleEditor& t )
{

    o << "PedObstacleEditor " << (void*)&t << " start" << std::endl;
    o << "PedObstacleEditor " << (void*)&t << " end" << std::endl;

    return o;
}

void PedObstacleEditor::processInput( const DevButtonEvent& devButtonEvent )
{
	PedPolygonEditor::processInput( devButtonEvent );

	if ( devButtonEvent.action() == DevButtonEvent::PRESS and active_ )
	{
		if ( 	devButtonEvent.scanCode() == DevKey::KEY_V and
				not devButtonEvent.wasCtrlPressed() and
		   		pHighlightVertex_ and
		   		pSelectedVertex_ )
		{
			processInsertVertex();
	 	}
 		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_X and
			 		not devButtonEvent.wasShiftPressed() and
					not devButtonEvent.wasCtrlPressed() and
		     		pSelectedVertex_ )
		{
			processDeleteVertex();
	 	}
		else if ( 	devButtonEvent.scanCode() == DevKey::F7 and
					devButtonEvent.wasShiftPressed() and
					not devButtonEvent.wasCtrlPressed() and
		     		pSelectedPolygon_ )
		{
			pSelectedPolygon_->hide( not pSelectedPolygon_->hidden() );
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_K and
					not devButtonEvent.wasCtrlPressed() and
		     		pSelectedPolygon_ )
		{
			size_t heightChange = devButtonEvent.wasShiftPressed() ? 10 : 1;
			pSelectedPolygon_->height( 	pSelectedPolygon_->height() + heightChange );
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_L and
					not devButtonEvent.wasCtrlPressed() and
		     		pSelectedPolygon_ )
		{
			size_t heightChange = devButtonEvent.wasShiftPressed() ? 10 : 1;

			if ( pSelectedPolygon_->height() > heightChange )
			{
				pSelectedPolygon_->height( pSelectedPolygon_->height() - heightChange );
			}
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_P and
					not devButtonEvent.wasShiftPressed()	)
		{
			processDisplayVerticalPolygons( true );
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_P and
					devButtonEvent.wasShiftPressed()	)
		{
			processDisplayVerticalPolygons( false );
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_P and
					devButtonEvent.wasCtrlPressed()	)
		{
			processComputeVerticalPolygons();
			processDisplayVerticalPolygons( true );
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_N and
					not devButtonEvent.wasShiftPressed() )
		{
			PedObstacle::highlightedType( PedObstacle::NORMAL );
			if( pSelectedPolygon_ )
			{
				processChangeObstacleType();
			}
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_N and
					devButtonEvent.wasShiftPressed() )
		{
			PedObstacle::highlightedType( PedObstacle::NORMAL );
			highlightAllObstacles();
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_W and
					not devButtonEvent.wasShiftPressed() )
		{
			PedObstacle::highlightedType( PedObstacle::WATER );
			if( pSelectedPolygon_ )
			{
				processChangeObstacleType();
			}
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_W and
					devButtonEvent.wasShiftPressed() )
		{
			PedObstacle::highlightedType( PedObstacle::WATER );
			highlightAllObstacles();
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_E and
					not devButtonEvent.wasShiftPressed() )
		{
			PedObstacle::highlightedType( PedObstacle::LOW );
			if( pSelectedPolygon_ )
			{
				processChangeObstacleType();
			}
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_E and
					devButtonEvent.wasShiftPressed() )
		{
			PedObstacle::highlightedType( PedObstacle::LOW );
			highlightAllObstacles();
		}
	}
	if (devButtonEvent.action() == DevButtonEvent::PRESS and
	    devButtonEvent.scanCode() == DevKey::F7 and
	    devButtonEvent.wasCtrlPressed() and
		not devButtonEvent.wasShiftPressed() )
	{
		hidePolygons_ = not hidePolygons_;
		hidePolygons( hidePolygons_ );
	}
}

// virtual
void PedObstacleEditor::readCspFile( const SysPathName& cspFileName )
{
	UtlLineTokeniser parser( cspFileName );

 	//Read definitions until finished
    while( not parser.finished() )
    {
        ASSERT( parser.tokens().size() != 0, "" );

        if( parser.tokens()[ 0 ] == "OBSTACLE" )
        {
            ctl_vector< MexPoint2d > verticies;
			//Must have line like "OBSTACLE 4 35.2", indicating obstacle with
            //4 points and height 35.2
            ASSERT( parser.tokens().size() == 3, "" );

            size_t nPoints = atoi( parser.tokens()[ 1 ].c_str() );
            ASSERT( nPoints > 2, "" );

            MATHEX_SCALAR height = atof( parser.tokens()[ 2 ].c_str() );
			MATHEX_SCALAR one = 1;
            height = std::max( one, height );
            parser.parseNextLine();
			if( parser.tokens()[ 0 ] == "WATER" )
			{
				PedObstacle::highlightedType( PedObstacle::WATER );
				parser.parseNextLine();
			}
			else if( parser.tokens()[ 0 ] == "LOW_OBSTACLE" )
			{
				PedObstacle::highlightedType( PedObstacle::LOW );
				parser.parseNextLine();
			}
			else
			{
				PedObstacle::highlightedType( PedObstacle::NORMAL );
			}
            //Read each point, pushing a MexPoint2d onto a vector
            while( nPoints-- != 0 )
            {
                ASSERT( parser.tokens().size() == 2, ("Points not found ad line: " + std::to_string(parser.lineNumber())).c_str() );
                MATHEX_SCALAR x = atof( parser.tokens()[ 0 ].c_str() );
                MATHEX_SCALAR y = atof( parser.tokens()[ 1 ].c_str() );
                verticies.push_back( MexPoint2d( x, y ) );
                parser.parseNextLine();
            }

			polygons_.push_back( createPolygon( verticies, height, false ) );
			numberPolygons();
        }
		else
		{
			parser.parseNextLine();
		}
	}
	PedObstacle::highlightedType( PedObstacle::NORMAL );
	highlightAllObstacles();
}

// virtual
PedPolygon* PedObstacleEditor::createPolygon( const PolyVerticies& verticies, MATHEX_SCALAR height, bool selected ) const
{
	TEST_INVARIANT;

	PedPolygon* pNew = _NEW( PedObstacle( verticies, height, selected, PedObstacle::highlightedType() ) );
	pNew->flatten( flattenPolygons_ );
	pNew->refreshMesh();

	TEST_INVARIANT;

	return pNew;
}

// virtual
PedPolygon* PedObstacleEditor::createDefaultPolygon() const
{
	PRE( pHighlightVertex_ );

	PolyVerticies verticies;

	MexPoint2d newPoint =  pHighlightVertex_->position();

	verticies.push_back( newPoint );

	MachPhysPlanetSurface* pSurface = pPlanet_->surface();

	newPoint += MexVec2( 2 * vertexSpacingX(), 0 );
	verticies.push_back( newPoint );
	newPoint += MexVec2( 0, 2 * vertexSpacingY() );
	verticies.push_back( newPoint );
	newPoint -= MexVec2( 2 * vertexSpacingX(), 0 );
	verticies.push_back( newPoint );

	return createPolygon( verticies, 100, false );
}

// virtual
void PedObstacleEditor::displayKeyboardCtrls()
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

	const char* tab = "\t";

	pSceneManager_->out() << "Shift + F7 : hide obstacle" << std::endl;
	pSceneManager_->out() << "D : drop new obstacle (at cursor pos)" << std::endl;
	pSceneManager_->out() << "V : insert vertex (at cursor pos, after highlighted vertex)" << std::endl;
	pSceneManager_->out() << "X : delete vertex" << tab << "( + Shift : obstacle )" << std::endl;
	pSceneManager_->out() << "I : previous vertex" << tab << "( + Shift : obstacle )" << std::endl;
	pSceneManager_->out() << "O : next vertex" << tab << "( + Shift : obstacle )" << std::endl;
	pSceneManager_->out() << "Y : vertex up" << tab << "( + Shift : obstacle )" << std::endl;
	pSceneManager_->out() << "B : vertex down" << tab << "( + Shift : obstacle )" << std::endl;
	pSceneManager_->out() << "H : vertex right" << tab << "( + Shift : obstacle )" << std::endl;
	pSceneManager_->out() << "G : vertex left" << tab << "( + Shift : obstacle )" << std::endl;
	pSceneManager_->out() << "K : raise 1m" << tab << "( + Shift : 10m )" << std::endl;
	pSceneManager_->out() << "L : lower 1m" << tab << "( + Shift : 10m )" << std::endl;
	pSceneManager_->out() << "F : 2d/3d obstacles" << std::endl;
	pSceneManager_->out() << "P : Display vertical polygons ( + shift hide | + ctrl recompute )" << std::endl;
	pSceneManager_->out() << "W,E,N : Define obstacle as water, low or normal obstacle" << std::endl;
	pSceneManager_->out() << "W,E,N + shift : Show all obstacles of that type" << std::endl;

	TEST_INVARIANT;
}

// virtual
void PedObstacleEditor::displayModeInfo()
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

	pSceneManager_->out() << "MODE : Obstacle Edit" << std::endl;

	displayVertexCoords();

	if ( pSelectedPolygon_ )
		pSceneManager_->out() << "Obstacle " << pSelectedPolygon_->uniqueId() << std::endl;
	if( verticalPolygons_.size() )
	{
		pSceneManager_->out() << "VERTICAL POLYGONS FOUND" << std::endl;
	}
	if( displayVerticalPolygons_ )
	{
		for( VerticalPolygons::iterator i = verticalPolygons_.begin(); i < verticalPolygons_.end(); ++i )
		{
		 	pSceneManager_->out() << *(*i) << std::endl;
		}
	}
	pSceneManager_->out() << "Type: ";
	pSceneManager_->out() << PedObstacle::highlightedType() << std::endl;
}

void PedObstacleEditor::processInsertVertex()
{
	for (   PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
			verIter != pSelectedPolygon_->verticies().end();
			++verIter )
	{
		if ( *verIter == pSelectedVertex_->position() )
		{
			pSelectedPolygon_->verticies().insert( verIter, pHighlightVertex_->position() );
			pSelectedPolygon_->refreshMesh();
			break;
   		}
	}
}

void PedObstacleEditor::processDeleteVertex()
{
	if ( pSelectedPolygon_->verticies().size() > 3 )
	{
		for (	PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
				verIter != pSelectedPolygon_->verticies().end();
				++verIter )
		{
			if ( *verIter == pSelectedVertex_->position() )
			{
			 	pSelectedPolygon_->verticies().erase( verIter );
				//pSelectedObstacle_->refreshMesh(); // Taken care of below...
				clearSelectedPolygon();
				break;
	   		}
		}
	}
}

// virtual
void PedObstacleEditor::processVertexRight()
{
	// Ignore if no selected vertex
	if ( pSelectedVertex_ )
	{
		if ( pSelectedPoint_->x() != pPlanet_->surface()->xMax() )
		{
			*pSelectedPoint_ += MexVec2( vertexSpacingX(), 0 );
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}
}

// virtual
void PedObstacleEditor::processVertexLeft()
{
	// Ignore if no selected vertex
	if ( pSelectedVertex_ )
	{
		if ( pSelectedPoint_->x() !=  pPlanet_->surface()->xMin() )
		{
			*pSelectedPoint_ -= MexVec2( vertexSpacingX(), 0 );
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}
}

// virtual
void PedObstacleEditor::processVertexUp()
{
	// Ignore if no selected vertex
	if ( pSelectedVertex_ )
	{
		if ( pSelectedPoint_->y() != pPlanet_->surface()->yMin() )
		{
			*pSelectedPoint_ -= MexVec2( 0, vertexSpacingY() );
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}
}

// virtual
void PedObstacleEditor::processVertexDown()
{
	// Ignore if no selected vertex
	if ( pSelectedVertex_ )
	{
		if ( pSelectedPoint_->y() != pPlanet_->surface()->yMax() )
		{
			*pSelectedPoint_ += MexVec2( 0, vertexSpacingY() );
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}
}

void PedObstacleEditor::processDisplayVerticalPolygons( bool displayPolygons )
{
   	displayVerticalPolygons_ = displayPolygons;
	for( VerticalPolygons::iterator i = verticalPolygons_.begin(); i < verticalPolygons_.end(); ++i )
	{
		(*i)->display( displayPolygons );
	}
}

void PedObstacleEditor::highlightAllObstacles()
{
	for( PedPolygonEditor::Polygons::iterator polyIter = polygons_.begin(); polyIter != polygons_.end(); ++polyIter )
	{
		(*polyIter)->refreshMesh();
	}
}

// virtual
void PedObstacleEditor::updatePolygon()
{
	if (  mouseDrag_ and
		  pSelectedPolygon_ and
		  pHighlightVertex_ )
	{
		*pSelectedPoint_ = pHighlightVertex_->position();
		pSelectedPolygon_->refreshMesh();

		_DELETE( pSelectedVertex_ );
		pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
	}
}

// virtual
void PedObstacleEditor::validate()
{
	// Remove old warnings
	warnings_.erase( warnings_.begin(), warnings_.end() );
	int count = 0;

	for ( Polygons::iterator iter = polygons_.begin(); iter != polygons_.end(); ++iter )
	{
		if ( not (*iter)->isConvex() )
		{
			char buffer[20];
//			itoa( (*iter)->uniqueId(), buffer, 10 );
            sprintf(buffer, "%zu", (*iter)->uniqueId());
			string msg("Warning : obstacle ");
			msg += buffer;
			msg += " concave or anti-clockwise";
		 	warnings_.push_back( msg );
		}
	}
}

void PedObstacleEditor::processChangeObstacleType()
{
	PRE( pSelectedPolygon_ );

	// Naughty but unavoidable
   	PedObstacle* pObstacle = _REINTERPRET_CAST(PedObstacle*, pSelectedPolygon_ );

   	pObstacle->cspType( PedObstacle::highlightedType() );
}

void PedObstacleEditor::processComputeVerticalPolygons()
{
	//Remove all verticalPolygons marker that were dynamically created
	for( VerticalPolygons::iterator i = verticalPolygons_.begin(); i < verticalPolygons_.end(); ++i )
	{
		_DELETE( *i );
	}
	verticalPolygons_.erase( verticalPolygons_.begin(), verticalPolygons_.end() );

	// Check for vertical polygons
	MachPhysPlanetSurfaceChecker::Triangles triangleList;
   	MachPhysPlanetSurfaceChecker verticalPolygonChecker( *(pPlanet_->surface()) );
	verticalPolygonChecker.checkGradients( pPlanet_->configSpace(), &triangleList );
	for( MachPhysPlanetSurfaceChecker::Triangles::iterator i = triangleList.begin(); i < triangleList.end(); ++i )
	{
		verticalPolygons_.push_back( _NEW( PedVerticalPolygonMarker( MachLogPlanet::instance().pWorld(), MexTransform3d(), (*i).point1(), (*i).point2(), (*i).point3() ) ) );
	}
}


/* End EDITOBST.CPP *************************************************/

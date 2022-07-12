/*
 * E D I T D O M A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/editdoma.hpp"

#include "planeted/domain.hpp"
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
#include "utility/tostring.hpp"

PedDomainEditor::PedDomainEditor()
{

    TEST_INVARIANT;
}

PedDomainEditor::~PedDomainEditor()
{
    TEST_INVARIANT;

}

void PedDomainEditor::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedDomainEditor& t )
{

    o << "PedDomainEditor " << (void*)&t << " start" << std::endl;
    o << "PedDomainEditor " << (void*)&t << " end" << std::endl;

    return o;
}

// virtual
void PedDomainEditor::readCspFile( const SysPathName& cspFileName )
{
	UtlLineTokeniser parser( cspFileName );

	//Read definitions until finished
    while( not parser.finished() )
    {
        ASSERT( parser.tokens().size() != 0, "" );

        if( parser.tokens()[ 0 ] == "DOMAIN" )
        {
            PolyVerticies verticies;

          	//Must have line like "DOMAIN 4 88 88 262 291"
            //ie DOMAIN <index> <xmin> <ymin> <xmax> <ymax>
            ASSERT( parser.tokens().size() == 6, "" );

            //Read the boundary data
            MATHEX_SCALAR xMin = atof( parser.tokens()[ 2 ].c_str() );
            MATHEX_SCALAR yMin = atof( parser.tokens()[ 3 ].c_str() );
            MATHEX_SCALAR xMax = atof( parser.tokens()[ 4 ].c_str() );
            MATHEX_SCALAR yMax = atof( parser.tokens()[ 5 ].c_str() );
            ASSERT( xMin < xMax, "" );
            ASSERT( yMin < yMax, "" );

            parser.parseNextLine();

			verticies.push_back( MexPoint2d( xMin, yMin ) );
			verticies.push_back( MexPoint2d( xMax, yMin ) );
			verticies.push_back( MexPoint2d( xMax, yMax ) );
			verticies.push_back( MexPoint2d( xMin, yMax ) );

			polygons_.push_back( createPolygon( verticies, 100, false ) );
			numberPolygons();
        }
		else if( parser.tokens()[ 0 ] == "NEWDOMAIN" )
        {
            PolyVerticies verticies;

          	//Must have line like 	"NEWDOMAIN <uniqueId> <number of verticies>
          	//						"<x1> <y1> <x2> <y2> <x3> <y3> "

			// Ignore <uniqueId>, it will be reassigned.

            size_t nPoints = atoi( parser.tokens()[ 2 ].c_str() );
            ASSERT( nPoints > 2, "" );
			parser.parseNextLine();

            //Read each point, pushing a MexPoint2d onto a vector
            while( nPoints-- != 0 )
            {
                ASSERT( parser.tokens().size() == 2, "" );
                MATHEX_SCALAR x = atof( parser.tokens()[ 0 ].c_str() );
                MATHEX_SCALAR y = atof( parser.tokens()[ 1 ].c_str() );
                verticies.push_back( MexPoint2d( x, y ) );
                parser.parseNextLine();
            }

			polygons_.push_back( createPolygon( verticies, 100, false ) );
			numberPolygons();
        }
		else
		{
			parser.parseNextLine();
		}
	}
}

// virtual
void PedDomainEditor::processInput( const DevButtonEvent& devButtonEvent )
{
	PedPolygonEditor::processInput( devButtonEvent );

  	if ( devButtonEvent.action() == DevButtonEvent::PRESS and active_ )
	{
		if (	devButtonEvent.scanCode() == DevKey::F6 and
				devButtonEvent.wasShiftPressed() and
				not devButtonEvent.wasCtrlPressed() and
			    pSelectedPolygon_ )
		{
			pSelectedPolygon_->hide( not pSelectedPolygon_->hidden() );
		}
		else if (	devButtonEvent.scanCode() == DevKey::F6 and
			    	devButtonEvent.wasCtrlPressed() and
					not devButtonEvent.wasShiftPressed() )
		{
			hidePolygons_ = not hidePolygons_;
			hidePolygons( hidePolygons_ );
		}
		else if ( 	devButtonEvent.scanCode() == DevKey::KEY_V and
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
	}
}

// virtual
void PedDomainEditor::displayKeyboardCtrls()
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

	const char* tab = "\t";

	pSceneManager_->out() << "Shift + F6 : hide domain" << std::endl;
	pSceneManager_->out() << "D : drop new domain (at cursor pos)" << std::endl;
	pSceneManager_->out() << "V : insert vertex (at cursor pos, after highlighted vertex)" << std::endl;
	pSceneManager_->out() << "X : delete vertex" << tab << "( + Shift : domain )" << std::endl;
	pSceneManager_->out() << "I : previous vertex" << tab << "( + Shift : domain )" << std::endl;
	pSceneManager_->out() << "O : next vertex" << tab << "( + Shift : domain )" << std::endl;
	pSceneManager_->out() << "Y : vertex up" << tab << "( + Shift : domain )" << std::endl;
	pSceneManager_->out() << "B : vertex down" << tab << "( + Shift : domain )" << std::endl;
	pSceneManager_->out() << "H : vertex right" << tab << "( + Shift : domain )" << std::endl;
	pSceneManager_->out() << "G : vertex left" << tab << "( + Shift : domain )" << std::endl;
	pSceneManager_->out() << "Alt + vertex drag: will move individual vertex rather than box" << std::endl;
	pSceneManager_->out() << "F : 2d/3d domains" << std::endl;

	TEST_INVARIANT;
}

// virtual
void PedDomainEditor::displayModeInfo()
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

	pSceneManager_->out() << "MODE : Domain Edit" << std::endl;

	displayVertexCoords();

	if ( pSelectedPolygon_ )
		pSceneManager_->out() << "Domain " << pSelectedPolygon_->uniqueId() << std::endl;
}

// virtual
void PedDomainEditor::processVertexRight()
{
	// Ignore if no selected vertex
	if ( pSelectedVertex_ )
	{
		if ( pSelectedPoint_->x() != pPlanet_->surface()->xMax() )
		{
			//changeX( pSelectedPoint_->x(), pSelectedPoint_->x() + vertexSpacingX() );
			*pSelectedPoint_ += MexVec2( vertexSpacingX(), 0 );
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}

}

// virtual
void PedDomainEditor::processVertexLeft()
{
	// Ignore if no selected vertex
	if ( pSelectedVertex_ )
	{
		if ( pSelectedPoint_->x() != pPlanet_->surface()->xMin() )
		{
			//changeX( pSelectedPoint_->x(), pSelectedPoint_->x() - vertexSpacingX() );
			*pSelectedPoint_ -= MexVec2( vertexSpacingX(), 0 );
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}
}

// virtual
void PedDomainEditor::processVertexUp()
{
	// Ignore if no selected vertex
	if ( pSelectedVertex_ )
	{
		if ( pSelectedPoint_->y() != pPlanet_->surface()->yMin() )
		{
			//changeY( pSelectedPoint_->y(), pSelectedPoint_->y() - vertexSpacingY() );
			*pSelectedPoint_ -= MexVec2( 0, vertexSpacingY() );
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}
}

// virtual
void PedDomainEditor::processVertexDown()
{
	// Ignore if no selected vertex
	if ( pSelectedVertex_ )
	{
		if ( pSelectedPoint_->y() != pPlanet_->surface()->yMax() )
		{
			//changeY( pSelectedPoint_->y(), pSelectedPoint_->y() + vertexSpacingY() );
			*pSelectedPoint_ += MexVec2( 0, vertexSpacingY() );
			pSelectedPolygon_->refreshMesh();
			_DELETE( pSelectedVertex_ );
			pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
		}
	}
}

// virtual
PedPolygon* PedDomainEditor::createPolygon( const PolyVerticies& verticies, MATHEX_SCALAR height, bool selected ) const
{
	TEST_INVARIANT;

	PedPolygon* pNew = _NEW( PedDomain( verticies, height, selected ) );
	pNew->flatten( flattenPolygons_ );
	pNew->refreshMesh();


	TEST_INVARIANT;

	return pNew;
}

// virtual
PedPolygon* PedDomainEditor::createDefaultPolygon() const
{
	PRE( pHighlightVertex_ );

	TEST_INVARIANT;

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

	TEST_INVARIANT;

    return createPolygon( verticies, 100, false );
}

// virtual
void PedDomainEditor::updatePolygon()
{
	if (  mouseDrag_ and
		  pSelectedPolygon_ and
		  pHighlightVertex_ )
	{
		if ( DevKeyboard::instance().altPressed() )
		{
			*pSelectedPoint_ = pHighlightVertex_->position();
		}
		else
		{
	   		changeX( pSelectedVertex_->position().x(), pHighlightVertex_->position().x() );
			changeY( pSelectedVertex_->position().y(), pHighlightVertex_->position().y() );
		}

		pSelectedPolygon_->refreshMesh();

		_DELETE( pSelectedVertex_ );
		pSelectedVertex_ = _NEW( PedVertexMarker( pSelectedPoint_->x(), pSelectedPoint_->y(), false ) );
	}
}

void PedDomainEditor::changeX( size_t fromX, size_t toX)
{
	PRE( pSelectedPolygon_ != NULL );

	// Dry run first
	size_t count = 0;
	PolyVerticies testVert = pSelectedPolygon_->verticies();
	for ( PolyVerticies::iterator verIter = testVert.begin();
		  verIter !=testVert.end();
		  ++verIter )
	{
		if ( (*verIter).x() == fromX or
			 (*verIter).x() == toX )
		{
			++count;
		}
	}

	// Actual change of x
	if ( count == 2 )
	{
		for ( PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
			  verIter != pSelectedPolygon_->verticies().end();
			  ++verIter )
		{
			if ( (*verIter).x() == fromX )
			{
				(*verIter).x( toX );
			}
		}
	}
}

void PedDomainEditor::changeY( size_t fromY, size_t toY)
{
	PRE( pSelectedPolygon_ != NULL );

	// Dry run first
	size_t count = 0;
	PolyVerticies testVert = pSelectedPolygon_->verticies();
	for ( PolyVerticies::iterator verIter = testVert.begin();
		  verIter !=testVert.end();
		  ++verIter )
	{
		if ( (*verIter).y() == fromY or
			 (*verIter).y() == toY )
		{
			++count;
		}
	}

	// Actual change of x
	if ( count == 2 )
	{
		for ( PolyVerticies::iterator verIter = pSelectedPolygon_->verticies().begin();
			  verIter != pSelectedPolygon_->verticies().end();
			  ++verIter )
		{
			if ( (*verIter).y() == fromY )
			{
				(*verIter).y( toY );
			}
		}
	}
}

// virtual
void PedDomainEditor::writeCspFile( std::ofstream& out )
{
	numberPolygons();

	PedPolygonEditor::writeCspFile( out );

	out << std::endl;
}

// virtual
void PedDomainEditor::validate()
{
	// Remove old warnings
	warnings_.erase( warnings_.begin(), warnings_.end() );

	if ( polygons_.empty() )
	{
	 	warnings_.push_back( "Warning : No domains" );
	}
	else
	{
		for(
			Polygons::iterator itDomainPtr = polygons_.begin();
			itDomainPtr != polygons_.end();
			++itDomainPtr)
			{
				if(not (*itDomainPtr)->isConvex())
				{
					string warningMsg("WARNING : Domain ");
					warningMsg += utlToString(_STATIC_CAST(unsigned long, (*itDomainPtr)->uniqueId()));
					warningMsg += " is concave.";

					warnings_.push_back(warningMsg);
				}
			}
	}
}

void PedDomainEditor::processInsertVertex()
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

void PedDomainEditor::processDeleteVertex()
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
/* End EDITDOMA.CPP *************************************************/

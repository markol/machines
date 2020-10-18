/*
 * E D I T P O R T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/editport.hpp"
#include "planeted/editdoma.hpp"
#include "planeted/portal.hpp"
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

// Use for_each<>().
#include <algorithm>

PedPortalEditor::PedPortalEditor()
: pDomainEditor_(NULL)
{

    TEST_INVARIANT;
}

PedPortalEditor::~PedPortalEditor()
{
    TEST_INVARIANT;

}

void PedPortalEditor::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedPortalEditor& t )
{

    o << "PedPortalEditor " << (void*)&t << " start" << std::endl;
    o << "PedPortalEditor " << (void*)&t << " end" << std::endl;

    return o;
}

// virtual
void PedPortalEditor::readCspFile( const SysPathName& cspFileName )
{
	UtlLineTokeniser parser( cspFileName );

	//Read definitions until finished
    while( not parser.finished() )
    {
        ASSERT( parser.tokens().size() != 0, "" );

       	if( parser.tokens()[ 0 ] == "PORTAL" )
        {
            PolyVerticies verticies;

            //Must have line like "PORTAL 4 7 88 88 262 291"
            //ie PORTAL <domain index> <domain index> <x1> <y1> <x2> <y2>
            ASSERT( parser.tokens().size() == 7, "" );

            //Get the endpoint coordinates
            MATHEX_SCALAR x1 = atof( parser.tokens()[ 3 ].c_str() );
            MATHEX_SCALAR y1 = atof( parser.tokens()[ 4 ].c_str() );
            MATHEX_SCALAR x2 = atof( parser.tokens()[ 5 ].c_str() );
            MATHEX_SCALAR y2 = atof( parser.tokens()[ 6 ].c_str() );

            parser.parseNextLine();

			verticies.push_back( MexPoint2d( x1, y1 ) );
			verticies.push_back( MexPoint2d( x2, y2 ) );

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
void PedPortalEditor::processInput( const DevButtonEvent& devButtonEvent )
{
	PedPolygonEditor::processInput( devButtonEvent );

	if ( devButtonEvent.action() == DevButtonEvent::PRESS and active_ )
	{
		if( devButtonEvent.scanCode() == DevKey::KEY_P )
		{
			processCreatePortalsFromDomains();
		}
	    if (devButtonEvent.scanCode() == DevKey::F8 and
			devButtonEvent.wasShiftPressed() and
			not devButtonEvent.wasCtrlPressed() and
		    pSelectedPolygon_ )
		{
			pSelectedPolygon_->hide( not pSelectedPolygon_->hidden() );
		}
	}

	if (devButtonEvent.action() == DevButtonEvent::PRESS and
	    devButtonEvent.scanCode() == DevKey::F8 and
	    devButtonEvent.wasCtrlPressed() and
		not devButtonEvent.wasShiftPressed() )
	{
		hidePolygons_ = not hidePolygons_;
		hidePolygons( hidePolygons_ );
	}
}

// virtual
void PedPortalEditor::displayKeyboardCtrls( void )
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

	const char* tab = "\t";

	pSceneManager_->out() << "Shift + F6 : hide portal" << std::endl;
	pSceneManager_->out() << "D : drop new portal (at cursor pos)" << std::endl;
	pSceneManager_->out() << "Shift + X : delete portal" << std::endl;
	pSceneManager_->out() << "I : previous vertex" << tab << "( + Shift : portal )" << std::endl;
	pSceneManager_->out() << "O : next vertex" << tab << "( + Shift : portal )" << std::endl;
	pSceneManager_->out() << "Y : vertex up" << tab << "( + Shift : portal )" << std::endl;
	pSceneManager_->out() << "B : vertex down" << tab << "( + Shift : portal )" << std::endl;
	pSceneManager_->out() << "H : vertex right" << tab << "( + Shift : portal )" << std::endl;
	pSceneManager_->out() << "G : vertex left" << tab << "( + Shift : portal )" << std::endl;
	pSceneManager_->out() << "F : 2d/3d portals" << std::endl;
	pSceneManager_->out() << "P : Auto create portals" << std::endl;

	TEST_INVARIANT;
}

// virtual
void PedPortalEditor::displayModeInfo( void )
{
	PRE( pSceneManager_ != NULL );

	TEST_INVARIANT;

	pSceneManager_->out() << "MODE : Portal Edit" << std::endl;

	displayVertexCoords();

	if ( pSelectedPolygon_ )
		pSceneManager_->out() << "Portal " << pSelectedPolygon_->uniqueId() << std::endl;
}

// virtual
void PedPortalEditor::processVertexRight()
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
void PedPortalEditor::processVertexLeft()
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
void PedPortalEditor::processVertexUp()
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
void PedPortalEditor::processVertexDown()
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

// virtual
void PedPortalEditor::updatePolygon( void )
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
PedPolygon* PedPortalEditor::createPolygon( const PolyVerticies& verticies, MATHEX_SCALAR height, bool selected ) const
{
	TEST_INVARIANT;

	PedPolygon* pNew = _NEW( PedPortal( verticies, height, selected ) );
	pNew->flatten( flattenPolygons_ );
	pNew->refreshMesh();

	TEST_INVARIANT;

	return pNew;
}

// virtual
PedPolygon* PedPortalEditor::createDefaultPolygon() const
{
	PRE( pHighlightVertex_ );

	TEST_INVARIANT;

	PolyVerticies verticies;

	MexPoint2d newPoint =  pHighlightVertex_->position();

	verticies.push_back( newPoint );

	MachPhysPlanetSurface* pSurface = pPlanet_->surface();

	newPoint += MexVec2( 2 * vertexSpacingX(), 0 );
	verticies.push_back( newPoint );

	TEST_INVARIANT;

    return createPolygon( verticies, 100, false );
}

void PedPortalEditor::domainEditor( PedDomainEditor* pDomainEditor )
{
	pDomainEditor_ = pDomainEditor;
}

// virtual
void PedPortalEditor::writeCspFile( std::ofstream& out )
{
	PRE( pDomainEditor_ != NULL );

	for (	Polygons::iterator portalIter = polygons_.begin();
			portalIter != polygons_.end();
			++portalIter )
	{
		PedPortal* pPortal = _REINTERPRET_CAST( PedPortal*, *portalIter );

		bool firstDomainFound = false;
		size_t domainOne = 0;
		size_t domainTwo = 0;

		for (	Polygons::const_iterator domainIter = pDomainEditor_->polygons().begin();
				domainIter != pDomainEditor_->polygons().end();
				++domainIter )
		{
			const PedPolygon* pPolygon = *domainIter;
			const PedDomain* pDomain = _REINTERPRET_CAST( const PedDomain*, pPolygon );

			if ( pDomain->intersectsPortal( *pPortal ) )
			{
				if ( firstDomainFound )
				{
					domainTwo = pDomain->uniqueId();
					break;
				}
				else
				{
					domainOne = pDomain->uniqueId();
					firstDomainFound = true;
				}
			}
		}

		pPortal->setDomains( domainOne, domainTwo );
	}

	PedPolygonEditor::writeCspFile( out );

	out << std::endl;
}

// virtual
void PedPortalEditor::validate()
{
	PRE( pDomainEditor_ != NULL );

	// Remove old warnings
	warnings_.erase( warnings_.begin(), warnings_.end() );

	// Work out if portals are valid (do they intersect two domains only)
	for (	Polygons::iterator portalIter = polygons_.begin();
			portalIter != polygons_.end();
			++portalIter )
	{
		PedPortal* pPortal = _REINTERPRET_CAST( PedPortal*, *portalIter );

		bool firstDomainFound = false;
		bool secondDomainFound = false;
		bool thirdDomainFound = false;

		for (	Polygons::const_iterator domainIter = pDomainEditor_->polygons().begin();
				domainIter != pDomainEditor_->polygons().end();
				++domainIter )
		{
			const PedPolygon* pPolygon = *domainIter;
			const PedDomain* pDomain = _REINTERPRET_CAST( const PedDomain*, pPolygon );

			if ( pDomain->intersectsPortal( *pPortal ) )
			{
				if ( secondDomainFound )
				{
					thirdDomainFound = true;
					break;
				}
				else if ( firstDomainFound )
				{
					secondDomainFound = true;
				}
				else
				{
					firstDomainFound = true;
				}
			}
		}

		char buffer[20];
//		itoa( pPortal->uniqueId(), buffer, 10 );
		sprintf( buffer, "%zu", pPortal->uniqueId());
		string msg("Warning : Portal ");
		msg += buffer;

		if ( thirdDomainFound )
		{
			msg += " connects too many domains";
			warnings_.push_back( msg );
		}
		else if ( not secondDomainFound and firstDomainFound )
		{
			msg += " is only connected correctly to one domain";
			warnings_.push_back( msg );
		}
		else if ( not firstDomainFound )
		{
			msg += " does not connect any domains";
			warnings_.push_back( msg );
		}
	}
}

void deletePolygons(PedPolygon* pPolygon)
{
	_DELETE(pPolygon);
}

void PedPortalEditor::processCreatePortalsFromDomains( void )
{
	Polygons domains = pDomainEditor_->polygons();
	uint nDomains = domains.size();

	for_each(polygons_.begin(), polygons_.end(), deletePolygons);
	polygons_.erase( polygons_.begin(), polygons_.end() );

	for( uint domain1Index = 0; domain1Index < nDomains; ++domain1Index )
	{
		for( uint domain2Index = 1 + domain1Index; domain2Index < nDomains; ++ domain2Index )
		{
			ASSERT(domain1Index != domain2Index, "The loop should ensure that this doesn't happen!");

			const PedDomain* pDomain1 = _REINTERPRET_CAST( PedDomain*, domains[ domain1Index ] );
			const PedDomain* pDomain2 = _REINTERPRET_CAST( PedDomain*, domains[ domain2Index ] );

			DANIEL_STREAM("Domains " << domain1Index << " " << domain2Index << std::endl);

			MexPoint2d portalStart;
			MexPoint2d portalEnd;

			if( pDomain1->intersectsDomain( *pDomain2, &portalStart, &portalEnd ) )
 			{
				DANIEL_STREAM("Portal found." << std::endl);

				PolyVerticies verticies;
				verticies.push_back( portalStart );
				verticies.push_back( portalEnd );

				polygons_.push_back( createPolygon( verticies, 100, false ) );
			}

			DANIEL_STREAM(std::endl);
		}
	}
}

/* End EDITPORT.CPP *************************************************/

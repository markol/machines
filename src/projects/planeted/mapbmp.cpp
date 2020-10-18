/*
 * M A P B M P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/mapbmp.hpp"

#include "planeted/editpoly.hpp"
#include "planeted/polygon.hpp"

#include "machlog/planet.hpp"

#include "machphys/plansurf.hpp"

#include "render/surface.hpp"
#include "render/render.hpp"
#include "render/display.hpp"
#include "render/device.hpp"
#include "render/colour.hpp"

#include "world4d/scenemgr.hpp"
#include "world4d/manager.hpp"

PedMapCreator::PedMapCreator( const PedPolygonEditor& domainEditor,
							  const PedPolygonEditor& portalEditor,
							  const PedPolygonEditor& obstacleEditor )
:	domainEditor_( domainEditor ),
 	portalEditor_( portalEditor ),
 	obstacleEditor_( obstacleEditor )
{

	TEST_INVARIANT;
}

PedMapCreator::~PedMapCreator()
{
    TEST_INVARIANT;

}

void PedMapCreator::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedMapCreator& t )
{

    o << "PedMapCreator " << (void*)&t << " start" << std::endl;
    o << "PedMapCreator " << (void*)&t << " end" << std::endl;

    return o;
}

void PedMapCreator::createBmp()
{
	TEST_INVARIANT;

	MachPhysPlanetSurface* pPlanet = MachLogPlanet::instance().surface();

	RenDisplay* pDisplay = RenDevice::current()->display();

	const size_t xMax = pPlanet->xMax();
	const size_t yMax = pPlanet->yMax();
	RenSurface surface = RenSurface::createAnonymousSurface( xMax, yMax, RenDevice::current()->backSurface() );

	typedef ctl_pvector< PedPolygon > Polygons;
	typedef ctl_vector< MexPoint2d > PolyVerticies;

	/*
	const MATHEX_SCALAR maxHeight = 150;

	for ( size_t x = 0; x < xMax; ++x )
	{
		W4dManager::instance().sceneManager()->out() << "Creating sat map. Please wait : " << xMax - x << std::endl;
 		W4dManager::instance().render();

		for ( size_t y = 0; y < yMax; ++y )
		{
			MATHEX_SCALAR height =	pPlanet->terrainHeight( x, y );
			if ( height < 0.0 )
				height = 0.0;

			surface.setPixel( x, y, RenColour( ( height / maxHeight ) + 0.05,
											   ( height / maxHeight ) + 0.05,
											   ( height / maxHeight ) + 0.05) );
		}
	}
	*/

	// Create next map filename...
	size_t count = 0;
	bool nextBmpFilename = false;
	string filename;

	while ( not nextBmpFilename )
	{
		filename = "map";

		char buffer[20];
//		itoa( count, buffer, 10 );
		sprintf( buffer, "%zu", count );

		int zeros = 3 - strlen( buffer );
		if ( zeros < 0 )
			zeros = 0;

		while ( zeros-- )
		{
			filename += "0";
		}

		filename += buffer;
		filename += ".bmp";

		SysPathName bmpfilename( filename );
		nextBmpFilename = not bmpfilename.existsAsFile();
		++count;
	}

	W4dManager::instance().sceneManager()->out() << "Creating map '" << filename << "'. Please wait." << std::endl;
	W4dManager::instance().render();

	if ( not domainEditor_.polygonsHidden() )
	{
		// Output solid domains...
		for ( Polygons::const_iterator polyIter = domainEditor_.polygons().begin();
			  polyIter != domainEditor_.polygons().end();
			  ++polyIter )
		{
			PolyVerticies verticies = (*polyIter)->verticies();

			surface.filledRectangle( RenSurface::Rect( 	verticies[0].x(),
														verticies[0].y(),
														verticies[2].x() - verticies[0].x(),
														verticies[2].y() - verticies[0].y() ), RenColour(0.0, 0.0, 0.4) );
		}
	}

	// Output grid...
	// Lighter grid-lines every 100
	for ( size_t y = 0; y < yMax; y += 10 )
	{
		PolyVerticies verticies;

		verticies.push_back( MexPoint2d( 0, y ) );
		verticies.push_back( MexPoint2d( xMax, y ) );

		surface.polyLine( verticies, RenColour(0.2, 0.2, 0.2), 1 );
	}

	for ( size_t x = 0; x < xMax; x += 10 )
	{
		PolyVerticies verticies;

		verticies.push_back( MexPoint2d( x, 0 ) );
		verticies.push_back( MexPoint2d( x, yMax ) );

		surface.polyLine( verticies, RenColour(0.2, 0.2, 0.2), 1 );
	}

	for ( size_t y = 0; y < yMax; y += 100 )
	{
		PolyVerticies verticies;

		verticies.push_back( MexPoint2d( 0, y ) );
		verticies.push_back( MexPoint2d( xMax, y ) );

		surface.polyLine( verticies, RenColour(0.4, 0.4, 0.4), 1 );
	}

	for ( size_t x = 0; x < xMax; x += 100 )
	{
		PolyVerticies verticies;

		verticies.push_back( MexPoint2d( x, 0 ) );
		verticies.push_back( MexPoint2d( x, yMax ) );

		surface.polyLine( verticies, RenColour(0.4, 0.4, 0.4), 1 );
	}

	if ( not domainEditor_.polygonsHidden() )
	{
		// Output domains...
		for ( Polygons::const_iterator polyIter = domainEditor_.polygons().begin();
			  polyIter != domainEditor_.polygons().end();
			  ++polyIter )
		{
			PolyVerticies verticies = (*polyIter)->verticies();
			verticies.push_back( verticies[0] );

			surface.polyLine( verticies, RenColour::blue(), 1 );
		}
	}

	if ( not obstacleEditor_.polygonsHidden() )
	{
		// Output obstacles...
		for ( Polygons::const_iterator polyIter = obstacleEditor_.polygons().begin();
			  polyIter != obstacleEditor_.polygons().end();
			  ++polyIter )
		{
			PolyVerticies verticies = (*polyIter)->verticies();
			verticies.push_back( verticies[0] );

			surface.polyLine( verticies, RenColour::red(), 1 );
		}
	}

	if ( not portalEditor_.polygonsHidden() )
	{
		// Output portals...
		for ( Polygons::const_iterator polyIter = portalEditor_.polygons().begin();
			  polyIter != portalEditor_.polygons().end();
			  ++polyIter )
		{
			PolyVerticies verticies = (*polyIter)->verticies();
			verticies.push_back( verticies[0] );

			surface.polyLine( verticies, RenColour::cyan(), 1 );
		}
	}

	surface.saveAsBmp( RenSurface::Rect( 0, 0, xMax, yMax ), filename );

    TEST_INVARIANT;
}

/* End MAPBMP.CPP ***************************************************/

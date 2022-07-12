/*
 * P O L Y G O N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/polygon.hpp"

#include "machlog/planet.hpp"

#include "render/device.hpp"
#include "render/mesh.hpp"

#include "mathex/cvexpgon.hpp"
#include "mathex/double.hpp"
#include "render/colour.hpp"
#include <fstream>

PedPolygon::PedPolygon( const PolyVerticies& verticies, MATHEX_SCALAR z, bool selected )
: 	W4dCustom( MachLogPlanet::instance().pWorld(), W4dTransform3d(), W4dEntity::NOT_SOLID, W4dDistance( 10000 ) ),
	verticies_( verticies ),
	z_( z ),
	selected_( selected ),
	hidden_( false ),
	flatten_( false ),
	uniqueId_( 0 )
{

    TEST_INVARIANT;
}

PedPolygon::~PedPolygon()
{
    TEST_INVARIANT;

}

void PedPolygon::refreshMesh()
{
	//Construct a material of the appropriate colour
    RenMaterial mat( selected_ ? RenColour::green() : getWireColour() );

	// Even if obstacle is hidden show it when it is selected
	if ( hidden_ and selected_ )
		attachTo( MachLogPlanet::instance().pWorld() );
	else if ( hidden_ and not selected_ )
		attachTo( &MachLogPlanet::instance().hiddenRoot() );

    //Construct a wire frame cuboid surrounding the boundary
	if ( hasMesh() )
	    emptyMesh( W4dDistance( 10000 ) );

    RenMesh& boxMesh = mesh();

	MexPoint2d firstPoint = verticies_.front();
	for ( PolyVerticies::const_iterator iter = verticies_.begin();
		  iter != verticies_.end();)
	{
		MexPoint3d pointOne  ( (*iter).x(), (*iter).y(), 0 );
		MexPoint3d pointThree( (*iter).x(), (*iter).y(), 0 + height() );

		++iter;

		MexPoint3d pointTwo( 0, 0, 0 );
		MexPoint3d pointFour( 0, 0, 0 );

		if ( iter == verticies_.end() )
		{
			pointTwo = MexPoint3d( firstPoint.x(), firstPoint.y(), 0 );
			pointFour = MexPoint3d( firstPoint.x(), firstPoint.y(), 0 + height() );
		}
		else
		{
			pointTwo = MexPoint3d( (*iter).x(), (*iter).y(), 0 );
			pointFour = MexPoint3d( (*iter).x(), (*iter).y(), 0 + height() );
		}

		boxMesh.addLine( pointOne, pointTwo, mat );

		if ( not flatten_ )
		{
			boxMesh.addLine( pointOne, pointThree, mat );
			boxMesh.addLine( pointThree, pointFour, mat );
		}
	}
}

void PedPolygon::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedPolygon& t )
{

    o << "PedPolygon " << (void*)&t << " start" << std::endl;
    o << "PedPolygon " << (void*)&t << " end" << std::endl;

    return o;
}

PolyVerticies& PedPolygon::verticies()
{
	return verticies_;
}

const PolyVerticies& PedPolygon::verticies() const
{
	return verticies_;
}

// virtual
void PedPolygon::select( bool selected )
{
	selected_ = selected;
}

void PedPolygon::hide( bool hide )
{
	hidden_ = hide;

	// Only hide if it isn't selected
	if ( not selected_ )
		attachTo( hidden_ ? &MachLogPlanet::instance().hiddenRoot() : MachLogPlanet::instance().pWorld() );
}

bool PedPolygon::hidden()
{
	return hidden_;
}

// virtual
size_t PedPolygon::height() const
{
	return z_;
}

void PedPolygon::flatten( bool flatten )
{
	flatten_ = flatten;
}

void PedPolygon::uniqueId( size_t uniqueId )
{
	uniqueId_ = uniqueId;
}

size_t PedPolygon::uniqueId() const
{
	return uniqueId_;
}

//virtual
void PedPolygon::height( size_t z )
{
	z_ = z;
	refreshMesh();
}

bool PedPolygon::isConvex() const
{
	MexConvexPolygon2d testPoly( verticies() );

	return testPoly.isClockwiseAndConvex();
}

/* End POLYGON.CPP **************************************************/

/*
 * V E T X M R K R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/vetxmrkr.hpp"

#include "machlog/planet.hpp"

#include "render/device.hpp"
#include "render/colour.hpp"
#include "render/mesh.hpp"
#include "mathex/double.hpp"

PedVertexMarker::PedVertexMarker( size_t x, size_t y, bool highlight )
: 	W4dCustom( 	MachLogPlanet::instance().pWorld(),
				W4dTransform3d(),
				W4dEntity::NOT_SOLID,
				W4dDistance( 10000 ) ),
	position_( x, y )
{
	//Construct a material of the appropriate colour
    RenMaterial mat( highlight ? RenColour::yellow() : RenColour::magenta() );

    //Construct a wire frame cuboid surrounding the boundary
    RenMesh& renMesh = mesh();

	const int z = 0;

	renMesh.addLine( MexPoint3d( x, y, z ), MexPoint3d( x, y, z + 100 ), mat );

 	addSmallBox( x, y, z, highlight );
	addSmallBox( x, y, z + 98, highlight );

    TEST_INVARIANT;
}

PedVertexMarker::PedVertexMarker( const MexPoint3d& point, bool highlight )
: 	W4dCustom( 	MachLogPlanet::instance().pWorld(),
				W4dTransform3d(),
				W4dEntity::NOT_SOLID,
				W4dDistance( 10000 ) ),
	position_( point.x(), point.y() )
{
	//Construct a material of the appropriate colour
    RenMaterial mat( highlight ? RenColour::yellow() : RenColour::magenta() );

    //Construct a wire frame cuboid surrounding the boundary
    RenMesh& renMesh = mesh();
	uint height = 100 - point.z();
	renMesh.addLine( point, MexPoint3d( point.x(), point.y(), point.z() + height ), mat );

 	addSmallBox( point.x(), point.y(), point.z(), highlight );
	addSmallBox( point.x(), point.y(), point.z() + 98, highlight );

    TEST_INVARIANT;
}

PedVertexMarker::~PedVertexMarker()
{
    TEST_INVARIANT;

}

void PedVertexMarker::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

const MexPoint2d& PedVertexMarker::position() const
{
	return position_;
}

void PedVertexMarker::addSmallBox( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z, bool highlight )
{
	//Construct a material of the appropriate colour
    RenMaterial mat( highlight ? RenColour::yellow() : RenColour::blue() );

    //Construct a wire frame cuboid surrounding the boundary
    RenMesh& renMesh = mesh();

	renMesh.addLine( MexPoint3d( x - 1, y - 1, z ), MexPoint3d( x + 1, y - 1, z ), mat );
	renMesh.addLine( MexPoint3d( x + 1, y - 1, z ), MexPoint3d( x + 1, y + 1, z ), mat );
	renMesh.addLine( MexPoint3d( x + 1, y + 1, z ), MexPoint3d( x - 1, y + 1, z ), mat );
	renMesh.addLine( MexPoint3d( x - 1, y + 1, z ), MexPoint3d( x - 1, y - 1, z ), mat );

	renMesh.addLine( MexPoint3d( x - 1, y - 1, z + 2 ), MexPoint3d( x + 1, y - 1, z + 2 ), mat );
	renMesh.addLine( MexPoint3d( x + 1, y - 1, z + 2 ), MexPoint3d( x + 1, y + 1, z + 2 ), mat );
	renMesh.addLine( MexPoint3d( x + 1, y + 1, z + 2 ), MexPoint3d( x - 1, y + 1, z + 2 ), mat );
	renMesh.addLine( MexPoint3d( x - 1, y + 1, z + 2 ), MexPoint3d( x - 1, y - 1, z + 2 ), mat );

	renMesh.addLine( MexPoint3d( x - 1, y - 1, z ), MexPoint3d( x - 1, y - 1, z + 2 ), mat );
	renMesh.addLine( MexPoint3d( x + 1, y - 1, z ), MexPoint3d( x + 1, y - 1, z + 2 ), mat );
	renMesh.addLine( MexPoint3d( x + 1, y + 1, z ), MexPoint3d( x + 1, y + 1, z + 2 ), mat );
	renMesh.addLine( MexPoint3d( x - 1, y + 1, z ), MexPoint3d( x - 1, y + 1, z + 2 ), mat );
}

ostream& operator <<( ostream& o, const PedVertexMarker& t )
{

    o << "PedVertexMarker " << (void*)&t << " start" << std::endl;
    o << "PedVertexMarker " << (void*)&t << " end" << std::endl;

    return o;
}

/* End VETXMRKR.CPP *************************************************/

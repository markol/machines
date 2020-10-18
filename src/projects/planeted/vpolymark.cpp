/*
 * V P O L Y M A R K . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/vpolymark.hpp"
#include "world4d/entity.hpp"
#include "render/render.hpp"
#include "render/device.hpp"
#include "render/mesh.hpp"
#include "render/vertex.hpp"
#include "render/meshinst.hpp"
#include "render/colour.hpp"
#include "mathex/mathex.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/double.hpp"
#include "mathex/point3d.hpp"
#include "machphys/surfchk.hpp"
#include "machlog/planet.hpp"


PedVerticalPolygonMarker::PedVerticalPolygonMarker(  W4dEntity* pParent, const MexTransform3d& localTransform, const MexPoint3d& vertex1, const MexPoint3d& vertex2, const MexPoint3d& vertex3 )
: 	W4dCustom(  pParent, 
				localTransform, 
				W4dEntity::NOT_SOLID, 
				W4dDistance( 10000 ) ),
	position_( vertex1 )
{
	// Create a simple marker consisting of a mesh around the triangle of points passed in
	//Construct a material of the appropriate colour
    RenMaterial mat( RenColour::yellow() );

	MATHEX_SCALAR HEIGHT = 0.5;

    RenMesh& viewVolumeMesh = mesh();
	MexPoint3d v1 = MexPoint3d( vertex1.x(), vertex1.y(), vertex1.z()+HEIGHT );
	MexPoint3d v2 = MexPoint3d( vertex2.x(), vertex2.y(), vertex2.z()+HEIGHT );
	MexPoint3d v3 = MexPoint3d( vertex3.x(), vertex3.y(), vertex3.z()+HEIGHT );

	viewVolumeMesh.addLine( v1, v2, mat );
	viewVolumeMesh.addLine( v2, v3, mat );
	viewVolumeMesh.addLine( v3, v1, mat );

    TEST_INVARIANT;
}

PedVerticalPolygonMarker::~PedVerticalPolygonMarker()
{
    TEST_INVARIANT;

}

void PedVerticalPolygonMarker::display( bool display )
{
	// Hide/show poly markers
	attachTo( display ? MachLogPlanet::instance().pWorld() : &MachLogPlanet::instance().hiddenRoot() );
}

void PedVerticalPolygonMarker::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedVerticalPolygonMarker& t )
{
    o << t.position_;

    return o;
}

/* End VPOLYMARK.CPP ************************************************/

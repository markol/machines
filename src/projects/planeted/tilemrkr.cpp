/*
 * T I L E M R K R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/tilemrkr.hpp"

#include "render/device.hpp"
#include "render/material.hpp"
#include "render/colour.hpp"
#include "render/mesh.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/double.hpp"


PedTileMarker::PedTileMarker( 	W4dEntity* pParent,
								const W4dTransform3d& localTransform,
								const MexAlignedBox3d& boundary,
								bool permanent )
:   W4dCustom( pParent, localTransform, W4dEntity::NOT_SOLID, W4dDistance( 10000 ) )
{
    //Construct a material of the appropriate colour
    RenMaterial mat( permanent ? RenColour::green() : RenColour::yellow() );

    //get the boundary limits

	// Changed by Wayne Evans
    // const MexPoint3d& minCorner = boundary.local().minCorner_;
    // const MexPoint3d& maxCorner = boundary.local().maxCorner_;

    const MexPoint3d& minCorner = boundary.minCorner();
    const MexPoint3d& maxCorner = boundary.maxCorner();

    MATHEX_SCALAR xMin = minCorner.x();// - 1.0;
    MATHEX_SCALAR yMin = minCorner.y();// - 1.0;
    MATHEX_SCALAR zMin = minCorner.z() + 0.5;
    MATHEX_SCALAR xMax = maxCorner.x();// + 1.0;
    MATHEX_SCALAR yMax = maxCorner.y();// + 1.0;
    MATHEX_SCALAR zMax = maxCorner.z() + 0.5;

    //Construct a wire frame cuboid surrounding the boundary
    RenMesh& boxMesh = mesh();
	boxMesh.addLine( MexPoint3d( xMin, yMin, zMin ), MexPoint3d( xMax, yMin, zMin ), mat );
	boxMesh.addLine( MexPoint3d( xMax, yMin, zMin ), MexPoint3d( xMax, yMax, zMin ), mat );
	boxMesh.addLine( MexPoint3d( xMax, yMax, zMin ), MexPoint3d( xMin, yMax, zMin ), mat );
	boxMesh.addLine( MexPoint3d( xMin, yMax, zMin ), MexPoint3d( xMin, yMin, zMin ), mat );

	boxMesh.addLine( MexPoint3d( xMin, yMin, zMax ), MexPoint3d( xMax, yMin, zMax ), mat );
	boxMesh.addLine( MexPoint3d( xMax, yMin, zMax ), MexPoint3d( xMax, yMax, zMax ), mat );
	boxMesh.addLine( MexPoint3d( xMax, yMax, zMax ), MexPoint3d( xMin, yMax, zMax ), mat );
	boxMesh.addLine( MexPoint3d( xMin, yMax, zMax ), MexPoint3d( xMin, yMin, zMax ), mat );

	boxMesh.addLine( MexPoint3d( xMin, yMin, zMin ), MexPoint3d( xMin, yMin, zMax ), mat );
	boxMesh.addLine( MexPoint3d( xMax, yMin, zMin ), MexPoint3d( xMax, yMin, zMax ), mat );
	boxMesh.addLine( MexPoint3d( xMax, yMax, zMin ), MexPoint3d( xMax, yMax, zMax ), mat );
	boxMesh.addLine( MexPoint3d( xMin, yMax, zMin ), MexPoint3d( xMin, yMax, zMax ), mat );

    TEST_INVARIANT;
}

PedTileMarker::~PedTileMarker()
{
    TEST_INVARIANT;

}

void PedTileMarker::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedTileMarker& t )
{

    o << "PedTileMarker " << (void*)&t << " start" << std::endl;
    o << "PedTileMarker " << (void*)&t << " end" << std::endl;

    return o;
}

bool PedTileMarker::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}


/* End TILEMRKR.CPP *************************************************/

/*
 * C A M R M A R K . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/camrmrkr.hpp"
#include "machlog/planet.hpp"
#include "machlog/camera.hpp"
#include "world4d/entity.hpp"
#include "render/render.hpp"
#include "render/device.hpp"
#include "render/mesh.hpp"
#include "render/vertex.hpp"
#include "render/meshinst.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/double.hpp"
#include "mathex/point3d.hpp"
#include "machphys/machphys.hpp"

PedCameraMarker::PedCameraMarker( W4dEntity* pParent, const MexTransform3d& localTransform, MachLogCamera::Type cameraType, const RenColour& colour )
: 	W4dCustom(  pParent,
				localTransform,
				W4dEntity::NOT_SOLID,
				W4dDistance( 10000 ) ),
	cameraType_( cameraType ),
	colour_( colour )

{
}

void PedCameraMarker::draw()
{
	// Draws marker ( a big arrow along x-axis )

	//Construct a material of the appropriate colour
    RenMaterial mat( colour_ );

    RenMesh& viewVolumeMesh = mesh();

	//double alpha = camera.horizontalFOVAngle();
   //	double beta = camera.verticalFOVAngle();
   //	double gamma = alpha/2;
   //	double theta = beta/2;

	int focalLength = 30;

   	// Calculate offsets for viewing volume
   //	int horizontalDisplacement = gamma * focalLength * tan( gamma );
   //	int verticalDisplacement = theta * focalLength * tan( theta );

	MATHEX_SCALAR minx, miny, minz;
	minx = miny = minz = 0;

	/*viewVolumeMesh.addLine(	MexPoint3d( minx, miny, minz ),
							MexPoint3d( minx+focalLength, miny+horizontalDisplacement, minz ), mat );
	viewVolumeMesh.addLine(	MexPoint3d( minx, miny, minz ),
							MexPoint3d( minx+focalLength, miny-horizontalDisplacement, minz ), mat );

	viewVolumeMesh.addLine(	MexPoint3d( minx, miny, minz ),
							MexPoint3d( minx+focalLength, miny, minz+verticalDisplacement ), mat );
	viewVolumeMesh.addLine(	MexPoint3d( minx, miny, minz ),
							MexPoint3d( minx+focalLength, miny, minz-verticalDisplacement ), mat );
	*/
	viewVolumeMesh.addLine(	MexPoint3d( minx, miny, minz ),
							MexPoint3d( minx+focalLength, miny, minz ), mat );

	// Draw simple arrow head
	viewVolumeMesh.addLine(	MexPoint3d( minx+focalLength, miny, minz ),
							MexPoint3d( minx+focalLength-5, miny+5, minz ), mat );
	viewVolumeMesh.addLine(	MexPoint3d( minx+focalLength, miny, minz ),
							MexPoint3d( minx+focalLength-5, miny-5, minz ), mat );
	TEST_INVARIANT;
}

PedCameraMarker::~PedCameraMarker()
{
    TEST_INVARIANT;

}

void PedCameraMarker::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedCameraMarker& t )
{

    o << "PedCameraMarker " << (void*)&t << " start" << std::endl;
    o << "PedCameraMarker " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CAMRMARK.CPP *************************************************/

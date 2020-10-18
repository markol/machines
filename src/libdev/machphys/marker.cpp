/*
 * M A R K E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/marker.hpp"
#include "machphys/machphys.hpp"
#include "machphys/internal/healthma.hpp"
#include "render/device.hpp"
#include "render/material.hpp"
#include "render/colour.hpp"
#include "render/mesh.hpp"
#include "render/ttftris.hpp"
#include "render/texmgr.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/double.hpp"
#include "system/registry.hpp"

PER_DEFINE_PERSISTENT( MachPhysMarker );

static void setMarkerDimensions
(
	RenTTFTriangles& ttf,
	MATHEX_SCALAR totalWidth,
	MATHEX_SCALAR totalHeight,
	MATHEX_SCALAR cornerSize,
	MATHEX_SCALAR lineThickness
)
{
	// Corner size is a percentage of totalWidth and totalHeight.
	// Corners must always be less than half the total size.
	PRE(cornerSize > 0);
	PRE(cornerSize <= 0.5);

	// First set the polygon dimensions.
	ttf.width(totalWidth);
	ttf.height(totalHeight);

	const MATHEX_SCALAR cw = cornerSize * totalWidth;
	const MATHEX_SCALAR ch = cornerSize * totalHeight;
	ttf.cornerWidth(cw);
	ttf.cornerHeight(ch);

	// Next stretch the texture UV coords so that the lines have a xyz-space
	// thickness of lineThickness.  We need to know how thick the line is in
	// uv space.  The texture is 32x32 and the lines are one texel wide.
	const MATHEX_SCALAR uvThickness = 1.0/32.0;
	const MATHEX_SCALAR u = uvThickness * cw / lineThickness;
	const MATHEX_SCALAR v = uvThickness * ch / lineThickness;

	ASSERT(u >= uvThickness,     "2D marker UV calculation error");
	ASSERT(v >= uvThickness,     "2D marker UV calculation error");
	ASSERT(u <= 1 - uvThickness, "2D marker UV calculation error");
	ASSERT(v <= 1 - uvThickness, "2D marker UV calculation error");

	const MexVec2 uChanged(u, 1);
	const MexVec2 vChanged(0, 1 - v);

	ttf.uv(uChanged, 1);
	ttf.uv(uChanged, 5);
	ttf.uv(uChanged, 7);
	ttf.uv(uChanged, 11);

	ttf.uv(vChanged, 2);
	ttf.uv(vChanged, 4);
	ttf.uv(vChanged, 8);
	ttf.uv(vChanged, 10);
}

MachPhysMarker::MachPhysMarker
(
    W4dEntity* pParent, const W4dTransform3d& localTransform,
    const MexAlignedBox3d& boundary, bool permanent, int percentageHp
)
:   W4dCustom( pParent, localTransform, W4dEntity::NOT_SOLID, _STATIC_CAST( MexDouble, RenDevice::current()->fogEnd() ) )
{
	// This entity is pure emissive so it needs no lighting.
	doNotLight(true);

    //get the boundary limits
    const MexPoint3d& minCorner = boundary.minCorner();
    const MexPoint3d& maxCorner = boundary.maxCorner();

	// There are two flavours of marker: 2D on a TTF or 3D wireframe boxes.
	// The choice is determined by a registry setting.
	const bool use2D = SysRegistry::instance().queryIntegerValue("Options\\Cursor Type", "2D", SysRegistry::CURRENT_USER);
	if (use2D)
	{
		// Change the alpha to be partly transparent.  Set a hardcoded texture.
		static const RenTexture tex1 = RenTexManager::instance().createTexture("mark3_t.bmp");
		static const MachPhysIHealthMaterials mats(20, tex1, 0.7);
		const RenMaterial& mat = (permanent)? mats.material(percentageHp): mats.blueMaterial();

		// For objects that are build buried in the ground, we don't want the
		// marker to extend down into the ground.  Thus clamp the minimum z.
	    const MATHEX_SCALAR zMin   = std::max( minCorner.z(), 0.0 );
		const MATHEX_SCALAR width  = maxCorner.x() - minCorner.x();
		const MATHEX_SCALAR depth  = maxCorner.y() - minCorner.y();
		MATHEX_SCALAR height       = maxCorner.z() - zMin;

		const MATHEX_SCALAR minTTFWidth  = 3.0;
		const MATHEX_SCALAR minTTFHeight = 2.5;

		// Average the width and depth and use it as the 2D width.
		// Set a lower limit on the width of the overall TTF.
		const MATHEX_SCALAR average = 0.5 * (width + depth);
		const MATHEX_SCALAR ttfWidth  = mexClamp(average, minTTFWidth, HUGE_VAL);

		// Firstly, if the width is greater than the height, increase the
		// height slightly.  This gives a better aspect ratio in zenith.
		if (width > height)
			height = 0.25 * (3 * height + width);

		// Set a lower limit on the overall height of the TTF.
		const MATHEX_SCALAR ttfHeight = mexClamp(height,  minTTFHeight, HUGE_VAL);

		// If we have artificially increased the TTF height, move the centre
		// up so the bottom edge isn't buried the ground.
		MexPoint3d centre = boundary.centroid();
		if (ttfHeight > height)
		{
			const MATHEX_SCALAR newZ = centre.z() + (ttfHeight - height) / 2;
			centre.z(newZ);
		}

		RenTTFTriangles ttf;
		ttf.centre(centre);
		ttf.depthOffset(-ttfWidth * 0.6);
		ttf.material(mat);

		// Compute an interpolated line thickness.  Larger objects have thicker lines.
		const MATHEX_SCALAR minLineThickness = 0.25;
		const MATHEX_SCALAR maxLineThickness = 0.8;
		const MATHEX_SCALAR minThicknessAt   = 3.5;
		const MATHEX_SCALAR maxThicknessAt   = 15.0;
		const MATHEX_SCALAR clampWidth1      = mexClamp(ttfWidth, minThicknessAt, maxThicknessAt);
		const MATHEX_SCALAR lineThickness    = mexInterpolate(clampWidth1,
															  minThicknessAt,   maxThicknessAt,
															  minLineThickness, maxLineThickness);

		// Increase the corner size with decreasing object size.
		const MATHEX_SCALAR smallCorner   =  7.0 / 32.0;
		const MATHEX_SCALAR largeCorner   = 12.0 / 32.0;
		const MATHEX_SCALAR smallCornerAt = 15.0;
		const MATHEX_SCALAR largeCornerAt =  5.0;
		const MATHEX_SCALAR clampWidth2   = mexClamp(average, largeCornerAt, smallCornerAt);
		const MATHEX_SCALAR cornerSize    = mexInterpolate(clampWidth2,
														   largeCornerAt, smallCornerAt,
														   largeCorner,   smallCorner);

		setMarkerDimensions(ttf, ttfWidth, height, cornerSize, lineThickness);

	    RenMesh& boxMesh = mesh();
		boxMesh.addTTFPolygon(ttf);
	}
	else	// use 3D wireframe
	{
		static const MachPhysIHealthMaterials mats(20);
		const RenMaterial& mat = (permanent)? mats.material(percentageHp): mats.blueMaterial();

 	    const MATHEX_SCALAR xMin = minCorner.x() - 1.0;
 	    const MATHEX_SCALAR yMin = minCorner.y() - 1.0;
 	    const MATHEX_SCALAR zMin = std::max( minCorner.z(), 0.0 ) + 0.5;
 	    const MATHEX_SCALAR xMax = maxCorner.x() + 1.0;
 	    const MATHEX_SCALAR yMax = maxCorner.y() + 1.0;
 	    const MATHEX_SCALAR zMax = maxCorner.z() + 0.5;

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
	}

	// Stop marker from getting affected by animations
	clientData( (int)MachPhys::ECD_DONTAPPLYANIMATION );

    TEST_INVARIANT;
}

MachPhysMarker::~MachPhysMarker()
{
    TEST_INVARIANT;

}

void MachPhysMarker::CLASS_INVARIANT
{
	INVARIANT( this );
}

ostream& operator <<( ostream& o, const MachPhysMarker& t )
{

    o << "MachPhysMarker " << (void*)&t << " start" << std::endl;
    o << "MachPhysMarker " << (void*)&t << " end" << std::endl;

    return o;
}

bool MachPhysMarker::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

MachPhysMarker::MachPhysMarker(PerConstructor con)
:W4dCustom( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysMarker& sphere )
{
    const W4dCustom& base = sphere;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysMarker& sphere )
{
    W4dCustom& base = sphere;

    istr >> base;
}
/* End MARKER.CPP ***************************************************/

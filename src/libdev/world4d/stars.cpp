/***********************************************************

  S T A R S . C P P
  (c) Charybdis Limited, 1998. All Rights Reserved

***********************************************************/

#include "world4d/stars.hpp"
#include "world4d/camera.hpp"

#include "render/stars.hpp"
#include "render/render.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"

#include "mathex/abox3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/double.hpp"

#include "stdlib/string.hpp"

#ifndef _INLINE
	#include "world4d/stars.ipp"
#endif

PER_DEFINE_PERSISTENT(W4dStars);

W4dStars::W4dStars(
	W4dEntity* pParent,
	RenStars::Configuration config,
	MATHEX_SCALAR radius,
	uint nStars)

	:W4dEntity(pParent, W4dTransform3d(), NOT_SOLID),
	opacity_(100.0),
	pStars_(_NEW(RenStars(config, radius, nStars)))
{
	// Fitting name I think.
	name("stars");

	// This is a horrid and evil hack, this **must** go.
	Ren::MeshPtr pMesh = RenMesh::createEmpty();
	ASSERT(pMesh.isDefined(), "The RenMesh::createEmpty() failed in W4dStars.");

	// To make sure that the bounding volume is computed correctly add a
	// dummy line that will force the volume we want.
	MexPoint3d minCorner;
	MexPoint3d maxCorner;

	if(config == RenStars::SPHERICAL)
	{
		minCorner.setPoint(-radius, -radius, -radius);
		maxCorner.setPoint(radius, radius, radius);
	}
	else if(config == RenStars::HEMISPHERICAL)
	{
		minCorner.setPoint(-radius, -radius, 0.0);
		maxCorner.setPoint(radius, radius, radius);
	}
	else
		ASSERT_FAIL("An invalid stars configuration was passed to W4dStars ctor.");

	// Doesn't matter what the material is as the mesh isn't rendered.
	pMesh->addLine(minCorner, maxCorner, RenMaterial());

	RenMeshInstance* pMeshInstance = _NEW(RenMeshInstance(pMesh));
	// W4dEntity deletes the pointer for you.
	add(pMeshInstance, MexDouble::plusInfinity(), W4dLOD(0));

	TEST_INVARIANT;
}

W4dStars::W4dStars(const W4dStars& copyMe, W4dEntity* pParent)

	:W4dEntity(copyMe, pParent, W4dTransform3d()),
	opacity_(copyMe.opacity()),
	pStars_(_NEW(RenStars(copyMe.configuration(), copyMe.radius(), copyMe.nStars())))
{
	TEST_INVARIANT;
}

// VIRTUAL //
W4dStars::~W4dStars( void )
{
	TEST_INVARIANT;

	_DELETE(pStars_);
}

// VIRTUAL //
bool W4dStars::intersectsLine(
	const MexLine3d& line,
	MATHEX_SCALAR* pDistance,
	Accuracy accuracy) const
{
    // Just check against bounding volume.
    return defaultIntersectsLine(line, pDistance, accuracy);
}

void W4dStars::CLASS_INVARIANT
{
	// Entities are removed runtime so this could happen.
	INVARIANT(this != NULL);
	INVARIANT(pStars_);
}

// PROTECTED VIRTUAL //
W4dEntity::RenderType W4dStars::render(const W4dCamera& camera, W4dComplexity& )
{
	TEST_INVARIANT;

	pStars_->render(
		opacity_,
    	camera.globalTransform(),
    	camera.verticalFOVAngle(),
    	camera.horizontalFOVAngle());

	// I think that no-one will place an entity below stars as it's obscure, but just in case.
	return RENDER_CHILDREN;
}

// PERSISTENCE //

void perWrite(PerOstream&, const W4dStars&)
{
	ASSERT_FAIL("W4dStars is not to be persisted by perWrite().");
}

void perRead(PerIstream&, W4dStars&)
{
	// Should assert out before it reaches here.
	ASSERT_FAIL("W4dStars is not to be persisted by perRead().");
}

W4dStars::W4dStars(PerConstructor con)

	:W4dEntity(con),
	// UtlPercentage has no default ctor.
	opacity_(100.0)
{
	ASSERT_FAIL("W4dStars is not to be persisted (per ctor).");
}

/* End STARS.CPP ******************************************/


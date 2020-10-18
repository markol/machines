/*
 * P O I N T S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/points.hpp"
#include "render/internal/inpoints.hpp"
#include "render/internal/glmath.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/colpack.hpp"
#include "mathex/point3d.hpp"
#include "render/colour.hpp"
#include "render/device.hpp"
#include "render/material.hpp"

RenPoints::RenPoints(size_t initial):
	points_(_NEW(RenIPoints(initial)))
{
    TEST_INVARIANT;
}

RenPoints::~RenPoints()
{
    TEST_INVARIANT;
	_DELETE(points_);
}

size_t RenPoints::nPoints() const
{
	return points_->size();
}

void RenPoints::addPoint(const MexPoint3d& p, const RenColour& c)
{
	// tu and tv are not initialised.  They shouldn't be used so garbage will do.
	RenIVertex vtx;
	vtx.x = p.x();
	vtx.y = p.y();
	vtx.z = p.z();
	vtx.color = packColourChecked(c.r(), c.g(), c.b(), c.a());
	vtx.specular = 0;

	points_->push_back(vtx);
}

void RenPoints::point(size_t idx, const MexPoint3d& p, const RenColour& c)
{
	PRE(idx < nPoints());
	RenIVertex& vtx = (*points_)[idx];

	// tu and tv are not set.  They shouldn't be used so garbage will do.
	vtx.x = p.x();
	vtx.y = p.y();
	vtx.z = p.z();
	vtx.color = packColourChecked(c.r(), c.g(), c.b(), c.a());
	vtx.specular = 0;
}

void RenPoints::render(const MexTransform3d& world) const
{
	PRE(RenDevice::current());
	RenDevice* dev = RenDevice::current();

	glm::mat4 glWorld;
	MexToGLMatrix(world, glWorld);
    dev->setModelMatrix(glWorld);

	RenMaterial withoutTexture;
	dev->setMaterialHandles(withoutTexture);

//	DWORD previous;
//	TRYRENDX(d3dDev->GetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, &previous));
//	TRYRENDX(d3dDev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true));

    RenIVertex* crufty = _STATIC_CAST(RenIVertex*, &(points_->front()));
    dev->renderPrimitive(crufty, points_->size(), withoutTexture, GL_POINTS);

//	TRYRENDX(d3dDev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, previous));
}

void RenPoints::CLASS_INVARIANT
{
    INVARIANT(this);
    INVARIANT(points_);
}

ostream& operator <<( ostream& o, const RenPoints& t )
{
    o << "RenPoints, size=" << t.nPoints() << ":" << std::endl;

	for (int i=0; i!=t.nPoints(); ++i)
		o << "  " << (*t.points_)[i] << std::endl;

    return o;
}

/* End POINTS.CPP ***************************************************/

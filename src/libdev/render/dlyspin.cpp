/*
 * DLYSPIN . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "mathex/point3d.hpp"
#include "render/internal/dlyspin.hpp"
#include "render/internal/glmath.hpp"
#include "render/internal/devicei.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/device.hpp"

RenIDelayedSpinPolygon::RenIDelayedSpinPolygon
(
	RenI::LitVtxAPtr&	v,			// array of spun and lit vertices
	size_t				nVtx,		// the number of elements in v
	const RenMaterial&	mat,		// material to render with
	const glm::mat4&	x			// the global xfrom of the STF
)
: RenIDepthSortedItem(mat),
  vertices_(v),
  nVertices_(nVtx),
  xform_(x)
{
	PRE(nVtx >= 3);

	// Pick an arbitrary vertex -- the first -- transform by the projection-
	// view matrix and use that as the basis for the depth sorting.  Using
	// the STF's base point might give better results??
	const glm::mat4& projView = RenIDeviceImpl::currentPimpl()->projViewMatrix();

	// Use the first vertex used by this group as the basis for the depth sort.
	const RenIVertex* tmp = vertices_.get();
	const RenIVertex& vtx = tmp[0];
	MexPoint3d point(vtx.x, vtx.y, vtx.z);
	MATHEX_SCALAR dummyW = 1;
	transformPt(projView, &point, &dummyW);

	depth_ = point.z();
}

RenIDelayedSpinPolygon::~RenIDelayedSpinPolygon()
{
}

void RenIDelayedSpinPolygon::render()
{
	// SetTransform is not const-correct w.r.t. its 2nd argument.
	glm::mat4* crufty = _CONST_CAST(glm::mat4*, &xform_);
    RenDevice::current()->setModelMatrix(*crufty);
	RenIDeviceImpl::currentPimpl()->setMaterialHandles(material_);

	RenIVertex* vtx = vertices_.get();
    RenDevice::current()->renderPrimitive(vtx, nVertices_, material_);
}

void RenIDelayedSpinPolygon::print(ostream& o) const
{
	o << "Delayed STF, depth=" << depth() << ", " << material_;
}

/* End DLYSPIN.CPP ***************************************************/

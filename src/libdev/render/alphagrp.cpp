/*
 * A L P H A G R P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "render/capable.hpp"
#include "render/internal/glmath.hpp"
#include "render/internal/alphagrp.hpp"
#include "render/internal/trigroup.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/devicei.hpp"
#include "render/internal/capablei.hpp"
#include "render/internal/matmgr.hpp"
#include "render/device.hpp"

RenIDelayedAlphaGroup::RenIDelayedAlphaGroup
(
	const RenIMaterialGroup*   g,
	RenI::LitVtxAPtr&	   v,
	const RenMaterial&     m,
	const glm::mat4&	   x
)
: RenIDepthSortedItem(m),
  group_(g),
  vertices_(v),
  xform_(x)
{
	PRE(group_);
	PRE(vertices_.get());

	// If this material is sorted on intra-mesh priority, then the depth must
	// agree with the depths of other groups in the mesh.  Hence, we must use
	// a fixed vertex index.  This is checked by an assertion in
	// RenIDepthSortedItem::operator<.
	static const Ren::VertexIdx zero = 0;
	const Ren::VertexIdx idx = (m.intraMeshAlphaPriority())? zero: group_->firstVertexIdx();

	glm::mat4 mx = RenIDeviceImpl::currentPimpl()->projViewMatrix() * xform_ ;

	// Use the first vertex used by this group as the basis for the depth sort.
	const RenIVertex* tmp = vertices_.get();
	const RenIVertex& vtx = tmp[idx];
	MexPoint3d point(vtx.x, vtx.y, vtx.z);
	MATHEX_SCALAR dummyW = 1;
	transformPt(mx, &point, &dummyW);

	depth_ = point.z();
}

// virtual
RenIDelayedAlphaGroup::~RenIDelayedAlphaGroup()
{
}

// virtual
void RenIDelayedAlphaGroup::render()
{
	// SetTransform is not const-correct w.r.t. its 2nd argument.
	glm::mat4* crufty = _CONST_CAST(glm::mat4*, &xform_);
    RenDevice::current()->setModelMatrix(*crufty);

	const RenICapabilities* caps = RenIDeviceImpl::currentPimpl()->capabilities().internal();
	ASSERT(caps, "No internal device capabilities defined.");

	// RENDER_STREAM("Rendering alpha group with " << material_ << "\n");

	// Note that coplanar priority and alpha priority are not mutually exclusive.
	// Apply a zBias value only if the device supports it (to avoid state changes).
	const bool doZBias = material_.interMeshCoplanar() && caps->supportsZBias();
	if (doZBias)
	{
		const int zBias = material_.coplanarPriority() - RenIMatManager::instance().minCoplanarValue();
		ASSERT_INFO(zBias);
		ASSERT(zBias >= caps->minZBias(), "Illegal zbias value in alpha sorter.");
		ASSERT(zBias <= caps->maxZBias(), "Illegal zbias value in alpha sorter.");

        glEnable( GL_POLYGON_OFFSET_FILL );
        glPolygonOffset( -(GLfloat)zBias, 1.0 );
		// RENDER_STREAM("  Set zBias=" << zBias << "\n");
	}

    if(!material_.usesBilinear() && !material_.texture().empty()) // This fixes issue with gun barrels rendering
    {
        glDepthMask(GL_TRUE);
        group_->render(vertices_, material_);
        glDepthMask(GL_FALSE);
    }
    else
        group_->render(vertices_, material_);

	// Unlike the coplanar groups, we cannot assume that the next item will set its
	// zbias.  So reset the drvier's state.  Perhaps we should track the current
	// value to avoid state changes.  Then agian, thre probably aren't enough of
	// them for it to be worthwhile.
	if (doZBias)
	{
        glDisable( GL_POLYGON_OFFSET_FILL );
        glPolygonOffset( (GLfloat)caps->minZBias(), 1.0 );
    }
}

void RenIDelayedAlphaGroup::print(ostream& o) const
{
	o << "Delayed, depth=" << depth() << ", meshId=" << meshId() << ", " << material_;
}


/* End DELAYGRP.CPP *************************************************/

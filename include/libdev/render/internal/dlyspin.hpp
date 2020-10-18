/*
 * DLYSPIN . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_TLPOLY_HPP
#define _RENDER_TLPOLY_HPP

#include "base/base.hpp"
#include "render/material.hpp"
#include "render/internal/polysord.hpp"
#include "render/internal/internal.hpp"
#include <glm/gtc/matrix_transform.hpp>

class RenIDelayedSpinPolygon : public RenIDepthSortedItem
{
public:
	// PRE(nVtx >= 3);
	RenIDelayedSpinPolygon(RenI::LitVtxAPtr& vertices, size_t nVtx, const RenMaterial& mat, const glm::mat4&);

	virtual ~RenIDelayedSpinPolygon();
	virtual void render();
	virtual void print(ostream&) const;

private:
	RenI::LitVtxAPtr			vertices_;
	const glm::mat4				xform_;
	const size_t				nVertices_;
};

#endif

/* End DLYSPIN.HPP ***************************************************/

/*
 * D E L A Y C O P . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_DELAYCOPLANARGRP_HPP
#define _RENDER_DELAYCOPLANARGRP_HPP

#include "base/base.hpp"
#include "render/material.hpp"
#include "render/internal/internal.hpp"
#include "render/internal/polysorp.hpp"

class RenIMaterialGroup;
class RenIVertexData;

// A RenIMaterialGroup which cannot be drawn in order.  These are pushed onto a
// list, then sorted and drawn after all other rendering is complete.
class RenIDelayedCoplanarGroup : public RenIPrioritySortedItem
{
public:
    RenIDelayedCoplanarGroup(const RenIMaterialGroup*, RenI::LitVtxAPtr&, const RenMaterial&, const glm::mat4&);

	virtual ~RenIDelayedCoplanarGroup();
	virtual void render();

protected:
    // Operations deliberately revoked.
    RenIDelayedCoplanarGroup( const RenIDelayedCoplanarGroup& );
    RenIDelayedCoplanarGroup& operator =( const RenIDelayedCoplanarGroup& );
    bool operator ==( const RenIDelayedCoplanarGroup& );

	virtual void print(ostream&) const;

    const RenIMaterialGroup*	group_;
	RenI::LitVtxAPtr			vertices_;
	const glm::mat4				xform_;
};

#endif

/* End DELAYCOP.HPP *************************************************/

/*
 * N O N M M X . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    RenINonMMXIlluminator

	A concrete class derived from RenIIlluminator which implements lighting
	calculations in the absence of an MMX CPU.
*/

#ifndef _RENDER_NONMMX_HPP
#define _RENDER_NONMMX_HPP

#include "base/base.hpp"
#include "render/internal/illumine.hpp"

class RenIFloatLightingBuffer;

class RenINonMMXIlluminator : public RenIIlluminator
{
public:
	RenINonMMXIlluminator(RenIDeviceImpl*);
	virtual ~RenINonMMXIlluminator();

	virtual const RenIVertex* applyMaterial(const RenMaterial&, const RenIVertexData& in, const Indices&);

private:
	virtual void computeLambertian(const RenIVertexData&, const MexTransform3d& world, const MexAlignedBox3d* pVolume);
	virtual void applyVertexMaterials(const RenIVertexData& in);
	virtual void doStartFrame();
	bool overflowPossible(const RenIMatBody*) const;
	bool specularRequired() const;

	RenIFloatLightingBuffer*	lightingBuffer_;
	ulong		   				overflows_, applications_;
};

#endif

/* End NONMMX.HPP ***************************************************/

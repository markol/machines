/*
 * M M X . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    RenIMMXIlluminator

	A concrete class derived from RenIIlluminator which implements lighting
	calculations using the mmxlight library.
*/

#ifndef _RENDER_MMX_HPP
#define _RENDER_MMX_HPP

#include "base/base.hpp"
#include "render/internal/illumine.hpp"

class RenIFixPtLightingBuffer;
class RenIVertex;

class RenIMMXIlluminator : public RenIIlluminator
{
public:
	RenIMMXIlluminator(RenIDeviceImpl*);
	virtual ~RenIMMXIlluminator();

	virtual const RenIVertex* applyMaterial(const RenMaterial&, const RenIVertexData& in, const Indices&);

private:
	virtual void computeLambertian(const RenIVertexData&, const MexTransform3d& world, const MexAlignedBox3d* pVolume);
	virtual void applyVertexMaterials(const RenIVertexData& in);

	RenIFixPtLightingBuffer*	lightingBuffer_;
};

#endif

/* End MMX.HPP ******************************************************/

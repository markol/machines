/*
 * M A T A P P L Y . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_MATAPPLY_HPP
#define _RENDER_MATAPPLY_HPP

#include "base/base.hpp"
#include "render/internal/matbody.hpp"
#include "render/internal/illumine.hpp"
#include "render/colour.hpp"

class RenIExpandedIntensityMap;
class RenIFloatLightingBuffer;

////////////////////////////////////////////////////////////////////////////
// Base class for applying materials to vertices.  This does *not* have a
// virtual apply method because the application functions must be inline.
// The intensity map is unused in the calculations at present.  This feature
// isn't required for Machines and the code is more efficient without it.
class RenIMaterialApplicator
{
public:
	RenIMaterialApplicator(const RenIMatBody*, const RenColour& ambient, const RenIExpandedIntensityMap*, bool, const RenColour& filter);

	void perVertexMats(bool b)		{ perVertexMats_ = b; }
	void specularRequired(bool b)	{ specularRequired_ = b; }

protected:
	void applySpecular(RenIVertex& vtx, float lambR, float lambG, float lambB) const;

	// Initialisation order of alpha_ is important.
	const float difR_, difG_, difB_, constR_, constG_, constB_;
	ulong alpha_;
	const uint packedAmbient_, packedBlack_;

	// packedAmbient_ can only be used if every entry in map_ is 1.0.
	// Otherwise, the ambient must be multiplied by the vertex intensity.
	const bool mapUnity_;
	const RenIExpandedIntensityMap* const map_;

	// Have per-vertex materials been applied?
	bool perVertexMats_;

	// Is specular oversaturation indicated?
	bool specularRequired_;

	// This is a filter which is applied to every output colour.
	const RenColour	filter_;
};

////////////////////////////////////////////////////////////////////////////
// Performs calculation required to apply a material to a lit vertex.  It is
// assumed that the lighting parameters and the material are such that the
// calculation cannot overflow, hence, no bounds checking is done.
class RenINoOverflowMatApp : public RenIMaterialApplicator
{
public:
	RenINoOverflowMatApp(const RenIMatBody*, const RenColour&, const RenIExpandedIntensityMap*, bool, const RenColour& filter);
	void applyToVertex(RenIVertex& vtx, float lambR, float lambG, float lambB) const;
	void applyViaIndices(const RenIIlluminator::Indices&, RenIFloatLightingBuffer*) const;
	void applyDirectToVertices(RenIFloatLightingBuffer*, size_t nVertices) const;
};

////////////////////////////////////////////////////////////////////////////
// Performs calculation required to apply a material to a lit vertex.  The
// lighting calculation *can* overflow, so bounds checking is done.
class RenICheckedMatApp : public RenIMaterialApplicator
{
public:
	RenICheckedMatApp(const RenIMatBody*, const RenColour&, const RenIExpandedIntensityMap*, bool, const RenColour& filter);
	void applyToVertex(RenIVertex& vtx, float lambR, float lambG, float lambB) const;
	void applyViaIndices(const RenIIlluminator::Indices&, RenIFloatLightingBuffer*) const;
	void applyDirectToVertices(RenIFloatLightingBuffer*, size_t nVertices) const;
};

////////////////////////////////////////////////////////////////////////////
// An optimisation which can be applied if there is no diffuse lighting.
// This happens for emissive only materials or when the lighting module is
// disabled (when the background is drawn).
class RenIUnlitMatApp : public RenIMaterialApplicator
{
public:
	RenIUnlitMatApp(const RenIMatBody*, const RenColour&, const RenIExpandedIntensityMap*, bool, const RenColour& filter);
	void applyToVertex(RenIVertex& vtx) const;
	void applyViaIndices(const RenIIlluminator::Indices&, RenILightingBuffer*) const;
	void applyDirectToVertices(RenILightingBuffer*, size_t nVertices) const;
};

#ifdef _INLINE
    #include "internal/matapply.ipp"
#endif

#endif

/* End MATAPPLY.HPP *************************************************/

/*
 * M A T A P P L Y . I P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include "render/internal/colpack.hpp"

// The colour filter is multiplied into the material properties in order to
// avoid a per-vertex multiplication.
_CODE_INLINE
RenIMaterialApplicator::RenIMaterialApplicator
(
	const RenIMatBody* body,
	const RenColour& ambient,
	const RenIExpandedIntensityMap* map,
	bool /*mapUnity*/,						// ignored: per-vertex intensity disabled
	const RenColour& filter
):
	difR_(body->diffuse().r() * filter.r()),
	difG_(body->diffuse().g() * filter.g()),
	difB_(body->diffuse().b() * filter.b()),
	constR_(filter.r() * (ambient.r() * body->ambient().r() + body->emissive().r())),
	constG_(filter.g() * (ambient.g() * body->ambient().g() + body->emissive().g())),
	constB_(filter.b() * (ambient.b() * body->ambient().b() + body->emissive().b())),
	alpha_(body->diffuse().packedAlpha()),
	packedAmbient_(packColourChecked(constR_, constG_, constB_, alpha_)),
	packedBlack_(packColourChecked(0,0,0, 0)),
	map_(map),
	mapUnity_(true),						// per-vertex intensity disabled
	perVertexMats_(false),
	specularRequired_(false),
	filter_(filter)
{
}

_CODE_INLINE
void RenIMaterialApplicator::applySpecular(RenIVertex& vtx, float lambR, float lambG, float lambB) const
{
	if (specularRequired_)
	{
		// Set the specular to a fraction of the light colour, *not* the
		// polygon colour.  What is the correct way to combine the filter
		// and overflow effects?  I think this will look OK.
		const float specR = (lambR > 1)? filter_.r() * (lambR-1): 0;
		const float specG = (lambG > 1)? filter_.g() * (lambG-1): 0;
		const float specB = (lambB > 1)? filter_.b() * (lambB-1): 0;

		// Specular alpha is used for per-vertex fogging.  This is currently
		// unsupported, so the correct value is zero.
		vtx.specular = packColourChecked(specR, specG, specB, 0);

		// RENDER_STREAM("packing: lamb=(" << lambR << ","  << lambG << ","  << lambB << ")");
		// RENDER_STREAM(" spec=(" << specR << ","  << specG << ","  << specB << ")\n");
	}
	else
	{
		vtx.specular = packedBlack_;
	}
}

_CODE_INLINE
RenINoOverflowMatApp::RenINoOverflowMatApp
(
	const RenIMatBody* body,
	const RenColour& ambient,
	const RenIExpandedIntensityMap* map,
	bool mapUnity,
	const RenColour& filter
):
	RenIMaterialApplicator(body, ambient, map, mapUnity, filter)
{
}

_CODE_INLINE
void RenINoOverflowMatApp::applyToVertex(RenIVertex& vtx, float lambR, float lambG, float lambB) const
{
	const float r = lambR * difR_ + constR_;
	const float g = lambG * difG_ + constG_;
	const float b = lambB * difB_ + constB_;
	vtx.color = packColourUnChecked(r,g,b, alpha_);
	applySpecular(vtx, lambR, lambG, lambB);
}

_CODE_INLINE
RenICheckedMatApp::RenICheckedMatApp
(
	const RenIMatBody* body,
	const RenColour& ambient,
	const RenIExpandedIntensityMap* map,
	bool mapUnity,
	const RenColour& filter
):
	RenIMaterialApplicator(body, ambient, map, mapUnity, filter)
{
}

_CODE_INLINE
void RenICheckedMatApp::applyToVertex(RenIVertex& vtx, float lambR, float lambG, float lambB) const
{
	const float r = lambR * difR_ + constR_;
	const float g = lambG * difG_ + constG_;
	const float b = lambB * difB_ + constB_;
	vtx.color = packColourChecked(r,g,b, alpha_);
	applySpecular(vtx, lambR, lambG, lambB);
}

_CODE_INLINE
RenIUnlitMatApp::RenIUnlitMatApp
(
	const RenIMatBody* body,
	const RenColour& ambient,
	const RenIExpandedIntensityMap* map,
	bool mapUnity,
	const RenColour& filter
):
	RenIMaterialApplicator(body, ambient, map, mapUnity, filter)
{
}

_CODE_INLINE
void RenIUnlitMatApp::applyToVertex(RenIVertex& vtx) const
{
	vtx.color    = packedAmbient_;
	vtx.specular = packedBlack_;
}

/* End MATAPPLY.IPP *************************************************/

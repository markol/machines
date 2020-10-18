/*
 * I L L U M I N E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "render/internal/illumine.hpp"

#include "render/light.hpp"
#include "render/mesh.hpp"
#include "render/camera.hpp"
#include "render/internal/inlight.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/lightbuf.hpp"
#include "render/internal/matbody.hpp"
#include "render/internal/glmath.hpp"
#include "render/internal/intmap.hpp"
#include "render/internal/vtxmat.hpp"
#include "render/internal/colpack.hpp"
#include "render/internal/matmgr.hpp"
#include "render/internal/devicei.hpp"
#include "render/device.hpp"

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <algorithm>

#ifndef _INLINE
    #include "render/internal/illumine.ipp"
#endif

RenIIlluminator::RenIIlluminator(RenIDeviceImpl* d):
	devImpl_(d),
	ambient_(RenColour(0.3)),
	disabled_(false),
	internallyDisabled_(false),
	perVertexMats_(false),
	expanded_(NULL),
	expansion_(NULL),
	lightingBuffer_(NULL),
	globalMaterialXform_(NULL)
{
	PRE(d);

    lightsOn_.reserve( 64 );

	// NB: we can't test the invariant here.  The invariant requires that the
	// owning RenDevice is correctly set-up.  This object is created by the
	// RenDevice ctor at which point the device isn't 100% initialised, so the
	// invariant doesn't make any sense.
}

RenIIlluminator::~RenIIlluminator()
{
	// No invariant test.  See note in ctor.
	// All the RenILights are owned by their corresponding RenLight and should
	// *not* be deleted.  The lighting buffer is owned by the derived class.
	_DELETE(expansion_);
}

void RenIIlluminator::useLightingBuffer(RenILightingBuffer* l)
{
	PRE(l);
	lightingBuffer_ = l;
}

void RenIIlluminator::startFrame()
{
	// The derived class must call useLightingBuffer before doing any
	// calculations (probably in it's ctor).
	PRE(lightingBuffer_);

	computeGeneric();

	// This assumes that meshes do not change during a render-pass.  All the
	// RenMesh::add* methods should have appropriate pre-conditions.
	lightingBuffer_->checkCapacity(RenMesh::maxVertices());

	// Per-vertex intensities are disabled because they're not required in Machines.
	//if (!expansion_)
	//	expansion_ = _NEW(RenIExpandedIntensityMap(RenMesh::maxVertices()));
	//ASSERT(expansion_, "No expanded intensity map in illuminator.");

	//expansion_->checkSize(RenMesh::maxVertices());
	//expanded_ = NULL;

	// Let the subclass do it's start frame initialisation (if any).
	doStartFrame();

	ctl_list<RenILight*>::iterator lightIt = lights_.begin();
	while (lightIt != lights_.end())
	{
		RenILight* light = *lightIt;
		ASSERT(light, "Null internal light in illuminator.");

		// TBD: could be removed from the production version.
		light->used(false);

		if (light->on())
			light->startFrame();

		++lightIt;
	}

	// If the material manager has a global material transform, get it and use
	// it for the next frame.  (The transform is not stored here because there
	// may be more than one illuminator, but there's only one material manager.)
	globalMaterialXform_ = RenIMatManager::instance().globalMaterialTransform();

	POST_INFO(lightingBuffer_->capacity());
	POST_INFO(RenMesh::maxVertices());
	POST(lightingBuffer_->capacity() >= RenMesh::maxVertices());
}

// Template method: the default does nothing.
// virtual
void RenIIlluminator::doStartFrame()
{
}

void RenIIlluminator::computeGeneric()
{
	genericLambertian_ = RenColour::black();

	// Add each light to the generic illumination.
	ctl_list<RenILight*>::const_iterator lightIt = lights_.begin();
	while (lightIt != lights_.end())
	{
		const RenILight* light = *lightIt;
		ASSERT(light, "Null internal light in illuminator.");

		if (light->on())
			light->accumulateGeneric(&genericLambertian_);

		++lightIt;
	}
}

void RenIIlluminator::addLight(RenLight* light)
{
	if (light)
	{
		RenILight* inLight = light->pILight();
		ASSERT(inLight, "Valid light has null internal light.");
		lights_.push_back(inLight);
		inLight->illuminator(this);

		if (inLight->on())
			lightTurnedOn(inLight);
	}
}

void RenIIlluminator::removeLight(RenLight* light)
{
	if (light)
	{
		RenILight* inLight = light->pILight();
		ASSERT(inLight, "Valid light has null internal light.");

		// Verify that the light is in the list.
		ASSERT_DATA(ctl_list<RenILight*>::iterator it = std::find(lights_.begin(), lights_.end(), inLight));
		ASSERT(it != lights_.end(), "Removed a light which wasn't previously added.");
		lights_.remove(inLight);
		inLight->illuminator(NULL);

		if (inLight->on())
		{
		    ctl_pvector<RenILight>::iterator it = std::find(lightsOn_.begin(), lightsOn_.end(), inLight);
			ASSERT(it != lightsOn_.end(), "On light not found in list.");
			lightsOn_.erase(it);
		}
	}
}

void RenIIlluminator::lightTurnedOn(RenILight* l)
{
	PRE(l);
	PRE(l->on());
	ASSERT(lightsOn_.end() == std::find(lightsOn_.begin(), lightsOn_.end(), l), "Light already in vector.");
	lightsOn_.push_back(l);

	internallyDisabled_ = disabled_;
}

void RenIIlluminator::lightTurnedOff(RenILight* l)
{
	PRE(l);
	PRE(!l->on());
	PRE(lightsOn_.size() > 0);
	ASSERT_DATA(size_t startSize = lightsOn_.size());

	// Clients are likely to coherently turn lights on and off in pairs.  Thus
	// it's likely that the light we are being asked to turn off is at the end
	// of the vector.  Check this first as it's probalby quicker.
	if (lightsOn_.back() == l)
	{
		lightsOn_.pop_back();
	}
	else
	{
		// It's not the last light, so there must be more than 1.
		ASSERT(lightsOn_.size() >= 2, "Light not in vector of turned on lights.");

		// The light isn't at the end, so we need to search thru the list.
		// By the same logic, searching backwards through the list will
		// probably find the light quickly.
	    ctl_pvector<RenILight>::iterator it = lightsOn_.end();
		--it;

	    while (it != lightsOn_.begin())
	    {
			--it;

			if (*it == l)
			{
				lightsOn_.erase(it);
				break;
			}
	    }
	}

	if (lightsOn_.size() == 0)
		internallyDisabled_= true;

	// Verify that the above logic did indeed remove a light.
	ASSERT(lightsOn_.size() == startSize-1, "Failed to remove light from vector.");
}

void RenIIlluminator::lightVertices
(
	const RenIVertexData& in,
	size_t nVertices,					// defines a sub-set of in
	const MexAlignedBox3d* pVolume
)
{
	// The 1st pre-condition could fail if an STF has more vertices than the
	// largest (non-STF) mesh.  This is unlikely as most STFs have 4 vertices.
	PRE_INFO(in.size());
	PRE_INFO(RenMesh::maxVertices());
	PRE(in.size() <= RenMesh::maxVertices());
	PRE(in.size() <= lightingBuffer_->capacity());
	PRE(nVertices <= in.size());
	PRE_DATA(litData_ = &in);

	glm::mat4 glWorld;
	glWorld = RenDevice::current()->getModelMatrix();
	MexTransform3d mexWorld;
    GLMatrixToMex(glWorld, &mexWorld);

	// Per-vertex fog is disabled for now.  TBD: put it back in.
	// The real version: const bool isFogOn = device_->isFogOn();
	//static const bool isFogOn = FALSE_NON_CONST;
	//if (isFogOn)
	//	computeFogAttenuation(in, mexWorld);

	// Not required now that memcpy is used to initialise lit vertices?
	// vtxBuffer_->setAllSpecular(RenColour::black());
	lightingBuffer_->copyCoords(in);

	if (!disabled())
		computeLambertian(in, mexWorld, pVolume);

	// If these vertices have an intensity map, then create an expanded version.
	const RenIVertexIntensities* intMap = in.intensityMap();

	// For now, support for per-vertex intensities is disabled because we don't
	// use this feature in Machines and the subsequent lighting code can be more
	// efficient without it.
	ASSERT(!intMap, "Mesh has intensity map.");

	if (intMap != expanded_)
	{
		// This should be allocated by start frame.
		ASSERT(expansion_, "No expanded intensity map in illuminator.");
		expanded_ = intMap;
		expansion_->expand(expanded_);
	}

	// If these vertices have a material map, then apply the material at
	// whichever vertices are affected.
	const RenIVertexMaterials* matMap = in.materialMap();
	perVertexMats_ = (matMap != NULL);

	if (perVertexMats_)
	{
		// Reset these flags to indicate that each vertex doesn't (yet) have its
		// own material applied to it.
		lightingBuffer_->setAllPerVertexFlags(false);

		// This function will set some of the above flags to true.
		applyVertexMaterials(in);
	}

	POST(lightingBuffer_->sizeMatches(nVertices));
	POST(litData_ == &in);
}

void RenIIlluminator::lightVertices
(
	const RenIVertexData& in,
	const MexAlignedBox3d* pVolume
)
{
	PRE(in.size() <= RenMesh::maxVertices());

	lightVertices(in, in.size(), pVolume);

	POST(lightingBuffer_->sizeMatches(in.size()));
	POST(litData_ == &in);
}

/* TBD: put vertex fog back in; create RenIMaterialApplicator subclasses for
 * fog.  Fog calculation looks like this:
	const RenColour& fogColour = device_->fogColour();
	if (isFogOn)
	{
		// Blend the fog colour with the calculated reflected colour,
		// using the attenuation factor as the blend proportion.
		const float att = *pAtts++;
		const float oneMinusAtt = 1 - att;
		const float r = oneMinusAtt * (lambR * difR + constR) + att * fogColour.r();
		const float g = oneMinusAtt * (lambG * difG + constG) + att * fogColour.g();
		const float b = oneMinusAtt * (lambB * difB + constB) + att * fogColour.b();

		vtx.color = RenICheckedMatApp(r,g,b, alpha_);
	}
*/

const RenIVertex* RenIIlluminator::applyMaterial
(
	const RenMaterial& m,
	const RenIVertexData& in
)
{
	PRE(in.size() == litData_->size());
	PRE(lightingBuffer_->sizeMatches(in.size()));
	return applyMaterial(m, in, in.size());
}

RenI::LitVtxAPtr RenIIlluminator::applyMaterialAndCopy
(
	const RenMaterial& m,
	const RenIVertexData& in,
	const ctl_min_memory_vector<Ren::VertexIdx>& indices,
	Ren::VertexIdx nIndicesUsed
)
{
	PRE(nIndicesUsed <= in.size());

	const RenIVertex* vtx = applyMaterial(m, in, indices);
	RenIVertex* copy = _NEW_ARRAY(RenIVertex, nIndicesUsed);
	memcpy(copy, vtx, nIndicesUsed * sizeof(RenIVertex));
	return copy;
}

RenI::LitVtxAPtr RenIIlluminator::applyMaterialAndCopy
(
	const RenMaterial& m,
	const RenIVertexData& in,
	size_t nVertices
)
{
	PRE(nVertices <= in.size());

	const RenIVertex* vtx = applyMaterial(m, in, nVertices);
	RenIVertex* copy = _NEW_ARRAY(RenIVertex, nVertices);
	memcpy(copy, vtx, nVertices * sizeof(RenIVertex));
	return copy;
}

static void resizeIndices(ctl_min_memory_vector<Ren::VertexIdx>& v, size_t newSize)
{
	if (newSize > v.size())
	{
		v.reserve(newSize);

		// Each element of the vector is it's own index.
		for (size_t i=v.size(); i!=newSize; ++i)
			v.push_back(_STATIC_CAST(Ren::VertexIdx, i));
	}
	else if (newSize < v.size())
	{
		v.erase(v.begin() + newSize, v.end());
	}

	POST(v.size() == newSize);
}

// Construct a vector of indices and call the virtual method which applys the
// material indirectly via the indices.  This can be done slightly more
// efficiently without the indirection.  However, a) it is only called for spin-
// to-face polys, i.e., infrequently; b) only 3 or 4 vertices are involved;
// c) it simplifies the code that does the low-level work; and d) it means we
// can considerably reduce the amount of low-level code.

const RenIVertex* RenIIlluminator::applyMaterial
(
	const RenMaterial& mat,
	const RenIVertexData& in,
	size_t nVertices
)
{
	static ctl_min_memory_vector<Ren::VertexIdx> indices;

	// This function is used by spin-to-face polys.  The resize might be horribly
	// inefficient; however, spin-to-face polys always have 4 (occaisionally 3)
	// vertices, so the resize should not be expensive.
	resizeIndices(indices, nVertices);

	applyMaterial(mat, in, indices);
	return lightingBuffer_->vertices();
}

void RenIIlluminator::applyMaterialGenericLighting
(
	const RenMaterial& mat,
	RenIVertex* vertices,
	size_t nVertices
)
{
	PRE(vertices);

	RenColour result = genericLambertian_;
	result *= mat.diffuse();
	RenColour amb = ambient_;
	amb *= mat.ambient();
	result += amb;
	result += mat.emissive();

	// Important: apply the filter to the result.
	result *= filter_;

	// Assemble the result into D3D format and modify all the input vertices.
	// The above calculations may have done all sorts of things to the alpha
	// value, depending on the emissive and ambient alpha values.  The one we
	// should use is the diffuse alpha (unmodified).
	const uint glResult = packColourChecked(result.r(), result.g(), result.b(), mat.diffuse().packedAlpha());
	static const uint packedBlack = packColourUnChecked(0,0,0, 0);

	for (int i=0; i!=nVertices; ++i)
	{
		{
			vertices[i].color = glResult;
		}
		vertices[i].specular = packedBlack;
	}
}

void RenIIlluminator::CLASS_INVARIANT
{
	INVARIANT(devImpl_);
	INVARIANT(implies(RenIDeviceImpl::currentPimpl()->rendering(), lightingBuffer_->capacity() >= RenMesh::maxVertices()));
}

ostream& operator <<( ostream& o, const RenIIlluminator& t )
{
	// TBD: if/when you can print a RenILight, make this a bit more verbose.
	o << "Scene illuminator with " << t.lights_.size() << " lights\n";
    return o;
}

/* End ILLUMINE.CPP *************************************************/

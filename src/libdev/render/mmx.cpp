/*
 * M M X . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "render/internal/mmx.hpp"
#include "render/internal/matbody.hpp"
#include "render/internal/inlight.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/lightbuf.hpp"
#include "render/internal/intmap.hpp"
#include "render/internal/vtxmat.hpp"
#include "render/internal/matapply.hpp"
//#include "mmxlight/matapply.h"

RenIMMXIlluminator::RenIMMXIlluminator(RenIDeviceImpl* dev):
	RenIIlluminator(dev),
	lightingBuffer_(_NEW(RenIFixPtLightingBuffer))
{
	useLightingBuffer(lightingBuffer_);
}

// virtual
RenIMMXIlluminator::~RenIMMXIlluminator()
{
	_DELETE(lightingBuffer_);
}

// virtual
void RenIMMXIlluminator::computeLambertian
(
	const RenIVertexData& in,
	const MexTransform3d& world,
	const MexAlignedBox3d* pVolume
)
{
	PRE(!disabled());

	// We use this action for the first light and ACCUMULATE for all others.
	RenILight::OutputAction sumType = RenILight::INITIALISE;
	ctl_pvector<RenILight>::const_iterator lightIt = lightsOn().begin();

	while (lightIt != lightsOn().end())
	{
		const RenILight* light = *lightIt;
		ASSERT(light, "Null internal light in illuminator.");
		ASSERT(light->on(), "List of lights contains an off light.");

		light->lightVerticesMMX(in, lightingBuffer_, world, sumType, pVolume);
		sumType = RenILight::ACCUMULATE;

		++lightIt;
	}
}

// Prevent an "unused argument" warning.
#ifdef NDEBUG
#define SECONDARG
#else
#define SECONDARG in
#endif

static char mmxDebugBuffer[1];

// virtual
const RenIVertex* RenIMMXIlluminator::applyMaterial
(
	const RenMaterial& m,
	const RenIVertexData& SECONDARG,
	const ctl_min_memory_vector<Ren::VertexIdx>& indices
)
{
    /*
	PRE_INFO(in.size());
	PRE(litData_ == &in);
	PRE(litData_->size() == in.size());
	PRE(lightingBuffer_->sizeMatches(in.size()));

	// Reset all the flags (quickly).
	lightingBuffer_->setAllAppliedFlags(0);

	// TBD: for vertex fog to work correctly the fog value must be calculated
	// at each vertex and put into the specular part of the colour.
	// The real version: const bool isFogOn = device_->isFogOn();
	// const RenColour& fogColour = device_->fogColour();

	const RenIMatBody* body = matBody(m);
	ASSERT(body, "Expected a valid ptr to material body.");
	const bool mapUnity = expanded() == NULL;

	if (disabled())
	{
		RenIUnlitMatApp app(body, ambient(), expansion(), mapUnity, filter());
		app.perVertexMats(perVertexMats());
		app.applyViaIndices(indices, lightingBuffer_);
	}
	else
	{
		MmxMaterial mmxMat;
		mmxMat.diffuse_.r_ = body->diffuse().r();
		mmxMat.diffuse_.g_ = body->diffuse().g();
		mmxMat.diffuse_.b_ = body->diffuse().b();
		mmxMat.diffuse_.a_ = body->diffuse().a();
		mmxMat.emissive_.r_ = body->emissive().r();
		mmxMat.emissive_.g_ = body->emissive().g();
		mmxMat.emissive_.b_ = body->emissive().b();
		mmxMat.emissive_.a_ = 0;

		MmxColourFloat amb, filt;
		amb.r_ = ambient().r();
		amb.g_ = ambient().g();
		amb.b_ = ambient().b();
		filt.r_ = filter().r();
		filt.g_ = filter().g();
		filt.b_ = filter().b();

		MmxVertexIndices mmxIndices;
		mmxIndices.indices_ = &(indices[0]);
		mmxIndices.nIndices_ = indices.size();

		MmxLightingOutputs liteOut;
		liteOut.lamberts_          = lightingBuffer_->lamberts();
		liteOut.matAppliedFlags_   = lightingBuffer_->matAppliedFlags();
		liteOut.perVertexMatFlags_ = lightingBuffer_->perVertexMatFlags();
		liteOut.nUsed_			   = lightingBuffer_->nValid();

		D3DLVERTEX* d3dVtx = lightingBuffer_->vertices();

		// RENDER_STREAM("Starting MMX calculations:\n");
		// mmxDebugBuffer[0] = '\0';

		if (perVertexMats())
			mmxApplyMaterialPerVertex(&liteOut, &mmxMat, &amb, &filt, &mmxIndices, d3dVtx, mmxDebugBuffer);
		else
			mmxApplyMaterial(&liteOut, &mmxMat, &amb, &filt, &mmxIndices, d3dVtx, mmxDebugBuffer);

		// RENDER_STREAM(mmxDebugBuffer << "MMX done\n\n");
	}
*/
	return lightingBuffer_->vertices();
}

void RenIMMXIlluminator::applyVertexMaterials
(
	const RenIVertexData& in
)
{
	PRE_INFO(in.size());
	PRE(in.materialMap());
	PRE(litData_ == &in);
	PRE(litData_->size() == in.size());
	PRE(lightingBuffer_->sizeMatches(in.size()));

//	D3DLVERTEX*  vertices   = lightingBuffer_->vertices();
	RenIVertex*  vertices   = lightingBuffer_->vertices();
	const bool	 mapUnity   = expanded() == NULL;
	bool*		 pPerVertex = lightingBuffer_->perVertexMatFlags();

	static ctl_min_memory_vector<Ren::VertexIdx> indices;
	static bool first = true;

	if (first)
	{
		first = false;
		indices.push_back(0);
	}

	const RenIVertexMaterials* mats = in.materialMap();
	RenIVertexMaterials::const_iterator it = mats->begin();
	while (it != mats->end())
	{
		const RenIMatBody*   body = matBody((*it).material());
		const Ren::VertexIdx idx  = (*it).index();
		ASSERT(body, "Expected a valid ptr to material body.");

		// Apply the material to the vertex.  As each material-vertex
		// combination could be unique, there's not much point in optimising
		// the calculations (compared with the applyMaterial methods).
		if (disabled())
		{
			RenIUnlitMatApp app(body, ambient(), expansion(), mapUnity, filter());
			app.perVertexMats(perVertexMats());
			app.applyToVertex(vertices[idx]);
		}
		else
		{
			// TBD: could the efficiency be improved?  Maybe it's not significant
			// enough.  (It depends on how much per-vertex stuff is built.)
			ASSERT(indices.size() == 1, "Index array is wrong size.");
			indices[0] = idx;
			applyMaterial((*it).material(), in, indices);
		}

		// Set the flag to indicate that this vertex has its own material.
		pPerVertex[idx] = true;

		++it;
	}
}

/* End MMX.CPP ******************************************************/

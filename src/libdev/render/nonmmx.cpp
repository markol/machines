/*
 * N O N M M X . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "render/internal/nonmmx.hpp"
#include "render/internal/matbody.hpp"
#include "render/internal/inlight.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/lightbuf.hpp"
#include "render/internal/intmap.hpp"
#include "render/internal/vtxmat.hpp"
#include "render/internal/colpack.hpp"
#include "render/internal/matapply.hpp"

RenINonMMXIlluminator::RenINonMMXIlluminator(RenIDeviceImpl* dev):
	RenIIlluminator(dev),
	lightingBuffer_(_NEW(RenIFloatLightingBuffer)),
	overflows_(0),
	applications_(0)
{
	useLightingBuffer(lightingBuffer_);
}

// virtual
RenINonMMXIlluminator::~RenINonMMXIlluminator()
{
	_DELETE(lightingBuffer_);
}

// Template method: the default does nothing.
// virtual
void RenINonMMXIlluminator::doStartFrame()
{
	static const char* const showOverFlow = getenv("CB_SHOW_OVERFLOW");
	if (showOverFlow && applications_ > 0)
	{
		ostream& o = Ren::out();
		const std::ios::fmtflags oldFlags = o.flags();
		const int oldPrecision       = o.precision();

		o.precision(1);
		o.setf(std::ios::fixed, std::ios::floatfield);

		const double percent = 100.0 * overflows_ / applications_;
		o << "Lighing overflowed in " << overflows_ << " (" << percent << "%) cases\n";

		o.precision(oldPrecision);
		o.setf(oldFlags);
	}

	overflows_ = applications_ =0;
}

// virtual
void RenINonMMXIlluminator::computeLambertian
(
	const RenIVertexData& in,
	const MexTransform3d& world,
	const MexAlignedBox3d* pVolume
)
{
	PRE(!disabled());

	lightingBuffer_->resetMaxLambertian();
	lightingBuffer_->setAllUnderflowFlags(false);

	// We use this action for the first light and ACCUMULATE for all others.
	RenILight::OutputAction sumType = RenILight::INITIALISE;
	ctl_pvector<RenILight>::const_iterator lightIt = lightsOn().begin();

	while (lightIt != lightsOn().end())
	{
		const RenILight* light = *lightIt;
		ASSERT(light, "Null internal light in illuminator.");
		ASSERT(light->on(), "List of lights contains an off light.");

		// Is this the last light in the list?
		const bool last = (light == lightsOn().back());
		light->lightVertices(in, lightingBuffer_, world, sumType, last, pVolume);

		sumType = RenILight::ACCUMULATE;
		++lightIt;
	}
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

// With the current lighting conditions, is an overflow possible when this
// material is applied?  We can predict this because we have computed the max
// reflected Lambertian intensity.  In the majority of cases with one light,
// no emissive and sensible ambient, the calculations won't overflow.
bool RenINonMMXIlluminator::overflowPossible(const RenIMatBody* body) const
{
	const float matDfR = body->diffuse().r();
	const float matDfG = body->diffuse().g();
	const float matDfB = body->diffuse().b();
	const float matAmR = body->ambient().r();
	const float matAmG = body->ambient().g();
	const float matAmB = body->ambient().b();
	const float matEmR = body->emissive().r();
	const float matEmG = body->emissive().g();
	const float matEmB = body->emissive().b();
	const float maxLambR = lightingBuffer_->maxLambertianR();
	const float maxLambG = lightingBuffer_->maxLambertianG();
	const float maxLambB = lightingBuffer_->maxLambertianB();

	const float maxR = maxLambR*matDfR + ambient().r()*matAmR + matEmR;
	const float maxG = maxLambG*matDfG + ambient().g()*matAmG + matEmG;
	const float maxB = maxLambB*matDfB + ambient().b()*matAmB + matEmB;

	return maxR > 1 || maxG > 1 || maxB > 1;
}

// Note that overflow and specular do not necessarilly occur at the same time.
// There are two different cases:
//
//    1) Strong diffuse and strong emissive combined can cause overflow
//       without specular oversaturation, e.g.
//       diffuseR=1, emissiveR=1, ambientR=0, lambertR=0.9, output=1.9, specR=0
//    2) The lambertian output can exceed 1, but combined with the diffuse
//       reflection, etc. it may not produce an overflow, e.g.
//       diffuseR=0.5, emissiveR=0, ambientR=0, lambertR=1.2, output=0.6, specR=0.2
//
// These evaluations are, of course, intimately tied to the behaviour of
// RenIMaterialApplicator and it's subclasses.  Perhaps they don't belong here?
bool RenINonMMXIlluminator::specularRequired() const
{
	const float maxLambR = lightingBuffer_->maxLambertianR();
	const float maxLambG = lightingBuffer_->maxLambertianG();
	const float maxLambB = lightingBuffer_->maxLambertianB();
	return maxLambR > 1 || maxLambG > 1 || maxLambB > 1;
}

// Prevent an "unused argument" warning.
#ifdef NDEBUG
#define SECONDARG
#else
#define SECONDARG in
#endif

// virtual
const RenIVertex* RenINonMMXIlluminator::applyMaterial
(
	const RenMaterial& m,
	const RenIVertexData& SECONDARG,
	const ctl_min_memory_vector<Ren::VertexIdx>& indices
)
{
	PRE_INFO(in.size());
	PRE(litData_ == &in);
	PRE(litData_->size() == in.size());
	PRE(lightingBuffer_->sizeMatches(in.size()));

	// If there's a global material xform in effect, apply it.
	// This needs to be super-efficient when no transform exists.  Hence,
	// the use of a material pointer rather than copying materials by value.
	static RenMaterial transformedMaterial(RenMaterial::NON_SHARABLE);
	const RenMaterial* pMat = &m;

	if (globalMaterialTransform())
	{
		globalMaterialTransform()->transform(m, &transformedMaterial);
		pMat = &transformedMaterial;
	}

	// Reset all the flags (quickly).
	lightingBuffer_->setAllAppliedFlags(0);

	// TBD: for vertex fog to work correctly the fog value must be calculated
	// at each vertex and put into the specular part of the colour.
	// The real version: const bool isFogOn = device()->isFogOn();
	// const RenColour& fogColour = device()->fogColour();

	ASSERT(pMat, "Material pointer not set.");
	const RenIMatBody* body = matBody(*pMat);
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
		// First, check to see if an overflow is possible given the current lighting
		// and material.  If per-vertex materials are in use, this can throw the
		// overflowPossible function because the per-vertex materials can be brighter
		// than the material referred to by body.  If so, use a white body for the
		// calculation.  (TBD: this still doens't account for emissive bodies; is
		// this good enough?)
		static RenMaterial white(RenColour::white());

		if (overflowPossible((mapUnity)? body: matBody(white)))
		{
			// Overflow will occur for some vertices.
			++overflows_;

			// RENDER_STREAM("Applying (overflow) " << m << "\n");
			// RENDER_INDENT(2);
			RenICheckedMatApp app(body, ambient(), expansion(), mapUnity, filter());
			app.perVertexMats(perVertexMats());
			app.specularRequired(specularRequired());
			app.applyViaIndices(indices, lightingBuffer_);
			// RENDER_INDENT(-2);
		}
		else
		{
			// RENDER_STREAM("Applying (no overflow) " << m << "\n");
			// RENDER_INDENT(2);
			RenINoOverflowMatApp app(body, ambient(), expansion(), mapUnity, filter());
			app.perVertexMats(perVertexMats());
			app.specularRequired(specularRequired());
			app.applyViaIndices(indices, lightingBuffer_);
			// RENDER_INDENT(-2);
		}
	}

	++applications_;

	// *Very* expensive.  Use only for special test cases.
	// RENDER_STREAM(*lightingBuffer_);
	return lightingBuffer_->vertices();
}

// This function is called if a mesh with per-vertex materials is lit.
// virtual
void RenINonMMXIlluminator::applyVertexMaterials
(
	const RenIVertexData& in
)
{
	PRE_INFO(in.size());
	PRE(in.materialMap());
	PRE(litData_ == &in);
	PRE(litData_->size() == in.size());
	PRE(lightingBuffer_->sizeMatches(in.size()));

	static RenMaterial transformedMaterial(RenMaterial::NON_SHARABLE);

	RenIVertex*  vertices   = lightingBuffer_->vertices();
	const float* pR		    = lightingBuffer_->rLamberts();
	const float* pG		    = lightingBuffer_->gLamberts();
	const float* pB		    = lightingBuffer_->bLamberts();
	bool*		 pPerVertex = lightingBuffer_->perVertexMatFlags();
	const bool	 mapUnity   = expanded() == NULL;

	const RenIVertexMaterials* mats = in.materialMap();
	RenIVertexMaterials::const_iterator it = mats->begin();
	while (it != mats->end())
	{
		const RenMaterial&   mat = (*it).material();
		const Ren::VertexIdx idx = (*it).index();

		// If there's a global material xform in effect, apply it.
		// This needs to be super-efficient when no transform exists.  Hence,
		// the use of a material pointer rather than copying materials by value.
		// TBD: doing this inside the loop is not good.
		const RenMaterial* pMat = &mat;
		if (globalMaterialTransform())
		{
			globalMaterialTransform()->transform(mat, &transformedMaterial);
			pMat = &transformedMaterial;
		}

		const RenIMatBody* body = matBody(*pMat);
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
			// RENDER_STREAM("Applying (per-vertex) " << mat << "\n");
			// RENDER_INDENT(2);
			// RENDER_STREAM("to " << vertices[idx] << "\n");
			RenICheckedMatApp app(body, ambient(), expansion(), mapUnity, filter());
			app.perVertexMats(perVertexMats());
			app.specularRequired(specularRequired());
			app.applyToVertex(vertices[idx], pR[idx], pG[idx], pB[idx]);
			// RENDER_INDENT(-2);
		}

		// Set the flag to indicate that this vertex has its own material.
		pPerVertex[idx] = true;

		++it;
	}

	// *Very* expensive.  Use only for special test cases.
	// RENDER_STREAM("Computed per-vertex materials.\n");
	// RENDER_STREAM(*lightingBuffer_);
}

/* End NONMMX.CPP ***************************************************/

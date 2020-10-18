/***********************************************************

  S T A R S I . C P P
  (c) Charybdis Limited, 1998. All Rights Reserved

***********************************************************/

//#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "render/render.hpp"
#include "render/colour.hpp"
#include "render/camera.hpp"
#include "render/stats.hpp"
#include "render/material.hpp"
#include "render/internal/starsi.hpp"
#include "render/internal/fn_stars.hpp"
#include "render/internal/devicei.hpp"
#include "render/device.hpp"

#include "ctl/algorith.hpp"

#include <algorithm>

#include "mathex/angle.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/transf3d.hpp"

#include "utility/percent.hpp"

#ifndef _INLINE
	#include "render/internal/starsi.ipp"
#endif

static const int N_SECTORS =16;
static const MATHEX_SCALAR SECTOR_WIDTH = 2 * Mathex::PI / N_SECTORS;
static const RenColour STAR_COLOUR = RenColour::white();
// Because of the non-cartesian culling a slight fudge factor is needed for the illusion.
static const MexRadians FUDGE_FACTOR = Mathex::PI / 24;
static glm::mat4 starsXform(
	    (1), 0.0,         0.0,         0.0,
		0.0,         (1), 0.0,         0.0,
		0.0,         0.0,         (1), 0.0,
		0.0,         0.0,         0.0,         (1));

RenIStarsImpl::RenIStarsImpl(
	RenStars::Configuration config,
	MATHEX_SCALAR radius,
	uint nStars)

	:configuration_(config),
	nStars_(nStars),
	radius_(radius),
	colourFilter_(RenColour::white()),
	sectors_(N_SECTORS)
{
	PRE(radius_ >= 1.0);
	PRE(nStars_ > 0);
	PRE(sectors_.size() == N_SECTORS);

	// Make sure that the member vectors of sectors are reserved.
	// TODO is this renIStarsImplReserveVectorsOp implemented anywhere?
	//ctl_for_each(sectors_, renIStarsImplReserveVectorsOp);

	RenIStarsImplD3DLVERTEXGen* pVertexGen = 0;
	if(configuration() == RenStars::SPHERICAL)
		pVertexGen = _NEW(RenIStarsImplSphericalD3DLVERTEXGen(radius_, STAR_COLOUR));
	else if(configuration() == RenStars::HEMISPHERICAL)
		pVertexGen = _NEW(RenIStarsImplHemisphericalD3DLVERTEXGen(radius_, STAR_COLOUR));
	else
		ASSERT_FAIL("An invalid stars configuration was found in RenIStarsImpl ctor.");

	ASSERT(pVertexGen, "The allocation of the vertex generator failed.");

	RenIStarsImplD3DLVERTEXInserterOp vertexInserter(&sectors_, N_SECTORS, SECTOR_WIDTH);

	// Create a set of random vertices in a particular configuration.
	for(int index=0; index < nStars; ++index)
		vertexInserter((*pVertexGen)());

	// Sort the vertices by height. The lowest at sectors_[0].
	ctl_for_each(sectors_, RenIStarsImplSortSectorOp());

	// Give the vertices some random alpha values.
	ctl_for_each(sectors_, RenIStarsImplD3DLVERTEXRandomiseAlphasOp(0.3, 1.0));

	TEST_INVARIANT;
}

void RenIStarsImpl::render(
	UtlPercentage,
    const MexTransform3d& cameraXform,
	MexRadians verticalFOV,
    MexRadians horizontalFOV)
{
	PRE(horizontalFOV.asScalar() <= Mathex::PI_DIV_2);
	RenIDeviceImpl* devImpl = RenIDeviceImpl::currentPimpl();

	ASSERT(devImpl, "There is no current device.");
	ASSERT(devImpl->currentCamera(), "There is no current camera.");

	RenColour currentColourFilter = devImpl->currentCamera()->colourFilter();

	// The stars colour is the same as the RGB of the colour filter of the current camera.
	// Update this in a lazy fashion.
	if(colourFilter_ != currentColourFilter)
	{
		ctl_for_each(sectors_, RenIStarsImplVerticesColourOp(currentColourFilter));
		colourFilter_ = currentColourFilter;
	}

	// Pointers to D3DLVERTEX arrays for Direct3D to draw with.
	ctl_vector<RenIVertex*> vertexPtrs;
	vertexPtrs.reserve(5);
	// The sizes of the arrays pointed to by the above.
	ctl_vector<int> sizes;
	sizes.reserve(5);

	RenderRegion renRegion = renderRegion(
		cameraXform.rotationAsEulerAngles().elevation(),
		verticalFOV);

	switch(renRegion)
	{
	case NORTH_CAP:
		renderNorthCap(verticalFOV, &vertexPtrs, &sizes);
		break;

	case SOUTH_CAP:
		renderSouthCap(verticalFOV, &vertexPtrs, &sizes);
		break;

	case TROPICS:
		renderTropics(cameraXform, verticalFOV, horizontalFOV, &vertexPtrs, &sizes);
		break;

	case NONE:
		break;

	default:
		ASSERT_FAIL("An invalid render region was received in RenIStarsImpl");
		break;
	}

	// The number of vertices rendered.
	int nPoints = std::accumulate(sizes.begin(), sizes.end(), 0);
	// Alter the render statistics accordingly.
	RenStats* pStats = devImpl->statistics();
	if(pStats)
		pStats->incrPointsCount(nPoints);

	// Texture needs to be turned off.  Use a default untextured material.
	static RenMaterial defaultMat;
	devImpl->setMaterialHandles(defaultMat);

    RenDevice::current()->setModelMatrix(starsXform);

	// for_each(sectors_.begin(), sectors_.end(), RenIPrintSectorOp(Diag::instance().renderStream()));

    glEnable(GL_BLEND);

	// Render the vertices.
	ctl_for_each(vertexPtrs, RenIStarsImplRenderVerticesOp( sizes.begin()));

	// Reset the previous states.
	glDisable(GL_BLEND);

	TEST_INVARIANT;
}

// Private.
MATHEX_SCALAR RenIStarsImpl::absCapHeight(MexRadians verticalFOV) const
{
	return radius() * sin(Mathex::PI_DIV_2 - verticalFOV.asScalar());
}

// Private.
RenIStarsImpl::RenderRegion RenIStarsImpl::renderRegion(
	MexRadians elevation,
	MexRadians verticalFOV) const
{
	RenderRegion result = TROPICS;

	MATHEX_SCALAR lower = 0.0;
	MATHEX_SCALAR upper = 0.0;

	switch(configuration())
	{
	case RenStars::SPHERICAL:
		lower = verticalFOV.asScalar() / 2 - Mathex::PI_DIV_2;
		upper = Mathex::PI_DIV_2 - verticalFOV.asScalar() / 2;

		if(elevation.asScalar() < lower)
			result = NORTH_CAP;
		else if(elevation.asScalar() > upper)
			result = SOUTH_CAP;
		break;

	case RenStars::HEMISPHERICAL:
		lower = verticalFOV.asScalar() / 2 - Mathex::PI_DIV_2;
		upper = verticalFOV.asScalar() / 2;

		if(elevation.asScalar() < lower)
			result = NORTH_CAP;
		else if(elevation.asScalar() > upper)
			result = NONE;
		break;

	default:
		ASSERT_FAIL("An Invalid star field configuration was found in RenIStarsImpl.");
		break;
	}

	return result;
}

// Private.
void RenIStarsImpl::renderNorthCap(
	MexRadians verticalFOV,
	ctl_vector<RenIVertex*>* const pVertexPtrs,
	ctl_vector<int>* const pSizes)
{
	PRE(pVertexPtrs->empty());
	PRE(pSizes->empty());

	MATHEX_SCALAR rimHeight = absCapHeight(verticalFOV);

	ctl_for_each(sectors_, RenIStarsImplDefineNorthCapOp(rimHeight, pVertexPtrs, pSizes));
}

// Private.
void RenIStarsImpl::renderSouthCap(
	MexRadians verticalFOV,
	ctl_vector<RenIVertex*>* const pVertexPtrs,
	ctl_vector<int>* const pSizes)
{
	PRE(configuration() == RenStars::SPHERICAL);
	PRE(pVertexPtrs->empty());
	PRE(pSizes->empty());

	MATHEX_SCALAR rimHeight = -absCapHeight(verticalFOV);

	ctl_for_each(sectors_, RenIStarsImplDefineSouthCapOp(rimHeight, pVertexPtrs, pSizes));
}

// Private.
void RenIStarsImpl::renderTropics(
	const MexTransform3d& cameraXform,
	MexRadians verticalFOV,
	MexRadians horizontalFOV,
	ctl_vector<RenIVertex*>* const pVertexPtrs,
	ctl_vector<int>* const pSizes)
{
	PRE(pVertexPtrs->empty());
	PRE(pSizes->empty());

	// Perform a little test to see whether the caps need rendering.
	MexRadians absElevation = Mathex::abs(cameraXform.rotationAsEulerAngles().elevation().asScalar());

	// This tests that the renderRegion() code is doing it's thing.
	ASSERT(absElevation.asScalar() <= Mathex::PI_DIV_2 - verticalFOV.asScalar() / 2, "");
	ASSERT(horizontalFOV < MexDegrees(180), "The horizontal FOV is too large.");

	// This code linearly interpolates the elevation to the half angular width.
	// This is used to calculate how many sectors you can see at any time.
	// The outputs are between horizontalFOV/2 and 180 degrees. The translation
	// isn't strictly linear more like exponential but this does the trick.
	MATHEX_SCALAR halfAngularWidth = mexInterpolate(
		absElevation.asScalar(),
		0.0,
		Mathex::PI_DIV_2 - verticalFOV.asScalar() / 2,
		horizontalFOV.asScalar() / 2,
		Mathex::PI_DIV_2);

	ctl_vector<ctl_vector<RenIVertex>*> sectorPtrs;
	sectorPtrs.reserve(N_SECTORS / 4);

	cullSectors(cameraXform, halfAngularWidth, &sectorPtrs);

	MexRadians negatedElevation(-cameraXform.rotationAsEulerAngles().elevation().asScalar());
	float minHeight = radius() * sin(negatedElevation - verticalFOV/2);
    float maxHeight = radius() * sin(negatedElevation + verticalFOV/2);

	RenIStarsImplPegStarsOp pegStars(pVertexPtrs, pSizes, minHeight, maxHeight);

	ctl_for_each(sectorPtrs, pegStars);

	POST(pVertexPtrs->size() == pSizes->size());
}

// Private.
// The best place for optimisations I think.
void RenIStarsImpl::cullSectors(
	const MexTransform3d& cameraXform,
	MATHEX_SCALAR halfAngularWidth,
    ctl_vector<ctl_vector<RenIVertex>*>* const pSectorPtrs)
{
	PRE(halfAngularWidth > SECTOR_WIDTH/2);
	PRE(pSectorPtrs);
	PRE(pSectorPtrs->empty());

	MATHEX_SCALAR anti = cameraXform.rotationAsEulerAngles().azimuth().asScalar() - halfAngularWidth;
	if(anti < -Mathex::PI)
	    anti += 2*Mathex::PI;

	MATHEX_SCALAR clock = cameraXform.rotationAsEulerAngles().azimuth().asScalar() + halfAngularWidth;
	if(clock > Mathex::PI)
		clock -= 2*Mathex::PI;

	RenIStarsImplSectorClipBinPred sectorClip(anti, clock);

	MATHEX_SCALAR antiWall = -Mathex::PI;
	MATHEX_SCALAR clockWall = SECTOR_WIDTH -Mathex::PI;

	for(
		int sectorIndex = 0;
		sectorIndex < N_SECTORS;
		++sectorIndex, antiWall += SECTOR_WIDTH, clockWall += SECTOR_WIDTH)
	{
		if(sectorClip(antiWall, clockWall))
			pSectorPtrs->push_back(&(sectors_[sectorIndex]));
	}
}

void RenIStarsImpl::CLASS_INVARIANT
{
	INVARIANT(this);
	INVARIANT(radius_ >= 1.0);
	INVARIANT(sectors_.size() == N_SECTORS);
}

/* End CULLEDPT.CPP ***************************************/


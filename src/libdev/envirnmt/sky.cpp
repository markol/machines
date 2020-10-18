/*
 * S K Y . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "mathex/double.hpp"

#include "envirnmt/sky.hpp"
#include "envirnmt/satelite.hpp"
#include "envirnmt/elevclut.hpp"

#include "render/render.hpp"
#include "render/mesh.hpp"
#include "render/vertex.hpp"

#include "world4d/custom.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/stars.hpp"

#include "system/pathname.hpp"

#ifndef _INLINE
	#include "envirnmt/sky.ipp"
#endif

////////////////////////////////////////////////////////////

// VIRTUAL //
EnvSky::~EnvSky()
{
    TEST_INVARIANT;

	if(pStars_)
		_DELETE(pStars_);
}

void EnvSky::CLASS_INVARIANT
{
    INVARIANT(this);
}

// PROTECTED //
EnvSky::EnvSky():
	pStars_(0),
	doColourOverride_(false),
	colourChanged_(false)
{
    TEST_INVARIANT;
}

void EnvSky::overrideColour(const RenColour& c)
{
	doColourOverride_ = true;
	colourChanged_    = true;
	overrideColour_   = c;

	POST(colourChanged());
	POST(doColourOverride());
}

void EnvSky::resetColour()
{
	doColourOverride_ = false;
	colourChanged_    = true;

	POST(colourChanged());
	POST(!doColourOverride());
}

void EnvSky::uniformlyColourMesh(const RenColour& col, RenMesh* mesh)
{
	PRE(mesh);

	RenMaterial vertexMaterial;
	vertexMaterial.makeNonSharable();
	vertexMaterial.diffuse(RenColour::black());
	vertexMaterial.emissive(col);

	const int nVertices = mesh->nVertices();
	for (Ren::VertexIdx i=0; i!=nVertices; ++i)
	{
		RenVertex vertex = mesh->vertex(i);
		vertex.material(vertexMaterial);
	}
}

ostream& operator <<(ostream& o, const EnvSky& t)
{
	t.write(o);
    return o;
}

////////////////////////////////////////////////////////////

// VIRTUAL //
EnvControlledSky::~EnvControlledSky( void )
{
	TEST_INVARIANT;
}

void EnvControlledSky::CLASS_INVARIANT
{
	INVARIANT(this);
}

// PROTECTED //
EnvControlledSky::EnvControlledSky( void )

	:pController_(0)
{
}

////////////////////////////////////////////////////////////

EnvUniformSky::EnvUniformSky( void )

    :pColourTable_(0),
    visible_(true)
{
	TEST_INVARIANT;
}

// VIRTUAL //
EnvUniformSky::~EnvUniformSky()
{
	TEST_INVARIANT;

	if(pColourTable_)
		_DELETE(pColourTable_);
}

// VIRTUAL //
void EnvUniformSky::update(W4dSceneManager* pSceneManager)
{
	PRE(controller());
	PRE(pColourTable_);

	pSceneManager->clearBackground(true);

	if(visible_)
	{
		// Don't use the colourChanged flag.  This means we keep doing this
		// every frame.  No harm should be done.
		if (doColourOverride())
			pSceneManager->backgroundColour(overrideColour());
		else
			pSceneManager->backgroundColour(pColourTable_->colour(controller()->elevation()));
	}
}

// VIRTUAL //
void EnvUniformSky::write(ostream& o) const
{
	PRE(pColourTable_);

	if(pStars())
	{
		o << "Uniform sky with stars and colours:" << std::endl;
		o << *pColourTable_ << std::endl;
	}
	else
	{
		o << "Uniform sky without stars and colours:" << std::endl;
		o << *pColourTable_ << std::endl;
	}
}

void EnvUniformSky::CLASS_INVARIANT
{
	INVARIANT(this);
}

// EnvDynamicSky //

EnvDynamicSky::EnvDynamicSky(W4dEntity* pParent)
    :pDome_(_NEW(W4dCustom(pParent, MexTransform3d(), W4dEntity::NOT_SOLID, MexDouble::plusInfinity()))),
	index_(0),
	maxElevation_(0),
	complexity_(UNTESTED)
{
	POST(pDome_);

	// Override the colour then reset it.  This will force a complete
	// colour update on the first frame.
	overrideColour(RenColour::white());
	resetColour();
	ASSERT(!doColourOverride(), "Recolouring logic error.");
	ASSERT(colourChanged(),     "Recolouring logic error.");

    TEST_INVARIANT;
}

// VIRTUAL //
EnvDynamicSky::~EnvDynamicSky()
{
    TEST_INVARIANT;

    _DELETE(pDome_);
}

// HELPER //
static MexRadians vertexElevation(const RenVertex& vtx)
{
	const MexPoint3d pos = vtx.position();
	const MATHEX_SCALAR adjacent = sqrt(pos.x() * pos.x() + pos.y() * pos.y());
	return atan2(pos.z(), adjacent);
}

void EnvDynamicSky::colourSomeVertices(RenMesh* mesh, int startIndex, int nVertices)
{
	PRE(mesh);
	PRE(startIndex >= 0);
	PRE(startIndex < mesh->nVertices());
	PRE(nVertices > 0);
	PRE(nVertices <= mesh->nVertices());

	const int endIndex = startIndex + nVertices;
	ASSERT(endIndex <= mesh->nVertices(), "Sky index calculation error.");

	for (int i=startIndex; i!=endIndex; ++i)
	{
		Ren::VertexIdx idx = _STATIC_CAST(Ren::VertexIdx, i);
		RenVertex vertex = mesh->vertex(idx);
		const MexDegrees elevation = vertexElevation(vertex);
		const RenColour& vertexColour = findVertexColour(elevation);

		// As these materials are created multiple times each frame, the
		// overhead of sharing will be too much (and the material manager is
		// unlikely to find matches because the sky colours will probably be
		// unique.)
		RenMaterial vertexMaterial;
		vertexMaterial.makeNonSharable();
		vertexMaterial.diffuse(RenColour::black());
		vertexMaterial.emissive(vertexColour);
		vertex.material(vertexMaterial);

		// This is < due to the max elevations being negative.
		if(elevation > maxElevation_)
		{
			maxElevation_ = elevation;
			topColour_ = vertexColour;
		}
	}
}

// VIRTUAL //
void EnvDynamicSky::update(W4dSceneManager* pSceneManager)
{
	PRE(controller());
	PRE(cluts_.size() > 1);
	PRE(pSceneManager);
	TEST_INVARIANT;

	ASSERT(pDome_->hasMesh(0), "Sky has no mesh.");
	RenMesh& mesh = pDome_->mesh(0);
	const int nVertices = mesh.nVertices();

	// This is a supposed 'if this is the first update call' conditional.
	if(complexity_ == UNTESTED)
	{
		complexity_ = testComplexity();

		// Update the entire sky on the first frame, regardless of type.
		colourSomeVertices(&mesh, 0, nVertices);
	}

	if(visible() and complexity_ == COMPLEX)
	{
		// An optimisation: assume that some domes are built without a top, i.e.
		// they are just a skirt around the horizon.  Set the background colour
		// to match the colour of the topmost elevation.  If the top-most
		// vertices of the skirt are all level, you won't be able to see the hole.
		pSceneManager->clearBackground(true);

		if (doColourOverride())
		{
			if (colourChanged())
			{
				// Reset the flag so we don't keep recolouring the mesh every frame.
				colourChangeUpdated();
				uniformlyColourMesh(overrideColour(), &mesh);
			}

			pSceneManager->backgroundColour(overrideColour());
		}
		else
		{
			// If the colour has changed, i.e., the override has been reset
			// then we need to do the dynamic calculation for the entire sky.
			if (colourChanged())
			{
				// Reset the flag so we don't keep recolouring the mesh every frame.
				colourChangeUpdated();
				colourSomeVertices(&mesh, 0, nVertices);

				pSceneManager->backgroundColour(topColour_);
				index_ = 0;
				maxElevation_ = 0;
			}
			else
			{
				// Normally do just one vertex per frame.
				colourSomeVertices(&mesh, index_, 1);
				++index_;

				if (index_ == nVertices)
				{
					pSceneManager->backgroundColour(topColour_);
					index_ = 0;
					maxElevation_ = 0;
				}
			}
		}
	}
}

EnvDynamicSky::ClutComplexity EnvDynamicSky::testComplexity() const
{
	PRE(complexity_ == UNTESTED);

	ClutComplexity clutComplexity = SIMPLE;

	ClutMap::const_iterator start  = cluts_.begin();
	ClutMap::const_iterator finish = cluts_.end();

	while(start != finish)
	{
		if((*start).second->nEntries() > 1)
		{
			clutComplexity = COMPLEX;
			break;
		}

		++start;
	}

	return clutComplexity;
}

// VIRTUAL //
void EnvDynamicSky::write(ostream& o) const
{
	TEST_INVARIANT;

	if(pStars())
		o << "Graduated sky with stars." << std::endl;
	else
		o << "Graduated sky without stars." << std::endl;
}

// virtual
bool EnvDynamicSky::visible() const
{
	TEST_INVARIANT;

	return pDome_->visible();
}

// virtual
void EnvDynamicSky::visible(bool on)
{
	TEST_INVARIANT;

	pDome_->visible(on);
}

void EnvDynamicSky::colourTable(MexDegrees elevation, EnvElevationColourTable* pElevationClut)
{
	PRE(pElevationClut);

	ClutMap::iterator it = cluts_.find(elevation);

	// There may already be an entry for the given elevation, in which case
	// we just replace what's there already.
	if (it != cluts_.end())
	{
		EnvElevationColourTable* existingClut = (*it).second;

		if (existingClut != pElevationClut)
		{
			_DELETE(existingClut);
			(*it).second = pElevationClut;
		}
	}
	else
	{
		// There isn't an existing entry for the given elevation, so insert a
		// new one.
		std::pair<const MexDegrees, EnvElevationColourTable*> newEntry(elevation, pElevationClut);
		cluts_.insert(newEntry);
	}

	TEST_INVARIANT;
}

void EnvDynamicSky::mesh(const SysPathName& meshFile)
{
	PRE(meshFile.existsAsFile());

	TEST_INVARIANT;

	pDome_->loadUniqueMesh(meshFile, "sky", MexDouble::plusInfinity());
	ASSERT(pDome_->hasMesh(0), "Sky has no mesh.");
}

// PRIVATE //
const RenColour& EnvDynamicSky::findVertexColour(MexDegrees vtxEl) const
{
	TEST_INVARIANT;

	const MexDegrees sunEl = controller()->elevation();
	ClutMap::const_iterator lower = cluts_.lower_bound(vtxEl);

	// This is just a sanity check on the behaviour of the map.
	ASSERT(implies(lower != cluts_.end(), (*lower).first >= vtxEl), "Error in use of map.");

	ClutMap::const_iterator lastEntry = cluts_.end();
	--lastEntry;

	// Beware: we return a reference to this static.  This will go horribly
	// wrong with re-entrant code or if this method is called twice in the
	// same expression, etc.
	static RenColour result;

	// Boundary conditions: lower could be past the end of the map, or it
	// could be an exact match for the last entry.
	if (lower == cluts_.end() || lower == lastEntry && vtxEl == (*lastEntry).first)
	{
		result = (*lastEntry).second->colour(sunEl);
	}
	else
	{
		// Boundary condition: if lower is the first entry, just use that.
		if (lower == cluts_.begin())
			result = (*cluts_.begin()).second->colour(sunEl);
		else
		{
			ClutMap::const_iterator previous = lower;
			--previous;

			const RenColour& c1 = (*previous).second->colour(sunEl);
			const RenColour& c2 = (*lower)   .second->colour(sunEl);
			const MATHEX_SCALAR el1 = (*previous).first.asScalar();
			const MATHEX_SCALAR el2 = (*lower   ).first.asScalar();

			result.linearInterpolate(vtxEl.asScalar(), c1, el1, c2, el2);
		}
	}

	return result;
}

void EnvDynamicSky::CLASS_INVARIANT
{
	INVARIANT(this);
	INVARIANT(pDome_);
}

////////////////////////////////////////////////////////////

EnvStaticSky::EnvStaticSky(W4dEntity* pParent)

	:pSkyMesh_(_NEW(W4dCustom(pParent, MexTransform3d(), W4dEntity::NOT_SOLID, MexDouble::plusInfinity()))),
	backgroundColour_(RenColour::blue())
{
	TEST_INVARIANT;
}

// VIRTUAL //
EnvStaticSky::~EnvStaticSky( void )
{
	TEST_INVARIANT;

	_DELETE(pSkyMesh_);
}

// VIRTUAL //
void EnvStaticSky::update(W4dSceneManager* pSceneManager)
{
	PRE(pSceneManager);

	if(visible())
	{
		ASSERT(pSkyMesh_->hasMesh(0), "Sky has no mesh.");
 		RenMesh& mesh = pSkyMesh_->mesh(0);
		pSceneManager->clearBackground(true);

		// If the override colour has changed, then one way or another we need
		// to recolour the entire sky.
		if (colourChanged())
		{
			if (doColourOverride())
				uniformlyColourMesh(overrideColour(), &mesh);
			else
				mesh.resetAllVertexMaterials();
		}

		if (doColourOverride())
			pSceneManager->backgroundColour(overrideColour());
		else
			pSceneManager->backgroundColour(backgroundColour());
	}
}

// VIRTUAL //
void EnvStaticSky::write(ostream& o) const
{
	o << "EnvStaticSky.backgroundColour() == " << backgroundColour() << std::endl;
}

// VIRTUAL //
bool EnvStaticSky::visible( void ) const
{
	TEST_INVARIANT;

	return pSkyMesh_->visible();
}

// VIRTUAL //
void EnvStaticSky::visible(bool yesNo)
{
	TEST_INVARIANT;

	pSkyMesh_->visible(yesNo);
}

void EnvStaticSky::mesh(const SysPathName& meshFile)
{
	PRE(meshFile.existsAsFile());

	TEST_INVARIANT;

	pSkyMesh_->loadUniqueMesh(meshFile, "sky", MexDouble::plusInfinity());
	ASSERT(pSkyMesh_->hasMesh(0), "Sky has no mesh.");
}

void EnvStaticSky::CLASS_INVARIANT
{
	INVARIANT(this);
	INVARIANT(pSkyMesh_);
}

/* End SKY.CPP **************************************************/

/***********************************************************

  F N _ S T A R S . H P P
  (c) Charybdis Limited, 1998. All Rights Reserved

***********************************************************/

/*
  There are quite a lot of functions and functors in here to help keep the stars fast.
*/

#ifndef _INTERNAL_RENDER_STARS_IMPL_FUNCTORS_HPP
#define _INTERNAL_RENDER_STARS_IMPL_FUNCTORS_HPP


#include "mathex/mathex.hpp"
#include "mathex/random.hpp"
#include "mathex/vec3.hpp"
#include "mathex/epsilon.hpp"

#include "render/render.hpp"
#include "render/colour.hpp"
#include "render/internal/colpack.hpp"
#include "render/device.hpp"

#include "ctl/algorith.hpp"

#include <numeric>
#include <iterator>
#include <functional>
#include <algorithm>

#include "utility/percent.hpp"

////////////////////////////////////////////////////////////
class RenIVertex;

void renIStarsImplReserveVectorsOp(ctl_vector<RenIVertex>& sector);

class RenIStarsImplD3DLVERTEXGen;
class RenIStarsImplSphericalD3DLVERTEXGen;
class RenIStarsImplHemisphericalD3DLVERTEXGen;

class RenIStarsImplD3DLVERTEXInserterOp;

class RenIStarsImplD3DLVERTEXHeightGreaterOrEqualBinPred;
class RenIStarsImplD3DLVERTEXHeightLesserOrEqualBinPred;

class RenIStarsImplD3DLVERTEXRandomiseAlphasAuxOp;
class RenIStarsImplD3DLVERTEXRandomiseAlphasOp;

struct RenIStarsImplSortSectorOp;

class RenIStarsImplVerticesColourOp;
class RenIStarsImplVerticesColourAuxOp;

class RenIStarsImplDefineNorthCapOp;
class RenIStarsImplDefineSouthCapOp;

class RenIStarsImplSectorClipBinPred;

class RenIStarsImplPegStarsOp;

class RenIStarsImplRenderVerticesOp;

////////////////////////////////////////////////////////////

void RenIStarsImplReserveVectorsOp(ctl_vector<RenIVertex>& sector)
{
	// This value is for an average hemisphere of 2,000 stars with 16 sectors.
	sector.reserve(125);
}

////////////////////////////////////////////////////////////

class RenIStarsImplD3DLVERTEXGen : public std::unary_function<void, RenIVertex>
{
public:
	virtual RenIVertex operator ()() = 0;
};

class RenIStarsImplSphericalD3DLVERTEXGen : public RenIStarsImplD3DLVERTEXGen
{
public:
	RenIStarsImplSphericalD3DLVERTEXGen(MATHEX_SCALAR radius, RenColour rgbColour)

		:radius_(radius),
		rgbColour_(rgbColour),
		random_(MexBasicRandom::constructSeededFromTime())
	{
	}

	RenIVertex operator ()()
	{
		MexVec3 vec(
		    mexRandomScalar(&random_, -1.0, 1.0),
		    mexRandomScalar(&random_, -1.0, 1.0),
		    mexRandomScalar(&random_, -1.0, 1.0));

		vec.makeUnitVector();
		vec *= radius_;

        RenIVertex d3dLVertex;
        d3dLVertex.x = vec.x(); d3dLVertex.y = vec.y(); d3dLVertex.z = vec.z();
        d3dLVertex.color = packColour( rgbColour_.r(), rgbColour_.b(), rgbColour_.b(), 1.0);
        d3dLVertex.specular = 0;

		return d3dLVertex;
	}

private:
    MATHEX_SCALAR radius_;
	RenColour rgbColour_;
	MexBasicRandom random_;
};

class RenIStarsImplHemisphericalD3DLVERTEXGen : public RenIStarsImplD3DLVERTEXGen
{
public:
	RenIStarsImplHemisphericalD3DLVERTEXGen(MATHEX_SCALAR radius, RenColour rgbColour)

		:radius_(radius),
		rgbColour_(rgbColour),
		random_(MexBasicRandom::constructSeededFromTime())
	{
	}

	RenIVertex operator ()()
	{
		MexVec3 vec(
		    mexRandomScalar(&random_, -1.0, 1.0),
		    mexRandomScalar(&random_, -1.0, 1.0),
		    mexRandomScalar(&random_, 0.0, 1.0));

		vec.makeUnitVector();
		vec *= radius_;

        RenIVertex d3dLVertex;
        d3dLVertex.x = vec.x(); d3dLVertex.y = vec.y(); d3dLVertex.z = vec.z();
        d3dLVertex.color = packColour( rgbColour_.r(), rgbColour_.b(), rgbColour_.b(), 1.0);
        d3dLVertex.specular = 0;
		return d3dLVertex;
	}

private:
    MATHEX_SCALAR radius_;
	RenColour rgbColour_;
	MexBasicRandom random_;
};

////////////////////////////////////////////////////////////

class RenIStarsImplD3DLVERTEXInserterOp : public std::unary_function<const RenIVertex&, void>
{
public:
	RenIStarsImplD3DLVERTEXInserterOp(
	    ctl_vector< ctl_vector<RenIVertex> >* const pSectors,
	    int nSectors,
	    MATHEX_SCALAR sectorWidth)

		:pSectors_(pSectors),
		nSectors_(nSectors),
		sectorWidth_(sectorWidth)
	{
	}

	void operator ()(const RenIVertex& vertex) const
	{
		// The azimuthal angle of the vector.
		MATHEX_SCALAR azi = atan2(vertex.y, vertex.x);

		// Place the vertex in the currect sector.
		MATHEX_SCALAR sectorPartition = sectorWidth_ - Mathex::PI;

		for(int sectorIndex =0; sectorIndex < nSectors_; ++sectorIndex)
		{
			if(azi < sectorPartition)
			{
				(*pSectors_)[sectorIndex].push_back(vertex);
				break;
			}

		    sectorPartition += sectorWidth_;
		}
	}

private:
	ctl_vector< ctl_vector<RenIVertex> >* const pSectors_;
	int nSectors_;
	MATHEX_SCALAR sectorWidth_;
};

////////////////////////////////////////////////////////////
/* */
struct RenIStarsImplD3DLVERTEXHeightLesserOrEqualBinPred : public std::binary_function<
	const RenIVertex,	const RenIVertex,
	bool>
{
	bool operator ()(const RenIVertex& lhs, const RenIVertex& rhs) const
	{
		return lhs.z <= rhs.z;
	}
};

struct RenIStarsImplD3DLVERTEXHeightGreaterOrEqualBinPred : public std::binary_function<
	RenIVertex,	RenIVertex,
	bool>
{
	bool operator ()(const RenIVertex& lhs, const RenIVertex& rhs) const
	{
		return lhs.z >= rhs.z;
	}
};

////////////////////////////////////////////////////////////

class RenIStarsImplD3DLVERTEXRandomiseAlphasAuxOp : public std::unary_function<RenIVertex, void>
{
public:
	RenIStarsImplD3DLVERTEXRandomiseAlphasAuxOp(MATHEX_SCALAR lowerBound, MATHEX_SCALAR upperBound)

		:random_(MexBasicRandom::constructSeededFromTime()),
		lowerBound_(lowerBound),
		upperBound_(upperBound)
	{
	}

	uint setD3DCOLORAlpha(uint colour, MATHEX_SCALAR alpha) const
	{
		PRE(alpha >= 0.0 and alpha <= 1.0);

		return (colour & 0x00ffffff) | (_STATIC_CAST(uint, alpha * 255) << 24);
	}

	void operator ()(RenIVertex& vertex)
	{
		MATHEX_SCALAR randAlpha = mexRandomScalar(&random_, lowerBound_, upperBound_);

		vertex.color = setD3DCOLORAlpha(vertex.color, randAlpha);
	}

private:
	MexBasicRandom random_;
	MATHEX_SCALAR lowerBound_;
	MATHEX_SCALAR upperBound_;
};

class RenIStarsImplD3DLVERTEXRandomiseAlphasOp : public std::unary_function<ctl_vector<RenIVertex>&, void>
{
public:
	RenIStarsImplD3DLVERTEXRandomiseAlphasOp(MATHEX_SCALAR lowerBound, MATHEX_SCALAR upperBound)

		:alphaRandomiser_(lowerBound, upperBound)
	{
		PRE_INFO(lowerBound);
		PRE_INFO(upperBound);
		PRE(lowerBound < upperBound);
		PRE(inIntervalOO(lowerBound, 0.0, 1.0));
		PRE(inIntervalOO(upperBound, 0.0, 1.0));
	}

	void operator ()(ctl_vector<RenIVertex>& sector) const
	{
		ctl_for_each(sector, alphaRandomiser_);
	}

private:
	RenIStarsImplD3DLVERTEXRandomiseAlphasAuxOp alphaRandomiser_;
};

////////////////////////////////////////////////////////////

struct RenIStarsImplSortSectorOp : public std::unary_function<ctl_vector<RenIVertex>&, void>
{
	void operator ()(ctl_vector<RenIVertex>& sector)
	{
		// Sort the sector in ascending order.
		sort(sector.begin(), sector.end(), RenIStarsImplD3DLVERTEXHeightLesserOrEqualBinPred());
	}
};

////////////////////////////////////////////////////////////

class RenIStarsImplVerticesColourAuxOp : public std::unary_function<RenIVertex&, void>
{
public:
	RenIStarsImplVerticesColourAuxOp(uint colour)

		:colour_(colour)
	{
	}

	void operator ()(RenIVertex& vertex)
	{
		// Take the current alpha and set the RGB.
		vertex.color = (vertex.color & 0xff000000) | (colour_ & 0x00ffffff);
	}

private:
	uint colour_;
};

class RenIStarsImplVerticesColourOp : public std::unary_function<ctl_vector<RenIVertex>&, void>
{
public:
	RenIStarsImplVerticesColourOp(RenColour colour)

		:colour_(0)
	{
		// NVG uses over-brighting so on this colour so make sure it's in a sensible boundary.
		colour_ = packColour(
			mexClamp(colour.r(), 0.0f, 1.0f),
			mexClamp(colour.g(), 0.0f, 1.0f),
			mexClamp(colour.b(), 0.0f, 1.0f),
			1.0);
	}

	void operator ()(ctl_vector<RenIVertex>& sector)
	{
		ctl_for_each(sector, RenIStarsImplVerticesColourAuxOp(colour_));
	}

private:
	uint colour_;
};

////////////////////////////////////////////////////////////

class RenIStarsImplDefineNorthCapOp : public std::unary_function<const ctl_vector<RenIVertex>&, void>
{
public:
	RenIStarsImplDefineNorthCapOp(
		MATHEX_SCALAR rimHeight,
		ctl_vector<RenIVertex*>* pVertexPtrs,
		ctl_vector<int>* pSizes)

		:rimHeight_(rimHeight),
		pVertexPtrs_(pVertexPtrs),
		pSizes_(pSizes)
	{
		PRE(rimHeight_ > 0.0);
		PRE(pVertexPtrs_);
		PRE(pVertexPtrs_->empty());
		//PRE(pVertexPtrs_->capacity() > 0);
		PRE(pSizes_);
		PRE(pSizes_->empty());
		//PRE(pSizes_->capacity() > 0);
	}

	void operator ()(ctl_vector<RenIVertex>& sector) const
	{
        static RenIVertex vertex;
        vertex.x = vertex.y = 0; vertex.z = rimHeight_;
        vertex.color = 0; vertex.specular = 0;
		ctl_vector<RenIVertex>::iterator itRim = std::find_if(
			sector.begin(),
			sector.end(),
            std::bind2nd( RenIStarsImplD3DLVERTEXHeightGreaterOrEqualBinPred(), vertex)
                                                   );

		ASSERT(itRim != sector.end(), "There are no stars in the north cap.");

		ptrdiff_t size = 0;
		// TODO check std::distance(itRim, sector.end(), size);
		size = std::distance(itRim, sector.end());

		ASSERT(size > 0, "Attempted to render an array of vertices zero elements long");

		(*pVertexPtrs_).push_back(&(*itRim));
		pSizes_->push_back(size);
	}

private:
	MATHEX_SCALAR rimHeight_;
	ctl_vector<RenIVertex*>* const pVertexPtrs_;
	ctl_vector<int>* const pSizes_;
};

class RenIStarsImplDefineSouthCapOp : public std::unary_function<const ctl_vector<RenIVertex>&, void>
{
public:
	RenIStarsImplDefineSouthCapOp(
		MATHEX_SCALAR rimHeight,
		ctl_vector<RenIVertex*>* pVertexPtrs,
		ctl_vector<int>* pSizes)

		:rimHeight_(rimHeight),
		pVertexPtrs_(pVertexPtrs),
		pSizes_(pSizes)
	{
		PRE(rimHeight_ < 0.0);
		PRE(pVertexPtrs_);
		PRE(pVertexPtrs_->empty());
		//PRE(pVertexPtrs_->capacity() > 0);
		PRE(pSizes_);
		PRE(pSizes_->empty());
		//PRE(pSizes_->capacity() > 0);
	}

	// I would like this to be a const parameter as I don't change the sequence at all.
	// I do however put a pointer to an element into another sequence. This means that
	// the type parameter to that sequence would need to be const D3DLVERTEX* and this
	// breaks when a const_iterator is instantiated (if the iterator is a typedefed pointer).
//	void operator ()(ctl_vector<D3DLVERTEX>& sector) const
	void operator ()(ctl_vector<RenIVertex>& sector) const
	{
        static RenIVertex vertex;
        vertex.x = vertex.y = 0; vertex.z = rimHeight_;
        vertex.color = 0; vertex.specular = 0;

		ctl_vector<RenIVertex>::iterator itRim = find_if(
			sector.begin(),
			sector.end(),
			std::bind2nd(RenIStarsImplD3DLVERTEXHeightGreaterOrEqualBinPred(), vertex)
                                                   );

		ASSERT(itRim != sector.end(), "There are no stars in the south cap.");

		ptrdiff_t size = 0;
		//std::distance(sector.begin(), itRim, size);
		size = std::distance(sector.begin(), itRim);

		ASSERT(size > 0, "Attempted to render an array of vertices zero elements long");

		pVertexPtrs_->push_back(&(*sector.begin()));
		pSizes_->push_back(size);
	}

private:
	MATHEX_SCALAR rimHeight_;
	ctl_vector<RenIVertex*>* const pVertexPtrs_;
	ctl_vector<int>* const pSizes_;
};

////////////////////////////////////////////////////////////

class RenIStarsImplSectorClipBinPred : public std::binary_function<MATHEX_SCALAR, MATHEX_SCALAR, bool>
{
public:
	RenIStarsImplSectorClipBinPred(MATHEX_SCALAR antiRay, MATHEX_SCALAR clockRay)

		:antiRay_(antiRay),
		clockRay_(clockRay)
	{
	}

	bool operator ()(MATHEX_SCALAR antiPartition, MATHEX_SCALAR clockPartition) const
	{
		bool result =false;

		if(antiRay_ > clockRay_)
		{
			if(not inIntervalOO(antiPartition, clockRay_, antiRay_)
				or not inIntervalOO(clockPartition, clockRay_, antiRay_))
				result = true;
		}
		else
		{
			if(inIntervalOO(antiPartition, antiRay_, clockRay_)
				or inIntervalOO(clockPartition, antiRay_, clockRay_))
				result = true;
		}

		return result;
	}

private:
	MATHEX_SCALAR antiRay_;
	MATHEX_SCALAR clockRay_;
};

////////////////////////////////////////////////////////////

class RenIStarsImplPegStarsOp : public std::unary_function<const ctl_vector<RenIVertex>* const, void>
{
public:
	RenIStarsImplPegStarsOp(
		ctl_vector<RenIVertex*>* const pVertexPtrs,
		ctl_vector<int>* const pSizes,
		float minHeight,
		float maxHeight)

		:pVertexPtrs_(pVertexPtrs),
		pSizes_(pSizes),
		minHeight_(minHeight),
		maxHeight_(maxHeight)
	{
	}

	void operator ()(ctl_vector<RenIVertex>* const pSector) const
	{

        RenIVertex minVertex;
        minVertex.x = minVertex.y = 0; minVertex.z = minHeight_;
        minVertex.color = 0; minVertex.specular = 0;
        RenIVertex maxVertex;
        maxVertex.x = maxVertex.y = 0; maxVertex.z = maxHeight_;
        maxVertex.color = 0; maxVertex.specular = 0;

		ctl_vector<RenIVertex>::iterator itMin = find_if(
			pSector->begin(),
			pSector->end(),
			std::bind2nd(RenIStarsImplD3DLVERTEXHeightGreaterOrEqualBinPred(), minVertex));

		ctl_vector<RenIVertex>::iterator itMax = find_if(
			pSector->begin(),
			pSector->end(),
			std::bind2nd(RenIStarsImplD3DLVERTEXHeightGreaterOrEqualBinPred(), maxVertex));

		// It is quite possible that at the extremes of elevation there is no vertex
		// that satisfies the max height condition (especially with a lot of sectors).
		// We do not have to check for the minimum as the >= will always get a vertex
		// if there is one. The south cap rendering cuts out when this gets to low as well.
		if(itMax == pSector->end())
		{
			itMax = pSector->end();
			--itMax;
		}

		ASSERT_INFO(minHeight_);
		ASSERT(itMin != pSector->end(), "No minimum vertex could be found.");

		ptrdiff_t size = 0;

		//std::distance(itMin, itMax, size);
		size = std::distance(itMin, itMax);

		ASSERT(size > 0, "Tried to render an array of vertices with size == zero.");

		pVertexPtrs_->push_back(&(*itMin));
		pSizes_->push_back(size);
	}

private:
	ctl_vector<RenIVertex*>* const pVertexPtrs_;
	ctl_vector<int>* const pSizes_;
	float minHeight_;
	float maxHeight_;
};

////////////////////////////////////////////////////////////

class RenIStarsImplRenderVerticesOp : public std::unary_function<RenIVertex*, void>
{
public:
	RenIStarsImplRenderVerticesOp(ctl_vector<int>::iterator itSizes)
		:
		itSizes_(itSizes)
	{
	}

	void operator ()(RenIVertex* pVertex)
	{
        static RenMaterial emptyMat = RenMaterial();
        RenDevice::current()->renderPrimitive(pVertex, *itSizes_, emptyMat, GL_POINTS);
		++itSizes_;
	}

private:
	ctl_vector<int>::iterator itSizes_;
};

#endif /* _INTERNAL_RENDER_STARS_IMPL_FUNCTORS_HPP ********/


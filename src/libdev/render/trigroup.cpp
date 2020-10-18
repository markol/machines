/*
 * T R I G R O U P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "mathex/point3d.hpp"	// not used: only needed for templates
#include "render/texture.hpp"
#include "render/internal/trigroup.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/illumine.hpp"
#include "render/internal/devicei.hpp"
#include "stdlib/memory.hpp"

#include "render/device.hpp"

#ifndef _INLINE
    #include "render/internal/trigroup.ipp"
#endif

PER_DEFINE_PERSISTENT_ABSTRACT( RenIMaterialGroup );
PER_DEFINE_PERSISTENT_ABSTRACT( RenITriangleGroup );
PER_DEFINE_PERSISTENT( RenIDistinctGroup );
PER_DEFINE_PERSISTENT( RenILineGroup );

// Define an ordering for material groups.  This is the ordering that is
// necessary for correct and efficient rendering.  The groups are sorted
// by coplanar priority first, this is necessary for correct rendering
// of these polygons.  Then as an optimisation, the groups are sorted by
// texture -- if two materials share the same texture then we won't
// switch textures when rendering the list of groups.
bool RenIMaterialGroup::operator<(const RenIMaterialGroup& g) const
{
	// Note we use depth rather than priority because it's slightly
	// more efficient.  Because of the inverse relationshiop, (i.e.
	// priority = SHRT_MAX - depth), the inequality is reversed here.
	if (material_.coplanarDepth() > g.material_.coplanarDepth())
		return true;

	// If the priorities are equal, sort them by texture.
	if (material_.coplanarDepth() == g.material_.coplanarDepth())
	{
		if (!material_.texture().isNull() && !g.material_.texture().isNull())
		{
			const std::string  n1 =   material_.texture().name();
			const std::string  n2 = g.material_.texture().name();

			return n1 < n2;
		}
		else if (material_.texture().isNull() && !g.material_.texture().isNull())
		{
			// A material without texture is considered less.
			return true;
		}
	}

	// If the groups are considered equivalent by the ordering above, then
	// we don't care about further sorting.
	return false;
}

RenIMaterialGroup::RenIMaterialGroup( PerConstructor )
{
}

RenITriangleGroup::RenITriangleGroup( PerConstructor p )
: RenIMaterialGroup( p )
{
}


RenIDistinctGroup::RenIDistinctGroup( PerConstructor p )
: RenITriangleGroup( p )
{
    indices_.persistAsRaw( true );
}

RenIDistinctGroup::RenIDistinctGroup(const RenMaterial& mat, size_t nFaces):
	RenITriangleGroup(mat),
	indices_(),
	nIndicesUsed_(0)
{
	PRE(Ren::initialised());
	indices_.reserve(3 * nFaces);
    indices_.persistAsRaw( true );
}

RenIDistinctGroup::~RenIDistinctGroup()
{
}

inline
void pushBackIndex
(
	ctl_min_memory_vector<Ren::VertexIdx>* vec,
	Ren::VertexIdx  idx,
	Ren::VertexIdx* max
)
{
	vec->push_back(idx);

	Ren::VertexIdx ip1 = (Ren::VertexIdx)(idx + 1);
	if (ip1 > *max)
		*max = ip1;
}

void RenIDistinctGroup::addTriangle(Ren::VertexIdx i1, Ren::VertexIdx i2, Ren::VertexIdx i3)
{
	pushBackIndex(&indices_, i1, &nIndicesUsed_);
	pushBackIndex(&indices_, i2, &nIndicesUsed_);
	pushBackIndex(&indices_, i3, &nIndicesUsed_);
}

// virtual
void RenIDistinctGroup::render(const RenIVertexData& vtx, const RenMaterial& mat) const
{
	PRE(RenIDeviceImpl::currentPimpl());
	PRE((indices_.size() % 3) == 0);

	RenIDeviceImpl*   devImpl = RenIDeviceImpl::currentPimpl();
	RenIIlluminator*  ill     = devImpl->illuminator();

	devImpl->setMaterialHandles(mat);
	const RenIVertex* lit = ill->applyMaterial(mat, vtx, indices_);

	// Assume that the usual backface setting is on.
	if (!backFace())
        glDisable(GL_CULL_FACE);

	// We pass nIndicesUsed_ to Direct3D, rather than the true size of the lit array.
	// It is likely that nIndicesUsed_ will be smaller, so there's a possibility that
	// D3D will save time by processing less vertices.
	ASSERT(nIndicesUsed_ <= vtx.size(), "Indices used by group don't match mesh's vertices.");
     RenDevice::current()->renderIndexed(lit, nIndicesUsed_, &(indices_.front()), indices_.size(), mat, GL_TRIANGLES);

	if (!backFace())
        glEnable(GL_CULL_FACE);
}

// virtual
void RenIDistinctGroup::render(const RenI::LitVtxAPtr& vtx, const RenMaterial& mat) const
{
	PRE(RenIDeviceImpl::currentPimpl());
	PRE((indices_.size() % 3) == 0);

	RenIDeviceImpl*   devImpl = RenIDeviceImpl::currentPimpl();

	devImpl->setMaterialHandles(mat);

	// Assume that the usual backface setting is on.
	if (!backFace())
        glDisable(GL_CULL_FACE);

	// We pass nIndicesUsed_ to Direct3D, rather than the true size of the lit array.
	// It is likely that nIndicesUsed_ will be smaller, so there's a possibility that
	// D3D will save time by processing less vertices.
    RenDevice::current()->renderIndexed(vtx.get(), nIndicesUsed_, &(indices_.front()), indices_.size(), mat, GL_TRIANGLES);
	if (!backFace())
        glEnable(GL_CULL_FACE);
}

// virtual
RenI::LitVtxAPtr RenIDistinctGroup::light(const RenIVertexData& vtx, const RenMaterial& mat) const
{
	RenIIlluminator* ill = RenIDeviceImpl::currentPimpl()->illuminator();
	return ill->applyMaterialAndCopy(mat, vtx, indices_, nIndicesUsed_);
}

Ren::VertexIdx RenIDistinctGroup::firstVertexIdx() const
{
	PRE(indices_.size() > 0);
	return indices_[0];
}

Ren::VertexIdx RenIDistinctGroup::maxUsedIndices() const
{
	return nIndicesUsed_;
}

void RenIDistinctGroup::triangle
(
	Ren::TriangleIdx tri,
	Ren::VertexIdx*  v1,
	Ren::VertexIdx*  v2,
	Ren::VertexIdx*  v3
) const
{
	PRE(v1 && v2 && v3);
	PRE(tri < nTriangles());
	const int idx = tri * 3;
	*v1 = indices_[idx];
	*v2 = indices_[idx+1];
	*v3 = indices_[idx+2];
}

RenILineGroup::RenILineGroup( PerConstructor p )
: RenIMaterialGroup( p )
{
}

RenILineGroup::RenILineGroup(const RenMaterial& m):
	RenIMaterialGroup(m),
	indices_(),
	nIndicesUsed_(0)
{
	PRE(Ren::initialised());

	// Lines are assumed to be uncommon -- triangles predominate.  So most
	// meshes will contain zero lines and this is the optimal size.
	indices_.reserve(0);
}

// virtual
void RenILineGroup::render(const RenIVertexData& vtx, const RenMaterial& mat) const
{
	PRE(RenIDeviceImpl::currentPimpl());
	PRE((indices_.size() % 2) == 0);

	RenIDeviceImpl*   devImpl = RenIDeviceImpl::currentPimpl();
	RenIIlluminator*  ill     = devImpl->illuminator();

	devImpl->setMaterialHandles(mat);
	const RenIVertex* lit = ill->applyMaterial(mat, vtx, indices_);

	// We pass nIndicesUsed_ to Direct3D, rather than the true size of the lit array.
	// It is likely that nIndicesUsed_ will be smaller, so there's a possibility that
	// D3D will save time by processing less vertices.
	ASSERT(nIndicesUsed_ <= vtx.size(), "Indices used by group don't match mesh's vertices.");

    RenDevice::current()->renderIndexed(lit, nIndicesUsed_, &(indices_.front()), indices_.size(), mat, GL_LINES);
}

// virtual
void RenILineGroup::render(const RenI::LitVtxAPtr& vtx, const RenMaterial& mat) const
{
	PRE(RenIDeviceImpl::currentPimpl());
	PRE((indices_.size() % 2) == 0);

	RenIDeviceImpl*   devImpl = RenIDeviceImpl::currentPimpl();

	devImpl->setMaterialHandles(mat);

	// We pass nIndicesUsed_ to Direct3D, rather than the true size of the lit array.
	// It is likely that nIndicesUsed_ will be smaller, so there's a possibility that
	// D3D will save time by processing less vertices.
    RenDevice::current()->renderIndexed(vtx.get(), nIndicesUsed_, &(indices_.front()), indices_.size(), mat, GL_LINES);
}

// virtual
RenI::LitVtxAPtr RenILineGroup::light(const RenIVertexData& vtx, const RenMaterial& mat) const
{
	PRE(RenIDeviceImpl::currentPimpl());
	RenIIlluminator* ill = RenIDeviceImpl::currentPimpl()->illuminator();
	return ill->applyMaterialAndCopy(mat, vtx, indices_, nIndicesUsed_);
}

void RenILineGroup::addLine(Ren::VertexIdx i1, Ren::VertexIdx i2)
{
	pushBackIndex(&indices_, i1, &nIndicesUsed_);
	pushBackIndex(&indices_, i2, &nIndicesUsed_);
}

Ren::VertexIdx RenILineGroup::firstVertexIdx() const
{
	PRE(indices_.size() > 0);
	return indices_[0];
}

Ren::VertexIdx RenILineGroup::maxUsedIndices() const
{
	return nIndicesUsed_;
}

ostream& operator <<( ostream& ostr, const RenIMaterialGroup& group )
{
    group.write( ostr );
    return ostr;
}

// virtual
void    RenIMaterialGroup::write( ostream& ostr ) const
{
    ostr << material_;
}

void perWrite( PerOstream& ostr, const RenIMaterialGroup& group )
{
    ostr << group.material_;
}

void perRead( PerIstream& istr, RenIMaterialGroup& group )
{
    istr >> group.material_;
}

ostream& operator <<( ostream& ostr, const RenITriangleGroup& group )
{
    group.write( ostr );
    return ostr;
}

// virtual
void    RenITriangleGroup::write( ostream& ostr ) const
{
    RenIMaterialGroup::write( ostr );
}

void perWrite( PerOstream& ostr, const RenITriangleGroup& group )
{
    const RenIMaterialGroup& base = group;
    ostr << base << group.backFace_;
}

void perRead( PerIstream& istr, RenITriangleGroup& group )
{
    RenIMaterialGroup& base = group;
    istr >> base >> group.backFace_;
}

ostream& operator <<( ostream& ostr, const RenIDistinctGroup& group )
{
    group.write( ostr );
    return ostr;
}

// virtual
void    RenIDistinctGroup::write( ostream& ostr ) const
{
    RenITriangleGroup::write( ostr );
}

void perWrite( PerOstream& ostr, const RenIDistinctGroup& group )
{
    const RenITriangleGroup& base = group;
    ostr << base;
    _CONST_CAST(RenIDistinctGroup&, group).indices_.persistAsRaw(true); //TODO something corrupts memory and flag is cleared again?
    ostr << group.indices_;
    ostr << group.nIndicesUsed_;
}

void perRead( PerIstream& istr, RenIDistinctGroup& group )
{
    RenITriangleGroup& base = group;
    istr >> base;
    istr >> group.indices_;
    istr >> group.nIndicesUsed_;
}

ostream& operator <<( ostream& ostr, const RenILineGroup& group )
{
    group.write( ostr );
    return ostr;
}

// virtual
void    RenILineGroup::write( ostream& ) const
{
}

void perWrite( PerOstream& ostr, const RenILineGroup& group )
{
    const RenIMaterialGroup& base = group;
    ostr << base;
    ostr << group.indices_;
    ostr << group.nIndicesUsed_;
}

void perRead( PerIstream& istr, RenILineGroup& group )
{
    RenIMaterialGroup& base = group;
    istr >> base;
    istr >> group.indices_;
    istr >> group.nIndicesUsed_;
}

/* End TRIGROUP.CPP *************************************************/

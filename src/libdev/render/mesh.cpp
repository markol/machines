/*
 * M E S H . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "render/mesh.hpp"
#include "gxin/gxmesh.hpp"
#include "utility/factory.hpp"
#include "utility/indent.hpp"
#include "mathex/point3d.hpp"
#include "render/material.hpp"
#include "render/matvec.hpp"
#include "render/texture.hpp"
#include "render/surfmgr.hpp"
#include "render/ttfpoly.hpp"
#include "render/spinpoly.hpp"
#include "render/uvanim.hpp"
#include "render/scale.hpp"
#include "render/stats.hpp"
#include "render/vertex.hpp"
#include "render/internal/glmath.hpp"
#include "render/internal/meshload.hpp"
#include "render/internal/gxmeshload.hpp"
#include "xin/XFileHelper.hpp"

#include "render/internal/meshfact.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/vtxmat.hpp"
#include "render/internal/trigroup.hpp"
#include "render/internal/alphagrp.hpp"
#include "render/internal/coplagrp.hpp"
#include "render/internal/illumine.hpp"
#include "render/internal/matmgr.hpp"
#include "render/uvtrans.hpp"
#include "render/internal/devicei.hpp"
#include "render/device.hpp"

#include "ctl/vector.hpp"
#include <algorithm>
#include <memory>

// statics
ulong RenMesh::meshCount_   = 0;
ulong RenMesh::maxVertices_ = 0;

PER_DEFINE_PERSISTENT( RenMesh );

//////////////////////////////////////////////////////////////////////////
// A functor which populates a material set for a given mesh.
class MaterialEnumerator
{
public:
	MaterialEnumerator(RenMaterialVec* s): set_(s)	{ PRE(s); }
	void operator()(const RenIMaterialGroup* group)
	{
		set_->push_back(group->material());
	}

private:
	RenMaterialVec*	set_;
};

//////////////////////////////////////////////////////////////////////////
// A functor which changes all the materials in a given mesh.
class MaterialSetChanger
{
public:
	MaterialSetChanger(const RenMaterialVec* s): set_(s), iterator_(s->begin())	{ PRE(s); }
	RenMaterialVec::const_iterator iterator() const		{ return iterator_; }
	void operator()(RenIMaterialGroup* group)
	{
		group->material(*iterator_);
		++iterator_;
	}

private:
	const RenMaterialVec*			set_;
	RenMaterialVec::const_iterator	iterator_;
};

//////////////////////////////////////////////////////////////////////////
// A functor which counts all the triangles in a given mesh.
class TriCounter
{
public:
	TriCounter(): count_(0) {}
	size_t nTriangles() const	{ return count_; }
	void operator()(const RenITriangleGroup* group)
	{
		count_ += group->nTriangles();
	}

private:
	size_t count_;
};

//////////////////////////////////////////////////////////////////////////
// An STL binary predicate for sorting pointers to material groups.
class CompMatGroups
{
public:
	bool operator()(const RenIMaterialGroup* g1, const RenIMaterialGroup* g2)
	{
		PRE(g1 && g2);
		return *g1 < *g2;
	}
};

//////////////////////////////////////////////////////////////////////////
// static
Ren::ConstMeshPtr RenMesh::createShared
(
	const SysPathName& path,
	const string& meshName,
	double scale
)
{
	// Pre-conditions are in RenMesh::RenMesh().
	RenIMeshID id( path, meshName, scale );
	return RenIMeshFactory::instance().part(id);
}

// static
Ren::MeshPtr RenMesh::createUnique
(
	const SysPathName& path,
	const string& meshName,
	double scale
)
{
	// Pre-conditions are in RenMesh::RenMesh().
    PRE_INFO( path );
    PRE_INFO( meshName );

	RenMesh* mesh = _NEW(RenMesh);

	if (mesh->read(path, meshName, scale))
		return mesh;
	else
	{
		_DELETE(mesh);
		return NULL;
	}
}

// static
Ren::MeshPtr RenMesh::createEmpty()
{
	// Pre-conditions are in RenMesh::RenMesh().
	return _NEW(RenMesh);
}

RenMesh::RenMesh():
	vertices_(NULL),
	uvAnimated_(NULL),
	isDirty_( false ),
	pVertexTexture_(NULL)
{
	PRE(Ren::initialised());
	PRE(RenIDeviceImpl::current());
	++meshCount_;
    TEST_INVARIANT;
}

RenMesh::~RenMesh()
{
    TEST_INVARIANT;

	ctl_min_memory_vector<RenITriangleGroup*>::iterator dIt = triangles_.begin();
	while (dIt != triangles_.end())
	{
		_DELETE(*dIt);
		*dIt = NULL;
		++dIt;
	}

	ctl_min_memory_vector<RenTTFPolygon*>::iterator tIt = ttfs_.begin();
	while (tIt != ttfs_.end())
	{
		_DELETE(*tIt);
		*tIt = NULL;
		++tIt;
	}

	ctl_min_memory_vector<RenSpinTFPolygon*>::iterator sIt = stfps_.begin();
	while (sIt != stfps_.end())
	{
		_DELETE(*sIt);
		*sIt = NULL;
		++sIt;
	}

	ctl_min_memory_vector<RenILineGroup*>::iterator lIt = lines_.begin();
	while (lIt != lines_.end())
	{
		_DELETE(*lIt);
		*lIt = NULL;
		++lIt;
	}

	 _DELETE(vertices_);
	 _DELETE(uvAnimated_);

	 _DELETE( pVertexTexture_ );

	--meshCount_;
}

//-----------------------------------------------------------------------------
// Inline helper functions and classes for render methods -- the bulk of the
// rendering code is very similar for each method.  The common code is factored
// out here.
inline void renderPreconditions()
{
	PRE(Ren::initialised());
	PRE(RenIDeviceImpl::current());
	PRE(RenIDeviceImpl::currentPimpl());
//	PRE(RenIDeviceImpl::currentPimpl()->device());
	PRE(RenIDeviceImpl::currentPimpl()->rendering());
	PRE(RenIDeviceImpl::currentPimpl()->rendering3D());
}

inline void setWorldMatrix
(
	const MexTransform3d& world,
	glm::mat4& World,
	const RenScale& scale
)
{
	MexToGLMatrix(world, World);
	scale.apply(&World);
	RenDevice::current()->setModelMatrix(World);
}

inline void animateVertices(const RenIVertexData* in, RenIVertexData*& out,
                            const RenUVTransform& anim, const RenMesh::Textures& vertexTexture )
{
	if (in)
	{
		if (!out)
			out = _NEW(RenIVertexData(*in));

		RenIVertexData::const_iterator from = in->begin();
		RenIVertexData::iterator       to   = out->begin();

		size_t idx = 0;
		while (from != in->end() )
		{
			// If the animation doesn't apply to this vertex just copy the data,
			// otherwise animate the UV co-ordinates.
			if (!anim.appliesTo(vertexTexture[idx]))
				*to = *from;
			else
				anim.transform((*from).tu, (*from).tv, &((*to).tu), &((*to).tv));

			++from;
			++to;
			++idx;
		}
	}
}

// A functor which can be used by std::for_each to draw a RenIMaterialGroup.
template <class T>
class GroupRenderFunctor
{
public:
	GroupRenderFunctor(const RenIVertexData& vtx, const glm::mat4& w):
		vertices_(vtx), world_(w) {}
	void operator()(const T*) const;
private:
	const RenIVertexData& 	vertices_;
	const glm::mat4&		world_;
};

template <class T>
void GroupRenderFunctor<T>::operator()(const T* group) const
{
	const RenMaterial& mat = group->material();
	RenIDeviceImpl* devImpl = RenIDeviceImpl::currentPimpl();

	if (mat.hasAlphaTransparency())
	{
		if (devImpl->isAlphaSortingEnabled())
		{
			// Shove it into the post-sorter.
			RenI::LitVtxAPtr lit = group->light(vertices_, mat);
			RenIDelayedAlphaGroup* delayed = _NEW(RenIDelayedAlphaGroup(group, lit, mat, world_));

			if (mat.intraMeshAlphaPriority())
				delayed->meshId(RenMesh::meshId());

			std::auto_ptr<RenIDepthSortedItem> item ( delayed );
			devImpl->alphaSorter().addItem(item);
		}
		else
		{
			// There is no post-sorter, so enable alpha and render it now.
			devImpl->enableAlphaBlending();
			group->render(vertices_, group->material());
			devImpl->disableAlphaBlending();
		}
	}
	else if (mat.interMeshCoplanar())
	{
		RenI::LitVtxAPtr lit = group->light(vertices_, mat);
		RenIDelayedCoplanarGroup* delayed = _NEW(RenIDelayedCoplanarGroup(group, lit, mat, world_));
		std::auto_ptr<RenIPrioritySortedItem> item (delayed);
		devImpl->coplanarSorter().addItem(item);
	}
	else
	{
		group->render(vertices_, group->material());
	}

	// TBD: maybe take this out of the final version for performance.
	RenStats* stats = devImpl->statistics();
	if (stats)
	{
		stats->incrPolyCount(group->nTriangles());
		stats->incrLineCount(group->nLines());
	}
}

// A functor which can be used by std::for_each to draw a RenIMaterialGroup.
template <class T>
class GroupRenderFunctorMatOverride
{
public:
	GroupRenderFunctorMatOverride(const RenIVertexData& vtx, const glm::mat4& w, RenMaterialVec::const_iterator& i):
		vertices_(vtx), world_(w), matIt_(i) {}
	void operator()(const T*) const;
private:
	const RenIVertexData& 			vertices_;
	const glm::mat4&				world_;
	RenMaterialVec::const_iterator&	matIt_;
};

template <class T>
void GroupRenderFunctorMatOverride<T>::operator()(const T* group) const
{
	RenIDeviceImpl* devImpl = RenIDeviceImpl::currentPimpl();
	const RenMaterial& mat = *matIt_;
	++matIt_;

	if (mat.hasAlphaTransparency())
	{
		if (devImpl->isAlphaSortingEnabled())
		{
			// Shove it into the post-sorter.
			RenI::LitVtxAPtr lit = group->light(vertices_, mat);
			RenIDelayedAlphaGroup* delayed = _NEW(RenIDelayedAlphaGroup(group, lit, mat, world_));

			if (mat.intraMeshAlphaPriority())
				delayed->meshId(RenMesh::meshId());

			std::auto_ptr<RenIDepthSortedItem> item (delayed);
			devImpl->alphaSorter().addItem(item);
		}
		else
		{
			// There is no post-sorter, so enable alpha and render it now.
			devImpl->enableAlphaBlending();
			group->render(vertices_, mat);
			devImpl->disableAlphaBlending();
		}
	}
	else if (mat.interMeshCoplanar())
	{
		RenI::LitVtxAPtr lit = group->light(vertices_, mat);
		RenIDelayedCoplanarGroup* delayed = _NEW(RenIDelayedCoplanarGroup(group, lit, mat, world_));
		std::auto_ptr<RenIPrioritySortedItem> item (delayed);
		devImpl->coplanarSorter().addItem(item);
	}
	else
	{
		group->render(vertices_, mat);
	}

	// TBD: maybe take this out of the final version for performance.
	RenStats* stats = devImpl->statistics();
	if (stats)
	{
		stats->incrPolyCount(group->nTriangles());
		stats->incrLineCount(group->nLines());
	}
}

//-----------------------------------------------------------------------------
// Render methods.  There are overrides for material & texture coord animation.
// Both the transform and the scaling functor are mandatory parameters.
void RenMesh::render(const MexTransform3d& world, const RenScale& scale) const
{
	renderPreconditions();
	PRE(implies(nTriangles() > 0, vertices_));
	RenIDeviceImpl*    	devImpl = RenIDeviceImpl::currentPimpl();
    glm::mat4 glWorld;
	setWorldMatrix(world, glWorld, scale);

	if (vertices_)
		devImpl->illuminator()->lightVertices(*vertices_);

	// Ensure that the post sorter has a unique ID for each rendered instance.
	nextId();

	// Invoke a functor for each triangle groups and all the line groups.
	GroupRenderFunctor<RenIMaterialGroup> rg1(*vertices_, glWorld);
	std::for_each(triangles_.begin(), triangles_.end(), rg1);
	std::for_each(lines_    .begin(), lines_    .end(), rg1);

	if (ttfs_.size() > 0)
	{
        glm::mat4 m = devImpl->projViewMatrix() * glWorld;

        MATHEX_SCALAR averageScale = scale.average();

		ctl_min_memory_vector<RenTTFPolygon*>::const_iterator tIt = ttfs_.begin();
		while (tIt != ttfs_.end())
		{
			const RenTTFPolygon* poly = *tIt;
			poly->render(m, averageScale, NULL);
			++tIt;
		}
	}

	if (stfps_.size() > 0)
	{
        glDisable(GL_CULL_FACE);

		ctl_min_memory_vector<RenSpinTFPolygon*>::const_iterator sIt = stfps_.begin();
		while (sIt != stfps_.end())
		{
			const RenSpinTFPolygon* spinPoly = *sIt;
			spinPoly->render(world, scale, NULL);
			++sIt;
		}

        glEnable(GL_CULL_FACE);
	}
}

void RenMesh::render
(
	const MexTransform3d& world,
	const RenMaterialVec* mats,
	const RenScale& scale
) const
{
	renderPreconditions();
	PRE(implies(nTriangles() > 0, vertices_));
	PRE(mats);
	PRE(mats->size() >= nMaterials());
	RenIDeviceImpl*    	devImpl = RenIDeviceImpl::currentPimpl();

    glm::mat4 glWorld;
	setWorldMatrix(world, glWorld, scale);

	if (vertices_)
		devImpl->illuminator()->lightVertices(*vertices_);

	// Ensure that the post sorter has a unique ID for each rendered instance.
	nextId();

	// Render all the distinct triangle groups and all the line groups.
	RenMaterialVec::const_iterator matIt = mats->begin();
	GroupRenderFunctorMatOverride<RenIMaterialGroup> rg1(*vertices_, glWorld, matIt);
	std::for_each(triangles_.begin(), triangles_.end(), rg1);
	std::for_each(lines_    .begin(), lines_    .end(), rg1);

	if (ttfs_.size() > 0)
	{
        glm::mat4 m = devImpl->projViewMatrix() * glWorld;
        MATHEX_SCALAR averageScale = scale.average();

		ctl_min_memory_vector<RenTTFPolygon*>::const_iterator tIt = ttfs_.begin();
		while (tIt != ttfs_.end())
		{
			const RenTTFPolygon* poly = *tIt;
			poly->render(m, *matIt, averageScale, NULL);
			++tIt;
			++matIt;
		}
	}

	if (stfps_.size() > 0)
	{
        glDisable(GL_CULL_FACE);

		ctl_min_memory_vector<RenSpinTFPolygon*>::const_iterator stIt = stfps_.begin();
		while (stIt != stfps_.end())
		{
			const RenSpinTFPolygon* sPoly = *stIt;
			sPoly->render(world, *matIt, scale, NULL);
			++stIt;
			++matIt;
		}

        glEnable(GL_CULL_FACE);
	}
}

void RenMesh::render
(
	const MexTransform3d& world,
	const RenUVTransform& anim,
	const RenScale& scale
) const
{
	renderPreconditions();
	PRE(implies(nTriangles() > 0, vertices_));
	RenIDeviceImpl*    	devImpl = RenIDeviceImpl::currentPimpl();

    glm::mat4 glWorld;
    setWorldMatrix(world, glWorld, scale);

	if( pVertexTexture_ == NULL )
	{
		_CONST_CAST(RenMesh*, this)->createTextures();
	}

	animateVertices(vertices_, _CONST_CAST(RenMesh*, this)->uvAnimated_, anim, *pVertexTexture_);

	if (uvAnimated_)
		devImpl->illuminator()->lightVertices(*uvAnimated_);

	// Ensure that the post sorter has a unique ID for each rendered instance.
	nextId();

	// Render all the distinct triangle groups and all the line groups.
	GroupRenderFunctor<RenIMaterialGroup> rg1(*uvAnimated_, glWorld);
	std::for_each(triangles_.begin(), triangles_.end(), rg1);
	std::for_each(lines_    .begin(), lines_    .end(), rg1);

	if (ttfs_.size() > 0)
	{
        glm::mat4 m = devImpl->projViewMatrix() * glWorld;

        MATHEX_SCALAR averageScale = scale.average();

		ctl_min_memory_vector<RenTTFPolygon*>::const_iterator tIt = ttfs_.begin();
		while (tIt != ttfs_.end())
		{
			const RenTTFPolygon* poly = *tIt;
			poly->render(m, averageScale, &anim);
			++tIt;
		}
	}

	if (stfps_.size() > 0)
	{
        glDisable(GL_CULL_FACE);

		ctl_min_memory_vector<RenSpinTFPolygon*>::const_iterator stIt = stfps_.begin();
		while (stIt != stfps_.end())
		{
			const RenSpinTFPolygon* sPoly = *stIt;
			sPoly->render(world, scale, &anim);
			++stIt;
		}

        glEnable(GL_CULL_FACE);
	}
}

void RenMesh::render
(
	const MexTransform3d& world,
	const RenMaterialVec* mats,
	const RenUVTransform& anim,
	const RenScale& scale
) const
{
	renderPreconditions();
	PRE(implies(nTriangles() > 0, vertices_));
	PRE(mats);
	PRE(mats->size() >= nMaterials());
	RenIDeviceImpl*    	devImpl = RenIDeviceImpl::currentPimpl();

    glm::mat4 glWorld;
    setWorldMatrix(world, glWorld, scale);

	if( pVertexTexture_ == NULL )
	{
		_CONST_CAST(RenMesh*, this)->createTextures();
	}

	animateVertices(vertices_, _CONST_CAST(RenMesh*, this)->uvAnimated_, anim, *pVertexTexture_);

	if (uvAnimated_)
		devImpl->illuminator()->lightVertices(*uvAnimated_);

	// Ensure that the post sorter has a unique ID for each rendered instance.
	nextId();

	// Render all the distinct triangle groups and all the line groups.
	RenMaterialVec::const_iterator matIt = mats->begin();
	GroupRenderFunctorMatOverride<RenIMaterialGroup> rg1(*uvAnimated_, glWorld, matIt);
	std::for_each(triangles_.begin(), triangles_.end(), rg1);
	std::for_each(lines_    .begin(), lines_    .end(), rg1);

	if (ttfs_.size() > 0)
	{
        glm::mat4 m = devImpl->projViewMatrix() * glWorld;
        MATHEX_SCALAR averageScale = scale.average();

		ctl_min_memory_vector<RenTTFPolygon*>::const_iterator tIt = ttfs_.begin();
		while (tIt != ttfs_.end())
		{
			const RenTTFPolygon* poly = *tIt;
			poly->render(m, *matIt, averageScale, &anim);
			++tIt;
			++matIt;
		}
	}

	if (stfps_.size() > 0)
	{
        glDisable(GL_CULL_FACE);

		ctl_min_memory_vector<RenSpinTFPolygon*>::const_iterator stIt = stfps_.begin();
		while (stIt != stfps_.end())
		{
			const RenSpinTFPolygon* sPoly = *stIt;
			sPoly->render(world, *matIt, scale, &anim);
			++stIt;
			++matIt;
		}

        glEnable(GL_CULL_FACE);
	}
}

//-----------------------------------------------------------------------------
// Accessors and modifiers for the bits and pieces of geometry.
// Count the total number of triangles in this mesh.  The returned
// value excludes TTFs.
int RenMesh::nVertices() const
{
	return (vertices_)? vertices_->size(): 0;
}

RenVertex RenMesh::vertex(Ren::VertexIdx index)
{
	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());
	PRE(index < nVertices());
	return RenVertex(this, index);
}

const RenVertex RenMesh::vertex(Ren::VertexIdx index) const
{
	PRE(index < nVertices());

	// The RenVertex ctor takes a non-const mesh pointer.  We *trust* that if
	// a vertex is const it will not (logically) modify this mesh even though
	// the compiler would allow non-const access.
	RenMesh* crufty = _CONST_CAST(RenMesh*, this);
	return RenVertex(crufty, index);
}

void RenMesh::resetAllVertexIntensities()
{
	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());
	if (vertices_)
		vertices_->resetAllIntensities();
}

void RenMesh::resetAllVertexMaterials()
{
	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());
	if (vertices_)
		vertices_->resetAllMaterials();
}

int RenMesh::nTriangles() const
{
	TriCounter counter;
	std::for_each(triangles_.begin(), triangles_.end(), counter);
	return counter.nTriangles();
}

int RenMesh::nLines() const
{
	int total = 0;

	ctl_min_memory_vector<RenILineGroup*>::const_iterator tIt = lines_.begin();
	while (tIt != lines_.end())
	{
		const RenILineGroup* lines = *tIt;
		total += lines->nLines();
		++tIt;
	}

	return total;
}

void RenMesh::addLine(const MexPoint3d& p1, const MexPoint3d& p2, const RenMaterial& mat)
{
	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());

	const double epsilon = 1e-4;
	const Ren::VertexIdx idx1 = addOrFindVertex(p1, epsilon);
	const Ren::VertexIdx idx2 = addOrFindVertex(p2, epsilon);

	RenILineGroup* group = NULL;

	// Search to see if there is an existing group with the required material.
	ctl_min_memory_vector<RenILineGroup*>::iterator it = lines_.begin();
	while (it != lines_.end())
	{
		group = (*it);
		if (group && group->material() == mat)
			break;

		++it;
	}

	// If there isn't an existing group, create a new one.
	if (!group)
	{
		group = _NEW(RenILineGroup(mat));
		lines_.push_back(group);
	}

	// One way or another we should have a valid group by now.
	ASSERT(group, "Failed to find or create new line material group.");
	ASSERT(lines_.size() > 0, "Failed to find or create new line material group.");

	group->addLine(idx1, idx2);

    isDirty_ = true;
}

static void checkMaxVertices(const RenIVertexData* v, ulong& max)
{
	if (v && v->size() > max)
		max = v->size();
}

Ren::VertexIdx RenMesh::addOrFindVertex(const MexPoint3d& pt, double epsilon)
{
	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());

	if (!vertices_)
		vertices_ = _NEW(RenIVertexData(20));

	const double epSqr = epsilon * epsilon;

	// Search for an existing vertex within epsilon of the given one.
	RenIVertexData::iterator it = vertices_->begin();
	Ren::VertexIdx result = 0;
	while (it != vertices_->end())
	{
		MexPoint3d vtx((*it).x, (*it).y, (*it).z);
		if (vtx.sqrEuclidianDistance(pt) < epSqr)
			return result;

		++it;
		++result;
	}

	static const MexVec3 newNormal(0,0,1);
	static const MexPoint2d newUV(0,0);
	vertices_->addVertex(pt, newNormal, newUV);
	checkMaxVertices(vertices_, maxVertices_);
    isDirty_ = true;
	return result;
}

size_t RenMesh::nMaterials() const
{
	// This is assuming that there's one group per material which is
	// currently the case but need not be so.  Watch out!  This won't
	// work in the case where there are materials shared between the
	// different types of polygon.
	return triangles_.size() + lines_.size() + ttfs_.size() + stfps_.size();
}

void RenMesh::materialVec(const RenMaterialVec* mats)
{
	PRE(mats);
	PRE(mats->size() == nMaterials());

	MaterialSetChanger changer(mats);
	std::for_each(triangles_.begin(), triangles_.end(), changer);
	std::for_each(lines_    .begin(), lines_    .end(), changer);

	ctl_min_memory_vector<RenTTFPolygon*>::iterator tIt = ttfs_.begin();
	RenMaterialVec::const_iterator matIt = changer.iterator();

	while (tIt != ttfs_.end())
	{
		RenTTFPolygon* ttf = *tIt;
		ttf->material(*matIt);
		++tIt;
		++matIt;
	}

	ctl_min_memory_vector<RenSpinTFPolygon*>::iterator sIt = stfps_.begin();

	matIt = changer.iterator();

	while (sIt != stfps_.end())
	{
		RenSpinTFPolygon* stfp = *sIt;
		stfp->material(*matIt);
		++sIt;
		++matIt;
	}
}

std::auto_ptr<RenMaterialVec> RenMesh::materialVec() const
{
	RenMaterialVec* matSet = _NEW(RenMaterialVec(nMaterials()));
	MaterialEnumerator enumerator(matSet);
	std::for_each(triangles_.begin(), triangles_.end(), enumerator);
	std::for_each(lines_    .begin(), lines_    .end(), enumerator);

	// TBD: this will add a material to the set for every single TTF.  When
	// the material manager exists, then it should be easy to check that only
	// unique materials are added to the set.
	ctl_min_memory_vector<RenTTFPolygon*>::const_iterator tIt = ttfs_.begin();
	while (tIt != ttfs_.end())
	{
		const RenTTFPolygon* ttf = *tIt;
		matSet->push_back(ttf->material());
		++tIt;
	}

	ctl_min_memory_vector<RenSpinTFPolygon*>::const_iterator sIt = stfps_.begin();
	while (sIt != stfps_.end())
	{
		const RenSpinTFPolygon* stf = *sIt;
		matSet->push_back(stf->material());
		++sIt;
	}

	//return matSet; TODO check
	return std::auto_ptr<RenMaterialVec>(matSet);
}

int RenMesh::nTTFPolygons() const
{
	return ttfs_.size();
}

const RenTTFPolygon& RenMesh::TTFPolygon(Ren::TriangleIdx idx) const
{
	PRE(idx < ttfs_.size());
	return *(ttfs_[idx]);
}

RenTTFPolygon& RenMesh::TTFPolygon(Ren::TriangleIdx idx)
{
	PRE(idx < ttfs_.size());
	return *(ttfs_[idx]);
}

void RenMesh::addTTFPolygon(const RenTTFPolygon& addMe)
{
	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());

	// If one polygon is added to a mesh, it is quite likely that more
	// will be, so increase the size of the vector.
	if (ttfs_.size() == 0)
		ttfs_.reserve(40);

	ttfs_.push_back(addMe.clone());
    isDirty_ = true;
}

void RenMesh::removeTTFPolygon(Ren::TriangleIdx idx)
{
	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());

	_DELETE(ttfs_[idx]);
	ttfs_.erase(ttfs_.begin() + idx);
    isDirty_ = true;
}

// TBD: put this back in (or a replacement).  It's used by W4dSprite.
// void RenMesh::modifyTTFPolygon(Ren::TriangleIdx idx, const RenTTFPolygon& newTTF)
// {
// 	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());
// 	PRE(idx < ttfs_.size());
// 	*(ttfs_[idx]) = newTTF;
//     isDirty_ = true;
// }

int RenMesh::nSpinTFPolygons() const
{
	return stfps_.size();
}

const RenSpinTFPolygon& RenMesh::spinTFPolygon(Ren::TriangleIdx idx) const
{
	PRE(idx < stfps_.size());
	return *(stfps_[idx]);
}

void RenMesh::addSpinTFPolygon(const RenSpinTFPolygon& addMe)
{
	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());

	// If one polygon is added to a mesh, it is quite likely that more
	// will be, so increase the size of the vector.
	if (stfps_.size() == 0)
		stfps_.reserve(40);

	stfps_.push_back(_NEW(RenSpinTFPolygon(addMe)));
    isDirty_ = true;
}

void RenMesh::removeSpinTFPolygon(Ren::TriangleIdx idx)
{
	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());
	_DELETE(stfps_[idx]);
	stfps_.erase(stfps_.begin() + idx);
    isDirty_ = true;
}

void RenMesh::modifySpinTFPolygon(Ren::TriangleIdx idx, const RenSpinTFPolygon& newSpinTFPolygon)
{
	PRE(!RenIDeviceImpl::currentPimpl()->rendering3D());
	PRE(idx < stfps_.size());
	*(stfps_[idx]) = newSpinTFPolygon;
    isDirty_ = true;
}

void RenMesh::updateAllData()
{
    updateBoundingVolume();
    isDirty_ = false;
}

void RenMesh::updateBoundingVolume()
{
    MATHEX_SCALAR xMin = 999999999.9;	 //just mean a very large number
    MATHEX_SCALAR yMin = xMin;
    MATHEX_SCALAR zMin = xMin;
    MATHEX_SCALAR xMax = -xMin;
    MATHEX_SCALAR yMax = xMax;
    MATHEX_SCALAR zMax = xMax;

	bool vertice = false;
	bool ttfs = false;
	bool stfs = false;

	if (vertices_  and  vertices_->size() != 0 )
	{
		vertice = true;

        RenIVertexData::iterator i = (*vertices_).begin();

        while( i != vertices_->end() )
        {
            MATHEX_SCALAR x = (*i).x;
            MATHEX_SCALAR y = (*i).y;
            MATHEX_SCALAR z = (*i).z;

            if( x > xMax )
                xMax = x;

            if( x < xMin )
                xMin = x;

            if( y > yMax )
                yMax = y;

            if( y < yMin )
                yMin = y;

            if( z > zMax )
                zMax = z;

            if( z < zMin )
                zMin = z;

			++i;
        }
	}

	if( ttfs_.size() != 0 )
	{
		ttfs = true;

		ctl_min_memory_vector<RenTTFPolygon*>::const_iterator tIt = ttfs_.begin();
		while (tIt != ttfs_.end())
		{
			const RenTTFPolygon* ttf = *tIt;
			const MATHEX_SCALAR radiusOfBoundingSphere = ttf->radius();
			const MexPoint3d& centre = ttf->centre();

			MATHEX_SCALAR xMinTTF = centre.x()-radiusOfBoundingSphere;
			MATHEX_SCALAR yMinTTF = centre.y()-radiusOfBoundingSphere;
			MATHEX_SCALAR zMinTTF = centre.z()-radiusOfBoundingSphere;

			MATHEX_SCALAR xMaxTTF = centre.x()+radiusOfBoundingSphere;
			MATHEX_SCALAR yMaxTTF = centre.y()+radiusOfBoundingSphere;
			MATHEX_SCALAR zMaxTTF = centre.z()+radiusOfBoundingSphere;

			if( xMinTTF < xMin )
				xMin = xMinTTF;

			if( yMinTTF < yMin )
				yMin = yMinTTF;

			if( zMinTTF < zMin )
				zMin = zMinTTF;

			if( xMaxTTF > xMax )
				xMax = xMaxTTF;

			if( yMaxTTF > yMax )
				yMax = yMaxTTF;

			if( zMaxTTF > zMax )
				zMax = zMaxTTF;

			++tIt;
		}
	}

	if( stfps_.size() != 0 )
	{
		stfs = true;

		ctl_min_memory_vector<RenSpinTFPolygon*>::const_iterator tIt = stfps_.begin();
		while (tIt != stfps_.end())
		{
			const RenSpinTFPolygon* stf = *tIt;

			const RenIVertexData* verts = stf->vertices();
			ASSERT(verts, "" );
			MexPoint3d basePoint = stf->spinAxis().base();

			MATHEX_SCALAR xMinSTF = (*verts)[0].x;
			MATHEX_SCALAR yMinSTF = (*verts)[0].y;

			MATHEX_SCALAR xMaxSTF = xMinSTF;
			MATHEX_SCALAR yMaxSTF = yMinSTF;

			for(uint i = 1; i<verts->size(); ++i)
			{
				if( xMinSTF > (*verts)[i].x )
					xMinSTF = (*verts)[i].x;

				if( xMaxSTF < (*verts)[i].x )
					xMaxSTF = (*verts)[i].x;

				if( yMinSTF > (*verts)[i].y )
					yMinSTF = (*verts)[i].y;

				if( yMaxSTF < (*verts)[i].y )
					yMaxSTF = (*verts)[i].y;
			}

			MATHEX_SCALAR minSTF = std::min(xMinSTF, yMinSTF);
			MATHEX_SCALAR maxSTF = std::max(xMaxSTF, yMaxSTF);


			xMinSTF = basePoint.x()+minSTF;
			xMaxSTF = basePoint.x()+maxSTF;

			yMinSTF = basePoint.y()+minSTF;
			yMaxSTF = basePoint.y()+maxSTF;

			MATHEX_SCALAR zMinSTF = basePoint.z()+minSTF;
			MATHEX_SCALAR zMaxSTF = basePoint.z()+maxSTF;

			if( xMinSTF < xMin )
				xMin = xMinSTF;

			if( yMinSTF < yMin )
				yMin = yMinSTF;

			if( zMinSTF < zMin )
				zMin = zMinSTF;

			if( xMaxSTF > xMax )
				xMax = xMaxSTF;

			if( yMaxSTF > yMax )
				yMax = yMaxSTF;

			if( zMaxSTF > zMax )
				zMax = zMaxSTF;

			++tIt;
		}
	}

	if (vertice or ttfs or stfs )
	{
        //boundingVolume_.corners( MexPoint3d( xMin, yMin, zMin ), MexPoint3d( xMax, yMax, zMax ) );
        boundingVolume_ = MexAlignedBox3d( MexPoint3d( xMin, yMin, zMin ), MexPoint3d( xMax, yMax, zMax ) );
	}
	else
	{
        boundingVolume_ = MexAlignedBox3d( MexPoint3d( 0, 0, 0 ), MexPoint3d( 0, 0, 0 ) );
	}
}

const MexAlignedBox3d& RenMesh::boundingVolume() const
{
    //ensure up-to-date
    if( isDirty_ )
    {
        _CONST_CAST(RenMesh*, this)->updateAllData();
    }

    return boundingVolume_;
}

// There are different levels of debug info available during mesh loading.
#define DBG_LOAD0(x)	RENDER_STREAM(x)
#define DBG_LOAD1(x)   	RENDER_STREAM(x)
#define DBG_LOAD2(x)    RENDER_STREAM(x)
#define DBG_LOAD3(x) 	RENDER_STREAM(x)

struct RenStreamIndenter
{
	RenStreamIndenter()		{ RENDER_INDENT(2); }
	~RenStreamIndenter()	{ RENDER_INDENT(-2); }
};

// Load a named mesh from a file.
bool RenMesh::read(const SysPathName& pathName, const string& meshName, double scale)
{
    PRE_INFO( pathName );
    PRE_INFO( meshName );

    RENDER_STREAM("RenMesh::read(" << pathName << "," << meshName << "," << scale << ")" << std::endl);

	PRE(meshName.length() > 0);			// a valid mesh name must be supplied
	PRE(RenIDeviceImpl::current());

    SysPathName withExtDX( pathName );
    withExtDX.extension( "x" );

	if (withExtDX.existsAsFile())
	{

      // Create a temporary Retained Mode mesh builder to read the .x file.
	  DBG_LOAD0("Loading mesh " << meshName << " from " << pathName << std::endl);
	  RenStreamIndenter indenter;

	  RenID3DMeshLoader::MeshData meshData = RenID3DMeshLoader::instance().load(withExtDX, meshName);
	  ASSERT_INFO(meshName);

      if (!meshData.mesh)
         return false;

//	  if (!TRYRENDX(builder->Scale(scale, scale, scale)))
//	   	return false;

	  if (!buildFromXMesh(meshData.scene, meshData.mesh))
	   	return false;

	  // Initialise these iff everything loaded correctly.
	  pathName_ = withExtDX.pathname();
	  meshName_ = meshName;
	  isDirty_ = true;
	  return true;
	}

    // No .X file, try to load an agt file
    SysPathName withExtAGT( pathName );
    withExtAGT.extension( "agt" );
	if (withExtAGT.existsAsFile())
	{
	  DBG_LOAD0("Loading mesh " << meshName << " from " << pathName << std::endl);
	  RenStreamIndenter indenter;

	  GXMesh *gxmesh=RenIGXMeshLoader::instance().load(withExtAGT,meshName);
	  if (!gxmesh)
	    return false;
	  DBG_LOAD3("Loaded AGT mesh {" << *gxmesh << "}" << std::endl);

	  if(!buildFromGXMesh(gxmesh))
	    return false;

	  DBG_LOAD0("Conversion to the RenMesh format succeeded" << std::endl);

      pathName_ = withExtAGT.pathname();
	  meshName_ = meshName;
	  isDirty_ = true;
      return true;
    }

    // file was neither .agt nor .x
    RENDER_STREAM("RenMesh::read() ; Could not read file " << withExtDX << " or file " << withExtAGT << std::endl);
	return false;


}

// If a triangle has two vertices which are closer than 0.01 units, then
// assume that a line will do and discard one of the very close vertices.
static bool isLine(int i1, int i2, int i3, RenIVertexData* vertices, int& line1, int& line2)
{
	const RenIVertexData& v = *vertices;
	MexPoint3d p1(v[i1].x, v[i1].y, v[i1].z);
	MexPoint3d p2(v[i2].x, v[i2].y, v[i2].z);
	MexPoint3d p3(v[i3].x, v[i3].y, v[i3].z);

	const MATHEX_SCALAR verySmallSquared = 0.01 * 0.01;	// 1 cm

	const MATHEX_SCALAR d1 = p1.sqrEuclidianDistance(p2);
	DBG_LOAD3("Distance from " << p1 << " to " << p2 << " is " << d1 << std::endl);

	if (d1 < verySmallSquared)
	{
		line1 = i1;
		line2 = i3;
		return true;
	}

	const MATHEX_SCALAR d2 = p1.sqrEuclidianDistance(p3);
	DBG_LOAD3("Distance from " << p1 << " to " << p3 << " is " << d2 << std::endl);

	if (d2 < verySmallSquared)
	{
		line1 = i1;
		line2 = i2;
		return true;
	}

	const MATHEX_SCALAR d3 = p2.sqrEuclidianDistance(p3);
	DBG_LOAD3("Distance from " << p2 << " to " << p3 << " is " << d3 << std::endl);

	if (d3 < verySmallSquared)
	{
		line1 = i1;
		line2 = i2;
		return true;
	}

	return false;
}

static bool findMatch(const RenIVertexData& vertices,
					  int i1, int i2, int i3, int i4, int i5, int i6, int& m)
{
	bool match;

	const MATHEX_SCALAR epsilon = 0.0005;

	bool v14 = fabs(vertices[i1].x - vertices[i4].x ) < epsilon && fabs(vertices[i1].y - vertices[i4].y ) < epsilon && fabs(vertices[i1].z - vertices[i4].z ) < epsilon;
	bool v15 = fabs(vertices[i1].x - vertices[i5].x ) < epsilon && fabs(vertices[i1].y - vertices[i5].y ) < epsilon && fabs(vertices[i1].z - vertices[i5].z ) < epsilon;
	bool v16 = fabs(vertices[i1].x - vertices[i6].x ) < epsilon && fabs(vertices[i1].y - vertices[i6].y ) < epsilon && fabs(vertices[i1].z - vertices[i6].z ) < epsilon;

	bool v24 = fabs(vertices[i2].x - vertices[i4].x ) < epsilon && fabs(vertices[i2].y - vertices[i4].y ) < epsilon && fabs(vertices[i2].z - vertices[i4].z ) < epsilon;
	bool v25 = fabs(vertices[i2].x - vertices[i5].x ) < epsilon && fabs(vertices[i2].y - vertices[i5].y ) < epsilon && fabs(vertices[i2].z - vertices[i5].z ) < epsilon;
	bool v26 = fabs(vertices[i2].x - vertices[i6].x ) < epsilon && fabs(vertices[i2].y - vertices[i6].y ) < epsilon && fabs(vertices[i2].z - vertices[i6].z ) < epsilon;

	bool v34 = fabs(vertices[i3].x - vertices[i4].x ) < epsilon && fabs(vertices[i3].y - vertices[i4].y ) < epsilon && fabs(vertices[i3].z - vertices[i4].z ) < epsilon;
	bool v35 = fabs(vertices[i3].x - vertices[i5].x ) < epsilon && fabs(vertices[i3].y - vertices[i5].y ) < epsilon && fabs(vertices[i3].z - vertices[i5].z ) < epsilon;
	bool v36 = fabs(vertices[i3].x - vertices[i6].x ) < epsilon && fabs(vertices[i3].y - vertices[i6].y ) < epsilon && fabs(vertices[i3].z - vertices[i6].z ) < epsilon;

	if( (v14 && v26 )||(v24 && v16 )||(v24 && v36 )||(v34 && v26 )||(v14 && v36 )||(v34 && v16 ) )
	{
		m = i5;
		match = true;
	}

	else if( (v15 && v26 )||(v25 && v16 )||(v25 && v36 )||(v35 && v26 )||(v15 && v36 )||(v35 && v16 ) )
	{
		m = i4;
		match = true;
	}

	else if( (v14 && v25 )||(v24 && v15 )||(v24 && v35 )||(v34 && v25 )||(v14 && v35 )||(v34 && v15 ) )
	{
		m = i6;
		match = true;
	}
 	else
	{
		m = 0;
		match = false;
	}

	return match;
}


//non member function
static void spinPolyElements
(
    const RenIVertexData& vertices,
	int i1, int i2, int i3, int i4, bool match,
    ctl_vector<MexPoint2d> &verts, ctl_vector<MexPoint2d> &uvs,	MexPoint3d& base,
	const MexVec3& direction

)
{
	//if the spin axis is the z-axis, then a 90 degree turn clockwise must be done since the spin axis in the stf
	// system is always the local x axis
	bool isZAxis = direction.isPerpendicularTo(MexVec3(1, 0, 0)) && direction.isPerpendicularTo(MexVec3(0, 1, 0));

	MexPoint2d vert1, vert2, vert3, vert4;

	const MATHEX_SCALAR tollerance = 0.01;

	bool xEqConst = (fabs(vertices[i1].x - vertices[i2].x) <tollerance) && (fabs(vertices[i1].x - vertices[i3].x) <tollerance);
	bool yEqConst = (fabs(vertices[i1].y - vertices[i2].y) <tollerance) && (fabs(vertices[i1].y - vertices[i3].y) <tollerance);
	bool zEqConst = (fabs(vertices[i1].z - vertices[i2].z) <tollerance) && (fabs(vertices[i1].z - vertices[i3].z) <tollerance);

	ASSERT_INFO( vertices[i1] );
	ASSERT_INFO( vertices[i2] );
	ASSERT_INFO( vertices[i3] );
	ASSERT_INFO( tollerance );
	ASSERT(xEqConst || yEqConst || zEqConst, "A spin-to-face must lie in a plane of constant x,y or z co-ordinates.");

	MATHEX_SCALAR constValue;
	MexPoint2d base2;

	if(not match)
	{
		if(xEqConst)
		{
			vert1 = MexPoint2d( vertices[i1].y, vertices[i1].z );
			vert2 = MexPoint2d( vertices[i2].y, vertices[i2].z );
			vert3 = MexPoint2d( vertices[i3].y, vertices[i3].z );
			constValue = (vertices[i1].x + vertices[i2].x + vertices[i3].x)/3.0;
		}

		if(yEqConst)
		{
			yEqConst = true;
			vert1 = MexPoint2d( vertices[i1].x, vertices[i1].z );
			vert2 = MexPoint2d( vertices[i2].x, vertices[i2].z );
			vert3 = MexPoint2d( vertices[i3].x, vertices[i3].z );
			constValue = (vertices[i1].y + vertices[i2].y + vertices[i3].y)/3.0;
		}


		if(zEqConst)
		{
			zEqConst = true;
			vert1 = MexPoint2d( vertices[i1].x, vertices[i1].y );
			vert2 = MexPoint2d( vertices[i2].x, vertices[i2].y );
			vert3 = MexPoint2d( vertices[i3].x, vertices[i3].y );
			constValue = (vertices[i1].z + vertices[i2].z + vertices[i3].z)/3.0;
		}

  		MexPoint2d uv1 = MexPoint2d( vertices[i1].tu, vertices[i1].tv );
		MexPoint2d uv2 = MexPoint2d( vertices[i2].tu, vertices[i2].tv );
		MexPoint2d uv3 = MexPoint2d( vertices[i3].tu, vertices[i3].tv );

		if(isZAxis)
		{
			vert1 = MexPoint2d(vert1.y(), -vert1.x());
			vert2 = MexPoint2d(vert2.y(), -vert2.x());
			vert3 = MexPoint2d(vert3.y(), -vert3.x());
		}


		MATHEX_SCALAR x = (vert1.x() + vert2.x() + vert3.x() ) / 3.0;
		MATHEX_SCALAR y = (vert1.y() + vert2.y() + vert3.y() ) / 3.0;

		base2 = MexPoint2d(x, y);

		vert1 -= base2;
		vert2 -= base2;
		vert3 -= base2;

 		verts.push_back(vert1);
		verts.push_back(vert2);
		verts.push_back(vert3);

 		uvs.push_back(uv1);
		uvs.push_back(uv2);
		uvs.push_back(uv3);
	}

	if(match)
	{
		if(xEqConst)
		{
			vert1 = MexPoint2d( vertices[i1].y, vertices[i1].z );
			vert2 = MexPoint2d( vertices[i2].y, vertices[i2].z );
			vert3 = MexPoint2d( vertices[i3].y, vertices[i3].z );
			vert4 = MexPoint2d( vertices[i4].y, vertices[i4].z );
			constValue = (vertices[i1].x + vertices[i2].x + vertices[i3].x + vertices[i4].x)/4.0;
		}

		if(yEqConst)
		{
			yEqConst = true;
			vert1 = MexPoint2d( vertices[i1].x, vertices[i1].z );
			vert2 = MexPoint2d( vertices[i2].x, vertices[i2].z );
			vert3 = MexPoint2d( vertices[i3].x, vertices[i3].z );
			vert4 = MexPoint2d( vertices[i4].x, vertices[i4].z );
			constValue = (vertices[i1].y + vertices[i2].y + vertices[i3].y + vertices[i4].y)/4.0;
		}


		if(zEqConst)
		{
			zEqConst = true;
			vert1 = MexPoint2d( vertices[i1].x, vertices[i1].y );
			vert2 = MexPoint2d( vertices[i2].x, vertices[i2].y );
			vert3 = MexPoint2d( vertices[i3].x, vertices[i3].y );
			vert4 = MexPoint2d( vertices[i4].x, vertices[i4].y );
			constValue = (vertices[i1].z + vertices[i2].z + vertices[i3].z + vertices[i4].z)/4.0;
		}

  		MexPoint2d uv1 = MexPoint2d( vertices[i1].tu, vertices[i1].tv );
		MexPoint2d uv2 = MexPoint2d( vertices[i2].tu, vertices[i2].tv );
		MexPoint2d uv3 = MexPoint2d( vertices[i3].tu, vertices[i3].tv );
		MexPoint2d uv4 = MexPoint2d( vertices[i4].tu, vertices[i4].tv );

		if(isZAxis)
		{
			vert1 = MexPoint2d(vert1.y(), -vert1.x());
			vert2 = MexPoint2d(vert2.y(), -vert2.x());
			vert3 = MexPoint2d(vert3.y(), -vert3.x());
			vert4 = MexPoint2d(vert4.y(), -vert4.x());
		}

		MATHEX_SCALAR x = (vert1.x() + vert2.x() + vert3.x() + vert4.x() ) / 4.0;
		MATHEX_SCALAR y = (vert1.y() + vert2.y() + vert3.y() + vert4.y() ) / 4.0;

		base2 = MexPoint2d(x, y);

		vert1 -= base2;
		vert2 -= base2;
		vert3 -= base2;
		vert4 -= base2;

		verts.push_back(vert1);
		verts.push_back(vert2);
		verts.push_back(vert3);
		verts.push_back(vert4);

		uvs.push_back(uv1);
		uvs.push_back(uv2);
		uvs.push_back(uv3);
		uvs.push_back(uv4);
	}

	if(xEqConst)
		base = MexPoint3d(constValue, base2.x(), base2.y());

	if(yEqConst)
		base = MexPoint3d(base2.x(), constValue, base2.y());

	if(zEqConst)
		base = MexPoint3d(base2.x(), base2.y(), constValue);

	//if(isZAxis)
		//base = MexPoint3d(-base.z(), base.y(), base.x());

	if(isZAxis)
	{
		if(xEqConst)
			base = MexPoint3d(constValue, -base2.y(), base2.x());

		if(yEqConst)
			base = MexPoint3d(-base2.y(), constValue, base2.x());

		if(zEqConst)
			base = MexPoint3d(-base2.y(), base2.x(), constValue);
	}
}

//local
static MATHEX_SCALAR areaOfTriangle(const MexPoint2d& p1, const MexPoint2d& p2, const MexPoint2d& p3)
{
	return p1.x() * ( p3.y()-p2.y() ) +
	       p2.x() * ( p1.y()-p3.y() ) +
	       p3.x() * ( p2.y()-p1.y() );
}

//local
static void unBowTie(ctl_vector<MexPoint2d> &verts)
{
 	int length = verts.size();

	if( length == 4 )
	{
		MATHEX_SCALAR areaOfTriangle012 = areaOfTriangle(verts[0], verts[1], verts[2]);
		MATHEX_SCALAR areaOfTriangle123 = areaOfTriangle(verts[1], verts[2], verts[3]);
		MATHEX_SCALAR areaOfTriangle230 = areaOfTriangle(verts[2], verts[3], verts[0]);

		bool pair1 = (areaOfTriangle012 * areaOfTriangle123 > 0);
		bool pair2 = (areaOfTriangle012 * areaOfTriangle230 > 0);

		bool NotBowTie = pair1&&pair2;

		if( !NotBowTie )
		{

			if( (areaOfTriangle012 * areaOfTriangle123 < 0)	&& (areaOfTriangle230 * areaOfTriangle123 > 0) )
				std::swap(verts[3], verts[2]);

			else
				std::swap(verts[0], verts[3]);
 		}
	}
}

//local
static void ttfPolyElements
(   const RenIVertexData& vertices,
	int i1, int i2, int i3, int i4,
    MexPoint3d* centre,
    MATHEX_SCALAR* width,
    MATHEX_SCALAR* height,
	ctl_vector<MexPoint2d> &uvs
)
{
	const MATHEX_SCALAR epsilon = 0.0005;
	MexPoint2d vert1, vert2, vert3, vert4;

	bool xEqConst = (fabs(vertices[i1].x - vertices[i2].x) <epsilon) && (fabs(vertices[i1].x - vertices[i3].x) <epsilon);
	bool yEqConst = (fabs(vertices[i1].y - vertices[i2].y) <epsilon) && (fabs(vertices[i1].y - vertices[i3].y) <epsilon);
	bool zEqConst = (fabs(vertices[i1].z - vertices[i2].z) <epsilon) && (fabs(vertices[i1].z - vertices[i3].z) <epsilon);

	MATHEX_SCALAR constValue;

	if(xEqConst)
	{
		vert1 = MexPoint2d( vertices[i1].y, vertices[i1].z );
		vert2 = MexPoint2d( vertices[i2].y, vertices[i2].z );
		vert3 = MexPoint2d( vertices[i3].y, vertices[i3].z );
		vert4 = MexPoint2d( vertices[i4].y, vertices[i4].z );

		constValue = (vertices[i1].x + vertices[i2].x + vertices[i3].x + vertices[i4].x)/4.0;
	}

	if(yEqConst)
	{
		vert1 = MexPoint2d( vertices[i1].x, vertices[i1].z );
		vert2 = MexPoint2d( vertices[i2].x, vertices[i2].z );
		vert3 = MexPoint2d( vertices[i3].x, vertices[i3].z );
		vert4 = MexPoint2d( vertices[i4].x, vertices[i4].z );

		constValue = (vertices[i1].y + vertices[i2].y + vertices[i3].y + vertices[i4].y)/4.0;
	}

	if(zEqConst)
	{
		vert1 = MexPoint2d( vertices[i1].x, vertices[i1].y );
		vert2 = MexPoint2d( vertices[i2].x, vertices[i2].y );
		vert3 = MexPoint2d( vertices[i3].x, vertices[i3].y );
		vert4 = MexPoint2d( vertices[i4].x, vertices[i4].y );

		constValue = (vertices[i1].z + vertices[i2].z + vertices[i3].z + vertices[i4].z)/4.0;
	}

  	MexPoint2d uv1 = MexPoint2d( vertices[i1].tu, vertices[i1].tv );
	MexPoint2d uv2 = MexPoint2d( vertices[i2].tu, vertices[i2].tv );
	MexPoint2d uv3 = MexPoint2d( vertices[i3].tu, vertices[i3].tv );
	MexPoint2d uv4 = MexPoint2d( vertices[i4].tu, vertices[i4].tv );

	MATHEX_SCALAR minX = vert1.x();
	if(minX > vert2.x() ) minX = vert2.x();
	if(minX > vert3.x() ) minX = vert3.x();
	if(minX > vert2.x() ) minX = vert4.x();

	MATHEX_SCALAR minY = vert1.y();
	if(minY > vert2.y() ) minY = vert2.y();
	if(minY > vert3.y() ) minY = vert3.y();
	if(minY > vert2.y() ) minY = vert4.y();

	MATHEX_SCALAR maxX = vert1.x();
	if(maxX < vert2.x() ) maxX = vert2.x();
	if(maxX < vert3.x() ) maxX = vert3.x();
	if(maxX < vert2.x() ) maxX = vert4.x();

	MATHEX_SCALAR maxY = vert1.y();
	if(maxY < vert2.y() ) maxY = vert2.y();
	if(maxY < vert3.y() ) maxY = vert3.y();
	if(maxY < vert2.y() ) maxY = vert4.y();

	*height = maxY - minY;
	*width = maxX - minX;

	MATHEX_SCALAR averageX = 0.25*(vert1.x() + vert2.x() + vert3.x() + vert4.x() );
	MATHEX_SCALAR averageY = 0.25*(vert1.y() + vert2.y() + vert3.y() + vert4.y() );

	if(xEqConst)
		*centre = MexPoint3d(constValue, averageX, averageY);

	if(yEqConst)
		*centre = MexPoint3d(averageX, constValue, averageY);

	if(zEqConst)
		*centre = MexPoint3d(averageX, averageY, constValue);

	uvs.push_back(uv4);
	uvs.push_back(uv3);
	uvs.push_back(uv2);
 	uvs.push_back(uv1);

	unBowTie(uvs);
}

bool RenMesh::copyFromMeshBuilder(IDirect3DRMMeshBuilder* builder)
{
	PRE(builder);
// THIS CODE IS LEFT FOR REFERENCE! Some values in model file are magic numbers. 
	// Create a temporary mesh from the builder object.
//    COMPTR(IDirect3DRMMesh) mesh;
//    if (!TRYRENDX(builder->CreateMesh(mesh.address())))
        return 0;

    //Get the number of groups
//    const size_t nGroups = mesh->GetGroupCount();

    // Visit each group and total up the vertex count from each group.
	size_t totalVertices = 0;
//    for( D3DRMGROUPINDEX groupIdx = 0; groupIdx < nGroups; ++groupIdx )
    {
        //Get the count data for this group
        size_t fCount, vCount, vPerFace;
//        DWORD fDataSize;
//        TRYRENDX(mesh->GetGroup( groupIdx, &vCount, &fCount, &vPerFace, &fDataSize, NULL));
		totalVertices += vCount;
	}

	// Create an array of vertices with exactly the required amount of space.
	vertices_ = _NEW(RenIVertexData(totalVertices));

    // Visit each group and create a list of triangles for each one.
    size_t nVertices = 0;
//    for( D3DRMGROUPINDEX groupIdx = 0; groupIdx < nGroups; ++groupIdx )
    {
        //Get the count data for this group
        size_t fCount, vCount, vPerFace;
        //DWORD fDataSize;
        //TRYRENDX(mesh->GetGroup( groupIdx, &vCount, &fCount, &vPerFace, &fDataSize, NULL));

        //Allocate some memory to get the vertices
        //D3DRMVERTEX* pVertices = _NEW_ARRAY( D3DRMVERTEX, vCount );
        //POST( pVertices != NULL );

        //Get the vertices
        //TRYRENDX(mesh->GetVertices(groupIdx, 0, vCount, pVertices));

		DBG_LOAD2("	group contains " << vCount << " vertices:" << std::endl);
        for (size_t i = 0; i < vCount; ++i)
        {
/*			const D3DRMVERTEX& rmVtx = pVertices[i];

			const MexPoint3d pt(rmVtx.position.x, rmVtx.position.y, rmVtx.position.z);
			const MexVec3 norm(rmVtx.normal.x, rmVtx.normal.y, rmVtx.normal.z);
			const MexPoint2d uv(rmVtx.tu, rmVtx.tv);
*/
			// It may be nonsensical, but 3D Studio appears to create zero
			// vector normals for some polygons.  The addVertex code chokes
			// on these, so filter out the zeros.
			static const MexVec3 almostZeroVec(0,0, 1.2 * MexEpsilon::instance());

/*			if (norm.isZeroVector())
				vertices_->addVertex(pt, almostZeroVec, uv);
			else
				vertices_->addVertex(pt, norm, uv);
*/
//			DBG_LOAD2("		added vtx " << pt << std::endl);
		}
		DBG_LOAD2("	group contains " << vertices_->normals().size() << " normals" << std::endl);

        //Free the vertex memory
//        _DELETE_ARRAY( pVertices );

        //Allocate some memory to receive the face data
//        size_t* fData = _NEW_ARRAY( size_t, fDataSize );

        //Get the face data
//        TRYRENDX(mesh->GetGroup( groupIdx, &vCount, &fCount, &vPerFace, &fDataSize, fData));

		// TBD: copy the other material attributes.
/*		const D3DVALUE scale = 1.0;
		const D3DCOLOR color = mesh->GetGroupColor(groupIdx);
		const D3DVALUE red   = D3DRMColorGetRed  (color);
		const D3DVALUE green = D3DRMColorGetGreen(color);
		const D3DVALUE blue  = D3DRMColorGetBlue (color);
		const D3DVALUE alpha = D3DRMColorGetAlpha(color);

		RenMaterial newMat(RenColour(scale * red, scale * green, scale * blue, alpha));

		// If the material has a specular power set, then use that as a coplanar
		// polygon priority.
		COMPTR(IDirect3DRMMaterial) rmMat;
		TRYRENDX(mesh->GetGroupMaterial(groupIdx, rmMat.address()));

		D3DVALUE specularRed=0;

		if (rmMat)
		{
			D3DVALUE power = rmMat->GetPower();

			ASSERT(power < SHRT_MAX, "Specular power must fit into a short.");
			ASSERT(power > SHRT_MIN, "Specular power must fit into a short.");
			const short shortPower = _STATIC_CAST(short, power);

			D3DVALUE r,g, dummy;
			TRYRENDX(rmMat->GetEmissive(&r, &g, &dummy));
			TRYRENDX(rmMat->GetSpecular(&specularRed, &dummy, &dummy));

			// Coplanar and special alpha sorting aren't actually mutually exclusive.
			// Fortunately the one current example where we need both (shadows)
			// is hardcoded in world4d.
			if (r > 0.5 && r <= 1.5)
			{
				// Check the limits on the coplanar priority but *only* for inter-mesh coplanar.
				newMat.interMeshCoplanar(true);
//				RenIMatManager::instance().checkCoplanarValue(newMat.coplanarPriority());
				newMat.coplanarPriority(shortPower);
				DBG_LOAD0("Set inter-mesh priority=" << shortPower << " for " << newMat << "\n");
			}
			else if (r > 1.5 && r <= 2.5)
			{
				newMat.intraMeshAlphaPriority(true);
				newMat.alphaPriority(shortPower);
				DBG_LOAD0("Set intra-mesh alpha priority=" << shortPower << " for " << newMat << "\n");
			}
			else if (r > 2.5 && r <= 3.5)
			{
				// If the flag is 3, negate the priority.
				newMat.absoluteAlphaPriority(true);
				newMat.alphaPriority(_STATIC_CAST(short, -shortPower));
				DBG_LOAD0("Set absolute alpha priority=" << -shortPower << " for " << newMat << "\n");
			}
			else if (r > 3.5 && r <= 4.5)
			{
				newMat.absoluteAlphaPriority(true);
				newMat.alphaPriority(shortPower);
				DBG_LOAD0("Set absolute alpha priority=" << shortPower << " for " << newMat << "\n");
			}

			// The polygon's diffuse colour is multiplied by the green component
			// of the emissive value and the result is used as the emissive material.
			RenColour emissive(g*red, g*green, g*blue);
			newMat.emissive(emissive);

			// If the polygon is emissive, set the diffuse colour to black.
			// Otherwise, we get lighting overflows for every emissive poly.
			// Is this tollerance OK?
			if (g > 0.03)
			{
				newMat.diffuse(RenColour(0,0,0, alpha));
				DBG_LOAD0("Set emissive for " << newMat << "\n");
			}
		}

		COMPTR(IDirect3DRMTexture) rmTex;
		TRYRENDX(mesh->GetGroupTexture(groupIdx, rmTex.address()));

		if (rmTex)
		{
			// The Retained Mode texture loading callback should have
			// previously created a RM texture and set its app data to
			// be a pointer to the required RenTexture.
			DWORD dword = rmTex->GetAppData();
			//RenTexture* tex  = _STATIC_CAST(RenTexture*, dword);
			RenTexture* tex  = _REINTERPRET_CAST(RenTexture*, dword);
			RenTexture copy = *tex;
			newMat.texture(copy);

			// TBD: re-implement mesh's list of textures.
			// texturesUsed_.push_back(newTex);
		}

		// Use the material's specular red component to indicate that a
		// triangle group contains spin-to-face polygons.

		bool makeSTF = false;
		MexVec3 direction;

		if(specularRed == 2.0)
		{
			direction = MexVec3(1, 0, 0);
			makeSTF = true;
		}

		if(specularRed == 3.0)
		{
			direction = MexVec3(0, 1, 0);
			makeSTF = true;
		}

		if(specularRed == 4.0)
		{
			direction = MexVec3(0, 0, 1);
			makeSTF = true;
		}

		if  (makeSTF )
		{
			size_t f1 = fCount;
        	size_t i = 0;

			ctl_vector<size_t> matchedTriangles;
			matchedTriangles.reserve(fCount);

			while(f1 >= 1)
			{
	            size_t nFaceVertices = (vPerFace == 0) ? fData[i++] : vPerFace;
            	ASSERT(i < fDataSize, logic_error());
				ASSERT(nFaceVertices == 3, runtime_error("Can only read triangles from .x files."));

				int i1 = fData[i++] + nVertices;
				int i2 = fData[i++] + nVertices;
				int i3 = fData[i++] + nVertices;

				ASSERT(i1 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));
				ASSERT(i2 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));
				ASSERT(i3 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));

				bool match = false;
				bool matched;
				int m;
				size_t j = i;
				size_t f2 = f1 -1;

				while(f2 > 0 && (match == false) )
        		{
            		nFaceVertices = (vPerFace == 0) ? fData[j++] : vPerFace;
            		ASSERT(j < fDataSize, logic_error());
					ASSERT(nFaceVertices == 3, runtime_error("Can only read triangles from .x files."));

					int i4 = fData[j++] + nVertices;
					int i5 = fData[j++] + nVertices;
					int i6 = fData[j++] + nVertices;

				   	ASSERT(i4 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));
					ASSERT(i5 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));
					ASSERT(i6 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));

					match = findMatch(*vertices_, i1, i2, i3, i4, i5, i6, m);
					if(match)
					{
						matchedTriangles.push_back(f2);
					}
					--f2;
				}


				if (not match)
				{
					ctl_vector<size_t>::iterator it = std::find(matchedTriangles.begin(), matchedTriangles.end(), f1);

					// not matched before if it's not in the list.
					if (it == matchedTriangles.end())
					{
						matched = false;
					}
					else
					{
						matched = true;
					}
				}
	  	  		if( match or not matched )
				{
	  		   		RenSpinTFPolygon stfPoly;

					ctl_vector<MexPoint2d> verts;
					verts.reserve(4);
					ctl_vector<MexPoint2d> uvs;
					uvs.reserve(4);

					MexPoint3d basePoint;

					// uv ust be asigned ahead of vertices otherwise uv coords may be in bowtie shape.
					spinPolyElements(*vertices_, i1, i2, i3, m, match, verts, uvs, basePoint, direction);

					stfPoly.uv(uvs);
					stfPoly.vertices(verts);

 					stfPoly.material(newMat);
					stfPoly.spinAxis(RenSpinTFPolygon::SpinAxis(basePoint, direction));

					stfPoly.normalType(RenSpinTFPolygon::USE_AXIS);
					addSpinTFPolygon(stfPoly);

					//Iain wants this stream to identify mysterious	stf
					RENDER_STREAM("an stf is created from a .x file: " << stfPoly << std::endl);
				}

				--f1;
			}

			nVertices += vCount;
		}
		else if  (specularRed == 5.0 or fabs(specularRed) >= 10.0 )
		{
			//make TTFs
			size_t f1 = fCount;
        	size_t i = 0;

			while(f1 >= 1)
			{
	            size_t nFaceVertices = (vPerFace == 0) ? fData[i++] : vPerFace;
            	ASSERT(i < fDataSize, logic_error());
				ASSERT(nFaceVertices == 3, runtime_error("Can only read triangles from .x files."));

				int i1 = fData[i++] + nVertices;
				int i2 = fData[i++] + nVertices;
				int i3 = fData[i++] + nVertices;

				ASSERT(i1 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));
				ASSERT(i2 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));
				ASSERT(i3 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));

				bool match = false;
				int m;
				size_t j = i;
				size_t f2 = f1 -1;

				while(f2 > 0 && (match == false) )
        		{
            		nFaceVertices = (vPerFace == 0) ? fData[j++] : vPerFace;
            		ASSERT(j < fDataSize, logic_error());
					ASSERT(nFaceVertices == 3, runtime_error("Can only read triangles from .x files."));

					int i4 = fData[j++] + nVertices;
					int i5 = fData[j++] + nVertices;
					int i6 = fData[j++] + nVertices;

				   	ASSERT(i4 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));
					ASSERT(i5 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));
					ASSERT(i6 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));

					match = findMatch(*vertices_, i1, i2, i3, i4, i5, i6, m);

					--f2;
				}

	  	  		if( match )
				{
			  		RenTTFRectangle ttf;
					MexPoint3d centre;
					MATHEX_SCALAR width, height;
					ctl_vector<MexPoint2d> uv;
						  uv.reserve(4);

					ttfPolyElements(*vertices_, i1, i2, i3, m, &centre, &width, &height, uv);
					ttf.centre(centre);
					ttf.width(width);
					ttf.height(height);
					ttf.material(newMat);
					ttf.uv(uv[0], 0);
					ttf.uv(uv[1], 1);
					ttf.uv(uv[2], 2);
					ttf.uv(uv[3], 3);

					if( specularRed >= 10.0 )
						ttf.depthOffset( specularRed - 10.0 );

					else if( specularRed <= -10.0 )
						ttf.depthOffset( specularRed + 10.0 );

					else
						ttf.depthOffset( 0.0 );

					addTTFPolygon(ttf);
				}

				--f1;
			}

			nVertices += vCount;
		}

		else
		{
			// Create a tri group.  It won't be used unless we actually find some
			// lines.  Ditto for lines.
			RenIDistinctGroup* group = _NEW(RenIDistinctGroup(newMat, fCount));
			RenILineGroup* lineGroup = _NEW(RenILineGroup(newMat));

        	//Add each face
        	size_t i = 0;
        	while( fCount-- )
        	{
				// Immediate mode can only draw triangles -- barf if there is
				// anything else in the .x file.
            	const size_t nFaceVertices = (vPerFace == 0) ? fData[i++] : vPerFace;
            	ASSERT(i < fDataSize, logic_error());
				ASSERT(nFaceVertices == 3, runtime_error("Can only read triangles from .x files."));

				const int i1 = fData[i++] + nVertices;
				const int i2 = fData[i++] + nVertices;
				const int i3 = fData[i++] + nVertices;

				ASSERT(i1 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));
				ASSERT(i2 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));
				ASSERT(i3 <= Ren::maxVertices, runtime_error("Too many vertices in mesh."));

				int line1, line2;
				if (isLine(i1, i2, i3, vertices_, line1, line2))
				{
					DBG_LOAD2("	added line (" << line1 << "," << line2 << ")" << std::endl);
					lineGroup->addLine((Ren::VertexIdx) line1, (Ren::VertexIdx) line2);
				}
				else
				{
					DBG_LOAD2("	added triangle (" << i1 << "," << i2 << "," << i3 << ")" << std::endl);
					group->addTriangle((Ren::VertexIdx) i1, (Ren::VertexIdx) i2, (Ren::VertexIdx) i3);
				}
        	}

			nVertices += vCount;

			// Only use the groups if tehy now contains more than zero triangles or lines.
			if (group->nTriangles() > 0)
			{
				triangles_.push_back(group);

				// Use the material's specular red component to indicate that
				// a triangle group should not be backface culled.
				if (specularRed >= 6.0)
					group->backFace(false);
			}
        	else
            	_DELETE( group );

			if (lineGroup->nLines() > 0)
				lines_.push_back(lineGroup);
        	else
            	_DELETE( lineGroup );

			}

	        _DELETE_ARRAY( fData );*/
    }

	// We traverse the groups twice and compute a total number of vertices
	// separately each time.  They should, of course, be the same.
	ASSERT(nVertices == totalVertices, logic_error(""));
	checkMaxVertices(vertices_, maxVertices_);

	// Sort the distinct polygon groups for correct coplanar ordering.
	PRE_INFO(triangles_.size());

	PRE_INFO(triangles_.end() - triangles_.begin());
	std::sort(triangles_.begin(), triangles_.end(), CompMatGroups());

	DBG_LOAD1(std::endl << "New sorted mesh has " << triangles_.size() << " groups:" << std::endl);
	ctl_min_memory_vector<RenITriangleGroup*>::const_iterator dIt = triangles_.begin();
	while (dIt != triangles_.end())
	{
		const RenITriangleGroup* group = *dIt;
		const size_t nTri = group->nTriangles();
		const RenMaterial& mat = group->material();
		DBG_LOAD1("   Created new group with " << nTri << " tris and " << mat << std::endl);
		DBG_LOAD1("   Set coplanar priorty to " << mat.coplanarPriority() << std::endl);
		++dIt;
	}
	DBG_LOAD1(std::endl);

    return true;
}

bool RenMesh::buildFromXMesh(XFile::Scene* scene, XFile::Mesh* mesh)
{
    PRE(mesh->mPosFaces.size() == mesh->mNormFaces.size());
    PRE(mesh->mPosFaces.size() == mesh->mFaceMaterials.size());

    Ren::VertexIdx renVertexIndex;
    vertices_ = _NEW(RenIVertexData(mesh->mPositions.size()));

    for(uint i = 0; i < mesh->mPositions.size(); ++i)
    {
        MexPoint3d renVertex(   mesh->mPositions[i].x,
                                mesh->mPositions[i].y,
                                mesh->mPositions[i].z);
        // each face has corresponding 3*vertex index and 3*normal index from separate tables
        // this is not handled correctly now
        // number of normals is greater or equal number of vericies
        // if not then the first one is used multiple times
        MexVec3 renNormal(mesh->mNormals[0].x,
                              mesh->mNormals[0].y,
                              mesh->mNormals[0].z);
        if( mesh->mNormals.size() >= mesh->mPositions.size() )
        {
            renNormal = MexVec3(mesh->mNormals[i].x,
                                mesh->mNormals[i].y,
                                mesh->mNormals[i].z);
        }

        MexVec2 renUV;
        // Add texture coordinates if file have them defined
        if ( mesh->mPositions.size() == mesh->mTexCoords[0].size())
            renUV = MexVec2(mesh->mTexCoords[0][i].x, mesh->mTexCoords[0][i].y);

        renVertexIndex=_STATIC_CAST(Ren::VertexIdx, vertices_->size());
        if(renNormal.modulus() > 1)
            renNormal.makeUnitVector();

        // It may be nonsensical, but 3d Studio appears to create zero
        // vector normals for some polygons.  The addVertex code chokes
        // on these, so filter out the zeros.
        static const MexVec3 almostZeroVec(0,0, 1.2 * MexEpsilon::instance());
        if (renNormal.isZeroVector())
            vertices_->addVertex(renVertex, almostZeroVec, renUV);
        else
            vertices_->addVertex(renVertex,renNormal, renUV);
    }

/////////////////////////
    std::vector<RenIDistinctGroup*> triangles;
    std::vector<XFile::Material>::iterator it = mesh->mMaterials.begin();
    while(it != mesh->mMaterials.end())
    {
        RenMaterial renMat;
        XFile::Material* material;
        if(it->mIsReference)
            material = scene->mGlobalMaterials[it->sceneIndex];
        else
            material = &(*it);

        float red = material->mDiffuse.r;
        float green = material->mDiffuse.g;
        float blue = material->mDiffuse.b;
        float alpha = material->mDiffuse.a;
        renMat.diffuse(RenColour(red, green, blue, alpha));


        if(material->mTextures.size() > 0 and material->mTextures[0].mName.size() > 4)
        {
            string txName = material->mTextures[0].mName;
            RenTexture renTex = RenSurfaceManager::instance().createTexture(txName);
            renMat.texture(renTex);
        }

        float r = material->mEmissive.r;
        float g = material->mEmissive.g;
        const short shortPower = _STATIC_CAST(short, material->mSpecularExponent);

        // Coplanar and special alpha sorting aren't actually mutually exclusive.
        // Fortunately the one current example where we need both (shadows)
        // is hardcoded in world4d.
        if (r > 0.5 && r <= 1.5)
        {
            // Check the limits on the coplanar priority but *only* for inter-mesh coplanar.
            renMat.interMeshCoplanar(true);
            RenIMatManager::instance().checkCoplanarValue(renMat.coplanarPriority());
            renMat.coplanarPriority(shortPower);
            DBG_LOAD0("Set inter-mesh priority=" << shortPower << " for " << renMat << "\n");
        }
        else if (r > 1.5 && r <= 2.5)
        {
            renMat.intraMeshAlphaPriority(true);
            renMat.alphaPriority(shortPower);
            DBG_LOAD0("Set intra-mesh alpha priority=" << shortPower << " for " << renMat << "\n");
        }
        else if (r > 2.5 && r <= 3.5)
        {
            // If the flag is 3, negate the priority.
            renMat.absoluteAlphaPriority(true);
            renMat.alphaPriority(_STATIC_CAST(short, -shortPower));
            DBG_LOAD0("Set absolute alpha priority=" << -shortPower << " for " << renMat << "\n");
        }
        else if (r > 3.5 && r <= 4.5)
        {
            renMat.absoluteAlphaPriority(true);
            renMat.alphaPriority(shortPower);
            DBG_LOAD0("Set absolute alpha priority=" << shortPower << " for " << renMat << "\n");
        }

        // The polygon's diffuse colour is multiplied by the green component
        // of the emissive value and the result is used as the emissive material.
        RenColour emissive(g*red, g*green, g*blue);
        renMat.emissive(emissive);

        // If the polygon is emissive, set the diffuse colour to black.
        // Otherwise, we get lighting overflows for every emissive poly.
        // Is this tollerance OK?
        if (g > 0.03)
        {
            renMat.diffuse(RenColour(0,0,0, alpha));
            DBG_LOAD0("Set emissive for " << renMat << "\n");
        }

        RenIDistinctGroup* triangleGroup = _NEW(RenIDistinctGroup(renMat));

        // Use the material's specular red component to indicate that
        // a triangle group should not be backface culled.
        if (material->mSpecular.r >= 6.0)
            triangleGroup->backFace(false);

        triangles_.push_back(triangleGroup);
        triangles.push_back(triangleGroup);
        ++it;
    }

    for(uint i = 0; i < mesh->mPosFaces.size(); ++i)
    {
        size_t groupNum = mesh->mFaceMaterials[i];
        RenIDistinctGroup* triangleGroup = triangles[groupNum];
        ASSERT(mesh->mPosFaces[i].mIndices.size() == 3, "Can only read triangles from .x files.");
        triangleGroup->addTriangle( (Ren::VertexIdx) mesh->mPosFaces[i].mIndices[0],
                                    (Ren::VertexIdx) mesh->mPosFaces[i].mIndices[1],
                                    (Ren::VertexIdx) mesh->mPosFaces[i].mIndices[2]);
    }

    checkMaxVertices(vertices_, maxVertices_);

    // Sort the distinct polygon groups for correct coplanar ordering.
	PRE_INFO(triangles_.size());

	PRE_INFO(triangles_.end() - triangles_.begin());
	std::sort(triangles_.begin(), triangles_.end(), CompMatGroups());

	DBG_LOAD1(std::endl << "New sorted mesh has " << triangles_.size() << " groups:" << std::endl);
	ctl_min_memory_vector<RenITriangleGroup*>::const_iterator dIt = triangles_.begin();
	while (dIt != triangles_.end())
	{
		const RenITriangleGroup* group = *dIt;
		const size_t nTri = group->nTriangles();
		const RenMaterial& mat = group->material();
		DBG_LOAD1("   Created new group with " << nTri << " tris and " << mat << std::endl);
		DBG_LOAD1("   Set coplanar priorty to " << mat.coplanarPriority() << std::endl);
		++dIt;
	}
	DBG_LOAD1(std::endl);

    return true;
}

bool RenMesh::buildFromGXMesh(GXMesh *gxmesh)
{

  // array containing the indexes (in the GXMesh) of polygons having the same color
  typedef ctl_vector<int> PolygonIndexes;
  // map of polygon index (each element correspond to polygon having the same material)
  // this map is used to perform the grouping job
  typedef ctl_map<GXMat, PolygonIndexes, std::less<GXMat> > GXPolygonMap;
  GXPolygonMap gxPSetMap;

  // map containing the indexes correspondance (Gx vertex <-> render vertex index)
  typedef ctl_map<GXPolyVert3,Ren::VertexIdx,std::less<GXPolyVert3> > IndexesMap;

  IndexesMap renIndexesMap;
  int renNumVertices=0;

  GXPolygon3 gxPolygon;
  GXPolyVert3 gxVertex;
  GXUVCoords gxUV;

  int gxNormalIndex;
  int gxVertexIndex;
  Ren::VertexIdx renVertexIndex;

  DBG_LOAD0("The AGT mesh " << gxmesh->name() << " is now being converted to the RenMesh format" << std::endl);

  // We perform a first sweep of the polygons data structures, in which:
  // 1) the number of polygon groups is counted and the indexes of the polygons
  //    in each group is recorded (a group being caracterised by the material)
  // 2) the number of render vertices is counted
  //    (number of different gxmesh pairs vertex/normal)
  DBG_LOAD0("--Gathering information on:" << std::endl);
  DBG_LOAD0(" - the number of triangle groups" << std::endl);
  DBG_LOAD0(" - the sets of polygons for each group" << std::endl);
  DBG_LOAD0(" - the number of vertices--" << std::endl);
  for (int gxPolygonIndex=0;
       gxPolygonIndex<gxmesh->numPolygons();
       ++gxPolygonIndex)
  {
    gxPolygon=gxmesh->polygon(gxPolygonIndex);
    GXMat gxPolygonMat(gxPolygon.mat());

    if (! gxPolygon.hasOneColor())
    {
	  DBG_LOAD3("Polygon having index " << gxPolygonIndex << " is shaded" << std::endl);
	  DBG_LOAD3("Its texture Id is " << gxPolygonMat.textureId() << std::endl);

	  // Try to find an entry of gxPSetMap having the same texture as the shaded polygon
	  // if one is find, insert the polygon in that polygon set, if not... create a new
	  // entry corrensponding to the fisrt vertex
	  // TODO: Warning, this could yield more than one triangles_ set for a given texture
 	  bool textureNotFound=true;
	  GXMat gxPSetKey;
	  GXPolygonMap::iterator gxPSetMapIt;

      gxPSetMapIt=gxPSetMap.begin();
      while (gxPSetMapIt!=gxPSetMap.end() && textureNotFound)
	  {
		 gxPSetKey=(*gxPSetMapIt).first;
	     if (gxPSetKey.textureId()==gxPolygonMat.textureId())
		 {
		   textureNotFound=false;
		 }
		 else
		 {
		   ++gxPSetMapIt;
		 }
	  }

	  if (textureNotFound==false)
	  {
	    ASSERT((*gxPSetMapIt).first.textureId()==gxPolygonMat.textureId(), "");
		// an entry has been found with the same texture

		DBG_LOAD3("A group exists for material {" << gxPolygonMat << "}" << std::endl);
		DBG_LOAD3("the polygon index "<< gxPolygonIndex << " is registered to it" << std::endl);

	    (*gxPSetMapIt).second.push_back(gxPolygonIndex);
	  }
	  else
	  {
		// no entry has been found with the texture
		// set the material color to the color of the first vertex and create an entry
		ASSERT(gxPolygon.numVertices()>0, runtime_error("the polygon has no vertices"));

		// This also set the hasOneColor flag of gxPolygonMat to true so that the
		// created entry can be used by future one color polygons
		gxPolygonMat.diffuseColor(gxPolygon.vertex(0).color());

	    DBG_LOAD3("A new group of polygons is created for the material {" << gxPolygonMat << "}" << std::endl);
		DBG_LOAD3("the polygon index "<< gxPolygonIndex << " is registered to it" << std::endl);

		PolygonIndexes initSet(1,gxPolygonIndex);
		GXPolygonMap::value_type newVal(gxPolygonMat,initSet);
		gxPSetMap.insert(newVal);

	  }

	}
	else  // polygon has one color
	{
	  // first record polygon index in the group of
	  // polygons having the same material
	  if (gxPSetMap.find(gxPolygonMat)==gxPSetMap.end())
	  {
	    DBG_LOAD3("A new group of polygons is created for the material {" << gxPolygonMat << "}" << std::endl);
		DBG_LOAD3("the polygon index "<< gxPolygonIndex << " is registered to it" << std::endl);
	    // No entry exists for color, create a new one
		PolygonIndexes initSet(1,gxPolygonIndex);
		GXPolygonMap::value_type newVal(gxPolygonMat,initSet);
		gxPSetMap.insert(newVal);
	  }
	  else
	  {
	    // entry exists
		DBG_LOAD3("A group exists for material {" << gxPolygonMat << "}" << std::endl);
		DBG_LOAD3("the polygon index "<< gxPolygonIndex << " is registered to it" << std::endl);
		gxPSetMap[gxPolygonMat].push_back(gxPolygonIndex);
      }
	} // if (! gxPolygon.hasOneColor())

    // Second, records polygon points and normals
    // A gamut-x vertex is recorded several times in the render library
	// sweep the GX polygon vertices, record them in the vertex array
    for (int gxPolygonVertexIndex=0;
       gxPolygonVertexIndex<gxPolygon.numVertices();
       ++gxPolygonVertexIndex)
	{
	  gxVertex=gxPolygon.vertex(gxPolygonVertexIndex);
	  if (renIndexesMap.find(gxVertex)==renIndexesMap.end())
	  {
	    // the pair gx-vertex/gx-normal has not been recorded, record it
		// with a corresponding render index null : this one will be set
		// when the vertices are added to vertices_
		++renNumVertices;
		IndexesMap::value_type new_val(gxVertex,0);
		renIndexesMap.insert(new_val);
      }
	} // for (int gxPolygonVertexIndex...
  }	// for (int gxPolygonIndex=0;

  ASSERT(renNumVertices==renIndexesMap.size(),logic_error(""));

  DBG_LOAD0("AGT mesh number of vertices is " << gxmesh->numPoints() << std::endl);
  DBG_LOAD0("Render mesh number of vertices is " << renNumVertices << std::endl);
  DBG_LOAD0("Render mesh number of triangle groups is " << gxPSetMap.size() << std::endl);
  DBG_LOAD0("(respectively");
  for (GXPolygonMap::const_iterator gxPSetMapIt=gxPSetMap.begin();
       gxPSetMapIt!=gxPSetMap.end();
       ++gxPSetMapIt)
  {
	   DBG_LOAD0(", " << (*gxPSetMapIt).second.size());
  }
  DBG_LOAD0(" triangles)" << std::endl);

  // end of the first sweep of the polygons, perform allocation of data, according
  // to the information	gathered

  DBG_LOAD0("--Creation of the vertices_ structure--" << std::endl);
  vertices_ = _NEW(RenIVertexData(renNumVertices));
  // perform a sweep of the renIndexesMap and record the vertices coordinates in vertices_
  // update the correspondance table between the points indexes in gxmesh and vertices_
  DBG_LOAD3(std::endl << "Indexes correspondance table" << std::endl);
  for (IndexesMap::iterator renMapIt=renIndexesMap.begin(); renMapIt!=renIndexesMap.end(); ++renMapIt)
  {
    gxVertex=(*renMapIt).first;
	gxVertexIndex=gxVertex.pointIndex();
	gxNormalIndex=gxVertex.normalIndex();
    // flip the yz coordinates
	MexPoint3d renVertex(gxmesh->point(gxVertexIndex).x(),
		  			     gxmesh->point(gxVertexIndex).z(),
		  			     gxmesh->point(gxVertexIndex).y());
	MexVec3 renNormal(gxmesh->normal(gxNormalIndex).x(),
		  			  gxmesh->normal(gxNormalIndex).z(),
		  			  gxmesh->normal(gxNormalIndex).y());
	MexVec2 renUV(gxVertex.uv().u(),1-gxVertex.uv().v());

    renVertexIndex=_STATIC_CAST(Ren::VertexIdx, vertices_->size());

	// It may be nonsensical, but 3d Studio appears to create zero
  	// vector normals for some polygons.  The addVertex code chokes
    // on these, so filter out the zeros.
	static const MexVec3 almostZeroVec(0,0, 1.2 * MexEpsilon::instance());
	if (renNormal.isZeroVector())
				vertices_->addVertex(renVertex, almostZeroVec, renUV);
			else
                vertices_->addVertex(renVertex,renNormal, renUV);

	(*renMapIt).second=renVertexIndex;
	DBG_LOAD3("AGT mesh vertex (" << (*renMapIt).first << ")" << std::endl);
	DBG_LOAD3("Corresponding index in render mesh " << (*renMapIt).second << std::endl);
	// Set vertex color... if the polygon is shaded
	if (gxVertex.hasColor())
	{
	  DBG_LOAD0("Warning: multi-color polygon will not be inserted in triangles_, although it is handled by vertices_" << std::endl);
	  GXColor gxVertexColor(gxVertex.color());
	  RenMaterial renVertexMat;
	  renVertexMat.diffuse(RenColour((gxVertexColor.red())/255.,
    						 (gxVertexColor.green())/255.,
    						 (gxVertexColor.blue())/255.,
							 1.0));
	   if(!vertices_->materialMap())
	   {
	     vertices_->createMaterialMap();
	   }
	   RenIVertexMaterials* renVerticesMat=vertices_->materialMap();
	   renVerticesMat->insert(renVertexIndex,renVertexMat);
	}

  } // for (renMapIt=renIndexesMap.begin()...

  // sweep the groups of polygons and copy them in triangles_
  DBG_LOAD0("--Creation of the triangles_ structure--" << std::endl);
  for (GXPolygonMap::const_iterator gxPSetMapIt=gxPSetMap.begin();
       gxPSetMapIt!=gxPSetMap.end();
       ++gxPSetMapIt)
  {

    GXMat gxMat((*gxPSetMapIt).first);
    PolygonIndexes gxPolygonSet((*gxPSetMapIt).second);
    DBG_LOAD3("Creating group of polygons (" << gxPolygonSet.size() << " elements) for material	{ " << gxMat << "}" << std::endl);

    RenMaterial renMat;
    renMat.diffuse(RenColour((gxMat.diffuseColor().red())/255.,
    						 (gxMat.diffuseColor().green())/255.,
    						 (gxMat.diffuseColor().blue())/255.,
    						 (100-gxMat.transparancy())/100.));
	 // Get the texture name of the current polygon
	if (gxMat.hasValidTexture())
	{

      GXIdPos gxTxId=gxMat.textureId();
	  GXTexture gxTx=gxmesh->texture(gxTxId);
	  string txName=gxTx.name().str();
	  RenTexture renTex = RenSurfaceManager::instance().createTexture(txName);
	  renMat.texture(renTex);
	}
    RenIDistinctGroup* triangleGroup = _NEW(RenIDistinctGroup(renMat, gxPolygonSet.size()));
    for (int gxPSetIndex=0;
         gxPSetIndex<gxPolygonSet.size();
		 ++gxPSetIndex)
	{
	  // sweep polygons of the group
	  int gxPolygonIndex=gxPolygonSet[gxPSetIndex];
	  GXPolygon3 gxPolygon(gxmesh->polygon(gxPolygonIndex));
	  ASSERT(gxPolygon.numVertices()==3, runtime_error("Can only read triangles from .x files."));
	  DBG_LOAD3("Inserting polygon " << gxPolygonIndex << " {" << gxPolygon << "}" << std::endl);

	  int gxPolygonVertexIndex=0;
	  gxVertex=gxPolygon.vertex(gxPolygonVertexIndex);
	  Ren::VertexIdx renVertex1Index=renIndexesMap[gxVertex];
	  DBG_LOAD3("First GX vertex: " << gxVertex  << " corresponding render index: " << renVertex1Index <<std::endl);
	  // chexk that the coordinates of the points are the same at
	  // gxVertexIndex position in gxmesh and renVertexIndex position in vertices_
	  ASSERT_DATA(GXPoint3 DBgxVertex(gxmesh->point(gxVertex.pointIndex())));
	  ASSERT_DATA(RenIVertex DBrenVertex((*vertices_)[renVertex1Index]));
	  ASSERT(DBrenVertex.x==DBgxVertex.x(), runtime_error("Vertex coordinate mismatch between vertices_ and gxmesh"));
	  ASSERT(DBrenVertex.y==DBgxVertex.z(), runtime_error("Vertex coordinate mismatch between vertices_ and gxmesh"));
	  ASSERT(DBrenVertex.z==DBgxVertex.y(), runtime_error("Vertex coordinate mismatch between vertices_ and gxmesh"));

	  ++gxPolygonVertexIndex;
	  gxVertex=gxPolygon.vertex(gxPolygonVertexIndex);
	  Ren::VertexIdx renVertex2Index=renIndexesMap[gxVertex];
	  DBG_LOAD3("Second GX vertex: " << gxVertex  << " corresponding render index: " << renVertex2Index <<std::endl);
	  ASSERT_DATA(GXPoint3 DB2gxVertex(gxmesh->point(gxVertex.pointIndex())));
	  ASSERT_DATA(RenIVertex DB2renVertex((*vertices_)[renVertex2Index]));
	  ASSERT(DB2renVertex.x==DB2gxVertex.x(), runtime_error("Vertex coordinate mismatch between vertices_ and gxmesh"));
	  ASSERT(DB2renVertex.y==DB2gxVertex.z(), runtime_error("Vertex coordinate mismatch between vertices_ and gxmesh"));
	  ASSERT(DB2renVertex.z==DB2gxVertex.y(), runtime_error("Vertex coordinate mismatch between vertices_ and gxmesh"));

	  ++gxPolygonVertexIndex;
	  gxVertex=gxPolygon.vertex(gxPolygonVertexIndex);
	  Ren::VertexIdx renVertex3Index=renIndexesMap[gxVertex];
	  DBG_LOAD3("Third GX vertex: " << gxVertex  << " corresponding render index: " << renVertex3Index <<std::endl);
	  ASSERT_DATA(GXPoint3 DB3gxVertex(gxmesh->point(gxVertex.pointIndex())));
	  ASSERT_DATA(RenIVertex DB3renVertex((*vertices_)[renVertex3Index]));
	  ASSERT(DB3renVertex.x==DB3gxVertex.x(), runtime_error("Vertex coordinate mismatch between vertices_ and gxmesh"));
	  ASSERT(DB3renVertex.y==DB3gxVertex.z(), runtime_error("Vertex coordinate mismatch between vertices_ and gxmesh"));
	  ASSERT(DB3renVertex.z==DB3gxVertex.y(), runtime_error("Vertex coordinate mismatch between vertices_ and gxmesh"));

	  // also flip the second and third vertex of the polygon
	  triangleGroup->addTriangle((Ren::VertexIdx) renVertex1Index,
	                             (Ren::VertexIdx) renVertex3Index,
	                             (Ren::VertexIdx) renVertex2Index);
      DBG_LOAD3("End insertion polygon" << std::endl);


	}
	// add group of triangles to triangles_
    DBG_LOAD3("Insertion of group of polygons in triangles_" << std::endl);
	triangles_.push_back(triangleGroup);
    DBG_LOAD3("End creation group of polygons" << std::endl);
  }	// for (gxPSetMapIt=gxPSetMap.begin()...

  checkMaxVertices(vertices_, maxVertices_);

  DBG_LOAD0("Conversion succeded" << std::endl);
  return true;
}

// static
void RenMesh::emptyCache()
{
	RenID3DMeshLoader::instance().deleteAll();
}

// static
void RenMesh::removeFromFactory(Ren::ConstMeshPtr mesh)
{
	RenIMeshFactory::instance().removeFromFactory(mesh);
}

// static
void RenMesh::removeAllFromFactory()
{
	RenIMeshFactory::instance().removeAllFromFactory();
}

// static
ulong RenMesh::totalMeshCount()
{
	// This pre-condition should prevent initialisation order bugs while
	// accessing the static member variable.
	PRE(Ren::initialised());
	return meshCount_;
}

// static
ulong RenMesh::maxVertices()
{
	// This pre-condition should prevent initialisation order bugs while
	// accessing the static member variable.
	PRE(Ren::initialised());
	return maxVertices_;
}

void RenMesh::backFaceAll(bool b)
{
	ctl_min_memory_vector<RenITriangleGroup*>::iterator dIt = triangles_.begin();
	while (dIt != triangles_.end())
	{
		RenITriangleGroup* g = *dIt;

		if (g)
			g->backFace(b);

		++dIt;
	}
}

void RenMesh::backFace(const RenMaterial& mat, bool b)
{
	ctl_min_memory_vector<RenITriangleGroup*>::iterator dIt = triangles_.begin();
	while (dIt != triangles_.end())
	{
		RenITriangleGroup* g = *dIt;

		if (g && g->material() == mat)
		{
			g->backFace(b);
			return;
		}

		++dIt;
	}
}

bool RenMesh::backFace(const RenMaterial& mat) const
{
	ctl_min_memory_vector<RenITriangleGroup*>::const_iterator dIt = triangles_.begin();
	while (dIt != triangles_.end())
	{
		const RenITriangleGroup* g = *dIt;

		if (g && g->material() == mat)
			return g->backFace();

		++dIt;
	}

	return false;
}

size_t RenMesh::faces( size_t* nVertices,
                     ctl_vector<MexPoint3d>* vertices, ctl_vector<size_t>* faceData ) const
{
    PRE( nVertices != NULL );
    PRE( vertices != NULL );
    PRE( faceData != NULL );

    size_t  nFaces = 0;
    size_t  vertexOffset = vertices->size();

	if (!vertices_)
	{
		*nVertices = 0;
	}
	else
	{
	    for( RenIVertexData::iterator i = (*vertices_).begin(); i != (*vertices_).end(); ++i )
	        vertices->push_back( MexPoint3d( (*i).x, (*i).y, (*i).z ) );

	    *nVertices = (*vertices_).size();
	}

	for( ctl_min_memory_vector<RenITriangleGroup*>::const_iterator i = triangles_.begin();
      i != triangles_.end(); ++i )
    {
        size_t nPolygons = (*i)->nTriangles();
		nFaces += nPolygons;

        for( Ren::TriangleIdx j = 0; j < nPolygons; ++j )
        {
            const size_t nVerticesInPoly = 3;
            faceData->push_back( nVerticesInPoly );

			Ren::VertexIdx v1, v2, v3;
			(*i)->triangle(j, &v1, &v2, &v3);
            faceData->push_back(v1);
            faceData->push_back(v2);
            faceData->push_back(v3);
        }
    }

    return nFaces;
}

void RenMesh::meshName(const string& n)
{
	meshName_ = n;
}

void perWrite( PerOstream& ostr, const RenMesh& mesh )
{
    ostr << mesh.pathName_;
    ostr << mesh.meshName_;
    ostr << mesh.vertices_;
    ostr << mesh.uvAnimated_;
    ostr << mesh.boundingVolume_;
    ostr << mesh.isDirty_;
    ostr << mesh.triangles_;
    ostr << mesh.ttfs_;
    ostr << mesh.stfps_;
    ostr << mesh.lines_;
    ostr << mesh.pVertexTexture_;
}

void perRead( PerIstream& istr, RenMesh& mesh )
{
    istr >> mesh.pathName_;
    istr >> mesh.meshName_;
    istr >> mesh.vertices_;
    istr >> mesh.uvAnimated_;
    istr >> mesh.boundingVolume_;
    istr >> mesh.isDirty_;
    istr >> mesh.triangles_;
    istr >> mesh.ttfs_;
    istr >> mesh.stfps_;
    istr >> mesh.lines_;
	istr >> mesh.pVertexTexture_;

	checkMaxVertices(mesh.vertices_, RenMesh::maxVertices_);
}

void RenMesh::CLASS_INVARIANT
{
    INVARIANT(this);
}

ostream& operator <<( ostream& o, const Ren::ConstMeshPtr& p )
{
    o << "Start RenMesh " << p->meshName_ << std::endl;

    UtlIndentOstream    ostr( o, "  " );

	ostr << "Pathname " << p->pathName_ << std::endl;
    if( p->vertices_ )
    {
        ostr << "Vertices:" << std::endl;
        ostr << *(p->vertices_);
    }
    else
    {
        ostr << "No vertices defined" << std::endl;
    }

    if( p->uvAnimated_ )
    {
        ostr << "UV animated vertices:" << std::endl;
        ostr << *(p->uvAnimated_);
    }
    else
    {
        ostr << "No UV animated vertices defined" << std::endl;
    }

    ostr << "Triangle groups:" << std::endl;

    for( size_t i = 0; i < p->triangles_.size(); ++i )
    {
        RenITriangleGroup* pGroup = p->triangles_[ i ];
        if( pGroup )
            ostr << *pGroup << std::endl;
        else
            ostr << "NULL" << std::endl;
    }

    ostr << "TTF polygons:" << std::endl;
    for( size_t i = 0; i < p->ttfs_.size(); ++i )
    {
        RenTTFPolygon* pGroup = p->ttfs_[ i ];
        if( pGroup )
            ostr << *pGroup;
        else
            ostr << "NULL" << std::endl;
    }

    ostr << "Spin to face polygons:" << std::endl;
    for( size_t i = 0; i < p->stfps_.size(); ++i )
    {
        RenSpinTFPolygon* pGroup = p->stfps_[ i ];
        if( pGroup )
            ostr << *pGroup;
        else
            ostr << "NULL" << std::endl;
    }

    ostr << "Line groups:" << std::endl;
    for( size_t i = 0; i < p->lines_.size(); ++i )
    {
        RenILineGroup* pGroup = p->lines_[ i ];
        if( pGroup )
            ostr << *pGroup;
        else
            ostr << "NULL" << std::endl;
    }

    o << "Finish RenMesh " << p->meshName_ << std::endl;

    return o;
}

void RenMesh::createTextures()
{
	pVertexTexture_ = _NEW( Textures );
	const size_t nVertces = nVertices();
	pVertexTexture_->reserve( nVertces );

	RenTexture empTexture = RenTexture();

	for( size_t i = 0; i< nVertces; ++i )
		pVertexTexture_->push_back( empTexture );

	size_t nTriangleGroups = triangles_.size();
	for( size_t i = 0; i< nTriangleGroups; ++i )
	{
		RenITriangleGroup* pTriangles = triangles_[i];
		RenTexture groupTexture = pTriangles->material().texture();

		size_t nTriangles = pTriangles->nTriangles();

		for( size_t j=0; j<nTriangles; ++j)
		{
			Ren::VertexIdx idx1, idx2, idx3;
			Ren::TriangleIdx triangleIdx = (Ren::TriangleIdx) j;
			pTriangles->triangle( triangleIdx, &idx1, &idx2, &idx3 );

			size_t v1 = (size_t)  idx1;
			size_t v2 = (size_t)  idx2;
			size_t v3 = (size_t)  idx3;

			(*pVertexTexture_)[v1] = groupTexture;
			(*pVertexTexture_)[v2] = groupTexture;
			(*pVertexTexture_)[v3] = groupTexture;
		}
	}
}

// static
void RenMesh::startFrame()
{
	privateMeshId() = 0;
}

// static
ushort RenMesh::nextId()
{
	ASSERT(privateMeshId() != USHRT_MAX, "You can only draw USHRT_MAX meshes per frame.");

	// Sorted items have a default meshId of zero.  Any we label as special
	// cases must have a different, i.e. non-zero id.  Hence the POST.
	const ushort result = ++privateMeshId();
	POST(result != 0);
	return result;
}

// static
ushort RenMesh::meshId()
{
	return privateMeshId();
}

ushort& RenMesh::privateMeshId()
{
	static ushort i = 0;
	return i;
}

/* End MESH.CPP *****************************************************/

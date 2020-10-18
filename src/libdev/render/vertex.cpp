/*
 * V E R T E X . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/vertex.hpp"
#include "render/mesh.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/intmap.hpp"
#include "render/internal/vtxmat.hpp"

#ifndef NDEBUG
#include "render/texture.hpp"		// only required for a pre-condition
#endif

RenVertex::RenVertex(RenMesh* m, Ren::VertexIdx i):
	mesh_(m),
	index_(i)
{
	PRE(m);
	PRE(m->vertices());
    TEST_INVARIANT;
}

RenVertex::~RenVertex()
{
    TEST_INVARIANT;
}

RenIVertex& RenVertex::vertexRep(RenMesh* m, Ren::VertexIdx i) const
{
	return (*(m->vertices()))[i];
}

const MexPoint3d RenVertex::position() const
{
    TEST_INVARIANT;
	const RenIVertex& v = vertexRep(mesh_, index_);
	return MexPoint3d(v.x, v.y, v.z);
}

void RenVertex::position(const MexPoint3d& p)
{
    TEST_INVARIANT;
	RenIVertex& v = vertexRep(mesh_, index_);
	v.x = p.x();
	v.y = p.y();
	v.z = p.z();
}

const MexVec3 RenVertex::normal() const
{
    TEST_INVARIANT;
	const RenIVertex& v = vertexRep(mesh_, index_);

	// Convert the internal, fixed-point rep to floating point mathex.
	MexVec3 result;
	mesh_->vertices()->normal(v).convertToMex(&result);
	return result;
}

void RenVertex::normal(const MexVec3& n)
{
    TEST_INVARIANT;

	const RenIVertex& v = vertexRep(mesh_, index_);
	RenIVec3FixPtS0_7& norm = mesh_->vertices()->normal(v);

	norm.setVector(n);
}

MATHEX_SCALAR RenVertex::intensity() const
{
	const RenIVertexIntensities* ints = mesh_->vertices()->intensityMap();
	if (!ints)
		return 1;
	else
		return ints->value(index_);
}

void RenVertex::intensity(MATHEX_SCALAR i)
{
	RenIVertexIntensities* ints = mesh_->vertices()->intensityMap();
	if (!ints)
	{
		mesh_->vertices()->createIntensityMap();
		ints = mesh_->vertices()->intensityMap();
	}

	ASSERT(ints, "No vertex intensity map: can't change intensity.");
	ints->value(index_, i);
}

bool RenVertex::hasMaterial() const
{
	RenIVertexMaterials* mats = mesh_->vertices()->materialMap();
	return mats && mats->contains(index_);
}

const RenMaterial& RenVertex::material() const
{
	PRE(hasMaterial());
	RenIVertexMaterials* mats = mesh_->vertices()->materialMap();
	ASSERT(mats, "No material map for mesh\n");

	return mats->find(index_);
}

void RenVertex::material(const RenMaterial& m)
{
	PRE(m.texture().isNull());

	RenIVertexMaterials* mats = mesh_->vertices()->materialMap();
	if (!mats)
	{
		mesh_->vertices()->createMaterialMap();
		mats = mesh_->vertices()->materialMap();
	}

	ASSERT(mats, "No vertex material map: can't change material.");
	mats->insert(index_, m);

	POST(hasMaterial());
	POST(material() == m);
}

void RenVertex::CLASS_INVARIANT
{
	INVARIANT(this);
	INVARIANT(mesh_->vertices());
	INVARIANT(mesh_->vertices()->size() > index_);
}

ostream& operator <<( ostream& o, const RenVertex& t )
{
	o << "RenVertex #" << t.index_ << " in " << t.mesh_->meshName() << " @ " << t.position();
    return o;
}

/* End VERTEX.CPP ***************************************************/

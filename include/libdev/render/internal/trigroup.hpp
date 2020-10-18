/*
 * T R I G R O U P . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_TRIGROUP_HPP
#define _RENDER_TRIGROUP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/mmvector.hpp"
#include "render/render.hpp"
#include "render/material.hpp"
#include "render/internal/internal.hpp"

class RenIVertexData;

//////////////////////////////////////////////////////////////////////////
// A list items which share the same RenMaterial.
class RenIMaterialGroup
{
public:
	// PRE(mat != NULL);
	RenIMaterialGroup(const RenMaterial& m): material_(m)	{}
	virtual ~RenIMaterialGroup()							{}

	// The first render method applies lighting to the given vertices.
	// The second version takes a list of pre-lit vertices.
	virtual void render(const RenIVertexData&,   const RenMaterial&) const = 0;
	virtual void render(const RenI::LitVtxAPtr&, const RenMaterial&) const = 0;
	virtual RenI::LitVtxAPtr light(const RenIVertexData&, const RenMaterial&) const = 0;
	virtual size_t nTriangles() const = 0;
	virtual size_t nLines    () const = 0;

	// This vertex's co-ordinates are used as the basis for alpha sorting.
	virtual Ren::VertexIdx firstVertexIdx() const = 0;

	// This is the maximum vertex index used by this group.  Knowing this makes
	// some optimisations possible, e.g. only lighting vertices [0,max).
	virtual Ren::VertexIdx maxUsedIndices() const = 0;

	const RenMaterial& material() const;
	void material(const RenMaterial&);

	bool operator<(const RenIMaterialGroup&) const;

    PER_MEMBER_PERSISTENT_ABSTRACT( RenIMaterialGroup );
    PER_FRIEND_READ_WRITE( RenIMaterialGroup );

    virtual void    write( ostream& ) const = 0;

private:
	RenMaterial material_;
};

PER_DECLARE_PERSISTENT( RenIMaterialGroup );
ostream& operator <<( ostream&, const RenIMaterialGroup& );

//////////////////////////////////////////////////////////////////////////
// A list of triangles which share the same RenMaterial.
class RenITriangleGroup : public RenIMaterialGroup
{
public:
	// PRE(mat != NULL);
	RenITriangleGroup(const RenMaterial& m): RenIMaterialGroup(m), backFace_(true)	{}
	virtual ~RenITriangleGroup()							{}
	virtual size_t nLines() const							{ return 0; }
	virtual void triangle(Ren::TriangleIdx, Ren::VertexIdx*, Ren::VertexIdx*, Ren::VertexIdx*) const = 0;

	bool backFace() const		{ return backFace_; }
	void backFace(bool b)		{ backFace_ = b; }

    virtual void    write( ostream& ) const;

    PER_MEMBER_PERSISTENT_ABSTRACT( RenITriangleGroup );
    PER_FRIEND_READ_WRITE( RenITriangleGroup );

private:
	bool backFace_;
};

PER_DECLARE_PERSISTENT( RenITriangleGroup );
ostream& operator <<( ostream&, const RenITriangleGroup& );

//////////////////////////////////////////////////////////////////////////
// A RenITriangleGroup in which every triangle specifies three separate vertices.
class RenIDistinctGroup : public RenITriangleGroup
{
public:
	// TBD: determine a good default size.
	RenIDistinctGroup(const RenMaterial& mat, size_t nFaces=40);
	virtual ~RenIDistinctGroup();

	virtual void render(const RenIVertexData&,   const RenMaterial&) const;
	virtual void render(const RenI::LitVtxAPtr&, const RenMaterial&) const;
	virtual RenI::LitVtxAPtr light(const RenIVertexData&, const RenMaterial&) const;
	virtual size_t nTriangles() const 	{ return indices_.size() / 3; }
	virtual Ren::VertexIdx firstVertexIdx() const;
	virtual Ren::VertexIdx maxUsedIndices() const;
	virtual void triangle(Ren::TriangleIdx, Ren::VertexIdx*, Ren::VertexIdx*, Ren::VertexIdx*) const;

	// TBD: add methods for removing polys.
	void addTriangle(Ren::VertexIdx i1, Ren::VertexIdx i2, Ren::VertexIdx i3);

	typedef	ctl_min_memory_vector<Ren::VertexIdx> IndexVec;

    virtual void    write( ostream& ) const;

    PER_MEMBER_PERSISTENT_VIRTUAL( RenIDistinctGroup );
    PER_FRIEND_READ_WRITE( RenIDistinctGroup );

private:
	IndexVec 		indices_;
	Ren::VertexIdx  nIndicesUsed_;
};

PER_DECLARE_PERSISTENT( RenIDistinctGroup );
ostream& operator <<( ostream&, const RenIDistinctGroup& );

//////////////////////////////////////////////////////////////////////////
// A list of lines which share the same RenMaterial.
class RenILineGroup : public RenIMaterialGroup
{
public:
	// PRE(mat != NULL);
	RenILineGroup(const RenMaterial& m);
	virtual ~RenILineGroup()									{}

	virtual void render(const RenIVertexData&,   const RenMaterial&) const;
	virtual void render(const RenI::LitVtxAPtr&, const RenMaterial&) const;
	virtual RenI::LitVtxAPtr light(const RenIVertexData&, const RenMaterial&) const;
	virtual Ren::VertexIdx firstVertexIdx() const;
	virtual Ren::VertexIdx maxUsedIndices() const;
	virtual size_t nLines    () const		{ return indices_.size() / 2; }
	virtual size_t nTriangles() const		{ return 0; }

	// TBD: add methods for removing.
	void addLine(Ren::VertexIdx i1, Ren::VertexIdx i2);

	typedef	ctl_min_memory_vector<Ren::VertexIdx> IndexVec;

    virtual void    write( ostream& ) const;
    PER_MEMBER_PERSISTENT_VIRTUAL( RenILineGroup );
    PER_FRIEND_READ_WRITE( RenILineGroup );

private:
	IndexVec 		indices_;
	Ren::VertexIdx  nIndicesUsed_;
};

PER_DECLARE_PERSISTENT( RenILineGroup );
ostream& operator <<( ostream&, const RenILineGroup& );

//////////////////////////////////////////////////////////////////////////
/* These classes might be useful optimisations.
class StripGroup : public RenITriangleGroup
{
public:
	virtual void render(const RenIVertexData&, const RenMaterial&) const	{}
	virtual RenI::LitVtxAPtr light(const RenIVertexData&, const RenMaterial&) const	{ return NULL; }
	virtual size_t nTriangles() const 					{ return 0; }
	virtual Ren::VertexIdx firstVertexIdx() const		{ return 0; }
	virtual Ren::VertexIdx maxUsedIndices() const		{ return 0; }
	virtual void triangle(Ren::TriangleIdx, Ren::VertexIdx*, Ren::VertexIdx*, Ren::VertexIdx*) const	{}
};

class FanGroup : public RenITriangleGroup
{
public:
	virtual void render(const RenIVertexData&, const RenMaterial&) const	{}
	virtual RenI::LitVtxAPtr light(const RenIVertexData&, const RenMaterial&) const	{ return NULL; }
	virtual size_t nTriangles() const 					{ return 0; }
	virtual Ren::VertexIdx firstVertexIdx() const		{ return 0; }
	virtual Ren::VertexIdx maxUsedIndices() const		{ return 0; }
	virtual void triangle(Ren::TriangleIdx, Ren::VertexIdx*, Ren::VertexIdx*, Ren::VertexIdx*) const	{}
};
*/

#ifdef _INLINE
    #include "internal/trigroup.ipp"
#endif

#endif

/* End TRIGROUP.HPP *************************************************/

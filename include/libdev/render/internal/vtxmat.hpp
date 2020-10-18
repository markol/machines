/*
 * V T X M A T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_VTXMAT_HPP
#define _RENDER_VTXMAT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "render/render.hpp"
#include "render/material.hpp"
#include "ctl/vector.hpp"

class RenIVtxMaterial
{
public:
	Ren::VertexIdx		index() const 		{ return index_; }
	const RenMaterial&	material() const	{ return mat_; }

	// For the purposes of running find on the intensity map, this is useful.
	bool operator==(const RenIVtxMaterial& r) const	{ return index_ == r.index_; }

    PER_MEMBER_PERSISTENT_DEFAULT( RenIVtxMaterial );
    PER_FRIEND_READ_WRITE( RenIVtxMaterial );

private:
	friend class RenIVertexMaterials;

	Ren::VertexIdx	index_;
	RenMaterial		mat_;
};

PER_DECLARE_PERSISTENT( RenIVtxMaterial );

// Provide a mapping from meshed vertex index to vertex material.
// We require traversal which is as efficient as possible, but we don't
// particularly require efficient search, so an STL map is *not* used.
//class RenIVertexMaterials : private ctl_vector<RenIVtxMaterial>
class RenIVertexMaterials : public ctl_vector<RenIVtxMaterial>
{
public:
	typedef ctl_vector<RenIVtxMaterial> Base;

    RenIVertexMaterials(size_t nElements);
    RenIVertexMaterials(const RenIVertexMaterials& copyMe);
    ~RenIVertexMaterials();

	// Does this object contain an entry for the given index?
	bool contains(Ren::VertexIdx i) const;

	const RenMaterial& find(Ren::VertexIdx i) const;

	void insert(Ren::VertexIdx i, RenMaterial newValue);	// POST(contains(i));
	void erase(Ren::VertexIdx i);							// POST(!contains(i));

	ctl_vector<RenIVtxMaterial>::begin;
	ctl_vector<RenIVtxMaterial>::end;

    PER_MEMBER_PERSISTENT( RenIVertexMaterials );
    PER_FRIEND_READ_WRITE( RenIVertexMaterials );

private:
	ctl_vector<RenIVtxMaterial>::iterator findIter(Ren::VertexIdx i);
	ctl_vector<RenIVtxMaterial>::const_iterator findIter(Ren::VertexIdx i) const;

    // Operation deliberately revoked
    //RenIVertexMaterials( const RenIVertexMaterials& );
    RenIVertexMaterials& operator =( const RenIVertexMaterials& );
    bool operator ==( const RenIVertexMaterials& );
};

PER_DECLARE_PERSISTENT( RenIVertexMaterials );

#endif

/* End VTXMAT.HPP ***************************************************/

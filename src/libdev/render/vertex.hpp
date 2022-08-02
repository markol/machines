/*
 * V E R T E X . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_VERTEX_HPP
#define _RENDER_VERTEX_HPP

#include "base/base.hpp"
#include "ctl/countptr.hpp"
#include "mathex/mathex.hpp"
#include "render/render.hpp"

class MexPoint3d;
class MexVec3;
class RenMesh;
class RenMaterial;
class RenIVertex;

// Read this as RenMeshedVertex -- it behaves differently because
// its in a mesh and shared amoung polys.  Clients cannot arbitrarily 
// create vertices -- they must use RenMesh::addVertex.  The actual
// data is owned by RenMesh, this is a sort of proxy class.
class RenVertex
{
public:
    ~RenVertex();
    RenVertex( const RenVertex& );
    RenVertex& operator =( const RenVertex& );
	
	// Semantics: this will return false for two separate objects which
	// have identical attributes.
    bool operator ==( const RenVertex& );

	const MexPoint3d position() const;
	void position(const MexPoint3d&);
	
	// TBD: when RenMesh's rep is changed to MexVec3, this should return a ref.
	const MexVec3 normal() const;
	void normal(const MexVec3&);
	
	MATHEX_SCALAR intensity() const;
	void intensity(MATHEX_SCALAR);
	
	// You can't apply a material with a texture to a vertex.
	// PRE(m.texture().isNull()); POST(hasMaterial()); POST(material() == m);
	void material(const RenMaterial& m);
	bool hasMaterial() const;
	const RenMaterial& material() const;	// PRE(hasMaterial());

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const RenVertex& t );

private:
	// Only meshes can create vertices.
	friend class RenMesh;
	RenVertex(RenMesh*, Ren::VertexIdx);
	
	// T'would be nice to use a RenMesh::Ptr here, but unfortunately RenMesh
	// can't provide a counted pointer to itself (due to double delete issues).
	RenMesh*				mesh_;
	const Ren::VertexIdx	index_;

	RenIVertex& vertexRep(RenMesh*, Ren::VertexIdx) const;
};

#endif

/* End VERTEX.HPP ***************************************************/

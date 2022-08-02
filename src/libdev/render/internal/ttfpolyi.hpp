/*
 * T T F P O L Y I . H P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

#ifndef _RENDER_TTFPOLYI_HPP
#define _RENDER_TTFPOLYI_HPP

#include "base/base.hpp"
#include "render/render.hpp"
#include "render/material.hpp"
#include <GL/glew.h>

class MexVec2;
class RenIVertex;

// A dumb pimple which hides the Direct3D specifics from the public interface.
class RenITTFImpl
{
public:
	RenITTFImpl(size_t nVertices, const Ren::VertexIdx*, size_t nIndices);
	RenITTFImpl(const RenITTFImpl&);
	~RenITTFImpl();

    void renderGL(RenIVertex*, const RenMaterial&, GLenum mode) const;

	RenIVertex*			    vertices() const	{ return vertices_; }
	size_t					nVertices() const	{ return nVertices_; }
	const RenMaterial&		material() const	{ return material_; }
	const Ren::VertexIdx*	indices() const		{ return indices_; }
	size_t 					nIndices() const	{ return nIndices_; }
	MexVec2*				uvs() const			{ return uvs_; }

	void material(const RenMaterial& m)			{ material_ = m; }

private:
	RenIVertex*			    vertices_;
	size_t					nVertices_;
	RenMaterial 			material_;
	const Ren::VertexIdx*	indices_;
	size_t 					nIndices_;
	MexVec2*				uvs_;

    RenITTFImpl& operator =( const RenITTFImpl& );
};

#ifdef _INLINE
    #include "render/internal/ttfpolyi.ipp"
#endif

#endif

/* End TTFPOLYI.HPP *************************************************/

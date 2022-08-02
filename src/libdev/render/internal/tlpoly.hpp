/*
 * T L P O L Y . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_TLPOLY_HPP
#define _RENDER_TLPOLY_HPP

#include "base/base.hpp"
#include "render/internal/polysord.hpp"

class RenITTFImpl;

// A transformed and lit polygon.  Used for post-sorting turn-to-face polygons.
class RenITLPolygon : public RenIDepthSortedItem
{
public:
	RenITLPolygon(const RenITTFImpl*, const RenMaterial& mat, MATHEX_SCALAR w, GLenum);

	virtual ~RenITLPolygon();
	virtual void render();
	virtual void print(ostream&) const;

private:
	const RenITTFImpl* const	pImpl_;
    RenIVertex*				    vertices_;
    GLenum	            		geomType_;
};

#endif

/* End TLPOLY.HPP ***************************************************/

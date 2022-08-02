/*
 * V P M A P . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_VPMAP_HPP
#define _RENDER_VPMAP_HPP

#include "base/base.hpp"
#include "render/internal/vtxdata.hpp"

//////////////////////////////////////////////////////////////////////////
// Implements the mapping from the clipping volume to screenspace.
// This is defined by a D3DVIEWPORT structure but the calculation
// is not made available by Direct3D.
class RenIViewportMapping
{
public:
	RenIViewportMapping(const int width, const int height, const int left = 0, const int top = 0);
	void mapPoint(RenIVertex& vtx) const; // Not used now in fact
	int screenLeft  () const;
	int screenTop   () const;
	int screenWidth () const;
	int screenHeight() const;

private:
	const float scaleX, scaleY, offsetX, offsetY;
	const int      left_, top_, width_, height_;
};

#ifdef _INLINE
    #include "render/internal/vpmap.ipp"
#endif

#endif

/* End VPMAP.HPP ****************************************************/

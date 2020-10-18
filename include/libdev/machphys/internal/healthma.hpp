/*
 * H E A L T H M A . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

#ifndef _MACHPHYS_HEALTHMA_HPP
#define _MACHPHYS_HEALTHMA_HPP

#include "base/base.hpp"
#include "render/texture.hpp"
#include "render/material.hpp"

// Map from percentage hit points to a material whose colour indicates a
// Machine's health.  Green is healthy; red means just about to die.  In
// between we have interpolated oranges and yellows.  The materials are
// allocated once and are non-shared so that we avoid the overhead of
// searching the material database every time we require a marker.
class MachPhysIHealthMaterials
{
public:
	MachPhysIHealthMaterials(int nMats, double alpha=1);
	MachPhysIHealthMaterials(int nMats, const RenTexture&, double alpha=1);
	~MachPhysIHealthMaterials();

	// These post-conditions apply to both of these functions.
	// POST(result.texture() == texture());
	// POST(result.diffuse().a() == alpha());
	const RenMaterial& material(double percentageHp) const;
	const RenMaterial& blueMaterial() const;

	// Change the alpha or texture of all current materials.  The materials 
	// are shared amoung markers, so this will affect them all.	
	// void alpha(double);
	// void texture(const RenTexture&);
	
	double alpha() const				{ return alpha_; }
	const RenTexture& texture() const	{ return texture_; }
	
private:
	RenColour computeColour(double percentageHp) const;
	void sharedCtor();

	RenMaterial* const	materials_;
	const int			nMats_;
	RenMaterial			blue_;
	const double		alpha_;
	const RenTexture	texture_;
};

#endif

/* End HEALTHMA.HPP *************************************************/

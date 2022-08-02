/*
 * L I T X F O R M . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

#ifndef _RENDER_LITXFORM_HPP
#define _RENDER_LITXFORM_HPP

#include "render/render.hpp"

class RenColour;

// A class which takes a an input colour and produces an output colour as
// some function of the input one.
class RenLightColourTransform
{
public:
	// PRE(out);
	virtual void transform(const RenColour& in, Ren::LightType, RenColour* out) const =0;
	virtual ~RenLightColourTransform()	{}
};

#endif

/* End LITXFORM.HPP *************************************************/

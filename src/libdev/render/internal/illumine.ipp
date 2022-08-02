/*
 * I L L U M I N E . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include "render/material.hpp"

_CODE_INLINE
const RenColour& RenIIlluminator::ambient() const
{
	return ambient_;
}

_CODE_INLINE
void RenIIlluminator::ambient(const RenColour& a)
{
	ambient_ = a;
}

_CODE_INLINE
const RenColour& RenIIlluminator::filter() const
{
	return filter_;
}

_CODE_INLINE
void RenIIlluminator::filter(const RenColour& f)
{
	filter_ = f;
}

_CODE_INLINE
void RenIIlluminator::disableLighting()
{
	disabled_ = true;
	internallyDisabled_= true;
}

_CODE_INLINE
void RenIIlluminator::enableLighting()
{
	disabled_ = false;
	internallyDisabled_= lightsOn_.size() == 0;
}

_CODE_INLINE
bool RenIIlluminator::lightingEnabled() const
{
	// Note this returns the flag which tracks the external disable calls made 
	// by clients.  The internal version is not returned.
	return !disabled_;
}	

_CODE_INLINE
const RenIMatBody* RenIIlluminator::matBody(const RenMaterial& m)
{
	return m.body_;
}

/* End MATERIAL.IPP **************************************************/

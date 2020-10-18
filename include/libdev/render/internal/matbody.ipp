/*
 * M A T B O D Y . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include <limits.h>

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include "render/texture.hpp"
#include "render/internal/matmgr.hpp"

_CODE_INLINE
void RenIMatBody::incRefCount()
{
	PRE(refCount_ < USHRT_MAX);
	++refCount_;
	DBG_MAT_SHARE( Diag::instance().renderStream() << "Incremented count to " << refCount_ << " for " << *this << endl);
}

_CODE_INLINE
void RenIMatBody::decRefCount()
{
	PRE(refCount_ > 0);

	--refCount_;
	DBG_MAT_SHARE(Diag::instance().renderStream() << "Decremented count to " << refCount_ << " for " << *this << endl);

	if (refCount_ == 0)
		RenIMatManager::instance().killMe(this);
}

_CODE_INLINE
ushort RenIMatBody::refCount() const
{
	return refCount_;
}

_CODE_INLINE
bool RenIMatBody::hasFlatTransparency() const
{
	if (dirty_)
		_CONST_CAST(RenIMatBody*, this)->updateTransparent();

	return transparent_;
}

_CODE_INLINE
bool RenIMatBody::hasAlphaTransparency() const
{
	if (dirty_)
		_CONST_CAST(RenIMatBody*, this)->updateTransparent();

	return alphaTransparent_ || texture_.hasAlphaTransparency();
}

_CODE_INLINE
bool RenIMatBody::usesBilinear() const
{
	return usesBilinear_;
}

_CODE_INLINE
void RenIMatBody::texture(const RenTexture& t)
{
	texture_ = t;

	// NB: the values true and false are used so that we can do equality tests
	// without having to worry about true values which are > 1.
	usesBilinear_ = (texture_.usesBilinear())? true: false;
	dirty_   = true;
}

_CODE_INLINE
void RenIMatBody::diffuse(const RenColour& c)
{
	diffuse_ = c;
	dirty_   = true;
}

_CODE_INLINE
void RenIMatBody::specular(const RenColour&)
{
	ASSERT(false, "Specular material attributes not supported.");
}

_CODE_INLINE
void RenIMatBody::ambient(const RenColour&)
{
	ASSERT(false, "Ambient material attributes not supported.");
}

_CODE_INLINE
void RenIMatBody::emissive(const RenColour& c)
{
	emissive_ = c;

	if( emissive_!=RenColour::black() )
	{
		if( not hasFogMultiplier() )
 		{
			fogMultiplier_=1.3;
			hasFogMultiplier_=true;
		}
	}
	else
	{
		fogMultiplier_=1.0;
		hasFogMultiplier_=false;
	}
	dirty_    = true;
}

_CODE_INLINE
void RenIMatBody::shininess(double)
{
	ASSERT(false, "Specular shininess material attributes not supported.");
}

_CODE_INLINE
const RenTexture& RenIMatBody::texture() const
{
	return texture_;
}

_CODE_INLINE
const RenColour& RenIMatBody::diffuse() const
{
	return diffuse_;
}

_CODE_INLINE
const RenColour& RenIMatBody::specular() const
{
	ASSERT(false, "Specular material attributes not supported.");
	static const RenColour dummy = RenColour::black();
	return dummy;
}

_CODE_INLINE
const RenColour& RenIMatBody::ambient() const
{
	return diffuse_;
}

_CODE_INLINE
const RenColour& RenIMatBody::emissive() const
{
	return emissive_;
}

_CODE_INLINE
double RenIMatBody::shininess() const
{
	ASSERT(false, "Specular shininess material attributes not supported.");
	return 0;
}

_CODE_INLINE
void RenIMatBody::coplanarPriority(short p)
{
	// Looks like unary negation causes an integral promotion.
	depth_	  = _STATIC_CAST(short, -p);
	dirty_    = true;
}

_CODE_INLINE
short RenIMatBody::coplanarPriority() const
{
	// Looks like unary negation causes an integral promotion.
	return _STATIC_CAST(short, -depth_);
}

_CODE_INLINE
short RenIMatBody::coplanarDepth() const
{
	return depth_;
}

_CODE_INLINE
void RenIMatBody::alphaPriority(short p)
{
	alphaPriority_ = p;
	dirty_ = true;
}

_CODE_INLINE
short RenIMatBody::alphaPriority() const
{
	return alphaPriority_;
}

_CODE_INLINE
void RenIMatBody::interMeshCoplanar(bool b)
{
	interMeshCoplanar_ = b;
	dirty_ = true;
}

_CODE_INLINE
bool RenIMatBody::interMeshCoplanar() const
{
	return interMeshCoplanar_;
}

_CODE_INLINE
bool RenIMatBody::sharable() const
{
	return sharable_;
}

_CODE_INLINE
void RenIMatBody::makeNonSharable()
{
	sharable_ = false;
}

_CODE_INLINE
bool RenIMatBody::absoluteAlphaPriority() const
{
	return absolutePriority_;
}

_CODE_INLINE
void RenIMatBody::absoluteAlphaPriority(bool b)
{
	absolutePriority_ = b;
}

_CODE_INLINE
bool RenIMatBody::intraMeshAlphaPriority() const
{
	return intraMeshPriority_;
}

_CODE_INLINE
void RenIMatBody::intraMeshAlphaPriority(bool b)
{
	intraMeshPriority_ = b;
}

_CODE_INLINE
ushort RenIMatBody::meshId() const
{
	return meshId_;
}

_CODE_INLINE
void RenIMatBody::meshId(ushort i)
{
	meshId_ = i;
}

_CODE_INLINE
float RenIMatBody::fogMultiplier() const
{
	return fogMultiplier_;
}

_CODE_INLINE
void RenIMatBody::fogMultiplier(float m)
{
	fogMultiplier_ = m;
}

_CODE_INLINE
bool RenIMatBody::hasFogMultiplier() const
{
	return hasFogMultiplier_;
}

_CODE_INLINE
void RenIMatBody::hasFogMultiplier(bool b)
{
	hasFogMultiplier_ = b;
}

/* End MATERIAL.IPP **************************************************/

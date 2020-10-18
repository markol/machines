/*
 * U V A N I M . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/uvanim.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( RenUVTransform );
PER_DEFINE_PERSISTENT( RenUVUnityTransform );

RenUVTransform::RenUVTransform()
{
	// filterTexture_ defaults to a null RenTexture.
}

// virtual
RenUVTransform::~RenUVTransform()
{
}

ostream& operator <<( ostream& o, const RenUVTransform& t )
{
	t.print(o);
    return o;
}

// virtual
bool RenUVTransform::isUnity() const
{
	return false;
}

// virtual
bool RenUVUnityTransform::isUnity() const
{
	return true;
}

// virtual
void RenUVUnityTransform::transform(float uIn, float vIn, float* uOut, float* vOut) const
{
	PRE(uOut && vOut);
	*uOut = uIn;
	*vOut = vIn;
}

void RenUVUnityTransform::print(ostream& o) const
{
	o << "unity UV transform";
}

RenUVTransform* RenUVUnityTransform::clone() const
{
	return _NEW(RenUVUnityTransform(*this));
}

void perWrite( PerOstream& str, const RenUVTransform& t)
{
	str << t.filterTexture_;
}

void perRead( PerIstream& str, RenUVTransform& t)
{
	str >> t.filterTexture_;
}

void perWrite( PerOstream& ostr, const RenUVUnityTransform& transform )
{
    const RenUVTransform& base = transform;
    ostr << base;
}

void perRead( PerIstream& istr, RenUVUnityTransform& transform )
{
    RenUVTransform& base = transform;
    istr >> base;
}

const RenTexture& RenUVTransform::filterTexture() const
{
	return filterTexture_;
}

void RenUVTransform::filterTexture( const RenTexture& texture )
{
	filterTexture_ =  texture;
}

bool RenUVTransform::appliesTo(const RenTexture& tex) const
{
	// A null filter texture is used to indicate that this animation applies
	// to all polygons in a mesh.
	return filterTexture_.isNull() || tex == filterTexture_;
}

/* End UVANIM.CPP ***************************************************/

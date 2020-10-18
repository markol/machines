/*
 * U V T R A N S . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/uvtrans.hpp"

PER_DEFINE_PERSISTENT( RenUVTranslation );

RenUVTranslation::RenUVTranslation()
{
}

RenUVTranslation::RenUVTranslation(const MexVec2& o):
	translation_(fmod(o.x(), 1.0), fmod(o.y(), 1.0))
{
}

// virtual 
RenUVTranslation::~RenUVTranslation()
{
}

// virtual 
void RenUVTranslation::transform(float uIn, float vIn, float* uOut, float* vOut) const
{
	*uOut = uIn + translation_.x();
	*vOut = vIn + translation_.y();
}

// virtual 
void RenUVTranslation::print(ostream& o) const
{
	o << "UV translation of " << translation_;
}

// virtual 
RenUVTransform* RenUVTranslation::clone() const
{
	return _NEW(RenUVTranslation(*this));
}

const MexVec2& RenUVTranslation::translation() const
{
	return translation_;
}

void RenUVTranslation::translation(const MexVec2& o)
{
	translation_.setVector(fmod(o.x(), 1.0), fmod(o.y(), 1.0));
}

void perWrite( PerOstream& ostr, const RenUVTranslation& translation )
{
    const RenUVTransform& base = translation;
    ostr << base;
    ostr << translation.translation_;
}

void perRead( PerIstream& istr, RenUVTranslation& translation )
{
    RenUVTransform& base = translation;
    istr >> base;
    istr >> translation.translation_;
}

/* End UVTRANS.CPP **************************************************/

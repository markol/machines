/*
 * U V T R A N S . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/uvtrans.hpp"
#include "render/uvanim.hpp"
#include "render/uvtrans.hpp"

PER_DEFINE_PERSISTENT( W4dUVTranslation );

W4dUVTranslation::W4dUVTranslation(const PhysRelativeTime& duration, W4dLOD maxLOD,
                                   const MexVec2& v, const MexVec2& o):
	W4dUVPlan(duration, maxLOD),
	offset_(o),
	velocity_(v),
    positionPtr_( _NEW( RenUVTranslation ) )
{
	uvTranslation().translation(offset_);
}

W4dUVTranslation::W4dUVTranslation( PerConstructor con )
: W4dUVPlan( con )
{
}

// virtual
W4dUVTranslation::~W4dUVTranslation()
{
}

// virtual 
const Ren::UVTransformPtr& W4dUVTranslation::transform(const PhysRelativeTime& t)
{
	PhysRelativeTime time = t;

	if (isDone(t))
		time = duration();

	MexVec2 pos(offset_.x() + velocity_.x() * time, offset_.y() + velocity_.y() * time);
	uvTranslation().translation(pos);
	return positionPtr_;
}

// virtual 
void W4dUVTranslation::print(ostream& o) const
{
	o << "translation of " << velocity_ << " m/s, dur=" << duration() << " offset=" << offset_;
}

const MexVec2& W4dUVTranslation::velocity() const
{
	return velocity_;
}

const MexVec2& W4dUVTranslation::position() const
{
	return uvTranslation().translation();
}

RenUVTranslation& W4dUVTranslation::uvTranslation()
{
    return _STATIC_CAST( RenUVTranslation&, *positionPtr_ );
}

const RenUVTranslation& W4dUVTranslation::uvTranslation() const
{
    return _STATIC_CAST( RenUVTranslation&, *positionPtr_ );
}

void perWrite( PerOstream& ostr, const W4dUVTranslation& translation )
{
    const   W4dUVPlan& base = translation;
    
    ostr << base;
    ostr << translation.velocity_;
    ostr << translation.offset_;
    ostr << translation.positionPtr_;
}

void perRead( PerIstream& istr, W4dUVTranslation& translation )
{
    W4dUVPlan& base = translation;
    
    istr >> base;
    istr >> translation.velocity_;
    istr >> translation.offset_;
    istr >> translation.positionPtr_;
}

void W4dUVTranslation::setFilterTexture( const RenTexture& texture )
{
	positionPtr_->filterTexture( texture );
}

/* End UVTRANS.CPP **************************************************/

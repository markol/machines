/*
 * F D E D A P L N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/fdedapln.hpp"
#include "world4d/manager.hpp"
#include "render/colour.hpp"
#include "render/matvec.hpp"

W4dSolidFadedAlphaPlan::W4dSolidFadedAlphaPlan
(
	uint nFadedFrames,
    uint nSolidFrames,
    uint nMaterialsInVector,
	const RenMaterial& mat,
    W4dLOD maxLOD,
	PhysRelativeTime duration,
    MATHEX_SCALAR minAlpha,
    MATHEX_SCALAR maxAlpha
)
:   W4dMaterialPlan( duration, maxLOD ),
	nFadedFrames_( nFadedFrames ),
	nFrames_( nFadedFrames+nSolidFrames ),
    minAlpha_(minAlpha),
    alphaDifference_(maxAlpha-minAlpha),
	material_(mat)
{
	PRE( nMaterialsInVector >= 1 );
	PRE( minAlpha >= 0 );
	PRE( maxAlpha <= 1 );
	PRE( maxAlpha >= minAlpha );

	material_.makeNonSharable();

	materialVecPtr_ = _NEW( RenMaterialVec(nMaterialsInVector) );
    for( uint j=0; j<nMaterialsInVector; ++j )
	{
    	materialVecPtr_->push_back( material_ );
	}

    TEST_INVARIANT;
}

W4dSolidFadedAlphaPlan::~W4dSolidFadedAlphaPlan()
{
    TEST_INVARIANT;
}

void W4dSolidFadedAlphaPlan::CLASS_INVARIANT
{
    INVARIANT( nFrames_ != 0 );
}

ostream& operator <<( ostream& o, const W4dSolidFadedAlphaPlan& t )
{

    o << "W4dSolidFadedAlphaPlan " << (void*)&t << " start" << std::endl;
    o << "W4dSolidFadedAlphaPlan " << (void*)&t << " end" << std::endl;

    return o;
}

bool W4dSolidFadedAlphaPlan::isLODDefined( W4dLOD lodId ) const
{
    return lodId <= maxLOD() and isFaded();
}

bool W4dSolidFadedAlphaPlan::isFaded() const
{
	return W4dManager::instance().frameNumber() % nFrames_ < nFadedFrames_;
}


//virtual
const Ren::MaterialVecPtr& W4dSolidFadedAlphaPlan::materialVec
(
    const PhysRelativeTime&, W4dLOD
) const
{
    //Get the current frame, apply the offset and take modulus of frame count
    uint i = ( W4dManager::instance().frameNumber() ) % nFrames_;

	if(i < nFadedFrames_ )
	{
		MATHEX_SCALAR half = 0.5*nFadedFrames_;
		MATHEX_SCALAR alpha = minAlpha_ + alphaDifference_*fabs(i-half)/half;
	    RenColour diffuseColour = material_.diffuse();
	    diffuseColour.a( alpha );
    	_CONST_CAST(W4dSolidFadedAlphaPlan*, this)->material_.diffuse( diffuseColour );
	}

	return materialVecPtr_;
}

/* End FDEDAPLN.CPP *************************************************/

























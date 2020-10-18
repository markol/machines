/*
 * E M C O L P L A N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/emcolplan.hpp"

#include "render/matvec.hpp"

W4dSimpleEmColPlan::W4dSimpleEmColPlan
(
 const RenMaterial& mat,
 uint nMaterialsInVector,
 const W4dColourPlanDataPtr& colourPlanPtr,
 W4dLOD maxLOD
)
:   W4dMaterialPlan( colourPlanPtr->duration(), maxLOD ),
    material_( mat ),
    colourPlanPtr_( colourPlanPtr )
{

    //Make the material non-shareable
    material_.makeNonSharable();

    //Construct a material vec referencing this single material
    RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( nMaterialsInVector ) );
    while( nMaterialsInVector-- )
        pMaterialVec->push_back( material_ );

    materialVecPtr_ = pMaterialVec;

    TEST_INVARIANT;
}

W4dSimpleEmColPlan::~W4dSimpleEmColPlan()
{
    TEST_INVARIANT;

}

void W4dSimpleEmColPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
bool W4dSimpleEmColPlan::isLODDefined( W4dLOD lodId ) const
{
    return lodId <= maxLOD();
}

//virtual
const Ren::MaterialVecPtr& W4dSimpleEmColPlan::materialVec
(
    const PhysRelativeTime& timeOffset, W4dLOD lodId
) const
{
    PRE( isLODDefined( lodId ) );

    //Compute the result of the alpha scalar plan
    RenColour colour = colourPlanPtr_->colour( timeOffset );

    //Set the material alpha
    RenColour emissiveColour = material_.emissive();
    emissiveColour.r( colour.r() );
    emissiveColour.g( colour.g() );
    emissiveColour.b( colour.b() );
    _CONST_CAST(W4dSimpleEmColPlan*, this)->material_.emissive( emissiveColour );

    //Return the material vector
    return materialVecPtr_;
}

/* End EMCOLPLAN.CPP **************************************************/

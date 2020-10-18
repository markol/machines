/*
 * A L P H S I M P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/simintpl.hpp"
#include "render/colour.hpp"
#include "render/matvec.hpp"


W4dSimpleIntensityPlan::W4dSimpleIntensityPlan
(
    const RenMaterial& mat, uint nMaterialsInVector,
    const PhysScalarPlanPtr& intensityPlanPtr, W4dLOD maxLOD
)
:   W4dMaterialPlan( intensityPlanPtr->duration(), maxLOD ),
    material_( mat ),
    intensityPlanPtr_( intensityPlanPtr )
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

W4dSimpleIntensityPlan::~W4dSimpleIntensityPlan()
{
    TEST_INVARIANT;

}

void W4dSimpleIntensityPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dSimpleIntensityPlan& t )
{

    o << "W4dSimpleIntensityPlan " << (void*)&t << " start" << std::endl;
    o << "W4dSimpleIntensityPlan " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool W4dSimpleIntensityPlan::isLODDefined( W4dLOD lodId ) const
{
    return lodId <= maxLOD();
}

//virtual
const Ren::MaterialVecPtr& W4dSimpleIntensityPlan::materialVec
(
    const PhysRelativeTime& timeOffset, W4dLOD lodId
) const
{
    PRE( isLODDefined( lodId ) );

    //Compute the result of the intensity scalar plan
    MATHEX_SCALAR intensity = intensityPlanPtr_->scalar( timeOffset );

    //Set the material intensity

    RenColour diffuseColour = material_.diffuse();
    diffuseColour = RenColour(intensity*diffuseColour.r(), intensity*diffuseColour.g(), intensity*diffuseColour.b() );
    _CONST_CAST(W4dSimpleIntensityPlan*, this)->material_.diffuse( diffuseColour );

    RenColour ambientColour = material_.ambient();
    ambientColour = RenColour(intensity*ambientColour.r(), intensity*ambientColour.g(), intensity*ambientColour.b() );
    _CONST_CAST(W4dSimpleIntensityPlan*, this)->material_.ambient( ambientColour );

    RenColour specularColour = material_.specular();
    specularColour = RenColour(intensity*specularColour.r(), intensity*specularColour.g(), intensity*specularColour.b() );
    _CONST_CAST(W4dSimpleIntensityPlan*, this)->material_.specular( specularColour );

    RenColour emissiveColour = material_.emissive();
    emissiveColour = RenColour(intensity*emissiveColour.r(), intensity*emissiveColour.g(), intensity*emissiveColour.b() );
    _CONST_CAST(W4dSimpleIntensityPlan*, this)->material_.emissive( emissiveColour );

    //Return the material vector
    return materialVecPtr_;
}
/* End ALPHSIMP.CPP *************************************************/

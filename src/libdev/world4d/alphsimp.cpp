/*
 * A L P H S I M P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/alphsimp.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/colour.hpp"
#include "render/matvec.hpp"
#include "render/material.hpp"
#include "render/texture.hpp"

#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"

W4dSimpleAlphaPlan::W4dSimpleAlphaPlan
(
    const RenMaterial& mat, uint nMaterialsInVector,
    const PhysScalarPlanPtr& alphaPlanPtr, W4dLOD maxLOD
)
:   W4dMaterialPlan( alphaPlanPtr->duration(), maxLOD ),
    material_( mat ),
    alphaPlanPtr_( alphaPlanPtr )
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

W4dSimpleAlphaPlan::~W4dSimpleAlphaPlan()
{
    TEST_INVARIANT;

}

void W4dSimpleAlphaPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dSimpleAlphaPlan& t )
{

    o << "W4dSimpleAlphaPlan " << (void*)&t << " start" << std::endl;
    o << "W4dSimpleAlphaPlan " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool W4dSimpleAlphaPlan::isLODDefined( W4dLOD lodId ) const
{
    return lodId <= maxLOD();
}

//virtual
const Ren::MaterialVecPtr& W4dSimpleAlphaPlan::materialVec
(
    const PhysRelativeTime& timeOffset, W4dLOD lodId
) const
{
    PRE( isLODDefined( lodId ) );

    //Compute the result of the alpha scalar plan
    MATHEX_SCALAR alpha = alphaPlanPtr_->scalar( timeOffset );

    //Set the material alpha
    RenColour diffuseColour = material_.diffuse();
    diffuseColour.a( alpha );
    _CONST_CAST(W4dSimpleAlphaPlan*, this)->material_.diffuse( diffuseColour );

    //Return the material vector
    return materialVecPtr_;
}

//static
void W4dSimpleAlphaPlan::makePlan(
									W4dEntity* pEntity,
									const PhysAbsoluteTime& startTime,
									const PhysScalarPlanPtr& alphaPlanPtr,
									W4dLOD maxLOD  )
{
    const RenMeshInstance& meshInst = _CONST_CAST(const W4dEntity*, pEntity)->mesh();
    const Ren::MaterialVecPtr& materialVecPtr = meshInst.materialVec();
	RenMaterialVec* pMaterialVec;

	if( materialVecPtr.isDefined() )
	{
		pMaterialVec = _NEW( RenMaterialVec( *materialVecPtr ) );
	}
	else
	{

		pMaterialVec =meshInst.mesh()->materialVec().release();
	}

	size_t nMat = pMaterialVec->size();

	ASSERT( nMat > 0, "no material " );

	if( nMat > 0 )
	{
		RenMaterial mat = (*pMaterialVec)[0];
	    W4dMaterialPlanPtr matPlanPtr = _NEW( W4dSimpleAlphaPlan( mat, nMat, alphaPlanPtr, maxLOD) );
		pEntity->entityPlanForEdit().materialPlan(matPlanPtr, startTime );
	}

	_DELETE( pMaterialVec );
}
/* End ALPHSIMP.CPP *************************************************/

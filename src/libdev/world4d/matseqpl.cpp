/*
 * M A T S E Q P L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/matseqpl.hpp"

#ifndef _INLINE
    #include "world4d/matseqpl.ipp"
#endif

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"
#include "render/matvec.hpp"

#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"

W4dMaterialSequencePlan::W4dMaterialSequencePlan
(
    const MaterialVecPtrs& materialVecPtrs, const PhysRelativeTime& duration, W4dLOD maxLOD
)
:   W4dMaterialPlan( duration, maxLOD )
{
    PRE( materialVecPtrs.size() != 0 );

    //Set the capacity of the collection
    materialVecPtrsVec_.reserve( maxLOD + 1 );

    //Store the lowest LOD
    materialVecPtrsVec_.push_back( materialVecPtrs );

    TEST_INVARIANT;
}

W4dMaterialSequencePlan::~W4dMaterialSequencePlan()
{
    TEST_INVARIANT;

}

void W4dMaterialSequencePlan::lodPlan( const MaterialVecPtrs& materialVecPtrs, W4dLOD lodId )
{
    PRE( lodId > 0  and  lodId <= maxLOD() );
    PRE( not isLODDefined( lodId ) );
    PRE( isLODDefined( lodId - 1 ) );
    PRE( materialVecPtrs.size() != 0 );

    //Store the mapping
    materialVecPtrsVec_.push_back( materialVecPtrs );
}

//virtual
bool W4dMaterialSequencePlan::isLODDefined( W4dLOD lodId ) const
{
    return lodId >= 0  and  lodId < materialVecPtrsVec_.size();
}

void W4dMaterialSequencePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
    INVARIANT( materialVecPtrsVec_.size() != 0 );
}

ostream& operator <<( ostream& o, const W4dMaterialSequencePlan& t )
{

    o << "W4dMaterialSequencePlan " << (void*)&t << " start" << std::endl;
    o << "W4dMaterialSequencePlan " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
const Ren::MaterialVecPtr& W4dMaterialSequencePlan::materialVec
(
    const PhysRelativeTime& timeOffset, W4dLOD lodId
) const
{
    PRE( timeOffset >= 0.0 );
    PRE( isLODDefined( lodId  ) );
    TEST_INVARIANT;

    //Get the plan for this level of detail
    const MaterialVecPtrs& materialVecPtrs = materialVecPtrsVec_[ lodId ];

    //Ensure we round time to nearest element, to avoid skipping or duplicating frames
    PhysRelativeTime d = duration();
    size_t nFrames = materialVecPtrs.size();
    size_t i;
    if( timeOffset >= d )
    {
        i = nFrames - 1;
    }
    else
    {
        double n = _STATIC_CAST( double, nFrames );
        i = _STATIC_CAST( size_t, (timeOffset / d) * n + 0.01 * (d/n) );
        if( i == nFrames )
            --i;
    }

    return materialVecPtrs[ i ];
}

//static
bool W4dMaterialSequencePlan::makePlan(W4dEntity* pEntity, const ctl_vector<RenTexture>& textures,
	                     const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration, uint animId)
{
	bool Found = false;

    const RenMeshInstance& meshInst = _CONST_CAST(const W4dEntity*, pEntity)->mesh();
    const Ren::MaterialVecPtr& materialVecPtr = meshInst.materialVec();
	RenMaterialVec* pAnimMaterialVec;

	if( materialVecPtr.isDefined() )
	{
		pAnimMaterialVec = _NEW( RenMaterialVec( *materialVecPtr ) );
	}
	else
	{
		pAnimMaterialVec =meshInst.mesh()->materialVec().release();
	}

	//find the animation keys in pAnimMaterialVec
	uint nMaterials = pAnimMaterialVec->size();
	ASSERT( nMaterials > 0, " no materials ");

	uint nTextures = textures.size();
	ASSERT( nTextures>0, " no key textures " );

	ctl_vector<uint> offsets;
	offsets.reserve(nTextures);

	for(uint animFrame = 0; animFrame < nMaterials; ++animFrame)
	{
		const RenTexture& tex = ((*pAnimMaterialVec)[animFrame]).texture();

		ctl_vector<RenTexture>::const_iterator it = find( textures.begin(), textures.end(), tex );
		if( it != textures.end() )
		{
			uint frameOffset = it - textures.begin();
			offsets.push_back(frameOffset);
			Found = true;
		}
		else
			offsets.push_back(9999);
	}


	MaterialVecPtrs *pMaterials = _NEW( MaterialVecPtrs );
	pMaterials->reserve(nTextures);

	for( size_t i = 0; i != nTextures; ++i )
	{
		RenMaterialVec* pMaterialsVec = _NEW( RenMaterialVec( nMaterials) );

		for(size_t j=0; j< nMaterials; ++j)
		{
			RenMaterial mat = (*pAnimMaterialVec)[j];
						mat.makeNonSharable();

			uint offset = offsets[j];
			if( offset != 9999 )
			{
				uint frameNumber = (i + offset)%nTextures;
				mat.texture(textures[frameNumber]);
			}
			pMaterialsVec->push_back(mat);
		}

		pMaterials->push_back( Ren::MaterialVecPtr( pMaterialsVec ) );

	}

	//Store in counted pointer

	W4dMaterialSequencePlan  *pMaterialSeqPlan =
	                     _NEW( W4dMaterialSequencePlan(*pMaterials, duration, 0) );

	W4dMaterialPlanPtr materialPlanPtr( pMaterialSeqPlan );
	pEntity->entityPlanForEdit().materialPlan( materialPlanPtr, startTime, 0, animId );

	_DELETE(pAnimMaterialVec);
	_DELETE( pMaterials );

	return Found;
}

//static
bool W4dMaterialSequencePlan::changeTexture( W4dEntity* pEntity, const RenTexture& fromTexture, const RenTexture& toTexture )
{
	bool found = false;

    const RenMeshInstance& meshInst = _CONST_CAST(const W4dEntity*, pEntity)->mesh();
    const Ren::MaterialVecPtr& meshInstMaterialVecPtr = meshInst.materialVec();
	RenMaterialVec* pAnimMaterialVec;

	if( meshInstMaterialVecPtr.isDefined() )
	{
		pAnimMaterialVec = _NEW( RenMaterialVec( *meshInstMaterialVecPtr ) );
	}
	else
	{
		pAnimMaterialVec =meshInst.mesh()->materialVec().release();
	}

	size_t	nMat = pAnimMaterialVec->size();

	Ren::MaterialVecPtr materialVecPtr = _NEW( RenMaterialVec( nMat) );

	for( size_t i=0; i<nMat; ++i)
	{
		RenMaterial& mat = (*pAnimMaterialVec)[i];
					 mat.makeNonSharable();

		if( mat.texture() == fromTexture )
		{
			found = true;
			mat.texture( toTexture );
		}
		materialVecPtr->push_back( mat );
	}

	//apply the new materialVecPtr to the mesh
	//_CONST_CAST(RenMeshInstance, meshInst).materialVec( materialVecPtr );
	_CONST_CAST(RenMeshInstance&, meshInst).materialVec( materialVecPtr );

	//delete the old to avoid mem leak
	_DELETE( pAnimMaterialVec );

	return found;
}

/* End MATSEQPL.CPP *************************************************/

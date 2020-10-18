/*
 * M A T F R M P L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/matfrmpl.hpp"
#include "world4d/manager.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"
#include "render/matvec.hpp"

W4dMaterialFramePlan::W4dMaterialFramePlan
(
    const MaterialVecPtrsPtr& materialVecPtrsPtr, uint frameOffset,
    const PhysRelativeTime& duration, uint nFrames, W4dLOD maxLOD
)
:   W4dMaterialPlan( duration, maxLOD ),
    frameOffset_( frameOffset ),
	nFrames_(nFrames)
{
    PRE( materialVecPtrsPtr->size() != 0 );
	PRE( nFrames_ >= 1 );

    //Set the capacity of the collection
    materialVecPtrsPtrVec_.reserve( maxLOD + 1 );

    //Store the lowest LOD
    materialVecPtrsPtrVec_.push_back( materialVecPtrsPtr );

    TEST_INVARIANT;
}

W4dMaterialFramePlan::~W4dMaterialFramePlan()
{
    TEST_INVARIANT;

}

//virtual
bool W4dMaterialFramePlan::isLODDefined( W4dLOD lodId ) const
{
    return lodId >= 0  and  lodId < materialVecPtrsPtrVec_.size();
}

void W4dMaterialFramePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dMaterialFramePlan& t )
{

    o << "W4dMaterialFramePlan " << (void*)&t << " start" << std::endl;
    o << "W4dMaterialFramePlan " << (void*)&t << " end" << std::endl;

    return o;
}

void W4dMaterialFramePlan::lodPlan( const MaterialVecPtrsPtr& materialVecPtrsPtr, W4dLOD lodId )
{
	PRE_INFO(lodId);
    PRE_INFO(materialVecPtrsPtr->size());
	PRE_DATA(for (int i=0; i!=maxLOD(); ++i))
	PRE_INFO(isLODDefined(i));
    PRE( lodId > 0  and  lodId <= maxLOD() );
    PRE( not isLODDefined( lodId ) );
    PRE( isLODDefined( lodId - 1 ) );
    PRE( materialVecPtrsPtr->size() != 0 );

    //Store the mapping
    materialVecPtrsPtrVec_.push_back( materialVecPtrsPtr );
}

//virtual
const Ren::MaterialVecPtr& W4dMaterialFramePlan::materialVec
(
    const PhysRelativeTime&, W4dLOD lodId
) const
{
    PRE( isLODDefined( lodId  ) );
    TEST_INVARIANT;

    //Get the plan for this level of detail
    const MaterialVecPtrsPtr& materialVecPtrsPtr = materialVecPtrsPtrVec_[ lodId ];

	uint totalFrames = nFrames_ * materialVecPtrsPtr->size();

    //Get the current frame, apply the offset and take modulus of totalFrames
    uint i = (W4dManager::instance().frameNumber() + frameOffset_) % totalFrames;
		 i /= nFrames_;

    //hence return the appropriate frame
    return (*materialVecPtrsPtr)[i];
}

//static
bool W4dMaterialFramePlan::makePlan(W4dEntity* pEntity, const ctl_vector<RenTexture>& textures,
	                     const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration, uint frameReps, uint startFrame, W4dLOD maxLOD)
{
	W4dMaterialPlanPtr materialPlanPtr( W4dMaterialFramePlan::createPlan(pEntity, textures,
	                     duration, frameReps, startFrame, maxLOD) );

	pEntity->entityPlanForEdit().materialPlan( materialPlanPtr, startTime );

	return true;
}
//static
W4dMaterialFramePlan* W4dMaterialFramePlan::createPlan(W4dEntity* pEntity, const ctl_vector<RenTexture>& textures,
	                     const PhysRelativeTime& duration, uint frameReps, uint startFrame, W4dLOD maxLOD)
{
	bool Found = false;
	W4dMaterialFramePlan* pMaterialFramePlan = NULL;

    for( W4dLOD lodId = 0; lodId <= maxLOD; ++lodId )
	{
	    const RenMeshInstance& meshInst = _CONST_CAST(const W4dEntity*, pEntity)->mesh( lodId );
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
		uint nTextures = textures.size();
		ctl_vector<uint> offsets;
		offsets.reserve(nTextures);

		for(uint animFrame = 0; animFrame < nMaterials; ++animFrame)
		{
			const RenTexture& tex = ((*pAnimMaterialVec)[animFrame]).texture();

			ctl_vector<RenTexture>::const_iterator it = find( textures.begin(), textures.end(), tex );
			if( it != textures.end() )
			{
				uint frameOffset; // = it - textures.begin();

				if( startFrame != 0 )
					frameOffset = startFrame;
				else
					frameOffset = it - textures.begin();

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
		MaterialVecPtrsPtr materialsPtr = pMaterials;

		if( pMaterialFramePlan == NULL )
			pMaterialFramePlan = _NEW( W4dMaterialFramePlan(materialsPtr, 0, duration, frameReps, maxLOD) );
		else
        	pMaterialFramePlan->lodPlan( materialsPtr, lodId );

		_DELETE(pAnimMaterialVec);
	}

	return pMaterialFramePlan;
}

/* End MATFRMPL.CPP *************************************************/

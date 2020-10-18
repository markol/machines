/*
 * H A L O M P L N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/halompln.hpp"

#include "stdlib/string.hpp"

#include "system/pathname.hpp"
#include "render/material.hpp"
#include "render/matvec.hpp"
#include "render/colour.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"

#include "phys/lsclplan.hpp"

#include "world4d/manager.hpp"

MachPhysLightningHaloMaterialPlan::MachPhysLightningHaloMaterialPlan
(
    const PhysRelativeTime& duration, W4dLOD maxLOD,
    MachPhysLightningHalo::HaloType type
)
:
W4dMaterialPlan(duration, maxLOD)
{
   	mat1_.makeNonSharable();
   	mat2_.makeNonSharable();
   	mat3_.makeNonSharable();
    mat4_.makeNonSharable();

    //Set the capacity of the collection
    materialVecPtrs_.reserve( 4 );

    PhysLinearScalarPlan::ScalarVec times;
    	times.reserve(1);
		times.push_back(duration);


	RenTexture newTex;

	switch(type)
	{
		case MachPhysLightningHalo::VIRUS:
		{
    		PhysLinearScalarPlan::ScalarVec scalesV;
    			scalesV.reserve(2);
        		scalesV.push_back(1);
        		scalesV.push_back(1);

    		PhysLinearScalarPlan* pAlphaPlanV = _NEW( PhysLinearScalarPlan(times, scalesV) );

	    	alphaPlanPtr_ = pAlphaPlanV;

			RenMaterial glowingYellow;
			glowingYellow.diffuse(RenColour::black());
			glowingYellow.emissive(RenColour::yellow());
			mat1_ = glowingYellow;
			mat2_ = glowingYellow;
			mat3_ = glowingYellow;
			mat4_ = glowingYellow;

			newTex = RenTexManager::instance().createTexture("vrus1_bt.bmp");
			mat1_.texture(newTex);

			newTex = RenTexManager::instance().createTexture("vrus2_bt.bmp");
			mat2_.texture(newTex);

			newTex = RenTexManager::instance().createTexture("vrus3_bt.bmp");
			mat3_.texture(newTex);

			newTex = RenTexManager::instance().createTexture("vrus4_bt.bmp");
			mat4_.texture(newTex);

			break;
		}
		case MachPhysLightningHalo::ELECTRIC:
	    {

    		PhysLinearScalarPlan::ScalarVec scalesE;
    			scalesE.reserve(2);
        		scalesE.push_back(1);
        		scalesE.push_back(0);

    		PhysLinearScalarPlan* pAlphaPlanE = _NEW( PhysLinearScalarPlan(times, scalesE) );

	    	alphaPlanPtr_ = pAlphaPlanE;

			RenMaterial glowingPink;
			glowingPink.diffuse(RenColour::black());
			glowingPink.emissive(RenColour::white());
			mat1_ = glowingPink;
			mat2_ = glowingPink;
			mat3_ = glowingPink;
			mat4_ = glowingPink;

			newTex = RenTexManager::instance().createTexture("elec1_bt.bmp");
			mat1_.texture(newTex);

			newTex = RenTexManager::instance().createTexture("elec2_bt.bmp");
			mat2_.texture(newTex);

			newTex = RenTexManager::instance().createTexture("elec3_bt.bmp");
			mat3_.texture(newTex);

			newTex = RenTexManager::instance().createTexture("elec4_bt.bmp");
			mat4_.texture(newTex);

			break;
		}
	}

     RenMaterialVec* pMaterialVec1 = _NEW( RenMaterialVec( 4 ) );
     	pMaterialVec1->push_back( mat1_ );
     	pMaterialVec1->push_back( mat2_ );
     	pMaterialVec1->push_back( mat3_ );
     	pMaterialVec1->push_back( mat4_ );

     RenMaterialVec* pMaterialVec2 = _NEW( RenMaterialVec( 4 ) );
     	pMaterialVec2->push_back( mat4_ );
     	pMaterialVec2->push_back( mat1_ );
     	pMaterialVec2->push_back( mat2_ );
     	pMaterialVec2->push_back( mat3_ );

     RenMaterialVec* pMaterialVec3 = _NEW( RenMaterialVec( 4 ) );
     	pMaterialVec3->push_back( mat3_ );
     	pMaterialVec3->push_back( mat4_ );
     	pMaterialVec3->push_back( mat1_ );
     	pMaterialVec3->push_back( mat2_ );

     RenMaterialVec* pMaterialVec4 = _NEW( RenMaterialVec( 4 ) );
     	pMaterialVec4->push_back( mat2_ );
     	pMaterialVec4->push_back( mat3_ );
     	pMaterialVec4->push_back( mat4_ );
     	pMaterialVec4->push_back( mat1_ );

     Ren::MaterialVecPtr materialVecPtr1( pMaterialVec1 );
     Ren::MaterialVecPtr materialVecPtr2( pMaterialVec2 );
     Ren::MaterialVecPtr materialVecPtr3( pMaterialVec3 );
     Ren::MaterialVecPtr materialVecPtr4( pMaterialVec4 );

     materialVecPtrs_.push_back( materialVecPtr1 );
     materialVecPtrs_.push_back( materialVecPtr2 );
     materialVecPtrs_.push_back( materialVecPtr3 );
     materialVecPtrs_.push_back( materialVecPtr4 );

    TEST_INVARIANT;
}

//copy ctor
MachPhysLightningHaloMaterialPlan::MachPhysLightningHaloMaterialPlan(const MachPhysLightningHaloMaterialPlan& rhs)
:materialVecPtrs_(rhs.materialVecPtrs_),
W4dMaterialPlan(rhs.duration(), rhs.maxLOD())
{
	//intentionally empty
}


//True if the plan is defined for level of detail lodId.
//virtual
bool MachPhysLightningHaloMaterialPlan::isLODDefined( W4dLOD lodId) const
{
    return lodId <= maxLOD();
}

    //Override to define result as a function of timeOffset.
    //If timeOffset is greater than duration, the mapping at time duration is to be returned.
    //The mapping for level of detail defined by lodId is to be returned.
//virtual
const Ren::MaterialVecPtr& MachPhysLightningHaloMaterialPlan::materialVec( const PhysRelativeTime& timeOffset,
                                                    W4dLOD) const
{
	//Compute the result of the alpha scalar plan
    MATHEX_SCALAR alpha = alphaPlanPtr_->scalar( timeOffset );
    //Set the alpha	for the 4 materials
    RenColour diffuseColour = mat1_.diffuse();
    	      diffuseColour.a( alpha );
    _CONST_CAST(MachPhysLightningHaloMaterialPlan*, this)->mat1_.diffuse( diffuseColour );

	diffuseColour = mat2_.diffuse();
    diffuseColour.a( alpha );
    _CONST_CAST(MachPhysLightningHaloMaterialPlan*, this)->mat2_.diffuse( diffuseColour );

	diffuseColour = mat3_.diffuse();
    diffuseColour.a( alpha );
    _CONST_CAST(MachPhysLightningHaloMaterialPlan*, this)->mat3_.diffuse( diffuseColour );

	diffuseColour = mat4_.diffuse();
    diffuseColour.a( alpha );
    _CONST_CAST(MachPhysLightningHaloMaterialPlan*, this)->mat4_.diffuse( diffuseColour );

	uint i = W4dManager::instance().frameNumber()%4;
	return materialVecPtrs_[i];
}


MachPhysLightningHaloMaterialPlan::~MachPhysLightningHaloMaterialPlan()
{
    TEST_INVARIANT;

}

void MachPhysLightningHaloMaterialPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLightningHaloMaterialPlan& t )
{

    o << "MachPhysLightningHaloMaterialPlan " << (void*)&t << " start" << std::endl;
    o << "MachPhysLightningHaloMaterialPlan " << (void*)&t << " end" << std::endl;

    return o;
}

/* End HALOMPLN.CPP *************************************************/

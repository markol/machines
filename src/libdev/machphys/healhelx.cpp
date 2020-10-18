/*
 * H E A L H E L X . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/healhelx.hpp"
#include "machphys/helix.hpp"
#include "machphys/effects.hpp"

#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"
#include "render/texture.hpp"
#include "sim/manager.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/matseqpl.hpp"
#include "render/colour.hpp"
#include "render/material.hpp"
#include "render/scale.hpp"
#include "render/matvec.hpp"
#include "phys/lsclplan.hpp"
#include "phys/timeangl.hpp"
#include "ctl/vector.hpp"

PER_DEFINE_PERSISTENT( MachPhysHealHelix );

MachPhysHealHelix::MachPhysHealHelix( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( pParent, localTransform, NOT_SOLID )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    //Helix models vary x coordinates from 0 to 5 metres
    const MexVec3 origin( 0.0, 0.0, 0.0 );

    //Add a pair of left and right helixes
    const MexTransform3d identityTransform;
    MachPhysHelix* pLeftHelix = _NEW( MachPhysHelix( this, identityTransform,
                                                     MachPhysHelix::LEFT_TWIST ) );
    MachPhysHelix* pRightHelix = _NEW( MachPhysHelix( this, identityTransform,
                                                      MachPhysHelix::RIGHT_TWIST ) );

    //First time construct various plans and things
    static bool firstTime = true;
    static PhysScalarPlanPtr alphaValuePlanPtr;
    static W4dMaterialPlanPtr alphaPlanPtr;
    static PhysMotionPlanPtr leftSpinPlanPtr;
    static PhysMotionPlanPtr rightSpinPlanPtr;

    if( firstTime )
    {
        firstTime = false;

        //Construct an alpha value plan
        PhysLinearScalarPlan::ScalarVec times;
        PhysLinearScalarPlan::ScalarVec values;
        times.reserve( 2 );
        times.push_back( 0.5 );
        times.push_back( 1.0 );
        values.reserve( 3 );
        values.push_back( 0.8 );
        values.push_back( 0.2 );
        values.push_back( 0.8 );

        PhysLinearScalarPlan* pAlphaValuePlan = _NEW( PhysLinearScalarPlan( times, values ) );
        alphaValuePlanPtr = pAlphaValuePlan;

        //Set up an alpha plan for them
        W4dSimpleAlphaPlan* pAlphaPlan =
            _NEW( W4dSimpleAlphaPlan( helixMaterial(), 1, alphaValuePlanPtr, 2 ) );
        alphaPlanPtr = pAlphaPlan;

        //Set up the motion plans
        //Create left spin angles
        PhysMotionPlan::Angles* pLeftAngles = _NEW( PhysMotionPlan::Angles );
        pLeftAngles->reserve( 4 );
        pLeftAngles->push_back( MexDegrees( 0.0 ) );
        pLeftAngles->push_back( MexDegrees( 120.0 ) );
        pLeftAngles->push_back( MexDegrees( 240.0 ) );
        pLeftAngles->push_back( MexDegrees( 360.0 ) );
        PhysMotionPlan::AnglesPtr leftAnglesPtr( pLeftAngles );

        //Create right spin angles
        PhysMotionPlan::Angles* pRightAngles = _NEW( PhysMotionPlan::Angles );
        pRightAngles->reserve( 4 );
        pRightAngles->push_back( MexDegrees( 360.0 ) );
        pRightAngles->push_back( MexDegrees( 240.0 ) );
        pRightAngles->push_back( MexDegrees( 120.0 ) );
        pRightAngles->push_back( MexDegrees( 0.0 ) );
        PhysMotionPlan::AnglesPtr rightAnglesPtr( pRightAngles );

        //Set up the times
        PhysMotionPlan::Times* pTimes = _NEW( PhysMotionPlan::Times );
        pTimes->reserve( 3 );
        pTimes->push_back( 1.0 );
        pTimes->push_back( 2.0 );
        pTimes->push_back( 3.0 );
        PhysMotionPlan::TimesPtr timesPtr( pTimes );

        //Now the motion plans
        PhysTimedAnglePlan* pLeftSpinPlan =
            _NEW( PhysTimedAnglePlan( leftAnglesPtr, timesPtr,
                                      MexVec3(1.0,0.0,0.0), origin ) );
        leftSpinPlanPtr = pLeftSpinPlan;

        PhysTimedAnglePlan* pRightSpinPlan =
            _NEW( PhysTimedAnglePlan( rightAnglesPtr, timesPtr,
                                      MexVec3(1.0,0.0,0.0), origin ) );
        rightSpinPlanPtr = pRightSpinPlan;
    }

    //Get current time and entity plans
    PhysAbsoluteTime now = SimManager::instance().currentTime();
    W4dEntityPlan& leftEntityPlan = pLeftHelix->entityPlanForEdit();
    W4dEntityPlan& rightEntityPlan = pRightHelix->entityPlanForEdit();

    //Apply the material plan
    leftEntityPlan.materialPlan( alphaPlanPtr, now, 10000 );
    rightEntityPlan.materialPlan( alphaPlanPtr, now, 10000 );

    //Apply the motion plans
    leftEntityPlan.absoluteMotion( leftSpinPlanPtr, now - 2.0, 10000 );
    rightEntityPlan.absoluteMotion( rightSpinPlanPtr, now, 10000 );

    TEST_INVARIANT;
}

MachPhysHealHelix::MachPhysHealHelix( PerConstructor con )
: W4dEntity( con )
{
}

//static
const RenMaterial& MachPhysHealHelix::helixMaterial()
{
    static RenMaterial mat( RenColour::black() );
    static bool firstTime = true;

    if( firstTime )
    {
        //Set colour to self-luminous green
        firstTime = false;
        mat.emissive( RenColour( 0.0, 1.0, 0.0 ) );
    }

    return mat;
}

MachPhysHealHelix::~MachPhysHealHelix()
{
    TEST_INVARIANT;

}

void MachPhysHealHelix::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysHealHelix& t )
{

    o << "MachPhysHealHelix " << (void*)&t << " start" << std::endl;
    o << "MachPhysHealHelix " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysHealHelix::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysHealHelix& helix )
{
    const W4dEntity& base = helix;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysHealHelix& helix )
{
    W4dEntity& base = helix;

    istr >> base;
}

/*
//static
const W4dMaterialPlanPtr& MachPhysHealHelix::healCyclingTexturePlan()
{
    //Set up a material sequence plan using the smoke textures
    //and an emissive green colour.
    static W4dMaterialPlanPtr thePlan;
    static firstTime = true;
    if( firstTime )
    {
        firstTime = false;

        //get the textures and count them
        const MachPhysEffects::Textures& textures = MachPhysEffects::smokeTextures();
        uint nTextures = textures.size();

        //set up a vector of counted pointer to material vector
        W4dMaterialSequencePlan::MaterialVecPtrs materialVecPtrs;
        materialVecPtrs.reserve( nTextures );

        //Create a material vector with a single element for each one
        for( uint i = 0; i != nTextures; ++i )
        {
            //Create a material with diffuse colour black, and emissive green
            RenMaterial mat( RenColour::black() );
            mat.emissive( RenColour(0.0,1.0,0.0) );

            //Set the texture
            mat.texture( textures[i] );

            //Create a single-element material vector from this material
            RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( 1 ) );
            pMaterialVec->push_back( mat );

            //Add this vector to the vector of material vectors
            Ren::MaterialVecPtr materialVecPtr( pMaterialVec );
            materialVecPtrs.push_back( materialVecPtr );
        }

        //Create the plan itself
        W4dMaterialSequencePlan* pPlan =
            _NEW( W4dMaterialSequencePlan( materialVecPtrs, PhysRelativeTime( 1.0 ), W4dLOD( 3 ) ) );

        thePlan = pPlan;
    }

    return thePlan;
}
*/
/* End HEALHELX.CPP *************************************************/

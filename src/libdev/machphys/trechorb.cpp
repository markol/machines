/*
 * T R E C H O R B . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"
#include "phys/lsclplan.hpp"
#include "phys/linemoti.hpp"
#include "render/material.hpp"
#include "render/colour.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/manager.hpp"
#include "machphys/trechorb.hpp"
#include "machphys/radidisc.hpp"
#include "machphys/racechng.hpp"
#include "machphys/wepdata.hpp"

PER_DEFINE_PERSISTENT( MachPhysTreacheryOrb );

MachPhysTreacheryOrb::MachPhysTreacheryOrb
(
    W4dEntity* pParent, const MexTransform3d& localTransform
)
:   MachPhysLinearProjectile( pParent, localTransform ),
    pDisc_( NULL )
{
    //Create a radial disc child
    pDisc_ = _NEW( MachPhysRadialDisc( this, MexTransform3d() ) );

    TEST_INVARIANT;
}

MachPhysTreacheryOrb::MachPhysTreacheryOrb( PerConstructor con )
: MachPhysLinearProjectile( con )
{
}

MachPhysTreacheryOrb::~MachPhysTreacheryOrb()
{
    TEST_INVARIANT;

}

void MachPhysTreacheryOrb::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysTreacheryOrb& t )
{

    o << "MachPhysTreacheryOrb " << (void*)&t << " start" << std::endl;
    o << "MachPhysTreacheryOrb " << (void*)&t << " end" << std::endl;

    return o;
}

PhysRelativeTime MachPhysTreacheryOrb::beLaunched
(
    const PhysAbsoluteTime& startTime, const MachPhysWeaponData& data, MachPhys::Race race
)
{
    PRE( pDisc_ != NULL );

    //this is the time spent expanding before flight
    const PhysRelativeTime expandTime = 0.7;

    //get the entity plans for the projectile and the child disc
    W4dEntityPlan& entityPlan = entityPlanForEdit();
    W4dEntityPlan& discEntityPlan = pDisc_->entityPlanForEdit();

    //Add a visibility plan so it becomes visible at launch time
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );

    //Set a scale plan for the expansion phase
    static W4dScalePlanPtr scalePlanPtr =
        _NEW( W4dSimpleUniformScalePlan( 0.01, 2.0, expandTime ) );

    discEntityPlan.scalePlan( scalePlanPtr, startTime );

    //Set up a material plan for the disc. It fades up from 0 to 50% during the
    //expansion phase. Then remains constant at 50% while flying.
    static PhysScalarPlanPtr alphaValuePlanPtr;
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;
        //Set up the scalar plan data etc and the alpha value plan
        PhysLinearScalarPlan::ScalarVec times;
        times.reserve( 3 );
        times.push_back( expandTime );
        times.push_back( expandTime + 0.01 );
        times.push_back( 100.0 );

        PhysLinearScalarPlan::ScalarVec values;
        values.reserve( 4 );
        values.push_back( 0.0 );
        values.push_back( 1.0 );
        values.push_back( 0.5 );
        values.push_back( 0.5 );

        alphaValuePlanPtr = _NEW( PhysLinearScalarPlan( times, values ) );
    }

    uint nMaterialsInVector = 1;
    W4dLOD maxLOD = 3;
    W4dMaterialPlanPtr materialPlanPtr =
        _NEW( W4dSimpleAlphaPlan( orbMaterial( race ), nMaterialsInVector,
                                  alphaValuePlanPtr, maxLOD ) );

    discEntityPlan.materialPlan( materialPlanPtr, startTime );

    //Set up the transform plan using standard function
    PhysRelativeTime duration = move( startTime + expandTime, data ) + expandTime;

    return duration;
}

//static
const RenMaterial& MachPhysTreacheryOrb::orbMaterial( MachPhys::Race race )
{
    const RenMaterial* pMat = NULL;

    switch( race )
    {
        case MachPhys::RED:
        {
            static RenMaterial mat( createOrbMaterial( race ) );
            pMat = &mat;
            break;
        }
        case MachPhys::BLUE:
        {
            static RenMaterial mat( createOrbMaterial( race ) );
            pMat = &mat;
            break;
        }
        case MachPhys::GREEN:
        {
            static RenMaterial mat( createOrbMaterial( race ) );
            pMat = &mat;
            break;
        }
        case MachPhys::YELLOW:
        {
            static RenMaterial mat( createOrbMaterial( race ) );
            pMat = &mat;
            break;
        }
        DEFAULT_ASSERT_BAD_CASE( race );
    }

    return *pMat;
}

//static
RenMaterial MachPhysTreacheryOrb::createOrbMaterial( MachPhys::Race race )
{
    //Create a material with black diffuse colour, and emissive race colour
    RenMaterial mat( RenColour::black() );
    mat.emissive( MachPhysRaceChanger::instance().raceColour( race ) );

    //Give it the cutout texture
    mat.texture( MachPhysRadialDisc::circleTexture() );

    return mat;
}

PhysRelativeTime MachPhysTreacheryOrb::suckRaceAt
(
    const PhysAbsoluteTime& destroyTime, MachPhys::Race oldRace
)
{
    //get the entity plans for the projectile and the child disc
    W4dEntityPlan& entityPlan = entityPlanForEdit();
    W4dEntityPlan& discEntityPlan = pDisc_->entityPlanForEdit();

    //See where the disc will be at the destruct time
    MexTransform3d stopPosition = localTransform();
    if( entityPlan.hasMotionPlan() )
    {
        uint nObsolete;
        entityPlan.transform( destroyTime, &stopPosition, &nObsolete );
    }

    //Set up a motion plan that doesn't move at the stop time at the stop position
    PhysMotionPlanPtr motionPlanPtr =
        _NEW( PhysLinearMotionPlan( stopPosition, stopPosition, 1.0 ) );

    entityPlan.absoluteMotion( motionPlanPtr, destroyTime );

    //Set a visibility plan
    const PhysRelativeTime expandTime = 0.5;
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
    visibilityPlanPtr->add( false, expandTime );
    entityPlan.visibilityPlan( visibilityPlanPtr, destroyTime );

    //Set up a scale plan for the disc
    static W4dScalePlanPtr scalePlanPtr =
        _NEW( W4dSimpleUniformScalePlan( 2.0, 5.0, expandTime ) );

    discEntityPlan.scalePlan( scalePlanPtr, destroyTime );

    //Set up a material plan for the disc. It fades up from 50% to 0%
    static PhysScalarPlanPtr alphaValuePlanPtr;
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;
        //Set up the scalar plan data etc and the alpha value plan
        PhysLinearScalarPlan::ScalarVec times;
        times.reserve( 1 );
        times.push_back( expandTime );

        PhysLinearScalarPlan::ScalarVec values;
        values.reserve( 2 );
        values.push_back( 1.0 );
        values.push_back( 0.0 );

        alphaValuePlanPtr = _NEW( PhysLinearScalarPlan( times, values ) );
    }

    uint nMaterialsInVector = 1;
    W4dLOD maxLOD = 3;
    W4dMaterialPlanPtr materialPlanPtr =
        _NEW( W4dSimpleAlphaPlan( orbMaterial( oldRace ), nMaterialsInVector,
                                  alphaValuePlanPtr, maxLOD ) );

    discEntityPlan.materialPlan( materialPlanPtr, destroyTime );

    return expandTime;
}

void perWrite( PerOstream& ostr, const MachPhysTreacheryOrb& orb )
{
    const MachPhysLinearProjectile& base = orb;

    ostr << base;
    ostr << orb.pDisc_;
}

void perRead( PerIstream& istr, MachPhysTreacheryOrb& orb )
{
    MachPhysLinearProjectile& base = orb;

    istr >> base;
    istr >> orb.pDisc_;
}

/* End TRECHORB.CPP *************************************************/

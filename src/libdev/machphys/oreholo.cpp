/*
 * O R E H O L O . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "mathex/point2d.hpp"
#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"

#include "phys/rampacce.hpp"
#include "phys/motplan.hpp"
#include "phys/timeangl.hpp"

#include "world4d/entyplan.hpp"
#include "world4d/link.hpp"
#include "world4d/composit.hpp"
#include "world4d/root.hpp"
#include "world4d/soundman.hpp"

#include "sim/manager.hpp"

#include "machphys/machphys.hpp"
#include "machphys/matconv.hpp"
#include "machphys/oreholo.hpp"
#include "machphys/snddata.hpp"
#include "machphys/private/otherper.hpp"

#include "system/pathname.hpp"

//////////////////////////////////////////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT( MachPhysOreHolograph );

MachPhysOreHolograph::MachPhysOreHolograph
(
    W4dEntity* pParent, const W4dTransform3d& localTransform,
    MachPhys::Race race, uint /*concentration*/, uint /*quantity*/
)
:   W4dComposite( factory(), pParent, localTransform )
{
    //Apply the spin plan
    plan( *spinPlan(), SimManager::instance().currentTime(), 1000000 );

    if( race != MachPhys::RED )
        convertMaterials( race );

    PhysAbsoluteTime time = PhysAbsoluteTime(0);
	W4dSoundManager::instance().play( this, SID_OREHOLO,
	                                  time, 0 );
//    W4dSoundManager::instance().play( this, SysPathName( "sounds/mincone.wav" ),
//                                      PhysAbsoluteTime( 0 ), 100.0, 5.0,
//                                      W4dSoundManager::LOOP_CONTINUOUSLY );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhysOreHolograph::~MachPhysOreHolograph()
{
    TEST_INVARIANT;

    W4dSoundManager::instance().stop( this );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysOreHolograph::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

//static
const MachPhysOreHolograph& MachPhysOreHolograph::factory()
{
    //static W4dRoot root( 1 );
    //static MachPhysOreHolograph holo( &root, MexTransform3d::MexTransform3d() );

    //return holo;
    return MachPhysOtherPersistence::instance().holographExemplar();
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhysOreHolograph::MachPhysOreHolograph( W4dEntity* pParent, const MexTransform3d& transform )
:   W4dComposite( pParent, transform, SOLID )
{
    //Construct using the composite file
    SysPathName cdfFile( "models/holo/level1/holo.cdf" );
    ASSERT_FILE_EXISTS( cdfFile.pathname().c_str() );
    readCompositeFile( cdfFile );
}
//////////////////////////////////////////////////////////////////////////////////////////

//static
W4dCompositePlanPtr MachPhysOreHolograph::newSpinPlan( const MachPhysOreHolograph& holo )
{
    //Find the links
    W4dLink* pMineralLink;
    bool linkFound = holo.findLink( "Hmineral", &pMineralLink );
    ASSERT( linkFound, "No Hmineral link in holograph" );

// No satellites    W4dLink* pSatellitesLink;
// No satellites    linkFound = holo.findLink( "HSatells", &pSatellitesLink );
// No satellites    ASSERT( linkFound, "No HSatells link in holograph" );

    //Get the link locations
    MexPoint3d mineralPos( pMineralLink->localTransform().position() );
// No satellites    MexPoint3d satellitesPos( pSatellitesLink->localTransform().position() );

    //Construct a timed angle plan for the bits
    PhysMotionPlan::Angles* pMineralAngles = _NEW( PhysMotionPlan::Angles );
    pMineralAngles->reserve( 4 );
// No satellites        PhysMotionPlan::Angles* pSatellitesAngles = _NEW( PhysMotionPlan::Angles );
    for( MATHEX_SCALAR angle = 0; angle <= 360; angle += 120 )
    {
        pMineralAngles->push_back( MexDegrees( angle ) );
// No satellites            pSatellitesAngles->push_back( MexDegrees( -angle ) );
    }

    PhysMotionPlan::Times* pMineralTimes = _NEW( PhysMotionPlan::Times );
    pMineralTimes->reserve( 3 );
// No satellites        PhysMotionPlan::Times* pSatellitesTimes = _NEW( PhysMotionPlan::Times );
    for( PhysRelativeTime time = 1; time < 4; time += 1 )
    {
        pMineralTimes->push_back( time * 2 );
// No satellites            pSatellitesTimes->push_back( time );
    }

    PhysTimedAnglePlan* pMineralPlan =
        _NEW( PhysTimedAnglePlan( PhysMotionPlan::AnglesPtr( pMineralAngles ),
                        PhysMotionPlan::TimesPtr( pMineralTimes ),
                        MexVec3(0,0,1), mineralPos ) );

// No satellites        PhysTimedAnglePlan* pSatellitesPlan =
// No satellites            _NEW( PhysTimedAnglePlan( PhysMotionPlan::AnglesPtr( pSatellitesAngles ),
// No satellites                            PhysMotionPlan::TimesPtr( pSatellitesTimes ),
// No satellites                            MexVec3(0,0,1), satellitesPos ) );

    //Hence make a composite plan
    PhysMotionPlanPtr mineralPlanPtr( pMineralPlan );
// No satellites        PhysMotionPlanPtr satellitesPlanPtr( pSatellitesPlan );

    W4dEntityPlan mineralLinkPlan;
// No satellites        W4dEntityPlan satellitesLinkPlan;

    mineralLinkPlan.absoluteMotion( mineralPlanPtr, 0 );
// No satellites        satellitesLinkPlan.absoluteMotion( satellitesPlanPtr, 0 );

    W4dCompositePlan* pHoloPlan = _NEW( W4dCompositePlan( "holo" ) );
    pHoloPlan->linkPlan( pMineralLink->id(), mineralLinkPlan );
// No satellites        pHoloPlan->linkPlan( pSatellitesLink->id(), satellitesLinkPlan );

    return W4dCompositePlanPtr( pHoloPlan );
}
//////////////////////////////////////////////////////////////////////////////////////////

//static
const W4dCompositePlanPtr& MachPhysOreHolograph::spinPlan()
{
    static W4dCompositePlanPtr planPtr( newSpinPlan( factory() ) );
    return planPtr;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysOreHolograph::convertMaterials( MachPhys::Race race )
{
    //  Set the override materials correctly to get
    //  the correct colour for this machine race.
    MachPhysMaterialConverter   converter( MachPhys::RED, race );
	converter.convert(this);
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachPhysOreHolograph::intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                          Accuracy accuracy ) const
{
    //Just check against bounding volume
    return defaultCompositeIntersectsLine( line, pDistance, accuracy );
}

MachPhysOreHolograph::MachPhysOreHolograph( PerConstructor con )
: W4dComposite( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysOreHolograph& holo )
{
    const W4dComposite& base = holo;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysOreHolograph& holo)
{
    W4dComposite& base = holo;

    istr >> base;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End OREHOLO.CPP **************************************************/

/*
 * C H A R G E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/charger.hpp"
#include "machphys/machine.hpp"
#include "machphys/healhelx.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/constron.hpp"
#include "machphys/snddata.hpp"

#include "sim/manager.hpp"
#include "world4d/root.hpp"
#include "world4d/objtrack.hpp"
#include "world4d/obtrmpln.hpp"
#include "world4d/obtrspln.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/soundman.hpp"
#include "world4d/light.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/abox3d.hpp"
#include "system/pathname.hpp"

#include "render/colour.hpp"

PER_DEFINE_PERSISTENT( MachPhysSuperCharger );

MachPhysSuperCharger::MachPhysSuperCharger
(
    W4dEntity* pParent, const MexTransform3d& localTransform,
    MachPhys::WeaponType type, MachPhys::Mounting mounting
)
:  MachPhysWeapon( exemplar( type ), mounting, pParent, localTransform ),
    pHealHelix_( NULL ),
	pLight_( NULL )
{
    TEST_INVARIANT;
}

//One-time ctor
MachPhysSuperCharger::MachPhysSuperCharger( MachPhys::WeaponType type )
:   MachPhysWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                    SysPathName( compositeFilePath( type ) ), type, MachPhys::LEFT ),
    pHealHelix_( NULL ),
	pLight_( NULL )
{
    TEST_INVARIANT;
}

MachPhysSuperCharger::~MachPhysSuperCharger()
{
    TEST_INVARIANT;
}

void MachPhysSuperCharger::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysSuperCharger& t )
{

    o << "MachPhysSuperCharger " << (void*)&t << " start" << std::endl;
    o << "MachPhysSuperCharger " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const MachPhysSuperCharger& MachPhysSuperCharger::exemplar( MachPhys::WeaponType type )
{
    return MachPhysWeaponPersistence::instance().superChargerExemplar(type);
}

//static
const char* MachPhysSuperCharger::compositeFilePath( MachPhys::WeaponType type )
{
    switch( type )
    {
        case MachPhys::SUPERCHARGE_ADVANCED:
            return "models/weapons/chargea/chargea.cdf";
        case MachPhys::SUPERCHARGE_SUPER:
            return "models/weapons/charges/charges.cdf";
    }

    return "";
}

void MachPhysSuperCharger::startHealing
(
    const MachPhysMachine& machine, const PhysAbsoluteTime& startTime,
    const PhysRelativeTime& duration
)
{
    //cancel any existing animation
    stopHealing();

    //Construct a heal helix attached to the weapon
    pHealHelix_ = _NEW( MachPhysHealHelix( this, MexTransform3d() ) );

    //Get useful data
    const MachPhysWeaponData& data = weaponData();
    MexPoint3d localOrigin = data.launchOffsets()[0];
    static MATHEX_SCALAR defaultLength = 5.0;
    MATHEX_SCALAR helixMaxLength = std::max( defaultLength, data.range() );

    //Get centroid of target machine
    const MexAlignedBox3d& targetBoundary = machine.compositeBoundingVolume();
    const MexPoint3d& minCorner = targetBoundary.minCorner();
    const MexPoint3d& maxCorner = targetBoundary.maxCorner();
    MexPoint3d targetOffset( (minCorner.x() + maxCorner.x()) * 0.5,
                             (minCorner.y() + maxCorner.y()) * 0.5,
                             (minCorner.z() + maxCorner.z()) * 0.5 );

    //Set up max tracking angles allowed
    static MATHEX_SCALAR maxAbsYawSine = 0.5; //30 degrees
    static MATHEX_SCALAR maxAbsPitchSine = 0.5; //30 degrees
    static MATHEX_SCALAR minLength = 1.0;

    //Set up visibility plan
    W4dEntityPlan& healHealixEntityPlan = pHealHelix_->entityPlanForEdit();
    pHealHelix_->visible( false );

    W4dVisibilityPlan* pVisibilityPlan = _NEW( W4dVisibilityPlan( true ) );
    pVisibilityPlan->add( false, duration );

    W4dVisibilityPlanPtr visibilityPlanPtr( pVisibilityPlan );
    healHealixEntityPlan.visibilityPlan( visibilityPlanPtr, startTime );

    //Set up an object tracker to make the heal helix try and reach the target machine
    W4dObjectTracker* pTracker =
        _NEW( W4dObjectTracker( *this, localOrigin, machine, targetOffset,
                                maxAbsYawSine, maxAbsPitchSine,
                                minLength, helixMaxLength, defaultLength ) );

    W4dObjectTrackerPtr trackerPtr( pTracker );

    //Apply a transform plan to the helix using the tracker
    W4dObjectTrackerMotionPlan* pMotionPlan =
        _NEW( W4dObjectTrackerMotionPlan( trackerPtr, duration ) );

    PhysMotionPlanPtr motionPlanPtr( pMotionPlan );
    healHealixEntityPlan.absoluteMotion( motionPlanPtr, startTime );

    //Apply a scale plan also using the tracker
    W4dObjectTrackerScalePlan* pScalePlan =
        _NEW( W4dObjectTrackerScalePlan( trackerPtr, duration ) );
    W4dScalePlanPtr scalePlanPtr( pScalePlan );

    pHealHelix_->propogateScalePlan( scalePlanPtr, startTime );

    //Set the helix to be garbage collected when finished
    helixDestructTime_ = startTime + duration;
    W4dGarbageCollector::instance().add( pHealHelix_, helixDestructTime_ );

    //Play the sound
    W4dSoundManager::instance().play( this, SID_SUPERHEAL,
                                      startTime, 0 );
//    W4dSoundManager::instance().play( this, SysPathName( "sounds/suprheal.wav" ), startTime,
//                                      100.0, 20.0, W4dSoundManager::LOOP_CONTINUOUSLY );

    //TBD: The light needs to be destructed in stopHealing - not working on the duration.
	//shine the light
	lighting(startTime, duration);
}

void MachPhysSuperCharger::stopHealing()
{

    //delete any existing helix, unless past garbage collect time
    if( pHealHelix_ )
    {
        if( SimManager::instance().currentTime() < helixDestructTime_ )
            _DELETE( pHealHelix_ );
        pHealHelix_ = NULL;
    }
	//delete existing light, unless past garbage collect time
    if(  pLight_ )
    {
        if( SimManager::instance().currentTime() < helixDestructTime_ )
			_DELETE( pLight_ );
		pLight_ = NULL;
    }

    //Stop any sound
    W4dSoundManager::instance().stop( this );
}

void MachPhysSuperCharger::lighting(const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration)
{
    const MachPhysWeaponData& data = weaponData();
    MexPoint3d offSet = data.launchOffsets()[0];
	MATHEX_SCALAR range = 3;

	pLight_ = _NEW(W4dPointLight(this, MexVec3(1, 0, 0), range));
    pLight_->localTransform( MexTransform3d(offSet) );
	pLight_->colour(RenColour::green());
	pLight_->maxRange(range);
	pLight_->constantAttenuation(0);
	pLight_->linearAttenuation(0.7);
	pLight_->quadraticAttenuation(0.3);
	pLight_->scope(W4dLight::LOCAL);

	//visibility plan
    W4dVisibilityPlanPtr lightVisibilityPlanPtr( _NEW( W4dVisibilityPlan( false ) ) );
	PhysRelativeTime roundOnTime = 0.7;
	uint nRounds = duration;

	for(int i = 1; i <= nRounds; ++i)
	{
		PhysRelativeTime onTime = i - roundOnTime;

		lightVisibilityPlanPtr->add( true, onTime );
		lightVisibilityPlanPtr->add( false, i );
	}

	//apply the visibility plan
    pLight_->entityPlanForEdit().visibilityPlan( lightVisibilityPlanPtr, startTime );

	//attach the entity to the light
	if(hasMachine())
		pLight_->illuminate(&machine());

	if(hasConstruction())
		pLight_->illuminate(&construction());

    W4dGarbageCollector::instance().add( pLight_, helixDestructTime_ );
}

MachPhysSuperCharger::MachPhysSuperCharger( PerConstructor con )
: MachPhysWeapon( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysSuperCharger& weapon )
{
    const MachPhysWeapon& base = weapon;

    ostr << base;
    ostr << weapon.pHealHelix_;
    ostr << weapon.helixDestructTime_;
    ostr << weapon.pLight_;

}

void perRead( PerIstream& istr, MachPhysSuperCharger& weapon )
{
    MachPhysWeapon& base = weapon;

    istr >> base;
    istr >> weapon.pHealHelix_;
    istr >> weapon.helixDestructTime_;
    istr >> weapon.pLight_;
}

// forced recompile 5/1/99 CPS

/* End CHARGER.CPP **************************************************/

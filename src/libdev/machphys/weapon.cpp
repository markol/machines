/*
 * W E A P O N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/weapon.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/racechng.hpp"
#include "machphys/machine.hpp"
#include "machphys/constron.hpp"
#include "machphys/light.hpp"
#include "machphys/attack.hpp"

#include "world4d/manager.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/light.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/soundman.hpp"
#include "world4d/mvecccha.hpp"
#include "world4d/root.hpp"
#include "world4d/link.hpp"

#include "mathex/transf3d.hpp"

#include "phys/linemoti.hpp"

#ifndef _INLINE
    #include "machphys/weapon.ipp"
#endif

PER_DEFINE_PERSISTENT( MachPhysWeapon );

MachPhysWeapon::MachPhysWeapon
(
    W4dEntity* pParent, const MexTransform3d& localTransform,
    const SysPathName& compositeFileName,
    MachPhys::WeaponType type, MachPhys::Mounting mounting
)
:   W4dComposite( pParent, localTransform, W4dEntity::SOLID ),
    type_( type ),
    mounting_( mounting ),
    recoilId_( 0 ),
    nextLaunchOffset_( 0 ),
    pMachine_( NULL ),
    pConstruction_( NULL )
{
    //Load the model
    readCompositeFile( compositeFileName );

    //If there is a recoil distance, look for the link named "recoil"
    if( weaponData().recoilDistance() != 0.0 )
    {
        W4dLink* pRecoilLink;
        if( findLink( "recoil", &pRecoilLink ) )
        {
            //Store link id plus 2
            recoilId_ = pRecoilLink->id() + 2;
        }
        else
        {
            //Indicate whole weapon to recoil
            recoilId_ = 1;
        }
    }
    TEST_INVARIANT;
}

MachPhysWeapon::MachPhysWeapon
(
    const MachPhysWeapon& copyMe, MachPhys::Mounting mounting,
    W4dEntity* pParent, const MexTransform3d& localTransform
)
:   W4dComposite( copyMe, pParent, localTransform ),
    type_( copyMe.type_ ),
    mounting_( mounting ),
    recoilId_( copyMe.recoilId_ ),
    nextLaunchOffset_( 0 ),
    pMachine_( NULL ),
    pConstruction_( NULL )
{
    TEST_INVARIANT;
}

MachPhysWeapon::MachPhysWeapon( PerConstructor con )
: W4dComposite( con )
{
    TEST_INVARIANT;
}

MachPhysWeapon::~MachPhysWeapon()
{
    TEST_INVARIANT;
    //Stop any playing sound
    W4dSoundManager::instance().stop( this );
}

void MachPhysWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysWeapon& t )
{

    o << "MachPhysWeapon " << (void*)&t << " start" << std::endl;
    o << "MachPhysWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysWeapon::intersectsLine
(
    const MexLine3d& line, MATHEX_SCALAR* pDistance, Accuracy accuracy
) const
{
    //use the default function
    return defaultIntersectsLine( line, pDistance, accuracy );
}

const MachPhysWeaponData& MachPhysWeapon::weaponData() const
{
    return MachPhysData::instance().weaponData( type() );
}

//virtual
PhysRelativeTime MachPhysWeapon::victimAnimation
(
    const PhysAbsoluteTime&, const MexLine3d&, MachPhysMachine*
) const
{
    //No default animation
    return 0.0;
}

//virtual
PhysRelativeTime MachPhysWeapon::victimAnimation
(
    const PhysAbsoluteTime&, const MexLine3d&, MachPhysConstruction*
) const
{
    //No default animation
    return 0.0;
}

uint MachPhysWeapon::nLaunchOffsets() const
{
    return weaponData().launchOffsets().size();
}

const MexPoint3d& MachPhysWeapon::localLaunchOffset( uint index ) const
{
    PRE( index < nLaunchOffsets() );
    return weaponData().launchOffsets()[ index ];
}

void MachPhysWeapon::globalLaunchPosition( uint index, MexTransform3d* pTransform ) const
{
    //Construct a translation only transform from the local launch offset
    MexTransform3d offset( localLaunchOffset( index ) );

    //Combine this with weapon global transform
    globalTransform().transform( offset, pTransform );
}

//virtual
PhysRelativeTime MachPhysWeapon::fire( const PhysAbsoluteTime&, int )
{
    //Do nothing
    return 0.0;
}

PhysRelativeTime MachPhysWeapon::recoil( const PhysAbsoluteTime& startTime, int numberInBurst )
{
    PhysRelativeTime duration;

    //Check there is a recoil defined
    if( recoilId_ != 0 )
    {
        //get the data
        const MachPhysWeaponData& data = weaponData();
        MATHEX_SCALAR distance = data.recoilDistance();
        PhysRelativeTime backTime = data.recoilBackTime();
        PhysRelativeTime foreTime = data.recoilForeTime();
        PhysRelativeTime burstDuration = data.burstDuration();
        uint nRounds = data.nRoundsPerBurst();
        duration = burstDuration;

        //Compute the spare time after recoil to start of next projectile
        PhysRelativeTime roundTime = burstDuration / nRounds;
        PhysRelativeTime spareTime = roundTime - (backTime + foreTime);
        if( spareTime < 0.01 )
        {

            ASSERT( spareTime > -0.01, "Inconsistent recoil/burst times" );
            //Modify times to eliminate spare time
            spareTime = 0.0;
            foreTime = roundTime - backTime;
        }

        //Get the required entity
        //W4dEntity* pRecoilEntity = (recoilId_ == 1 ? this : links()[recoilId_ - 2]);
        W4dEntity* pRecoilEntity =
          (recoilId_ == 1 ? this : _STATIC_CAST(W4dEntity*, links()[recoilId_ - 2]) );

        //Set up the motion plan for a single burst/recoil
        MexTransform3d startPos = pRecoilEntity->localTransform();
        MexTransform3d shift( MexPoint3d (-distance,0.0,0.0) );
        MexTransform3d backPos;
        startPos.transform( shift, &backPos );

        PhysLinearMotionPlan* pPlan =
            _NEW( PhysLinearMotionPlan( startPos, backPos, backTime ) );
        pPlan->add( startPos, backTime + foreTime );
        pPlan->add( startPos, roundTime );

        PhysMotionPlanPtr planPtr( pPlan );

        pRecoilEntity->entityPlanForEdit().absoluteMotion( planPtr, startTime, numberInBurst - 1 );
    }
    else
    {
        //No recoil
        duration = 0.0;
    }

    return duration;
}

void MachPhysWeapon::nextLaunchOffset( uint index )
{
    nextLaunchOffset_ = (index % weaponData().launchOffsets().size());
}

void perWrite( PerOstream& ostr, const MachPhysWeapon& weapon )
{
    const W4dComposite& base = weapon;

    ostr << base;

    ostr << weapon.type_;
    ostr << weapon.mounting_;
    ostr << weapon.recoilId_;
    ostr << weapon.nextLaunchOffset_;
    ostr << weapon.pMachine_;
    ostr << weapon.pConstruction_;
}

void perRead( PerIstream& istr, MachPhysWeapon& weapon )
{
    W4dComposite& base = weapon;

    istr >> base;

    istr >> weapon.type_;
    istr >> weapon.mounting_;
    istr >> weapon.recoilId_;
    istr >> weapon.nextLaunchOffset_;
    istr >> weapon.pMachine_;
    istr >> weapon.pConstruction_;
}

void MachPhysWeapon::lighting(const RenColour& col, const PhysAbsoluteTime& startTime, const MATHEX_SCALAR distance, MachPhysWeapon::Shadow shadow)
{
    const MachPhysWeaponData& data = weaponData();
    uint nRounds = data.nRoundsPerBurst();
    PhysRelativeTime burstDuration = data.burstDuration();
    MexPoint3d offSet = data.launchOffsets()[0];

	//W4dUniformLight* pLight = _NEW(W4dUniformLight(this, MexVec3(1, 0, 0), distance));
	W4dPointLight* pLight = _NEW(W4dPointLight(this, MexVec3(1, 0, 0), distance));

    pLight->localTransform( MexTransform3d(offSet) );
	pLight->colour(col);
	pLight->maxRange(distance);
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.7);
	pLight->quadraticAttenuation(0.3);
	pLight->scope(W4dLight::LOCAL);
	pLight->visible(true);


	//visibility plan
    W4dVisibilityPlanPtr lightVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	PhysRelativeTime roundTime = burstDuration/nRounds;
	PhysRelativeTime roundOffTime = 0.75*roundTime;
	PhysRelativeTime roundOnTime = 0.25*roundTime;

	for(int i = 1; i < nRounds; ++i)
	{
		PhysRelativeTime offTime = roundOffTime*i;
		lightVisibilityPlanPtr->add( false, offTime );

		PhysRelativeTime onTime = offTime + roundOnTime;
		lightVisibilityPlanPtr->add( true, onTime );
	}

	lightVisibilityPlanPtr->add( false, burstDuration - roundOnTime );

	//apply the visibility plan
    pLight->entityPlanForEdit().visibilityPlan( lightVisibilityPlanPtr, startTime );

	//attach the entity to the light
	W4dEntity* pParent = NULL;

	if(hasMachine())
	{
		pLight->illuminate(&machine());
		pParent = &machine();
	}

	if(hasConstruction())
	{
		pLight->illuminate(&construction());
		pParent = &construction();
	}

	//also create a light shadow on the ground if required
	if( pParent != NULL and shadow == SHADOW )
	{
		MexPoint3d shadowPositon = pLight->globalTransform().position();
				   shadowPositon.z( pParent->globalTransform().position().z() + 0.066 );  //plus the z-buffer resolution to avoid the interference with the planet surface

		(pParent->globalTransform()).transformInverse( &shadowPositon );

		MachPhysLight* pGroundLight = _NEW( MachPhysLight( pParent, MexTransform3d( shadowPositon ) ) );
	    pGroundLight->entityPlanForEdit().visibilityPlan( lightVisibilityPlanPtr, startTime );

	    W4dGarbageCollector::instance().add( pGroundLight, startTime + burstDuration );
	}

    W4dGarbageCollector::instance().add( pLight, startTime + burstDuration );
}

void MachPhysWeapon::convertMaterials()
{
    //Ensure we have a machine or construction to get race from
    bool haveRace = true;
    MachPhys::Race race;
    if( pMachine_ != NULL )
        race = pMachine_->race();
    else if( pConstruction_ != NULL )
        race = pConstruction_->race();
    else
        haveRace = false;

    //We need to convert even for RED, in case reverting back to red from treachery
    if( haveRace )
    {
        //Use the race changer to do the job
        const W4dCompositeMaterialVecChanger& changer =
            MachPhysRaceChanger::instance().changer( *this, race );

        changer.applyOverrides( this );
    }
}

bool MachPhysWeapon::firedLightRecently(PhysRelativeTime launchTime) const
{
	// Deterimine if the owning machine has fired any of it's
	// multi-weapons recently.
	MachPhysCanAttack* pCanAttack = NULL;
	if(hasMachine() && machine().hasCanAttack())
		pCanAttack = machine().canAttack();

    const MachPhysWeaponData& data = weaponData();
    const uint nRounds = data.nRoundsPerBurst();
    const PhysRelativeTime burstDuration = data.burstDuration();

	// Check for a light within the round duration.  This should give one
	// light per round, rather than burst.  Cf sounds.
    const PhysAbsoluteTime roundTime = burstDuration / nRounds;
    const PhysAbsoluteTime interval2  = 0.95 * roundTime;
	return pCanAttack && pCanAttack->hasLaunchedLightWithin(type(), interval2, launchTime);
}

bool MachPhysWeapon::playedSoundRecently(PhysRelativeTime launchTime) const
{
	// Deterimine if the owning machine has fired any of it's
	// multi-weapons recently.
	MachPhysCanAttack* pCanAttack = NULL;
	if(hasMachine() && machine().hasCanAttack())
		pCanAttack = machine().canAttack();

    const MachPhysWeaponData& data = weaponData();
    const uint nRounds = data.nRoundsPerBurst();
    const PhysRelativeTime burstDuration = data.burstDuration();

	// Check for a sound within the burst duration.  This should give one
	// sound per burst.
    const PhysAbsoluteTime interval = 0.95 * burstDuration;
	return pCanAttack && pCanAttack->hasPlayedSoundWithin(type(), interval, launchTime);
}

void MachPhysWeapon::changeRace()
{
	convertMaterials();
}

void MachPhysWeapon::changeColour( MachPhys::Race newRace )
{
    //Use the race changer to do the job
    const W4dCompositeMaterialVecChanger& changer =
        MachPhysRaceChanger::instance().changer( *this, newRace );

    changer.applyOverrides( this );
}

/* End WEAPON.CPP ***************************************************/

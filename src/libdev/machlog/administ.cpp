/*
 * A D M I N I S T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include <iostream>
#include "stdlib/string.hpp"

#include "ctl/algorith.hpp"
#include "ctl/list.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp"
#include "world4d/domain.hpp"

#include "machphys/machphys.hpp"
#include "machphys/administ.hpp"
#include "machphys/admndata.hpp"
#include "machphys/weapon.hpp"

#include "machlog/administ.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/weapon.hpp"
#include "machlog/armourer.hpp"
#include "machlog/canattac.hpp"
#include "machlog/charger.hpp"
#include "machlog/constron.hpp"
#include "machlog/mcmotseq.hpp"
//#include "machlog/cntrl.hpp"
//#include "machlog/cntrl_ai.hpp"

PER_DEFINE_PERSISTENT( MachLogAdministrator );
/* //////////////////////////////////////////////////////////////// */

MachLogAdministrator::MachLogAdministrator( const MachPhys::AdministratorSubType& subType,
									MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location, MachPhys::WeaponCombo wc )
: MachLogMachine( MachPhys::ADMINISTRATOR, hwLevel, swLevel, pRace,
                  pNewPhysAdministrator( subType, hwLevel, swLevel, pRace, location, wc ),
                  MachLog::ADMINISTRATOR ),
  MachLogCanAdminister( pRace->race() ),
  MachLogCanAttack( this, &physAdministrator(), wc ),
  subType_( subType )
{
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().administrators( pRace->race() ), this );
	MachLogArmourer::arm( this );
}

MachLogAdministrator::MachLogAdministrator( const MachPhys::AdministratorSubType& subType,
									MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location, MachPhys::WeaponCombo wc, UtlId withId )
: MachLogMachine( MachPhys::ADMINISTRATOR, hwLevel, swLevel, pRace,
                  pNewPhysAdministrator( subType, hwLevel, swLevel, pRace, location, wc ),
                  MachLog::ADMINISTRATOR, withId ),
  MachLogCanAdminister( pRace->race() ),
  MachLogCanAttack( this, &physAdministrator(), wc ),
  subType_( subType )
{
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().administrators( pRace->race() ), this );
	MachLogArmourer::arm( this );
}

MachLogAdministrator::~MachLogAdministrator()
{

}

PhysRelativeTime MachLogAdministrator::update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR junk )
{
	MachLogCanAttack::update();

	//Do the main work
	DEBUG_STREAM( DIAG_HAL,"(" << id() << ") MLAdministrator::update\n" );

	if( not( isDead() or isDying() or evading() or insideAPC() )
		and not MachLogRaces::instance().inSpecialActorUpdate()
		and willCheckForTargets() )
		checkAndAttackCloserTarget( this );

	diminishAlertnessAndInaccuracy();

	// if moving, maintain medium alert
	if( motionSeq().hasDestination() )
	{
		setMinimumAlertness( 45 );
	}

	//PhysRelativeTime alertnessResponse = ( 4.0 - ( _CONST_CAST( PhysRelativeTime, alertness() ) / 30.0 ) );
	PhysRelativeTime alertnessResponse = ( 4.0 - (  alertness()  / 30.0 ) );
	PhysRelativeTime result = std::min( MachLogMachine::update( maxCPUTime, junk ), alertnessResponse );

	DEBUG_STREAM( DIAG_HAL,"(" << id() << ") MLAdministrator::update exit\n" );
	return result;
}


/* //////////////////////////////////////////////////////////////// */

void MachLogAdministrator::doOutputOperator( ostream& o ) const
{
	MachLogMachine::doOutputOperator( o );
}

/* //////////////////////////////////////////////////////////////// */
//static
MachPhysAdministrator* MachLogAdministrator::pNewPhysAdministrator
(
    const MachPhys::AdministratorSubType& subType,
    Level hwLevel, Level swLevel,
    MachLogRace * pRace, const MexPoint3d& location,
	MachPhys::WeaponCombo wc
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, 0, &localTransform );

    //Construct the physical machine
    return _NEW( MachPhysAdministrator( pDomain, localTransform, subType, hwLevel, swLevel, pRace->race(), wc ) );
}

//virtual
const MachPhysMachineData& MachLogAdministrator::machineData() const
{
	return data();
}

const MachPhysAdministratorData& MachLogAdministrator::data() const
{
	return _STATIC_CAST( const MachPhysAdministratorData&, physMachine().machineData() );
}

/* //////////////////////////////////////////////////////////////// */

MachPhysAdministrator& MachLogAdministrator::physAdministrator()
{
	return _STATIC_CAST( MachPhysAdministrator&, physObject() );
}

const MachPhysAdministrator& MachLogAdministrator::physAdministrator() const
{
	//return _STATIC_CAST( MachPhysAdministrator&, physObject() );
	return _STATIC_CAST( MachPhysAdministrator&, _CONST_CAST( W4dEntity&, physObject() ));
}


/* //////////////////////////////////////////////////////////////// */
void MachLogAdministrator::handleIdleTechnician( MachLogCommsId /*obj*/ )
{
	;
//	logRace().controller().handleIdleTechnician( obj );
}

void MachLogAdministrator::receiveMessage( const MachLogMessage& /*msg*/ )
{
	;
//	ASSERT( false, logic_error( "shouldn't be here!" ) );
}

const MachPhys::AdministratorSubType& MachLogAdministrator::subType() const
{
	return subType_;
}

//virtual
bool MachLogAdministrator::fearsThisActor( const MachActor& otherActor ) const
{
		ASSERT( otherActor.objectIsCanAttack(), "Shouldn't be testing for fear on an actor that isn't attack-capable." );

	// automatically brick myself if he can attack me but I can't attack him,
	// otherwise, only turn tail and run if the other actor is at least 200% stronger than me and my nearby mates
	// put together

	return( 	otherActor.asCanAttack().canFireAt( *this ) and not( canFireAt( otherActor ) )
			or	otherActor.militaryStrength() > ( localStrength() * 2) );
}

//virtual
int MachLogAdministrator::militaryStrength() const
{
	return _STATIC_CAST( int, hpRatio() * _STATIC_CAST( MATHEX_SCALAR, objectData().cost() ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachLogAdministrator::beHit( const int& damage, MachPhys::WeaponType byType,
						MachActor* pByActor, MexLine3d* pByDirection, MachActor::EchoBeHit echo )
{
	int hpBeforeImpact = hp();
	MachLogMachine::beHit( damage, byType, pByActor, pByDirection, echo );
	int hpAfterImpact = hp();

	MATHEX_SCALAR dazing = ( (MATHEX_SCALAR)( hpBeforeImpact - hpAfterImpact ) / maximumhp() ) * 40;
	MachLogCanAttack::increaseCurrentInaccuracy( dazing );

	if( pByActor
		and allowedToCheckForNewTarget()
		and not isDead()
		and not evading()
		and not isIn1stPersonView()
		and canFireAt( *pByActor )
		and willTestHitByCandidate( *pByActor ) )
	{
		MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( race(), pByActor->race() );

		// only take a pop at this schmo if we don't think it's a friend (same race or ally),
		if( disposition == MachLogRaces::NEUTRAL or disposition == MachLogRaces::ENEMY )
		{
			// will try to initiate a counterattack against the actor who hit me.
			checkAndAttackCloserTarget( this, pByActor );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// virtual
void MachLogAdministrator::beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo )
{
	int hpBeforeImpact = hp();
	MachLogMachine::beHitWithoutAnimation( damage, physicalTimeDelay, pByActor, echo );
	int hpAfterImpact = hp();

	MATHEX_SCALAR dazing = ( (MATHEX_SCALAR)( hpBeforeImpact - hpAfterImpact ) / maximumhp() ) / 4.0;
	MachLogCanAttack::increaseCurrentInaccuracy( dazing );

	if( pByActor
		and allowedToCheckForNewTarget()
		and not isDead()
		and not evading()
		and not isIn1stPersonView()
		and canFireAt( *pByActor )
		and willTestHitByCandidate( *pByActor ) )
	{
		MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( race(), pByActor->race() );

		// only take a pop at this schmo if we don't think it's a friend (same race or ally),
		if( disposition == MachLogRaces::NEUTRAL or disposition == MachLogRaces::ENEMY )
		{
			// will try to initiate a counterattack against the actor who hit me.
			checkAndAttackCloserTarget( this, pByActor );
		}
	}
}

/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogAdministrator& actor )
{
	const MachLogMachine& base1 = actor;
	const MachLogCanAttack& base2 = actor;
	const MachLogCanAdminister& base3 = actor;

	ostr << base1;
	ostr << base2;
	ostr << base3;

	ostr << actor.subType_;
	MexTransform3d trans = actor.globalTransform();

    //If we don't do this, the large coords of the transform cause are used when read back in to find
    //a planet domain, which of course falls over big time.
    if( actor.insideAPC() )
        trans.position( MexPoint3d(10,10,0) );

	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogAdministrator& actor )
{
	MachLogMachine& base1 = actor;
	MachLogCanAttack& base2 = actor;
	MachLogCanAdminister& base3 = actor;

	istr >> base1;
	istr >> base2;
	istr >> base3;

	istr >> actor.subType_;

	MexTransform3d trans;
	W4dId id;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );

	MachPhysAdministrator* pPhysAdministrator = MachLogAdministrator::pNewPhysAdministrator( actor.subType_, base1.hwLevel(), base1.swLevel(), &actor.logRace(), trans.position(), base2.weaponCombo() );
	actor.setObjectPtr( pPhysAdministrator, trans );
	actor.setPhysCanAttack( pPhysAdministrator );
	actor.id( id );
	MachLogArmourer::rearm( &actor );

	actor.machineCreated();
	actor.persistenceReadStrategy( istr );
}

MachLogAdministrator::MachLogAdministrator( PerConstructor con )
:	MachLogMachine( con ),
	MachLogCanAdminister( con ),
	MachLogCanAttack( con )
{
}

// Forced recompile 16/2/99 CPS
/* //////////////////////////////////////////////////////////////// */


/* End ADMINIST.CPP *************************************************/

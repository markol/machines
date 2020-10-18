/*
 * A G G R E S S R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "stdlib/string.hpp"
#include "ctl/algorith.hpp"
#include "ctl/list.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/poly2d.hpp"
#include "phys/motchunk.hpp"
#include "world4d/domain.hpp"

#include "machphys/aggressr.hpp"
#include "machphys/plansurf.hpp"

#include "machlog/aggressr.hpp"
#include "machphys/aggrdata.hpp"
#include "machlog/canattac.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/spacial.hpp"
#include "machlog/weapon.hpp"
#include "machlog/armourer.hpp"
#include "machlog/constron.hpp"

PER_DEFINE_PERSISTENT( MachLogAggressor )

/* //////////////////////////////////////////////////////////////// */

MachLogAggressor::MachLogAggressor( const MachPhys::AggressorSubType& subType,
						Level hwLevel,  Level swLevel,
    					MachLogRace * pRace, const MexPoint3d& location,
    					MachPhys::WeaponCombo wc )
: MachLogMachine( MachPhys::AGGRESSOR, hwLevel, swLevel, pRace,
                  pNewPhysAggressor( subType, hwLevel, swLevel, pRace, location, wc ), MachLog::AGGRESSOR ),
	MachLogCanAttack( this, &physAggressor(), wc ),
	subType_( subType )
{
	//HAL_STREAM("MLAggressor::CTOR (void*)this " << (void*)this << std::endl );
	currentTarget( NULL );
// 	objectType( MachLog::AGGRESSOR );
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().aggressors( pRace->race() ), this );
	MachLogArmourer::arm( this );
}

MachLogAggressor::MachLogAggressor( const MachPhys::AggressorSubType& subType,
						Level hwLevel,  Level swLevel,
    					MachLogRace * pRace, const MexPoint3d& location,
    					MachPhys::WeaponCombo wc, UtlId withId )
: MachLogMachine( MachPhys::AGGRESSOR, hwLevel, swLevel, pRace,
                  pNewPhysAggressor( subType, hwLevel, swLevel, pRace, location, wc ), MachLog::AGGRESSOR, withId ),
	MachLogCanAttack( this, &physAggressor(), wc ),
	subType_( subType )
{
	//HAL_STREAM("MLAggressor::CTOR (void*)this " << (void*)this << std::endl );
	currentTarget( NULL );
// 	objectType( MachLog::AGGRESSOR );
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().aggressors( pRace->race() ), this );
	MachLogArmourer::arm( this );
}

/* //////////////////////////////////////////////////////////////// */

PhysRelativeTime MachLogAggressor::update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR junk )
{
	HAL_STREAM("(" << id() << ") MachLogAggressor::update\n" );

	MachLogCanAttack::update();

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

	//Do the main work
	//PhysRelativeTime alertnessResponse = ( 4.0 - ( _CONST_CAST( PhysRelativeTime, std::min( 100, alertness() ) ) / 30.0 ) );
	PhysRelativeTime alertnessResponse = ( 4.0 - ( std::min( 100, alertness() ) / 30.0 ) );
	PhysRelativeTime result = std::min( MachLogMachine::update( maxCPUTime, junk ), alertnessResponse );

	HAL_STREAM("(" << id() << ") MachLogAggressor::update DONE\n" );
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachLogAggressor::beHit( const int& damage, MachPhys::WeaponType byType,
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
void MachLogAggressor::beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo )
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

/* //////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////// */

/*
void MachLogAggressor::doActOnClassification()
{
	if( subjects().empty() )
	{
        //do nothing
	}
	else
	{
		// respond to the most important stimulus
		MachClassification c = *min_element( subjects().begin(), subjects().end() );
		MachLogMachine * pTarget = MachLogMachines::instance()[ c.subjectId() ];

		strategy().newOperation( _NEW( MachAttackOperation( this, pTarget ) ) );
	}
}
*/

/* //////////////////////////////////////////////////////////////// */

MachPhysAggressor& MachLogAggressor::physAggressor()
{
	return _STATIC_CAST( MachPhysAggressor&, physObject() );
}

const MachPhysAggressor& MachLogAggressor::physAggressor() const
{
	return _STATIC_CAST( const MachPhysAggressor&, physObject() );
}

/* //////////////////////////////////////////////////////////////// */

//static
MachPhysAggressor* MachLogAggressor::pNewPhysAggressor
(
    const MachPhys::AggressorSubType& subType, Level hwLevel, Level swLevel,
    MachLogRace * pRace, const MexPoint3d& location, MachPhys::WeaponCombo wc
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, 0, &localTransform );

    //Construct the physical machine
    return _NEW( MachPhysAggressor( pDomain, localTransform, subType, hwLevel, swLevel, pRace->race(), wc ) );
}
//virtual
const MachPhysMachineData& MachLogAggressor::machineData() const
{
	return data();
}

const MachPhysAggressorData& MachLogAggressor::data() const
{
	//return _STATIC_CAST( MachPhysAggressorData, physMachine().machineData() );
	return  _STATIC_CAST( const MachPhysAggressorData&, physMachine().machineData() ) ;
//	return physMachine().machinedata();
}

const MachPhys::AggressorSubType& MachLogAggressor::subType() const
{
	return subType_;
}

//virtual
bool MachLogAggressor::fearsThisActor( const MachActor& otherActor ) const
{
	ASSERT( otherActor.objectIsCanAttack(), "Shouldn't be testing for fear on an actor that isn't attack-capable." );

	// automatically brick myself if he can attack me but I can't attack him,
	// otherwise, only turn tail and run if the other actor is at least 200% stronger than me and my nearby mates
	// put together

	return( 	otherActor.asCanAttack().canFireAt( *this ) and not( canFireAt( otherActor ) )
			or	otherActor.militaryStrength() > ( localStrength() * 2) );

}

//virtual
int MachLogAggressor::militaryStrength() const
{
	return _STATIC_CAST( int, hpRatio() * _STATIC_CAST( MATHEX_SCALAR, objectData().cost() ) );
}



/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogAggressor& actor )
{
	const MachLogMachine& base1 = actor;
	const MachLogCanAttack& base2 = actor;

	ostr << base1;
	ostr << base2;

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

void perRead( PerIstream& istr, MachLogAggressor& actor )
{
	MachLogMachine& base1 = actor;
	MachLogCanAttack& base2 = actor;

	istr >> base1;
	istr >> base2;

	istr >> actor.subType_;

	MexTransform3d trans;
	W4dId id;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );

	MachPhysAggressor* pPhysAggressor = MachLogAggressor::pNewPhysAggressor( actor.subType_, base1.hwLevel(), base1.swLevel(), &actor.logRace(), trans.position(), base2.weaponCombo() );
	actor.setObjectPtr( pPhysAggressor, trans );
    actor.setPhysCanAttack( pPhysAggressor );
	actor.id( id );
	MachLogArmourer::rearm( &actor );
	actor.machineCreated();
	actor.persistenceReadStrategy( istr );
}

MachLogAggressor::MachLogAggressor( PerConstructor con )
:	MachLogMachine( con ),
	MachLogCanAttack( con )
{
}

bool MachLogAggressor::isEradicator() const
{
	return subType() == MachPhys::BALLISTA
		   and hwLevel() == 3
		   and weaponCombo() == MachPhys::T_VORTEX;
}

// Forced recompile 16/2/99 CPS
/* //////////////////////////////////////////////////////////////// */

/* End AGGRESSR.CPP **************************************************/

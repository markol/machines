/*
 * I O N B E A M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/ionbeam.hpp"
#include "machlog/internal/ionbeami.hpp"

#include "mathex/line3d.hpp"

#include "phys/cspace2.hpp"

#include "world4d/domain.hpp"
#include "world4d/composit.hpp"

#include "machphys/beam.hpp"
#include "machphys/ionbeam.hpp"
#include "machphys/ionweap.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/mphydata.hpp"

#include "sim/manager.hpp"

#include "machlog/actor.hpp"
#include "machlog/expandbl.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/network.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"


MachLogIonBeam::MachLogIonBeam(	MachLogRace* pRace,
								        const MexPoint3d& startPosition,
										const MachPhysWeaponData& weaponData,
								        MachActor* pOwner )
:	MachLogExpandingBlast( pRace,
						   pNewPhysIonBeam( startPosition, weaponData ), 		// <- note that physical bomb is created here
						   startPosition,
						   pOwner,
						   weaponData.extras()[0] * 2 )	// for the extra range of the singe ring
{
	CB_MachLogIonBeam_DEPIMPL();

	ASSERT( pPhysIonBeam_, "Unexpected NULL for pPhysIonBeam_!" );

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	destructionTime_ = timeNow + pPhysIonBeam_->startExplosion( timeNow, *MachLogPlanet::instance().surface() ) + 0.5;

 	firstWaveStartTime_ = timeNow;
	firstWaveFinishTime_ = timeNow;			// not implemented for now
	secondWaveStartTime_ = timeNow;
	secondWaveFinishTime_ = destructionTime_ - 1.5;
	thirdWaveStartTime_ = timeNow;
	thirdWaveFinishTime_ = destructionTime_ - 1.5;

    // Echo explosion effect across network.
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		MachLogNetwork::instance().messageBroker().sendCreateSpecialWeaponEffectMessage( startPosition,
																						 MachPhys::ION_ORBITAL_CANNON );
	}

	//set up collison data and animations

	//setUpBuildingCollisions( startPosition );

    TEST_INVARIANT;

}

//virtual
PhysAbsoluteTime MachLogIonBeam::firstWaveStartTime() const
{
	CB_MachLogIonBeam_DEPIMPL();

	return firstWaveStartTime_;
}

//virtual
PhysAbsoluteTime MachLogIonBeam::firstWaveFinishTime() const
{
	CB_MachLogIonBeam_DEPIMPL();

	return firstWaveFinishTime_;
}

//virtual
PhysAbsoluteTime MachLogIonBeam::secondWaveStartTime() const
{
	CB_MachLogIonBeam_DEPIMPL();

	return secondWaveStartTime_;
}

//virtual
PhysAbsoluteTime MachLogIonBeam::secondWaveFinishTime() const
{
	CB_MachLogIonBeam_DEPIMPL();

	return secondWaveFinishTime_;
}

//virtual
PhysAbsoluteTime MachLogIonBeam::thirdWaveStartTime() const
{
	CB_MachLogIonBeam_DEPIMPL();

	return thirdWaveStartTime_;
}

//virtual
PhysAbsoluteTime MachLogIonBeam::thirdWaveFinishTime() const
{
	CB_MachLogIonBeam_DEPIMPL();

	return thirdWaveFinishTime_;
}

//virtual
PhysAbsoluteTime MachLogIonBeam::destructionTime() const
{
	CB_MachLogIonBeam_DEPIMPL();

	return destructionTime_;
}


MachLogIonBeam::~MachLogIonBeam()
{
	TEST_INVARIANT;

	_DELETE( pImpl_ );
}

void MachLogIonBeam::CLASS_INVARIANT
{
	CB_MachLogIonBeam_DEPIMPL();

    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogIonBeam& t )
{

    o << "MachLogIonBeam " << (void*)&t << " start" << std::endl;
    o << "MachLogIonBeam " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogIonBeam::doBeDestroyed()
{
//	MachLogLinearProjectile::genericCheckForDamage( 0.25, MachLogLinearProjectile::CONSTANT_DAMAGE, MachPhys::PULSE_RIFLE );
	TEST_INVARIANT;
}

MachPhysIonBeam* MachLogIonBeam::pNewPhysIonBeam( const MexPoint3d& startPosition, const MachPhysWeaponData& weaponData )
{
	pImpl_ = _NEW( MachLogIonBeamImpl( &weaponData ) );

	CB_MachLogIonBeam_DEPIMPL();

	// just calls into static method
	MachPhysIonBeam* pPhysBeam = pNewPhysIonBeam( startPosition , &pPhysIonBeam_ );

	ASSERT( pPhysIonBeam_, "Unexpected NULL for pPhysIonBeam_!" );

	return pPhysBeam;
}

// static
MachPhysIonBeam* MachLogIonBeam::pNewPhysIonBeam(
    const MexPoint3d& startPosition, MachPhysIonBeam** ppPhysIonBeam )
{
	HAL_STREAM("MLIonBeam::pNewPhysIonBeam\n" );
    //get domain and transform to use
    MexTransform3d localTransform;

    const MexRadians    zAngle = 0.0;

    W4dDomain* pDomain = MachLogPlanetDomains::pDomainPosition(
      startPosition, zAngle, &localTransform );

    //Construct the physical missile

    MachPhysIonBeam* pPhysIonBeam = _NEW( MachPhysIonBeam( pDomain, localTransform ) );
	*ppPhysIonBeam = pPhysIonBeam;
    return pPhysIonBeam;
}

//virtual
bool MachLogIonBeam::hitVictimFirstWave( const MachActor& victim ) const
{
	//static MATHEX_SCALAR beamRadius( const PhysRelativeTime& timeOffset );

	return false;
}

//virtual
void MachLogIonBeam::inflictDamageFirstWave( MachActor* pDamagedVictim )
{
	// do nothing
}

//virtual
bool MachLogIonBeam::hitVictimSecondWave( const MachActor& victim ) const
{
	MATHEX_SCALAR checkRadiusSize =  MachPhysBeam::outerRadius( SimManager::instance().currentTime() - secondWaveStartTime() );

  	bool result = actorWithinRadius( victim, checkRadiusSize );

	return result;
}


//virtual
void MachLogIonBeam::inflictDamageSecondWave( MachActor* pDamagedVictim )
{
	CB_MachLogIonBeam_DEPIMPL();

	MachActor* pByActor = pOwner();
	if( pOwner() and pOwner()->isDead() )
		pByActor = NULL;

	// don't want to damage this victim any more after this
	finishedWithVictim( pDamagedVictim );

	if( ( pDamagedVictim->hp() + pDamagedVictim->armour() ) <= pWeaponData_->damagePoints() )
	{
		// the actor has received sufficient damage to destroy it	(blam!)

		const MachActor& cactor = *pDamagedVictim;
		MachPhysIonWeapon::destroy( (W4dComposite*)&cactor.physObject().asComposite() , SimManager::instance().currentTime() );
		if( MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendApplySpecialWeaponAnimationMessage( pDamagedVictim->id(), MachPhys::ION_ORBITAL_CANNON );
		pDamagedVictim->beHitWithoutAnimation( pWeaponData_->damagePoints(), 0.1, pByActor );
	}
	else
	{
		// insufficient damage to destroy the actor. Just make it glow a bit.

		MexLine3d hitLine( startPosition(), pDamagedVictim->position() );
		pDamagedVictim->beHit(  pWeaponData_->damagePoints(), MachPhys::PULSE_RIFLE, pByActor, &hitLine );
	}

	TEST_INVARIANT;
}


//virtual
bool MachLogIonBeam::hitVictimThirdWave( const MachActor& victim ) const
{
	MATHEX_SCALAR basicWaveRadius = MachPhysBeam::waveRadius( SimManager::instance().currentTime() - thirdWaveStartTime() );
	MATHEX_SCALAR innerCheckRadiusSize =  basicWaveRadius - 8.0;
	MATHEX_SCALAR outerCheckRadiusSize =  basicWaveRadius - 3.0;

  	bool result = outerCheckRadiusSize > 12 and actorWithinRadius( victim, outerCheckRadiusSize )
  				  and not actorWithinRadius( victim, innerCheckRadiusSize );

	return result;
}

//virtual
void MachLogIonBeam::inflictDamageThirdWave( MachActor* pDamagedVictim )
{
	CB_MachLogIonBeam_DEPIMPL();

	MachActor* pByActor = pOwner();
	if( pOwner() and pOwner()->isDead() )
		pByActor = NULL;

	int damagePoints = 3;		// flash burns - may cause weaker machines to pop nicely

	victimMayHaveBeenKilled( pDamagedVictim, damagePoints );

	MexLine3d hitLine( startPosition(), pDamagedVictim->position() );
	pDamagedVictim->beHit( damagePoints, MachPhys::FLAME_THROWER2, pByActor, &hitLine );
}

//virtual
MATHEX_SCALAR MachLogIonBeam::potentialKillRadiusMultiplier() const
{
	return 1.15;
}

//forced recompile 3/2/99 CPS

/* End IONBEAM.CPP *************************************************/

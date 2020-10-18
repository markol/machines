/*
 * B E E B O M E X . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/beebomex.hpp"
#include "machlog/internal/beebomxi.hpp"

#include "mathex/line3d.hpp"

#include "phys/cspace2.hpp"

#include "world4d/domain.hpp"
#include "world4d/composit.hpp"

#include "machphys/beewave.hpp"
#include "machphys/beebombe.hpp"
#include "machphys/random.hpp"
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


MachLogBeeBombExplosion::MachLogBeeBombExplosion(	MachLogRace* pRace,
								        const MexPoint3d& startPosition,
										const MachPhysWeaponData& weaponData,
								        MachActor* pOwner )
:	MachLogExpandingBlast( pRace,
						   pNewPhysBeeBombExplosion( startPosition, weaponData ), 		// <- note that physical BombExplosion is created here
						   startPosition,
						   pOwner,
						   40,
						   CANT_HIT_AIR_UNITS )	// errrrm?
{
	CB_MachLogBeeBombExplosion_DEPIMPL();

	ASSERT( pPhysBeeBombExplosion_, "Unexpected NULL for pPhysBeeBombExplosion_!" );

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

 	firstWaveStartTime_ = timeNow;
	firstWaveFinishTime_ = timeNow + pPhysBeeBombExplosion_->startExplosion( timeNow, *MachLogPlanet::instance().surface() ) - 1.3;

	destructionTime_ = firstWaveFinishTime_ + 1.0;

    // Echo explosion effect across network.
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		MachLogNetwork::instance().messageBroker().sendCreateSpecialWeaponEffectMessage( startPosition,
																						 MachPhys::BEE_BOMB );
	}

	//set up collison data and animations

	//setUpBuildingCollisions( startPosition );

    TEST_INVARIANT;

}

//virtual
PhysAbsoluteTime MachLogBeeBombExplosion::firstWaveStartTime() const
{
	CB_MachLogBeeBombExplosion_DEPIMPL();

	return firstWaveStartTime_;
}

//virtual
PhysAbsoluteTime MachLogBeeBombExplosion::firstWaveFinishTime() const
{
	CB_MachLogBeeBombExplosion_DEPIMPL();

	return firstWaveFinishTime_;
}

//virtual
PhysAbsoluteTime MachLogBeeBombExplosion::destructionTime() const
{
	CB_MachLogBeeBombExplosion_DEPIMPL();

	return destructionTime_;
}


MachLogBeeBombExplosion::~MachLogBeeBombExplosion()
{
	TEST_INVARIANT;

	_DELETE( pImpl_ );
}

void MachLogBeeBombExplosion::CLASS_INVARIANT
{
	CB_MachLogBeeBombExplosion_DEPIMPL();

    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogBeeBombExplosion& t )
{

    o << "MachLogBeeBombExplosion " << (void*)&t << " start" << std::endl;
    o << "MachLogBeeBombExplosion " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogBeeBombExplosion::doBeDestroyed()
{
//	MachLogLinearProjectile::genericCheckForDamage( 0.25, MachLogLinearProjectile::CONSTANT_DAMAGE, MachPhys::PULSE_RIFLE );
	TEST_INVARIANT;
}

MachPhysBeeBombExplosion* MachLogBeeBombExplosion::pNewPhysBeeBombExplosion( const MexPoint3d& startPosition, const MachPhysWeaponData& weaponData )
{
	pImpl_ = _NEW( MachLogBeeBombExplosionImpl( &weaponData ) );

	CB_MachLogBeeBombExplosion_DEPIMPL();

	// just calls into static method
	MachPhysBeeBombExplosion* pPhysBomb = pNewPhysBeeBombExplosion( startPosition , &pPhysBeeBombExplosion_ );

	ASSERT( pPhysBeeBombExplosion_, "Unexpected NULL for pPhysBeeBombExplosion_!" );

	return pPhysBomb;
}

// static
MachPhysBeeBombExplosion* MachLogBeeBombExplosion::pNewPhysBeeBombExplosion(
    const MexPoint3d& startPosition, MachPhysBeeBombExplosion** ppPhysBeeBombExplosion )
{
	HAL_STREAM("MLBeeBombExplosion::pNewPhysBeeBombExplosion\n" );
    //get domain and transform to use
    MexTransform3d localTransform;

    const MexRadians    zAngle = 0.0;

    W4dDomain* pDomain = MachLogPlanetDomains::pDomainPosition(
      startPosition, zAngle, &localTransform );

    //Construct the physical missile

    MachPhysBeeBombExplosion* pPhysBeeBombExplosion = _NEW( MachPhysBeeBombExplosion( pDomain, localTransform ) );
	*ppPhysBeeBombExplosion = pPhysBeeBombExplosion;
    return pPhysBeeBombExplosion;

	//
}

//virtual
bool MachLogBeeBombExplosion::hitVictimFirstWave( const MachActor& victim ) const
{
	MATHEX_SCALAR checkRadiusSize =  MachPhysBeeBombWave::radius( SimManager::instance().currentTime() - firstWaveStartTime() );

  	bool result = actorWithinRadius( victim, checkRadiusSize );

	return result;
}

//virtual
void MachLogBeeBombExplosion::inflictDamageFirstWave( MachActor* pDamagedVictim )
{
	CB_MachLogBeeBombExplosion_DEPIMPL();

	MachActor* pByActor = pOwner();
	if( pOwner() and pOwner()->isDead() )
		pByActor = NULL;

	// don't want to damage this victim any more after this
	finishedWithVictim( pDamagedVictim );

	int damageInflicted = pWeaponData_->damagePoints();

	pDamagedVictim->beHit( damageInflicted , MachPhys::BOLTER, pByActor );	//(??)

	TEST_INVARIANT;
}

//virtual
MATHEX_SCALAR MachLogBeeBombExplosion::potentialKillRadiusMultiplier() const
{
	return 1.15;
}

//forced recompile 3/2/99 CPS

/* End BEEBombExplosion.CPP *************************************************/

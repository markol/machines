/*
 * N U C L B O M B . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/nuclbomb.hpp"
#include "machlog/internal/nuclbomi.hpp"

#include "mathex/line3d.hpp"

#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"

#include "world4d/domain.hpp"
#include "world4d/composit.hpp"

#include "machphys/nuclbomb.hpp"
#include "machphys/nuclweap.hpp"
#include "machphys/random.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/mphydata.hpp"

#include "sim/manager.hpp"

#include "machlog/actor.hpp"
#include "machlog/expandbl.hpp"
//#include "machlog/lineproj.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/network.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"


MachLogNuclearBomb::MachLogNuclearBomb(	MachLogRace* pRace,
								        const MexPoint3d& startPosition,
										const MachPhysWeaponData& weaponData,
								        MachActor* pOwner )
:	MachLogExpandingBlast( pRace,
						   pNewPhysNuclearBomb( startPosition, weaponData ), 		// <- note that physical bomb is created here
						   startPosition,
						   pOwner,
						   180 )	// arbitrary figure for now
{
	CB_MachLogNuclearBomb_DEPIMPL();

	ASSERT( pPhysNuclearBomb_, "Unexpected NULL for pPhysNuclearBomb_!" );

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	destructionTime_ = timeNow + pPhysNuclearBomb_->startExplosion( timeNow, MachLogPlanet::instance().surface() );

	PhysRelativeTime shockWaveBeginOffset = weaponData.extras()[ 6 ];
	PhysRelativeTime shockWaveDuration = weaponData.extras()[ 14 ];

 	firstWaveStartTime_ = timeNow;
	firstWaveFinishTime_ = timeNow + 1.0;
	secondWaveStartTime_ = timeNow + shockWaveBeginOffset;
	secondWaveFinishTime_ = secondWaveStartTime_ + (shockWaveDuration * 0.5 );
	// outside this radius, things just get singed a bit rather than really blasted. The damage decreases as a
	// linear function of distance from the epicentre to this "main damage radius". Outside that, singe damage is
	// constant.
	shockwaveMainDamageRadius_ = MachPhysNuclearBomb::shockwaveRadius( shockWaveDuration * 0.5 );

	thirdWaveStartTime_ = secondWaveFinishTime_;
	thirdWaveFinishTime_ = secondWaveStartTime_ + shockWaveDuration;

    // Echo explosion effect across network.
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		MachLogNetwork::instance().messageBroker().sendCreateSpecialWeaponEffectMessage( startPosition,
																						 MachPhys::NUCLEAR_MISSILE );
	}

	//set up collison data and animations

	//setUpBuildingCollisions( startPosition );

    TEST_INVARIANT;

}

//virtual
PhysAbsoluteTime MachLogNuclearBomb::firstWaveStartTime() const
{
	CB_MachLogNuclearBomb_DEPIMPL();

	return firstWaveStartTime_;
}

//virtual
PhysAbsoluteTime MachLogNuclearBomb::firstWaveFinishTime() const
{
	CB_MachLogNuclearBomb_DEPIMPL();

	return firstWaveFinishTime_;
}

//virtual
PhysAbsoluteTime MachLogNuclearBomb::secondWaveStartTime() const
{
	CB_MachLogNuclearBomb_DEPIMPL();

	return secondWaveStartTime_;
}

//virtual
PhysAbsoluteTime MachLogNuclearBomb::secondWaveFinishTime() const
{
	CB_MachLogNuclearBomb_DEPIMPL();

	return secondWaveFinishTime_;
}

//virtual
PhysAbsoluteTime MachLogNuclearBomb::thirdWaveStartTime() const
{
	CB_MachLogNuclearBomb_DEPIMPL();

	return thirdWaveStartTime_;
}

//virtual
PhysAbsoluteTime MachLogNuclearBomb::thirdWaveFinishTime() const
{
	CB_MachLogNuclearBomb_DEPIMPL();

	return thirdWaveFinishTime_;
}

//virtual
PhysAbsoluteTime MachLogNuclearBomb::destructionTime() const
{
	CB_MachLogNuclearBomb_DEPIMPL();

	return destructionTime_;
}


MachLogNuclearBomb::~MachLogNuclearBomb()
{
	TEST_INVARIANT;

	_DELETE( pImpl_ );
}

void MachLogNuclearBomb::CLASS_INVARIANT
{
	CB_MachLogNuclearBomb_DEPIMPL();

    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogNuclearBomb& t )
{

    o << "MachLogNuclearBomb " << (void*)&t << " start" << std::endl;
    o << "MachLogNuclearBomb " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogNuclearBomb::doBeDestroyed()
{
//	MachLogLinearProjectile::genericCheckForDamage( 0.25, MachLogLinearProjectile::CONSTANT_DAMAGE, MachPhys::PULSE_RIFLE );
	TEST_INVARIANT;
}

MachPhysNuclearBomb* MachLogNuclearBomb::pNewPhysNuclearBomb( const MexPoint3d& startPosition, const MachPhysWeaponData& weaponData )
{
	pImpl_ = _NEW( MachLogNuclearBombImpl( &weaponData ) );

	CB_MachLogNuclearBomb_DEPIMPL();

	// just calls into static method
	MachPhysNuclearBomb* pPhysBomb = pNewPhysNuclearBomb( startPosition , &pPhysNuclearBomb_ );

	ASSERT( pPhysNuclearBomb_, "Unexpected NULL for pPhysNuclearBomb_!" );

	return pPhysBomb;
}

// static
MachPhysNuclearBomb* MachLogNuclearBomb::pNewPhysNuclearBomb(
    const MexPoint3d& startPosition, MachPhysNuclearBomb** ppPhysNuclearBomb )
{
    //get domain and transform to use
    MexTransform3d localTransform;

    const MexRadians    zAngle = 0.0;

    W4dDomain* pDomain = MachLogPlanetDomains::pDomainPosition(
      startPosition, zAngle, &localTransform );

    //Construct the physical missile

    MachPhysNuclearBomb* pPhysNuclearBomb = _NEW( MachPhysNuclearBomb( pDomain, localTransform ) );
	*ppPhysNuclearBomb = pPhysNuclearBomb;

	return pPhysNuclearBomb;
}

//virtual
bool MachLogNuclearBomb::hitVictimFirstWave( const MachActor& victim ) const
{
	return actorWithinRadius( victim, 30 );
}

//virtual
void MachLogNuclearBomb::inflictDamageFirstWave( MachActor* pDamagedVictim )
{
	CB_MachLogNuclearBomb_DEPIMPL();

	MachActor* pByActor = pOwner();
	if( pOwner() and pOwner()->isDead() )
		pByActor = NULL;

	// don't want to damage this victim any more after this
	finishedWithVictim( pDamagedVictim );

	int damagePoints = pWeaponData_->damagePoints();

	pDamagedVictim->beHit( damagePoints, MachPhys::BOLTER, pByActor );

	TEST_INVARIANT;
}

//virtual
bool MachLogNuclearBomb::hitVictimSecondWave( const MachActor& victim ) const
{
	MATHEX_SCALAR checkRadiusSize =  MachPhysNuclearBomb::shockwaveRadius( SimManager::instance().currentTime() - secondWaveStartTime() );

  	bool result = actorWithinRadius( victim, checkRadiusSize );

	return result;
}


//virtual
void MachLogNuclearBomb::inflictDamageSecondWave( MachActor* pDamagedVictim )
{
	CB_MachLogNuclearBomb_DEPIMPL();

	ASSERT( secondWaveFinishTime() > secondWaveStartTime(), "This would cause a division by zero. Something must be wrong with code elsewhere allowing this method to be called." );

	MachActor* pByActor = pOwner();
	if( pOwner() and pOwner()->isDead() )
		pByActor = NULL;

	// don't want to damage this victim any more after this
	finishedWithVictim( pDamagedVictim );

	MexLine3d hitLine( startPosition(), pDamagedVictim->position() );

	// the further out from the epicentre, the less damage.
	MATHEX_SCALAR damageMultiplier = 1.0 - ( hitLine.length() / shockwaveMainDamageRadius_ );

	int damage = ( pWeaponData_->damagePoints() * damageMultiplier ) + 5.0;			// guaranteed to take SOME damage.

	pDamagedVictim->beHit( damage, MachPhys::FLAME_THROWER2, pByActor, &hitLine );

	TEST_INVARIANT;
}


//virtual
bool MachLogNuclearBomb::hitVictimThirdWave( const MachActor& victim ) const
{
	MATHEX_SCALAR checkRadiusSize =  MachPhysNuclearBomb::shockwaveRadius( SimManager::instance().currentTime() - secondWaveStartTime() ) - 5.0;
	// Note! The use of secondWaveStartTime in this calculation is NOT an error. This wave calculates its distance from
	// the epicentre as though it were a continuation of the second wave.

  	bool result = actorWithinRadius( victim, checkRadiusSize );

	return result;
}

//virtual
void MachLogNuclearBomb::inflictDamageThirdWave( MachActor* pDamagedVictim )
{
	CB_MachLogNuclearBomb_DEPIMPL();

	MachActor* pByActor = pOwner();
	if( pOwner() and pOwner()->isDead() )
		pByActor = NULL;

	// don't want to damage this victim any more after this
	finishedWithVictim( pDamagedVictim );

	int damagePoints = 5;		// flash burns - may cause very weak machines to pop nicely

	MexLine3d hitLine( startPosition(), pDamagedVictim->position() );
	pDamagedVictim->beHit( damagePoints, MachPhys::FLAME_THROWER2, pByActor, &hitLine );

	TEST_INVARIANT;
}

//virtual
MATHEX_SCALAR MachLogNuclearBomb::potentialKillRadiusMultiplier() const
{
	return 1.10;
}

//forced recompile 3/2/99 CPS

/* End NUCLBOMB.CPP *************************************************/

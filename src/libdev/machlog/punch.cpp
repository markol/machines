/*
 * V O R T B O M B . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/punch.hpp"
#include "machlog/internal/punchi.hpp"

#include "phys/cspace2.hpp"

#include "world4d/domain.hpp"
#include "world4d/composit.hpp"

#include "machphys/punch.hpp"
#include "machphys/punweap.hpp"
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
#include "machlog/spacial.hpp"


MachLogPunchBlast::MachLogPunchBlast(	MachLogRace* pRace,
								        MachPhysPunchBlast* pPhysPunchBlast,
										const MachPhysWeaponData& weaponData,
								        MachActor* pOwner )
:	MachLogExpandingBlast( pRace,
						   pPhysPunchBlast,
						   pPhysPunchBlast->globalTransform().position(),
						   pOwner,
						   MachPhysPunchBlast::range(),
						   CANT_HIT_AIR_UNITS ),
	pImpl_( _NEW( MachLogPunchBlastImpl( &weaponData ) ) )
{


	CB_MachLogPunchBlast_DEPIMPL();

	pPhysPunchBlast_ = pPhysPunchBlast;

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	ASSERT( pPhysPunchBlast_, "Unexpected NULL for pPhysPunchBlast_!" );

	destructionTime_ = pPhysPunchBlast->blastEndTime();
	destructionWaveStartTime_ = pPhysPunchBlast->blastBeginTime();
	destructionWaveFinishTime_ = destructionWaveStartTime_ + 1.2;	// best guess

    TEST_INVARIANT;

}

//virtual
PhysAbsoluteTime MachLogPunchBlast::firstWaveStartTime() const
{
	CB_MachLogPunchBlast_DEPIMPL();

	return destructionWaveStartTime_;
}

//virtual
PhysAbsoluteTime MachLogPunchBlast::firstWaveFinishTime() const
{
	CB_MachLogPunchBlast_DEPIMPL();

	return destructionWaveFinishTime_;
}

//virtual
PhysAbsoluteTime MachLogPunchBlast::destructionTime() const
{
	CB_MachLogPunchBlast_DEPIMPL();

	return destructionTime_;
}


MachLogPunchBlast::~MachLogPunchBlast()
{
	TEST_INVARIANT;

	_DELETE( pImpl_ );
}

/*
MachLogPunchBlastImpl* MachLogPunchBlast::pImpl()
{
	pImpl_ = _NEW( MachLogPunchBlastImpl( &weaponData ) );
	return pImpl_;
}
*/
void MachLogPunchBlast::CLASS_INVARIANT
{
	CB_MachLogPunchBlast_DEPIMPL();

    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogPunchBlast& t )
{

    o << "MachLogPunchBlast " << (void*)&t << " start" << std::endl;
    o << "MachLogPunchBlast " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogPunchBlast::doBeDestroyed()
{
//	MachLogLinearProjectile::genericCheckForDamage( 0.25, MachLogLinearProjectile::CONSTANT_DAMAGE, MachPhys::PULSE_RIFLE );
	TEST_INVARIANT;
}

//virtual
bool MachLogPunchBlast::hitVictimFirstWave( const MachActor& victim ) const
{
	CB_MachLogPunchBlast_DEPIMPL();

	bool result = false;

	MATHEX_SCALAR basicWaveRadius = MachPhysPunchBlast::radius( SimManager::instance().currentTime() - firstWaveStartTime() );
	MATHEX_SCALAR minimumCheckRadiusSize =  basicWaveRadius - 8.0;

  	bool targetIsAtHitDistance = actorWithinRadius( victim, basicWaveRadius )
  				  				 and not actorWithinRadius( victim, minimumCheckRadiusSize );

	if( targetIsAtHitDistance )
	{
		// certainly within range.....but is it within angle?

		// use the punch blast's global transform to calculate which actors are within its cone of influence
		const MexTransform3d& blastOriginAsTransform = pPhysPunchBlast_->globalTransform();

		MexRadians absTempRelativeAngle = fabs( MachLogSpacialManipulation::angleToTurnToFace( blastOriginAsTransform, victim.position() ).asScalar() );

		if( absTempRelativeAngle < MachPhysPunchBlast::absMaxDestructionAngle() )
		{
			// yup, that's the monkey.
			result = true;
		}
	}

	return result;
}

//virtual
void MachLogPunchBlast::inflictDamageFirstWave( MachActor* pDamagedVictim )
{
	CB_MachLogPunchBlast_DEPIMPL();

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
MATHEX_SCALAR MachLogPunchBlast::potentialKillRadiusMultiplier() const
{
	return 3.0;
}

//virtual
void MachLogPunchBlast::ownerDestroyed()
{
	finished( true );

	MachLogExpandingBlast::ownerDestroyed();
}

/* End VORTBOMB.CPP *************************************************/

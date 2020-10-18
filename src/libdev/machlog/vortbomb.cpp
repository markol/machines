/*
 * V O R T B O M B . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/vortbomb.hpp"
#include "machlog/internal/vortbomi.hpp"

#include "mathex/line3d.hpp"
#include "mathex/eulerang.hpp"

#include "phys/cspace2.hpp"

#include "world4d/domain.hpp"
#include "world4d/composit.hpp"

#include "machphys/constron.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/vortbomb.hpp"
#include "machphys/vortweap.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"


#include "sim/manager.hpp"

#include "machlog/actor.hpp"
#include "machlog/constron.hpp"
#include "machlog/expandbl.hpp"
#include "machlog/machine.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/network.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"


MachLogVortexBomb::MachLogVortexBomb(	MachLogRace* pRace,
								        const MexPoint3d& startPosition,
										const MachPhysWeaponData& weaponData,
								        MachActor* pOwner )
:	MachLogExpandingBlast( pRace,
						   pNewPhysVortexBomb( startPosition, weaponData, pOwner ), 		// <- note that physical bomb is created here
						   startPosition,
						   pOwner,
						   weaponData.extras()[0] )
{
	CB_MachLogVortexBomb_DEPIMPL();

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	ASSERT( pPhysVortexBomb_, "Unexpected NULL for pPhysVortexBomb_!" );

	PhysRelativeTime expansionBeginOffset = 2.0;	// time after vortex appears that it starts to expand outwards

	destructionTime_ = timeNow + pPhysVortexBomb_->startExplosion( timeNow );
	destructionWaveStartTime_ = timeNow + expansionBeginOffset;

	PhysAbsoluteTime totalExpansionDuration;
 	MachPhysVortexBomb::radiusTime( blastRadius(), &totalExpansionDuration );
	totalExpansionDuration -= expansionBeginOffset;
									// because radiustime returns a value from the very start of the entire bomb,
									// whereas we're interested only in the duration after the main expansion has
									// started (expansionBeginOffset seconds after the vortex first appears)

	destructionWaveFinishTime_ = destructionWaveStartTime_ + totalExpansionDuration;

    // Echo explosion effect across network.
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		ASSERT( pOwner != NULL, "Unexpected NULL pointer for pOwner!" );
		MachLogNetwork::instance().messageBroker().sendCreateSpecialWeaponEffectMessage( startPosition, MachPhys::VORTEX, pOwner->id() );
	}

	//set up collison data and animations

	//setUpBuildingCollisions( startPosition );

    TEST_INVARIANT;

}

//virtual
PhysAbsoluteTime MachLogVortexBomb::firstWaveStartTime() const
{
	CB_MachLogVortexBomb_DEPIMPL();

	return destructionWaveStartTime_;
}

//virtual
PhysAbsoluteTime MachLogVortexBomb::firstWaveFinishTime() const
{
	CB_MachLogVortexBomb_DEPIMPL();

	return destructionWaveFinishTime_;
}

//virtual
PhysAbsoluteTime MachLogVortexBomb::destructionTime() const
{
	CB_MachLogVortexBomb_DEPIMPL();

	return destructionTime_;
}


MachLogVortexBomb::~MachLogVortexBomb()
{
	TEST_INVARIANT;

	_DELETE( pImpl_ );
}

/*
MachLogVortexBombImpl* MachLogVortexBomb::pImpl()
{
	pImpl_ = _NEW( MachLogVortexBombImpl( &weaponData ) );
	return pImpl_;
}
*/
void MachLogVortexBomb::CLASS_INVARIANT
{
	CB_MachLogVortexBomb_DEPIMPL();

    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogVortexBomb& t )
{

    o << "MachLogVortexBomb " << (void*)&t << " start" << std::endl;
    o << "MachLogVortexBomb " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogVortexBomb::doBeDestroyed()
{
//	MachLogLinearProjectile::genericCheckForDamage( 0.25, MachLogLinearProjectile::CONSTANT_DAMAGE, MachPhys::PULSE_RIFLE );
	TEST_INVARIANT;
}

MachPhysVortexBomb* MachLogVortexBomb::pNewPhysVortexBomb( const MexPoint3d& startPosition, const MachPhysWeaponData& weaponData, MachActor* pOwnerActor )
{
	pImpl_ = _NEW( MachLogVortexBombImpl( &weaponData ) );

	CB_MachLogVortexBomb_DEPIMPL();

	// just calls into static method
	MachPhysVortexBomb* pPhysBomb = pNewPhysVortexBomb( startPosition , pOwnerActor->id(), &pPhysVortexBomb_ );

	ASSERT( pPhysVortexBomb_, "Unexpected NULL for pPhysVortexBomb_!" );

	return pPhysBomb;
}

// static
MachPhysVortexBomb* MachLogVortexBomb::pNewPhysVortexBomb(
    const MexPoint3d& startPosition, UtlId firingActorId, MachPhysVortexBomb** ppPhysVortexBomb )
{
    //get domain and transform to use
    MexTransform3d localTransform;

    const MexRadians    zAngle = 0.0;

    W4dDomain* pDomain = NULL;

	MachActor* pOwnerActor = NULL;

	if( firingActorId != 0 )
	{
		MachLogRaces& races = MachLogRaces::instance();

		if( MachLogRaces::instance().actorExists( firingActorId ) )
		{
			pOwnerActor = &( races.actor( firingActorId ) );
		}
	}

    if( pOwnerActor
    	and pOwnerActor->objectIsMachine()
    	and pOwnerActor->asMachine().insideBuilding() )
    {
		ASSERT( pOwnerActor->asMachine().insideWhichBuilding().physConstruction().hasInterior(), "Unexpectedly found that owner actor was inside a building flagged as having no interior!" );
    	pDomain = &( pOwnerActor->asMachine().insideWhichBuilding().interiorDomain() );

		//Create the global transform to the position
	    MexTransform3d globalTransform( MexEulerAngles( zAngle, 0, 0 ), startPosition );

	    //Get the domain's global inverse transform
	    MexTransform3d domainInverseTransform;
	    pDomain->globalTransform().invert( &domainInverseTransform );

	    //Hence compute the desired local transform
	    domainInverseTransform.transform( globalTransform, &localTransform );


    }
	else
	{
		pDomain = MachLogPlanetDomains::pDomainPosition( startPosition, zAngle, &localTransform );
	}


    //Construct the physical missile

    MachPhysVortexBomb* pPhysVortexBomb = _NEW( MachPhysVortexBomb( pDomain, localTransform ) );
	*ppPhysVortexBomb = pPhysVortexBomb;

	// Now make sure overlapping domains have an intersect relationship with this physical vortex bomb object. Thing.
	MachLogPlanetDomains::Domains intersectingDomains;

	// This is a hard-wired hack, and I know it. But to do it properly would be......difficult.
	MexAlignedBox2d vortexBoundary( startPosition, 60.0);	// 60.0 is a value I happen to like. 40 is the kill radius
															// of the vortex, but we give an extra 20 for some of the
															// grey whoosh at the end (not the full amount, but it's
															// a cost-benefit thing - will clip out for a split-second
															// in the pathological case, but will be more efficient in
															// the general case).

	MachLogPlanetDomains::intersecting( vortexBoundary, &intersectingDomains );
	HAL_STREAM("number of intersecting domains is: " << intersectingDomains.size() << std::endl );
	ASSERT( intersectingDomains.size() > 0, "Linear projectile MUST intersect with at least one domain\n" );

	CERI_STREAM( "=======ADDING DOMAIN INTERSECTS==============" << std::endl );
	CERI_STREAM( "Vortex actually lives in domain at position " << pPhysVortexBomb->pParent()->globalTransform().position() << std::endl );
	CERI_STREAM( "startPosition is " << startPosition << std::endl );
	for( MachLogPlanetDomains::Domains::iterator i = intersectingDomains.begin(); i != intersectingDomains.end(); ++i )
	{
		W4dDomain* pDomain = (*i);
		if( pDomain != pPhysVortexBomb->pParent() )
		{
			CERI_STREAM( "Added a domain intersection at position" << pDomain->globalTransform().position() << std::endl );
			pPhysVortexBomb->intersects( pDomain, true );
		}

	}

     return pPhysVortexBomb;
}

//virtual
bool MachLogVortexBomb::hitVictimFirstWave( const MachActor& victim ) const
{
	bool result = false;

	MATHEX_SCALAR checkRadiusSize =  MachPhysVortexBomb::radius( SimManager::instance().currentTime() - firstWaveStartTime() ) + 2.0;

	result = actorWithinRadius( victim, checkRadiusSize ) and checkRadiusSize >= 4;

	return result;
}

//virtual
void MachLogVortexBomb::inflictDamageFirstWave( MachActor* pDamagedVictim )
{
	CB_MachLogVortexBomb_DEPIMPL();

	MachActor* pByActor = pOwner();
	if( pOwner() and pOwner()->isDead() )
		pByActor = NULL;

	// don't want to damage this victim any more after this
	finishedWithVictim( pDamagedVictim );

	if( ( pDamagedVictim->hp() + pDamagedVictim->armour() ) <= pWeaponData_->damagePoints() )
	{
		// the actor has received sufficient damage to destroy it	(whoosh!)

		const MachActor& cactor = *pDamagedVictim;
		MachPhysVortexWeapon::destroy( &( pDamagedVictim->physObject() ), SimManager::instance().currentTime() );
		if( MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendApplySpecialWeaponAnimationMessage( pDamagedVictim->id(), MachPhys::VORTEX );
		pDamagedVictim->beHitWithoutAnimation( pWeaponData_->damagePoints(), 0.3, pByActor );
	}
	else
	{
		// insufficient damage to destroy the actor. Initiate shaking animation as it goes through its painful
		// (although not fatal) ordeal.

		MexLine3d hitLine( startPosition(), pDamagedVictim->position() );

		if( hitLine.length() == 0 )
		{
			// The line must appear to come from somewhere

			MexPoint3d end2 = hitLine.end2();
			end2 += MexVec3( 0.01, 0.0, 0.0 );
			hitLine.setLine( hitLine.end1(), end2 );
		}

		pDamagedVictim->beHit(  pWeaponData_->damagePoints(), MachPhys::VORTEX, pByActor, &hitLine );
	}

	TEST_INVARIANT;
}

//virtual
MATHEX_SCALAR MachLogVortexBomb::potentialKillRadiusMultiplier() const
{
	return 2.0;
}

/* End VORTBOMB.CPP *************************************************/

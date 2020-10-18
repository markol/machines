/*
 * A T T A C K . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/attack.hpp"
#include "machphys/internal/attacki.hpp"

#include "machphys/weapon.hpp"
#include "machphys/turntrak.hpp"
#include "machphys/wepdata.hpp"

#include "world4d/composit.hpp"
#include "world4d/link.hpp"
#include "mathex/transf3d.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( MachPhysCanAttack );

typedef MachPhysCanAttackImpl::Weapons Weapons;
typedef MachPhysCanAttackImpl::TurnerTrackers TurnerTrackers;

MachPhysCanAttack::MachPhysCanAttack()
:   pImpl_( _NEW( MachPhysCanAttackImpl ) )
{
	CB_DEPIMPL( Weapons, weapons_ );
	CB_DEPIMPL( TurnerTrackers, turnerTrackers_ );

    //Set sensible allocation size.
    weapons_.reserve( MachPhys::N_MOUNTINGS );
    turnerTrackers_.reserve( MachPhys::N_MOUNTINGS );
}

MachPhysCanAttack::~MachPhysCanAttack()
{
    _DELETE( pImpl_ );
}

void MachPhysCanAttack::initialiseMountingLinks()
{
    PRE( MachPhys::N_MOUNTINGS == 3 );
    PRE( MachPhys::RIGHT == 0 );
    PRE( MachPhys::LEFT == 1 );
    PRE( MachPhys::TOP == 2 );

	CB_DEPIMPL( ctl_fixed_vector< W4dLink* >, mountingLinks_ );

    char* linkNames[] = { "r_gun", "l_gun", "gun" };
    W4dComposite& attacker = asComposite();

    W4dLink* pLink;
    for( int i = 0; i != MachPhys::N_MOUNTINGS; ++i )
    {
        if( attacker.findLink( linkNames[i], &pLink ) )
        {
            mountingLinks_[i] = pLink;
            pLink->clearMeshes();
        }
    }
}

MachPhysCanAttack::MachPhysCanAttack( const MachPhysCanAttack& rhs, W4dComposite* pAttacker )
:   pImpl_( _NEW( MachPhysCanAttackImpl ) )
{
	CB_DEPIMPL( ctl_fixed_vector< W4dLink* >, mountingLinks_ );
	CB_DEPIMPL( Weapons, weapons_ );
	CB_DEPIMPL( TurnerTrackers, turnerTrackers_ );

    mountingLinks_ = rhs.pImpl_->mountingLinks_;

    //Mounted weapons are not copied. So set sensible allocation size.
    weapons_.reserve( MachPhys::N_MOUNTINGS );
    turnerTrackers_.reserve( MachPhys::N_MOUNTINGS );

    //identify the links for this instance using the ids from rhs
    int n = mountingLinks_.size();
    const W4dComposite::W4dLinks& myLinks = pAttacker->links();

    for( int i = 0; i != n ; ++i )
    {
        W4dLink* pRhsLink = mountingLinks_[i];
        if( pRhsLink != NULL )
            mountingLinks_[i] = myLinks[ pRhsLink->id() ];
    }
}

bool MachPhysCanAttack::hasMounting( MachPhys::Mounting mounting )
{
	CB_DEPIMPL( ctl_fixed_vector< W4dLink* >, mountingLinks_ );
    return mountingLinks_[ mounting ] != NULL;
}

void MachPhysCanAttack::mount( MachPhysWeapon* pWeapon, MachPhys::Mounting mounting )
{
    PRE( hasMounting( mounting ) );

	CB_DEPIMPL( ctl_fixed_vector< W4dLink* >, mountingLinks_ );
	CB_DEPIMPL( Weapons, weapons_ );
	CB_DEPIMPL( TurnerTrackers, turnerTrackers_ );

    //Get the link to act as parent
    W4dLink* pParent = mountingLinks_[ mounting ];

    //See if any of the mounted weapons already have this parent
    bool alreadyLoaded = false;
    size_t n = weapons_.size();
    size_t index;
    for( size_t i = 0; not alreadyLoaded  and  i != n; ++i )
    {
        MachPhysWeapon* pOldWeapon = weapons_[i];

        if( pOldWeapon->pParent() == pParent )
        {
            _DELETE( pOldWeapon );
            weapons_[i] = pWeapon;
            alreadyLoaded = true;
            index = i;
        }
    }

    //If not remounted in above loop, add a new weapon element
    if( not alreadyLoaded )
    {
        index = weapons_.size();
        weapons_.push_back( pWeapon );
    }

    const MachPhysWeaponData& weaponData = pWeapon->weaponData();
    MachPhysTurnerTracker* pTurnerTracker = NULL;
    if( weaponData.canTilt() )
    {
        //Use hardcoded tilt rate
        MexRadians tiltRate = MexDegrees( 180 );
        pTurnerTracker = _NEW( MachPhysTurnerTracker( pParent, W4d::Y_AXIS, W4d::X_AXIS, tiltRate ) );
        pTurnerTracker->limits( weaponData.minTiltAngle(), weaponData.maxTiltAngle() );
    }

    //Prepare to set up the object which control weapon tilting
    if( index < turnerTrackers_.size() )
    {
        //Delete the old tracker and insert the new one
        _DELETE( turnerTrackers_[ index ] );
        turnerTrackers_[ index ] = pTurnerTracker;
    }
    else
    {
        //Add the new entry
        turnerTrackers_.push_back( pTurnerTracker );
    }

    //Attach the weapon to the link
    asComposite().hold( pWeapon, pParent, MexTransform3d() );

    //Get the nearest angle to zero that we can turn to and go there
    if( pTurnerTracker )
    {
        MexRadians nearestAngleToZero( 0.0 );
        if( pTurnerTracker->minAngle() > nearestAngleToZero )
            nearestAngleToZero = pTurnerTracker->minAngle();
        else if( pTurnerTracker->maxAngle() < nearestAngleToZero )
            nearestAngleToZero = pTurnerTracker->maxAngle();

        pTurnerTracker->turnToAngle( nearestAngleToZero );
    }
}

uint MachPhysCanAttack::nMountedWeapons() const
{
	CB_DEPIMPL( Weapons, weapons_ );
    return weapons_.size();
}

MachPhysWeapon& MachPhysCanAttack::weapon( uint index )
{
    PRE( index < nMountedWeapons() );
	CB_DEPIMPL( Weapons, weapons_ );
    return *weapons_[ index ];
}

void perWrite( PerOstream& ostr, const MachPhysCanAttack& attack )
{
    ostr << attack.pImpl_;
//    ostr << attack.weapons_;
}

void perRead( PerIstream& istr, MachPhysCanAttack& attack )
{
	_DELETE( attack.pImpl_ );

    istr >> attack.pImpl_;
//    istr >> attack.weapons_;
}

void MachPhysCanAttack::trackTarget( const W4dEntity& targetObject )
{
    //Make the weapon base track the target if it can
    if( canTrackWeaponBase() )
        doWeaponBaseTrackTarget( targetObject );

    //set up the weapon tilt tracking
    trackTargetWithWeapons( targetObject );
}

//virtual
void MachPhysCanAttack::doWeaponBaseTrackTarget( const W4dEntity& )
{
    //Nothing can be done
}

void MachPhysCanAttack::stopTrackingTarget()
{
	CB_DEPIMPL( TurnerTrackers, turnerTrackers_ );

    //If the weapon base can stop, make it so!
    if( canTrackWeaponBase() )
        doStopWeaponBaseTrackingTarget();

    //Make any tilting weapons stop tracking the target, and return to zero tilt angle
    for( TurnerTrackers::iterator it = turnerTrackers_.begin(); it != turnerTrackers_.end(); ++it )
    {
        MachPhysTurnerTracker* pTurnerTracker = (*it);
        if( pTurnerTracker != NULL )
        {
            pTurnerTracker->stopTracking();

            //Get the nearest angle to zero that we can turn to
            MexRadians nearestAngleToZero( 0.0 );
            if( pTurnerTracker->minAngle() > nearestAngleToZero )
                nearestAngleToZero = pTurnerTracker->minAngle();
            else if( pTurnerTracker->maxAngle() < nearestAngleToZero )
                nearestAngleToZero = pTurnerTracker->maxAngle();

            pTurnerTracker->turnToAngle( nearestAngleToZero );
        }
    }
}

//virtual
void MachPhysCanAttack::doStopWeaponBaseTrackingTarget()
{
    //Nothing can be done
}

const MexTransform3d& MachPhysCanAttack::globalWeaponAspectTransform() const
{
	CB_DEPIMPL( Weapons, weapons_ );

    return weapons_[ 0 ]->globalTransform();
}

bool MachPhysCanAttack::atTiltLimits( uint weaponIndex ) const
{
    PRE( weaponIndex < nMountedWeapons() );

	CB_DEPIMPL( TurnerTrackers, turnerTrackers_ );

    //Considered at limits if can't tilt - ie no tracker set up, or tracker at its limits
    MachPhysTurnerTracker* pTurnerTracker = turnerTrackers_[ weaponIndex ];
    return pTurnerTracker == NULL  or  pTurnerTracker->nearLimits( MexDegrees( 1.0 ) ) ;
}

bool MachPhysCanAttack::hasLaunchedLightWithin
(
	MachPhys::WeaponType weapon,
	PhysRelativeTime minimumTime,
	PhysAbsoluteTime firingTime
) const
{
	CB_DEPIMPL( MachPhys::WeaponType, lastLightWeapon_ );
	CB_DEPIMPL( PhysAbsoluteTime, lastLightLaunchTime_ );

	bool hasFired = false;
	if(lastLightWeapon_ == weapon)
		if(firingTime - minimumTime < lastLightLaunchTime_)
			hasFired = true;

	return hasFired;
}

bool MachPhysCanAttack::hasPlayedSoundWithin
(
	MachPhys::WeaponType weapon,
	PhysRelativeTime minimumTime,
	PhysAbsoluteTime firingTime
) const
{
	CB_DEPIMPL( MachPhys::WeaponType, lastSoundWeapon_ );
	CB_DEPIMPL( PhysAbsoluteTime, lastSoundPlayTime_ );

	bool hasFired = false;
	if(lastSoundWeapon_ == weapon)
		if(firingTime - minimumTime < lastSoundPlayTime_)
			hasFired = true;

	return hasFired;
}

void MachPhysCanAttack::cacheSoundPlay(MachPhys::WeaponType weapon, PhysAbsoluteTime firingTime)
{
	CB_DEPIMPL( MachPhys::WeaponType, lastSoundWeapon_ );
	CB_DEPIMPL( PhysAbsoluteTime, lastSoundPlayTime_ );

	lastSoundWeapon_ = weapon;
	lastSoundPlayTime_ = firingTime;
}

void MachPhysCanAttack::cacheLightLaunch(MachPhys::WeaponType weapon, PhysAbsoluteTime firingTime)
{
	CB_DEPIMPL( MachPhys::WeaponType, lastLightWeapon_ );
	CB_DEPIMPL( PhysAbsoluteTime, lastLightLaunchTime_ );

	lastLightWeapon_ = weapon;
	lastLightLaunchTime_ = firingTime;
}

void MachPhysCanAttack::trackTargetWithWeapons( const W4dEntity& targetObject )
{
	CB_DEPIMPL( TurnerTrackers, turnerTrackers_ );

    //Make any tilting weapons track the target
    for( TurnerTrackers::iterator it = turnerTrackers_.begin(); it != turnerTrackers_.end(); ++it )
    {
        MachPhysTurnerTracker* pTurnerTracker = (*it);
        if( pTurnerTracker != NULL )
            pTurnerTracker->trackTarget( targetObject );
    }
}
/* End ATTACK.CPP ***************************************************/

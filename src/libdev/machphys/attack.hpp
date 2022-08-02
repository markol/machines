/*
 * A T T A C K . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * Mixin for physical objects that support an
 * attack animation.
 */

#ifndef _MACHPHYS_ATTACK_HPP
#define _MACHPHYS_ATTACK_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "machphys/machphys.hpp"

//forward refs
class MexTransform3d;
class W4dComposite;
class W4dLink;
class W4dEntity;
class MachPhysWeapon;
class MachPhysTurnerTracker;
class MachPhysCanAttackImpl;

/* //////////////////////////////////////////////////////////////// */

class MachPhysCanAttack
{
public:

	virtual ~MachPhysCanAttack();

    //Makes the underlying physical model available
    virtual W4dComposite& asComposite() = 0;
    virtual const W4dComposite& asComposite() const = 0;

    //True iff this attacker has a weapon mounting point at the position of mounting
    bool hasMounting( MachPhys::Mounting mounting );

    //Take ownership of pWeapon and mount at location defined by mounting.
    //Any previously mounted weapon at this location is deleted.
    void mount( MachPhysWeapon* pWeapon, MachPhys::Mounting mounting );
    //PRE( hasMounting( mounting ) );

    //Number of mounted weapons
    uint nMountedWeapons() const;

    //The index'th mounted weapon
    MachPhysWeapon& weapon( uint index );
    //PRE( index < nMountedWeapons() );

    //A transform which represents the global location/orientation of the weapons.
    //Can be used to determine the direction in which a weapon is pointing.
    const MexTransform3d& globalWeaponAspectTransform() const;

    //True iff the attacker can turn its weapon carrier to track a target
    virtual bool canTrackWeaponBase() const = 0;

    //Track targetObject by turning any weapon mounting to face it if possible,
    //and tilting the mounted weapons if possible.
    void trackTarget( const W4dEntity& targetObject );

    //Same as trackTarget() above, but only does it by tilting the weapons, not
    //turning the head.
    void trackTargetWithWeapons( const W4dEntity& targetObject );

    //Override to cause the weapon base to turn in order to track targetObject.
    //Default implementation does nothing.
    virtual void doWeaponBaseTrackTarget( const W4dEntity& targetObject );

    //Stop tracking any current target
    void stopTrackingTarget();

    //Override to stop the weapon base tracking any target.
    //Default implementation does nothing.
    virtual void doStopWeaponBaseTrackingTarget();

    //True if weapon weaponIndex can't tilt or is very close to its limits.
    bool atTiltLimits( uint weaponIndex ) const;
    //PRE( index < nMountedWeapons() );

	// Has this attacker fired a weapon with a light or played a firing sound
	// within the given time interval.
	bool hasLaunchedLightWithin(MachPhys::WeaponType weapon, PhysRelativeTime, PhysAbsoluteTime firingTime) const;
	bool hasPlayedSoundWithin  (MachPhys::WeaponType weapon, PhysRelativeTime, PhysAbsoluteTime firingTime) const;

	// Cache this weapon as having launched a light or played a sound at the specified time.
	void cacheSoundPlay(MachPhys::WeaponType weapon, PhysAbsoluteTime firingTime);
	void cacheLightLaunch(MachPhys::WeaponType weapon, PhysAbsoluteTime firingTime);

    PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( MachPhysCanAttack );
    PER_FRIEND_READ_WRITE( MachPhysCanAttack );

protected:
    //ctors. pAttacker is this attacking machine as a composite. (The asComposite fn
    //can't be used inside the ctor).
    MachPhysCanAttack();
    MachPhysCanAttack( const MachPhysCanAttack& rhs, W4dComposite* pAttacker );

    //Set up the pointers to the mounting links by searching for appropriately named
    //links: r_gun, l_gun, gun.
    //Deletes any mesh instances associated with such links.
    //This method should only be used by a 1-time constructor derived class.
    void initialiseMountingLinks();

private:
    MachPhysCanAttackImpl* pImpl_;
};

/* //////////////////////////////////////////////////////////////// */

PER_DECLARE_PERSISTENT( MachPhysCanAttack );

#endif	/*	#ifndef 	_MACHPHYS_ATTACK_HPP	*/

/* End ATTACK.HPP ***************************************************/

/*
 * A T T A C K I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysCanAttackImpl

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_ATTACKI_HPP
#define _MACHPHYS_ATTACKI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/fixedvec.hpp"
#include "ctl/pvector.hpp"
#include "machphys/machphys.hpp"

class W4dLink;
class MachPhysWeapon;
class MachPhysTurnerTracker;

class MachPhysCanAttackImpl
// Canonical form revoked
{
public:
    MachPhysCanAttackImpl( void );
    ~MachPhysCanAttackImpl( void );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( MachPhysCanAttackImpl );
    PER_FRIEND_READ_WRITE( MachPhysCanAttackImpl );

    typedef ctl_pvector< MachPhysWeapon > Weapons;
    typedef ctl_pvector< MachPhysTurnerTracker > TurnerTrackers;

private:
    friend class MachPhysCanAttack;
    friend ostream& operator <<( ostream& o, const MachPhysCanAttackImpl& t );

    MachPhysCanAttackImpl( const MachPhysCanAttackImpl& );
    MachPhysCanAttackImpl& operator =( const MachPhysCanAttackImpl& );

    //typedef ctl_pvector< MachPhysWeapon > Weapons;
    //typedef ctl_pvector< MachPhysTurnerTracker > TurnerTrackers;

    //data members
    ctl_fixed_vector< W4dLink* > mountingLinks_; //The links to which weapons are attached
    Weapons weapons_; //The mounted weapons
    TurnerTrackers turnerTrackers_; //Objects doing tilt control for each weapon

	MachPhys::WeaponType	lastSoundWeapon_;		// Most recently weapon which played a sound
	PhysAbsoluteTime		lastSoundPlayTime_;		// Time at which this weapon was fired
	MachPhys::WeaponType	lastLightWeapon_;		// Most recently weapon which launched a light
	PhysAbsoluteTime		lastLightLaunchTime_;	// The time at which this weapon was fired
};

PER_DECLARE_PERSISTENT( MachPhysCanAttackImpl );


#endif

/* End ATTACKI.HPP **************************************************/

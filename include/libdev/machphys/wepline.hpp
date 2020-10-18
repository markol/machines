/*
 * W E P L I N E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLinearWeapon

    Represents a weapon that fires linear projectiles
*/

#ifndef _MACHPHYS_WEPLINE_HPP
#define _MACHPHYS_WEPLINE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/weapon.hpp"

class MachPhysLinearWeapon : public MachPhysWeapon
// Canonical form revoked
{
public:
    //dtor.
    virtual ~MachPhysLinearWeapon( void );

    //Construct and return a new projectile of the appropriate ttype for this weapon.
    //It is the index'th projectile in a burst starting at burstStartTime.
    //The projectile's owner is to be pParent.
    //The projectile is to be aimed at a point targetOffset from target.
    virtual MachPhysLinearProjectile* createProjectile
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    ) = 0;

    //Performs standard computations for producing the data for launching a projectile.
    //Most arguments as above.
    //Returns the actual launch time for the projectile.
    //The local transform for the projectile at the start of its flight is returned in
    //pStartTransform.
    PhysAbsoluteTime launchData
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset,
        MexTransform3d* pStartTransform,
		MATHEX_SCALAR* distance
    ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLinearWeapon& t );

    PER_MEMBER_PERSISTENT_ABSTRACT( MachPhysLinearWeapon );

protected:
    //ctor.
    MachPhysLinearWeapon( W4dEntity* pParent, const MexTransform3d& localTransform,
                    const SysPathName& compositeFileName,
                    MachPhys::WeaponType type, MachPhys::Mounting mounting );

    //Similar copy ctor.
    MachPhysLinearWeapon( const MachPhysLinearWeapon& copyMe, MachPhys::Mounting mounting,
                    W4dEntity* pParent, const W4dTransform3d& localTransform );

private:
    MachPhysLinearWeapon( const MachPhysLinearWeapon& );
    MachPhysLinearWeapon& operator =( const MachPhysLinearWeapon& );
    bool operator ==( const MachPhysLinearWeapon& );

};

PER_DECLARE_PERSISTENT( MachPhysLinearWeapon );
PER_READ_WRITE( MachPhysLinearWeapon );

#endif

/* End WEPLINE.HPP **************************************************/

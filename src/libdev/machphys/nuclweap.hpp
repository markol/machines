/*
 * N U C L W E A P . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysNuclearWeapon

    an emsenble of the elements of the nuclear weapon.
*/

#ifndef _MACHPHYS_NUCLWEAP_HPP
#define _MACHPHYS_NUCLWEAP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/wepline.hpp"

class MachPhysConstruction;
class MachPhysLinearProjectile;
class MachPhysMissile;

class MachPhysNuclearWeapon : public MachPhysLinearWeapon
// Canonical form revoked
{
public:
    //Constructs appropriate
    MachPhysNuclearWeapon( W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::Mounting mounting );

    //dtor
    virtual ~MachPhysNuclearWeapon();

    //Return an exemplar  - ensures the mesh is loaded
    static const MachPhysNuclearWeapon& exemplar();

	static PhysRelativeTime destroy(W4dComposite* pComposit, const PhysAbsoluteTime& startTime);

    //////////////////////////////////////////////////////////
    // Inherited from MachPhysLinearWeapon
    //Construct and return a new projectile of the appropriate ttype for this weapon.
    //It is the index'th projectile in a burst starting at burstStartTime.
    //The projectile's owner is to be pParent.
    //The projectile is to be aimed at a point targetOffset from target.
    virtual MachPhysLinearProjectile* createProjectile
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

    //////////////////////////////////////////////////////////

    //Construct and return a missile projectile.
    //Arguments as for createProjectile.
    MachPhysMissile* createMissile
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

    //////////////////////////////////////////////////////////
    // Inherited from MachPhysWeapon

    //Induce the weapon's firing animation at startTime, returning the duration of the animation.
    //This includes recoil, sound, smoke coming from end of gun, lights etc, but
    //NOT launching of any projectiles, victim animations etc.
    //Default implementation does nothing.
    virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );

    //////////////////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysNuclearWeapon& t );

    PER_MEMBER_PERSISTENT( MachPhysNuclearWeapon );

private:
    MachPhysNuclearWeapon( const MachPhysNuclearWeapon& );
    MachPhysNuclearWeapon& operator =( const MachPhysNuclearWeapon& );
    bool operator ==( const MachPhysNuclearWeapon& );

    //One-time constructor  used to create the exemplar
    MachPhysNuclearWeapon();

    //the composite file path
    static const char* compositeFilePath();

    friend class MachPhysWeaponPersistence;
};

PER_DECLARE_PERSISTENT( MachPhysNuclearWeapon );
PER_READ_WRITE( MachPhysNuclearWeapon );

#endif

/* End NUCLWEAP.HPP ****************************************************/

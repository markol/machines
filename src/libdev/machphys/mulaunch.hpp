/*
 * M U L A U N C H . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMultiLauncher

    Models the various types of multi-launcher weapon.
*/

#ifndef _MACHPHYS_MULAUNCH_HPP
#define _MACHPHYS_MULAUNCH_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "machphys/machphys.hpp"
#include "machphys/wepline.hpp"

//forward refs
class W4dEntity;
class MexPoint3d;
class MexTransform3d;
class MachPhysLinearProjectile;
class MachPhysMissile;

class MachPhysMultiLauncher : public MachPhysLinearWeapon
// Canonical form revoked
{
public:
    //ctor. Attached to pParent at offset localTransform.
    //Actual weapon type is type, which must be one of the multi-launcher types.
    //Appropriate mounting is mounting.
    MachPhysMultiLauncher( W4dEntity* pParent, const MexTransform3d& localTransform,
                           MachPhys::WeaponType type, MachPhys::Mounting mounting );


    //Return an exemplar multi-launcher - ensures the mesh is loaded
    static const MachPhysMultiLauncher& exemplar( MachPhys::WeaponType type );

    //dtor.
    virtual ~MachPhysMultiLauncher();

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

    friend ostream& operator <<( ostream& o, const MachPhysMultiLauncher& t );

    PER_MEMBER_PERSISTENT( MachPhysMultiLauncher );

private:
    //revoked
    MachPhysMultiLauncher( const MachPhysMultiLauncher& );
    MachPhysMultiLauncher& operator =( const MachPhysMultiLauncher& );
    bool operator ==( const MachPhysMultiLauncher& );

    //One-time ctor used to read the mesh (once per multi-launcher type)
    MachPhysMultiLauncher( MachPhys::WeaponType type );

    //The file path to the composite definition file
    static const char* compositeFilePath( MachPhys::WeaponType type );

    friend class MachPhysWeaponPersistence;
};

PER_DECLARE_PERSISTENT( MachPhysMultiLauncher );
PER_READ_WRITE( MachPhysMultiLauncher );

#endif

/* End MULAUNCH.HPP *************************************************/

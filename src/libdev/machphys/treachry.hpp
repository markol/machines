/*
 * T R E A C H R Y . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysTreacheryWeapon

    Models the orb of treachery weapon
*/

#ifndef _MACHPHYS_TREACHRY_HPP
#define _MACHPHYS_TREACHRY_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/machphys.hpp"
#include "machphys/wepline.hpp"

//forward refs
class MexTransform3d;
class MachPhysTreacheryOrb;

class MachPhysTreacheryWeapon : public MachPhysLinearWeapon
// Canonical form revoked
{
public:
    //Constructs a new weapon attached to pParent at offset localTransform, using mounting.
    MachPhysTreacheryWeapon( W4dEntity* pParent, const MexTransform3d& localTransform,
                             MachPhys::Mounting mounting );

    //dtor
    virtual ~MachPhysTreacheryWeapon();

    //Return an exemplar weapon - ensures the mesh is loaded
    static const MachPhysTreacheryWeapon& exemplar();

    //Apply the traitor animation to pMachine starting at startTime.
    //It is defecting from oldRace to newRace.
    //Returns duration of the animation.
    static PhysRelativeTime traitorAnimation
    (
        MachPhysMachine* pMachine, const PhysAbsoluteTime& startTime,
        MachPhys::Race oldRace, MachPhys::Race newRace
    );

    /////////////////////////////////////////////////////
    // Inherited from MachPhysWeapon

    //Induce the weapon's firing animation at startTime, returning the duration of the animation.
    //This includes recoil, sound, smoke coming from end of gun, lights etc, but
    //NOT launching of any projectiles, victim animations etc.
    virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );

    /////////////////////////////////////////////////////
    // Inherited from MachPhysLinearWeapon

    //Construct and return a new projectile of the appropriate type for this weapon.
    //It is the index'th projectile in a burst starting at burstStartTime.
    //The projectile's owner is to be pParent.
    //The projectile is to be aimed at a point targetOffset from target.
    virtual MachPhysLinearProjectile* createProjectile
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

    //////////////////////////////////////////////////////

    //Construct and return a treachery orb projectile.
    //It is the index'th projectile in a burst starting at burstStartTime.
    //The projectile's owner is to be pParent.
    //The projectile is to be aimed at a point targetOffset from target.
    MachPhysTreacheryOrb* createTreacheryOrb
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysTreacheryWeapon& t );

    PER_MEMBER_PERSISTENT( MachPhysTreacheryWeapon );

private:
    //revoked
    MachPhysTreacheryWeapon( const MachPhysTreacheryWeapon& );
    MachPhysTreacheryWeapon& operator =( const MachPhysTreacheryWeapon& );
    bool operator ==( const MachPhysTreacheryWeapon& );

    //One-time constructor used to create the exemplar
    MachPhysTreacheryWeapon();

    friend class MachPhysWeaponPersistence;
};

PER_DECLARE_PERSISTENT( MachPhysTreacheryWeapon );
PER_READ_WRITE( MachPhysTreacheryWeapon );

#endif

/* End TREACHRY.HPP *************************************************/

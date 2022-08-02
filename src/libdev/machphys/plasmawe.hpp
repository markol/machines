/*
 * P L A S M A W E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPlasmaWeapon

    Models a plasma rifle or cannon weapon
*/

#ifndef _MACHPHYS_PLASMAWE_HPP
#define _MACHPHYS_PLASMAWE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/machphys.hpp"
#include "machphys/wepline.hpp"

//forward refs
class W4dEntity;
class MexTransform3d;
class MachPhysPlasmaBolt;
class MachPhysArtefact;

class MachPhysPlasmaWeapon : public MachPhysLinearWeapon
// Canonical form revoked
{
public:
    //ctor. Attached to pParent at offset localTransform.
    //Actual weapon type is type, which must be one of the plasma types.
    //Appropriate mounting is mounting.
    MachPhysPlasmaWeapon( W4dEntity* pParent, const MexTransform3d& localTransform,
                          MachPhys::WeaponType type, MachPhys::Mounting mounting );

    //Return an exemplar plasma weapon - ensures the mesh is loaded
    static const MachPhysPlasmaWeapon& exemplar( MachPhys::WeaponType type );

    //dtor.
    virtual ~MachPhysPlasmaWeapon();

    //////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////

    //Construct and return a plasma bolt.
    //Arguments as for createProjectile.
    MachPhysPlasmaBolt* createPlasmaBolt
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

    //////////////////////////////////////////////////////////
    // Inherited from MachPhysWeapon

    //Induce the weapon's firing animation at startTime, returning the duration of the animation.
    //This includes recoil, sound, smoke coming from end of gun, lights etc, but
    //NOT launching of any projectiles, victim animations etc.
    virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );

    //Play the victim animation for a machine or construction at startTime.
    //fromDirection indicates the flight path of the hitting projectile.
    //Returns the duration of the animation.
    virtual PhysRelativeTime victimAnimation( const PhysAbsoluteTime& startTime,
                                              const MexLine3d& fromDirection,
                                              MachPhysMachine* pMachine ) const;
    virtual PhysRelativeTime victimAnimation( const PhysAbsoluteTime& startTime,
                                              const MexLine3d& fromDirection,
                                              MachPhysConstruction* pConstruction ) const;

    //////////////////////////////////////////////////////////

    //Do the work of applying a victim animation to machine/construction/artefact.
    //Return animation duration.
    static PhysRelativeTime applyVictimAnimation( const PhysAbsoluteTime& startTime,
                                                  const MexLine3d& fromDirection,
                                                  MachPhysMachine* pMachine );
    static PhysRelativeTime applyVictimAnimation( const PhysAbsoluteTime& startTime,
                                                  const MexLine3d& fromDirection,
                                                  MachPhysConstruction* pConstruction );
    static PhysRelativeTime applyVictimAnimation( const PhysAbsoluteTime& startTime,
                                                  const MexLine3d& fromDirection,
                                                  MachPhysArtefact* pArtefact );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPlasmaWeapon& t );

    PER_MEMBER_PERSISTENT( MachPhysPlasmaWeapon );

private:
    //revoked
    MachPhysPlasmaWeapon( const MachPhysPlasmaWeapon& );
    MachPhysPlasmaWeapon& operator =( const MachPhysPlasmaWeapon& );
    bool operator ==( const MachPhysPlasmaWeapon& );

    //One-time ctor used to read the mesh (once per plasma weapon type)
    MachPhysPlasmaWeapon( MachPhys::WeaponType type );

    //Play the firing sounds at startTime
    void playFiringSounds( const PhysAbsoluteTime& startTime );

    //The file path to the composite definition file
    static const char* compositeFilePath( MachPhys::WeaponType type );

    friend class MachPhysWeaponPersistence;
};

PER_DECLARE_PERSISTENT( MachPhysPlasmaWeapon );
PER_READ_WRITE( MachPhysPlasmaWeapon );

#endif

/* End PLASMAWE.HPP *************************************************/

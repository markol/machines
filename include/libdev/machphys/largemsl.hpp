/*
 * L A R G E M S L . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLargeMissile

    Models the various types of multi-launcher weapon.
*/

#ifndef _MACHPHYS_LARGEMSL_HPP
#define _MACHPHYS_LARGEMSL_HPP

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

class MachPhysLargeMissile : public MachPhysLinearWeapon
// Canonical form revoked
{
public:
    //ctor. Attached to pParent at offset localTransform.
    //Actual weapon type is type, which must be one of the multi-launcher types.
    //Appropriate mounting is mounting.
    MachPhysLargeMissile( W4dEntity* pParent, const MexTransform3d& localTransform,	MachPhys::Mounting mounting );


    //Return an exemplar multi-launcher - ensures the mesh is loaded
    static const MachPhysLargeMissile& exemplar();

    //dtor.
    virtual ~MachPhysLargeMissile();

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

    friend ostream& operator <<( ostream& o, const MachPhysLargeMissile& t );

    PER_MEMBER_PERSISTENT( MachPhysLargeMissile );

private:
    //revoked
    MachPhysLargeMissile( const MachPhysLargeMissile& );
    MachPhysLargeMissile& operator =( const MachPhysLargeMissile& );
    bool operator ==( const MachPhysLargeMissile& );

    //One-time ctor used to read the mesh (once per multi-launcher type)
    MachPhysLargeMissile();

    //The file path to the composite definition file
    static const char* compositeFilePath();

    friend class MachPhysWeaponPersistence;

	MachPhysMissile* pMissile_;
};

PER_DECLARE_PERSISTENT( MachPhysLargeMissile );
PER_READ_WRITE( MachPhysLargeMissile );

#endif

/* End LARGEMSL.HPP *************************************************/

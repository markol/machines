/*
 * P U L S E W E P . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPulseWepon
    MachPhysPulseRifle
    MachPhysPulseCannon

    Models the pulse weapons
*/

#ifndef _MACHPHYS_PULSEWEP_HPP
#define _MACHPHYS_PULSEWEP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "machphys/machphys.hpp"
#include "machphys/wepline.hpp"

//forward refs
class MexTransform3d;
class SysPathName;
class MachPhysPulseBlob;
class MachPhysArtefact;

////////////////////////////////////////////////////////////////////////////
//Generic pulse weapon behaviour
class MachPhysPulseWeapon : public MachPhysLinearWeapon
// Canonical form revoked
{
public:

    //dtor.
    virtual ~MachPhysPulseWeapon();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPulseWeapon& t );

    /////////////////////////////////////////////
    //Inherited from MachPhysWeapon

    //Induce the weapon's firing animation at startTime, returning the duration of the animation.
    //This includes recoil, sound, smoke coming from end of gun, lights etc, but
    //NOT launching of any projectiles, victim animations etc.
    virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );

    //Play the victim animation for a machine or construction.
    //fromDirection indicates the flight path of the hitting projectile.
    //Returns the duration of the animation.
    virtual PhysRelativeTime victimAnimation( const PhysAbsoluteTime& startTime,
                                              const MexLine3d& fromDirection,
                                              MachPhysMachine* pMachine ) const;
    virtual PhysRelativeTime victimAnimation( const PhysAbsoluteTime& startTime,
                                              const MexLine3d& fromDirection,
                                              MachPhysConstruction* pConstruction ) const;
    /////////////////////////////////////////////

    /////////////////////////////////////////////
    //Inherited from MachPhysLinearWeapon

    //Construct and return a new projectile of the appropriate ttype for this weapon.
    //It is the index'th projectile in a burst starting at burstStartTime.
    //The projectile's owner is to be pParent.
    //The projectile is to be aimed at a point targetOffset from target.
    virtual MachPhysLinearProjectile* createProjectile
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

    /////////////////////////////////////////////

    //Construct and return a pulse blob projectile.
    //Arguments as for createProjectile.
    MachPhysPulseBlob* createPulseBlob
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

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

    //Initiate a single pulse disc attached to pParent with offset localPosition
    //at startTime. Returns durstion of disc.
    static PhysRelativeTime createPulseDisc( W4dEntity* pParent, const MexTransform3d& localPosition,
                          const PhysAbsoluteTime& startTime );

    PER_MEMBER_PERSISTENT( MachPhysPulseWeapon );

protected:
    //ctor. Creates a hidden version of the weapon which has type type.
    //the composite fil defining the model is compositeFileName.
    //For use with 1-time ctors.
    MachPhysPulseWeapon( const SysPathName& compositeFileName, MachPhys::WeaponType type );

    //copy ctor
    MachPhysPulseWeapon( const MachPhysPulseWeapon& copyMe, MachPhys::Mounting mounting,
                         W4dEntity* pParent, const MexTransform3d& localTransform );

    //Initiate the animation of the disc(s) at the gun barrel
    void createFiringDiscs( const PhysAbsoluteTime& startTime );

private:
    MachPhysPulseWeapon( const MachPhysPulseWeapon& );
    MachPhysPulseWeapon& operator =( const MachPhysPulseWeapon& );
    bool operator ==( const MachPhysPulseWeapon& );
};

PER_DECLARE_PERSISTENT( MachPhysPulseWeapon );
PER_READ_WRITE( MachPhysPulseWeapon );

////////////////////////////////////////////////////////////////////////////

class MachPhysPulseRifle : public MachPhysPulseWeapon
// Canonical form revoked
{
public:
    //ctor. Will owned by pParent at offset localTransform.
    //Mounting version defined by mounting.
    MachPhysPulseRifle( W4dEntity* pParent, const MexTransform3d& localTransform,
                        MachPhys::Mounting mounting );

    //dtor.
    virtual ~MachPhysPulseRifle();

    //Return exemplar for given mounting
    static const MachPhysPulseRifle& exemplar();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPulseRifle& t );

    PER_MEMBER_PERSISTENT( MachPhysPulseRifle );

private:
    MachPhysPulseRifle( const MachPhysPulseRifle& );
    MachPhysPulseRifle& operator =( const MachPhysPulseRifle& );
    bool operator ==( const MachPhysPulseRifle& );

    //ctor used by exemplar method.
    MachPhysPulseRifle();

    friend class MachPhysWeaponPersistence;
};

PER_DECLARE_PERSISTENT( MachPhysPulseRifle );
PER_READ_WRITE( MachPhysPulseRifle );

////////////////////////////////////////////////////////////////////////////

class MachPhysPulseCannon : public MachPhysPulseWeapon
// Canonical form revoked
{
public:
    //ctor. Will owned by pParent at offset localTransform.
    //Mounting version defined by mounting.
    MachPhysPulseCannon( W4dEntity* pParent, const MexTransform3d& localTransform,
                        MachPhys::Mounting mounting );

    //dtor.
    virtual ~MachPhysPulseCannon();

    //Return exemplar for given mounting
    static const MachPhysPulseCannon& exemplar();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPulseCannon& t );

    PER_MEMBER_PERSISTENT( MachPhysPulseCannon );

private:
    MachPhysPulseCannon( const MachPhysPulseCannon& );
    MachPhysPulseCannon& operator =( const MachPhysPulseCannon& );
    bool operator ==( const MachPhysPulseCannon& );

    //ctor used by exemplar method.
    MachPhysPulseCannon();

    friend class MachPhysWeaponPersistence;
};
////////////////////////////////////////////////////////////////////////////

PER_DECLARE_PERSISTENT( MachPhysPulseCannon );
PER_READ_WRITE( MachPhysPulseCannon );

#ifdef _INLINE
    #include "machphys/pulsewep.ipp"
#endif


#endif

/* End PULSEWEP.HPP *************************************************/

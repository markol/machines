/*
 * B O L T E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysBolter

    Models the various types of bolter weapon.
*/

#ifndef _MACHPHYS_BOLTER_HPP
#define _MACHPHYS_BOLTER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/weapon.hpp"

#include "mathex/point3d.hpp"

class W4dGeneric;
class W4dLink;
class MachPhysArtefact;

class MachPhysBolter : public MachPhysWeapon
// Canonical form revoked
{
public:
    //Constructs appropriate 
    MachPhysBolter( W4dEntity* pParent, const MexTransform3d& localTransform,
                    MachPhys::WeaponType type, MachPhys::Mounting mounting );

    //dtor
    virtual ~MachPhysBolter();

    //Return an exemplar bolter - ensures the mesh is loaded
    static const MachPhysBolter& exemplar( MachPhys::WeaponType type );

    //Return a flash mesh entity for use with firing animation
    static const W4dGeneric& exemplarFlash( MachPhys::WeaponType type );

    //////////////////////////////////////////////
    // Inherited from MachPhysWeapon
    //Induce the weapon's firing animation at startTime, returning the duration of the animation.
    //This includes recoil, sound, smoke coming from end of gun, lights etc, but
    //NOT launching of any projectiles, victim animations etc.
    //Default implementation does nothing.
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

    //////////////////////////////////////////////

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

    //Display some bolter impact sparks attached to pEntity at startTime.
    //They start at startPoint and fly off roughly in direction of direction
    //(relative to pEntity).
    static void impactSparks
    (
        const PhysAbsoluteTime& startTime, W4dEntity* pEntity,
        const MexPoint3d& startPoint, const MexVec3& direction
    );
    
    //the material to use for sparks
    static const RenMaterial& sparkMaterial();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysBolter& t );

    PER_MEMBER_PERSISTENT( MachPhysBolter );
    PER_FRIEND_READ_WRITE( MachPhysBolter );
    
private:
    MachPhysBolter( const MachPhysBolter& );
    MachPhysBolter& operator =( const MachPhysBolter& );
    bool operator ==( const MachPhysBolter& );

    //One-time constructor (per type) used to create the exemplars
    MachPhysBolter( MachPhys::WeaponType type );

    //the composite file path for given type
    static const char* compositeFilePath( MachPhys::WeaponType type );

    //Play the firing sounds at startTime
    void playFiringSounds( const PhysAbsoluteTime& startTime );

    //Return a new flash object of type type
    static W4dGeneric* newFlash( MachPhys::WeaponType type );

    //data members
    W4dLink* pTurnLink_; //The link which rotates on the auto-cannon only

    friend class MachPhysWeaponPersistence;
};

PER_DECLARE_PERSISTENT( MachPhysBolter );

#endif

/* End BOLTER.HPP ****************************************************/

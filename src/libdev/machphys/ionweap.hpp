/*
 * I O N W E A P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysIonWeapon

    Models the various types of ion weapon.
*/

#ifndef _MACHPHYS_IONWEAP_HPP
#define _MACHPHYS_IONWEAP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/weapon.hpp"

class W4dGeneric;
class W4dComposite;

class MachPhysIonWeapon : public MachPhysWeapon
// Canonical form revoked
{
public:
    //Constructs appropriate 
    MachPhysIonWeapon( W4dEntity* pParent, const MexTransform3d& localTransform,
                    MachPhys::WeaponType type, MachPhys::Mounting mounting );

    //dtor
    virtual ~MachPhysIonWeapon();

    //Return an exemplar ionWeapon - ensures the mesh is loaded
    static const MachPhysIonWeapon& exemplar( MachPhys::WeaponType type );

    //apply the destruction animation to pVictim at startTime
	static PhysRelativeTime destroy(W4dEntity* pVictim, const PhysAbsoluteTime& startTime);

    //////////////////////////////////////////////
    // Inherited from MachPhysWeapon
    // does nothing.
    virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );

    //////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysIonWeapon& t );

    PER_MEMBER_PERSISTENT( MachPhysIonWeapon );

private:
    MachPhysIonWeapon( const MachPhysIonWeapon& );
    MachPhysIonWeapon& operator =( const MachPhysIonWeapon& );
    bool operator ==( const MachPhysIonWeapon& );

    //One-time constructor (per type) used to create the exemplars
    MachPhysIonWeapon( MachPhys::WeaponType type );

    //the composite file path for given type
    static const char* compositeFilePath( MachPhys::WeaponType type );

    friend class MachPhysWeaponPersistence;
};

PER_DECLARE_PERSISTENT( MachPhysIonWeapon );
PER_READ_WRITE( MachPhysIonWeapon );

#endif

/* End ionWeap.HPP ****************************************************/

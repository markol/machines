/*
 * B O L T E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysVortexWeapon

    Models the various types of Vortex weapon.
*/

#ifndef _MACHPHYS_VORTWEAP_HPP
#define _MACHPHYS_VORTWEAP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/weapon.hpp"

class W4dGeneric;
class W4dComposite;

class MachPhysVortexWeapon : public MachPhysWeapon
// Canonical form revoked
{
public:
    //Constructs appropriate 
    MachPhysVortexWeapon( W4dEntity* pParent, const MexTransform3d& localTransform,
                    MachPhys::Mounting mounting );

    //dtor
    virtual ~MachPhysVortexWeapon();

    //Return an exemplar VortexWeapon - ensures the mesh is loaded
    static const MachPhysVortexWeapon& exemplar();

    //Apply the vortex destruction animation to pVictim at startTime
	static PhysRelativeTime destroy(W4dEntity* pVictim, const PhysAbsoluteTime& startTime);

    //////////////////////////////////////////////
    // Inherited from MachPhysWeapon
    //does nothing.
    virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );

    //////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysVortexWeapon& t );

    PER_MEMBER_PERSISTENT( MachPhysVortexWeapon );

private:
    MachPhysVortexWeapon( const MachPhysVortexWeapon& );
    MachPhysVortexWeapon& operator =( const MachPhysVortexWeapon& );
    bool operator ==( const MachPhysVortexWeapon& );

    //One-time constructor (per type) used to create the exemplars
    MachPhysVortexWeapon();

    //the composite file path for given type
    static const char* compositeFilePath();

    friend class MachPhysWeaponPersistence;
};

PER_DECLARE_PERSISTENT( MachPhysVortexWeapon );
PER_READ_WRITE( MachPhysVortexWeapon );

#endif

/* End VortexWeap.HPP ****************************************************/

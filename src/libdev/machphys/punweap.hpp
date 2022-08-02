/*
 * P U N W E A P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPunchWeapon

    garrila punch weapon
*/

#ifndef _MACHPHYS_PUNWEAP_HPP
#define _MACHPHYS_PUNWEAP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/weapon.hpp"

class MachPhysPunchBlast;
class MachPhysPlanetSurface;

class MachPhysPunchWeapon : public MachPhysWeapon
// Canonical form revoked
{
public:
    //Constructs appropriate 
    MachPhysPunchWeapon( W4dEntity* pParent, const MexTransform3d& localTransform);

    //dtor
    virtual ~MachPhysPunchWeapon();

    //Return an exemplar  - ensures the mesh is loaded
    static const MachPhysPunchWeapon& exemplar();

    
     //Construct and return a MachPhysPunchBlast.
    MachPhysPunchBlast* createPunchBlast( const PhysAbsoluteTime& startTime, const MachPhysPlanetSurface& surface );
    
    //////////////////////////////////////////////////////////
    // Inherited from MachPhysWeapon

    //Induce the weapon's firing animation at startTime, returning the duration of the animation.
    //This includes recoil, sound, smoke coming from end of gun, lights etc, but
    //NOT launching of any projectiles, victim animations etc.
    //Default implementation does nothing.
    virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );
    
    //////////////////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPunchWeapon& t );

    PER_MEMBER_PERSISTENT( MachPhysPunchWeapon );

private:
    MachPhysPunchWeapon( const MachPhysPunchWeapon& );
    MachPhysPunchWeapon& operator =( const MachPhysPunchWeapon& );
    bool operator ==( const MachPhysPunchWeapon& );

    //One-time constructor  used to create the exemplar
    MachPhysPunchWeapon();

    //the composite file path 
    static const char* compositeFilePath();

    friend class MachPhysWeaponPersistence;
};

PER_DECLARE_PERSISTENT( MachPhysPunchWeapon );
PER_READ_WRITE( MachPhysPunchWeapon );

#endif

/* End PUNWEAP.HPP ****************************************************/
